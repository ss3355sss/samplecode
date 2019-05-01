#include "bindMPxNode.h"

#include "base/mathUtil.h"
#include "base/fileUtil.h"
#include "base/random.h"

#include "baseUsd/usdImporter.h"

#include "baseMaya/mayaMeshExtractor.h"
#include "baseMaya/util.h"
#include "baseMaya/zelosUtil.h"

#include <thrust/execution_policy.h>
#include <thrust/for_each.h>
#include <thrust/random.h>
#include <thrust/system/omp/execution_policy.h>


NAMESPACE_BASE_BEGIN
///------------------------------------------------- DistributorNode
MTypeId BindMPxNode::id(RegisteredNodes->BindMPxNode.id);
MString BindMPxNode::nodeName(RegisteredNodes->BindMPxNode.name);

///------------------------------------------------------------ node attribute
///------------------------------------------------- import cache
MObject BindMPxNode::calcRestStateObj;
MObject BindMPxNode::restBodyObj;
MObject BindMPxNode::restPositionArrayObj;
MObject BindMPxNode::restTexCoordArrayObj;
MObject BindMPxNode::restTriangleIndexArrayObj;

MObject BindMPxNode::inTimeObj;
MObject BindMPxNode::fileNameObj;

MObject BindMPxNode::sidSeedObj;
MObject BindMPxNode::sidRatioArrayObj;


///------------------------------------------------------------ input attributes
IMPL_IN_BODY_ATTRIBUTE(BindMPxNode);
IMPL_IN_SOURCE_ATTRIBUTE(BindMPxNode);

///------------------------------------------------------------ output attributes
IMPL_OUTPUT_ATTRIBUTE(BindMPxNode);

