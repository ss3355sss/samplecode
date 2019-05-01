#pragma once
#include "maya/pch.h"

#include "base/hackedQueue.h"
#include "base/messageQueue.h"
#include "base/triangleMesh.h"

#include "maya/messageMPxData.h"
#include "maya/pointsMPxData.h"
#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

///------------------------------------------------- BindMPxNode
NAMESPACE_BASE_BEGIN
class BindMPxNode : public MPxNode
{
public:
	static MTypeId id;
	static MString nodeName;
public:
	///------------------------------------------------------------ node attributes
	static MObject calcRestStateObj;
	static MObject restBodyObj;
	static MObject restPositionArrayObj;
	static MObject restTexCoordArrayObj;
	static MObject restTriangleIndexArrayObj;
	
	static MObject inTimeObj;
	static MObject fileNameObj;
	
	static MObject sidSeedObj;
	static MObject sidRatioArrayObj;
	

	///------------------------------------------------------------ input attributes
	DECL_IN_BODY_ATTRIBUTE;
	DECL_IN_SOURCE_ATTRIBUTE;
	
	///------------------------------------------------------------ output attributes
	DECL_OUTPUT_ATTRIBUTE;
	///------------------------------------------------------------ internal attributes
	DECL_MISCELLANEOUS_ATTRIBUTE;

private:
	enum State
	{
		kRESTSTATE_ATTRIBUTE_CHANGED,
		kBODY_ATTRIBUTE_CHANGED,
		kSOURCE_ATTRIBUTE_CHANGED,
		kPOINT_ATTRIBUTE_CHANGED,
	};
	typedef MessageQueueProxy<State> MessageQueue;
	std::shared_ptr<MessageQueue> pMsgQueue_;
private:
	typedef Array1<Triangle3f> Triangle3fArray;
	struct RestState final
	{
		std::shared_ptr<Points>          pPoints;
		std::shared_ptr<Vec3fArray>      pInitialError;
		std::shared_ptr<IntArray>        pTriangleIdArrays;
		std::shared_ptr<Triangle3fArray> pTriangleArray;
		std::shared_ptr<Mat3x3fArray>    pInvLocalAxisMtxArray;
		std::shared_ptr<Float3Array>     pBccArray;
	};
	std::shared_ptr<RestState> pRestState_;
public:
	///------------------------------------------------------------ static functions
	static void* 	Creator();
	static MStatus 	Initialize();
public:
	///------------------------------------------------------------ constructor / destructor
	BindMPxNode();
	virtual ~BindMPxNode();

	///------------------------------------------------------------ virtual functions
	virtual void 	postConstructor();
	virtual MStatus setDependentsDirty(const MPlug &plug, MPlugArray &plugArray);
	virtual MStatus compute(const MPlug &plug, MDataBlock &datablock);

public:
	///------------------------------------------------------------ public helper functions
private:
	///------------------------------------------------------------ private helper functions
	///-------------------------------------- compute outMessage
	MStatus computeOutMessage_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute RestState
	MStatus computeRestState_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute outBody
	MStatus computeOutBody_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute outSources
	MStatus computeOutSources_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute outPoints
	MStatus computeOutPoints_(const MPlug &plug, MDataBlock &datablock);

private:
	std::shared_ptr<RestState> getRestState_(const MPlug &plug, MDataBlock &datablock);
	void 	calcLocalAxis_(const Point3f& p0, const Point3f& p1, const Point3f& p2, Vec3f* target, Vec3f* up, Vec3f* right);
	void 	rotateLocalAxis_(const float radx, const float rady, const float radz, Vec3f* target, Vec3f* up, Vec3f* right);
	MStatus applySourceIDRatio_(MDataBlock& datablock, const std::shared_ptr<Points> outPointsPtr);
	bool    getIndices_(const int polyId, const MFnMesh& meshFn, int* ntriangles,  MIntArray* objectRelativeID,  MIntArray* faceRelativeID);
};

NAMESPACE_BASE_END
