#include "maya/taneMPxShape.h"

#include "base/hackedQueue.h"

#include "base/points.h"

#include "lib/baseMaya/util.h"

#include "maya/messageMPxData.h"
#include "maya/pointsMPxData.h"
#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxShape
MTypeId TaneMPxShape::id(RegisteredNodes->TaneMPxShape.id);
MString TaneMPxShape::nodeName(RegisteredNodes->TaneMPxShape.name);
MString TaneMPxShape::drawOverrideRegistrantId(MString(RegisteredNodes->TaneMPxShape.name) + "GeometryOverride");
MString TaneMPxShape::drawDbClassification("drawdb/geometry/" + MString(RegisteredNodes->TaneMPxShape.name));

///------------------------------------------------------------ display attributes
MObject TaneMPxShape::displayModeObj;
MObject TaneMPxShape::pointSizeObj;
MObject TaneMPxShape::colorModeObj;
MObject TaneMPxShape::colorObj;
MObject TaneMPxShape::transparencyObj;

///------------------------------------------------------------ renderman attributes
MObject TaneMPxShape::groupIDObj;
MObject TaneMPxShape::immediateSubdivideObj;

///------------------------------------------------------------ time attributes
MObject TaneMPxShape::forceStaticObj;
MObject TaneMPxShape::frameOffsetObj;
MObject TaneMPxShape::animCycleObj;

///------------------------------------------------------------ input attributes
MObject TaneMPxShape::inTimeObj;
IMPL_INPUT_ATTRIBUTE(TaneMPxShape);

///------------------------------------------------------------ output attributes
IMPL_OUTPUT_ATTRIBUTE(TaneMPxShape);

///------------------------------------------------------------ internal attributes
MObject TaneMPxShape::npointsObj;
MObject TaneMPxShape::drawBoundObj;
MObject TaneMPxShape::enableIcrObj;
MObject TaneMPxShape::enableLODObj;
MObject TaneMPxShape::minDistanceObj;
MObject TaneMPxShape::maxDistanceObj;
MObject TaneMPxShape::noLightingObj;
MObject TaneMPxShape::noWireObj;
MObject TaneMPxShape::overrideMaskObj;
MObject TaneMPxShape::notesObj;

IMPL_BOUNDINGBOX_ATTRIBUTE(TaneMPxShape)
IMPL_MISCELLANEOUS_ATTRIBUTE(TaneMPxShape);

MObject TaneMPxShape::displayPointInfoObj;
MObject TaneMPxShape::localAxisScaleObj;
MObject TaneMPxShape::displayLocalAxisObj;
MObject TaneMPxShape::cachedPointsObj;
MObject TaneMPxShape::addedPointsObj;

///------------------------------  manip attrib
MObject TaneMPxShape::useFrustumCullingObj;
MObject TaneMPxShape::mCulleds;
MObject TaneMPxShape::mRenderables;
MObject TaneMPxShape::mSourceIDs;
MObject TaneMPxShape::mEulerAngles;
MObject TaneMPxShape::mEulerAngleValueX;
MObject TaneMPxShape::mEulerAngleValueY;
MObject TaneMPxShape::mEulerAngleValueZ;
MObject TaneMPxShape::mScales;
MObject TaneMPxShape::mScaleValueX;
MObject TaneMPxShape::mScaleValueY;
MObject TaneMPxShape::mScaleValueZ;

