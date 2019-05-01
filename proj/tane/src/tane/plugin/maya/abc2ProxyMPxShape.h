#pragma once
#include "maya/pch.h"
#include "maya/proxyMPxShape.h"
#include "maya/proxyMPxShapeDrawOverride.h"

#include "lib/baseMaya/util.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxSurfaceShape
class Abc2ProxyMPxShape : public ProxyMPxShape
{
public:
	typedef ProxyMPxShape Base;

public:
	static MTypeId id;
	static MString nodeName;
	static MString drawOverrideRegistrantId;
	static MString drawDbClassification;

public:
	///------------------------------------------------------------ node attributes
	static MObject filePath0Obj;
	static MObject objectPath0Obj;
	static MObject proxyPath0Obj;
	static MObject proxyObjectPath0Obj;
	
	static MObject useLOD1Obj;
	static MObject filePath1Obj;
	static MObject objectPath1Obj;
	static MObject proxyPath1Obj;
	static MObject proxyObjectPath1Obj;

	static MObject useLOD2Obj;
	static MObject filePath2Obj;
	static MObject objectPath2Obj;
	static MObject proxyPath2Obj;
	static MObject proxyObjectPath2Obj;
	
private:
	AbcReader   renderReader_[LOD::maxLevel];
	AbcReader   proxyReader_[LOD::maxLevel];
	
public:
	///------------------------------------------------------------ static functions
	static void* Creator()
	{
		return new Abc2ProxyMPxShape;
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
		
		filePath0Obj = tAttr.create(nodeAttr::abc2->filePath0, nodeAttr::abc2->filePath0,  MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->filePath0);
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
		ADD_ATTRIBUTE(filePath0Obj);
		objectPath0Obj = tAttr.create(nodeAttr::abc2->objectPath0, nodeAttr::abc2->objectPath0, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->objectPath0);
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
		ADD_ATTRIBUTE(objectPath0Obj);
		
		proxyPath0Obj = tAttr.create(nodeAttr::abc2->proxyPath0, nodeAttr::abc2->proxyPath0, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyPath0);
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
		ADD_ATTRIBUTE(proxyPath0Obj);
		proxyObjectPath0Obj = tAttr.create(nodeAttr::abc2->proxyObjectPath0, nodeAttr::abc2->proxyObjectPath0, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyObjectPath0);
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
		ADD_ATTRIBUTE(proxyObjectPath0Obj);
		
		
		useLOD1Obj = nAttr.create(nodeAttr::abc2->useLOD1, nodeAttr::abc2->useLOD1, MFnNumericData::kBoolean, false, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->useLOD1);
		SET_ATTRIBUTE_STATUS(nAttr,
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
		ADD_ATTRIBUTE(useLOD1Obj);
		filePath1Obj = tAttr.create(nodeAttr::abc2->filePath1, nodeAttr::abc2->filePath1, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->filePath1);
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
		ADD_ATTRIBUTE(filePath1Obj);
		objectPath1Obj = tAttr.create(nodeAttr::abc2->objectPath1, nodeAttr::abc2->objectPath1, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->objectPath1);
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
		ADD_ATTRIBUTE(objectPath1Obj);
		
		proxyPath1Obj = tAttr.create(nodeAttr::abc2->proxyPath1, nodeAttr::abc2->proxyPath1, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyPath1);
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
		ADD_ATTRIBUTE(proxyPath1Obj);
		proxyObjectPath1Obj = tAttr.create(nodeAttr::abc2->proxyObjectPath1, nodeAttr::abc2->proxyObjectPath1, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyObjectPath1);
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
		ADD_ATTRIBUTE(proxyObjectPath1Obj);
		
		useLOD2Obj = nAttr.create(nodeAttr::abc2->useLOD2, nodeAttr::abc2->useLOD2, MFnNumericData::kBoolean, false, &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->useLOD2);
		SET_ATTRIBUTE_STATUS(nAttr,
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
		ADD_ATTRIBUTE(useLOD2Obj);
		filePath2Obj = tAttr.create(nodeAttr::abc2->filePath2, nodeAttr::abc2->filePath2, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->filePath2);
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
		ADD_ATTRIBUTE(filePath2Obj);
		objectPath2Obj = tAttr.create(nodeAttr::abc2->objectPath2, nodeAttr::abc2->objectPath2, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->objectPath2);
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
		ADD_ATTRIBUTE(objectPath2Obj);
		proxyPath2Obj = tAttr.create(nodeAttr::abc2->proxyPath2, nodeAttr::abc2->proxyPath2, MFnData::kString, MFnStringData().create(""), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyPath2);
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
		ADD_ATTRIBUTE(proxyPath2Obj);
		proxyObjectPath2Obj = tAttr.create(nodeAttr::abc2->proxyObjectPath2, nodeAttr::abc2->proxyObjectPath2, MFnData::kString, MFnStringData().create("/"), &stat);
		MCHECKERROR(stat, "Failed to create %s attribute", nodeAttr::abc2->proxyObjectPath2);
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
		ADD_ATTRIBUTE(proxyObjectPath2Obj);
		
		ATTRIBUTE_AFFECTS(filePath0Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(objectPath0Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyPath0Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyObjectPath0Obj, outSourceObj);
		
		ATTRIBUTE_AFFECTS(useLOD1Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(filePath1Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(objectPath1Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyPath1Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyObjectPath1Obj, outSourceObj);
		
		ATTRIBUTE_AFFECTS(useLOD2Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(filePath2Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(objectPath2Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyPath2Obj, outSourceObj);
		ATTRIBUTE_AFFECTS(proxyObjectPath2Obj, outSourceObj);
		
		return stat;
		
	}

public:
	///------------------------------------------------------------ constructor / destructor
	Abc2ProxyMPxShape() : Base()
	{
	}
	virtual ~Abc2ProxyMPxShape()
	{
	}

public:
	///------------------------------------------------------------ virtual functions
	virtual void postConstructor()
	{
		Base::postConstructor();
		DEFAULT_DAG_POST_CONSTRUCTOR(RegisteredNodes->Abc2ProxyMPxShape.name);
	}
	virtual MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		stat = Base::setDependentsDirty(plug, plugArray);
		MCHECKERROR(stat, "Failed to Base::setDependentsDirty");
		
		if(plug == filePath0Obj ||
		   plug == objectPath0Obj ||
		   plug == proxyPath0Obj ||
		   plug == proxyObjectPath0Obj ||
		   plug == useLOD1Obj ||
		   plug == filePath1Obj ||
		   plug == objectPath1Obj ||
		   plug == proxyPath1Obj ||
		   plug == proxyObjectPath1Obj ||
		   plug == useLOD2Obj ||
		   plug == filePath2Obj ||
		   plug == objectPath2Obj ||
		   plug == proxyPath2Obj ||
		   plug == proxyObjectPath2Obj)
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
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus nodeAdded(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus archiveAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus displayModeAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		stat = computeOutSource_(dataBlock);
		MCHECKERROR(stat, "Failed to compute outSource");
		return stat;
	}
	virtual MStatus shaderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus renderAttribChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		return stat;
	}
	virtual MStatus timeChanged(MDataBlock& dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		MStatus stat;
		stat = updateRenderItem_(dataBlock);
		MCHECKERROR(stat, "Failed to update renderItem");
		return stat;
	}
	
public:
	///------------------------------------------------------------ public helper functions

private:
	///------------------------------------------------------------ private helper functions
	MStatus computeOutSource_(MDataBlock &dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		
		MStatus stat = MS::kSuccess;
		
		/* clear FileInfo */
		//stat = clearNotes_(datablock);
		//MCHECKERROR(stat, "Failed to clear notes");
		
		const MTime & inTime      = dataBlock.inputValue(inTimeObj, &stat).asTime();
		const double& frameOffset = dataBlock.inputValue(frameOffsetObj, &stat).asDouble();
		const short & animCycle   = dataBlock.inputValue(animCycleObj, &stat).asShort();
		
		const MString& filePath0        = dataBlock.inputValue(filePath0Obj, &stat).asString();
		const MString& objectPath0      = dataBlock.inputValue(objectPath0Obj, &stat).asString();
		const MString& proxyPath0       = dataBlock.inputValue(proxyPath0Obj, &stat).asString();
		const MString& proxyObjectPath0 = dataBlock.inputValue(proxyObjectPath0Obj, &stat).asString();
		
		const bool   & useLOD1          = dataBlock.inputValue(useLOD1Obj, &stat).asBool();
		const MString& filePath1        = dataBlock.inputValue(filePath1Obj, &stat).asString();
		const MString& objectPath1      = dataBlock.inputValue(objectPath1Obj, &stat).asString();
		const MString& proxyPath1       = dataBlock.inputValue(proxyPath1Obj, &stat).asString();
		const MString& proxyObjectPath1 = dataBlock.inputValue(proxyObjectPath1Obj, &stat).asString();
		
		const bool   & useLOD2          = dataBlock.inputValue(useLOD2Obj, &stat).asBool();
		const MString& filePath2        = dataBlock.inputValue(filePath2Obj, &stat).asString();
		const MString& objectPath2      = dataBlock.inputValue(objectPath2Obj, &stat).asString();
		const MString& proxyPath2       = dataBlock.inputValue(proxyPath2Obj, &stat).asString();
		const MString& proxyObjectPath2 = dataBlock.inputValue(proxyObjectPath2Obj, &stat).asString();
		
		const bool& forceStatic = dataBlock.inputValue(forceStaticObj, &stat).asBool();
		
		//LOGGER_DEBUG("filePath0  = %s", filePath0.asChar());
		//LOGGER_DEBUG("proxyPath0 = %s", proxyPath0.asChar());
		//
		//LOGGER_DEBUG("useLOD1    = %d", useLOD1);
		//LOGGER_DEBUG("filePath1  = %s", filePath1.asChar());
		//LOGGER_DEBUG("proxyPath1 = %s", proxyPath1.asChar());
		//
		//LOGGER_DEBUG("useLOD2    = %d", useLOD2);
		//LOGGER_DEBUG("filePath2  = %s", filePath2.asChar());
		//LOGGER_DEBUG("proxyPath2 = %s", proxyPath2.asChar());
		
		/* check valid render alembic files loaded */
		if(!renderReader_[0].open(filePath0.asChar(), objectPath0.asChar()))
		{
			STDOUT_ERROR("Failed to open renderFile0, %s:%s", filePath0.asChar(), objectPath0.asChar());
			return MS::kFailure;
		}
		if(useLOD1 && !renderReader_[1].open(filePath1.asChar(), objectPath1.asChar()))
		{
			STDOUT_ERROR("Failed to open renderFile1, %s:%s", filePath1.asChar(), objectPath1.asChar());
			return MS::kFailure;
		}
		if(useLOD2 && !renderReader_[2].open(filePath2.asChar(), objectPath2.asChar()))
		{
			STDOUT_ERROR("Failed to open renderFile2, %s:%s", filePath2.asChar(), objectPath2.asChar());
			return MS::kFailure;
		}
		
		/* check valid proxy alembic files loaded */
		if(!proxyReader_[0].open(proxyPath0.asChar(), proxyObjectPath0.asChar()))
		{
			STDOUT_ERROR("Failed to open proxyFile0, %s:%s", proxyPath0.asChar(), proxyObjectPath0.asChar());
			return MS::kFailure;
		}
		if(useLOD1 && !proxyReader_[1].open(proxyPath1.asChar(), proxyObjectPath1.asChar()))
		{
			STDOUT_ERROR("Failed to open proxyFile1, %s:%s", proxyPath1.asChar(), proxyObjectPath1.asChar());
			return MS::kFailure;
		}
		if(useLOD2 && !proxyReader_[2].open(proxyPath2.asChar(), proxyObjectPath2.asChar()))
		{
			STDOUT_ERROR("Failed to open proxyFile2, %s:%s", proxyPath2.asChar(), proxyObjectPath2.asChar());
			return MS::kFailure;
		}
		
		Time targetTime = GetTime(inTime);
		targetTime.sec += (frameOffset / targetTime.fps);
		LOGGER_DEBUG("targetTime = (%5.2f, %5.2f)", targetTime.sec, targetTime.fps);
		
		/* build geometry data for proxy drawing */
		std::shared_ptr<RenderItem> pRenderItem = nullptr;
		LOGGER_IN("make lod geometry data");
		{
			const BBox3f bbox                         = proxyReader_[0].bbox(animCycle, targetTime.sec);
			const std::shared_ptr<ExpandedGeom> pGeom = CreateExpandedLODGeom(animCycle,
			                                                                  targetTime.sec,
			                                                                  bbox,
			                                                                  proxyReader_[0],
			                                                                  useLOD1 ? proxyReader_[1] : AbcReader(),
			                                                                  useLOD2 ? proxyReader_[2] : AbcReader()
			);
			
			/* create renderItem */
			pRenderItem  = std::make_shared<ExpandedGeomItem>(bbox, pGeom);
		}
		LOGGER_OUT();
		
		if(!pRenderItem)
		{
			STDOUT_ERROR("Failed to get valid pRenderItem");
			return MS::kFailure;
		}
		
		
		MDataHandle   outSourceHnd = dataBlock.outputValue(outSourceObj, &stat);
		/* create SourceMPxData */
		MFnPluginData pluginDataFn;
		pluginDataFn.create(MTypeId(SourceMPxData::id), &stat);
		MCHECKERROR(stat, "Failed to create SourceMPxData");
		SourceMPxData* pSourceMPxData = dynamic_cast<SourceMPxData*>(pluginDataFn.data(&stat));
		MCHECKERROR(stat, "Failed to get SourceMPxData");
		
		/* set pSource to new SourceMPxData */
		MDagPath objPath = GetDagPath(thisMObject(), &stat);
		MCHECKERROR(stat, "Failed to get DagPath");
		pSourceMPxData->data() = std::make_shared<SourceAlembic>(objPath,
		                                                         pRenderItem,
		                                                         filePath0,
		                                                         objectPath0,
		                                                         proxyPath0,
		                                                         proxyObjectPath0,
		                                                         useLOD1,
		                                                         filePath1,
		                                                         objectPath1,
		                                                         proxyPath1,
		                                                         proxyObjectPath1,
		                                                         useLOD2,
		                                                         filePath2,
		                                                         objectPath2,
		                                                         proxyPath2,
		                                                         proxyObjectPath2
		);
		
		/* set SourceMPxData to outSourceObj */
		stat = outSourceHnd.set(pSourceMPxData);
		MCHECKERROR(stat, "Failed to set sourceMPxData to outSourceHnd");
		
		/* Update the bounding box attributes */
		stat = Base::computeBoundingBox_(dataBlock);
		MCHECKERROR(stat, "Failed to compute boundingBox");
		
		///* set FileInfo */
		//stat = setNotes_(datablock);
		//MCHECKERROR(stat, "Failed to set notes");
		return stat;
	}
	
	MStatus updateRenderItem_(MDataBlock &dataBlock)
	{
		DECL_DG_LOGGER(logging::maya->Abc2ProxyMPxShape);
		
		MStatus     stat;
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
		
		const MTime& inTime = dataBlock.outputValue(inTimeObj, &stat).asTime();
		MCHECKERROR(stat, "Failed to get inTime");
		const double& frameOffset = dataBlock.outputValue(frameOffsetObj, &stat).asDouble();
		MCHECKERROR(stat, "Failed to get frameOffset");
		const short& animCycle = dataBlock.outputValue(animCycleObj, &stat).asShort();
		MCHECKERROR(stat, "Failed to get animCycle");
		const bool& forceStatic = dataBlock.outputValue(forceStaticObj, &stat).asBool();
		MCHECKERROR(stat, "Failed to get forceStatic");
		
		const bool& useLOD1 = dataBlock.inputValue(useLOD1Obj, &stat).asBool();
		const bool& useLOD2 = dataBlock.inputValue(useLOD2Obj, &stat).asBool();
		
		Time targetTime = GetTime(inTime);
		targetTime.sec += (frameOffset / targetTime.fps);
		LOGGER_INFO("targetTime = (%5.2f, %5.2f)", targetTime.sec, targetTime.fps);
		
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
				const std::shared_ptr<BBoxItem>& pCurItem = std::dynamic_pointer_cast<BBoxItem>(pRenderItem);
				
				const BBox3f bbox0 = proxyReader_[0].bbox(targetTime.sec, animCycle);
				pCurItem->bbox() = bbox0;
			}
			break;
			case RenderItem::kExpandedGeom:
			{
				const std::shared_ptr<ExpandedGeomItem>& pCurItem = std::dynamic_pointer_cast<ExpandedGeomItem>(pRenderItem);
				
				const BBox3f                        bbox0 = proxyReader_[0].bbox(targetTime.sec, animCycle);
				const std::shared_ptr<ExpandedGeom> pGeom = CreateExpandedLODGeom(animCycle,
				                                                                  targetTime.sec,
				                                                                  bbox0,
				                                                                  proxyReader_[0],
				                                                                  useLOD1 ? proxyReader_[1] : AbcReader(),
				                                                                  useLOD2 ? proxyReader_[2] : AbcReader()
				);
				if(!pGeom)
				{
					STDOUT_ERROR("Failed to get pGeom");
					return MS::kFailure;
				}
				
				pCurItem->bbox() = bbox0;
				pCurItem->item()->pointArray  = pGeom->pointArray;
				pCurItem->item()->normalArray = pGeom->normalArray;
				pCurItem->item()->uvArray     = pGeom->uvArray;
				pCurItem->item()->indexArray  = pGeom->indexArray;
			}
			break;
		}
		return stat;

	}
	
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxSurfaceShapeUI
class Abc2ProxyMPxShapeUI : public MPxSurfaceShapeUI
{
public:
	static void* Creator()
	{
		return new Abc2ProxyMPxShapeUI ;
	}

public:
	///------------------------------------------------------------ constructor / destructor
	Abc2ProxyMPxShapeUI ()
	{
	}
	virtual ~Abc2ProxyMPxShapeUI ()
	{
	}
public:
	///------------------------------------------------------------ virtual functions
	virtual void getDrawRequests(const MDrawInfo& info, bool objectAndActiveOnly, MDrawRequestQueue& requests)
	{
		MPxSurfaceShapeUI::getDrawRequests(info, objectAndActiveOnly, requests);
	}
	virtual void draw(const MDrawRequest& request, M3dView& view) const
	{
		MPxSurfaceShapeUI::draw(request, view);
	}
	virtual bool select(MSelectInfo& selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const
	{
		return MPxSurfaceShapeUI::select(selectInfo, selectionList, worldSpaceSelectPts);
	}
	
};
NAMESPACE_BASE_END


NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AlembicProxyMPxShapeDrawOverride
class Abc2ProxyMPxShapeDrawOverride: public ProxyMPxShapeDrawOverride
{
public:
	typedef ProxyMPxShapeDrawOverride Base;
public:
	static MHWRender::MPxDrawOverride *	Creator(const MObject &obj)
	{
		return new Abc2ProxyMPxShapeDrawOverride(obj);
	}
public:
	Abc2ProxyMPxShapeDrawOverride(const MObject &obj) : Base(obj)
	{
	}
	virtual ~Abc2ProxyMPxShapeDrawOverride()
	{
	}
	
};
NAMESPACE_BASE_END
