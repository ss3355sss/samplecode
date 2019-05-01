#pragma once
#include "maya/pch.h"

#include "base/instancingItem.h"
#include "base/messageQueue.h"

#include "maya/messageMPxData.h"
#include "maya/sourcesMPxData.h"
#include "maya/pointsMPxData.h"
///------------------------------------------------- TaneMPxSurfaceShape
NAMESPACE_BASE_BEGIN
class TaneMPxShape : public MPxSurfaceShape
{
public:
	static MTypeId id;
	static MString nodeName;
	static MString drawDbClassification;
	static MString drawOverrideRegistrantId;

public:
	///------------------------------------------------------------ display attributes
	static MObject displayModeObj;
	static MObject pointSizeObj;
	static MObject colorModeObj;
	static MObject colorObj;
	static MObject transparencyObj;
	
	///------------------------------------------------------------ renderman attributes
	static MObject groupIDObj;
	static MObject immediateSubdivideObj;
	
	///------------------------------------------------------------ time attributes
	static MObject forceStaticObj;
	static MObject frameOffsetObj;
	static MObject animCycleObj;
	
	///------------------------------------------------------------ input attributes
	static MObject inTimeObj;
	DECL_INPUT_ATTRIBUTE;

	///------------------------------------------------------------ output attributes
	DECL_OUTPUT_ATTRIBUTE;

	///------------------------------------------------------------ internal attributes
	static MObject npointsObj;
	static MObject drawBoundObj;
	
	static MObject enableIcrObj;
	
	static MObject enableLODObj;
	static MObject minDistanceObj;
	static MObject maxDistanceObj;
	
	static MObject noLightingObj;
	static MObject noWireObj;
	static MObject overrideMaskObj;
	
	static MObject notesObj;
	
	DECL_BOUNDINGBOX_ATTRIBUTE
	DECL_MISCELLANEOUS_ATTRIBUTE;
	
	static MObject displayPointInfoObj;
	static MObject localAxisScaleObj;
	static MObject displayLocalAxisObj;

	static MObject cachedPointsObj;
	static MObject addedPointsObj;
	
	///------------------------------------------------------------ tweak attributes
	static MObject useFrustumCullingObj;
	
	static MObject mCulleds;			    //for frustum culling
	static MObject mRenderables;			//for manip
	static MObject mSourceIDs;
	
	static MObject mEulerAngles;
		static MObject mEulerAngleValueX;
		static MObject mEulerAngleValueY;
		static MObject mEulerAngleValueZ;

	static MObject mScales;
		static MObject mScaleValueX;
		static MObject mScaleValueY;
		static MObject mScaleValueZ;
		
public:
	enum State {
		kNODE_DELECETED,

		kNODE_ADDED,

		kINPUT_DIRTIED,

		kMANIPULATING_SOURCE_ID,
		kMANIPULATING_RENDERABLE,
		kMANIPULATING_TRANSFORM,

		kDISPLAY_MODE_CHANGED,
		kSHADER_ATTRIBUTE_CHANGED,
		kRENDER_ATTRIBUTE_CHANGED,
		
		kTIME_CHANGED,
	};
	static std::string StateStr(const State& state)
	{
		switch (state)
		{
			case kNODE_DELECETED:
				return "kNODE_DELECTED";
			case kNODE_ADDED:
				return "kNODE_ADDED";
			case kINPUT_DIRTIED:
				return "kINPUT_DIRTIED";
			case kMANIPULATING_SOURCE_ID:
				return "kMANIPULATING_SOURCE_ID";
			case kMANIPULATING_RENDERABLE:
				return "kMANIPULATING_RENDERABLE";
			case kMANIPULATING_TRANSFORM:
				return "kMANIPULATING_TRANSFORM";
			case kDISPLAY_MODE_CHANGED:
				return "kDISPLAY_MODE_CHANGED";
			case kSHADER_ATTRIBUTE_CHANGED:
				return "kSHADER_ATTRIBUTE_CHANGED";
			case kRENDER_ATTRIBUTE_CHANGED:
				return "kRENDER_ATTRIBUTE_CHANGED";
			case kTIME_CHANGED:
				return "kTIME_CHANGED";
		}
	}
	typedef MessageQueueProxy<State> StateQueue;
private:
	std::shared_ptr<StateQueue>     pStateQueue_;
	std::shared_ptr<InstancingItem> pInstancingItem_;

	bool        	                hasHistoryOnCreate_;
	MCallbackId 	                timeChangeCallbackId_;

public:
	///------------------------------------------------------------ static functions
	static void* 	Creator();
	static MStatus 	Initialize();
	static void 	NodeAddedToModelCallback(MObject &obj, void* clientData);
	static void 	NodeRemovedFromModelCallback(MObject &obj, void* clientData);
	static void 	TimeChangeCallback(MTime &time, void* clientData);
public:
	///------------------------------------------------------------ constructor / destructor
	TaneMPxShape ();
	virtual ~TaneMPxShape ();

	///------------------------------------------------------------ virtual functions
	virtual void postConstructor();

	/* connection methods*/
	virtual MStatus connectionMade(const MPlug &plug, const MPlug &otherPlug, bool asSrc);
	virtual MStatus connectionBroken(const MPlug &plug, const MPlug &otherPlug, bool asSrc);
	virtual MStatus shouldSave(const MPlug &plug, bool &result);

	///* setDependentsDirty methods*/
	virtual MStatus setDependentsDirty(const MPlug &plug, MPlugArray &plugArray);

	///* Bounding box methods*/
	virtual bool isBounded() const;
	virtual MBoundingBox boundingBox() const;

