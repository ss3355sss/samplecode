#pragma once
#include "baseMaya/pch.h"

#include <pxr/base/tf/stringUtils.h>
#include <pxr/usd/usdUtils/pipeline.h>

NAMESPACE_BASE_BEGIN
#define USD_VARIANT_PLUG_NAME_PREFIX "usdVariantSet_"

typedef std::set<std::string> VariantSetNames;
typedef std::vector<std::pair<std::string /* variant name */, std::string /*varaint select */>> VariantSelects;

inline const VariantSetNames GetVariantSetNamesForStageCache(const MFnDependencyNode& depNodeFn)
{
	const auto& regVarSets  = pxr::UsdUtilsGetRegisteredVariantSets();
	if(!regVarSets.empty())
	{
		std::set<std::string> ret;
		for (const auto& regVarSet: regVarSets)
			ret.insert(regVarSet.name);
		return ret;
	}
	
	VariantSetNames varSetNames;
	for (unsigned int i = 0; i < depNodeFn.attributeCount(); i++)
	{
		MObject attrObj = depNodeFn.attribute(i);
		if(attrObj.isNull())
			continue;
		
		MPlug attrPlug = depNodeFn.findPlug(attrObj);
		if(attrPlug.isNull())
			continue;
		
		
		std::string attrName(attrPlug.partialName().asChar());
		if(!pxr::TfStringStartsWith(attrName, USD_VARIANT_PLUG_NAME_PREFIX))
			continue;
		
		std::string variantSet = attrName.substr(std::string(USD_VARIANT_PLUG_NAME_PREFIX).size());
		varSetNames.insert(variantSet);
	}
	return varSetNames;
}

inline const VariantSelects GetVariantSelects(const VariantSetNames& variantSetNames, const MFnDependencyNode& depNodeFn)
{
	VariantSelects variantSelects;
	for(std::set<std::string>::iterator iter = variantSetNames.begin(); iter != variantSetNames.end(); ++iter)
	{
		const std::string& variantName = *iter;
		MString plugName = USD_VARIANT_PLUG_NAME_PREFIX + MString(variantName.c_str());
		MPlug varSetPlg = depNodeFn.findPlug(plugName, true);
		if(!varSetPlg.isNull())
		{
			MString varSetVal = varSetPlg.asString();
			if(varSetVal.length() > 0)
			{
				variantSelects.push_back(VariantSelects::value_type(variantName, varSetPlg.asString().asChar()));
			}
		}
	}
	return variantSelects;
}


NAMESPACE_BASE_END
