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
class FetchAttributeMPxNode : public MPxNode
{
public:
	static MTypeId id;
	static MString nodeName;
public:
	///------------------------------------------------------------ node attributes
	static MObject npointsObj;
	static MObject idObj;
	static MObject pointAttribObj;
	
	///------------------------------------------------------------ input attributes
	DECL_INPUT_ATTRIBUTE;
	
	///------------------------------------------------------------ output attributes
	static MObject outAttributeObj;
		static MObject outPidObj;
		static MObject outSidObj;
		static MObject outRenderableObj;
		static MObject outPositionObj;
		static MObject outEulerAnglesObj;
		static MObject outScaleObj;
	//DECL_OUTPUT_ATTRIBUTE;
	
private:
	enum State
	{
	};
	typedef MessageQueueProxy<State> MessageQueue;
	std::shared_ptr<MessageQueue> pMsgQueue_;
public:
	///------------------------------------------------------------ static functions
	static void* 	Creator();
	static MStatus 	Initialize();
public:
	///------------------------------------------------------------ constructor / destructor
	FetchAttributeMPxNode();
	virtual ~FetchAttributeMPxNode();

	///------------------------------------------------------------ virtual functions
	virtual void 	postConstructor();
	virtual MStatus setDependentsDirty(const MPlug &plug, MPlugArray &plugArray);
	virtual MStatus compute(const MPlug &plug, MDataBlock &datablock);

public:
	///------------------------------------------------------------ public helper functions
private:
	///------------------------------------------------------------ private helper functions
	
private:
	///------------------------------------------------------------ auxiliary functions
};

NAMESPACE_BASE_END
