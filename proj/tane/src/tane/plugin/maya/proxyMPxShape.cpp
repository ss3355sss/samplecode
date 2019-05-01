#pragma once

#include "maya/pch.h"
#include "maya/proxyMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------ display attributes
MObject ProxyMPxShape::displayModeObj;
MObject ProxyMPxShape::colorObj;
MObject ProxyMPxShape::transparencyObj;

///------------------------------------------------------------ renderman attributes
MObject ProxyMPxShape::immediateSubdivideObj;

///------------------------------------------------------------ time attributes
MObject ProxyMPxShape::forceStaticObj;
MObject ProxyMPxShape::frameOffsetObj;
MObject ProxyMPxShape::animCycleObj;

///------------------------------------------------------------ input attributes
MObject ProxyMPxShape::inTimeObj;

///------------------------------------------------------------ output attributes
IMPL_OUT_SOURCE_ATTRIBUTE(ProxyMPxShape);

///------------------------------------------------------------ internal attributes
MObject ProxyMPxShape::drawBoundObj;

MObject ProxyMPxShape::enableIcrObj;

MObject ProxyMPxShape::enableLODObj;
MObject ProxyMPxShape::minDistanceObj;
MObject ProxyMPxShape::maxDistanceObj;

MObject ProxyMPxShape::noLightingObj;
MObject ProxyMPxShape::noWireObj;
MObject ProxyMPxShape::overrideMaskObj;

IMPL_MISCELLANEOUS_ATTRIBUTE(ProxyMPxShape);
IMPL_BOUNDINGBOX_ATTRIBUTE(ProxyMPxShape);
MObject ProxyMPxShape::notesObj;

