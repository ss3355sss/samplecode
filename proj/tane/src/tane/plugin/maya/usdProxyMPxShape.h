#pragma once

#include "maya/pch.h"

#include <usdMaya/usdPrimProvider.h>
#include <usdMaya/stageCache.h>

#include "base/usdReader.h"

#include "lib/baseMaya/util.h"
#include "lib/baseMaya/exclude/usdUtil.h"

#include "maya/proxyMPxShape.h"
#include "maya/proxyMPxShapeDrawOverride.h"


#define USD_VARIANT_PLUG_NAME_PREFIX "usdVariantSet_"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxShape
class UsdProxyMPxShape : public ProxyMPxShape,
                         public UsdMayaUsdPrimProvider
{
public:
	typedef ProxyMPxShape Base;

public:
	static MTypeId Id;
	static MString NodeName;
	static MString DrawOverrideRegistrantId;
	static MString DrawDbClassification;

public:
	static MObject renderFileObj;
	static MObject primPathObj;
	static MObject purposeObj;
	static MObject variantKeyObj;

private:
	UsdReader reader_;

public:
	///------------------------------------------------------------ static functions
	static void* Creator()
	{
		return new UsdProxyMPxShape;
	}
	static MStatus Initialize()
	{
		MStatus stat;
		stat = Base::Initialize();
		MCHECKERROR(stat, "Failed to Base::Initialize");
		
		MFnTypedAttribute    tAttr;
		MFnNumericAttribute  nAttr;
		MFnCompoundAttribute cAttr;
		MFnUnitAttribute     uAttr;
		MFnMatrixAttribute   mAttr;
		MFnEnumAttribute     eAttr;
		
		renderFileObj = tAttr.create(nodeAttr::usd->renderFile, nodeAttr::usd->renderFile, MFnData::kString, MObject::kNullObj, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::usd->renderFile);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(renderFileObj);
		primPathObj = tAttr.create(nodeAttr::usd->primPath, nodeAttr::usd->primPath, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::usd->primPath);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(primPathObj);
		
		purposeObj = eAttr.create(nodeAttr::usd->purpose, nodeAttr::usd->purpose, 0, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::usd->purpose);
		eAttr.addField("proxy", 0);
		eAttr.addField("render", 1);
		//eAttr.addField("default", 2);
		SET_ATTRIBUTE_STATUS(eAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     true,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(purposeObj);
		
		variantKeyObj = tAttr.create(nodeAttr::usd->variantKey, nodeAttr::usd->variantKey, MFnData::kString, MObject::kNullObj, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::usd->renderFile);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     true                       /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(variantKeyObj);
		
		
		ATTRIBUTE_AFFECTS(renderFileObj, outSourceObj);
		ATTRIBUTE_AFFECTS(primPathObj, outSourceObj);
		ATTRIBUTE_AFFECTS(purposeObj, outSourceObj);
		ATTRIBUTE_AFFECTS(variantKeyObj, outSourceObj);
		return stat;
	}

public:
	UsdProxyMPxShape() : Base()
	{
	}
	virtual ~UsdProxyMPxShape()
	{
	}
public:
	virtual void postConstructor()
	{
		Base::postConstructor();
		DEFAULT_DAG_POST_CONSTRUCTOR(RegisteredNodes->UsdProxyMPxShape.name);
	}
	virtual MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		stat = Base::setDependentsDirty(plug, plugArray);
		MCHECKERROR(stat, "Failed to Base::setDependentsDirty");
		
		if(plug == renderFileObj ||
		   plug == primPathObj ||
		   plug == purposeObj ||
		   plug == variantKeyObj
			)
		{
			LOGGER_INFO("plug.info() = %s", plug.info().asChar());
			stateQueue()->push(State::kARCHIVE_ATTRIBUTE_CHANGED);
			signalDirtyToViewport();
		}
		else
		{
			std::string name(plug.partialName().asChar());
			LOGGER_DEBUG("name = %s", name.c_str());
			if(name.find("usdVariantSet_") != std::string::npos)
			{
				LOGGER_INFO("plug.info() = %s", plug.info().asChar());
				MFnDagNode shapeFn(thisMObject());
				MPlug      outSourcePlug = shapeFn.findPlug("outSource");
				plugArray.append(outSourcePlug);
				
				stateQueue()->push(State::kARCHIVE_ATTRIBUTE_CHANGED);
				signalDirtyToViewport();
			}
			else
			{
				LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
			}
		}
		return stat;
	}
	virtual MStatus nodeDeleted(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus nodeAdded(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus archiveAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus displayModeAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus shaderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus renderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus timeChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		MStatus stat;
		stat = updateRenderItem_(dataBlock);
		MCHECKERROR(stat, "Failed to update renderItem");
		return stat;
	}
public:
	virtual UsdPrim usdPrim() const
	{
		UsdPrim usdPrim;
		if(!reader_.usdPrim())
			return usdPrim; // .empty UsdPrim
	
		return reader_.usdPrim();
	}

private:
	//std::set<std::string> getVariantSetNamesForStageCache_(const MFnDependencyNode& depNodeFn)
	//{
	//	DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
	//	const auto& regVarSets  = UsdUtilsGetRegisteredVariantSets();
	//	if(!regVarSets.empty())
	//	{
	//		std::set<std::string> ret;
	//		for (const auto& regVarSet: regVarSets)
	//			ret.insert(regVarSet.name);
	//		return ret;
	//	}
	//
	//	std::set<std::string> varSetNames;
	//	for (unsigned int i = 0; i < depNodeFn.attributeCount(); i++)
	//	{
	//		MObject attrObj = depNodeFn.attribute(i);
	//		if(attrObj.isNull())
	//			continue;
	//
	//		MPlug attrPlug = depNodeFn.findPlug(attrObj);
	//		if(attrPlug.isNull())
	//			continue;
	//
	//
	//		std::string attrName(attrPlug.partialName().asChar());
	//		if(!TfStringStartsWith(attrName, USD_VARIANT_PLUG_NAME_PREFIX))
	//			continue;
	//
	//		std::string variantSet = attrName.substr(std::string(USD_VARIANT_PLUG_NAME_PREFIX).size());
	//		varSetNames.insert(variantSet);
	//	}
	//	return varSetNames;
	//}
	
	
	
	MStatus computeOutSource_(MDataBlock& dataBlock)
	{
		DECL_LOGGER(logging::maya->UsdProxyMPxShape);
		LOGGER_INFO("recalculate geometry data");
		
		
		MStatus stat;
		
		const MTime & inTime      = dataBlock.inputValue(inTimeObj, &stat).asTime();
		const bool  & forceStatic = dataBlock.inputValue(forceStaticObj, &stat).asBool();
		const double& frameOffset = dataBlock.inputValue(frameOffsetObj, &stat).asDouble();
		const short & animCycle   = dataBlock.inputValue(animCycleObj, &stat).asShort();
		
		const MString& renderFile     = dataBlock.inputValue(renderFileObj, &stat).asString();
		const MString& renderPrimPath = dataBlock.inputValue(primPathObj, &stat).asString();
		const short  & purpose        = dataBlock.inputValue(purposeObj, &stat).asShort();
		const MString& variantKey     = dataBlock.inputValue(variantKeyObj, &stat).asString();
		
		UsdStageRefPtr pStage = UsdStage::Open(renderFile.asChar());
		if(!pStage)
		{
			LOGGER_ERROR("Failed open file, %s", renderFile.asChar());
			return MS::kFailure;
		}
		
		//UsdVariantSets            variantSet   = pStage->GetDefaultPrim().GetVariantSets();
		//std::vector<std::string>  variantNames = variantSet.GetNames();

		/* get VariantSet value */
		UsdReader::VariantSelects variantSelects;
		{
			MFnDependencyNode depNodeFn(thisMObject());
			//std::set<std::string> variantSetNames = getVariantSetNamesForStageCache_(depNodeFn);
			std::set<std::string> variantSetNames = GetVariantSetNamesForStageCache(depNodeFn);

			LOGGER_DEBUG("variantSetNames.size() = %d", variantSetNames.size());
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
						variantSelects.push_back(UsdReader::VariantSelects::value_type(variantName, varSetPlg.asString().asChar()));
					}
				}
			}
		}
		
		if(!reader_.open(renderFile.asChar(), renderPrimPath.asChar(), variantSelects))
		{
			std::string msg;
			msg = (boost::format("Failed open file, %s:%s") % renderFile.asChar() % renderPrimPath.asChar()).str();
			MGlobal::displayError(msg.c_str());
			LOGGER_ERROR("%s", msg.c_str());
			return MS::kFailure;
		}

		double targetFrame = inTime.value() + frameOffset;
		LOGGER_DEBUG("targetFrame = %5.2f", targetFrame);
		/* set RenderItem (make renderItem for proxy(Abc Case : BBoxItem or GeomItem)) */
		std::shared_ptr<RenderItem> pRenderItem = nullptr;
		
		BBox3f                        bbox;
		std::shared_ptr<ExpandedGeom> pGeom;
		
		pxr::TfToken purposeToken = UsdGeomTokens->default_;
		switch (purpose)
		{
			case 0:
				purposeToken = UsdGeomTokens->proxy;
				break;
			case 1:
				purposeToken = UsdGeomTokens->render;
				break;
			//case 2:
			//	purposeToken = UsdGeomTokens->default_;
			//	break;
		}
		
		/* set item */
		bbox  = reader_.bbox(purposeToken, animCycle, targetFrame);
		pGeom = reader_.expandedSample(purposeToken, animCycle, targetFrame);
		
		/* set lod0 */
		pGeom->drawCmd[0].indexOffset = 0;
		pGeom->drawCmd[0].indexCount  = pGeom->indexArray.size();
		/* set lod1 */
		pGeom->drawCmd[1].indexOffset = 0;
		pGeom->drawCmd[1].indexCount  = pGeom->indexArray.size();
		/* set lod2 */
		pGeom->drawCmd[2].indexOffset = 0;
		pGeom->drawCmd[2].indexCount  = pGeom->indexArray.size();
		
		/* create renderItem */
		pRenderItem = std::make_shared<ExpandedGeomItem>(bbox, pGeom);
		if(!pRenderItem)
		{
			LOGGER_ERROR("Failed to create valid renderItem");
			return MS::kFailure;
		}
		
		
		///----------------------------------set source properties end
		MDataHandle   outSourceHnd = dataBlock.outputValue(outSourceObj, &stat);
		///create SourceMPxData
		MFnPluginData pluginDataFn;
		pluginDataFn.create(MTypeId(SourceMPxData::id), &stat);
		MCHECKERROR(stat, "Failed to create SourceMPxData");
		SourceMPxData* pSourceMPxData = dynamic_cast<SourceMPxData*>(pluginDataFn.data(&stat));
		MCHECKERROR(stat, "Failed to get SourceMPxData");
		
		///set pSource to new SourceMPxData
		MDagPath objPath = GetDagPath(thisMObject(), &stat);
		MCHECKERROR(stat, "Failed to get DagPath");
		pSourceMPxData->data() = std::make_shared<Source>(objPath,
		                                                  pRenderItem,
		                                                  sourceType::usd
		);
		
		///set SourceMPxData to outSourceObj
		stat = outSourceHnd.set(pSourceMPxData);
		MCHECKERROR(stat, "Failed to set sourceMPxData to outSourceHnd");
		
		/// Update the bounding box attributes
		stat = Base::computeBoundingBox_(dataBlock);
		MCHECKERROR(stat, "Failed to compute boundingBox");
		
		/////- set FileInfo
		//stat = setNotes_(datablock);
		//MCHECKERROR(stat, "Failed to set notes");
		
		stat = MGlobal::executePythonCommand("cmds.refreshEditorTemplates()");
		MCHECKERROR(stat, "Failed to refreshEditorTemplates");
		
		
		return stat;
	}
	MStatus updateRenderItem_(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->UsdProxyMPxShape);
		
		MStatus     stat;
		MDataHandle outSourceHnd = dataBlock.outputValue(outSourceObj);
		SourceMPxData* pMPxData = dynamic_cast<SourceMPxData*>(outSourceHnd.asPluginData());
		if(!pMPxData)
		{
			LOGGER_ERROR("Failed to get pMPxData");
			return MS::kFailure;
		}
		std::shared_ptr<Source>& pData = pMPxData->data();
		if(!pData)
		{
			LOGGER_ERROR("Failed to get pData");
			return MS::kFailure;
		}
		
		const MTime& inTime = dataBlock.outputValue(inTimeObj, &stat).asTime();
		MCHECKERROR(stat, "Failed to get inTime");
		const double& frameOffset = dataBlock.outputValue(frameOffsetObj, &stat).asDouble();
		MCHECKERROR(stat, "Failed to get frameOffset");
		const short& animCycle = dataBlock.outputValue(animCycleObj, &stat).asShort();
		MCHECKERROR(stat, "Failed to get animCycle");
		const bool& forceStatic = dataBlock.outputValue(forceStaticObj, &stat).asBool();
		MCHECKERROR(stat, "Failed to get forceStatic");
		const short& purpose = dataBlock.inputValue(purposeObj, &stat).asShort();
		
		double targetFrame = inTime.value() + frameOffset;
		LOGGER_DEBUG("targetFrame = %5.2f", targetFrame);
		
		const std::shared_ptr<RenderItem>& pRenderItem = pData->renderItem();
		if(!pRenderItem)
		{
			LOGGER_ERROR("Failed to get pRenderItem");
			return MS::kFailure;
		}
		
		pxr::TfToken purposeToken = UsdGeomTokens->default_;
		switch (purpose)
		{
			case 0:
				purposeToken = UsdGeomTokens->proxy;
				break;
			case 1:
				purposeToken = UsdGeomTokens->render;
				break;
			case 2:
				purposeToken = UsdGeomTokens->default_;
				break;
		}
		switch (pRenderItem->type())
		{
			case RenderItem::kInvalid:
				break;
			case RenderItem::kBBox:
			{
				const BBox3f bbox = reader_.bbox(purposeToken, targetFrame , animCycle);
				const std::shared_ptr<BBoxItem>& pDerived = std::dynamic_pointer_cast<BBoxItem>(pRenderItem);
				pDerived->bbox() = bbox;
			}
				break;
			case RenderItem::kExpandedGeom:
			{
				const BBox3f                        bbox  = reader_.bbox(purposeToken, animCycle, targetFrame );
				const std::shared_ptr<ExpandedGeom> pGeom = reader_.expandedSample(purposeToken, animCycle, targetFrame);
				
				const std::shared_ptr<ExpandedGeomItem>& pDerived = std::dynamic_pointer_cast<ExpandedGeomItem>(pRenderItem);
				pDerived->bbox() = bbox;
				pDerived->item()->pointArray  = pGeom->pointArray;
				pDerived->item()->normalArray = pGeom->normalArray;
				pDerived->item()->uvArray     = pGeom->uvArray;
				pDerived->item()->indexArray  = pGeom->indexArray;
			}
				break;
		}
		return stat;
	}
	
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxShapeUI
class UsdProxyMPxShapeUI : public ProxyMPxShapeUI
{
public:
	typedef ProxyMPxShapeUI Base;
public:
	enum {
		kDrawVertices, // component token
		kDrawWireframe,
		kDrawWireframeOnShaded,
		kDrawSmoothShaded,
		kDrawFlatShaded,
		kDrawBoundingBox,
		kDrawRedPointAtCenter,  // for userInteraction example code
		kLastToken
	};
public:
	static void* Creator()
	{
		return new UsdProxyMPxShapeUI;
	}
public:
	UsdProxyMPxShapeUI() : Base()
	{
	}
	virtual ~UsdProxyMPxShapeUI()
	{
	}
public:
};

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxShapeDrawOverride
class UsdProxyMPxShapeDrawOverride : public ProxyMPxShapeDrawOverride
{
public:
	typedef ProxyMPxShapeDrawOverride Base;
public:
	static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
	{
		return new UsdProxyMPxShapeDrawOverride(obj);
	}
public:
	UsdProxyMPxShapeDrawOverride(const MObject& obj) : Base(obj)
	{
	}
	virtual ~UsdProxyMPxShapeDrawOverride()
	{
	}
public:
};


NAMESPACE_BASE_END