///------------------------------------------------------------ static functions
void* TaneMPxShape::Creator()
{
	return new TaneMPxShape;
}
MStatus TaneMPxShape::Initialize()
{
	MStatus              stat;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnUnitAttribute     uAttr;
	MFnEnumAttribute     eAttr;
	MFnCompoundAttribute cAttr;
	MFnMessageAttribute  msgAttr;
	MFnPluginData        pluginDataFn;
	
	displayModeObj = eAttr.create("displayMode", "displayMode", mode::display::proxy, &stat);
	MCHECKERROR(stat, "Failed to create displayMode attribute");
	eAttr.addField("None",        mode::display::none);
	eAttr.addField("Points",      mode::display::points);
	eAttr.addField("BoundingBox", mode::display::boundingbox);
	eAttr.addField("ProxyMesh",   mode::display::proxy);
	eAttr.addField("For Each",    mode::display::foreach);
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
	pointSizeObj = nAttr.create("pointSize", "pointSize", MFnNumericData::kInt, 2, &stat);
	MCHECKERROR(stat, "Failed to create pointSize attribute");
	nAttr.setMin(0);
	nAttr.setSoftMax(5);
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     false,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(pointSizeObj);
	colorModeObj = eAttr.create("colorMode", "colorMode", mode::color::foreach, &stat);
	MCHECKERROR(stat, "Failed to create colorMode attribute");
	eAttr.addField("Solid Color",      mode::color::solid);
	eAttr.addField("Per Source Color", mode::color::foreach);
	//eAttr.addField("Random Color",     mode::color::random);
	SET_ATTRIBUTE_STATUS(eAttr,
	                     false,                     /* Readable              */
	                     true ,                     /* Writable              */
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
	ADD_ATTRIBUTE(colorModeObj);
	
	
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
	groupIDObj = nAttr.create("groupID", "groupID", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create groupID attribute error");
	nAttr.setMin(0);
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(groupIDObj);
	
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
	
	INITIALIZE_INPUT_ATTRIBUTE;
	///------------------------------------------------------------ output attributes
	INITIALIZE_OUTPUT_ATTRIBUTE;
	
	///------------------------------------------------------------ internal attributes
	npointsObj = nAttr.create("npoints", "npoints", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create npoints attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     true,                      /* Readable              */
	                     true,                      /* Writable              */
	                     false,                     /* Connectable           */
	                     false,                     /* Stroable              */
	                     false,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(npointsObj);
	#ifdef DEBUG
	drawBoundObj = nAttr.create("drawBound", "drawBound", MFnNumericData::kBoolean, false, &stat);
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
	
	INITIALIZE_BOUNDINGBOX_ATTRIBUTE;
	INITIALIZE_MISCELLANEOUS_ATTRIBUTE(false);
	
	
	#ifdef DEBUG
	displayPointInfoObj = nAttr.create("displayPointInfo", "displayPointInfo", MFnNumericData::kBoolean, false, &stat);
	#else
	displayPointInfoObj = nAttr.create("displayPointInfo", "displayPointInfo", MFnNumericData::kBoolean, false, &stat);
	#endif
	MCHECKERROR(stat, "Failed to create displayPointInfo attribute error");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                     /* Writable              */
	                     false,                     /* Connectable           */
	                     false,                     /* Stroable              */
	                     false,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(displayPointInfoObj);

#ifdef DEBUG
	displayLocalAxisObj = nAttr.create("displayLocalAxis", "displayLocalAxis", MFnNumericData::kBoolean, false , &stat);
#else
	displayLocalAxisObj = nAttr.create("displayLocalAxis", "displayLocalAxis", MFnNumericData::kBoolean, false, &stat);
#endif
	MCHECKERROR(stat, "Failed to create displayLocalAxis attribute error");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                     /* Writable              */
	                     false,                     /* Connectable           */
	                     false,                     /* Stroable              */
	                     false,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(displayLocalAxisObj);
	localAxisScaleObj = nAttr.create("localAxisScale", "localAxisScale", MFnNumericData::kFloat, 1.0f, &stat);
	MCHECKERROR(stat, "Failed to create localAxisScale attribute error");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     false,                     /* Readable              */
	                     true,                     /* Writable              */
	                     false,                     /* Connectable           */
	                     false,                     /* Stroable              */
	                     false,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(localAxisScaleObj);

	
	cachedPointsObj = tAttr.create("cachedPoints", "cachedPoints", PointsMPxData::id, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create cachedPoints attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
	                     true,                     /* Readable              */
	                     true,                     /* Writable              */
	                     false,                     /* Connectable           */
	                     false,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     true,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(cachedPointsObj);
	
	addedPointsObj = tAttr.create("addedPoints", "addedPoints", PointsMPxData::id, pluginDataFn.create(MTypeId(PointsMPxData::id)), &stat);
	MCHECKERROR(stat, "Failed to create addedPoints attribute");
	SET_ATTRIBUTE_STATUS(tAttr,
	                     true,                      /* Readable              */
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
	ADD_ATTRIBUTE(addedPointsObj);
	
	///------------------------------------------------------------ tweak attributes
	useFrustumCullingObj = nAttr.create("useFrustumCulling", "useFrustumCulling", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create useFrustumCulling attribute");
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
	ADD_ATTRIBUTE(useFrustumCullingObj);
	
	
	mCulleds = nAttr.create("mCulleds", "mCulleds", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create mCulleds attribute error");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     true ,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     true ,                     /* Array                 */
	                     true ,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     true ,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(mCulleds);
	
	mRenderables = nAttr.create("mRenderables", "mRenderables", MFnNumericData::kBoolean, true, &stat);
	MCHECKERROR(stat, "Failed to create mRenderables attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     true ,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     true ,                     /* Array                 */
	                     true ,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     true ,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(mRenderables);
	
	mSourceIDs = nAttr.create("mSourceIDs", "mSourceIDs", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create mySourceIDs attribute");
	SET_ATTRIBUTE_STATUS(nAttr,
	                     true ,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     true ,                     /* Array                 */
	                     true ,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     true ,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(mSourceIDs);
	
	mEulerAngles  = cAttr.create("mEulerAngles", "mEulerAngles", &stat);
	MCHECKERROR(stat, "Failed to create eulerAngles attribute");
		mEulerAngleValueX = nAttr.create("eulerAngleValueX", "eulerAngleValueX", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create eulerAngleValueX attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mEulerAngleValueX);
		mEulerAngleValueY = nAttr.create("eulerAngleValueY", "eulerAngleValueY", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create eulerAngleValueY attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mEulerAngleValueY);
		mEulerAngleValueZ = nAttr.create("eulerAngleValueZ", "eulerAngleValueZ", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create eulerAngleValueZ attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mEulerAngleValueZ);
		cAttr.addChild(mEulerAngleValueX);
		cAttr.addChild(mEulerAngleValueY);
		cAttr.addChild(mEulerAngleValueZ);
		SET_ATTRIBUTE_STATUS(cAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     true ,                     /* Array                 */
		                     true ,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
	ADD_ATTRIBUTE(mEulerAngles);
	
	mScales  = cAttr.create("mScales", "mScales", &stat);
	MCHECKERROR(stat, "Failed to create scales attribute");
		mScaleValueX = nAttr.create("scaleValueX", "scaleValueX", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create scaleValueX attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mScaleValueX);
		mScaleValueY = nAttr.create("scaleValueY", "scaleValueY", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create scaleValueY attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false ,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mScaleValueY);
		mScaleValueZ = nAttr.create("scaleValueZ", "scaleValueZ", MFnNumericData::kDouble, 0.0, &stat);
		MCHECKERROR(stat, "Failed to create scaleValueZ attribute");
		SET_ATTRIBUTE_STATUS(nAttr,
		                     true ,                     /* Readable              */
		                     true ,                     /* Writable              */
		                     true ,                     /* Connectable           */
		                     true ,                     /* Stroable              */
		                     true ,                     /* Cached                */
		                     false,                     /* ChannelBox            */
		                     false,                     /* Array                 */
		                     false,                     /* Use Array DataBuilder */
		                     true ,                     /* Keyable               */
		                     true ,                     /* Hidden                */
		                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
		                     false                      /* Affect Appearance     */
		);
		ADD_ATTRIBUTE(mScaleValueZ);
		cAttr.addChild(mScaleValueX);
		cAttr.addChild(mScaleValueY);
		cAttr.addChild(mScaleValueZ);
	SET_ATTRIBUTE_STATUS(cAttr,
	                     true ,                     /* Readable              */
	                     true ,                     /* Writable              */
	                     true ,                     /* Connectable           */
	                     true ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     false,                     /* ChannelBox            */
	                     true ,                     /* Array                 */
	                     true ,                     /* Use Array DataBuilder */
	                     true ,                     /* Keyable               */
	                     true ,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	ADD_ATTRIBUTE(mScales);
	
	///------------------------------------------------------------ set input/output attribute affects
	///------------------------------------------------------------ display attributes
	///------------------------------------------------------------ renderman attributes
	///------------------------------------------------------------ time attributes
	///------------------------------------------------------------ input attributes
	ATTRIBUTE_AFFECTS(inputObj,        outputObj);
	///------------------------------------------------------------ output attributes
	///------------------------------------------------------------ internal attributes
	
	ATTRIBUTE_AFFECTS(cachedPointsObj, outputObj);
	
	ATTRIBUTE_AFFECTS(recomputeObj,    outputObj);
	ATTRIBUTE_AFFECTS(recomputeObj,    cachedPointsObj);
	
	///------------------------------------------------------------ tweak attributes
	ATTRIBUTE_AFFECTS(mCulleds,        outputObj);
	ATTRIBUTE_AFFECTS(mCulleds,        cachedPointsObj);

	ATTRIBUTE_AFFECTS(mRenderables,     outputObj);
	ATTRIBUTE_AFFECTS(mRenderables,     cachedPointsObj);

	ATTRIBUTE_AFFECTS(mSourceIDs,       outputObj);
	ATTRIBUTE_AFFECTS(mSourceIDs,       cachedPointsObj);
	
	ATTRIBUTE_AFFECTS(mControlPoints,  outputObj);
	ATTRIBUTE_AFFECTS(mControlPoints,  cachedPointsObj);
		ATTRIBUTE_AFFECTS(mControlValueX,  outputObj);
		ATTRIBUTE_AFFECTS(mControlValueX,  cachedPointsObj);
		ATTRIBUTE_AFFECTS(mControlValueY,  outputObj);
		ATTRIBUTE_AFFECTS(mControlValueY,  cachedPointsObj);
		ATTRIBUTE_AFFECTS(mControlValueZ,  outputObj);
		ATTRIBUTE_AFFECTS(mControlValueZ,  cachedPointsObj);
	
	ATTRIBUTE_AFFECTS(mEulerAngles,       outputObj);
	ATTRIBUTE_AFFECTS(mEulerAngles,       cachedPointsObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueX, outputObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueX, cachedPointsObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueY, outputObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueY, cachedPointsObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueZ, outputObj);
		ATTRIBUTE_AFFECTS(mEulerAngleValueZ, cachedPointsObj);
	
	ATTRIBUTE_AFFECTS(mScales,       outputObj);
	ATTRIBUTE_AFFECTS(mScales,       cachedPointsObj);
		ATTRIBUTE_AFFECTS(mScaleValueX, outputObj);
		ATTRIBUTE_AFFECTS(mScaleValueX, cachedPointsObj);
		ATTRIBUTE_AFFECTS(mScaleValueY, outputObj);
		ATTRIBUTE_AFFECTS(mScaleValueY, cachedPointsObj);
		ATTRIBUTE_AFFECTS(mScaleValueZ, outputObj);
		ATTRIBUTE_AFFECTS(mScaleValueZ, cachedPointsObj);
	
	ATTRIBUTE_AFFECTS(inputObj,        minCornerObj);
	ATTRIBUTE_AFFECTS(inputObj,        maxCornerObj);
	
	//MPxManipContainer::addToManipConnectTable(id);
	return stat;
}

void TaneMPxShape::NodeAddedToModelCallback(MObject &obj, void* clientData)
{
	DECL_LOGGER(false);
	MFnDagNode dagNode(obj);
	TaneMPxShape* node = dynamic_cast<TaneMPxShape*>(dagNode.userNode());
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	node->timeChangeCallbackId_ = MDGMessage::addTimeChangeCallback(TimeChangeCallback, node);
	node->pStateQueue_->push(kNODE_ADDED);
	node->signalDirtyToViewport();
}
void TaneMPxShape::NodeRemovedFromModelCallback(MObject &obj, void* clientData)
{
	DECL_LOGGER(false);
	MFnDagNode dagNode(obj);
	TaneMPxShape* node = dynamic_cast<TaneMPxShape*>(dagNode.userNode());
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	
	if(node->timeChangeCallbackId_)
	{
		MMessage::removeCallback(node->timeChangeCallbackId_);
		node->timeChangeCallbackId_ = 0;
	}
	
	node->pStateQueue_->push(kNODE_DELECETED);
	node->signalDirtyToViewport();
	
}
void TaneMPxShape::TimeChangeCallback(MTime &time, void* clientData)
{
	DECL_LOGGER(false);
	TaneMPxShape* node = reinterpret_cast<TaneMPxShape*>(clientData);
	if(!node)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	node->pStateQueue_->push(kTIME_CHANGED);
	node->signalDirtyToViewport();
}
///------------------------------------------------------------ constructor / destructor
TaneMPxShape::TaneMPxShape()
{
}
TaneMPxShape::~TaneMPxShape()
{
}
///------------------------------------------------------------ virtual functions
void TaneMPxShape::postConstructor()
{
	DECL_LOGGER(false);
	MStatus stat;

	DEFAULT_DAG_POST_CONSTRUCTOR(RegisteredNodes->TaneMPxShape.name);

	///-------------------------------- initialize data
	pStateQueue_        = std::make_shared<StateQueue>();
	pInstancingItem_    = InstancingItem::Create();
	
	///-------------------------------- hide mControlPoints
	MFnAttribute fnAttribute(mControlPoints, &stat);
	fnAttribute.setHidden(true);
	
	///-------------------------------- add callback
	MModelMessage::addNodeAddedToModelCallback(thisMObject(),     NodeAddedToModelCallback);
	MModelMessage::addNodeRemovedFromModelCallback(thisMObject(), NodeRemovedFromModelCallback);
	
	///-------------------------------- set history
	hasHistoryOnCreate_ = false;
	
	///-------------------------------- set renderable
	setRenderable(true);
}

///* connection methods*/
MStatus TaneMPxShape::connectionMade(const MPlug &plug, const MPlug &otherPlug, bool asSrc)
{
	//DECL_LOGGER(maya::TaneMPxShape);
	DECL_LOGGER(false);
	if(plug == inputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		
		MStatus stat;
		MObject thisObj = thisMObject();
		MPlug   historyPlug(thisObj, mHasHistoryOnCreate);
		stat = historyPlug.setValue(true);
		MCHECKERROR(stat, "setValue(mHasHistoryOnCreate)");
	}
	else if(plug == inTimeObj)
	{
	}
	else if(asSrc)
	{
	}
	else
	{
	}
	return MPxNode::connectionMade(plug, otherPlug, asSrc);
}
MStatus TaneMPxShape::connectionBroken(const MPlug &plug, const MPlug &otherPlug, bool asSrc)
{
	DECL_LOGGER(false);
	if(plug == inputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		
		MStatus stat;
		MObject thisObj = thisMObject();
		MPlug   historyPlug(thisObj, mHasHistoryOnCreate);
		stat            = historyPlug.setValue(false);
		MCHECKERROR(stat, "setValue(mHasHistoryOnCreate) error");
	}
	else if(plug == inTimeObj)
	{
	}
	else if(asSrc && otherPlug.node().hasFn(MFn::kShadingEngine) && (plug.attribute() == instObjGroups || plug.attribute() == objectGroups))
	{
	}
	else
	{
	}
	return MPxNode::connectionBroken(plug, otherPlug, asSrc);
	
}
MStatus TaneMPxShape::shouldSave(const MPlug &plug, bool &result)
{
	/// Description
	///    During file save this method is called to determine which
	///    attributes of this node should get written. The default behavior
	///    is to only save attributes whose values differ from the default.
	MStatus status = MS::kSuccess;
	
	if(plug == mCulleds    ||
	   plug == mRenderables ||
	   plug == mSourceIDs   ||
	   plug == mControlPoints || plug == mControlValueX || plug == mControlValueY || plug == mControlValueZ ||
	   plug == mEulerAngles || plug == mEulerAngleValueX || plug == mEulerAngleValueY || plug == mEulerAngleValueZ ||
	   plug == mScales || plug == mScaleValueX || plug == mScaleValueY || plug == mScaleValueZ
	)
	{
		if( hasHistory() )
		{
			/// Calling this will only write tweaks if they are different than the default value.
			status = MPxNode::shouldSave( plug, result );
		}
		else
		{
			result = false;
		}
	}
	else
	{
		status = MPxNode::shouldSave(plug, result);
	}
	return status;
}

///* setDependentsDirty methods*/
MStatus TaneMPxShape::setDependentsDirty(const MPlug &plug, MPlugArray &affectedPlugs)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	//DECL_LOGGER(false);
	MStatus stat;
	///------------------------------------------------------------ display attributes
	if(plug == displayModeObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(kDISPLAY_MODE_CHANGED);
		signalDirtyToViewport();
	}
	///------------------------------------------------------------ renderman attributes
	else if(plug == immediateSubdivideObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(State::kRENDER_ATTRIBUTE_CHANGED);
		signalDirtyToViewport();
	}
	///------------------------------------------------------------ time attributes
	///------------------------------------------------------------ input attributes
	else if(plug == inputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(State::kINPUT_DIRTIED);
		signalDirtyToViewport();
	}
	///------------------------------------------------------------ output attributes
	///------------------------------------------------------------ internal attributes
	else if(plug == colorModeObj    ||
	        plug == colorObj        || plug.parent() == colorObj ||
	        plug == transparencyObj ||
	        plug == drawBoundObj    ||
	        plug == enableIcrObj    ||
	        plug == enableLODObj    ||
	        plug == minDistanceObj  ||
	        plug == maxDistanceObj  ||
	        plug == noLightingObj   ||
	        plug == noWireObj       ||
	        plug == overrideMaskObj
		)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(State::kSHADER_ATTRIBUTE_CHANGED);
		signalDirtyToViewport();
	}
	else if(plug == pointSizeObj)
	{
		//signal to GeometryOverride Only
		signalDirtyToViewport();
	}
	///------------------------------------------------------------ tweak attributes
	else if(plug == mSourceIDs)
	{
		pStateQueue_->push(State::kINPUT_DIRTIED);
		signalDirtyToViewport();
	}
	else if(plug == mControlPoints || plug == mControlValueX     || plug == mControlValueY      || plug == mControlValueZ       ||
	        plug == mEulerAngles   || plug == mEulerAngleValueX  || plug == mEulerAngleValueY   || plug == mEulerAngleValueZ    ||
	        plug == mScales        || plug == mScaleValueX       || plug == mScaleValueY        || plug == mScaleValueZ
		)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(kMANIPULATING_TRANSFORM);
		signalDirtyToViewport();
	}
	else if(plug == mRenderables || plug == mCulleds)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pStateQueue_->push(kMANIPULATING_RENDERABLE);
		signalDirtyToViewport();
	}
	else
	{
		LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
	}
	
	return stat;
	
}

///* Bounding box methods*/
bool TaneMPxShape::isBounded() const
{
	return true;
}
MBoundingBox TaneMPxShape::boundingBox() const
{
	MObject thisNode = thisMObject();
	MPlug   c1Plug(thisNode, minCornerObj);
	MPlug   c2Plug(thisNode, maxCornerObj);
	
	MObject corner1Object;
	MObject corner2Object;
	c1Plug.getValue(corner1Object);
	c2Plug.getValue(corner2Object);
	
	double3 corner1, corner2;
	
	MFnNumericData fnData;
	fnData.setObject(corner1Object);
	fnData.getData(corner1[0], corner1[1], corner1[2]);
	fnData.setObject(corner2Object);
	fnData.getData(corner2[0], corner2[1], corner2[2]);
	
	MPoint corner1Point(corner1[0], corner1[1], corner1[2]);
	MPoint corner2Point(corner2[0], corner2[1], corner2[2]);
	
	
	return MBoundingBox(corner1Point, corner2Point);
}

///* set/get InternalValue method */
bool TaneMPxShape::setInternalValue(const MPlug &plug, const MDataHandle &handle)
{
	//DECL_LOGGER(log::TaneMPxShape);
	DECL_LOGGER(false);
	
	bool isOk = true;
	if((plug == mControlPoints) || (plug == mControlValueX) || (plug == mControlValueY) || (plug == mControlValueZ))
	{
		if(hasHistory())
		{
			LOGGER_DEBUG("hasHistroy() is true");
			verticesUpdated_();
			return MPxNode::setInternalValue(plug, handle);
		}
		else
		{
			LOGGER_DEBUG("hasHistroy() is not true");
			if(plug == mControlPoints && !plug.isArray())
			{
				unsigned index = plug.logicalIndex();
				setValue_(index, handle.asDouble3());
			}
			else if(plug == mControlValueX)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				setValue_(index, 0, handle.asDouble());
			}
			else if(plug == mControlValueY)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				setValue_(index, 1, handle.asDouble());
			}
			else if(plug == mControlValueZ)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				setValue_(index, 2, handle.asDouble());
			}
		}
	}
	else if(plug == mHasHistoryOnCreate)
	{
		hasHistoryOnCreate_ = handle.asBool();
	}
	else
	{
		isOk = MPxSurfaceShape::setInternalValue(plug, handle);
	}
	
	return isOk;
}
bool TaneMPxShape::getInternalValue(const MPlug &plug, MDataHandle &result)
{
	//DECL_LOGGER(log::TaneMPxShape);
	DECL_LOGGER(false);
	
	bool isOk = true;
	if((plug == mControlPoints) || (plug == mControlValueX) || (plug == mControlValueY) || (plug == mControlValueZ))
	{
		if(hasHistory())
		{
			LOGGER_DEBUG("hasHistroy() is true");
			//STDOUT_WARRNING("getInternalValue::hasHistroy() is true");
			return MPxNode::getInternalValue(plug, result);
		}
		else
		{
			LOGGER_DEBUG("getInternalValue::hasHistroy() is not true");
			//STDOUT_WARRNING("getInternalValue::hasHistroy() is not true");
			double val = 0.0;
			if((plug == mControlPoints) && !plug.isArray())
			{
				double3 pnt;
				int     index = plug.logicalIndex();
				getValue_(index, pnt);
				result.set(pnt[0], pnt[1], pnt[2]);
			}
			else if(plug == mControlValueX)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				getValue_(index,0, val);
				result.set(val);
			}
			else if(plug == mControlValueY)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				getValue_(index, 1, val);
				result.set(val);
			}
			else if(plug == mControlValueZ)
			{
				MPlug parentPlug = plug.parent();
				int   index      = parentPlug.logicalIndex();
				//value_(index, 2, val);
				getValue_(index, 2, val);
				result.set(val);
			}
		}
	}
	else if(plug == mHasHistoryOnCreate)
	{
		result.set(hasHistoryOnCreate_);
	}
	else
	{
		isOk = MPxSurfaceShape::getInternalValue(plug, result);
	}
	
	return (isOk);
}

///* Component method */
bool TaneMPxShape::deleteComponents(const MObjectArray &componentList, MDoubleArray &undoInfo)
{
	///TODO::need to be optimized
	
	MStatus stat = MS::kSuccess;
	for (int i = 0; i < componentList.length(); ++i)
	{
		const MObject &comp = componentList[i];
		MFnSingleIndexedComponent fnComponent(componentList[i]);
		if(fnComponent.elementCount())
		{
			if(fnComponent.componentType() == MFn::kMeshVertComponent)
			{
				MIntArray deleteIds;
				fnComponent.getElements(deleteIds);
				
				std::shared_ptr<Points> pOutPoints 	= outPoints();
				int npoints = pOutPoints->size();
				Int32Array& renderableArray = pOutPoints->get<int>(sementic::attribute::renderable);
				
				MDataBlock datablock 		= forceCache();
				MArrayDataHandle rpArrayHnd = datablock.inputArrayValue(mRenderables, &stat);
				MCHECKERROR(stat, "Failed to get rpArrayHnd");
				for(int i = 0; i < deleteIds.length(); ++i)
				{
					const int elemIndex = deleteIds[i];
					rpArrayHnd.jumpToElement(elemIndex);
					MDataHandle renHnd = rpArrayHnd.outputValue();
					bool &ren 		   = renHnd.asBool();
					ren = false;
					
					if(elemIndex < npoints)
						renderableArray[elemIndex] = ren;
				}
				pStateQueue_->push(kMANIPULATING_RENDERABLE);
				updateInstanceItem_(outSources(), outPoints());
				signalDirtyToViewport();
			}
		}
	}
	return true;
}
bool TaneMPxShape::undeleteComponents(const MObjectArray &componentList, MDoubleArray &undoInfo)
{
	MStatus stat = MS::kSuccess;
	for (int i = 0; i < componentList.length(); ++i)
	{
		const MObject &comp = componentList[i];
		MFnSingleIndexedComponent fnComponent(componentList[i]);
		if(fnComponent.elementCount())
		{
			if(fnComponent.componentType() == MFn::kMeshVertComponent)
			{
				MIntArray unDeleteIds;
				fnComponent.getElements(unDeleteIds);
				
				std::shared_ptr<Points> pOutPoints 	= outPoints();
				int npoints = pOutPoints->size();
				Int32Array& renderableArray = pOutPoints->get<int>(sementic::attribute::renderable);
				
				
				MDataBlock datablock 		= forceCache();
				MArrayDataHandle rpArrayHnd = datablock.inputArrayValue(mRenderables, &stat);
				MCHECKERROR(stat, "Failed to get rpArrayHnd");
				for(int i = 0; i < unDeleteIds.length(); ++i)
				{
					const int elemIndex = unDeleteIds[i];
					rpArrayHnd.jumpToElement(elemIndex);
					
					MDataHandle renHnd = rpArrayHnd.outputValue();
					bool &ren 		   = renHnd.asBool();
					ren = true;
					
					if(elemIndex < npoints)
						renderableArray[elemIndex] = ren;
				}
				pStateQueue_->push(kMANIPULATING_RENDERABLE);
				updateInstanceItem_(outSources(), outPoints());
				signalDirtyToViewport();
			}
		}
	}
	return true;
}

///* Compute */
MStatus TaneMPxShape::compute(const MPlug &plug, MDataBlock &datablock)
{
	//DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	if(plug == outputObj || plug == cachedPointsObj)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		MDataHandle inMessageHnd = datablock.inputValue(inMessageObj, &stat);
		MessageMPxData* pInMessageMPxData = dynamic_cast<MessageMPxData*>(inMessageHnd.asPluginData());
		if(!pInMessageMPxData || (pInMessageMPxData && !pInMessageMPxData->data()))
		{
			LOGGER_ERROR("Failed to get valid pInMessageMPxData")
			return MS::kFailure;
		}
		
		std::shared_ptr<Message> pInMsg = pInMessageMPxData->data();
		LOGGER_DEBUG("templetType = %d", pInMsg->templet);
		
		while(!pInMsg->msgQueue.empty())
		{
			switch(pInMsg->msgQueue.top())
			{
				case Message::kBODY_DIRTY:
				LOGGER_IN("Message::kBODY_DIRTY");
				{
					//stat = computeInBody_(plug, datablock);
					//MCHECKERRORNORET(stat, "Failed to compute inBody");
				}
				LOGGER_OUT();
				break;
				case Message::kSOURCE_DIRTY:
				LOGGER_IN("Message::kSOURCE_DIRTY");
				{
					stat = computeInSources_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute inSources");
				}
				LOGGER_OUT();
				break;
				case Message::kPOINT_DIRTY:
				LOGGER_IN("Message::kPOINT_DIRTY");
				{
					stat = computeInPoints_(plug, datablock);
					MCHECKERROR(stat, "Failed to compute inPoints");
				}
				LOGGER_OUT();
				break;
			}
			pInMsg->msgQueue.pop();
		}
		
		//stat = computeInBody_(plug, datablock);
		//MCHECKERRORNORET(stat, "Failed to compute inBody");
		//stat = computeInSources_(plug, datablock);
		//MCHECKERROR(stat, "Failed to compute inSources");
		//stat = computeInPoints_(plug, datablock);
		//MCHECKERROR(stat, "Failed to compute inPoints");
		
		stat = datablock.setClean(plug);
		MCHECKERROR(stat, "Failed to set clean plug, %s", plug.info().asChar());
		
		stat = computeOutput_(plug, datablock);
		MCHECKERROR(stat, "Failed to compute output");
		
		MDataHandle outMessageHnd = datablock.outputValue(outMessageObj, &stat);
		MCHECKERROR(stat, "Failed to get inMessageHnd");
		stat = outMessageHnd.set(pInMessageMPxData);
		MCHECKERROR(stat, "Failed to set pInMessageMPxData");
	
	}
	else if(plug == minCornerObj || plug.parent() == minCornerObj ||
	        plug == maxCornerObj || plug.parent() == maxCornerObj
		)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);

		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		stat = computeBoundingBox_(datablock);
		MCHECKERROR(stat, "Failed to compute boundingbox");
	}
	else
	{
		//LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
		stat = MS::kUnknownParameter;
	}
	
	
	return stat;
}
///* Support the translate/rotate/scale tool (components) */
void TaneMPxShape::transformUsing(const MMatrix &matrix, const MObjectArray &componentList)
{
	transformUsing(matrix, componentList, MPxSurfaceShape::kNoPointCaching, NULL);
}
void TaneMPxShape::transformUsing(const MMatrix &mat, const MObjectArray &componentList, MVertexCachingMode cachingMode,MPointArray* pointCache)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);

	MStatus                 stat;
	MStringArray results;
	stat 					= MGlobal::executeCommand("currentCtx", results);
	MString currentCtx 		= results[0];
	
	std::shared_ptr<Points> pointsPtr 	= outPoints();
	Vec3f* 	orientationPtr              = pointsPtr->data<Vec3f>(sementic::attribute::eulerAngle);
	Vec3f* 	scalePtr 		            = pointsPtr->data<Vec3f>(sementic::attribute::scale);
	
	/// Create cachingMode boolean values for clearer reading of conditional code below
	bool savePoints    = (cachingMode == MPxSurfaceShape::kSavePoints);		//this flag will be set to "save" initially
	bool restorePoints = (cachingMode == MPxSurfaceShape::kRestorePoints);
	
	unsigned int i   	= 0;
	unsigned int j 		= 0;
	unsigned int len 	= componentList.length();
	
	/// restore points when undo
	if(restorePoints)
	{
		unsigned int cacheLen = (pointCache->length());
		if(len > 0)
		{
			for (i = 0; i < len && j < cacheLen; i++)
			{
				MObject comp = componentList[i];
				
				MFnSingleIndexedComponent fnComp(comp);
				int elemCount = fnComp.elementCount();
				for (int idx = 0; idx < elemCount && j < cacheLen; idx++, j+=3)
				{
					int elemIndex = fnComp.element(idx);
					(*pointsPtr)[elemIndex][0]      = (*pointCache)[j + 0][0];
					(*pointsPtr)[elemIndex][1]      = (*pointCache)[j + 0][1];
					(*pointsPtr)[elemIndex][2]      = (*pointCache)[j + 0][2];
					orientationPtr[elemIndex][0]    = (*pointCache)[j + 1][0];
					orientationPtr[elemIndex][1]    = (*pointCache)[j + 1][1];
					orientationPtr[elemIndex][2]    = (*pointCache)[j + 1][2];
					scalePtr[elemIndex][0]		    = (*pointCache)[j + 2][0];
					scalePtr[elemIndex][1]		    = (*pointCache)[j + 2][1];
					scalePtr[elemIndex][2]		    = (*pointCache)[j + 2][2];
				}
			}
		}
		else
		{
			
			len = pointsPtr->size();
			for (unsigned int idx = 0; idx < len && j < cacheLen; idx++, j+=3)
			{
				(*pointsPtr)[idx][0]    = (*pointCache)[j + 0][0];
				(*pointsPtr)[idx][1]    = (*pointCache)[j + 0][1];
				(*pointsPtr)[idx][2]    = (*pointCache)[j + 0][2];
				orientationPtr[idx][0] 	= (*pointCache)[j + 1][0];
				orientationPtr[idx][1] 	= (*pointCache)[j + 1][1];
				orientationPtr[idx][2] 	= (*pointCache)[j + 1][2];
				scalePtr[idx][0]		= (*pointCache)[j + 2][0];
				scalePtr[idx][1]		= (*pointCache)[j + 2][1];
				scalePtr[idx][2]		= (*pointCache)[j + 2][2];
			}
		}
	}
	else
	{
		
		if(len > 0)
		{
			
			for (i = 0; i < len; i++)
			{
				MObject                   comp      = componentList[i];
				MFnSingleIndexedComponent fnComp(comp);
				int                       elemCount = fnComp.elementCount();
				
				//set size increment of pointCache
				if(savePoints && 0 == i)
				{
					//pointCache->setSizeIncrement(elemCount);
					pointCache->setSizeIncrement(elemCount * 3);
				}
				
				for (int idx = 0; idx < elemCount; idx++)
				{
					int elemIndex = fnComp.element(idx);
					if(savePoints)
					{
						//save translation
						pointCache->append(MPoint((*pointsPtr)[elemIndex][0],
						                          (*pointsPtr)[elemIndex][1],
						                          (*pointsPtr)[elemIndex][2]));
						//save orientation
						pointCache->append(MPoint(orientationPtr[elemIndex][0],
						                          orientationPtr[elemIndex][1],
						                          orientationPtr[elemIndex][2]));
						//save scale
						pointCache->append(MPoint(scalePtr[elemIndex][0],
						                          scalePtr[elemIndex][1],
						                          scalePtr[elemIndex][2]));
						
						unsigned int cacheLen = (pointCache->length());
						
					}
					MTransformationMatrix mTransformationMatrix(mat);
					if(currentCtx == "moveSuperContext")
					{
						MVector t = mTransformationMatrix.getTranslation(MSpace::kWorld, &stat);
						(*pointsPtr)[elemIndex][0] += t[0];
						(*pointsPtr)[elemIndex][1] += t[1];
						(*pointsPtr)[elemIndex][2] += t[2];
					}
					if(currentCtx == "RotateSuperContext")
					{
						MEulerRotation 	r = mTransformationMatrix.eulerRotation();
						orientationPtr[elemIndex][0] += r[0];
						orientationPtr[elemIndex][1] += r[1];
						orientationPtr[elemIndex][2] += r[2];
						
					}
					if(currentCtx == "scaleSuperContext")
					{
						double  s[3];
						mTransformationMatrix.getScale(s, MSpace::kWorld);
						
						scalePtr[elemIndex][0] *= s[0];
						scalePtr[elemIndex][1] *= s[1];
						scalePtr[elemIndex][2] *= s[2];
					}
					
					//TODO::set normal
					//(*data)[elemIndex] *= mat;
					//geomPtr->normals[idx] = geomPtr->normals[idx].transformAsNormal(mat);
				}
			}
		}
		else
		{
			len = pointsPtr->size();
			//set size increment of pointCache
			if(savePoints)
			{
				pointCache->setSizeIncrement(len * 3);
			}
			
			for (unsigned int idx = 0; idx < len; ++idx)
			{
				if(savePoints)
				{
					//save translation
					pointCache->append(MPoint((*pointsPtr)[idx][0],
					                          (*pointsPtr)[idx][1],
					                          (*pointsPtr)[idx][2]));
					//save orientation
					pointCache->append(MPoint(orientationPtr[idx][0],
					                          orientationPtr[idx][1],
					                          orientationPtr[idx][2]));
					//save scale
					pointCache->append(MPoint(scalePtr[idx][0],
					                          scalePtr[idx][1],
					                          scalePtr[idx][2]));
					unsigned int cacheLen = (pointCache->length());
				}
				
				MTransformationMatrix mTransformationMatrix(mat);
				if(currentCtx == "moveSuperContext")
				{
					MVector t = mTransformationMatrix.getTranslation(MSpace::kWorld, &stat);
					(*pointsPtr)[idx][0] += t[0];
					(*pointsPtr)[idx][1] += t[1];
					(*pointsPtr)[idx][2] += t[2];
				}
				if(currentCtx == "RotateSuperContext")
				{
					MEulerRotation 	r = mTransformationMatrix.eulerRotation();
					orientationPtr[idx][0] += r[0];
					orientationPtr[idx][1] += r[1];
					orientationPtr[idx][2] += r[2];
					
				}
				if(currentCtx == "scaleSuperContext")
				{
					double  s[3];
					mTransformationMatrix.getScale(s, MSpace::kWorld);
					
					scalePtr[idx][0] *= s[0];
					scalePtr[idx][1] *= s[1];
					scalePtr[idx][2] *= s[2];
				}
				
				//TODO::set normal
				//(*data)[idx] *= mat;
				//geomPtr->normals[idx] = geomPtr->normals[idx].transformAsNormal(mat);
				
			}
		}
	}
	
	/// Update
	updateTweaks_(pointsPtr, componentList);
	MDataBlock block = forceCache();
	stat = computeBoundingBox_(block);
	MCHECKERRORNORET(stat, "Failed to compute boundingBox");
	signalDirtyToViewport();
	
	
}