	/* set/get InternalValue method */
	virtual bool setInternalValue(const MPlug &plug, const MDataHandle &handle);
	virtual bool getInternalValue(const MPlug&plug, MDataHandle &result);

	/* Component method */
	virtual bool deleteComponents(const MObjectArray &componentList, MDoubleArray &undoInfo);
	virtual bool undeleteComponents(const MObjectArray &componentList, MDoubleArray &undoInfo);
	/* Compute */
	virtual MStatus compute(const MPlug &plug, MDataBlock &datablock);

	/* Support the translate/rotate/scale tool (components) */
	virtual void transformUsing(const MMatrix &matrix, const MObjectArray &componentList);
	virtual void transformUsing(const MMatrix &mat, const MObjectArray &componentList, MVertexCachingMode cachingMode, MPointArray* pointCache);

	/* Support deformers (components) */
	virtual void weightedTransformUsing(const MTransformationMatrix &xform, const MMatrix* space, const MObjectArray &componentList, MVertexCachingMode cachingMode, MPointArray* pointCache, const MPlane* freezePlane);
	virtual void weightedTweakUsing(const MTransformationMatrix &xform, const MMatrix* space, const MObjectArray &componentList, MVertexCachingMode cachingMode, MPointArray* pointCache, const MPlane* freezePlane, MArrayDataHandle &handle);
	/*virtual MObject createFullVertexGroup() const*/
	/*virtual MObject localShapeInAttr() const*/
	/*virtual MObject localShapeOutAttr() const*/
	/*virtual MObject cachedShapeAttr() const*/
	/*virtual MObject geometryData() const*/
	/*virtual void closestPoint(const MPoint &toThisPoint, MPoint &theClosestPoint, double tolerance) const*/

	/* Attribute to component (components) */
	virtual void componentToPlugs(MObject &component, MSelectionList &list) const;
	virtual MPxSurfaceShape::MatchResult matchComponent(const MSelectionList &item, const MAttributeSpecArray &spec, MSelectionList &list);
	virtual bool match(const MSelectionMask &mask, const MObjectArray &componentList) const;

	/* Set SelectionMask */
	virtual MSelectionMask getShapeSelectionMask() const;
	virtual MSelectionMask getComponentSelectionMask() const;

	/* Associates a user defined iterator with the shape (components) */
	virtual MPxGeometryIterator* geometryIteratorSetup(MObjectArray &componentList, MObject &components, bool forReadOnly);
	virtual bool acceptsGeometryIterator(bool writeable);
	virtual bool acceptsGeometryIterator(MObject &, bool writeable, bool forReadOnly);

public:
	/////------------------------------------------------------------ public helper functions
	bool 						    hasHistory();

	std::shared_ptr<Points>  	    outPoints();
	std::shared_ptr<SourceManager> 	outSources();
	std::shared_ptr<Message>        outMessage();

	std::shared_ptr<Points>  	    cachedPoints();
	
	std::shared_ptr<InstancingItem> instancingItem();
	std::shared_ptr<StateQueue>     stateQueue();
	void 						    signalDirtyToViewport();
public:
	///------------------------------------------------------------ private helper functions
	bool 	setValue_(int pntInd, int vlInd, double val);
	bool 	setValue_(int pntInd, const double3 &val);
	bool 	getValue_(int pntInd, int vlInd, double &val) const;
	bool 	getValue_(int pntInd, double3 &val) const;
	
	MStatus computeInMessage_(const MPlug &plug, MDataBlock &datablock);
	MStatus computeInBody_(const MPlug &plug, MDataBlock &datablock);
	MStatus computeInSources_(const MPlug &plug, MDataBlock &datablock);
	MStatus computeInPoints_(const MPlug &plug, MDataBlock &datablock);
	MStatus computeOutPoints_(const MPlug &plug, MDataBlock &datablock);
	MStatus computeOutput_(const MPlug &plug, MDataBlock &datablock);
	
	//MStatus computeInput_(const MPlug &plug, MDataBlock &datablock);
	//MStatus computeInBody_(const MPlug &plug, MDataBlock &datablock);
	//MStatus computeInSources_(const MPlug &plug, MDataBlock &datablock);
	//MStatus computeInPoints_(const MPlug &plug, MDataBlock &datablock);
	//MStatus computeOutput_(const MPlug &plug, MDataBlock &datablock);
	
	
	MStatus updateTweaks_(const std::shared_ptr<Points> pPoints, const MObjectArray& componentList);
	MStatus applyTweaks_(MDataBlock& datablock, const std::shared_ptr<Points> pCachedPoints, const std::shared_ptr<Points> pOutPoints);
	MStatus cleanTweaks_(MDataBlock& datablock);
	MStatus clearTweaks_(MDataBlock& datablock);
	
	MStatus updateInstanceItem_(const std::shared_ptr<SourceManager>& pOutSourecs, const std::shared_ptr<Points>& pOutPoints);
	
	MStatus buildTweakContainers_(MDataBlock &datablock, int count);
	
	void 	verticesUpdated_();
	MStatus computeBoundingBox_(MDataBlock &datablock);
	
	MStatus updateNumPoints(MDataBlock& datablock, size_t npoints);
	
private:    //back up

};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxSurfaceShapeUI
class TaneMPxShapeUI : public MPxSurfaceShapeUI
{
public:
	static void* Creator();
public:
	TaneMPxShapeUI();
	virtual ~TaneMPxShapeUI();
	virtual void getDrawRequests(const MDrawInfo &info, bool objectAndActiveOnly, MDrawRequestQueue &requests);
	virtual void draw(const MDrawRequest &request, M3dView &view) const;
	virtual bool select(MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts) const;

private:

};
NAMESPACE_BASE_END