///------------------------------------------------------------ internal attributes
IMPL_MISCELLANEOUS_ATTRIBUTE(BindMPxNode);
///------------------------------------------------------------ static functions
void* BindMPxNode::Creator()
{
	return new BindMPxNode;
}
MStatus BindMPxNode::Initialize()
{
	MStatus              stat;
	MFnUnitAttribute     uAttr;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnCompoundAttribute cAttr;
	MFnMatrixAttribute   mAttr;
	MFnEnumAttribute     eAttr;
	
	///------------------------------------------------------------ node attribute
	calcRestStateObj = nAttr.create("calcRestState", "calcRestState", MFnNumericData::kBoolean, true, &stat);
	MCHECKERROR(stat, "Failed to create calcRestState attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                     /* Writable              */
	                     true,                     /* Connectable           */
	                     true,                     /* Stroable              */
	                     true,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     true,                     /* Keyable               */
	                     true,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(calcRestStateObj);
	///------------------------------------------------------------ input attributes
	restBodyObj = tAttr.create("restBody", "restBody", MFnData::kAny, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create inBody attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
						 false,                     /* Readable              */
						 true ,                     /* Writable              */
						 true ,                     /* Connectable           */
						 true ,                     /* Stroable              */
						 true ,                     /* Cached                */
						 false,                     /* ChannelBox            */
						 false,                     /* Array                 */
						 false,                     /* Use Array DataBuilder */
						 false,                     /* Keyable               */
						 true,                     /* Hidden                */
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */
						 false                      /* Affect Appearance     */
						 );
	ADD_ATTRIBUTE(restBodyObj);
	restPositionArrayObj = tAttr.create("restPositionArray", "restPositionArray", MFnData::kPointArray, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create restPositionArray attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
	                     false,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     true,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(restPositionArrayObj);
	restTexCoordArrayObj = tAttr.create("restTexCoordArray", "restTexCoordArray", MFnData::kPointArray, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create restTexCoordArray attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
	                     false,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     true,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(restTexCoordArrayObj);
	restTriangleIndexArrayObj = tAttr.create("restTriangleIndexArray", "restTriangleIndexArray", MFnData::kIntArray, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create restTriangleIndexArray attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
	                     false,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     true,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	
	ADD_ATTRIBUTE(restTriangleIndexArrayObj);
	fileNameObj = tAttr.create("fileName", "fileName", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create fileName attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
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
	ADD_ATTRIBUTE(fileNameObj);
	sidSeedObj = nAttr.create("sidSeed", "sidSeed", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create npoints attribute");
	nAttr.setMin(0);
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
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
	ADD_ATTRIBUTE(sidSeedObj);
	sidRatioArrayObj = nAttr.create("sidRatioArray", "sidRatioArray", MFnNumericData::kFloat, 100.0f, &stat);
	MCHECKERROR(stat, "Failed to create sidRatioArray attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                     /* Writable              */
	                     true,                     /* Connectable           */
	                     true,                     /* Stroable              */
	                     true,                     /* Cached                */
	                     true,                     /* ChannelBox            */
	                     true,                     /* Array                 */
	                     true,                     /* Use Array DataBuilder */
	                     false,                    /* Keyable               */
	                     false,                    /* Hidden                */
	                     MFnAttribute::kDelete,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	nAttr.setMin(0.0f);
	nAttr.setMax(100.0f);
	ADD_ATTRIBUTE(sidRatioArrayObj);
	
	///------------------------------------------------------------ input attributes
	inTimeObj = uAttr.create("inTime", "inTime", MFnUnitAttribute::kTime, 1.0);
	MCHECKERROR(stat, "Failed to create inTime attribute");
	SET_ATTRIBUTE_STATUS(uAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     true,                      /* Connectable           */
	                     false,                    /* Stroable              */
	                     true,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(inTimeObj);
	
	
	
	///------------------------------------------------- input attributes
	INITIALIZE_IN_BODY_ATTRIBUTE;
	INITIALIZE_IN_SOURCE_ATTRIBUTE;
	
	///------------------------------------------------- output attributes
	INITIALIZE_OUTPUT_ATTRIBUTE;
	
	///------------------------------------------------- set input/output attribute affects
	ATTRIBUTE_AFFECTS(calcRestStateObj,   outputObj);
	ATTRIBUTE_AFFECTS(restBodyObj,        outputObj);
	
	ATTRIBUTE_AFFECTS(fileNameObj,        outputObj);
	
	ATTRIBUTE_AFFECTS(sidSeedObj,         outputObj);
	ATTRIBUTE_AFFECTS(sidRatioArrayObj,   outputObj);
	
	ATTRIBUTE_AFFECTS(inBodyObj,   outputObj);
	ATTRIBUTE_AFFECTS(inSourceObj, outputObj);
	
	return MS::kSuccess;
}

///------------------------------------------------------------ constructor / destructor
BindMPxNode::BindMPxNode()
{
}
BindMPxNode::~BindMPxNode()
{
}

///------------------------------------------------------------ virtual functions
void BindMPxNode::postConstructor()
{
	DEFAULT_DG_POST_CONSTRUCTOR(RegisteredNodes->BindMPxNode.name);
	
	pMsgQueue_ = std::make_shared<MessageQueue>();
	pMsgQueue_->push(kSOURCE_ATTRIBUTE_CHANGED);
	pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	
}
MStatus BindMPxNode::setDependentsDirty(const MPlug& plug, MPlugArray& affectedPlugs)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	
	MStatus stat = MS::kSuccess;
	
	///------------------------------------------------------------ node attributes
	if(plug == calcRestStateObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kRESTSTATE_ATTRIBUTE_CHANGED);
	}
	else if(plug == sidSeedObj ||
	        plug == sidRatioArrayObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
	else if(plug == inBodyObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kBODY_ATTRIBUTE_CHANGED);
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
	else if(plug == inSourceObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kSOURCE_ATTRIBUTE_CHANGED);
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
	else
	{
		LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
	}
	return stat;
}

MStatus BindMPxNode::compute(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	MStatus stat = MS::kUnknownParameter;
	if(plug == outputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		MessageQueue msgQueue = pMsgQueue_->clone();
		while (!msgQueue.empty())
		{
			LOGGER_INFO("msg = %d", msgQueue.top());
			switch (msgQueue.top())
			{
				case kRESTSTATE_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kRESTSTATE_ATTRIBUTE_CHANGED");
					stat = computeRestState_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute restState");
					break;
					
				case kBODY_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kBODY_ATTRIBUTE_CHANGED");
					stat = computeOutBody_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute compute OutBody");
				
				case kSOURCE_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kSOURCE_ATTRIBUTE_CHANGED");
					stat = computeOutSources_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute outSource");
					break;
				case kPOINT_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kPOINT_ATTRIBUTE_CHANGED");
					stat = computeOutPoints_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute outPoints");
					break;
			}
			msgQueue.pop();
		}
		stat = computeOutMessage_(plug, datablock);
		MCHECKERROR(stat, "computeOutMessage_(plug, datablock) is failed");
		
		datablock.setClean(plug);
	}
	else if(plug == inBodyObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
		stat = computeOutPoints_(plug, datablock);
		MCHECKERROR(stat, "Failed to compute outPoints");
		
		stat = computeOutMessage_(plug, datablock);
		MCHECKERROR(stat, "computeOutMessage_(plug, datablock) is failed");
		
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
///-------------------------------------- compute restState
void CalcLocalAxis(const Zelos::ZPoint& p0, const Zelos::ZPoint& p1, const Zelos::ZPoint& p2,
                   Zelos::ZVector* target,
                   Zelos::ZVector* up,
                   Zelos::ZVector* right)
{
	Zelos::ZVector p0p1 = p1 - p0;
	Zelos::ZVector p0p2 = p2 - p0;
	
	*target = Zelos::ZVector(0.0f, 0.0f, 1.0f).normalized();  //front vector(z-axis)
	*up     = (p0p1^ p0p2).normalized();
	*right  = (*up^ *target).normalized();
	*target = (*right^ *up).normalized();
}

MStatus BindMPxNode::computeRestState_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	MStatus stat = MS::kSuccess;
	
	/* Set Currest InBody Data to RestBody Data*/
	MDataHandle restBodyHnd = datablock.outputValue(restBodyObj, &stat);
	MCHECKERROR(stat, "Failed to get restBodyHnd");
	{
		MDataHandle inBodyHnd = datablock.inputValue(inBodyObj, &stat);
		MCHECKERROR(stat, "Failed to get inBodyHnd");
		
		restBodyHnd.set(inBodyHnd.data());
		MCHECKERROR(stat, "Failed to set restBodyHnd");
	}
	
	/* reset restState data */
	pRestState_.reset();
	
	return stat;
}
///-------------------------------------- compute outBody
MStatus BindMPxNode::computeOutBody_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat = MS::kSuccess;
	
	MPlug inBodyPlug(thisMObject(), inBodyObj);
	if(inBodyPlug.isConnected())
	{
		MFnData::Type typedDataType = MFnData::kInvalid;
		
		MObject obj  = datablock.inputValue(inBodyObj).data();
		if(obj.apiType() == MFn::kMeshData)
		{
			LOGGER_INFO("MFn::kMeshData");
		}
		else if(obj.apiType()== MFn::kNurbsCurveData)
		{
			LOGGER_INFO("MFn::kNurbsCurveData");
		}
		else if(obj.apiType() == MFn::kBezierCurveData)
		{
			LOGGER_INFO("MFn::kBezierCurveData");
		}
		else if(obj.apiType() == MFn::kNurbsSurfaceData)
		{
			LOGGER_INFO("MFn::kNurbsSurfaceData");
		}
		else
		{
			MString msg = "Failed to get supporting inBody node";
			MGlobal::displayError(msg);
			LOGGER_ERROR("%s", msg.asChar());
			return MS::kFailure;
		}
	}
	else
	{
		MString msg = "inBodyPlug is not connected";
		MGlobal::displayError(msg);
		
		stat = MS::kFailure;
	}
	return stat;
}

///-------------------------------------- compute outSources
MStatus BindMPxNode::computeOutSources_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	
	MStatus stat = MS::kSuccess;
	
	///------------------------------------------- create new SourcesMPxData
	MFnPluginData pluginDataFn;
	pluginDataFn.create(MTypeId(SourcesMPxData::id), &stat);                                        MCHECKERROR(stat, "Failed to create pSourcesMPxData");
	SourcesMPxData* pSourcesMPxData = dynamic_cast<SourcesMPxData*>(pluginDataFn.data(&stat));      MCHECKERROR(stat, "Failed to get pSourcesMPxData");
	
	std::shared_ptr<SourceManager> pSourceMgr = pSourcesMPxData->data();
	///------------------------------------------- get inSourceMPxData
	MArrayDataHandle inSourceArrayHnd = datablock.inputArrayValue(inSourceObj, &stat);
	MCHECKERROR(stat, "Failed to get inSourceArrayHnd");

	unsigned int elemCount = inSourceArrayHnd.elementCount(&stat);
	MCHECKERROR(stat, "Failed to get inSourceArrayHnd's elementCount");
	
	MArrayDataHandle sidRatioArrayHnd = datablock.outputArrayValue(sidRatioArrayObj, &stat);
	MCHECKERROR(stat, "Failed to get sidRatioArrayHnd ");
	
	MArrayDataBuilder sidRatioBuilder = sidRatioArrayHnd.builder(&stat);
	MCHECKERROR(stat, "Failed to create sidRatioBuilder");
	
	for (unsigned int i = 0; i < elemCount; ++i)
	{
		unsigned int elemIndex = inSourceArrayHnd.elementIndex(&stat);
		MCHECKERROR(stat, "Failed to get inSourceArrayHnd's elementIndex");
		
		LOGGER_DEBUG("elemIndex = %u", elemIndex);
		
		MDataHandle inSorceHnd = inSourceArrayHnd.inputValue(&stat);
		MCHECKERROR(stat, "Failed to get inSorceHnd");
		
		SourceMPxData* pInSourceMPxData = dynamic_cast<SourceMPxData*>(inSorceHnd.asPluginData());
		if(!pInSourceMPxData)
		{
			STDOUT_ERROR("Failed to get pInSourceMPxData");
			return MS::kFailure;
		}
		
		std::shared_ptr<Source> pInSource = pInSourceMPxData->data();
		if(!pInSource)
		{
			STDOUT_ERROR("Failed to get pInSource");
			return MS::kFailure;
		}
		
		bool insertOk = pSourceMgr->insert(SourceManager::value_type(elemIndex, pInSource)).second;
		if(!insertOk)
		{
			STDOUT_ERROR("Failed to insert source, %d", elemIndex);
			return MS::kFailure;
		}
		
		/*double& ratio = */ sidRatioBuilder.addElement(elemIndex).asDouble();
		inSourceArrayHnd.next();
	}
	stat = sidRatioArrayHnd.set(sidRatioBuilder);                               MCHECKERROR(stat, "Failed to set sidRatioBuilder to sidRatioArrayHnd");
	stat = sidRatioArrayHnd.setAllClean();                                      MCHECKERROR(stat, "Failed to sidRatioArrayHnd.setAllClean()");
	
	///------------------------------------------- set new SourcesMPxData to outSourcesHnd
	MDataHandle outSourcesHnd = datablock.outputValue(outSourcesObj, &stat);    MCHECKERROR(stat, "Failed to get outSourcesHnd");
	stat = outSourcesHnd.set(pSourcesMPxData);                                  MCHECKERROR(stat, "Failed to set outSourcesHnd");
	
	return stat;
}
///-------------------------------------- compute outPoints
MStatus BindMPxNode::computeOutPoints_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	
	MStatus stat;
	std::shared_ptr<RestState> pRestState = getRestState_(plug, datablock);
	if(pRestState == nullptr)
	{
		STDOUT_ERROR("Failed to create pRestState");
		return MS::kFailure;
	}
	
	std::shared_ptr<Points>          pRestPoints           = pRestState->pPoints;
	std::shared_ptr<Vec3fArray>      pInitialError         = pRestState->pInitialError;
	std::shared_ptr<IntArray>        pTriangleIdArrays     = pRestState->pTriangleIdArrays;
	std::shared_ptr<Triangle3fArray> pTriangleArray        = pRestState->pTriangleArray;
	std::shared_ptr<Mat3x3fArray>    pInvLocalAxisMtxArray = pRestState->pInvLocalAxisMtxArray;
	std::shared_ptr<Float3Array>     pBccArray             = pRestState->pBccArray;
	
	const int npoints = pRestPoints->size();
	std::shared_ptr<Points> pOutPoints = Points::Create(npoints);
	*pOutPoints = *pRestPoints; 	/* copy data */
	
	BoolArray isDeleted = BoolArray(npoints, false);
	///------------------------------------------ bind to current inBody state
	{
		const Int32Array     & pidArray0        = pRestPoints->get<int>(sementic::attribute::pid);
		const Int32Array     & sidArray0        = pRestPoints->get<int>(sementic::attribute::sid);
		const Int32Array     & renderableArray0 = pRestPoints->get<int>(sementic::attribute::renderable);
		const Point3fArray   & pointArray0      = pRestPoints->get();
		const Vec3fArray     & eulerAngleArray0 = pRestPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		const Vec3fArray     & scaleArray0      = pRestPoints->get<Vec3f>(sementic::attribute::scale);
		const Mat3x3fArray   & localAxisArray0  = pRestPoints->get<Mat3x3f>(sementic::attribute::localAxis);
		const TexCoord3fArray& uvCoordArray0    = pRestPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		const FloatArray     & radiusArray0     = pRestPoints->get<float>(sementic::attribute::radius);
	
		Int32Array           & pidArray1        = pOutPoints->get<int>(sementic::attribute::pid);
		Int32Array           & sidArray1        = pOutPoints->get<int>(sementic::attribute::sid);
		Int32Array           & renderableArray1 = pOutPoints->get<int>(sementic::attribute::renderable);
		Point3fArray         & pointArray1      = pOutPoints->get();
		Vec3fArray           & eulerAngleArray1 = pOutPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		Vec3fArray           & scaleArray1      = pOutPoints->get<Vec3f>(sementic::attribute::scale);
		Mat3x3fArray         & localAxisArray1  = pOutPoints->get<Mat3x3f>(sementic::attribute::localAxis);
		TexCoord3fArray      & uvCoordArray1    = pOutPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		FloatArray           & radiusArray1     = pOutPoints->get<float>(sementic::attribute::radius);
		
		MDataHandle inBodyHnd = datablock.inputValue(inBodyObj, &stat);
		MCHECKERROR(stat, "Failed to get inBodyHnd");
		
		MObject inBodyDataObj = inBodyHnd.asMesh();
		Zelos::ZTriMesh zInBodyTriMesh;
		LOGGER_IN("convert zInTriMesh");
		stat = Convert(zInBodyTriMesh, inBodyDataObj, false, "currentUVSet");
		LOGGER_OUT();
		MCHECKERROR(stat, "Failed to get valid ZTriMesh of inBody");
		
		LOGGER_IN("Calc current points");
		{
			bool robust = false;
			//MMeshIntersector intersector;
			//intersector.create(inBodyDataObj);
			thrust::for_each(thrust::system::omp::par,
			                 thrust::counting_iterator<size_t>(0),
			                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
			                 [&](const size_t idx)
			                 {
				                 const int& triangleId = (*pTriangleIdArrays)[idx];
		
				                 const float& w0 = (*pBccArray)[idx][0];
				                 const float& w1 = (*pBccArray)[idx][1];
				                 const float& w2 = (*pBccArray)[idx][2];
		
				                 const int& v0 = zInBodyTriMesh.v012[triangleId][0];
				                 const int& v1 = zInBodyTriMesh.v012[triangleId][1];
				                 const int& v2 = zInBodyTriMesh.v012[triangleId][2];
		
				                 const Zelos::ZPoint& p0 = zInBodyTriMesh.p[v0];
				                 const Zelos::ZPoint& p1 = zInBodyTriMesh.p[v1];
				                 const Zelos::ZPoint& p2 = zInBodyTriMesh.p[v2];
		
				                 // newly estimated pivot point (new origin)
				                 const Zelos::ZPoint O(WeightedSum(p0, p1, p2, w0, w1, w2));
				                 pointArray1[idx] = Point3f(O[0], O[1], O[2]) + (*pInitialError)[idx];

				                 //Gram-Schmidt orthonormalize
				                 Zelos::ZVector X, Y, Z;
				                 X = (zInBodyTriMesh.p[v0] - O ).robustNormalize();
				                 Z = RobustNormal( p0, p1, p2 );

				                 X = ( X - (Z*(Z*X)) ).robustNormalize();
				                 Y = ( Z ^ X ).robustNormalize();

				                 const Zelos::ZMatrix R(X, Y, Z); /* Orientation */
				                 const Zelos::ZVector localXAxis = R.column(0).normalized();
				                 const Zelos::ZVector localYAxis = R.column(1).normalized();
				                 const Zelos::ZVector localZAxis = R.column(2).normalized();

				                 localAxisArray1[idx].col(0) = Vec3f(localXAxis[0], localXAxis[1], localXAxis[2]);
				                 localAxisArray1[idx].col(1) = Vec3f(localYAxis[0], localYAxis[1], localYAxis[2]);
				                 localAxisArray1[idx].col(2) = Vec3f(localZAxis[0], localZAxis[1], localZAxis[2]);

				                 const Mat3x3f& invRot = (*pInvLocalAxisMtxArray)[idx];
				                 Zelos::ZMatrix zInvRot(invRot(0,0), invRot(0,1), invRot(0,2),
				                                        invRot(1,0), invRot(1,1), invRot(1,2),
				                                        invRot(2,0), invRot(2,1), invRot(2,2));
				                 float rx, ry, rz;
				                 const Zelos::ZMatrix RM(R * zInvRot);
				                 RM.getRotation(rx, ry, rz, false /*asDegrees*/);
				                 eulerAngleArray1[idx] = eulerAngleArray0[idx] + Vec3f(rx, ry, rz);
			                 });
		}
		LOGGER_OUT();
	
	}
	
	if(pOutPoints == nullptr)
	{
		STDOUT_ERROR("Failed to create outPoints");
		return MS::kFailure;
	}

	///------------------------------------------ delete point by mask
	//pOutPoints->remove(isDeleted, Points::kDELETE_MASK);

	///// ----------------------------------------- apply source id, physical id  */
	//stat = applySourceIDRatio_(datablock, pOutPoints);
	//MCHECKERROR(stat, "Failed to apply source id ratio");

	///------------------------------------------ create new PointsMpxData
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(PointsMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create PointsMpxData");

	PointsMPxData* newPointsMpxData = dynamic_cast<PointsMPxData*>(fnDataCreator.data(&stat));
	MCHECKERROR(stat, "Failed to getPointsMpxData");

	newPointsMpxData->data() = pOutPoints;

	MDataHandle outPointsHandle = datablock.outputValue(outPointsObj);
	outPointsHandle.set(newPointsMpxData);
	return stat;
}
///-------------------------------------- compute outMessage
MStatus BindMPxNode::computeOutMessage_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	
	MStatus       stat;
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(MessageMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create MessageMPxData");
	
	MessageMPxData* pMPxData = dynamic_cast<MessageMPxData*>(fnDataCreator.data(&stat));
	MCHECKERROR(stat, "Failed to get MessageMPxData");
	
	std::shared_ptr<Message> pMessage = pMPxData->data();
	pMessage->templet = templetType::bind;
	
	while (!pMsgQueue_->empty())
	{
		switch (pMsgQueue_->top())
		{
			case kRESTSTATE_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kRESTSTATE_ATTRIBUTE_CHANGED");
				pMessage->msgQueue.push(Message::kBODY_DIRTY);
				break;
			case kBODY_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kBODY_CHANGED");
				pMessage->msgQueue.push(Message::kBODY_DIRTY);
				
			case kSOURCE_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kSOURCE_ATTRIBUTE_CHANGED");
				pMessage->msgQueue.push(Message::kSOURCE_DIRTY);
				break;
			case kPOINT_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kPOINT_ATTRIBUTE_CHANGED");
				pMessage->msgQueue.push(Message::kPOINT_DIRTY);
				break;
		}
		pMsgQueue_->pop();
	}
	MDataHandle outMessageHnd = datablock.outputValue(outMessageObj, &stat);
	outMessageHnd.set(pMPxData);
	return stat;
}

std::shared_ptr<BindMPxNode::RestState> BindMPxNode::getRestState_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	MStatus stat;
	
	/* get restBody data */
	MDataHandle restBodyHnd = datablock.outputValue(restBodyObj, &stat);
	if(stat != MS::kSuccess)
	{
		LOGGER_ERROR("Failed to get restBodyHnd");
		return nullptr;
	}
	
	if(!pRestState_)
	{
		LOGGER_INFO("Initialize Rest State");
		MObject restBodyDataObj = restBodyHnd.asMesh();
		Zelos::ZTriMesh zTriMesh;
		stat = Convert(zTriMesh, restBodyDataObj, false);
		if(stat != MS::kSuccess)
		{
			LOGGER_ERROR("Failed to get valid ZTriMesh of restBody");
			return nullptr;
		}
		
		/* import point data from usd(PointInstancer Schema)*/
		const MTime  & inTime   = datablock.inputValue(inTimeObj, &stat).asTime();
		const MString& fileName = datablock.inputValue(fileNameObj, &stat).asString();
		UsdGeomPointInstancerImporter importer;
		if (!importer.open(fileName.asChar()))
		{
			LOGGER_ERROR("Failed to open usd file, %s", fileName.asChar());
			return nullptr;
		}
		
		std::shared_ptr<Points> pPoints = importer.getPoints(mode::animCycle::none, UsdTimeCode(inTime.value()));
		
		const int npoints = pPoints->size();
		std::shared_ptr<Vec3fArray>      pInitialError         = std::make_shared<Vec3fArray>     (npoints);
		std::shared_ptr<IntArray>        pTriangleIdArrays     = std::make_shared<IntArray>       (npoints);
		std::shared_ptr<Triangle3fArray> pTriangleArray        = std::make_shared<Triangle3fArray>(npoints);
		std::shared_ptr<Mat3x3fArray>    pInvLocalAxisMtxArray = std::make_shared<Mat3x3fArray>   (npoints);
		std::shared_ptr<Float3Array>     pBccArray             = std::make_shared<Float3Array>    (npoints);
		
		Int32Array     & pidArray        = pPoints->get<int>(sementic::attribute::pid);
		Int32Array     & sidArray        = pPoints->get<int>(sementic::attribute::sid);
		Int32Array     & renderableArray = pPoints->get<int>(sementic::attribute::renderable);
		Point3fArray   & pointArray      = pPoints->get();
		Vec3fArray     & eulerAngleArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		Vec3fArray     & scaleArray      = pPoints->get<Vec3f>(sementic::attribute::scale);
		Mat3x3fArray   & localAxisArray  = pPoints->get<Mat3x3f>(sementic::attribute::localAxis);
		TexCoord3fArray& uvCoordArray    = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		FloatArray     & radiusArray     = pPoints->get<float>(sementic::attribute::radius);
		
		/* Calc RestState Data */
		Zelos::ZTriMeshDistTree zTriMeshDistTree;
		if (!zTriMeshDistTree.set(zTriMesh))
		{
			LOGGER_ERROR("Failed to set zTriMesh to zTriMeshDistTree");
			return nullptr;
		}
		
		LOGGER_IN("calc rest state");
		{
			thrust::for_each(thrust::system::omp::par,
			                 thrust::counting_iterator<size_t>(0),
			                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
			                 [&](const size_t idx)
			                 {
				                 const Zelos::ZPoint& zInPoint = Zelos::ZPoint(pointArray[idx][0], pointArray[idx][1], pointArray[idx][2]);
				                 //LOGGER_DEBUG("p[%d] = (%5.2f, %5.2f, %5.2f)", idx, p[0], p[1], p[2]);
				                 
				                 Zelos::ZPoint closestPoint;
				                 int           closestTriangleId;
				                 Float3        bcc;
				                 zTriMeshDistTree.getClosestPoint(zInPoint,
				                                                  closestPoint,
				                                                  closestTriangleId,
				                                                  bcc[0],
				                                                  bcc[1],
				                                                  Zelos::Z_LARGE);
				                 bcc[2] = 1.0f - bcc[0] - bcc[1];
				                 
				                 const int& triangleId = closestTriangleId;
				
				                 const float& w0 = bcc[0];
				                 const float& w1 = bcc[1];
				                 const float& w2 = bcc[2];
				
				                 const int& v0 = zTriMesh.v012[closestTriangleId][0];
				                 const int& v1 = zTriMesh.v012[closestTriangleId][1];
				                 const int& v2 = zTriMesh.v012[closestTriangleId][2];
				
				                 const Zelos::ZPoint& p0 = zTriMesh.p[v0];
				                 const Zelos::ZPoint& p1 = zTriMesh.p[v1];
				                 const Zelos::ZPoint& p2 = zTriMesh.p[v2];
				                 
				                 //LOGGER_DEBUG("closestPoint[%d] = (%5.2f, %5.2f, %5.2f)", idx, closestPoint[0], closestPoint[1], closestPoint[2]);
				
				                 // newly estimated pivot point (new origin)
				                 Zelos::ZPoint  O(WeightedSum(p0, p1, p2, w0, w1, w2));
				                 //Zelos::ZPoint  O = Zelos::ZPoint(p[0], p[1], p[2]);
				                 
				                 //Gram-Schmidt orthonormalize
				                 Zelos::ZVector X, Y, Z;
				                 X = (zTriMesh.p[v0] - O).robustNormalize();
				                 Z = RobustNormal(p0, p1, p2);
					             X = (X - (Z * (Z * X))).robustNormalize();
					             Y = (Z ^ X).robustNormalize();
				
				                 const Zelos::ZMatrix R(X, Y, Z);          /* Orientation */
				
				                 const Zelos::ZVector localXAxis = R.column(0).normalized();
				                 const Zelos::ZVector localYAxis = R.column(1).normalized();
				                 const Zelos::ZVector localZAxis = R.column(2).normalized();
				
				                 localAxisArray[idx].col(0) = Vec3f(localXAxis[0], localXAxis[1], localXAxis[2]);
				                 localAxisArray[idx].col(1) = Vec3f(localYAxis[0], localYAxis[1], localYAxis[2]);
				                 localAxisArray[idx].col(2) = Vec3f(localZAxis[0], localZAxis[1], localZAxis[2]);
				
				                 const Zelos::ZMatrix invR = R.inversed();
				
				                 const Zelos::ZVector invLocalXAxis = invR.column(0).normalized();
				                 const Zelos::ZVector invLocalYAxis = invR.column(1).normalized();
				                 const Zelos::ZVector invLocalZAxis = invR.column(2).normalized();
				                 
				                 /* set initial error */
				                 {
				                 	Zelos::ZVector e(zInPoint - O);
				                 	//LOGGER_DEBUG("e[%d] = (%5.2f, %5.2f, %5.2f)", idx, e[0], e[1], e[2]);
					                (*pInitialError)[idx] = Vec3f(e[0], e[1], e[2]);
				                 }
				                 /* set triangle id*/
				                 {
					                 (*pTriangleIdArrays)[idx] = closestTriangleId;
				                 }
				                 /* set triangle */
				                 {
				                 	Triangle3f triangle;
				                 	
				                 }
				                 /* set inversed localAxisMtx  */
				                 {
					                 (*pInvLocalAxisMtxArray)[idx].col(0) = Vec3f(invLocalXAxis[0], invLocalXAxis[1], invLocalXAxis[2]);
					                 (*pInvLocalAxisMtxArray)[idx].col(1) = Vec3f(invLocalYAxis[0], invLocalYAxis[1], invLocalYAxis[2]);
					                 (*pInvLocalAxisMtxArray)[idx].col(2) = Vec3f(invLocalZAxis[0], invLocalZAxis[1], invLocalZAxis[2]);
				                 }
				                 /* set bcc  */
				                 {
					                 (*pBccArray)[idx] = bcc;
				                 }
			                 });
		}
		LOGGER_OUT();
		pRestState_ = std::make_shared<RestState>(RestState{
						pPoints,
				        pInitialError,
				        pTriangleIdArrays,
				        pTriangleArray,
						pInvLocalAxisMtxArray,
				        pBccArray
				});
	}
	
	if(!pRestState_)
	{
		LOGGER_ERROR("Failed to get pRestPoints from UsdImporter");
		return nullptr;
	}
	return pRestState_;
}


void BindMPxNode::calcLocalAxis_(const Point3f& p0, const Point3f& p1, const Point3f& p2, Vec3f* target, Vec3f* up, Vec3f* right)
{
	Vec3f p0p1 = p1 - p0;
	Vec3f p0p2 = p2 - p0;
	
	*target = Vec3f(0.0f, 0.0f, 1.0f).normalized();  //front vector(z-axis)
	*up     = cross(p0p1, p0p2).normalized();
	*right  = cross(*up, *target).normalized();
	*target = cross(*right, *up).normalized();
}
void BindMPxNode::rotateLocalAxis_(const float radx, const float rady, const float radz, Vec3f* target, Vec3f* up, Vec3f* right)
{
	//---------------- local x axis
	*up     = RotateByAxis(*up, *right, radx).normalized();
	*target = RotateByAxis(*target, *right, radx).normalized();
	//---------------- local y axis
	*right  = RotateByAxis(*right, *up, rady).normalized();
	*target = RotateByAxis(*target, *up, rady).normalized();
	//---------------- local z axis
	*right  = RotateByAxis(*right, *target, radz).normalized();
	*up     = RotateByAxis(*up, *target, radz).normalized();
	
}

MStatus BindMPxNode::applySourceIDRatio_(MDataBlock& datablock, const std::shared_ptr<Points> pOutPoints)
{
	DECL_LOGGER(logging::maya->BindMPxNode);
	MStatus stat;
	
	MArrayDataHandle inSourceArrayHnd = datablock.inputArrayValue(inSourceObj, &stat);
	MCHECKERROR(stat, "Failed to get inSourceArrayHnd");
	
	unsigned int elemCount = inSourceArrayHnd.elementCount(&stat);
	MCHECKERROR(stat, "Failed to get inSourceArrayHnd's elementCount");
	
	if(elemCount > 0)
	{
		MArrayDataHandle sidRatioArrayHnd = datablock.outputArrayValue(sidRatioArrayObj, &stat);
		MCHECKERROR(stat, "Failed to create sidRatioArrayHnd");
		
		MArrayDataBuilder sidBuilder = sidRatioArrayHnd.builder(&stat);
		MPlug             sidRatioPlug(thisMObject(), sidRatioArrayObj);
		
		float                     sum = 0.0f;
		std::vector<unsigned int> sourceIDs(elemCount);
		std::vector<float>        ratios(elemCount);
		for (int i = 0; i < elemCount; ++i)
		{
			unsigned int elemIndex = inSourceArrayHnd.elementIndex(&stat);
			MCHECKERROR(stat, "Failed to get inSourceArrayHnd's elementIndex");
			LOGGER_DEBUG("elemIndex = %u", elemIndex);
			
			MPlug ratioPlug = sidRatioPlug.elementByLogicalIndex(elemIndex, &stat);
			float ratio     = static_cast<float>(ratioPlug.asDouble() * 0.01);
			LOGGER_DEBUG("ratio[%u] = %5.2f", elemIndex, ratio);
			
			sourceIDs[i] = elemIndex;
			ratios[i] = ratio;
			sum += ratio;
			
			inSourceArrayHnd.next();
		}
		
		for (int i = 0; i < elemCount; ++i)
		{
			ratios[i] /= sum;
			//logger.message("ratios[%d] = %5.2f", i, ratios[i]);
		}
		
		
		
		const int& npoints      = pOutPoints->size();
		
		IntArray& pidArray = pOutPoints->get<int>(sementic::attribute::pid);
		IntArray& sidArray = pOutPoints->get<int>(sementic::attribute::sid);
		
		const int& seed = datablock.inputValue(sidSeedObj, &stat).asInt();
		MCHECKERROR(stat, "Failed to get seed attribute");
		
		thrust::for_each(thrust::omp::par,
		                 thrust::counting_iterator<size_t>(0),
		                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
		                 [&](const size_t idx)
		                 {
			
			                 ///-------------------------------------------- discard random variable
			                 thrust::default_random_engine seedRng(seed);
			                 thrust::uniform_int_distribution<int> randomSeed(0, std::numeric_limits<int>::max());
			                 seedRng.discard(idx);
			
			                 ///-------------------------------------------- set pid
			                 std::mt19937                 randSourceIDsRng(randomSeed(seedRng));
			                 std::discrete_distribution<> sourceIDsDist(ratios.begin(), ratios.end());
			                 randSourceIDsRng.discard(idx);
			
			                 pidArray[idx] = idx;
			
			                 ///-------------------------------------------- set sid
			                 if (elemCount > 0)
				                 sidArray[idx] = sourceIDs[sourceIDsDist(randSourceIDsRng)];
			                 else
				                 sidArray[idx] = 0;
		                 });
	}
	
	
	return stat;
}


bool BindMPxNode::getIndices_(const int polyId, const MFnMesh& meshFn, int* ntriangles, MIntArray* objectRelativeID, MIntArray* faceRelativeID)
{
	MStatus stat;
	
	MIntArray vertexList;
	stat = meshFn.getPolygonVertices(polyId, vertexList);
	if(stat != MS::kSuccess)
	{
		return false;
	}
	
	const int vtxCnt    = vertexList.length();
	const int arraySize = (vtxCnt - 2) * 3;
	*objectRelativeID = MIntArray(arraySize);
	*faceRelativeID   = MIntArray(arraySize);
	
	int      index = 0;
	for (int i     = 0; i < vtxCnt - 2; ++i)
	{
		(*objectRelativeID)[index + 0] = vertexList[vertexList.length() - 1];
		(*objectRelativeID)[index + 1] = vertexList[i];
		(*objectRelativeID)[index + 2] = vertexList[i + 1];
		
		(*faceRelativeID)[index + 0] = vertexList.length() - 1;
		(*faceRelativeID)[index + 1] = i;
		(*faceRelativeID)[index + 2] = i + 1;
		
		index += 3;
	}
	*ntriangles = arraySize / 3;
	return true;
}

NAMESPACE_BASE_END