///* Support deformers (components) */
void TaneMPxShape::weightedTransformUsing(const MTransformationMatrix &xform, const MMatrix* space, const MObjectArray &componentList, MVertexCachingMode cachingMode, MPointArray* pointCache, const MPlane* freezePlane)
{
	MGlobal::displayWarning("not support soft selection mode");
}
void TaneMPxShape::weightedTweakUsing(const MTransformationMatrix &xform, const MMatrix* space, const MObjectArray &componentList, MVertexCachingMode cachingMode, MPointArray* pointCache, const MPlane* freezePlane, MArrayDataHandle &handle)
{
	MGlobal::displayWarning("not support soft selection mode");
}

///* Attribute to component (components) */
void TaneMPxShape::componentToPlugs(MObject &component, MSelectionList &list) const
{
	//DECL_LOGGER(log::TaneMPxShape);
	DECL_LOGGER(false);
	if(component.hasFn(MFn::kSingleIndexedComponent))
	{
		MStatus stat;
		MFnSingleIndexedComponent fnVtxComp(component);
		MObject                   thisNode = thisMObject();

		MPlug cpPlug(thisNode, mControlPoints);
		MPlug srPlug(thisNode, mEulerAngles);
		MPlug ssPlug(thisNode, mScales);
		
		int len = fnVtxComp.elementCount();
		LOGGER_DEBUG("len = %d", len);
		
		MIntArray elementsArray;
		stat = fnVtxComp.getElements(elementsArray);

		for (int i = 0; i < len; i++)
		{
			int elemIndex = fnVtxComp.element(i);
			cpPlug.selectAncestorLogicalIndex(fnVtxComp.element(i), cpPlug.attribute());
			list.add(cpPlug);
			srPlug.selectAncestorLogicalIndex(fnVtxComp.element(i), srPlug.attribute());
			list.add(srPlug);
			ssPlug.selectAncestorLogicalIndex(fnVtxComp.element(i), ssPlug.attribute());
			list.add(ssPlug);
		}
	}
	
}

