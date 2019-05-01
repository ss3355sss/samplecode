#pragma once
#include "maya/pch.h"

#include "base/hackedQueue.h"
#include "base/messageQueue.h"
#include "base/triangleMesh.h"
#include "base/texture.h"

#include "maya/messageMPxData.h"
#include "maya/pointsMPxData.h"
#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

namespace Zelos
{
class ZImageMap;
}


///------------------------------------------------- DistributorNode (for environemnt templates)
NAMESPACE_BASE_BEGIN
class EnvironmentMPxNode : public MPxNode
{
public:
	static MTypeId id;
	static MString nodeName;
public:
	///------------------------------------------------------------ node attributes
	///------------------------------------------------- distribute points
	static MObject seedObj;
	static MObject npointsObj;
	static MObject sidRatioArrayObj;
	static MObject distributionTypeObj;
	
	///------------------------------------ upVector
	static MObject useUpVectorObj;
	static MObject upVectorObj;

	///------------------------------------ using map
	static MObject uvSetNameObj;
	
	///------------------------------------ density map
	static MObject useDensityMapObj;
	static MObject shiftDensityValueObj;
	static MObject densityMapObj;
	///------------------------------------ remove map
	static MObject useRemoveMapObj;
	static MObject removeEpsilonObj;
	static MObject removeMapObj;
	///------------------------------------ scale map
	static MObject useScaleMapObj;
	static MObject shiftScaleValueObj;
	static MObject scaleMapObj;

	///------------------------------------ face angle
	static MObject useFaceAngleObj;
	static MObject faceAngleRangeObj;

	///------------------------------------------------- randmize points attributes
	static MObject xTranslateRangeObj;
	static MObject yTranslateRangeObj;
	static MObject zTranslateRangeObj;

	static MObject xRotateRangeObj;
	static MObject yRotateRangeObj;
	static MObject zRotateRangeObj;

	static MObject useUniformScaleObj;
	static MObject unifScaleRangeObj;
	static MObject xScaleRangeObj;
	static MObject yScaleRangeObj;
	static MObject zScaleRangeObj;

	///------------------------------------------------- sampling points
	//static MObject useSamplingSchemeObj;
	//static MObject radiusObj;
	//static MObject radiusScaleObj;
	//static MObject useRadiusMapObj;
	//static MObject shiftRadiusValueObj;
	//static MObject radiusMapObj;
	
	///------------------------------------------------- import cache
	static MObject inTimeObj;
	static MObject importSeedObj;
	static MObject importCacheObj;
	static MObject cacheFileObj;
	static MObject attachToBodyObj;
	static MObject applyVariationObj;
	static MObject applySidRatioObj;
	static MObject importCacheRatioObj;
	
	
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
		kBODY_ATTRIBUTE_CHANGED,
		kSOURCE_ATTRIBUTE_CHANGED,
		kPOINT_ATTRIBUTE_CHANGED,
	};
	typedef MessageQueueProxy<State> MessageQueue;
	std::shared_ptr<MessageQueue> pMsgQueue_;
private:
	enum ScatterType : short
	{
		kRandomFaceSelection,
		kRandomFaceSelectionFaceAreaInvariant,
	};

private:
	/*---------------------------------------------------- internal inBody */
	using InBody = boost::variant<
			std::shared_ptr<TriangleMesh3f>
	>;
	InBody inBody_;

private:
	/*---------------------------------------------------- callback id */
	MCallbackId timeChangeCallbackId_;
	
public:
	///------------------------------------------------------------ static functions
	static void* 	Creator();
	static MStatus 	Initialize();
	static void     TimeChangeCallback(MTime &time, void* clientData);
public:
	///------------------------------------------------------------ constructor / destructor
	EnvironmentMPxNode();
	virtual ~EnvironmentMPxNode();

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
	///-------------------------------------- compute outBody
	MStatus computeOutBody_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute outSources
	MStatus computeOutSources_(const MPlug &plug, MDataBlock &datablock);
	///-------------------------------------- compute outPoints
	MStatus computeOutPoints_(const MPlug &plug, MDataBlock &datablock);
	
	MStatus appendPointsByCache_(const int targetPoints,
	                             MDataBlock& datablock,
	                             std::shared_ptr<Points> pOutPoints
	);
	MStatus randomFaceSelectionScatter_(const MPlug &plug,
	                                    MDataBlock &datablock,
	                                    std::shared_ptr<Points> pOutPoints,
	                                    BoolArray* pOutIsDeleted
	);
	//MStatus randomFaceSelectionFaceAraeInvariant_(const MPlug &plug,
	//                                              MDataBlock &datablock,
	//                                              std::shared_ptr<Points> pOutPoints,
	//                                              BoolArray* pOutIsDeleted
	//);
	MStatus randomFaceSelectionFaceAreaInvariant2_(const MPlug &plug,
	                                               MDataBlock &datablock,
	                                               std::shared_ptr<Points> pOutPoints,
	                                               BoolArray* pOutIsDeleted
	);
	MStatus randomFaceSelectionFaceAreaInvariant3_(const MPlug &plug,
	                                               MDataBlock &datablock,
	                                               std::shared_ptr<Points> pOutPoints,
	                                               BoolArray* pOutIsDeleted
	);
	
	
	bool testFaceAngle_(const Point3f& p0, const Point3f& p1, const Point3f& p2, const float& minAngle, const float& maxAngle);
	//bool testFaceAngle_(const Normal3f& n0, const float& minAngle, const float& maxAngle);

	
	MStatus openZImageMap_(const bool use, const MString& path, bool* sucess, Zelos::ZImageMap* map);
	bool 	testDensityMap_(const Zelos::ZImageMap& densityMap, const TexCoord3f& uv, const float& prob, const float& shiftDensity);
	bool 	testRemoveMap_(const Zelos::ZImageMap& removeMap, const TexCoord3f& uv, const float& removeEps);
	float 	getScaleFromMap_(const Zelos::ZImageMap& scaleMap, const TexCoord3f& uv);
	
	void 	calcLocalAxis_(const Point3f& p0, const Point3f& p1, const Point3f& p2, Vec3f* target, Vec3f* up, Vec3f* right);
	void 	rotateLocalAxis_(const float radx, const float rady, const float radz, Vec3f* target, Vec3f* up, Vec3f* right);
	MStatus applySourceIDRatio_(MDataBlock& datablock, const std::shared_ptr<Points> outPointsPtr);
	MStatus getInBodyData_(MDataBlock& datablock,
	                       std::vector<Vec3f>* pointsArrayPtr,
	                       std::vector<Vec2f>* uvsArrayPtr,
	                       std::vector<float>* areaArrayPtr,
	                       int* ntrianglePtr,
	                       float* totalAreaPtr
	);
	
	
	bool getIndices_(const int polyId, const MFnMesh& meshFn,
					 int* ntriangles,
					 MIntArray* objectRelativeID,
					 MIntArray* faceRelativeID);
};

NAMESPACE_BASE_END
