#pragma once
#include "maya/pch.h"

#include <boost/format.hpp>
#include <boost/functional/hash.hpp>

#include <pxr/pxr.h>
#include <pxr/usd/sdf/types.h>


NAMESPACE_BASE_BEGIN
inline pxr::SdfValueTypeName GetUsdValueType(const MPlug& attrPlug)
{
	MObject attrObj(attrPlug.attribute());
	if (attrObj.isNull())
	{
		STDOUT_ERROR("Failed to get valid MPlug");
		return pxr::SdfValueTypeName();
	}
	MFnData::Type            typedDataType   = MFnData::kInvalid;
	MFnUnitAttribute::Type   unitDataType    = MFnUnitAttribute::kInvalid;
	MFnNumericData::Type     numericDataType = MFnNumericData::kInvalid;

	if(attrObj.hasFn(MFn::kNumericAttribute))
	{
		MFnNumericAttribute numericAttrFn(attrObj);
		numericDataType = numericAttrFn.unitType();
	}
	else if(attrObj.hasFn(MFn::kTypedAttribute))
	{
		MFnTypedAttribute typedAttrFn(attrObj);
		typedDataType = typedAttrFn.attrType();

		if(typedDataType == MFnData::kNumeric)
		{
			// Inspect the type of the data itself to find the actual type.
			MObject plugObj = attrPlug.asMObject();
			if(plugObj.hasFn(MFn::kNumericData))
			{
				MFnNumericData numericDataFn(plugObj);
				numericDataType = numericDataFn.numericType();
			}
		}
	}
	else if(attrObj.hasFn(MFn::kUnitAttribute))
	{
		MFnUnitAttribute unitAttrFn(attrObj);
		unitDataType = unitAttrFn.unitType();
	}

	pxr::SdfValueTypeName valueType;
	if(attrObj.hasFn(MFn::kEnumAttribute))
		return  pxr::SdfValueTypeNames->Int;

	if(attrObj.hasFn(MFn::kMatrixAttribute))
		return pxr::SdfValueTypeNames->Matrix4d;

	switch (typedDataType)
	{
		case MFnData::kString:          return pxr::SdfValueTypeNames->String;
		case MFnData::kMatrix:          return pxr::SdfValueTypeNames->Matrix4d;
		case MFnData::kStringArray:     return pxr::SdfValueTypeNames->StringArray;
		case MFnData::kDoubleArray:     return pxr::SdfValueTypeNames->DoubleArray;
		case MFnData::kFloatArray:      return pxr::SdfValueTypeNames->FloatArray;
		case MFnData::kIntArray:        return pxr::SdfValueTypeNames->IntArray;
		case MFnData::kPointArray:      return pxr::SdfValueTypeNames->Point3dArray;
		case MFnData::kVectorArray:     return pxr::SdfValueTypeNames->Vector3dArray;
	}
	switch (numericDataType)
	{
		case MFnNumericData::kBoolean:  return pxr::SdfValueTypeNames->Bool;

		case MFnNumericData::kByte:
		case MFnNumericData::kChar:
		case MFnNumericData::kShort:
		case MFnNumericData::kInt:      return pxr::SdfValueTypeNames->Int;

		case MFnNumericData::k2Short:
		case MFnNumericData::k2Int:     return pxr::SdfValueTypeNames->Int2;

		case MFnNumericData::k3Short:
		case MFnNumericData::k3Int:     return pxr::SdfValueTypeNames->Int3;

		case MFnNumericData::kFloat:    return pxr::SdfValueTypeNames->Float;
		case MFnNumericData::k2Float:   return pxr::SdfValueTypeNames->Float2;
		case MFnNumericData::k3Float:
			if (MFnAttribute(attrObj).isUsedAsColor())
				return pxr::SdfValueTypeNames->Color3f;
			else
				return pxr::SdfValueTypeNames->Float3;

		case MFnNumericData::kDouble:   return pxr::SdfValueTypeNames->Double;
		case MFnNumericData::k2Double:  return pxr::SdfValueTypeNames->Double2;
		case MFnNumericData::k3Double:  return pxr::SdfValueTypeNames->Double3;
			if (MFnAttribute(attrObj).isUsedAsColor())
				return pxr::SdfValueTypeNames->Color3d;
			else
				return pxr::SdfValueTypeNames->Double3;
		case MFnNumericData::k4Double:  return pxr::SdfValueTypeNames->Double4;
	}

	switch (unitDataType)
	{
		case MFnUnitAttribute::kAngle:
		case MFnUnitAttribute::kDistance:   return pxr::SdfValueTypeNames->Double;
	}

	STDOUT_ERROR("Failed to get valid SdfValueTypeName");
	return pxr::SdfValueTypeName();
}