MPxSurfaceShape::MatchResult TaneMPxShape::matchComponent(const MSelectionList &item, const MAttributeSpecArray &spec, MSelectionList &list)
{
	//DECL_LOGGER(log::TaneMPxShape);
	DECL_LOGGER(false);

	MPxSurfaceShape::MatchResult result   = MPxSurfaceShape::kMatchOk;
	MAttributeSpec               attrSpec = spec[0];
	int                          dim      = attrSpec.dimensions();

	/// Look for attributes specifications of the form : vtx[ index ] vtx[ lower:upper ]
	if((1 == spec.length()) && (dim > 0) && (attrSpec.name() == "vtx"))
	{
		int             numVertices = outPoints()->size();
		MAttributeIndex attrIndex   = attrSpec[0];

		int upper = 0;
		int lower = 0;
		if(attrIndex.hasLowerBound())
		{
			attrIndex.getLower(lower);
		}
		if(attrIndex.hasUpperBound())
		{
			attrIndex.getUpper(upper);
		}

		/// Check the attribute index range is valid
		if((lower > upper) || (upper >= numVertices))
		{
			result = MPxSurfaceShape::kMatchInvalidAttributeRange;
		}
		else
		{
			MDagPath path;
			item.getDagPath(0, path);
			MFnSingleIndexedComponent fnVtxComp;
			MObject vtxComp = fnVtxComp.create(MFn::kMeshVertComponent);
			
			LOGGER_DEBUG("lower = %d", lower);
			LOGGER_DEBUG("upper = %d", upper);

			for (int i = lower; i <= upper; i++)
			{
				fnVtxComp.addElement(i);
			}
			list.add(path, vtxComp);
		}
	}
	else
	{
		/// Pass this to the parent class
		return MPxSurfaceShape::matchComponent(item, spec, list);
	}

	return result;
}
bool TaneMPxShape::match(const MSelectionMask &mask, const MObjectArray &componentList) const
{
	//DECL_LOGGER(log::TaneMPxShape);
	DECL_LOGGER(false);
	bool result = false;

	if(componentList.length() == 0)
	{
		result = mask.intersects(MSelectionMask::kSelectMeshes);
	}
	else
	{
		for (int i = 0; i < (int) componentList.length(); i++)
		{
			if((componentList[i].apiType() == MFn::kMeshVertComponent) && (mask.intersects(MSelectionMask::kSelectMeshVerts)))
			{
				result = true;
				break;
			}
		}
	}

	return result;
}

