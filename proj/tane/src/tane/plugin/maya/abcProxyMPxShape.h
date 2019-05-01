#pragma once
#include "maya/pch.h"

#include "lib/baseMaya/util.h"

#include "maya/proxyMPxShape.h"
#include "maya/proxyMPxShapeDrawOverride.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShape
class AbcProxyMPxShape : public ProxyMPxShape
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
	static MObject renderObjectPathObj;
	static MObject useProxyFileObj;
	
	static MObject proxyFileObj;
	static MObject useProxyObjectPathObj;
	static MObject proxyObjectPathObj;
	
	static MObject flipvObj;    //AlembicRiProcedrual2 attrib

private:
	AbcReader renderReader_;
	AbcReader proxyReader_;

public:
	///------------------------------------------------------------ static functions
	static void* Creator()
	{
		return new AbcProxyMPxShape;
	}
	static MStatus 	Initialize()
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
		
		renderFileObj = tAttr.create(nodeAttr::abc->renderFile, nodeAttr::abc->renderFile, MFnData::kString, MObject::kNullObj, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc->renderFile);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(renderFileObj);
		
		renderObjectPathObj = tAttr.create(nodeAttr::abc->renderObjectPath, nodeAttr::abc->renderObjectPath, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc->renderObjectPath);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(renderObjectPathObj);
		
		useProxyFileObj = nAttr.create(nodeAttr::abc->useProxyFile, nodeAttr::abc->useProxyFile, MFnNumericData::kBoolean, false, &stat);
		MCHECKERROR(stat, "Failed create %s attribute", nodeAttr::abc->useProxyFile);
		SET_ATTRIBUTE_STATUS(nAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     true ,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(useProxyFileObj);
		
		proxyFileObj = tAttr.create(nodeAttr::abc->proxyFile, nodeAttr::abc->proxyFile, MFnData::kString, MObject::kNullObj, &stat);
		MCHECKERROR(stat, "Failed to %s attribute", nodeAttr::abc->proxyFile);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(proxyFileObj);
		
		useProxyObjectPathObj = nAttr.create(nodeAttr::abc->useProxyObjectPath, nodeAttr::abc->useProxyObjectPath, MFnNumericData::kBoolean, false, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc->useProxyObjectPath);
		SET_ATTRIBUTE_STATUS(nAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     true ,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(useProxyObjectPathObj);
		
		proxyObjectPathObj = tAttr.create(nodeAttr::abc->proxyObjectPath, nodeAttr::abc->proxyObjectPath, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc->proxyObjectPath);
		SET_ATTRIBUTE_STATUS(tAttr,
		                     false,                     /* Readable              */
		                     true,                      /* Writable              */
		                     false,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(proxyObjectPathObj);
		
		ATTRIBUTE_AFFECTS(renderFileObj,        outSourceObj);
		ATTRIBUTE_AFFECTS(renderObjectPathObj,  outSourceObj);
		ATTRIBUTE_AFFECTS(useProxyFileObj,      outSourceObj);
		
		ATTRIBUTE_AFFECTS(proxyFileObj,          outSourceObj);
		ATTRIBUTE_AFFECTS(useProxyObjectPathObj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyObjectPathObj,    outSourceObj);
		
		
		
		return stat;
	}

public:
	AbcProxyMPxShape() : Base()
	{
	}
	virtual ~AbcProxyMPxShape()
	{
	}
public:
	virtual void postConstructor()
	{
		Base::postConstructor();
		DEFAULT_DAG_POST_CONSTRUCTOR(RegisteredNodes->AbcProxyMPxShape.name);
	}
	virtual MStatus setDependentsDirty(const MPlug &plug, MPlugArray &plugArray)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		stat = Base::setDependentsDirty(plug, plugArray);
		MCHECKERROR(stat, "Failed to Base::setDependentsDirty");
		
		if( plug == renderFileObj           ||
		    plug == renderObjectPathObj     ||
			plug == useProxyFileObj         ||
			plug == proxyFileObj            ||
			plug == useProxyObjectPathObj   ||
			plug == proxyObjectPathObj
		)
		{
			LOGGER_INFO("plug.info() = %s", plug.info().asChar());
			stateQueue()->push(State::kARCHIVE_ATTRIBUTE_CHANGED);
			signalDirtyToViewport();
		}
		else
		{
			LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
		}
		return stat;
	}
	virtual MStatus nodeDeleted(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus nodeAdded(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus archiveAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus displayModeAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus shaderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus renderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus timeChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		MStatus stat;
		stat = updateRenderItem_(dataBlock);
		MCHECKERROR(stat, "Failed to update renderItem");
		return stat;
	}
private:
	MStatus computeOutSource_(MDataBlock &dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		
		MStatus stat;
		
		const MTime&    inTime              = dataBlock.inputValue(inTimeObj, &stat).asTime();                 MCHECKERROR(stat, "Failed to get inTime");
		const bool& 	forceStatic         = dataBlock.inputValue(forceStaticObj, &stat).asBool();            MCHECKERROR(stat, "Failed to get forceStatic");
		const double&   frameOffset         = dataBlock.inputValue(frameOffsetObj, &stat).asDouble();          MCHECKERROR(stat, "Failed to get frameOffset");
		const short&    animCycle           = dataBlock.inputValue(animCycleObj, &stat).asShort();             MCHECKERROR(stat, "Failed to get animCycle");
		const MString& 	filePath 	        = dataBlock.inputValue(renderFileObj, &stat).asString();           MCHECKERROR(stat, "Failed to get filePath");
		const bool& 	drawAsBBox 	        = dataBlock.inputValue(useProxyFileObj, &stat).asBool();       MCHECKERROR(stat, "Failed to get drawAsBBox");
		const MString& 	objectPath 	        = dataBlock.inputValue(renderObjectPathObj, &stat).asString();         MCHECKERROR(stat, "Failed to get objpath");
		const MString& 	proxyPath 	        = dataBlock.inputValue(proxyFileObj, &stat).asString();            MCHECKERROR(stat, "Failed to get proxyPath");
		const bool& 	useDifferentObjPath = dataBlock.inputValue(useProxyObjectPathObj, &stat).asBool();     MCHECKERROR(stat, "Failed to get useDifferentObjPath");
		const MString& 	objPathForProxy 	= dataBlock.inputValue(proxyObjectPathObj, &stat).asString();       MCHECKERROR(stat, "Failed to get objPathForProxy");
		
		
		LOGGER_DEBUG("inTime               = %5.2f", inTime.value());
		LOGGER_DEBUG("forceStatic          = %s", (forceStatic ? "true" : "false"));
		LOGGER_DEBUG("frameOffset          = %5.2f", frameOffset);
		LOGGER_DEBUG("animCycle            = %s", mode::animCycle::ToString(animCycle).c_str());
		LOGGER_DEBUG("filePath             = %s", filePath.asChar());
		LOGGER_DEBUG("drawAsBBox           = %s", (drawAsBBox ? "true " : "false"));
		LOGGER_DEBUG("objectPath           = %s", objectPath.asChar());
		LOGGER_DEBUG("proxyPath            = %s", proxyPath.asChar());
		LOGGER_DEBUG("useDiffernectObjPath = %s", (useDifferentObjPath ? "true" : "false"));
		LOGGER_DEBUG("objPathForProxy      = %s", objPathForProxy.asChar());
		
		////- clear FileInfo
		//stat = clearNotes_(datablock);
		//MCHECKERROR(stat, "Failed to clear notes");
		
		if(drawAsBBox)
		{
			/* get bounding box from render file*/
			if(!renderReader_.open(filePath.asChar(), objectPath.asChar()))
			{
				std::string msg;
				msg = (boost::format("Failed open render file, %s:%s") % filePath.asChar() % objectPath.asChar()).str();
				
				MGlobal::displayError(msg.c_str());
				STDOUT_ERROR("%s", msg.c_str());
				return MS::kFailure;
			}
		}
		else
		{
			/* valid check render file only with objpath */
			AbcReader renderReader;
			if(!renderReader.open(filePath.asChar(), objectPath.asChar()))
			{
				std::string msg;
				msg = (boost::format("Failed open render file, %s:%s") % filePath.asChar() % objectPath.asChar()).str();
				
				MGlobal::displayError(msg.c_str());
				STDOUT_ERROR("%s", msg.c_str());
				return MS::kFailure;
			}
			
			if(!useDifferentObjPath)
			{
				/* use same object path */
				if(!proxyReader_.open(proxyPath.asChar(), objectPath.asChar()))
				{
					std::string msg;
					msg = (boost::format("Failed open proxy file, %s:%s") % filePath.asChar() % objectPath.asChar()).str();
					
					
					MGlobal::displayError(msg.c_str());
					STDOUT_ERROR("%s", msg.c_str());
					return MS::kFailure;
				}
			}
			else
			{
				if(!proxyReader_.open(proxyPath.asChar(), objPathForProxy.asChar()))
				{
					std::string msg;
					msg = (boost::format("Failed open proxy file, %s:%s") % filePath.asChar() % objectPath.asChar()).str();
					
					MGlobal::displayError(msg.c_str());
					STDOUT_ERROR("%s", msg.c_str());
					return MS::kFailure;
				}
			}
		}
		
		Time targetTime = GetTime(inTime);
		targetTime.sec += (frameOffset / targetTime.fps);
		LOGGER_DEBUG("targetTime = (%5.2f, %5.2f)", targetTime.sec, targetTime.fps);
		
		/* set RenderItem (make renderItem for proxy(Abc Case : BBoxItem or GeomItem)) */
		std::shared_ptr<RenderItem> pRenderItem = nullptr;
		if(drawAsBBox)
		{
			LOGGER_IN("make BBoxItem from render file");
			/* set item */
			const BBox3f bbox = proxyReader_.bbox(animCycle, targetTime.sec);
			
			/* create renderItem */
			pRenderItem = std::make_shared<BBoxItem>(bbox);
			LOGGER_OUT();
		}
		else
		{
			LOGGER_IN("make IndexedGeomItem from proxy file");
			/* set item */
			const BBox3f                        bbox  = proxyReader_.bbox(animCycle, targetTime.sec);
			const std::shared_ptr<ExpandedGeom> pGeom = proxyReader_.expandedSample(animCycle, targetTime.sec);
			
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
			pRenderItem  = std::make_shared<ExpandedGeomItem>(bbox, pGeom);
			LOGGER_OUT();
		}
		if(!pRenderItem )
		{
			STDOUT_ERROR("Failed to create valid renderItem");
			return MS::kFailure;
		}
		
		///----------------------------------set source properties end
		MDataHandle 	outSourceHnd = dataBlock.outputValue(outSourceObj, &stat);
		///create SourceMPxData
		MFnPluginData pluginDataFn;
		pluginDataFn.create(MTypeId(SourceMPxData::id), &stat);                                     MCHECKERROR(stat, "Failed to create SourceMPxData");
		SourceMPxData* pSourceMPxData = dynamic_cast<SourceMPxData*>(pluginDataFn.data(&stat));     MCHECKERROR(stat, "Failed to get SourceMPxData");
		
		///set pSource to new SourceMPxData
		MDagPath objPath = GetDagPath(thisMObject(), &stat);
		MCHECKERROR(stat, "Failed to get DagPath");
		pSourceMPxData->data() = std::make_shared<SourceAbc>(objPath,
		                                                     pRenderItem,
		                                                     filePath.asChar(),
		                                                     drawAsBBox,
		                                                     objectPath.asChar(),
		                                                     proxyPath.asChar(),
		                                                     useDifferentObjPath,
		                                                     objPathForProxy.asChar()
		);
		
		///set SourceMPxData to outSourceObj
		stat = outSourceHnd.set(pSourceMPxData);  MCHECKERROR(stat, "Failed to set sourceMPxData to outSourceHnd");
		
		/// Update the bounding box attributes
		stat = Base::computeBoundingBox_(dataBlock);    MCHECKERROR(stat, "Failed to compute boundingBox");
		
		/////- set FileInfo
		//stat = setNotes_(datablock);
		//MCHECKERROR(stat, "Failed to set notes");
		
		return stat;
	}
	MStatus updateRenderItem_(MDataBlock &dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->AbcProxyMPxShape);
		
		MStatus stat;
		MDataHandle outSourceHnd = dataBlock.outputValue(outSourceObj);
		SourceMPxData* pMPxData = dynamic_cast<SourceMPxData*>(outSourceHnd.asPluginData());
		if(!pMPxData)
		{
			STDOUT_ERROR("Failed to get pMPxData");
			return MS::kFailure;
		}
		std::shared_ptr<Source>& pData = pMPxData->data();
		if(!pData)
		{
			STDOUT_ERROR("Failed to get pData");
			return MS::kFailure;
		}
		
		const MTime&    inTime              = dataBlock.outputValue(inTimeObj, &stat).asTime();        MCHECKERROR(stat, "Failed to get inTime");
		const double&   frameOffset         = dataBlock.outputValue(frameOffsetObj, &stat).asDouble(); MCHECKERROR(stat, "Failed to get frameOffset");
		const short&    animCycle           = dataBlock.outputValue(animCycleObj, &stat).asShort();    MCHECKERROR(stat, "Failed to get animCycle");
		const bool& 	forceStatic         = dataBlock.outputValue(forceStaticObj, &stat).asBool();   MCHECKERROR(stat, "Failed to get forceStatic");
		
		Time targetTime = GetTime(inTime);
		targetTime.sec += (frameOffset / targetTime.fps);
		LOGGER_DEBUG("targetTime = (%5.2f, %5.2f)", targetTime.sec, targetTime.fps);
		
		const std::shared_ptr<RenderItem>& pRenderItem = pData->renderItem();
		if(!pRenderItem)
		{
			STDOUT_ERROR("Failed to get pRenderItem");
			return MS::kFailure;
		}
		
		
		switch (pRenderItem->type())
		{
			case RenderItem::kInvalid:
				break;
			case RenderItem::kBBox:
			{
				const BBox3f bbox = proxyReader_.bbox(targetTime.sec, animCycle);
				
				const std::shared_ptr<BBoxItem>& pDerived = std::dynamic_pointer_cast<BBoxItem>(pRenderItem) ;
				pDerived->bbox() = bbox;
			}
				break;
			case RenderItem::kExpandedGeom:
			{
				const std::shared_ptr<ExpandedGeomItem>& pDerived = std::dynamic_pointer_cast<ExpandedGeomItem>(pRenderItem) ;
				
				const BBox3f                        bbox  = proxyReader_.bbox(animCycle, targetTime.sec);
				const std::shared_ptr<ExpandedGeom> pGeom = proxyReader_.expandedSample(animCycle, targetTime.sec);
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
///------------------------------------------------------------------ AbcProxyMPxShapeUI
class AbcProxyMPxShapeUI : public ProxyMPxShapeUI
{
public:
	typedef ProxyMPxShapeUI Base;
public:
	static void* Creator()
	{
		return new AbcProxyMPxShapeUI;
	}
public:
	AbcProxyMPxShapeUI() : Base()
	{
	}
	virtual ~AbcProxyMPxShapeUI()
	{
	}
public:

};

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeDrawOverride
class AbcProxyMPxShapeDrawOverride : public ProxyMPxShapeDrawOverride
{
public:
	typedef ProxyMPxShapeDrawOverride Base;
public:
	static MHWRender::MPxDrawOverride *	Creator(const MObject &obj)
	{
		return new AbcProxyMPxShapeDrawOverride(obj);
	}
public:
	AbcProxyMPxShapeDrawOverride(const MObject &obj) : Base(obj)
	{
	}
	virtual ~AbcProxyMPxShapeDrawOverride()
	{
	}
public:
};
NAMESPACE_BASE_END
