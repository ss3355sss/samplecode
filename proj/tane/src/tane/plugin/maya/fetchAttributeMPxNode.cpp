#include "maya/fetchAttributeMPxNode.h"

#include "baseMaya/util.h"
#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------- DistributorNode
MTypeId FetchAttributeMPxNode::id(RegisteredNodes->FetchAttributeMPxNode.id);
MString FetchAttributeMPxNode::nodeName(RegisteredNodes->FetchAttributeMPxNode.name);

///------------------------------------------------------------ node attribute
MObject FetchAttributeMPxNode::npointsObj;
MObject FetchAttributeMPxNode::idObj;
///------------------------------------------------------------ input attributes
IMPL_INPUT_ATTRIBUTE(FetchAttributeMPxNode);

///------------------------------------------------------------ output attributes
MObject FetchAttributeMPxNode::outAttributeObj;
	MObject FetchAttributeMPxNode::outPidObj;
	MObject FetchAttributeMPxNode::outSidObj;
	MObject FetchAttributeMPxNode::outRenderableObj;
	MObject FetchAttributeMPxNode::outPositionObj;
	MObject FetchAttributeMPxNode::outEulerAnglesObj;
	MObject FetchAttributeMPxNode::outScaleObj;
	//MObject FetchAttributeMPxNode::outLocalAxisMtxObj;
	//MObject FetchAttributeMPxNode::outTextureCoordObj;
	//MObject FetchAttributeMPxNode::outRadiusObj;