///* Set SelectionMask */
MSelectionMask TaneMPxShape::getShapeSelectionMask() const
{
	MSelectionMask::SelectionType selType = MSelectionMask::kSelectMeshes;
	return MSelectionMask(selType);
}
MSelectionMask TaneMPxShape::getComponentSelectionMask() const
{
	MSelectionMask retVal(MSelectionMask::kSelectMeshVerts);
	return retVal;
}

/* Associates a user defined iterator with the shape (components) */
MPxGeometryIterator* TaneMPxShape::geometryIteratorSetup(MObjectArray &componentList, MObject &components, bool forReadOnly)
{
	//DECL_LOGGER(log::TaneMPxSurfaceShape);
	PointsIterator* result = nullptr;
	if(components.isNull())
		result = new PointsIterator(outPoints().get(), componentList);
	else
		result = new PointsIterator(outPoints().get(), components);
	return result;
}
bool TaneMPxShape::acceptsGeometryIterator(bool writeable)
{
	return true;
}
bool TaneMPxShape::acceptsGeometryIterator(MObject &, bool writeable, bool forReadOnly)
{
	return true;
}

///------------------------------------------------------------ public helper functions
bool TaneMPxShape::hasHistory()
{
	return hasHistoryOnCreate_;
}

std::shared_ptr<Points> TaneMPxShape::outPoints()
{
	MStatus     stat;
	MDataBlock  dataBlock = forceCache();
	MDataHandle outputHnd    = dataBlock.inputValue(outputObj);
	MDataHandle outPointsHnd = outputHnd.child(outPointsObj);
	
	MFnPluginData fnData(outPointsHnd.data());
	PointsMPxData* data = dynamic_cast<PointsMPxData*>(fnData.data(&stat));
	MCHECKERRORNORET(stat, "Failed to get PointsMPxData");
	
	std::shared_ptr<Points> ret = nullptr;
	if(data != nullptr)
	{
		ret = data->data();
	}
	return ret;
}
std::shared_ptr<SourceManager> TaneMPxShape::outSources()
{
	MStatus     stat;
	MDataBlock  dataBlock = forceCache();
	MDataHandle outputHnd    = dataBlock.inputValue(outputObj);
	MDataHandle outSourcesHnd = outputHnd.child(outSourcesObj);
	
	MFnPluginData fnData(outSourcesHnd.data());
	SourcesMPxData* data = dynamic_cast<SourcesMPxData*>(fnData.data(&stat));
	MCHECKERRORNORET(stat, "Failed to get SourcesMPxData");
	
	std::shared_ptr<SourceManager> ret = nullptr;
	if(data != nullptr)
	{
		ret = data->data();
	}
	
	return ret;
}
std::shared_ptr<Message> TaneMPxShape::outMessage()
{
	MStatus     stat;
	MDataBlock  dataBlock       = forceCache();
	MDataHandle outputHnd       = dataBlock.inputValue(outputObj);
	MDataHandle outMessageHnd   = outputHnd.child(outMessageObj);
	
	MFnPluginData fnData(outMessageHnd.data());
	MessageMPxData* data = dynamic_cast<MessageMPxData*>(fnData.data(&stat));
	MCHECKERRORNORET(stat, "Failed to get MessageMPxData");
	
	std::shared_ptr<Message> ret = nullptr;
	if(data != nullptr)
	{
		ret = data->data();
	}
	
	return ret;
}

std::shared_ptr<Points> TaneMPxShape::cachedPoints()
{
	//DECL_LOGGER(log::TaneMPxSurfaceShape);
	
	MDataBlock  dataBlock = forceCache();
	MDataHandle handle    = dataBlock.outputValue(cachedPointsObj);
	
	MStatus       stat;
	MFnPluginData fnData(handle.data());
	PointsMPxData* data = dynamic_cast<PointsMPxData*>(fnData.data(&stat));
	MCHECKERRORNORET(stat, "Failed to get PointsMPxData");
	
	std::shared_ptr<Points> ret = nullptr;
	if(data != nullptr)
	{
		ret = data->data();
	}
	
	
	return ret;
}
std::shared_ptr<InstancingItem> TaneMPxShape::instancingItem()
{
	return pInstancingItem_;
}
std::shared_ptr<TaneMPxShape::StateQueue> TaneMPxShape::stateQueue()
{
	return pStateQueue_;
}

void TaneMPxShape::signalDirtyToViewport()
{
	//DECL_LOGGER(logging::maya->TaneMPxShape);
	DECL_LOGGER(false);
	//fShapeDirty = true;

	MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());

	MStatus stat;
	MFnDagNode dagNodeFn(thisMObject(), &stat);             //MCHECKERRORNORET(stat, "Failed to create dagNodeFn");
	MObject parentObj = dagNodeFn.parent(0, &stat);         //MCHECKERRORNORET(stat, "Failed to get parent object ");
	MFnDependencyNode dependencyNodeFn(parentObj, &stat);   //MCHECKERRORNORET(stat, "Failed to create dependencyNodeFn");
	
	if(dependencyNodeFn.typeId() != RegisteredNodes->TaneMPxTransform.id)
	{
		LOGGER_WARNING("dependencyNodeFn.typeId() != TN_TANE_MPXTRANSFORM_ID");
		return;
	}
	
	MHWRender::MRenderer::setGeometryDrawDirty(parentObj);
}

///------------------------------------------------------------ private helper functions
bool TaneMPxShape::setValue_(int pntInd, int vlInd, double val)
{
	DECL_LOGGER(false);
	bool result = false;
	
	TaneMPxShape* nonConstThis = reinterpret_cast<TaneMPxShape*>(this);
	std::shared_ptr<Points> geomPtr = nonConstThis->cachedPoints();
	
	
	if(NULL != geomPtr)
	{
		(*geomPtr)[pntInd][vlInd] = val;
		
		result = true;
	}
	
	verticesUpdated_();
	
	return result;
}
bool TaneMPxShape::setValue_(int pntInd, const double3 &val)
{
	DECL_LOGGER(false);
	bool result = false;
	
	TaneMPxShape* nonConstThis = reinterpret_cast<TaneMPxShape*>(this);
	std::shared_ptr<Points> geomPtr = nonConstThis->cachedPoints();
	if(NULL != geomPtr)
	{
		(*geomPtr)[pntInd][0] = val[0];
		(*geomPtr)[pntInd][1] = val[1];
		(*geomPtr)[pntInd][2] = val[2];
		result = true;
	}
	
	verticesUpdated_();
	
	return result;
}
bool TaneMPxShape::getValue_(int pntInd, int vlInd, double &val) const
{
	bool result = false;
	
	TaneMPxShape* nonConstThis = const_cast<TaneMPxShape*>(this);
	std::shared_ptr<Points> geomPtr = nonConstThis->cachedPoints();
	
	
	if(NULL != geomPtr)
	{
		//MPoint point = (*geomPtr)[pntInd];
		//val = point[vlInd];
		
		val    = (*geomPtr)[pntInd][vlInd];
		result = true;
	}
	
	return result;
}
bool TaneMPxShape::getValue_(int pntInd, double3 &val) const
{
	bool result = false;
	
	TaneMPxShape* nonConstThis = const_cast<TaneMPxShape*>(this);
	std::shared_ptr<Points> geomPtr = nonConstThis->cachedPoints();
	
	
	if(NULL != geomPtr)
	{
		//MPoint point = (*geomPtr)[pntInd];
		//val = point;
		
		val[0] = (*geomPtr)[pntInd][0];
		val[1] = (*geomPtr)[pntInd][1];
		val[2] = (*geomPtr)[pntInd][2];
		
		result = true;
	}
	
	return result;
}