//inline MStatus CreateCustomDataByMPlug(const MPlug& plug, pxr::UsdPrim* pOutPrim)
//{
//    DECL_LOGGER(true);
//
//    if(!pOutPrim || !pOutPrim->IsValid())
//    {
//        STDOUT_ERROR("Failed to get valid prim");
//        return MS::kFailure;
//    }
//
//    MStatus stat;
//    MObject attrObj(plug.attribute());
//    if (attrObj.isNull())
//    {
//        STDOUT_ERROR("Failed to get valid MPlug");
//        return MS::kFailure;
//    }
//    std::string name = plug.partialName().asChar();
//    MCHECKERROR(stat, "Failed to get plug name");
//
//    LOGGER_DEBUG("name = %s", name.c_str());
//	LOGGER_DEBUG("name = %s", plug.name().asChar());
//	LOGGER_DEBUG("name = %s", plug.partialName().asChar());
//
//
//    pxr::SdfValueTypeName valueType = GetUsdValueType(plug);
//    if(!valueType)
//    {
//        STDOUT_ERROR("Failed to get valid valueType");
//        return MS::kFailure;
//    }
//
//    if(valueType == pxr::SdfValueTypeNames->String)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(plug.asString().asChar()));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Matrix4d)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//
//    }
//    else if(valueType == pxr::SdfValueTypeNames->StringArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->DoubleArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->FloatArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->IntArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Point3dArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Vector3dArray)
//    {
//        //TODO::
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Bool)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(plug.asBool()));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Int)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(plug.asInt()));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Int2)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec2i(plug.child(0).asInt(),
//                                                                         plug.child(1).asInt())
//        ));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Int3)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec3i(plug.child(0).asInt(),
//                                                                         plug.child(1).asInt(),
//                                                                         plug.child(2).asInt()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Float)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(plug.asFloat()));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Float2)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec2f(plug.child(0).asFloat(),
//                                                                         plug.child(1).asFloat()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Float3)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec3f(plug.child(0).asFloat(),
//                                                                         plug.child(1).asFloat(),
//                                                                         plug.child(2).asFloat()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Color3f)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec3f(plug.child(0).asFloat(),
//                                                                         plug.child(1).asFloat(),
//                                                                         plug.child(2).asFloat()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Double)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(plug.asDouble()));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Double2)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec2d(plug.child(0).asDouble(),
//                                                                         plug.child(1).asDouble()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Double3)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec3d(plug.child(0).asDouble(),
//                                                                         plug.child(1).asDouble(),
//                                                                         plug.child(2).asDouble()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Color3d)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec3d(plug.child(0).asDouble(),
//                                                                         plug.child(1).asDouble(),
//                                                                         plug.child(2).asDouble()
//        )));
//    }
//    else if(valueType == pxr::SdfValueTypeNames->Double4)
//    {
//        pOutPrim->SetCustomDataByKey(pxr::TfToken(name), VtValue(GfVec4d(plug.child(0).asDouble(),
//                                                                         plug.child(1).asDouble(),
//                                                                         plug.child(2).asDouble(),
//                                                                         plug.child(3).asDouble()
//        )));
//    }
//    else
//    {
//        STDOUT_ERROR("Failed to get supporting valueType");
//        return MS::kFailure;
//    }
//    return stat;
//}


//inline MStatus CreateUniformUsdAttribute(const MPlug& plug,
//                                         const pxr::UsdPrim& prim,
//                                         const bool custom = true
//)
//{
//	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
//
//	DECL_LOGGER(false);
//	MStatus stat;
//	MObject attrObj(plug.attribute());
//	if (attrObj.isNull())
//	{
//		STDOUT_ERROR("Failed to get valid MPlug");
//		return MS::kFailure;
//	}
//	std::string name = plug.name(&stat).asChar();
//	MCHECKERROR(stat, "Failed to get plug name");
//
//	pxr::SdfValueTypeName valueType = GetUsdValueType(plug);
//	if(!valueType)
//	{
//		STDOUT_ERROR("Failed to get valid valueType");
//		return MS::kFailure;
//	}
//
//	boost::char_separator<char> sep{"."};
//	Tokenizer                   tokenizer(name, sep);
//	std::vector<std::string>    tokens;
//	for (Tokenizer::iterator    iter = tokenizer.begin(); iter != tokenizer.end(); ++iter)
//		tokens.push_back(*iter);
//
//	const std::string attribName = tokens[tokens.size() - 1];
//	pxr::UsdAttribute usdAttrib = prim.CreateAttribute(pxr::TfToken(attribName),
//		                                               valueType,
//		                                               custom,
//		                                               pxr::SdfVariability::SdfVariabilityUniform);
//	if(!usdAttrib)
//	{
//		STDOUT_ERROR("Failed to get usdAttrib, %s", name.c_str());
//		return MS::kFailure;
//	}
//
//
//	if(valueType == pxr::SdfValueTypeNames->String)
//	{
//		usdAttrib.Set(std::string(plug.asString().asChar()));
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Matrix4d)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->StringArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->DoubleArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->FloatArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->IntArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Point3dArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Vector3dArray)
//	{
//		//TODO::
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Bool)
//	{
//		usdAttrib.Set(plug.asBool());
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Int)
//	{
//		usdAttrib.Set(plug.asInt());
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Int2)
//	{
//		usdAttrib.Set(pxr::GfVec2i(plug.child(0).asInt(),
//		                           plug.child(1).asInt())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Int3)
//	{
//		usdAttrib.Set(pxr::GfVec3i(plug.child(0).asInt(),
//		                           plug.child(1).asInt(),
//		                           plug.child(2).asInt())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Float)
//	{
//		usdAttrib.Set(plug.asFloat());
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Float2)
//	{
//		usdAttrib.Set(pxr::GfVec2f(plug.child(0).asFloat(),
//		                           plug.child(1).asFloat())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Float3)
//	{
//		usdAttrib.Set(pxr::GfVec3f(plug.child(0).asFloat(),
//		                           plug.child(1).asFloat(),
//		                           plug.child(2).asFloat())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Color3f)
//	{
//		usdAttrib.Set(pxr::GfVec3f(plug.child(0).asFloat(),
//		                           plug.child(1).asFloat(),
//		                           plug.child(2).asFloat())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Double)
//	{
//		usdAttrib.Set(plug.asDouble());
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Double2)
//	{
//		usdAttrib.Set(pxr::GfVec2d(plug.child(0).asDouble(),
//		                           plug.child(1).asDouble())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Double3)
//	{
//		usdAttrib.Set(pxr::GfVec3d(plug.child(0).asDouble(),
//		                           plug.child(1).asDouble(),
//		                           plug.child(2).asDouble()));
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Color3d)
//	{
//		usdAttrib.Set(pxr::GfVec3d(plug.child(0).asDouble(),
//		                           plug.child(1).asDouble(),
//		                           plug.child(2).asDouble())
//		);
//	}
//	else if(valueType == pxr::SdfValueTypeNames->Double4)
//	{
//		usdAttrib.Set(pxr::GfVec4d(plug.child(0).asDouble(),
//		                           plug.child(1).asDouble(),
//		                           plug.child(2).asDouble(),
//		                           plug.child(3).asDouble())
//		);
//	}
//	else
//	{
//		STDOUT_ERROR("Failed to get supporting valueType");
//	}
//	return stat;
//}


//inline MStatus CreateUsdPrimvar(const MPlug& plug,
//                                const pxr::UsdPrim& prim,
//                                const pxr::TfToken& interpolation = pxr::TfToken(),
//                                const int           elementSize = -1
//);

NAMESPACE_BASE_END
