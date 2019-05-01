#pragma once
#include "maya/pch.h"

#include "base/abcReader.h"
#include "base/messageQueue.h"

#include "maya/sourceMPxData.h"

NAMESPACE_BASE_BEGIN
namespace State
{
typedef short Type;
enum : short
{
	kNODE_DELECETED,
	kNODE_ADDED,
	kARCHIVE_ATTRIBUTE_CHANGED,
	kDISPLAY_MODE_ATTRIBUTE_CHANGED,
	kSHADER_ATTRIBUTE_CHANGED,
	kRENDER_ATTRIBUTE_CHAGNED,
	kTIME_CHAGNED,
};
inline std::string ToString(const Type e)
{
	switch (e)
	{
		case kNODE_DELECETED:
			return ENUM_TO_STRING(kNODE_DELECETED);
		case kNODE_ADDED:
			return ENUM_TO_STRING(kNODE_ADDED);
		case kARCHIVE_ATTRIBUTE_CHANGED:
			return ENUM_TO_STRING(kARCHIVE_ATTRIBUTE_CHANGED);
		case kDISPLAY_MODE_ATTRIBUTE_CHANGED:
			return ENUM_TO_STRING(kDISPLAY_MODE_ATTRIBUTE_CHANGED);
		case kSHADER_ATTRIBUTE_CHANGED:
			return ENUM_TO_STRING(kSHADER_ATTRIBUTE_CHANGED);
		case kRENDER_ATTRIBUTE_CHAGNED:
			return ENUM_TO_STRING(kRENDER_ATTRIBUTE_CHAGNED);
		case kTIME_CHAGNED:
			return ENUM_TO_STRING(kTIME_CHAGNED);
			
	}
}
}
typedef MessageQueueProxy<short> StateQueue;
NAMESPACE_BASE_END


NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ ProxyMPxShape
class ProxyMPxShape : public MPxSurfaceShape
{
public:
	typedef MPxSurfaceShape Base;
public:
	///------------------------------------------------------------ display attributes
	static MObject displayModeObj;
	static MObject colorObj;
	static MObject transparencyObj;

	///------------------------------------------------------------ renderman attributes
	static MObject immediateSubdivideObj;
	
	///------------------------------------------------------------ time attributes
	static MObject forceStaticObj;
	static MObject frameOffsetObj;
	static MObject animCycleObj;

	///------------------------------------------------------------ input attributes
	static MObject inTimeObj;

	///------------------------------------------------------------ output attributes
	DECL_OUT_SOURCE_ATTRIBUTE;
	
	///------------------------------------------------------------ internal attributes
	static MObject drawBoundObj;

	static MObject enableIcrObj;

	static MObject enableLODObj;
	static MObject minDistanceObj;
	static MObject maxDistanceObj;

	static MObject noLightingObj;
	static MObject noWireObj;
	static MObject overrideMaskObj;
	
	DECL_MISCELLANEOUS_ATTRIBUTE;
	DECL_BOUNDINGBOX_ATTRIBUTE;

	static MObject notesObj;
	
private:
	bool        hasHistoryOnCreate_;
	MCallbackId timeChangedCallbackID_;
	MCallbackId nodeAddedToModelCallbackID_;
	MCallbackId nodeRemovedFromModelCallbackID_;

	std::shared_ptr<StateQueue> pStateQueue_;
	
public:
	///------------------------------------------------------------ static functions
	static MStatus 	Initialize();
	static void     NodeAddedToModelCallback(MObject& obj, void *clientData);
	static void     NodeRemovedFromModelCallback(MObject& obj, void *clientData);
	static void     TimeChangedCallback(MTime& time, void *clientData);
public:
	///------------------------------------------------------------ constructor / destructor
	ProxyMPxShape();
	virtual ~ProxyMPxShape();

public:
	///------------------------------------------------------------ virtual functions
	virtual void            postConstructor();
	virtual MStatus         setDependentsDirty(const MPlug &plug, MPlugArray &plugArray);
	virtual MStatus         compute(const MPlug &plug, MDataBlock &dataBlock);
	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const;
	virtual MSelectionMask  getShapeSelectionMask() const;
	virtual MSelectionMask  getComponentSelectionMask() const;

protected:
	///------------------------------------------------------------ protected helper functions
	virtual MStatus nodeDeleted(MDataBlock& dataBlock);
	virtual MStatus nodeAdded(MDataBlock& dataBlock);
	virtual MStatus archiveAttribChanged(MDataBlock& dataBlock);
	virtual MStatus displayModeAttribChanged(MDataBlock& dataBlock);
	virtual MStatus shaderAttribChanged(MDataBlock& dataBlock);
	virtual MStatus renderAttribChanged(MDataBlock& dataBlock);
	virtual MStatus timeChanged(MDataBlock& dataBlock);
	virtual MStatus computeBoundingBox_(MDataBlock &datablock);
	
public:
	///------------------------------------------------------------ public helper functions
	MStatus                     nodeDeleted();
	MStatus                     nodeAdded();
	MStatus                     archiveAttribChanged();
	MStatus                     displayModeAttribChanged();
	MStatus                     shaderAttribChanged();
	MStatus                     renderAttribChanged();
	MStatus                     timeChanged();
	std::shared_ptr<StateQueue> stateQueue();
	std::shared_ptr<Source>     outSource();
	void                        signalDirtyToViewport();
private:
	std::string getAttribName_(const MPlug& plug);
	
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeUI
class ProxyMPxShapeUI : public MPxSurfaceShapeUI
{
public:
	ProxyMPxShapeUI();
	virtual ~ProxyMPxShapeUI();
	
public:
	virtual void getDrawRequests(const MDrawInfo &info, bool objectAndActiveOnly, MDrawRequestQueue &requests);
	virtual void draw(const MDrawRequest &request, M3dView &view) const;
	virtual bool select(MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts) const;
};

NAMESPACE_BASE_END