MStatus TaneMPxShape::computeInMessage_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	/* get inMessage */
	MDataHandle inMessageHnd = datablock.inputValue(inMessageObj, &stat);
	MCHECKERROR(stat, "Failed to get inMessageHnd ");
	MessageMPxData* inMessageMPxData = dynamic_cast<MessageMPxData*>(inMessageHnd.asPluginData());
	if(!inMessageMPxData)
	{
		LOGGER_ERROR("outPointsMPxData is invalid");
		return MS::kFailure;
	}
	
	return stat;
}
MStatus TaneMPxShape::computeInBody_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	MDataHandle inBodyHnd = datablock.inputValue(inBodyObj, &stat);
	MCHECKERROR(stat, "Failed to get inBodyHnd");
	
	//MDataHandle outBodyHnd = datablock.outputValue(outBodyObj, &stat);
	//MCHECKERROR(stat, "Failed to get outBodyHnd");
	//
	//stat = outBodyHnd.set(outBodyHnd.asMesh());
	//MCHECKERROR(stat, "Failed to set outBodyHnd");
	
	return stat;
}
MStatus TaneMPxShape::computeInSources_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	
	MDataHandle inSourcesHnd = datablock.inputValue(inSourcesObj, &stat);       MCHECKERROR(stat, "Failed to get inSourcesHnd");
	MDataHandle outSourcesHnd = datablock.outputValue(outSourcesObj, &stat);    MCHECKERROR(stat, "Failed to get outSourcesHnd");
	stat = outSourcesHnd.set(inSourcesHnd.asPluginData());
	MCHECKERROR(stat, "Failed to set outSourcesHnd");
	
	return stat;
}
MStatus TaneMPxShape::computeInPoints_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	
	/// Create the cachedPoints
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(PointsMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create cached PointsMpxData");
	PointsMPxData* pCachedPointsMPxData = (PointsMPxData*) fnDataCreator.data(&stat);
	MCHECKERROR(stat, "Failed to get cached PointsMpxData");
	
	/// Get InPoints
	MDataHandle inPointsHnd = datablock.inputValue(inPointsObj, &stat);
	MCHECKERROR(stat, "Failed to get inPoints Handle");
	PointsMPxData* pInPointsMPxData = dynamic_cast<PointsMPxData*>(inPointsHnd.asPluginData());
	
	if(pInPointsMPxData  == nullptr || (pInPointsMPxData && !pInPointsMPxData->data()))
	{
		LOGGER_ERROR("Failed to get valid pInPointsMPxData");
		return MS::kFailure;
	}
	
	std::shared_ptr<Points> pInPoints = pInPointsMPxData->data();
	LOGGER_DEBUG("pInPoints's.size() = %u", pInPoints->size());
	
	std::shared_ptr<Points> pCachedPoints = pCachedPointsMPxData->data();
	*pCachedPoints = *pInPoints;
	
	MDataHandle cachedPointsHnd = datablock.outputValue(cachedPointsObj, &stat);
	MCHECKERROR(stat, "Failed to get cachedPointsHnd");
	stat = cachedPointsHnd.set(pCachedPointsMPxData);
	
	
	return stat;
}
MStatus TaneMPxShape::computeOutput_(const MPlug &plug, MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	
	MDataHandle cachedHandle = datablock.outputValue(cachedPointsObj, &stat);
	MCHECKERROR(stat, "Failed to get cachedSurface");
	
	PointsMPxData* cached = dynamic_cast<PointsMPxData*>(cachedHandle.asPluginData());
	if(!cached)
	{
		LOGGER_ERROR("Failed to get cached points");
		return MS::kFailure;
	}
	
	/* append added point */
	LOGGER_IN("append added Points");
	{
		MDataHandle addedPointHnd = datablock.outputValue(addedPointsObj, &stat);
		MCHECKERROR(stat, "Failed to get addedPointHnd");
		
		PointsMPxData* pMPxData = dynamic_cast<PointsMPxData*>(addedPointHnd.asPluginData());
		if(pMPxData  == nullptr || (pMPxData && !pMPxData->data()))
		{
			LOGGER_ERROR("Failed to get valid pMPxData");
			return MS::kFailure;
		}
		const std::shared_ptr<Points>& pData = pMPxData->data();
		if(pData->size() > 0)
		{
			cached->data()->append(*pData);
		}
	}
	LOGGER_OUT();
	
	
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(PointsMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create PointMpxData");
	
	PointsMPxData* pOutPointsMPxData = dynamic_cast<PointsMPxData*>(fnDataCreator.data(&stat));
	MCHECKERROR(stat, "Failed to get pOutPointsMPxData");
	
	/* copy points data */
	*pOutPointsMPxData->data() = *cached->data();
	
	/* apply tweaks */
	stat = applyTweaks_(datablock, cached->data(), pOutPointsMPxData->data());
	
	MDataHandle outPointsHandle = datablock.outputValue(outPointsObj);
	outPointsHandle.set(pOutPointsMPxData);
	
	/// Update InstanceItem
	const std::shared_ptr<SourceManager> pSourceMgr = outSources();
	const std::shared_ptr<Points>        pOutPoints = outPoints();
	
	stat = updateInstanceItem_(pSourceMgr, pOutPoints);
	MCHECKERROR(stat, "Failed to update InstanceItem");
	
	/// Update numPoints
	stat = updateNumPoints(datablock, pOutPoints->size());
	MCHECKERROR(stat, "Failed to update numPoints");
	
	/// Update bounding box
	stat = computeBoundingBox_(datablock);
	
	
	return stat;
}
MStatus TaneMPxShape::updateTweaks_(const std::shared_ptr<Points> pPoints, const MObjectArray &componentList)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus      stat;
	unsigned int len = componentList.length();
	
	MDataBlock datablock = forceCache();
	
	MDataHandle cachedHandle = datablock.outputValue(cachedPointsObj, &stat);
	MCHECKERRORNORET(stat, "Failed to get cachedPoints");
	PointsMPxData* cached 					= (PointsMPxData*) cachedHandle.asPluginData();
	std::shared_ptr<Points> cachedPointPtr 	= cached->data();
	
	MDataHandle siHandle = datablock.outputValue(mSourceIDs, &stat);
	MCHECKERRORNORET(stat, "get sidHandle");
	
	MDataHandle tHandle = datablock.outputValue(mControlPoints, &stat);
	MCHECKERRORNORET(stat, "get tHandle");
	
	MDataHandle rHandle = datablock.outputValue(mEulerAngles, &stat);
	MCHECKERRORNORET(stat, "get rHandle");
	
	MDataHandle sHandle = datablock.outputValue(mScales, &stat);
	MCHECKERRORNORET(stat, "get sHandle");
	
	
	int*        cachedSidPtr 	    = cachedPointPtr->data<int>(sementic::attribute::sid);
	Point3f*    cachedPositionPtr 	= cachedPointPtr->data();
	Vec3f* 	    cachedOreintPtr		= cachedPointPtr->data<Vec3f>(sementic::attribute::eulerAngle);
	Vec3f* 		cachedScalePtr		= cachedPointPtr->data<Vec3f>(sementic::attribute::scale);
	
	int*        sidPtr 	            = pPoints->data<int>(sementic::attribute::sid);
	Point3f*    positionPtr  		= pPoints->data();
	Vec3f* 	    orientPtr 			= pPoints->data<Vec3f>(sementic::attribute::eulerAngle);
	Vec3f* 		scalePtr 			= pPoints->data<Vec3f>(sementic::attribute::scale);
	
	if(hasHistory() && (NULL != cached))
	{
		MArrayDataHandle siArrayHnd(siHandle, &stat);
		MCHECKERRORNORET(stat, "get siArrayHnd error");

		MArrayDataHandle cpArrayHnd(tHandle, &stat);
		MCHECKERRORNORET(stat, "get cpHandle error");
		
		MArrayDataHandle srArrayHnd(rHandle, &stat);
		MCHECKERRORNORET(stat, "get srArrayHnd error");
		
		MArrayDataHandle ssArrayHnd(sHandle , &stat);
		MCHECKERRORNORET(stat, "get ssArrayHnd error");
		
		
		/// Loop through the component list and transform each vertex.
		for (unsigned int i = 0; i < len; i++)
		{
			MObject comp = componentList[i];
			
			MFnSingleIndexedComponent fnComp(comp);
			int elemCount = fnComp.elementCount();
			for (int idx = 0; idx < elemCount; idx++)
			{
				int elemIndex = fnComp.element(idx);
				
				/// apply the tweaks to sid
				{
					siArrayHnd.jumpToElement(elemIndex);
					MDataHandle siHandle = siArrayHnd.outputValue();
					int& sid     = siHandle.asInt();
					sid = sidPtr[elemIndex] - cachedSidPtr[elemIndex];
				}
				/// apply the tweaks to the positions
				{
					cpArrayHnd.jumpToElement(elemIndex);
					MDataHandle cpHandle = cpArrayHnd.outputValue();
					double3     &pnt     = cpHandle.asDouble3();
					
					pnt[0] = (positionPtr[elemIndex][0] - cachedPositionPtr[elemIndex][0]);
					pnt[1] = (positionPtr[elemIndex][1] - cachedPositionPtr[elemIndex][1]);
					pnt[2] = (positionPtr[elemIndex][2] - cachedPositionPtr[elemIndex][2]);
					
					//LOGGER_DEBUG("outPosition  [%d] = (%5.2f, %5.2f, %5.2f)", elemIndex, positionPtr[elemIndex][0], positionPtr[elemIndex][1], positionPtr[elemIndex][2]);
					//LOGGER_DEBUG("cachedPositon[%d] = (%5.2f, %5.2f, %5.2f)", elemIndex, cachedPositionPtr[elemIndex][0], cachedPositionPtr[elemIndex][1], cachedPositionPtr[elemIndex][2]);
					//LOGGER_DEBUG("pnt          [%d] = (%5.2f, %5.2f, %5.2f)", elemIndex, pnt[0], pnt[1], pnt[2]);
				}
				/// apply the tweaks to source rotation
				{
					srArrayHnd.jumpToElement(elemIndex);
					MDataHandle srHandle  = srArrayHnd.outputValue();
					double3     &rot      = srHandle.asDouble3();
					
					rot[0] = (orientPtr[elemIndex][0] - cachedOreintPtr[elemIndex][0]);
					rot[1] = (orientPtr[elemIndex][1] - cachedOreintPtr[elemIndex][1]);
					rot[2] = (orientPtr[elemIndex][2] - cachedOreintPtr[elemIndex][2]);
					
				}
				/// apply the tweaks to source scale
				{
					ssArrayHnd.jumpToElement(elemIndex);
					MDataHandle ssHandle  = ssArrayHnd.outputValue();
					double3     &sc       = ssHandle.asDouble3();
					
					sc[0] = (scalePtr[elemIndex][0] - cachedScalePtr[elemIndex][0]);
					sc[1] = (scalePtr[elemIndex][1] - cachedScalePtr[elemIndex][1]);
					sc[2] = (scalePtr[elemIndex][2] - cachedScalePtr[elemIndex][2]);
				}
			}
		}
	}
	MObject thisObj = thisMObject();
	
	MPlug pSIs(thisObj, mSourceIDs);
	pSIs.setValue(siHandle);
	
	MPlug pCPs(thisObj , mControlPoints);
	pCPs.setValue(tHandle);
	
	MPlug pSRs(thisObj, mEulerAngles);
	pSRs.setValue(rHandle);
	
	MPlug pSSs(thisObj, mScales);
	pSSs.setValue(sHandle);
	
	/// Moving vertices will likely change the bounding box.
	computeBoundingBox_(datablock);
	
	/// Tell maya the bounding box for this object has changed and thus "boundingBox()" needs to be called.
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	
	/// Signal to the viewport that it needs to update the object
	signalDirtyToViewport();
	
}
MStatus TaneMPxShape::applyTweaks_(MDataBlock& datablock, const std::shared_ptr<Points> pCachedPoints, const std::shared_ptr<Points> pOutPoints)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat = MS::kSuccess;

	int npoints = pOutPoints->size();
	stat = buildTweakContainers_(datablock, npoints);
	MCHECKERROR(stat, "Failed to build tweak containers");
	
	MArrayDataHandle clArrayHnd = datablock.inputArrayValue(mCulleds, &stat);        MCHECKERROR(stat, "Failed to get clArrayHnd");
	MArrayDataHandle rpArrayHnd = datablock.inputArrayValue(mRenderables, &stat);    MCHECKERROR(stat, "Failed to get rpArrayHnd");
	MArrayDataHandle siArrayHnd = datablock.inputArrayValue(mSourceIDs, &stat);      MCHECKERROR(stat, "Failed to get siArrayHnd");
	MArrayDataHandle cpArrayHnd = datablock.inputArrayValue(mControlPoints, &stat);  MCHECKERROR(stat, "Failed to get cpArrayHnd");
	MArrayDataHandle srArrayHnd = datablock.inputArrayValue(mEulerAngles, &stat);    MCHECKERROR(stat, "Failed to get srArrayHnd");
	MArrayDataHandle ssArrayHnd = datablock.inputArrayValue(mScales, &stat);         MCHECKERROR(stat, "Failed to get sArraysHnd");
	
	/* pCachedPoints */
	LOGGER_DEBUG("get 0");
	const Int32Array  & sidArray0        = pCachedPoints->get<int>(sementic::attribute::sid);
	const Int32Array  & renderableArray0 = pCachedPoints->get<int>(sementic::attribute::renderable);
	const Point3fArray& pointArray0      = pCachedPoints->get();
	const Vec3fArray  & eulerAngleArray0 = pCachedPoints->get<Vec3f>(sementic::attribute::eulerAngle);
	const Vec3fArray  & scaleArray0      = pCachedPoints->get<Vec3f>(sementic::attribute::scale);
	
	/* pOutPoints */
	LOGGER_DEBUG("get 1");
	Int32Array        & sidArray1        = pOutPoints->get<int>(sementic::attribute::sid);
	Int32Array        & renderableArray1 = pOutPoints->get<int>(sementic::attribute::renderable);
	Point3fArray      & pointArray1      = pOutPoints->get();
	Vec3fArray        & eulerAngleArray1 = pOutPoints->get<Vec3f>(sementic::attribute::eulerAngle);
	Vec3fArray        & scaleArray1      = pOutPoints->get<Vec3f>(sementic::attribute::scale);
	
	MDataHandle useFrustumCullingHnd = datablock.outputValue(useFrustumCullingObj, &stat);
	const bool  useFrustumCulling    = useFrustumCullingHnd.asBool();
	
	/* all tweak container has same element count, use mControlPoints elemCounts */
	int elemCount = cpArrayHnd.elementCount();
	for (int idx = 0; idx < elemCount; idx++)
	{
		//cout << "idx = " << idx << endl;
	
		/* apply the tweaks visible */
		{
			bool culled = false;
			if(useFrustumCulling)
			{
				bool culled = false;
				{
					MDataHandle clHnd = clArrayHnd.outputValue(&stat);
					culled = clHnd.asBool();
				}
	
			}
			int         elemIndex = rpArrayHnd.elementIndex(&stat);
			MDataHandle renHnd    = rpArrayHnd.outputValue(&stat);
			bool renderable       = renHnd.asBool();
	
			if(renderable && (!culled))
			{
				if(elemIndex < npoints)
					renderableArray1[elemIndex] = true;
			}
			else
			{
				if(elemIndex < npoints)
					renderableArray1[elemIndex] = false;
			}
	
		}
		/* apply the tweaks source id */
		{
			int         elemIndex = siArrayHnd.elementIndex(&stat);
			MDataHandle sidHandle = siArrayHnd.outputValue(&stat);
			int         sid       = sidHandle.asInt();
			if(elemIndex < npoints)
			{
				//LOGGER_DEBUG("sidHandle[%d] = %d", elemIndex, sid);
				sidArray1[elemIndex] = sidArray0[elemIndex] + sid;
			}
	
		}
		/* apply the tweaks positions */
		{
			int         elemIndex = cpArrayHnd.elementIndex(&stat);
			MDataHandle pntHandle = cpArrayHnd.outputValue(&stat);
			double3     &pnt      = pntHandle.asDouble3();
	
			if(elemIndex < npoints)
			{
				pointArray1[elemIndex][0] = pointArray0[elemIndex][0] + pnt[0];
				pointArray1[elemIndex][1] = pointArray0[elemIndex][1] + pnt[1];
				pointArray1[elemIndex][2] = pointArray0[elemIndex][2] + pnt[2];
			}
	
		}
		/* apply the tweaks source rotation */
		{
			int         elemIndex 	= srArrayHnd.elementIndex();
			MDataHandle rotHnd    	= srArrayHnd.outputValue();
			double3 &rot 			= rotHnd.asDouble3();
	
			if(elemIndex < npoints)
			{
				eulerAngleArray1[elemIndex][0] = eulerAngleArray0[elemIndex][0] + rot[0];
				eulerAngleArray1[elemIndex][1] = eulerAngleArray0[elemIndex][1] + rot[1];
				eulerAngleArray1[elemIndex][2] = eulerAngleArray0[elemIndex][2] + rot[2];
			}
		}
		/* apply the tweaks source scale */
		{
			int         elemIndex = ssArrayHnd.elementIndex();
			MDataHandle ssHandle  = ssArrayHnd.outputValue();
			double3     &sc       = ssHandle.asDouble3();
	
			if(elemIndex < npoints)
			{
				scaleArray1[elemIndex][0] = scaleArray0[elemIndex][0] + sc[0];
				scaleArray1[elemIndex][1] = scaleArray0[elemIndex][1] + sc[1];
				scaleArray1[elemIndex][2] = scaleArray0[elemIndex][2] + sc[2];
	
			}
		}
		clArrayHnd.next();
		rpArrayHnd.next();
		siArrayHnd.next();
		cpArrayHnd.next();
		srArrayHnd.next();
		ssArrayHnd.next();
	}
	return stat;
}
MStatus TaneMPxShape::cleanTweaks_(MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	
	MStatus stat;
	
	////clean mControlPoints
	//MArrayDataHandle cpArrayHnd = datablock.inputArrayValue(mControlPoints, &stat); MCHECKERROR(stat, "error getting cpArrayHnd");
	//MArrayDataHandle visArrayHnd = datablock.inputArrayValue(mVisible, &stat);      MCHECKERROR(stat, "error getting visArrayHnd");
	//MArrayDataHandle rpArrayHnd = datablock.inputArrayValue(mRenderable, &stat);    MCHECKERROR(stat, "error getting rpArrayHnd");
	//MArrayDataHandle srArrayHnd = datablock.inputArrayValue(mOrientation, &stat);   MCHECKERROR(stat, "error getting srArrayHnd");
	//MArrayDataHandle ssArrayHnd = datablock.inputArrayValue(mScale, &stat);         MCHECKERROR(stat, "error getting sArraysHnd");
	//MArrayDataHandle siArrayHnd = datablock.inputArrayValue(mSourceID, &stat);      MCHECKERROR(stat, "error getting siArrayHnd");
	//
	//cpArrayHnd.setAllClean();
	//visArrayHnd.setAllClean();
	//rpArrayHnd.setAllClean();
	//srArrayHnd.setAllClean();
	//ssArrayHnd.setAllClean();
	//siArrayHnd.setAllClean();
	
	return stat;
}
MStatus TaneMPxShape::clearTweaks_(MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	
	MStatus stat;
	
	//MDataHandle clearTweaksHnd = datablock.inputValue(clearTweaksObj, &stat);
	//MCHECKERROR(stat, "Failed to get clearTweaksHnd");
	//if(!clearTweaksHnd.asBool())
	//{
	//	return stat;
	//}
	//MArrayDataHandle cpArrayHnd = datablock.outputArrayValue(mControlPoints, &stat);    MCHECKERROR(stat, "Faield to get cpArrayHnd");
	//MArrayDataHandle visArrayHnd = datablock.outputArrayValue(mVisible, &stat);         MCHECKERROR(stat, "Faield to get visArrayHnd");
	//MArrayDataHandle rpArrayHnd = datablock.outputArrayValue(mRenderable, &stat);       MCHECKERROR(stat, "Faield to get rpArrayHnd ");
	//MArrayDataHandle srArrayHnd = datablock.outputArrayValue(mOrientation, &stat);      MCHECKERROR(stat, "Faield to get srArrayHnd");
	//MArrayDataHandle ssArrayHnd = datablock.outputArrayValue(mScale, &stat);            MCHECKERROR(stat, "Faield to get ssArrayHnd");
	//MArrayDataHandle siArrayHnd = datablock.outputArrayValue(mSourceID, &stat);         MCHECKERROR(stat, "Faield to get siArrayHnd");
	//
	//MArrayDataBuilder cpBuilder(mControlPoints, 0, &stat);  MCHECKERROR(stat, "Failed to create cpBuilder");
	////MArrayDataBuilder visBuilder(mVisible, 0, &stat);     //MCHECKERROR(stat, "Failed to create visBuilder");
	//MArrayDataBuilder rpBuilder(mRenderable, 0, &stat);     MCHECKERROR(stat, "Failed to create rpBuilder");
	//MArrayDataBuilder srBuilder(mOrientation, 0, &stat);    MCHECKERROR(stat, "Failed to create srBuilder");
	//MArrayDataBuilder ssBuilder(mScale, 0, &stat);          MCHECKERROR(stat, "Failed to create ssBuilder");
	//MArrayDataBuilder siBuilder(mSourceID, 0, &stat);       MCHECKERROR(stat, "Failed to create siBuilder");
	//
	//stat = cpArrayHnd.set(cpBuilder);       MCHECKERROR(stat, "Failed to set cpBuilder");
	////stat = visArrayHnd.set(visBuilder);   //MCHECKERROR(stat, "Failed to set visBuilder");
	//stat = rpArrayHnd.set(rpBuilder);       MCHECKERROR(stat, "Failed to set rpBuilder");
	//stat = srArrayHnd.set(srBuilder);       MCHECKERROR(stat, "Failed to set srBuilder");
	//stat = ssArrayHnd.set(ssBuilder);       MCHECKERROR(stat, "Failed to set ssBuilder");
	//stat = siArrayHnd.set(siBuilder);       MCHECKERROR(stat, "Failed to set siBuilder");
	
	return stat;
}