///------------------------------------------------------------ static functions
MStatus ProxyMPxShape::Initialize()
{
	MStatus              stat;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnCompoundAttribute cAttr;
	MFnUnitAttribute     uAttr;
	MFnMatrixAttribute   mAttr;
	MFnEnumAttribute     eAttr;
	
	///------------------------------------------------------------ display attributes
	displayModeObj = eAttr.create("displayMode", "displayMode", mode::display::proxy, &stat);
	MCHECKERROR(stat, "Failed to create displayMode attribute");
	eAttr.addField("None",        mode::display::none);
	eAttr.addField("BoundingBox", mode::display::boundingbox);
	eAttr.addField("ProxyMesh",   mode::display::proxy);
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
	ADD_ATTRIBUTE(displayModeObj);
	
	colorObj = nAttr.createColor("color", "color", &stat);
	MCHECKERROR(stat, "Failed to create color attribute");
	nAttr.setDefault(0.25f, 0.25f, 0.25f);
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
	ADD_ATTRIBUTE(colorObj);
	transparencyObj = nAttr.create("transparency", "transparency", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
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
	ADD_ATTRIBUTE(transparencyObj);
	
	
	///------------------------------------------------------------ renderman attributes
	immediateSubdivideObj = nAttr.create("immediateSubdivide", "immediateSubdivide", MFnNumericData::kInt, 1, &stat);
	MCHECKERROR(stat, "Failed to create immediateSubdivideObj attribute");
	nAttr.setMin(0);
	nAttr.setMax(2);
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
	ADD_ATTRIBUTE(immediateSubdivideObj);
	
	///------------------------------------------------------------ time attributes
	forceStaticObj = nAttr.create("forceStatic", "forceStatic", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create forceStatic attribute error");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(forceStaticObj);
	
	frameOffsetObj = nAttr.create("frameOffset", "frameOffset", MFnNumericData::kDouble, 0.0, &stat);
	nAttr.setSoftMax(240);
	nAttr.setSoftMin(-240);
	MCHECKERROR(stat, "Failed to create frameOffset attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(frameOffsetObj);
	
	animCycleObj = eAttr.create("animCycle", "animCycle", mode::animCycle::none, &stat);
	MCHECKERROR(stat, "Failed to create animCycle attribute");
	eAttr.addField("None", mode::animCycle::none);
	eAttr.addField("Looped", mode::animCycle::looped);
	eAttr.addField("Osciilated", mode::animCycle::osciilated);
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
	ADD_ATTRIBUTE(animCycleObj);
	
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
	
	///------------------------------------------------------------ output attributes
	INITIALIZE_OUT_SOURCE_ATTRIBUTE;
	
	///------------------------------------------------------------ internal attributes
	#ifdef DEBUG
	drawBoundObj = nAttr.create("drawBound", "drawBound", MFnNumericData::kBoolean, true, &stat);
	#else
	drawBoundObj = nAttr.create("drawBound", "drawBound", MFnNumericData::kBoolean, false, &stat);
	#endif
	MCHECKERROR(stat, "Failed to create forceStatic attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
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
	ADD_ATTRIBUTE(drawBoundObj);
	
	enableIcrObj = nAttr.create("enableIcr", "enableIcr", MFnNumericData::kBoolean, true, &stat);
	MCHECKERROR(stat, "Failed to create enableIcr attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(enableIcrObj);
	
	enableLODObj = nAttr.create("enableLOD", "enableLOD", MFnNumericData::kBoolean, true, &stat);
	MCHECKERROR(stat, "Failed to create enableLOD attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(enableLODObj);
	minDistanceObj = nAttr.create("minDistance", "minDistance", MFnNumericData::kDouble, 50.0, &stat);
	nAttr.setMin(0);
	nAttr.setSoftMax(200);
	MCHECKERROR(stat, "Failed to create minDistance attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(minDistanceObj);
	maxDistanceObj = nAttr.create("maxDistance", "maxDistance", MFnNumericData::kDouble, 200.0, &stat);
	nAttr.setMin(0);
	nAttr.setSoftMax(400);
	MCHECKERROR(stat, "Failed to create maxDistance attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(maxDistanceObj);
	noLightingObj = nAttr.create("noLighting", "noLighting", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create noLighting attribute");
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
	ADD_ATTRIBUTE(noLightingObj);
	noWireObj = nAttr.create("noWire", "noWire", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create noWire attribute");
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
	ADD_ATTRIBUTE(noWireObj);
	overrideMaskObj = nAttr.create("overrideMask", "overrideMask", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create overrideMask attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
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
	ADD_ATTRIBUTE(overrideMaskObj);
	
	INITIALIZE_BOUNDINGBOX_ATTRIBUTE;
	INITIALIZE_MISCELLANEOUS_ATTRIBUTE(false);
	
	notesObj = tAttr.create("notes", "notes", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create notes attribute");
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
	ADD_ATTRIBUTE(notesObj);
	
	ATTRIBUTE_AFFECTS(displayModeObj, outSourceObj);
	
	ATTRIBUTE_AFFECTS(frameOffsetObj, outSourceObj);
	ATTRIBUTE_AFFECTS(animCycleObj,   outSourceObj);
	ATTRIBUTE_AFFECTS(forceStaticObj, outSourceObj);
	
	ATTRIBUTE_AFFECTS(recomputeObj, outSourceObj);
	
	ATTRIBUTE_AFFECTS(outSourceObj, minCornerObj);
	ATTRIBUTE_AFFECTS(outSourceObj, maxCornerObj);
	
	return stat;
}
void ProxyMPxShape::NodeAddedToModelCallback(MObject& obj, void* clientData)
{
	DECL_LOGGER(logging::maya->ProxyMPxShape)
	MFnDagNode shapeFn(obj);
	ProxyMPxShape* node = reinterpret_cast<ProxyMPxShape*>(shapeFn.userNode());
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}

	
	MStatus stat;
	/* add TimeChangedCallback */
	node->timeChangedCallbackID_ = MDGMessage::addTimeChangeCallback(TimeChangedCallback, node);
	
	node->stateQueue()->push(State::kNODE_ADDED);
	node->signalDirtyToViewport();
}
void ProxyMPxShape::NodeRemovedFromModelCallback(MObject& obj, void* clientData)
{
	DECL_LOGGER(logging::maya->ProxyMPxShape)
	MFnDagNode shapeFn(obj);
	ProxyMPxShape* node = reinterpret_cast<ProxyMPxShape*>(shapeFn.userNode());
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}

	if(node->timeChangedCallbackID_)
	{
		MMessage::removeCallback(node->timeChangedCallbackID_);
		node->timeChangedCallbackID_ = 0;
	}
	
	node->stateQueue()->push(State::kNODE_DELECETED);
	node->signalDirtyToViewport();
}
void ProxyMPxShape::TimeChangedCallback(MTime& time, void* clientData)
{
	DECL_LOGGER(logging::maya->ProxyMPxShape)
	ProxyMPxShape* node = reinterpret_cast<ProxyMPxShape*>(clientData);
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	node->stateQueue()->push(State::kTIME_CHAGNED);
	node->timeChanged();
	node->signalDirtyToViewport();
}
///------------------------------------------------------------ constructor / destructor
ProxyMPxShape::ProxyMPxShape() : Base()
{

}
ProxyMPxShape::~ProxyMPxShape()
{

}

///------------------------------------------------------------ virtual functions
void ProxyMPxShape::postConstructor()
{
	///-------------------------------- initialize data
	pStateQueue_ = std::make_shared<StateQueue>();
	
	///-------------------------------- set history
	hasHistoryOnCreate_ = false;
	
	///-------------------------------- set renderable
	setRenderable(true);
	
	///-------------------------------- register callback
	nodeAddedToModelCallbackID_     = MModelMessage::addNodeAddedToModelCallback(thisMObject(), NodeAddedToModelCallback);
	nodeRemovedFromModelCallbackID_ = MModelMessage::addNodeRemovedFromModelCallback(thisMObject(), NodeRemovedFromModelCallback);
	
}
MStatus ProxyMPxShape::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	
	std::string name;
	if(plug.isChild(&stat))
		name = plug.parent().partialName().asChar();
	else
		name = plug.partialName().asChar();
	LOGGER_DEBUG("name = %s", name.c_str());
	
	///------------------------------------------------------------ display attributes
	if(name == "displayMode")
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		pStateQueue_->push(State::kDISPLAY_MODE_ATTRIBUTE_CHANGED);
		signalDirtyToViewport();
	}
		///------------------------------------------------------------ renderman attributes
	else if(name == "immediateSubdivide")
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		pStateQueue_->push(State::kRENDER_ATTRIBUTE_CHAGNED);
		signalDirtyToViewport();
	}
		///------------------------------------------------------------ time attributes
	else if(name == "forceStatic" ||
	        name == "frameOffset" ||
	        name == "animCycle"
		)
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		pStateQueue_->push(State::kTIME_CHAGNED);
		signalDirtyToViewport();
	}
		///------------------------------------------------------------ input attributes
		///------------------------------------------------------------ output attributes

		///------------------------------------------------------------ internal attributes
	else if(name == "color"        ||
	        name == "transparency" ||
	        name == "drawBound"    ||
	        name == "enableIcr"    ||
	        name == "enableLOD"    ||
	        name == "minDistance"  ||
	        name == "maxDistance"  ||
	        name == "noLighting"   ||
	        name == "noWire"       ||
	        name == "overrideMask"
		)
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		pStateQueue_->push(State::kSHADER_ATTRIBUTE_CHANGED);
		signalDirtyToViewport();
	}
	return stat;
	
	
	/////------------------------------------------------------------ display attributes
	//if(plug == displayModeObj)
	//{
	//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	//
	//	pStateQueue_->push(State::kDISPLAY_MODE_ATTRIBUTE_CHANGED);
	//	signalDirtyToViewport();
	//}
	//	///------------------------------------------------------------ renderman attributes
	//else if(plug == immediateSubdivideObj)
	//{
	//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	//	pStateQueue_->push(State::kRENDER_ATTRIBUTE_CHAGNED);
	//	signalDirtyToViewport();
	//}
	//	///------------------------------------------------------------ time attributes
	//else if(plug == forceStaticObj ||
	//        plug == frameOffsetObj ||
	//        plug == animCycleObj
	//	)
	//{
	//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	//	pStateQueue_->push(State::kTIME_CHAGNED);
	//	signalDirtyToViewport();
	//}
	//	///------------------------------------------------------------ input attributes
	//	///------------------------------------------------------------ output attributes
	//
	//	///------------------------------------------------------------ internal attributes
	//else if(plug == colorObj        || plug.parent() == colorObj ||
	//        plug == transparencyObj ||
	//        plug == drawBoundObj    ||
	//        plug == enableIcrObj    ||
	//        plug == enableLODObj    ||
	//        plug == minDistanceObj  ||
	//        plug == maxDistanceObj  ||
	//        plug == noLightingObj   ||
	//        plug == noWireObj       ||
	//        plug == overrideMaskObj
	//	)
	//{
	//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
	//	pStateQueue_->push(State::kSHADER_ATTRIBUTE_CHANGED);
	//	signalDirtyToViewport();
	//}
	//return stat;
}
MStatus ProxyMPxShape::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	
	MStatus     stat;
	
	std::string name;
	if(plug.isChild(&stat))
		name = plug.parent().partialName().asChar();
	else
		name = plug.partialName().asChar();
	if(name == "outSource")
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		
		StateQueue queue = pStateQueue_->clone();
		while (!queue.empty())
		{
			LOGGER_DEBUG("state = %s", State::ToString(queue.top()).c_str());
			switch (queue.top())
			{
				case State::kNODE_DELECETED:
					stat = nodeDeleted(dataBlock);
					MCHECKERROR(stat, "Failed to nodeDeleted()");
					break;
				case State::kNODE_ADDED:
					stat = nodeAdded(dataBlock);
					MCHECKERROR(stat, "Failed to nodeAdded()");
					break;
				case State::kARCHIVE_ATTRIBUTE_CHANGED:
					stat = archiveAttribChanged(dataBlock);
					MCHECKERROR(stat, "Failed to archiveAttribChanged()");
					break;
				case State::kDISPLAY_MODE_ATTRIBUTE_CHANGED:
					stat = displayModeAttribChanged(dataBlock);
					MCHECKERROR(stat, "Failed to displayModeAttribChanged()");
					break;
				case State::kSHADER_ATTRIBUTE_CHANGED:
					stat = shaderAttribChanged(dataBlock);
					MCHECKERROR(stat, "Failed to shaderAttribChanged()");
					break;
				case State::kRENDER_ATTRIBUTE_CHAGNED:
					stat = renderAttribChanged(dataBlock);
					MCHECKERROR(stat, "Failed to renderAttribChanged()");
					break;
				case State::kTIME_CHAGNED:
					stat = timeChanged(dataBlock);
					MCHECKERROR(stat, "Failed to timeChanged()");
					break;
			}
			queue.pop();
		};
		stat = dataBlock.setClean(plug);
	}
	else if(name == "minCorner" || name == "maxCorner")
	{
		LOGGER_INFO("attrib name = %s", name.c_str());
		stat = computeBoundingBox_(dataBlock);
		MCHECKERROR(stat, "Failed to compute boundingbox");
	}
	else
	{
		LOGGER_WARNING("attrib name = %s", name.c_str());
		stat = MS::kUnknownParameter;
	}
	return stat;
}
bool ProxyMPxShape::isBounded() const
{
	return false;
}
MBoundingBox ProxyMPxShape::boundingBox() const
{
	MObject thisNode = thisMObject();
	MPlug   minCornerPlug(thisNode, minCornerObj);
	MPlug   maxCornerPlug(thisNode, maxCornerObj);
	MObject minCornerObject;
	MObject maxCornerObject;
	minCornerPlug.getValue(minCornerObject);
	maxCornerPlug.getValue(maxCornerObject);

	double3 minCorner, maxCorner;

	MFnNumericData fnData;
	fnData.setObject(minCornerObject);
	fnData.getData(minCorner[0], minCorner[1], minCorner[2]);
	fnData.setObject(maxCornerObject);
	fnData.getData(maxCorner[0], maxCorner[1], maxCorner[2]);


	return MBoundingBox(MPoint(minCorner[0], minCorner[1], minCorner[2]),
	                    MPoint(maxCorner[0], maxCorner[1], maxCorner[2]));
}
MSelectionMask ProxyMPxShape::getShapeSelectionMask() const
{
	MSelectionMask::SelectionType selType = MSelectionMask::kSelectMeshes;
	return MSelectionMask(selType);
}
MSelectionMask ProxyMPxShape::getComponentSelectionMask() const
{
	MSelectionMask retVal(MSelectionMask::kSelectMeshVerts);
	return retVal;
}

///------------------------------------------------------------ protected helper functions
MStatus ProxyMPxShape::nodeDeleted(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("nodeDeleted() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::nodeAdded(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("nodeAdded() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::archiveAttribChanged(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("archiveAttribChanged() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::displayModeAttribChanged(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("displayModeAttirbChanged() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::shaderAttribChanged(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("shaderAttribChanged() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::renderAttribChanged(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("renderAttribChanged() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::timeChanged(MDataBlock& dataBlock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus stat;
	STDOUT_WARRNING("timeChanged() is not overrided");
	return stat;
}
MStatus ProxyMPxShape::computeBoundingBox_(MDataBlock& datablock)
{
	DECL_DG_LOGGER(logging::maya->ProxyMPxShape)
	MStatus                 stat;
	std::shared_ptr<Source> pSource = outSource();
	if(pSource == nullptr || (pSource && !pSource->renderItem()))
	{
		LOGGER_ERROR("Failed to get valid source");
		return MS::kFailure;
	}
	
	/// Update bounding box
	MDataHandle lowerHandle = datablock.outputValue(minCornerObj);
	MDataHandle upperHandle = datablock.outputValue(maxCornerObj);
	double3& lower = lowerHandle.asDouble3();
	double3& upper = upperHandle.asDouble3();
	
	const BBox3f & bound     = pSource->bound();
	const Point3f& minCorner = bound.lower();
	const Point3f& maxCorner = bound.upper();
	
	lower[0] = minCorner[0];
	lower[1] = minCorner[1];
	lower[2] = minCorner[2];
	
	upper[0] = maxCorner[0];
	upper[1] = maxCorner[1];
	upper[2] = maxCorner[2];
	
	lowerHandle.setClean();
	upperHandle.setClean();
	
	LOGGER_DEBUG("lower = (%5.2f, %5.2f, %5.2f)", lower[0], lower[1], lower[2]);
	LOGGER_DEBUG("upper = (%5.2f, %5.2f, %5.2f)", upper[0], upper[1], upper[2]);
	
	// Signal that the bounding box has changed.
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return stat;
}

///------------------------------------------------------------ public helper functions
MStatus ProxyMPxShape::nodeDeleted()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat = nodeDeleted(dataBlock);
	MCHECKERROR(stat, "Failed to nodeDeleted()");
	return stat;
}
MStatus ProxyMPxShape::nodeAdded()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = nodeAdded(dataBlock);
	MCHECKERROR(stat, "Failed to nodeAdded()");
	return stat;
}
MStatus ProxyMPxShape::archiveAttribChanged()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = archiveAttribChanged(dataBlock);
	MCHECKERROR(stat, "Failed to archiveAttribChanged()");
	return stat;
}
MStatus ProxyMPxShape::displayModeAttribChanged()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = displayModeAttribChanged(dataBlock);
	MCHECKERROR(stat, "Failed to displayModeAttribChanged()");
	return stat;
}
MStatus ProxyMPxShape::shaderAttribChanged()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = shaderAttribChanged(dataBlock);
	MCHECKERROR(stat, "Failed to shaderAttribChanged()");
	return stat;
}
MStatus ProxyMPxShape::renderAttribChanged()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = renderAttribChanged(dataBlock);
	MCHECKERROR(stat, "Failed to renderAttribChanged()");
	return stat;
}
MStatus ProxyMPxShape::timeChanged()
{
	MStatus    stat;
	MDataBlock dataBlock = forceCache();
	stat                 = timeChanged(dataBlock);
	MCHECKERROR(stat, "Failed to timeChanged()");
	return stat;
}
std::shared_ptr<StateQueue> ProxyMPxShape::stateQueue()
{
	return pStateQueue_;
}
std::shared_ptr<Source> ProxyMPxShape::outSource()
{
	MStatus stat;
	
	MDataBlock  dataBlock    = forceCache();
	MDataHandle outSourceHnd = dataBlock.inputValue(outSourceObj);
	
	MFnPluginData fnData(outSourceHnd.data());
	SourceMPxData* pMPxData = dynamic_cast<SourceMPxData*>(fnData.data(&stat));
	MCHECKERRORNORET(stat, "Failed to get SourceMPxData");
	
	std::shared_ptr<Source> ret = nullptr;
	if(pMPxData != nullptr)
	{
		ret = pMPxData->data();
	}
	return ret;
}
void ProxyMPxShape::signalDirtyToViewport()
{
	MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
}
std::string ProxyMPxShape::getAttribName_(const MPlug& plug)
{
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
	
	MStatus     stat;
	std::string name = plug.name(&stat).asChar();
	MCHECKERRORNORET(stat, "Failed to get plug name");
	
	boost::char_separator<char> sep{"."};
	Tokenizer                   tokenizer(name, sep);
	std::vector<std::string>    tokens;
	for (Tokenizer::iterator    iter = tokenizer.begin(); iter != tokenizer.end(); ++iter)
		tokens.push_back(*iter);
	return tokens[tokens.size() - 1];
	
}

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeUI
ProxyMPxShapeUI::ProxyMPxShapeUI()
{
}
ProxyMPxShapeUI::~ProxyMPxShapeUI()
{
}

void ProxyMPxShapeUI::getDrawRequests(const MDrawInfo& info, bool objectAndActiveOnly, MDrawRequestQueue& requests)
{
	MPxSurfaceShapeUI::getDrawRequests(info, objectAndActiveOnly, requests);
}
void ProxyMPxShapeUI::draw(const MDrawRequest& request, M3dView& view) const
{
	MPxSurfaceShapeUI::draw(request, view);
}
bool ProxyMPxShapeUI::select(MSelectInfo& selectInfo, MSelectionList& selectionList, MPointArray& worldSpaceSelectPts) const
{
	return MPxSurfaceShapeUI::select(selectInfo, selectionList, worldSpaceSelectPts);
}

NAMESPACE_BASE_END