///------------------------------------------------------------ static functions
void* FetchAttributeMPxNode::Creator()
{
	return new FetchAttributeMPxNode;
}
MStatus FetchAttributeMPxNode::Initialize()
{
	MStatus              stat;
	MFnUnitAttribute     uAttr;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnCompoundAttribute cAttr;
	MFnMatrixAttribute   mAttr;
	MFnEnumAttribute     eAttr;
	
	///------------------------------------------------------------ input attributes
	INITIALIZE_INPUT_ATTRIBUTE;

	///------------------------------------------------------------ node attribute
	npointsObj = nAttr.create("npoints", "npoints", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create npoints attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     true,                      /* Connectable           */
	                     true,                      /* Stroable              */
	                     true,                      /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     true,                      /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(npointsObj);
	
	idObj = nAttr.create("id", "id", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create pid attribute");
	nAttr.setMin(0);
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                    /* Readable              */
	                     true,                     /* Writable              */
	                     true,                     /* Connectable           */
	                     true,                     /* Stroable              */
	                     true,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     true,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(idObj);

	///------------------------------------------------------------ output attributes
	outAttributeObj = cAttr.create("outAttribute", "outAttribute", &stat);
		outPidObj = nAttr.create("outPid", "outPid", MFnNumericData::kInt, 0, &stat);
		MCHECKERROR(stat, "Failed to create outPid attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                    /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kNothing,   /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outPidObj);
		outSidObj = nAttr.create("outSid", "outSid", MFnNumericData::kInt, 0, &stat);
		MCHECKERROR(stat, "Failed to create outSid attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                     /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outSidObj);
		outRenderableObj = nAttr.create("outRenderable", "outRenderable", MFnNumericData::kInt, 0, &stat);
		MCHECKERROR(stat, "Failed to create outRenderable attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                     /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outRenderableObj);
		outPositionObj = nAttr.create("outPosition", "outPosition", MFnNumericData::k3Double, 0, &stat);
		MCHECKERROR(stat, "Failed to create outPosition attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                     /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outPositionObj);
		outEulerAnglesObj = nAttr.create("outEulerAngles", "outEulerAngles", MFnNumericData::k3Double, 0, &stat);
		MCHECKERROR(stat, "Failed to create outEulerAngles attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                     /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outEulerAnglesObj);
		outScaleObj = nAttr.create("outScale", "outScale", MFnNumericData::k3Double, 0, &stat);
		MCHECKERROR(stat, "Failed to create outScale attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true,                     /* Readable              */
		                     false,                     /* Writable              */
		                     true,                     /* Connectable           */
		                     true,                     /* Stroable              */
		                     true,                     /* Cached                */
		                     false,                    /* ChannelBox            */
		                     false,                    /* Array                 */
		                     false,                    /* Use Array DataBuilder */
		                     false,                     /* Keyable               */
		                     false,                    /* Hidden                */
		                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
		                     false                     /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(outScaleObj);
	cAttr.addChild(outPidObj);
	cAttr.addChild(outSidObj);
	cAttr.addChild(outRenderableObj);
	cAttr.addChild(outPositionObj);
	cAttr.addChild(outEulerAnglesObj);
	cAttr.addChild(outScaleObj);
	SET_ATTRIBUTE_STATUS(cAttr,
	                     true,                     /* Readable              */
	                     false,                     /* Writable              */
	                     true,                     /* Connectable           */
	                     true,                     /* Stroable              */
	                     true,                     /* Cached                */
	                     false,                    /* ChannelBox            */
	                     false,                    /* Array                 */
	                     false,                    /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                    /* Hidden                */
	                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
	                     false                     /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(outAttributeObj);
	
	///------------------------------------------------------------ set input/output attribute affects
	ATTRIBUTE_AFFECTS(inputObj,    outAttributeObj);
	ATTRIBUTE_AFFECTS(idObj,       outAttributeObj);

	
	return MS::kSuccess;
}

///------------------------------------------------------------ constructor / destructor
FetchAttributeMPxNode::FetchAttributeMPxNode()
{
}
FetchAttributeMPxNode::~FetchAttributeMPxNode()
{
}

///------------------------------------------------------------ virtual functions
void FetchAttributeMPxNode::postConstructor()
{
	DEFAULT_DG_POST_CONSTRUCTOR(RegisteredNodes->FetchAttributeMPxNode.name);
	pMsgQueue_ = std::make_shared<MessageQueue>();
}
MStatus FetchAttributeMPxNode::setDependentsDirty(const MPlug& plug, MPlugArray& affectedPlugs)
{
	DECL_LOGGER(RegisteredNodes->FetchAttributeMPxNode.logging);
	
	MStatus stat = MS::kSuccess;
	
	///------------------------------------------------------------ node attributes
	if(plug == inputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	}
	//else if(plug == idObj || plug == attributeNameObj)
	//{
	//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	//}
	else
	{
		LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
	}
	return stat;
}

MStatus FetchAttributeMPxNode::compute(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(RegisteredNodes->FetchAttributeMPxNode.logging);
	MStatus stat = MS::kUnknownParameter;
	
	if(plug             == outAttributeObj   ||
	   plug.parent()    == outAttributeObj   ||
	   plug             == outPidObj         ||
	   plug             == outSidObj         ||
	   plug             == outRenderableObj  ||
       plug             == outPositionObj    ||
	   plug.parent()    == outPositionObj    ||
       plug             == outEulerAnglesObj ||
	   plug.parent()    == outEulerAnglesObj ||
       plug             == outScaleObj       ||
	   plug.parent()    == outScaleObj
	)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		MDataHandle inputHnd     = datablock.inputValue(inputObj);
		MDataHandle inPointsHnd = inputHnd.child(TaneMPxShape::inPointsObj);
		
		PointsMPxData* pPointsMPxData = dynamic_cast<PointsMPxData*>(inPointsHnd.asPluginData());
		if(!pPointsMPxData)
		{
			LOGGER_ERROR("Failed to get pPointsMPxData");
			return MS::kFailure;
		}
		std::shared_ptr<Points> pPoints = pPointsMPxData->data();
		if(!pPoints)
		{
			LOGGER_ERROR("Failed to get pPoints");
			return MS::kFailure;
		}
		
		if(!(pPoints->size() > 0))
		{
			LOGGER_ERROR("Failed to get valid pPoints");
			return MS::kFailure;
		}
		const int npoints = pPoints->size();
		const int targetId = datablock.inputValue(idObj).asInt();
		if(!(targetId < npoints))
		{
			MString name = GetAbsoluteName(thisMObject(), &stat);
			MString msg = "Input id is not valid, " + name;
			MGlobal::displayError(msg);
			LOGGER_ERROR("%s", msg.asChar());
			return MS::kFailure;
		}
		const Int32Array&       pidArray        = pPoints->get<int>(sementic::attribute::pid);
		const Int32Array&       sidArray        = pPoints->get<int>(sementic::attribute::sid);
		const Int32Array&       renderableArray = pPoints->get<int>(sementic::attribute::renderable);
		const Point3fArray&     pointArray      = pPoints->get();
		const Vec3fArray&       eulerAngleArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		const Vec3fArray&       scaleArray      = pPoints->get<Vec3f>(sementic::attribute::scale);
		const Mat3x3fArray&     localAxisArray  = pPoints->get<Mat3x3f>(sementic::attribute::localAxis);
		const TexCoord3fArray&  uvCoordArray    = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		const FloatArray&       radArray        = pPoints->get<float>(sementic::attribute::radius);
		
		MDataHandle outAttributeHnd = datablock.outputValue(outAttributeObj, &stat);
		MCHECKERROR(stat, "Failed to get outAttributeHnd");
		{
			MDataHandle outPidHnd = outAttributeHnd.child(FetchAttributeMPxNode::outPidObj);
			outPidHnd.set(static_cast<int>(pidArray[targetId]));
		}
		{
			MDataHandle outSidHnd = outAttributeHnd.child(FetchAttributeMPxNode::outSidObj);
			outSidHnd.set(static_cast<int>(sidArray[targetId]));
		}
		{
			MDataHandle outRenderableHnd = outAttributeHnd.child(FetchAttributeMPxNode::outRenderableObj);
			outRenderableHnd.set(static_cast<int>(renderableArray[targetId]));
		}
		{
			MDataHandle outPositionHnd = outAttributeHnd.child(FetchAttributeMPxNode::outPositionObj);
			outPositionHnd.set(static_cast<double>(pointArray[targetId][0]),
			                   static_cast<double>(pointArray[targetId][1]),
			                   static_cast<double>(pointArray[targetId][2]));
		}
		{
			const Vec3f& eulerAngle = eulerAngleArray[targetId];
			double rx = static_cast<double>(Rad2Deg(eulerAngle[0]));
			double ry = static_cast<double>(Rad2Deg(eulerAngle[1]));
			double rz = static_cast<double>(Rad2Deg(eulerAngle[2]));
			MDataHandle outEulerAnglesHnd = outAttributeHnd.child(FetchAttributeMPxNode::outEulerAnglesObj);
			outEulerAnglesHnd.set(rx, ry, rz);
		}
		{
			MDataHandle outScaleHnd = outAttributeHnd.child(FetchAttributeMPxNode::outScaleObj);
			outScaleHnd.set(static_cast<double>(scaleArray[targetId][0]),
			                static_cast<double>(scaleArray[targetId][1]),
			                static_cast<double>(scaleArray[targetId][2]));
		}
		datablock.setClean(plug);
	}
	else
	{
		LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
		stat = MS::kUnknownParameter;
	}
	return stat;
}
///------------------------------------------------------------ public helper functions
///------------------------------------------------------------ private helper functions


NAMESPACE_BASE_END