MStatus TaneMPxShape::updateInstanceItem_(const std::shared_ptr<SourceManager>& pOutSourecs, const std::shared_ptr<Points>& pOutPoints)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	
	MStatus stat;
	if(pOutSourecs == nullptr)
	{
		LOGGER_ERROR("Failed to get sources");
		return MS::kFailure;
	}
	if(pOutPoints == nullptr)
	{
		LOGGER_ERROR("Failed to get sources");
		return MS::kFailure;
	}
	//if(pStateQueue_->empty())
	//{
	//	LOGGER_DEBUG("msgQueue is .empty");
	//	return MS::kFailure;
	//}
	
	size_t npoints  = pOutPoints->size();
	size_t nsources = pOutSourecs->size();
	
	LOGGER_DEBUG("npoints = %u", npoints);
	if(npoints > 0)
	{
		//StateQueue queue = pStateQueue_->clone();
		//while(!queue.empty())
		//{
		//	switch (queue.top())
		//	{
		//		case kNODE_ADDED:
		//		{
		//			//LOGGER_DEBUG("kNODE_ADDED");
		//		}
		//		break;
		//		case kINPUT_DIRTIED:
		//		{
		//			//LOGGER_DEBUG("kINPUT_DIRTIED");
		//		}
		//		break;
		//		case kMANIPULATING_SOURCE_ID:
		//		{
		//			//LOGGER_DEBUG("kMANIPULATING_SOURCE_ID");
		//		}
		//		break;
		//		case kMANIPULATING_RENDERABLE:
		//		{
		//			//LOGGER_DEBUG("kMANIPULATING_RENDERABLE");
		//		}
		//		break;
		//		case kMANIPULATING_TRANSFORM:
		//		{
		//			//LOGGER_DEBUG("kMANIPULATING_TRANSFORM");
		//		}
		//		break;
		//		case kTIME_CHANGED:
		//		{
		//			//LOGGER_DEBUG("kTIME_CHANGED");
		//		}
		//		break;
		//		case kDISPLAY_MODE_CHANGED:
		//		{
		//			//LOGGER_DEBUG("kDISPLAYMODE_CHANGED");
		//		}
		//		break;
		//		case kSHADER_ATTRIBUTE_CHANGED:
		//		{
		//			//LOGGER_DEBUG("kCOLORMODE_CHANGED");
		//		}
		//		break;
		//		case kNODE_DELECETED:
		//		{
		//			//LOGGER_DEBUG("kNODE_DELECETED");
		//		}
		//		break;
		//	}
		//	queue.pop();
		//}
		pInstancingItem_->reset2(pOutSourecs->sourceIDs(), pOutPoints);
	}
	return stat;
}
MStatus TaneMPxShape::buildTweakContainers_(MDataBlock &datablock, int count)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	MStatus stat;
	MArrayDataHandle clArrayHnd = datablock.outputArrayValue(mCulleds, &stat);          MCHECKERROR(stat, "Failed to get clArrayHnd");
	MArrayDataHandle rpArrayHnd = datablock.outputArrayValue(mRenderables, &stat);      MCHECKERROR(stat, "Failed to get rpArrayHnd");
	MArrayDataHandle siArrayHnd = datablock.outputArrayValue(mSourceIDs, &stat);        MCHECKERROR(stat, "Failed to get siArrayHnd");
	MArrayDataHandle cpArrayHnd = datablock.outputArrayValue(mControlPoints, &stat);    MCHECKERROR(stat, "Failed to get cpArrayHnd");
	MArrayDataHandle srArrayHnd = datablock.outputArrayValue(mEulerAngles, &stat);      MCHECKERROR(stat, "Failed to get srArrayHnd");
	MArrayDataHandle ssArrayHnd = datablock.outputArrayValue(mScales, &stat);           MCHECKERROR(stat, "Failed to get ssArrayHnd");
	
	MArrayDataBuilder oldClBuilder = clArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldClBuilder ");
	MArrayDataBuilder oldRpBuilder = rpArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldRpBuilder ");
	MArrayDataBuilder oldSiBuilder = siArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldSiBuilder ");

	MArrayDataBuilder oldCpBuilder = cpArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldCpBuilder ");
	MArrayDataBuilder oldSrBuilder = srArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldSrBuilder");
	MArrayDataBuilder oldSsBuilder = ssArrayHnd.builder(&stat);     MCHECKERROR(stat, "Failed to get oldSsBuilder");
	
	bool rebuild = false;
	rebuild = (rebuild == true || count != oldClBuilder.elementCount(&stat)) ? true : false;
	rebuild = (rebuild == true || count != oldRpBuilder.elementCount(&stat)) ? true : false;
	rebuild = (rebuild == true || count != oldSiBuilder.elementCount(&stat)) ? true : false;
	rebuild = (rebuild == true || count != oldCpBuilder.elementCount(&stat)) ? true : false;
	rebuild = (rebuild == true || count != oldSrBuilder.elementCount(&stat)) ? true : false;
	rebuild = (rebuild == true || count != oldSsBuilder.elementCount(&stat)) ? true : false;

	if(rebuild)
	{
		LOGGER_WARNING("rebuild manipulating array builde");
		
		MArrayDataBuilder clBuilder(oldClBuilder);
		MArrayDataBuilder rpBuilder(oldRpBuilder);
		MArrayDataBuilder siBuilder(oldSiBuilder);
		MArrayDataBuilder cpBuilder(oldCpBuilder);
		MArrayDataBuilder srBuilder(oldSrBuilder);
		MArrayDataBuilder ssBuilder(oldSsBuilder);
		
		for (int elemIndex = 0; elemIndex < count; elemIndex++)
		{
			/*bool&    flag = */ clBuilder.addElement(elemIndex, &stat).asBool();    MCHECKERROR(stat, "Failed to add element to clBuilder");
			/*bool&    flag = */ rpBuilder.addElement(elemIndex, &stat).asBool();    MCHECKERROR(stat, "Failed to add element to rpBuilder");
			/*int&     sid  = */ siBuilder.addElement(elemIndex, &stat).asInt();    MCHECKERROR(stat, "Failed to add element to siBuilder");
			/*double3& pt   = */ cpBuilder.addElement(elemIndex, &stat).asDouble3(); MCHECKERROR(stat, "Failed to add element to cpBuilder");
			/*double3& pt   = */ srBuilder.addElement(elemIndex, &stat).asDouble3(); MCHECKERROR(stat, "Failed to add element to srBuilder");
			/*double3& pt   = */ ssBuilder.addElement(elemIndex, &stat).asDouble3(); MCHECKERROR(stat, "Failed to add element to ssBuilder");
			
			///set next handle
			clArrayHnd.next();
			rpArrayHnd.next();
			siArrayHnd.next();
			cpArrayHnd.next();
			srArrayHnd.next();
			ssArrayHnd.next();
		}
		clArrayHnd.set(clBuilder);
		rpArrayHnd.set(rpBuilder);
		siArrayHnd.set(siBuilder);
		cpArrayHnd.set(cpBuilder);
		srArrayHnd.set(srBuilder);
		ssArrayHnd.set(ssBuilder);
		
		clArrayHnd.setAllClean();
		rpArrayHnd.setAllClean();
		siArrayHnd.setAllClean();
		cpArrayHnd.setAllClean();
		srArrayHnd.setAllClean();
		ssArrayHnd.setAllClean();
	}
	
	return stat;
}
void TaneMPxShape::verticesUpdated_()
{
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	childChanged(MPxSurfaceShape::kObjectChanged);
}
MStatus TaneMPxShape::computeBoundingBox_(MDataBlock &datablock)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxShape.logging);
	/// Update bounding box
	MStatus stat = MS::kSuccess;
	MDataHandle lowerHnd = datablock.outputValue(minCornerObj, &stat);
	MCHECKERROR(stat, "Failed to get minCornerHnd");
	MDataHandle upperHnd = datablock.outputValue(maxCornerObj, &stat);
	MCHECKERROR(stat, "Failed to get maxCornerHnd");
	
	double3 &lower = lowerHnd.asDouble3();
	double3 &upper = upperHnd.asDouble3();
	
	std::shared_ptr<Points> pPoints = outPoints();
	if(!pPoints || (pPoints && !(pPoints->size() > 0)))
	{
		LOGGER_ERROR("Failed to get valid points");
		return MS::kFailure;
	}
	size_t npoints = pPoints->size();
	{
		/// This clears any old bbox values
		lower[0] = (*pPoints)[0][0];
		lower[1] = (*pPoints)[0][1];
		lower[2] = (*pPoints)[0][2];
		
		upper[0] = (*pPoints)[0][0];
		upper[1] = (*pPoints)[0][1];
		upper[2] = (*pPoints)[0][2];

		for (int pid = 1; pid < npoints; pid++)
		{
			if((*pPoints)[pid][0] < lower[0]) lower[0] = (*pPoints)[pid][0];
			if((*pPoints)[pid][1] < lower[1]) lower[1] = (*pPoints)[pid][1];
			if((*pPoints)[pid][2] < lower[2]) lower[2] = (*pPoints)[pid][2];
			
			if((*pPoints)[pid][0] > upper[0]) upper[0] = (*pPoints)[pid][0];
			if((*pPoints)[pid][1] > upper[1]) upper[1] = (*pPoints)[pid][1];
			if((*pPoints)[pid][2] > upper[2]) upper[2] = (*pPoints)[pid][2];
		}
	}
	
	lowerHnd.setClean();
	upperHnd.setClean();
	
	/// Signal that the bounding box has changed.
	childChanged(MPxSurfaceShape::kBoundingBoxChanged);
	return stat;
}

MStatus TaneMPxShape::updateNumPoints(MDataBlock& datablock, size_t npoints)
{
	MStatus stat;
	
	//SetPlugValue<int>(thisMObject(), "npoints", npoints, &stat);

	//MDataHandle npointsHnd = datablock.outputValue(npointsObj, &stat);  MCHECKERROR(stat, "Failed to get npointsHnd");

	//int npoints = pointsPtr->size();
	//int32_t* renderablePtr = pointsPtr->data<int32_t>(sementic::attribute::renderable);
	//int nvisible = std::count(renderablePtr, renderablePtr + npoints, true);
	//npointsHnd.setInt(nvisible);
	
	//npointsHnd.setInt(static_cast<int>(npoints));
	//
	//MPlug npointsPlug(thisMObject(), npointsObj);
	//bool isLocked = npointsPlug.isLocked(&stat);
	//if(!isLocked)
	//	npointsPlug.setLocked(true);
	
	return stat;
	
}
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxSurfaceShapeUI
void* TaneMPxShapeUI::Creator()
{
	return new TaneMPxShapeUI;
}

TaneMPxShapeUI::TaneMPxShapeUI()
{
}

TaneMPxShapeUI::~TaneMPxShapeUI()
{
}

void TaneMPxShapeUI::getDrawRequests(const MDrawInfo &info, bool objectAndActiveOnly, MDrawRequestQueue &requests)
{
	MPxSurfaceShapeUI::getDrawRequests(info, objectAndActiveOnly, requests);
}
void TaneMPxShapeUI::draw(const MDrawRequest &request, M3dView &view) const
{
	MPxSurfaceShapeUI::draw(request, view);
}
bool TaneMPxShapeUI::select(MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts) const
{
	return MPxSurfaceShapeUI::select(selectInfo, selectionList, worldSpaceSelectPts);
}

NAMESPACE_BASE_END








