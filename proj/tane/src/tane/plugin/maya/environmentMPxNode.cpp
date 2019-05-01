#include "maya/environmentMPxNode.h"

#include "base/mathUtil.h"
#include "base/fileUtil.h"
#include "base/random.h"

#include "baseUsd/usdImporter.h"

#include "baseMaya/mayaMeshExtractor.h"
#include "baseMaya/util.h"
#include "baseMaya/zelosUtil.h"
#include <ZelosBase.h>

#include <thrust/execution_policy.h>
#include <thrust/for_each.h>
#include <thrust/random.h>
#include <thrust/system/omp/execution_policy.h>


NAMESPACE_BASE_BEGIN
///------------------------------------------------- DistributorNode
MTypeId EnvironmentMPxNode::id(RegisteredNodes->EnvironmentMPxNode.id);
MString EnvironmentMPxNode::nodeName(RegisteredNodes->EnvironmentMPxNode.name);

///------------------------------------------------------------ node attribute
MObject EnvironmentMPxNode::npointsObj;
MObject EnvironmentMPxNode::seedObj;

MObject EnvironmentMPxNode::sidRatioArrayObj;

MObject EnvironmentMPxNode::distributionTypeObj;
MObject EnvironmentMPxNode::useUpVectorObj;
MObject EnvironmentMPxNode::upVectorObj;

MObject EnvironmentMPxNode::uvSetNameObj;

///------------------------------------ density map
MObject EnvironmentMPxNode::useDensityMapObj;
MObject EnvironmentMPxNode::shiftDensityValueObj;
MObject EnvironmentMPxNode::densityMapObj;

///------------------------------------ remove map
MObject EnvironmentMPxNode::useRemoveMapObj;
MObject EnvironmentMPxNode::removeEpsilonObj;
MObject EnvironmentMPxNode::removeMapObj;

///------------------------------------ scale map
MObject EnvironmentMPxNode::useScaleMapObj;
MObject EnvironmentMPxNode::shiftScaleValueObj;
MObject EnvironmentMPxNode::scaleMapObj;

///------------------------------------ face angle
MObject EnvironmentMPxNode::useFaceAngleObj;
MObject EnvironmentMPxNode::faceAngleRangeObj;

///------------------------------------------------- randmize points
MObject EnvironmentMPxNode::xTranslateRangeObj;
MObject EnvironmentMPxNode::yTranslateRangeObj;
MObject EnvironmentMPxNode::zTranslateRangeObj;

MObject EnvironmentMPxNode::xRotateRangeObj;
MObject EnvironmentMPxNode::yRotateRangeObj;
MObject EnvironmentMPxNode::zRotateRangeObj;

MObject EnvironmentMPxNode::useUniformScaleObj;
MObject EnvironmentMPxNode::unifScaleRangeObj;
MObject EnvironmentMPxNode::xScaleRangeObj;
MObject EnvironmentMPxNode::yScaleRangeObj;
MObject EnvironmentMPxNode::zScaleRangeObj;

///------------------------------------------------- sampling points
//MObject EnvironmentMPxNode::useSamplingSchemeObj;
//MObject EnvironmentMPxNode::radiusObj;
//MObject EnvironmentMPxNode::radiusScaleObj;
//MObject EnvironmentMPxNode::useRadiusMapObj;
//MObject EnvironmentMPxNode::shiftRadiusValueObj;
//MObject EnvironmentMPxNode::radiusMapObj;

///------------------------------------------------- import cache
MObject EnvironmentMPxNode::inTimeObj;
MObject EnvironmentMPxNode::importSeedObj;
MObject EnvironmentMPxNode::importCacheObj;
MObject EnvironmentMPxNode::cacheFileObj;
MObject EnvironmentMPxNode::attachToBodyObj;
MObject EnvironmentMPxNode::importCacheRatioObj;
MObject EnvironmentMPxNode::applySidRatioObj;
MObject EnvironmentMPxNode::applyVariationObj;


///------------------------------------------------------------ input attributes
IMPL_IN_BODY_ATTRIBUTE(EnvironmentMPxNode);
IMPL_IN_SOURCE_ATTRIBUTE(EnvironmentMPxNode);

///------------------------------------------------------------ output attributes
IMPL_OUTPUT_ATTRIBUTE(EnvironmentMPxNode);

///------------------------------------------------------------ internal attributes
IMPL_MISCELLANEOUS_ATTRIBUTE(EnvironmentMPxNode);
///------------------------------------------------------------ static functions
void* EnvironmentMPxNode::Creator()
{
	return new EnvironmentMPxNode;
}
MStatus EnvironmentMPxNode::Initialize()
{
	MStatus              stat;
	MFnUnitAttribute     uAttr;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnCompoundAttribute cAttr;
	MFnMatrixAttribute   mAttr;
	MFnEnumAttribute     eAttr;
	
	///------------------------------------------------------------ node attribute
	///------------------------------------------------- distribute points
	seedObj = nAttr.create("seed", "seed", MFnNumericData::kInt, 0, &stat);
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
	ADD_ATTRIBUTE(seedObj);
	
	npointsObj = nAttr.create("npoints", "npoints", MFnNumericData::kInt, 4, &stat);
	MCHECKERROR(stat, "Failed to create npoints attribute error");
	nAttr.setMin(0);
	nAttr.setSoftMax(5000);
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
	ADD_ATTRIBUTE(npointsObj);
	
	applySidRatioObj = nAttr.create("applySidRatio", "applySidRatio", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create applySidRatio attribute");
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
	ADD_ATTRIBUTE(applySidRatioObj);
	
	applyVariationObj = nAttr.create("applyVariation", "applyVariation", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create applySidRatio attribute");
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
	ADD_ATTRIBUTE(applyVariationObj);
	
	
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
	
	distributionTypeObj = eAttr.create("distributionType", "distributionType", kRandomFaceSelection, &stat);
	MCHECKERROR(stat, "Failed to create distributionType attribute");
	eAttr.addField("Surface (random face selection)", kRandomFaceSelection);
	eAttr.addField("Surface (using residual)",        kRandomFaceSelectionFaceAreaInvariant);
	//TODO::scatter mode added
	//eAttr.addField("Face Center", 1);
	SET_ATTRIBUTE_STATUS(eAttr,
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
	ADD_ATTRIBUTE(distributionTypeObj);
	
	useUpVectorObj = nAttr.create("useUpVector", "useUpVector", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "create useUpVector attribute error");
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
	ADD_ATTRIBUTE(useUpVectorObj);
	
	///------------------------------------ upVector
	upVectorObj = nAttr.create("upVector", "upVector", MFnNumericData::k3Float, 0, &stat);
	MCHECKERROR(stat, "create rotateRange attribute error");
	nAttr.setDefault(0.0, 1.0, 0.0);
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
	ADD_ATTRIBUTE(upVectorObj);
	
	uvSetNameObj = tAttr.create("uvSetName", "uvSetName", MFnData::kString, MFnStringData().create("map1"), &stat);
	MCHECKERROR(stat, "create uvSetName attribute error");
	SET_ATTRIBUTE_STATUS(tAttr,
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
	ADD_ATTRIBUTE(uvSetNameObj);
	
	///------------------------------------ density map
	useDensityMapObj = nAttr.create("useDensityMap", "useDensityMap", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "create useDensityMap attribute error");
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
	ADD_ATTRIBUTE(useDensityMapObj);
	shiftDensityValueObj = nAttr.create("shiftDensityValue", "shiftDensityValue", MFnNumericData::kFloat, 0.0f, &stat);
	MCHECKERROR(stat, "create shiftDensityValue attribute error");
	nAttr.setMin(-1.0);
	nAttr.setMax(+1.0);
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
	ADD_ATTRIBUTE(shiftDensityValueObj);
	densityMapObj = tAttr.create("densityMap", "densityMap", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "create densityMap attribute error");
	SET_ATTRIBUTE_STATUS(tAttr,
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
	ADD_ATTRIBUTE(densityMapObj);
	
	///------------------------------------ remove map
	useRemoveMapObj = nAttr.create("useRemoveMap", "useRemoveMap", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "create useRemoveMap attribute error");
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
	ADD_ATTRIBUTE(useRemoveMapObj);
	removeEpsilonObj = nAttr.create("removeEpsilon", "removeEpsilon", MFnNumericData::kFloat, 0.01f, &stat);
	MCHECKERROR(stat, "create removeEpsilonObj attribute error");
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
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
	ADD_ATTRIBUTE(removeEpsilonObj);
	removeMapObj = tAttr.create("removeMap", "removeMap", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "create removeMap attribute error");
	SET_ATTRIBUTE_STATUS(tAttr,
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
	ADD_ATTRIBUTE(removeMapObj);
	
	///------------------------------------ scale map
	useScaleMapObj = nAttr.create("useScaleMap", "useScaleMap", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "create useScaleMap attribute error");
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
	ADD_ATTRIBUTE(useScaleMapObj);
	shiftScaleValueObj = nAttr.create("shiftScaleValue", "shiftScaleValue", MFnNumericData::kFloat, 0.0f, &stat);
	MCHECKERROR(stat, "create shiftScaleValue attribute error");
	nAttr.setMin(0.0);
	nAttr.setSoftMax(+1.0);
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
	ADD_ATTRIBUTE(shiftScaleValueObj);
	scaleMapObj = tAttr.create("scaleMap", "scaleMap", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "create scaleMap attribute error");
	SET_ATTRIBUTE_STATUS(tAttr,
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
	ADD_ATTRIBUTE(scaleMapObj);
	
	///------------------------------------ face angle
	useFaceAngleObj = nAttr.create("useFaceAngle", "useFaceAngle", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "create useFaceAngle attribute error");
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
	ADD_ATTRIBUTE(useFaceAngleObj);
	faceAngleRangeObj = nAttr.create("faceAngleRange", "faceAngleRange", MFnNumericData::k2Float, 0.0f, &stat);
	MCHECKERROR(stat, "create faceAngle attribute error");
	nAttr.setMin(-180.0, -180.0);
	nAttr.setMax(+180.0, +180.0);
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
	ADD_ATTRIBUTE(faceAngleRangeObj);
	
	///------------------------------------------------- randmize points attributes
	xTranslateRangeObj = nAttr.create("xTranslateRange", "xTranslateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create xTranslateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
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
	ADD_ATTRIBUTE(xTranslateRangeObj);
	yTranslateRangeObj = nAttr.create("yTranslateRange", "yTranslateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create yTranslateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
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
	ADD_ATTRIBUTE(yTranslateRangeObj);
	zTranslateRangeObj = nAttr.create("zTranslateRange", "zTranslateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create zTranslateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
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
	ADD_ATTRIBUTE(zTranslateRangeObj);
	
	xRotateRangeObj = nAttr.create("xRotateRange", "xRotateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create rotateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
	nAttr.setMin(-180.0, -180.0);
	nAttr.setMax(+180.0, +180.0);
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
	ADD_ATTRIBUTE(xRotateRangeObj);
	yRotateRangeObj = nAttr.create("yRotateRange", "yRotateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create yRotateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
	nAttr.setMin(-180.0, -180.0);
	nAttr.setMax(+180.0, +180.0);
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
	ADD_ATTRIBUTE(yRotateRangeObj);
	zRotateRangeObj = nAttr.create("zRotateRange", "zRotateRange", MFnNumericData::k2Float, 0.0, &stat);
	MCHECKERROR(stat, "create zRotateRange attribute error");
	nAttr.setDefault(0.0, 0.0);
	nAttr.setMin(-180.0, -180.0);
	nAttr.setMax(+180.0, +180.0);
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
	ADD_ATTRIBUTE(zRotateRangeObj);
	
	useUniformScaleObj = nAttr.create("useUniformScale", "useUniformScale", MFnNumericData::kBoolean, true, &stat);
	MCHECKERROR(stat, "create disable attribute error");
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
	ADD_ATTRIBUTE(useUniformScaleObj);
	unifScaleRangeObj = nAttr.create("uniformScaleRange", "uniformScaleRange", MFnNumericData::k2Float, 1.0, &stat);
	MCHECKERROR(stat, "create scaleRange  attribute error");
	nAttr.setDefault(1.0, 1.0);
	nAttr.setMin(0.0, 0.0);
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
	ADD_ATTRIBUTE(unifScaleRangeObj);
	xScaleRangeObj = nAttr.create("xScaleRange", "xScaleRange", MFnNumericData::k2Float, 1.0, &stat);
	MCHECKERROR(stat, "create xScalesRange attribute error");
	nAttr.setDefault(1.0, 1.0);
	nAttr.setMin(0.0, 0.0);
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
	ADD_ATTRIBUTE(xScaleRangeObj);
	yScaleRangeObj = nAttr.create("yScaleRange", "yScaleRange", MFnNumericData::k2Float, 1.0, &stat);
	MCHECKERROR(stat, "create xScalesRange attribute error");
	nAttr.setDefault(1.0, 1.0);
	nAttr.setMin(0.0, 0.0);
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
	ADD_ATTRIBUTE(yScaleRangeObj);
	zScaleRangeObj = nAttr.create("zScaleRange", "zScaleRange", MFnNumericData::k2Float, 1.0, &stat);
	MCHECKERROR(stat, "create xScalesRange attribute error");
	nAttr.setDefault(1.0, 1.0);
	nAttr.setMin(0.0, 0.0);
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
	ADD_ATTRIBUTE(zScaleRangeObj);
	
	/////------------------------------------------------- sampling points
	//useSamplingSchemeObj = nAttr.create("useSamplingScheme", "useSamplingScheme", MFnNumericData::kBoolean, false, &stat);
	//MCHECKERROR(stat, "create useSamplingScheme attribute error");
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(useSamplingSchemeObj);
	//radiusObj = nAttr.create("radius", "radius", MFnNumericData::kFloat, 0.0f, &stat);
	//MCHECKERROR(stat, "create radius attribute error");
	//nAttr.setMin(0.0f);
	//nAttr.setSoftMax(10.0f);
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(radiusObj);
	//radiusScaleObj = nAttr.create("radiusScale", "radiusScale", MFnNumericData::kFloat, 1.0f, &stat);
	//MCHECKERROR(stat, "create radius attribute error");
	//nAttr.setMin(0.0f);
	//nAttr.setSoftMax(10.0);
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(radiusScaleObj);
	//useRadiusMapObj = nAttr.create("useRadiusMap", "useRadiusMap", MFnNumericData::kBoolean, false, &stat);
	//MCHECKERROR(stat, "create useRadiusMap attribute error");
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(useRadiusMapObj);
	//shiftRadiusValueObj = nAttr.create("shiftRadiusValue", "shiftRadiusValue", MFnNumericData::kFloat, 0.0, &stat);
	//MCHECKERROR(stat, "create shiftRadiusValue attribute error");
	//nAttr.setMin(-1.0);
	//nAttr.setMax(+1.0);
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(shiftRadiusValueObj);
	//radiusMapObj = tAttr.create("radiusMap", "radiusMap", MFnData::kString, MObject::kNullObj, &stat);
	//MCHECKERROR(stat, "create radiusMap attribute error");
	//SET_ATTRIBUTE_STATUS(nAttr,
	//                     false,                     /* Readable              */
	//                     true,                     /* Writable              */
	//                     true,                     /* Connectable           */
	//                     true,                     /* Stroable              */
	//                     true,                     /* Cached                */
	//                     false,                     /* ChannelBox            */
	//                     false,                     /* Array                 */
	//                     false,                     /* Use Array DataBuilder */
	//                     true,                     /* Keyable               */
	//                     false,                     /* Hidden                */
	//                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	//                     false                      /* Affect Appearance     */
	//);
	//ADD_ATTRIBUTE(radiusMapObj);
	
	///------------------------------------------------- import cache
	inTimeObj = uAttr.create("inTime", "inTime", MFnUnitAttribute::kTime, 1.0, &stat);
	SET_ATTRIBUTE_STATUS(uAttr,
	                     false,                     /* Readable              */
	                     true,                      /* Writable              */
	                     true,                     /* Connectable           */
	                     false ,                     /* Stroable              */
	                     true ,                     /* Cached                */
	                     true ,                     /* ChannelBox            */
	                     false,                     /* Array                 */
	                     false,                     /* Use Array DataBuilder */
	                     false,                     /* Keyable               */
	                     false,                     /* Hidden                */
	                     MFnAttribute::kNothing,    /* Disconnect Behavior   */
	                     false                      /* Affect Appearance     */
	);
	MCHECKERROR(stat, "Failed to create inTime");
	ADD_ATTRIBUTE(inTimeObj);
	
	importSeedObj = nAttr.create("importSeed", "importSeed", MFnNumericData::kInt, 0, &stat);
	MCHECKERROR(stat, "Failed to create importSeed attribute");
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
	ADD_ATTRIBUTE(importSeedObj);
	importCacheObj = nAttr.create("importCache", "importCache", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create importCache attribute");
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
	ADD_ATTRIBUTE(importCacheObj);
	cacheFileObj = tAttr.create("cacheFile", "cacheFile", MFnData::kString, MObject::kNullObj, &stat);
	MCHECKERROR(stat, "Failed to create cacheFile attribute");
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
	ADD_ATTRIBUTE(cacheFileObj);
	attachToBodyObj = nAttr.create("attachToBody", "attachToBody", MFnNumericData::kBoolean, false, &stat);
	MCHECKERROR(stat, "Failed to create attachToBody attribute");
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
	ADD_ATTRIBUTE(attachToBodyObj);
	importCacheRatioObj = nAttr.create("importCacheRatio", "importCacheRatio", MFnNumericData::kFloat, 1.0, &stat);
	MCHECKERROR(stat, "Failed to create importCacheRatio attribute");
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
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
	ADD_ATTRIBUTE(importCacheRatioObj);
	
	///------------------------------------------------- input attributes
	INITIALIZE_IN_BODY_ATTRIBUTE;
	INITIALIZE_IN_SOURCE_ATTRIBUTE;
	
	///------------------------------------------------- output attributes
	INITIALIZE_OUTPUT_ATTRIBUTE;
	///------------------------------------------------- internal attributes
	INITIALIZE_MISCELLANEOUS_ATTRIBUTE(false);
	
	///------------------------------------------------- set input/output attribute affects
	///----------------------------------- distribute points
	ATTRIBUTE_AFFECTS(npointsObj, outputObj);
	ATTRIBUTE_AFFECTS(seedObj, outputObj);
	
	ATTRIBUTE_AFFECTS(sidRatioArrayObj, outputObj);
	
	ATTRIBUTE_AFFECTS(distributionTypeObj, outputObj);
	
	///------------------------------------ upVector
	ATTRIBUTE_AFFECTS(useUpVectorObj, outputObj);
	ATTRIBUTE_AFFECTS(upVectorObj, outputObj);
	ATTRIBUTE_AFFECTS(uvSetNameObj, outputObj);
	
	///------------------------------------ density map
	ATTRIBUTE_AFFECTS(useDensityMapObj, outputObj);
	ATTRIBUTE_AFFECTS(shiftDensityValueObj, outputObj);
	ATTRIBUTE_AFFECTS(densityMapObj, outputObj);
	
	///------------------------------------ remove map
	ATTRIBUTE_AFFECTS(useRemoveMapObj, outputObj);
	ATTRIBUTE_AFFECTS(removeEpsilonObj, outputObj);
	ATTRIBUTE_AFFECTS(removeMapObj, outputObj);
	
	///------------------------------------ scale map
	ATTRIBUTE_AFFECTS(useScaleMapObj, outputObj);
	ATTRIBUTE_AFFECTS(shiftScaleValueObj, outputObj);
	ATTRIBUTE_AFFECTS(scaleMapObj, outputObj);
	
	///------------------------------------ face angle
	ATTRIBUTE_AFFECTS(useFaceAngleObj, outputObj);
	ATTRIBUTE_AFFECTS(faceAngleRangeObj, outputObj);
	
	///------------------------------------------------- randmize points
	ATTRIBUTE_AFFECTS(xTranslateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(yTranslateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(zTranslateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(xRotateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(yRotateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(zRotateRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(useUniformScaleObj, outputObj);
	ATTRIBUTE_AFFECTS(unifScaleRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(xScaleRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(yScaleRangeObj, outputObj);
	ATTRIBUTE_AFFECTS(zScaleRangeObj, outputObj);
	
	/////------------------------------------------------- sampling points
	//ATTRIBUTE_AFFECTS(useSamplingSchemeObj, outputObj);
	//ATTRIBUTE_AFFECTS(radiusObj,            outputObj);
	//ATTRIBUTE_AFFECTS(useRadiusMapObj,      outputObj);
	//ATTRIBUTE_AFFECTS(shiftRadiusValueObj,  outputObj);
	//ATTRIBUTE_AFFECTS(radiusMapObj,         outputObj);
	
	///------------------------------------------------- import cache
	ATTRIBUTE_AFFECTS(inTimeObj,           outputObj);
	ATTRIBUTE_AFFECTS(importCacheObj,      outputObj);
	ATTRIBUTE_AFFECTS(cacheFileObj,        outputObj);
	ATTRIBUTE_AFFECTS(attachToBodyObj,     outputObj);
	ATTRIBUTE_AFFECTS(importCacheRatioObj, outputObj);
	ATTRIBUTE_AFFECTS(applySidRatioObj,    outputObj);
	ATTRIBUTE_AFFECTS(applyVariationObj,   outputObj);
	
	///------------------------------------------------- input attributes
	ATTRIBUTE_AFFECTS(inBodyObj,   outputObj);
	ATTRIBUTE_AFFECTS(inSourceObj, outputObj);
	
	///------------------------------------------------- internal attributes
	ATTRIBUTE_AFFECTS(recomputeObj, outputObj);
	
	return MS::kSuccess;
}
//void EnvironmentMPxNode::TimeChangeCallback(MTime &time, void* clientData)
//{
//	EnvironmentMPxNode* pNode = reinterpret_cast<ImportCacheMPxNode*>(clientData);
//	if(!pNode)
//	{
//		STDOUT_ERROR("Failed to get nodePtr");
//		return;
//	}
//	pNode->updateMask_.set(kUPDATE_OUTPOINTS);
//}


///------------------------------------------------------------ constructor / destructor
EnvironmentMPxNode::EnvironmentMPxNode()
{
}
EnvironmentMPxNode::~EnvironmentMPxNode()
{
}

///------------------------------------------------------------ virtual functions
void EnvironmentMPxNode::postConstructor()
{
	DEFAULT_DG_POST_CONSTRUCTOR(RegisteredNodes->EnvironmentMPxNode.name);
	
	pMsgQueue_ = std::make_shared<MessageQueue>();
	pMsgQueue_->push(kBODY_ATTRIBUTE_CHANGED);
	pMsgQueue_->push(kSOURCE_ATTRIBUTE_CHANGED);
	pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	
}
MStatus EnvironmentMPxNode::setDependentsDirty(const MPlug& plug, MPlugArray& affectedPlugs)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	
	MStatus stat = MS::kSuccess;
	
	///------------------------------------------------------------ node attributes
	///------------------------------------------------- distribute points
	if( plug == seedObj              ||
	    plug == npointsObj           ||
	    plug == sidRatioArrayObj     ||
	    plug == distributionTypeObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ upVector
	else if(plug == useUpVectorObj  ||
	        plug == upVectorObj     || plug.parent() == upVectorObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ using map
	else if(plug == uvSetNameObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ density map
	else if(plug == useDensityMapObj ||
	        plug == shiftDensityValueObj ||
	        plug == densityMapObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ remove map
	else if(plug == useRemoveMapObj     ||
	        plug == removeEpsilonObj    ||
	        plug == removeMapObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ scale map
	else if(plug == useScaleMapObj      ||
	        plug == shiftScaleValueObj  ||
	        plug == scaleMapObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------ face angle
	else if(plug == useFaceAngleObj     ||
	        plug == faceAngleRangeObj   || plug.parent() == faceAngleRangeObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------------------- randmize points attributes
	else if(plug == xTranslateRangeObj  || plug.parent() == xTranslateRangeObj ||
	        plug == yTranslateRangeObj  || plug.parent() == yTranslateRangeObj ||
	        plug == zTranslateRangeObj  || plug.parent() == zTranslateRangeObj ||
	        plug == xRotateRangeObj     || plug.parent() == xRotateRangeObj    ||
	        plug == yRotateRangeObj     || plug.parent() == yRotateRangeObj    ||
	        plug == zRotateRangeObj     || plug.parent() == zRotateRangeObj    ||
	        plug == useUniformScaleObj  ||
	        plug == unifScaleRangeObj   || plug.parent() == unifScaleRangeObj  ||
	        plug == xScaleRangeObj      || plug.parent() == xScaleRangeObj     ||
	        plug == yScaleRangeObj      || plug.parent() == yScaleRangeObj     ||
	        plug == zScaleRangeObj      || plug.parent() == zScaleRangeObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		//	///------------------------------------------------- sampling points
		//else if(plug == useSamplingSchemeObj    ||
		//        plug == radiusObj               ||
		//        plug == radiusScaleObj          ||
		//        plug == useRadiusMapObj         ||
		//        plug == shiftRadiusValueObj     ||
		//        plug == radiusMapObj
		//		)
		//{
		//	LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		//	pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
		//}
		///------------------------------------------------- import cache
	else if(plug == importCacheObj      ||
			plug == inTimeObj           ||
	        plug == cacheFileObj        ||
			plug == attachToBodyObj     ||
			plug == importSeedObj       ||
	        plug == importCacheRatioObj ||
	        plug == applySidRatioObj    ||
			plug == applyVariationObj
			)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		pMsgQueue_->push(kSOURCE_ATTRIBUTE_CHANGED);
		pMsgQueue_->push(kPOINT_ATTRIBUTE_CHANGED);
	}
		///------------------------------------------------------------ input attributes
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
		///------------------------------------------------------------ output attributes
		///------------------------------------------------------------ internal attributes
	else
	{
		LOGGER_WARNING("plug.info() = %s", plug.info().asChar());
	}
	return stat;
}

MStatus EnvironmentMPxNode::compute(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat = MS::kUnknownParameter;
	if(plug == outputObj)
	{
		LOGGER_INFO("plug.info() = %s", plug.info().asChar());
		MessageQueue msgQueue = pMsgQueue_->clone();
		while (!msgQueue.empty())
		{
			switch (msgQueue.top())
			{
				case kBODY_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kBODY_ATTRIBUTE_CHANGED");
					{
						stat = computeOutBody_(plug, datablock);
						MCHECKERRORNORET(stat, "Failed to compute outBody");
					}
				case kSOURCE_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kSOURCE_ATTRIBUTE_CHANGED");
					{
						stat = computeOutSources_(plug, datablock);
						MCHECKERRORNORET(stat, "Failed to compute outSource");
					}
					break;
				case kPOINT_ATTRIBUTE_CHANGED:
					LOGGER_INFO("kPOINT_ATTRIBUTE_CHANGED");
					{
						stat = computeOutPoints_(plug, datablock);
						MCHECKERROR(stat, "Failed to compute outPoints");
					}
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
///-------------------------------------- compute outMessage
MStatus EnvironmentMPxNode::computeOutMessage_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	
	MStatus       stat;
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(MessageMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create MessageMPxData");
	
	MessageMPxData* pMPxData = dynamic_cast<MessageMPxData*>(fnDataCreator.data(&stat));
	MCHECKERROR(stat, "Failed to get MessageMPxData");
	
	std::shared_ptr<Message> pMessage = pMPxData->data();
	pMessage->templet = templetType::environment;
	
	while (!pMsgQueue_->empty())
	{
		switch (pMsgQueue_->top())
		{
			case kBODY_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kBODY_CHANGED");
				pMessage->msgQueue.push(Message::kBODY_DIRTY);
				break;
			case kSOURCE_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kSOURCE_CHANGED");
				pMessage->msgQueue.push(Message::kSOURCE_DIRTY);
				break;
			case kPOINT_ATTRIBUTE_CHANGED:
				LOGGER_DEBUG("kPOINTS_CHANGED");
				pMessage->msgQueue.push(Message::kPOINT_DIRTY);
				break;
		}
		pMsgQueue_->pop();
	}
	MDataHandle outMessageHnd = datablock.outputValue(outMessageObj, &stat);
	outMessageHnd.set(pMPxData);
	return stat;
}
///-------------------------------------- compute outBody
MStatus EnvironmentMPxNode::computeOutBody_(const MPlug& plug, MDataBlock& datablock)
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
			//MObject meshObj = datablock.inputValue(inBodyObj).asMeshTransformed();
			//MayaMeshExtractor<int> extractor(obj, true);
			//extractor.compute();

			//for(int i = 0; i < extractor.positions()->size(); ++i)
			//	LOGGER_DEBUG("pPointArray[%d] = (%5.2f, %5.2f, %5.2f)", i, (*extractor.positions())[i][0], (*extractor.positions())[i][1], (*extractor.positions())[i][2]);
			//for(int i = 0; i < extractor.uvs()->size(); ++i)
			//	LOGGER_DEBUG("pUvArray[%d] = (%5.2f, %5.2f)", i, (*extractor.uvs())[i][0], (*extractor.uvs())[i][1]);
			//for(int fid = 0; fid < extractor.triangleIndices()->size(); ++fid)
			//	LOGGER_DEBUG("pIndexArray[%d] = (%d, %d, %d)", fid, (*extractor.triangleIndices())[fid][0], (*extractor.triangleIndices())[fid][1], (*extractor.triangleIndices())[fid][2]);
			//inBody_ = std::make_shared<TriangleMesh3f>(TriangleMesh3f(*extractor.positions(),
			//		                                                  *extractor.normals(),
			//		                                                  *extractor.uvs(),
			//		                                                  *extractor.triangleIndices()
			//));
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

	//	MDataHandle inBodyHnd = datablock.inputValue(inBodyObj, &stat);
	//	MCHECKERROR(stat, "Failed to get inBody Handle");
	//
	//	MDataHandle outBodyHnd = datablock.outputValue(outBodyObj, &stat);
	//	MCHECKERROR(stat, "Failed to get outBody Handle");
	//	outBodyHnd.set(inBodyHnd.asMesh());
	//	stat = MS::kSuccess;
	
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
MStatus EnvironmentMPxNode::computeOutSources_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	
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
MStatus EnvironmentMPxNode::computeOutPoints_(const MPlug& plug, MDataBlock& datablock)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	
	MStatus stat;
	
	const int& targetPoints = datablock.inputValue(npointsObj, &stat).asInt();
	LOGGER_DEBUG("targetPoints = %d", targetPoints);
	
	std::shared_ptr<Points> pOutPoints = Points::Create(targetPoints);
	
	LOGGER_DEBUG("pOutPoints->size() = %u", pOutPoints->size());
	
	//const bool importCache = datablock.inputValue(importCacheObj, &stat).asBool();
	//if (importCache)
	//{
	//	LOGGER_IN("append points in cache");
	//	{
	//		stat = appendPointsByCache_(targetPoints, datablock, pOutPoints);
	//		MCHECKERRORNORET(stat, "Failed to scatter on surface by cache");
	//	}
	//	LOGGER_OUT();
	//}
	
	///------------------------------------------ check inBody connection state
	const int npoints   = pOutPoints->size();
	LOGGER_DEBUG("npoints = %u", npoints);
	BoolArray isDeleted = BoolArray(npoints, false);

	MPlug inBodyPlug(thisMObject(), inBodyObj);
	bool bodyConneceted = inBodyPlug.isConnected();
	if(!bodyConneceted)
	{
		MGlobal::displayWarning("Body is not connected");
		LOGGER_WARNING("Body is not connected");
		return MS::kFailure;
	}
	
	const short distributionType = datablock.inputValue(distributionTypeObj, &stat).asShort();
	switch (distributionType)
	{
		case kRandomFaceSelection:
			stat = randomFaceSelectionScatter_(plug,
			                                   datablock,
			                                   pOutPoints,
			                                   &isDeleted
			);
			MCHECKERROR(stat, "Failed to get kRandomFaceSelection");
			break;
		case kRandomFaceSelectionFaceAreaInvariant:
			stat = randomFaceSelectionFaceAreaInvariant2_(plug,
			                                              datablock,
			                                              pOutPoints,
			                                              &isDeleted
			);
			MCHECKERROR(stat, "Failed to get randomFaceSelectionFaceAreaInvariant2_");
			break;
	}
	
	if(pOutPoints == nullptr)
	{
		STDOUT_ERROR("Failed to create outPoints");
		return MS::kFailure;
	}
	
	///------------------------------------------ apply post processing
	/* none */
	
	///------------------------------------------ delete point by mask
	LOGGER_DEBUG("remove points by mask");
	pOutPoints->remove(isDeleted, Points::kDELETE_MASK);
	
	/// ----------------------------------------- apply source id, physical id  */
	LOGGER_DEBUG("apply source id ratio");
	stat = applySourceIDRatio_(datablock, pOutPoints);
	MCHECKERROR(stat, "Failed to apply source id ratio");
	
	LOGGER_DEBUG("pOutPoints->size() = %u", pOutPoints->size());
	
	///------------------------------------------ create new PointsMpxData
	LOGGER_DEBUG("create new PointsMpxData");
	MFnPluginData fnDataCreator;
	fnDataCreator.create(MTypeId(PointsMPxData::id), &stat);
	MCHECKERROR(stat, "Failed to create PointsMpxData");

	PointsMPxData* newPointsMpxData = dynamic_cast<PointsMPxData*>(fnDataCreator.data(&stat));
	MCHECKERROR(stat, "Failed to getPointsMpxData");
	if(!newPointsMpxData)
	{
		LOGGER_ERROR("Failed to create newPointsMPxData");
		return MS::kFailure;
	}
	
	
	LOGGER_DEBUG("set internal data");
	newPointsMpxData->data() = pOutPoints;
	
	LOGGER_DEBUG("set outPointsHandle");
	MDataHandle outPointsHandle = datablock.outputValue(outPointsObj);
	stat = outPointsHandle.set(newPointsMpxData);
	MCHECKERROR(stat, "Failed to set outPointsHandle");
	return stat;
}

MStatus EnvironmentMPxNode::appendPointsByCache_(const int targetPoints,
                                                 MDataBlock& datablock,
                                                 std::shared_ptr<Points> pOutPoints
)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat;
	
	const MTime  & inTime       = datablock.inputValue(inTimeObj, &stat).asTime();
	const MString& cacheFile    = datablock.inputValue(cacheFileObj, &stat).asString();
	const bool   & attachToBody = datablock.inputValue(attachToBodyObj, &stat).asBool();
	
	LOGGER_DEBUG("inTime           = %5.2f", inTime.value());
	LOGGER_DEBUG("cacheFile        = %s",    cacheFile.asChar());
	
	std::shared_ptr<Points> pPoints = nullptr;
	const std::string extension = GetFileExtension(cacheFile.asChar());
	LOGGER_DEBUG("extension = %s", extension.c_str());
	if(extension == ".tane")
	{
		MString msg = "Failed to get supporting cache format";
		STDOUT_ERROR("%s", msg.asChar());
		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	else if(extension == ".usd" || extension == ".usda")
	{
		std::string primPath = "/";
	
		UsdGeomPointInstancerImporter importer;
		if(!importer.open(cacheFile.asChar(), "/"))
		{
			STDOUT_ERROR("Failed to open usd file, %s", cacheFile.asChar());
			return MS::kFailure;
		}
		
		pPoints = importer.getPoints(mode::animCycle::none,
				                     UsdTimeCode(inTime.value())
		);
		
		bool hasLocalAxis = importer.hasLocalAxis();
		bool hasRootS     = importer.hasRootS();
		bool hasRootT     = importer.hasRootT();
		bool hasRadius    = importer.hasRadius();
		
		MPlug inBodyPlug(thisMObject(), inBodyObj);
		bool bodyConneceted = inBodyPlug.isConnected();

		/* attach to body */
		const int npoints = pPoints->size();
		if(bodyConneceted && attachToBody)
		{
			MObject meshObj = datablock.inputValue(inBodyObj).asMesh();
			MFnMesh meshFn(meshObj, &stat);
			MCHECKERROR(stat, "Failed to get meshFn");
			
			Point3fArray& pointArray     = pPoints->get();
			Mat3x3fArray& localAxisArray = pPoints->get<Mat3x3f>(sementic::attribute::localAxis);
			
			MMeshIntersector intersector;
			intersector.create(meshObj);
			
			//LOGGER_IN("find closest point");
			//{
			//	thrust::for_each(thrust::omp::par,
			//	                 thrust::counting_iterator<size_t>(0),
			//	                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
			//	                 [&](const size_t idx)
			//	                 {
			//		                 MStatus stat;
			//
			//		                 MPoint p = MPoint(pointArray[idx][0], pointArray[idx][1], pointArray[idx][2]);
			//
			//		                 MPointOnMesh q;
			//		                 stat = intersector.getClosestPoint(p, q);
			//		                 if (stat != MS::kSuccess)
			//			                 return;
			//
			//		                 const MFloatPoint& closestPoint  = q.getPoint();
			//		                 const MFloatPoint& closestNormal = q.getNormal();
			//		                 q.faceIndex();
			//		                 q.getBarycentricCoords()
			//
			//		                 pointArray[idx] = Point3f(closestPoint[0], closestPoint[1], closestPoint[2]);
			//
			//		                 Normal3f up = Normal3f(closestNormal[0], closestNormal[1], closestNormal[2]);
			//
			//		                 std::tuple<Vec3f, Vec3f> tangentials = Tangential(up);
			//		                 Vec3f                    target      = std::get<0>(tangentials);
			//		                 Vec3f                    right       = std::get<1>(tangentials);
			//
			//		                 localAxisArray[idx].col(0) = right;
			//		                 localAxisArray[idx].col(1) = up;
			//		                 localAxisArray[idx].col(2) = target;
			//
			//		                 //meshFn.getClosestNormal();
			//		                 //meshFn.getClosestPoint();
			//		                 //meshFn.getClosestUVs();
			//		                 //meshFn.getClosestPointAndNormal();
			//
			//	                 });
			//}
			//LOGGER_OUT();
		}
		else
		{
			LOGGER_WARNING("Failed to calc localAxis, body is not connected");
		}
		
		
		
		//!importer.hasLocalAxis()
		
		
	}
	else
	{
		MString msg = "Failed to get supporting cache format";
		STDOUT_ERROR("%s", msg.asChar());
		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	
	const int  & importSeed       = datablock.inputValue(importSeedObj, &stat).asInt();
	const float& importCacheRatio = datablock.inputValue(importCacheRatioObj, &stat).asFloat();
	if(importCacheRatio < 1.0f)
	{
		const int npoints = pPoints->size();
		//IntArray mask = RandomSequence<int>(npoints, 0, 1, importSeed);
		
		LOGGER_DEBUG("importCacheRatio = %5.2f", importCacheRatio);
		LOGGER_DEBUG("importSeed = %d", importSeed);
		Array1<size_t> removeIDs(npoints);
		std::iota(removeIDs.begin(), removeIDs.end(), 0);
		
		std::mt19937 randomShuffleRng(importSeed);
		std::shuffle(removeIDs.begin(), removeIDs.end(), randomShuffleRng);
		
		const size_t removeCnt = static_cast<size_t>(npoints * (1.0f - importCacheRatio));
		LOGGER_DEBUG("removeCnt = %u", removeCnt);
		removeIDs.resize(removeCnt);
		pOutPoints->remove(removeIDs);
	}
	
	pOutPoints->append(*pPoints);
	return stat;
}

MStatus EnvironmentMPxNode::randomFaceSelectionScatter_(const MPlug &plug,
                                                        MDataBlock &datablock,
                                                        std::shared_ptr<Points> pOutPoints,
                                                        BoolArray* pOutIsDeleted
)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat;
	/* -------------------------------------------- initialize param */
	/* get upVector */
	const bool  & useUpVector = datablock.inputValue(useUpVectorObj, &stat).asBool();
	const float3& upVector    = datablock.inputValue(upVectorObj, &stat).asFloat3();
	
	/* get face angle remover */
	const bool  & useFaceAngle   = datablock.inputValue(useFaceAngleObj, &stat).asBool();
	const float2& faceAngleRange = datablock.inputValue(faceAngleRangeObj, &stat).asFloat2();
	
	/* get densityMap */
	const bool   & useDensityMap  = datablock.inputValue(useDensityMapObj, &stat).asBool();
	const float  & shiftDensity   = datablock.inputValue(shiftDensityValueObj, &stat).asFloat();
	const MString& densityMapPath = datablock.inputValue(densityMapObj, &stat).asString();
	
	/* get removeMap */
	const bool   & useRemoveMap  = datablock.inputValue(useRemoveMapObj, &stat).asBool();
	const float  & removeEpsilon = datablock.inputValue(removeEpsilonObj, &stat).asFloat();
	const MString& removeMapPath = datablock.inputValue(removeMapObj, &stat).asString();
	
	/* get scaleMap */
	const bool   & useScaleMap     = datablock.inputValue(useScaleMapObj, &stat).asBool();
	const float  & shiftScaleValue = datablock.inputValue(shiftScaleValueObj, &stat).asFloat();
	const MString& scaleMapPath    = datablock.inputValue(scaleMapObj, &stat).asString();
	
	/* get translate variation */
	const float2& xTranslateRange = datablock.inputValue(xTranslateRangeObj).asFloat2();
	const float2& yTranslateRange = datablock.inputValue(yTranslateRangeObj).asFloat2();
	const float2& zTranslateRange = datablock.inputValue(zTranslateRangeObj).asFloat2();
	
	/* get rotate variation */
	const float2& xRotateRange    = datablock.inputValue(xRotateRangeObj).asFloat2();
	const float2& yRotateRange    = datablock.inputValue(yRotateRangeObj).asFloat2();
	const float2& zRotateRange    = datablock.inputValue(zRotateRangeObj).asFloat2();
	
	/* get scale variation */
	const bool  & useUniformScale = datablock.inputValue(useUniformScaleObj).asBool();
	const float2& unifScaleRange  = datablock.inputValue(unifScaleRangeObj).asFloat2();
	const float2& xScaleRange     = datablock.inputValue(xScaleRangeObj).asFloat2();
	const float2& yScaleRange     = datablock.inputValue(yScaleRangeObj).asFloat2();
	const float2& zScaleRange     = datablock.inputValue(zScaleRangeObj).asFloat2();
	
	/* -------------------------------------------- get densityMap */
	bool applyDensityMap = false;
	Zelos::ZImageMap densityMap;
	stat = openZImageMap_(useDensityMap, densityMapPath, &applyDensityMap, &densityMap);
	if(stat != MS::kSuccess)
	{
		MString msg = "Failed to open density map" + densityMapPath;
		LOGGER_ERROR(msg.asChar());
		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	
	/* -------------------------------------------- get removeMap */
	bool applyRemoveMap = false;
	Zelos::ZImageMap removeMap;
	stat = openZImageMap_(useRemoveMap, removeMapPath, &applyRemoveMap, &removeMap);
	if(stat != MS::kSuccess)
	{
		MString msg = "Failed to open remove map" + removeMapPath;
		LOGGER_ERROR(msg.asChar());
		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	
	/* -------------------------------------------- get scaleMap */
	bool applyScaleMap = false;
	Zelos::ZImageMap scaleMap;
	stat = openZImageMap_(useScaleMap, scaleMapPath, &applyScaleMap, &scaleMap);
	if(stat != MS::kSuccess)
	{
		MString msg = "Failed to open scale map" + scaleMapPath;
		LOGGER_ERROR(msg.asChar());
		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	
	const int& targetPoints = datablock.inputValue(npointsObj, &stat).asInt();
	LOGGER_DEBUG("targetPoints = %d", targetPoints);

	const int npoints   = pOutPoints->size();
	LOGGER_DEBUG("npoints = %d", npoints);

	Int32Array     & sourceIdArary   = pOutPoints->get<int>(sementic::attribute::sid);
	Int32Array     & renderableArray = pOutPoints->get<int>(sementic::attribute::renderable);
	Point3fArray   & positionArray   = pOutPoints->get();
	Vec3fArray     & eulerAngleArray = pOutPoints->get<Vec3f>(sementic::attribute::eulerAngle);
	Vec3fArray     & scaleArray      = pOutPoints->get<Vec3f>(sementic::attribute::scale);
	Mat3x3fArray   & localAxisArray  = pOutPoints->get<Mat3x3f>(sementic::attribute::localAxis);
	TexCoord3fArray& uvArray         = pOutPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
	FloatArray     & radiusArray     = pOutPoints->get<float>(sementic::attribute::radius);

	/* get mesh data */
	MObject meshObj = datablock.inputValue(inBodyObj).asMesh();
	MFnMesh meshFn(meshObj, &stat);
	MCHECKERROR(stat, "Failed to get meshFn");

	MFloatPointArray pointsArray;
	MFloatArray      uArray;
	MFloatArray      vArray;
	stat = meshFn.getPoints(pointsArray, MSpace::kWorld);
	MCHECKERROR(stat, "Failed to Get pointsArray from MeshFn");
	
	const MString& uvSetName = datablock.inputValue(uvSetNameObj, &stat).asString();
	stat = meshFn.getUVs(uArray, vArray, &uvSetName);
	MCHECKERROR(stat, "Failed to Get UVs from MeshFn");
	
	const int& seed = datablock.inputValue(seedObj, &stat).asInt();
	MCHECKERROR(stat, "Failed to get seed attribute");
	
	const bool& applyVariation = datablock.inputValue(applyVariationObj, &stat).asBool();
	MCHECKERROR(stat, "Failed to get applyVariation attribute");
	
	const int npolygons = meshFn.numPolygons(&stat);
	//thrust::for_each(thrust::omp::par,
    thrust::for_each(thrust::host,
	                 thrust::counting_iterator<size_t>(0),
	                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
	                 [&](const size_t idx)
	                 {
		                 thrust::default_random_engine seedRng(seed);
		                 thrust::uniform_int_distribution<int> randomSeed(0, std::numeric_limits<int>::max());
		                 seedRng.discard(idx);
		
		                 Point3f p0 = Point3f(0.0f, 0.0f, 0.0f);
		                 Point3f p1 = Point3f(0.0f, 0.0f, 0.0f);
		                 Point3f p2 = Point3f(0.0f, 0.0f, 0.0f);
		
		                 if(idx < targetPoints)
		                 {
			                 int polyId = 0;
			                 {
				                 thrust::default_random_engine polyRng(randomSeed(seedRng));
				                 thrust::uniform_int_distribution<int> randomPolyId(0, std::nextafter(npolygons - 1, (npolygons - 1) + 1));
				                 polyRng.discard(idx);
				                 polyId = randomPolyId(polyRng);
			                 }

			                 int       triId      = 0;
			                 int       ntriangles = 0;
			                 MIntArray objectRelativeID;
			                 MIntArray faceRelativeID;
			                 {
				                 getIndices_(polyId, meshFn, &ntriangles, &objectRelativeID, &faceRelativeID);

				                 thrust::default_random_engine         triRng(randomSeed(seedRng));
				                 thrust::uniform_int_distribution<int> randomTriangleID(0, std::nextafter(ntriangles - 1, (ntriangles - 1) + 1));
				                 triRng.discard(idx);
				                 triId = randomTriangleID(triRng);
			                 }


			                 /* ------------------------------------------------------- calc alpha, beta */
			                 float alpha = 0;
			                 float beta  = 0;
			                 {
				                 thrust::default_random_engine alphaRng(randomSeed(seedRng));
				                 thrust::default_random_engine betaRng(randomSeed(seedRng));
				                 thrust::uniform_real_distribution<float> randomAlpha(0, std::nextafter(1.0, 2.0));
				                 thrust::uniform_real_distribution<float> randomBeta(0, std::nextafter(1.0, 2.0));
				                 alphaRng.discard(idx);
				                 betaRng.discard(idx);

				                 alpha = randomAlpha(alphaRng);
				                 beta  = randomBeta(betaRng);
				                 if (alpha + beta >= 1.0f)
				                 {
					                 alpha = 1.0f - alpha;
					                 beta  = 1.0f - beta;
				                 }
			                 }

			                 /* ------------------------------------------------------- get position  */
			                 p0 = Point3f(pointsArray[objectRelativeID[triId * 3 + 0]][0], pointsArray[objectRelativeID[triId * 3 + 0]][1], pointsArray[objectRelativeID[triId * 3 + 0]][2]);
			                 p1 = Point3f(pointsArray[objectRelativeID[triId * 3 + 1]][0], pointsArray[objectRelativeID[triId * 3 + 1]][1], pointsArray[objectRelativeID[triId * 3 + 1]][2]);
			                 p2 = Point3f(pointsArray[objectRelativeID[triId * 3 + 2]][0], pointsArray[objectRelativeID[triId * 3 + 2]][1], pointsArray[objectRelativeID[triId * 3 + 2]][2]);

			                 /* ------------------------------------------------------- get uv */
			                 int uvid0, uvid1, uvid2;
			                 stat = meshFn.getPolygonUVid(polyId, faceRelativeID[triId * 3 + 0], uvid0, &uvSetName);
			                 stat = meshFn.getPolygonUVid(polyId, faceRelativeID[triId * 3 + 1], uvid1, &uvSetName);
			                 stat = meshFn.getPolygonUVid(polyId, faceRelativeID[triId * 3 + 2], uvid2, &uvSetName);

			                 float u0, v0, u1, v1, u2, v2;
			                 stat = meshFn.getUV(uvid0, u0, v0, &uvSetName);
			                 stat = meshFn.getUV(uvid1, u1, v1, &uvSetName);
			                 stat = meshFn.getUV(uvid2, u2, v2, &uvSetName);

			                 /* ------------------------------------------------------- calc position */
			                 positionArray[idx] = alpha * p0 + beta * p1 + (1.0f - alpha - beta) * p2;

			                 /* ------------------------------------------------------- calc localAxis */
			                 Vec3f right  = Vec3f(1.0f, 0.0f, 0.0f);
			                 Vec3f up     = Vec3f(0.0f, 1.0f, 0.0f);
			                 Vec3f target = Vec3f(0.0f, 0.0f, 1.0f);
			                 if(!useUpVector)
			                 {
				                 calcLocalAxis_(p0, p1, p2, &target, &up, &right);
			                 }
			                 localAxisArray[idx].col(0) = right;
			                 localAxisArray[idx].col(1) = up;
			                 localAxisArray[idx].col(2) = target;

			                 /* ------------------------------------------------------- calc uv */
			                 float u = (alpha * u0) + (beta * u1) + (1.0f - alpha - beta) * u2;
			                 float v = (alpha * v0) + (beta * v1) + (1.0f - alpha - beta) * v2;
			                 uvArray[idx] = TexCoord3f(u, v, 0.0f);
		                 }
		
						 /* -------------------------------------------- initialize variation params */
		                 thrust::default_random_engine            densityMapRng(randomSeed(seedRng));
		                 thrust::uniform_real_distribution<float> randomDensityMap(0.0, std::nextafter(1.0, 2.0));
		                 densityMapRng.discard(idx);

		                 thrust::default_random_engine rotXRng(randomSeed(seedRng));
		                 thrust::default_random_engine rotYRng(randomSeed(seedRng));
		                 thrust::default_random_engine rotZRng(randomSeed(seedRng));
		                 thrust::uniform_real_distribution<float> randomRotX(xRotateRange[0], std::nextafter(xRotateRange[1], xRotateRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomRotY(yRotateRange[0], std::nextafter(yRotateRange[1], yRotateRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomRotZ(zRotateRange[0], std::nextafter(zRotateRange[1], zRotateRange[1] + 1.0));
		                 rotXRng.discard(idx);
		                 rotYRng.discard(idx);
		                 rotZRng.discard(idx);

		                 thrust::default_random_engine            unifScaleRng(randomSeed(seedRng));
		                 thrust::uniform_real_distribution<float> randomUnifScale(unifScaleRange[0], std::nextafter(unifScaleRange[1], unifScaleRange[1] + 1.0));
		                 unifScaleRng.discard(idx);

		                 thrust::default_random_engine            scaleXRng(randomSeed(seedRng));
		                 thrust::default_random_engine            scaleYRng(randomSeed(seedRng));
		                 thrust::default_random_engine            scaleZRng(randomSeed(seedRng));
		                 thrust::uniform_real_distribution<float> randomScaleX(xScaleRange[0], std::nextafter(xScaleRange[1], xScaleRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomScaleY(yScaleRange[0], std::nextafter(yScaleRange[1], yScaleRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomScaleZ(zScaleRange[0], std::nextafter(zScaleRange[1], zScaleRange[1] + 1.0));
		                 scaleXRng.discard(idx);
		                 scaleYRng.discard(idx);
		                 scaleZRng.discard(idx);

		                 thrust::default_random_engine transXRng(randomSeed(seedRng));
		                 thrust::default_random_engine transYRng(randomSeed(seedRng));
		                 thrust::default_random_engine transZRng(randomSeed(seedRng));
		                 thrust::uniform_real_distribution<float> randomTransX(xTranslateRange[0], std::nextafter(xTranslateRange[1], xTranslateRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomTransY(yTranslateRange[0], std::nextafter(yTranslateRange[1], yTranslateRange[1] + 1.0));
		                 thrust::uniform_real_distribution<float> randomTransZ(zTranslateRange[0], std::nextafter(zTranslateRange[1], zTranslateRange[1] + 1.0));
		                 transXRng.discard(idx);
		                 transYRng.discard(idx);
		                 transZRng.discard(idx);
		
		                 if(idx >= targetPoints && !applyVariation)
		                 	return;

		                 /* -------------------------------------------- faceAngleRemoval  */
		                 {
			                 static const float eps = 0.001f;
			                 const Vec3f& up = localAxisArray[idx].col(1);
			                 if (useFaceAngle && !testFaceAngle_(p0, p1, p2, faceAngleRange[0] - eps, faceAngleRange[1] + eps))
			                 {
				                 (*pOutIsDeleted)[idx] = true;
				                 return;
			                 }
		                 }
		                 /* -------------------------------------------- apply density map */
		                 {
			                 const TexCoord3f& uv = uvArray[idx];
			                 if (applyDensityMap && !testDensityMap_(densityMap, uv, randomDensityMap(densityMapRng), shiftDensity))
			                 {
				                 (*pOutIsDeleted)[idx] = true;
				                 return;
			                 }
		                 }
		                 /* -------------------------------------------- apply remove map */
		                 {
			                 const TexCoord3f& uv = uvArray[idx];
			                 if(applyRemoveMap && !testRemoveMap_(removeMap, uv, static_cast<float>(removeEpsilon)))
			                 {
				                 (*pOutIsDeleted)[idx] = true;
				                 return;
			                 }
		                 }

		                 /* -------------------------------------------- set orientation variation */
		                 {
			                 Vec3f right  = localAxisArray[idx].col(0);
			                 Vec3f up     = localAxisArray[idx].col(1);
			                 Vec3f target = localAxisArray[idx].col(2);

			                 float radx = Deg2Rad(randomRotX(rotXRng));
			                 float rady = Deg2Rad(randomRotY(rotYRng));
			                 float radz = Deg2Rad(randomRotZ(rotZRng));
			                 rotateLocalAxis_(radx, rady, radz, &target, &up, &right);

			                 localAxisArray[idx].col(0) = right;
			                 localAxisArray[idx].col(1) = up;
			                 localAxisArray[idx].col(2) = target;

			                 float rx, ry, rz;
			                 CalcEulerAngles(right, up, target, &rx, &ry, &rz);

			                 eulerAngleArray[idx] = Vec3f(rx, ry, rz);        ///euler angles, unit : radian
		                 }



		                 /* -------------------------------------------- set scale variation */
		                 {
			                 if (useUniformScale)
			                 {
				                 float s = randomUnifScale(unifScaleRng);
				                 if(applyScaleMap)
				                 {
					                 const TexCoord3f& uv = uvArray[idx];
					                 s *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
				                 }
				                 scaleArray[idx] = Vec3f(s, s, s);

			                 }
			                 else
			                 {
				                 float sx = randomScaleX(scaleXRng);
				                 float sy = randomScaleY(scaleYRng);
				                 float sz = randomScaleZ(scaleZRng);
				                 if (applyScaleMap)
				                 {
					                 const TexCoord3f& uv = uvArray[idx];
					                 sx *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
					                 sy *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
					                 sz *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
				                 }
				                 scaleArray[idx] = Vec3f(sx, sy, sz);
			                 }
			
		                 }

		                 /* -------------------------------------------- set translationY variation */
		                 {
			                 const Vec3f& right  = localAxisArray[idx].col(0);
			                 const Vec3f& up     = localAxisArray[idx].col(1);
			                 const Vec3f& target = localAxisArray[idx].col(2);

			                 float transX = randomTransX(transXRng);
			                 float transY = randomTransY(transYRng);
			                 float transZ = randomTransZ(transZRng);
			                 positionArray[idx] += transY * up;
		                 }
	                 });
	return stat;
}

MStatus EnvironmentMPxNode::randomFaceSelectionFaceAreaInvariant2_(const MPlug &plug,
                                                                   MDataBlock &datablock,
                                                                   std::shared_ptr<Points> pOutPoints,
                                                                   BoolArray* pOutIsDeleted
)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat;

	/* -------------------------------------------- initialize param */
	/* get upVector */
	const bool  & useUpVector = datablock.inputValue(useUpVectorObj, &stat).asBool();
	const float3& upVector    = datablock.inputValue(upVectorObj, &stat).asFloat3();
	/* get face angle remover */
	const bool  & useFaceAngle   = datablock.inputValue(useFaceAngleObj, &stat).asBool();
	const float2& faceAngleRange = datablock.inputValue(faceAngleRangeObj, &stat).asFloat2();
	/* get densityMap */
	const bool   & useDensityMap  = datablock.inputValue(useDensityMapObj, &stat).asBool();
	const float  & shiftDensity   = datablock.inputValue(shiftDensityValueObj, &stat).asFloat();
	const MString& densityMapPath = datablock.inputValue(densityMapObj, &stat).asString();
	/* get removeMap */
	const bool   & useRemoveMap  = datablock.inputValue(useRemoveMapObj, &stat).asBool();
	const float  & removeEpsilon = datablock.inputValue(removeEpsilonObj, &stat).asFloat();
	const MString& removeMapPath = datablock.inputValue(removeMapObj, &stat).asString();
	/* get scaleMap */
	const bool   & useScaleMap     = datablock.inputValue(useScaleMapObj, &stat).asBool();
	const float  & shiftScaleValue = datablock.inputValue(shiftScaleValueObj, &stat).asFloat();
	const MString& scaleMapPath    = datablock.inputValue(scaleMapObj, &stat).asString();
	/* get translate variation */
	const float2& xTranslateRange = datablock.inputValue(xTranslateRangeObj).asFloat2();
	const float2& yTranslateRange = datablock.inputValue(yTranslateRangeObj).asFloat2();
	const float2& zTranslateRange = datablock.inputValue(zTranslateRangeObj).asFloat2();
	/* get rotate variation */
	const float2& xRotateRange    = datablock.inputValue(xRotateRangeObj).asFloat2();
	const float2& yRotateRange    = datablock.inputValue(yRotateRangeObj).asFloat2();
	const float2& zRotateRange    = datablock.inputValue(zRotateRangeObj).asFloat2();
	/* get scale variation */
	const bool  & useUniformScale = datablock.inputValue(useUniformScaleObj).asBool();
	const float2& unifScaleRange  = datablock.inputValue(unifScaleRangeObj).asFloat2();
	const float2& xScaleRange     = datablock.inputValue(xScaleRangeObj).asFloat2();
	const float2& yScaleRange     = datablock.inputValue(yScaleRangeObj).asFloat2();
	const float2& zScaleRange     = datablock.inputValue(zScaleRangeObj).asFloat2();

	/* rand seed  */
	const int& seed = datablock.inputValue(seedObj, &stat).asInt();
	MCHECKERROR(stat, "Failed to get seed attribute");

	std::mt19937                          seedRng(seed);
	std::uniform_int_distribution<int>    randomSeed(0, std::numeric_limits<int>::max());

	/* rand baricentric  */
	std::mt19937                          randAlphaRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> randAlpha = std::uniform_real_distribution<float>(0.0f, std::nextafter(1.0f, 2.0f));
	std::mt19937                          randBetaRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> randBeta = std::uniform_real_distribution<float>(0.0f, std::nextafter(1.0f, 2.0f));
	/* rand density map  */
	std::mt19937 							randDensityRng;
	std::uniform_real_distribution<float> 	randDensity(0.0f, std::nextafter(1.0f, 2.0f));
	/* rand translation  */
	std::mt19937 							randTranslateXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateX(xTranslateRange[0], std::nextafter(xTranslateRange[1], xTranslateRange[1] + 1.0f));
	std::mt19937 							randTranslateYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateY(yTranslateRange[0], std::nextafter(yTranslateRange[1], yTranslateRange[1] + 1.0f));
	std::mt19937 							randTranslateZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateZ(zTranslateRange[0], std::nextafter(zTranslateRange[1], zTranslateRange[1] + 1.0f));
	/* rand rotation  */
	std::mt19937 							randRotXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateX(xRotateRange[0], std::nextafter(xRotateRange[1], xRotateRange[1] + 1.0));
	std::mt19937 							randRotYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateY(yRotateRange[0], std::nextafter(yRotateRange[1], yRotateRange[1] + 1.0));
	std::mt19937 							randRotZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateZ(zRotateRange[0], std::nextafter(zRotateRange[1], zRotateRange[1] + 1.0));
	/* rand uniform scale */
	std::mt19937 							randUnifScaleRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randUnifScale(unifScaleRange[0], std::nextafter(unifScaleRange[1], unifScaleRange[1] + 1.0));
	/* rand scale per axis*/
	std::mt19937                          	randScaleXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleX(xScaleRange[0], std::nextafter(xScaleRange[1], xScaleRange[1] + 1.0));
	std::mt19937                          	randScaleYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleY(yScaleRange[0], std::nextafter(yScaleRange[1], yScaleRange[1] + 1.0));
	std::mt19937                          	randScaleZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleZ(zScaleRange[0], std::nextafter(zScaleRange[1], zScaleRange[1] + 1.0));

	/* -------------------------------------------- get densityMap */
	bool applyDensityMap = false;
	Zelos::ZImageMap densityMap;
	stat = openZImageMap_(useDensityMap, densityMapPath, &applyDensityMap, &densityMap);
	MCHECKERROR(stat, "Failed to open density map, %s", densityMapPath.asChar());
	/* -------------------------------------------- get removeMap */
	bool applyRemoveMap = false;
	Zelos::ZImageMap removeMap;
	stat = openZImageMap_(useRemoveMap, removeMapPath, &applyRemoveMap, &removeMap);
	MCHECKERROR(stat, "Failed to open remove map, %s", removeMapPath.asChar());
	/* -------------------------------------------- get scaleMap */
	bool applyScaleMap = false;
	Zelos::ZImageMap scaleMap;
	stat = openZImageMap_(useScaleMap, scaleMapPath, &applyScaleMap, &scaleMap);
	MCHECKERROR(stat, "Failed to open scale map, %s", scaleMapPath.asChar());

	const int& targetPoints = datablock.inputValue(npointsObj, &stat).asInt();
	LOGGER_DEBUG("targetPoints = %d", targetPoints);

	const int npoints   = pOutPoints->size();
	LOGGER_DEBUG("npoints = %d", npoints);

	IntArray       & sourceIdArary   = pOutPoints->get<int>(sementic::attribute::sid);
	IntArray       & renderableArray = pOutPoints->get<int>(sementic::attribute::renderable);
	Point3fArray   & positionArray   = pOutPoints->get();
	Vec3fArray     & eulerAngleArray = pOutPoints->get<Vec3f>(sementic::attribute::eulerAngle);
	Vec3fArray     & scaleArray      = pOutPoints->get<Vec3f>(sementic::attribute::scale);
	Mat3x3fArray   & localAxisArray  = pOutPoints->get<Mat3x3f>(sementic::attribute::localAxis);
	TexCoord3fArray& uvArray         = pOutPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
	FloatArray     & radiusArray     = pOutPoints->get<float>(sementic::attribute::radius);

	LOGGER_DEBUG("positionArray.size() = %u", positionArray.size());

	
	
	/* get mesh data */
	int                numTriangle;
	float              totalArea;
	Zelos::ZFloatArray zAreaArray;
	Zelos::ZTriMesh    zInBodyMesh;
	{
		MString uvSetName = datablock.inputValue(uvSetNameObj, &stat).asString();
		MObject meshObj   = datablock.inputValue(inBodyObj).asMesh();
		
		stat = Convert(zInBodyMesh, meshObj, false, uvSetName.asChar());
		MCHECKERROR(stat, "Failed to convert ZTriMesh");
		
		const Zelos::ZPointArray& p    = zInBodyMesh.p;
		const Zelos::ZPointArray& uv   = zInBodyMesh.uv;
		const Zelos::ZInt3Array & v012 = zInBodyMesh.v012;
		
		numTriangle = v012.length();

		/* calc area */
		totalArea = 0.0f;
		zAreaArray = Zelos::ZFloatArray(numTriangle, 0.0f);
		//thrust::for_each(thrust::system::omp::par,
        thrust::for_each(thrust::host,
		                 thrust::counting_iterator<size_t>(0),
		                 thrust::counting_iterator<size_t>(static_cast<size_t>(numTriangle)),
		                 [&](const size_t idx)
		                 {
		                 	const Zelos::ZInt3&  vIndices = v012[idx];
         		            const Zelos::ZPoint& p0 = p[vIndices[0]];
			                const Zelos::ZPoint& p1 = p[vIndices[1]];
			                const Zelos::ZPoint& p2 = p[vIndices[2]];
		                 
			                const float area = 0.5f * ((p1 - p0) ^ (p2 - p0)).l2Norm();
			                zAreaArray[idx] = area;
		                 });

		/* calc total Area */
		totalArea = thrust::reduce(thrust::system::omp::par,
		                           zAreaArray.begin(),
		                           zAreaArray.end());
	}
	
	if(IsZero(totalArea))
	{
		LOGGER_ERROR("Failed to get valid totalArea")
		return MS::kFailure;
	}
	
	
	const Zelos::ZPointArray& p    = zInBodyMesh.p;
	const Zelos::ZPointArray& uv   = zInBodyMesh.uv;
	const Zelos::ZInt3Array & v012 = zInBodyMesh.v012;
	
	LOGGER_DEBUG("p.length()    = %u",    p.length());
	LOGGER_DEBUG("uv.length()   = %u",    uv.length());
	LOGGER_DEBUG("v012.length() = %u",    v012.length());
	LOGGER_DEBUG("numTriangle   = %d",    numTriangle);
	LOGGER_DEBUG("totalArea     = %5.2f", totalArea);
	
	LOGGER_IN("scatter points");
	{
		int pid = -1;
		float residue = 0.0f;
		for (size_t triId = 0; triId < numTriangle; ++triId)
		{
			const float& area = zAreaArray[triId];
			int nGeneration = targetPoints * (area / totalArea);
			residue += (targetPoints * (area / totalArea)) - nGeneration;
			if(residue > 1.0f)
			{
				residue -= 1.0f;
				++nGeneration;
			}
			for (size_t i = 0; i < nGeneration; ++i)
			{
				pid++;
				if(pid >= targetPoints)
					continue;
				
				const Zelos::ZInt3&  vIndices = v012[triId];
				
				const Vec3f& p0 = Point3f(p[vIndices[0]][0], p[vIndices[0]][1], p[vIndices[0]][2]);
				const Vec3f& p1 = Point3f(p[vIndices[1]][0], p[vIndices[1]][1], p[vIndices[1]][2]);
				const Vec3f& p2 = Point3f(p[vIndices[2]][0], p[vIndices[2]][1], p[vIndices[2]][2]);

				const TexCoord3f& uv0 = TexCoord3f(uv[3 * triId  + 0][0], uv[3 * triId  + 0][1], 0.0f);
				const TexCoord3f& uv1 = TexCoord3f(uv[3 * triId  + 1][0], uv[3 * triId  + 1][1], 0.0f);
				const TexCoord3f& uv2 = TexCoord3f(uv[3 * triId  + 2][0], uv[3 * triId  + 2][1], 0.0f);
				
				/* calc face normal */
				Vec3f p0p1   = p1 - p0;
				Vec3f p0p2   = p2 - p0;
				Vec3f normal = Cross(p0p1, p0p2).normalized(); /* face normal */

				/* calc barycentric coord */
				float alpha = randAlpha(randAlphaRng);
				float beta  = randBeta(randBetaRng);
				if(alpha + beta >= 1.0f)
				{
					alpha = 1.0f - alpha;
					beta  = 1.0f - beta;
				}
				/* calc position */
				Vec3f p  = alpha * p0 + beta * p1 + (1.0f - alpha - beta) * p2;
				/* calc uv */
				TexCoord3f uv = alpha * uv0 + beta * uv1 + (1.0f - alpha - beta) * uv2;
				/* -------------------------------------------- apply density map */
				if(applyDensityMap && !testDensityMap_(densityMap, uv, randDensity(randDensityRng), static_cast<float>(shiftDensity)))
				{
					(*pOutIsDeleted)[pid] = true;
					continue;
				}
				/* -------------------------------------------- apply remove map */
				if(applyRemoveMap && !testRemoveMap_(removeMap, uv, static_cast<float>(removeEpsilon)))
				{
					(*pOutIsDeleted)[pid] = true;
					continue;
				}
				/* -------------------------------------------- apply face angle */
				float eps = 0.001f;
				if(useFaceAngle && !testFaceAngle_(p0, p1, p2, faceAngleRange[0] - eps, faceAngleRange[1] + eps))
				{
					(*pOutIsDeleted)[pid] = true;
					continue;
				}
				/* set point */
				positionArray[pid] = p;

				/*  set texture coord */
				uvArray[pid] = uv;

				/* set local axis */
				Vec3f right  = Vec3f(1.0f, 0.0f, 0.0f);
				Vec3f up     = Vec3f(0.0f, 1.0f, 0.0f);
				Vec3f target = Vec3f(0.0f, 0.0f, 1.0f);
				if(!useUpVector)
				{
					calcLocalAxis_(p0, p1, p2, &target, &up, &right);
				}
				localAxisArray[pid].col(0) = right;
				localAxisArray[pid].col(1) = up;
				localAxisArray[pid].col(2) = target;

				/* set Orientation variation */
				float radx = Deg2Rad(randRotateX(randRotXRng));
				float rady = Deg2Rad(randRotateY(randRotYRng));
				float radz = Deg2Rad(randRotateZ(randRotZRng));

				rotateLocalAxis_(radx, rady, radz, &target, &up, &right);

				float rx, ry, rz;
				CalcEulerAngles(right, up, target, &rx, &ry, &rz);
				eulerAngleArray[pid] = Float3(rx, ry, rz);        ///euler angles, unit : radian
				/* set Scale variation */
				if(useUniformScale)
				{
					float s = randUnifScale(randUnifScaleRng);
					if(applyScaleMap)
					{
						s *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
					}
					//logger.message("pid = %d, scaleOffset from map = %5.2f", pid, scaleOffset);
					//logger.message("(sx, sy, sz) = (%5.2f, %5.2f, %5.2f)", s, s, s);
					scaleArray[pid] = Float3(s, s, s);
				}
				else
				{
					float sx = randScaleX(randScaleXRng);
					float sy = randScaleY(randScaleYRng);
					float sz = randScaleZ(randScaleZRng);
					if(applyScaleMap)
					{
						sx *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
						sy *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
						sz *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
					}
					//logger.message("(sx, sy, sz) = (%5.2f, %5.2f, %5.2f)", sx, sy, sz);
					scaleArray[pid] = Float3(sx, sy, sz);
				}
				/* set translationY variation */
				float transX = (randTranslateX(randTranslateXRng));
				float transY = (randTranslateY(randTranslateYRng));
				float transZ = (randTranslateZ(randTranslateZRng));
				positionArray[pid] += transY * up;
			}
		}
		(*pOutIsDeleted)[positionArray.size() - 1] = true;
		
	}
	LOGGER_OUT();
	return stat;
}

MStatus EnvironmentMPxNode::randomFaceSelectionFaceAreaInvariant3_(const MPlug &plug,
                                                                   MDataBlock &datablock,
                                                                   std::shared_ptr<Points> pOutPoints,
                                                                   BoolArray* pOutIsDeleted
)
{
	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	MStatus stat;
	
	/* -------------------------------------------- initialize param */
	/* get upVector */
	const bool  & useUpVector = datablock.inputValue(useUpVectorObj, &stat).asBool();
	const float3& upVector    = datablock.inputValue(upVectorObj, &stat).asFloat3();
	/* get face angle remover */
	const bool  & useFaceAngle   = datablock.inputValue(useFaceAngleObj, &stat).asBool();
	const float2& faceAngleRange = datablock.inputValue(faceAngleRangeObj, &stat).asFloat2();
	/* get densityMap */
	const bool   & useDensityMap  = datablock.inputValue(useDensityMapObj, &stat).asBool();
	const float  & shiftDensity   = datablock.inputValue(shiftDensityValueObj, &stat).asFloat();
	const MString& densityMapPath = datablock.inputValue(densityMapObj, &stat).asString();
	/* get removeMap */
	const bool   & useRemoveMap  = datablock.inputValue(useRemoveMapObj, &stat).asBool();
	const float  & removeEpsilon = datablock.inputValue(removeEpsilonObj, &stat).asFloat();
	const MString& removeMapPath = datablock.inputValue(removeMapObj, &stat).asString();
	/* get scaleMap */
	const bool   & useScaleMap     = datablock.inputValue(useScaleMapObj, &stat).asBool();
	const float  & shiftScaleValue = datablock.inputValue(shiftScaleValueObj, &stat).asFloat();
	const MString& scaleMapPath    = datablock.inputValue(scaleMapObj, &stat).asString();
	/* get translate variation */
	const float2& xTranslateRange = datablock.inputValue(xTranslateRangeObj).asFloat2();
	const float2& yTranslateRange = datablock.inputValue(yTranslateRangeObj).asFloat2();
	const float2& zTranslateRange = datablock.inputValue(zTranslateRangeObj).asFloat2();
	/* get rotate variation */
	const float2& xRotateRange    = datablock.inputValue(xRotateRangeObj).asFloat2();
	const float2& yRotateRange    = datablock.inputValue(yRotateRangeObj).asFloat2();
	const float2& zRotateRange    = datablock.inputValue(zRotateRangeObj).asFloat2();
	/* get scale variation */
	const bool  & useUniformScale = datablock.inputValue(useUniformScaleObj).asBool();
	const float2& unifScaleRange  = datablock.inputValue(unifScaleRangeObj).asFloat2();
	const float2& xScaleRange     = datablock.inputValue(xScaleRangeObj).asFloat2();
	const float2& yScaleRange     = datablock.inputValue(yScaleRangeObj).asFloat2();
	const float2& zScaleRange     = datablock.inputValue(zScaleRangeObj).asFloat2();
	
	/* rand seed  */
	const int& seed = datablock.inputValue(seedObj, &stat).asInt();
	MCHECKERROR(stat, "Failed to get seed attribute");
	
	std::mt19937                          seedRng(seed);
	std::uniform_int_distribution<int>    randomSeed(0, std::numeric_limits<int>::max());
	
	/* rand baricentric  */
	std::mt19937                          randAlphaRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> randAlpha = std::uniform_real_distribution<float>(0.0f, std::nextafter(1.0f, 2.0f));
	std::mt19937                          randBetaRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> randBeta = std::uniform_real_distribution<float>(0.0f, std::nextafter(1.0f, 2.0f));
	/* rand density map  */
	std::mt19937 							randDensityRng;
	std::uniform_real_distribution<float> 	randDensity(0.0f, std::nextafter(1.0f, 2.0f));
	/* rand translation  */
	std::mt19937 							randTranslateXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateX(xTranslateRange[0], std::nextafter(xTranslateRange[1], xTranslateRange[1] + 1.0f));
	std::mt19937 							randTranslateYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateY(yTranslateRange[0], std::nextafter(yTranslateRange[1], yTranslateRange[1] + 1.0f));
	std::mt19937 							randTranslateZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randTranslateZ(zTranslateRange[0], std::nextafter(zTranslateRange[1], zTranslateRange[1] + 1.0f));
	/* rand rotation  */
	std::mt19937 							randRotXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateX(xRotateRange[0], std::nextafter(xRotateRange[1], xRotateRange[1] + 1.0));
	std::mt19937 							randRotYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateY(yRotateRange[0], std::nextafter(yRotateRange[1], yRotateRange[1] + 1.0));
	std::mt19937 							randRotZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randRotateZ(zRotateRange[0], std::nextafter(zRotateRange[1], zRotateRange[1] + 1.0));
	/* rand uniform scale */
	std::mt19937 							randUnifScaleRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randUnifScale(unifScaleRange[0], std::nextafter(unifScaleRange[1], unifScaleRange[1] + 1.0));
	/* rand scale per axis*/
	std::mt19937                          	randScaleXRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleX(xScaleRange[0], std::nextafter(xScaleRange[1], xScaleRange[1] + 1.0));
	std::mt19937                          	randScaleYRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleY(yScaleRange[0], std::nextafter(yScaleRange[1], yScaleRange[1] + 1.0));
	std::mt19937                          	randScaleZRng(randomSeed(seedRng));
	std::uniform_real_distribution<float> 	randScaleZ(zScaleRange[0], std::nextafter(zScaleRange[1], zScaleRange[1] + 1.0));
	
	/* -------------------------------------------- get densityMap */
	bool applyDensityMap = false;
	Zelos::ZImageMap densityMap;
	stat = openZImageMap_(useDensityMap, densityMapPath, &applyDensityMap, &densityMap);
	MCHECKERROR(stat, "Failed to open density map, %s", densityMapPath.asChar());
	/* -------------------------------------------- get removeMap */
	bool applyRemoveMap = false;
	Zelos::ZImageMap removeMap;
	stat = openZImageMap_(useRemoveMap, removeMapPath, &applyRemoveMap, &removeMap);
	MCHECKERROR(stat, "Failed to open remove map, %s", removeMapPath.asChar());
	/* -------------------------------------------- get scaleMap */
	bool applyScaleMap = false;
	Zelos::ZImageMap scaleMap;
	stat = openZImageMap_(useScaleMap, scaleMapPath, &applyScaleMap, &scaleMap);
	MCHECKERROR(stat, "Failed to open scale map, %s", scaleMapPath.asChar());
	
	const int& targetPoints = datablock.inputValue(npointsObj, &stat).asInt();
	LOGGER_DEBUG("targetPoints = %d", targetPoints);
	
	const int npoints   = pOutPoints->size();
	LOGGER_DEBUG("npoints = %d", npoints);
	
	IntArray       & sourceIdArary   = pOutPoints->get<int>(sementic::attribute::sid);
	IntArray       & renderableArray = pOutPoints->get<int>(sementic::attribute::renderable);
	Point3fArray   & positionArray   = pOutPoints->get();
	Vec3fArray     & eulerAngleArray = pOutPoints->get<Vec3f>(sementic::attribute::eulerAngle);
	Vec3fArray     & scaleArray      = pOutPoints->get<Vec3f>(sementic::attribute::scale);
	Mat3x3fArray   & localAxisArray  = pOutPoints->get<Mat3x3f>(sementic::attribute::localAxis);
	TexCoord3fArray& uvArray         = pOutPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
	FloatArray     & radiusArray     = pOutPoints->get<float>(sementic::attribute::radius);
	
	LOGGER_DEBUG("positionArray.size() = %u", positionArray.size());
	
	/* get mesh data */
	std::shared_ptr<Int3Array>       pTriangleIndices;
	std::shared_ptr<Int2Array>       pWireIndices;
	std::shared_ptr<Point3fArray>    pPositions;
	std::shared_ptr<Normal3fArray>   pNormals;
	std::shared_ptr<TexCoord2fArray> pUvs;

	std::shared_ptr<FloatArray>      pAreaArray;
	int                              nTriangles = 0;
	float                            totalArea  = 0.0f;
	LOGGER_IN("Get InBody Data");
	{
		const MString& uvSetName   = datablock.inputValue(uvSetNameObj, &stat).asString();
		
		MObject                meshObj = datablock.inputValue(inBodyObj).asMesh();
		MayaMeshExtractor<int> extractor(meshObj, true);
		LOGGER_IN("compute extractor");
		extractor.compute();
		
		pTriangleIndices = extractor.triangleIndices();
		pWireIndices     = extractor.wireIndices();
		pPositions       = extractor.positions();
		pNormals         = extractor.normals();
		pUvs             = extractor.uvs();

		
		nTriangles       = pTriangleIndices->size();

		LOGGER_DEBUG("nTriangles         = %u", nTriangles);
		LOGGER_DEBUG("pPositions->size() = %u", pPositions->size());
		LOGGER_DEBUG("pNormals->size()   = %u", pNormals->size());
		LOGGER_DEBUG("pUvs->size()       = %u", pUvs->size());
		
		
		if(nTriangles <= 0 || pPositions->size() == 0 || pNormals->size() == 0 || pUvs->size() == 0)
		{
			LOGGER_ERROR("Failed to get valid InBodyData");
			LOGGER_OUT();
			return MS::kFailure;
		}

		/* calc area */
		pAreaArray = std::make_shared<FloatArray>(nTriangles, 0.0f);
		//thrust::for_each(thrust::system::omp::par,
        thrust::for_each(thrust::host,
		                 thrust::counting_iterator<size_t>(0),
		                 thrust::counting_iterator<size_t>(static_cast<size_t>(nTriangles)),
		                 [&](const size_t idx)
		                 {
			                 const Int3& vIndices = (*pTriangleIndices)[idx];
			
			                 const Point3f& p0 = (*pPositions)[vIndices[0]];
			                 const Point3f& p1 = (*pPositions)[vIndices[1]];
			                 const Point3f& p2 = (*pPositions)[vIndices[2]];
			                 
			                 /* push_back triangle area */
			                 float area = 0.5f * cross((p1 - p0), (p2 - p0)).norm();
			                 (*pAreaArray)[idx] = area;
		                 });
		
		/* calc total Area */
		totalArea = thrust::reduce(thrust::system::omp::par,
				                   pAreaArray->begin(),
				                   pAreaArray->end()
		);
		
		LOGGER_DEBUG("totalArea         = %5.2f", totalArea);
		if(IsZero(totalArea))
		{
			LOGGER_ERROR("Failed to get valid InBodyData");
			LOGGER_OUT();
			return MS::kFailure;
		}
		
		
		
	}
	LOGGER_OUT();
	
	//LOGGER_IN("scatter points");
	//{
	//	int pid = -1;
	//	float residue = 0.0f;
	//	for (size_t triId = 0; triId < nTriangles; ++triId)
	//	{
	//		const float& area = (*pAreaArray)[triId];
	//		int nGeneration = targetPoints * (area / totalArea);
	//		residue += (targetPoints * (area / totalArea)) - nGeneration;
	//		if(residue > 1.0f)
	//		{
	//			residue -= 1.0f;
	//			++nGeneration;
	//		}
	//		for (size_t i = 0; i < nGeneration; ++i)
	//		{
	//			pid++;
	//			if(pid >= targetPoints)
	//				continue;
	//
	//			const Vec3f& p0 = vertices[triId * 3 + 0];
	//			const Vec3f& p1 = vertices[triId * 3 + 1];
	//			const Vec3f& p2 = vertices[triId * 3 + 2];
	//			const TexCoord3f& uv0 = TexCoord3f(uvs[triId * 3 + 0][0], uvs[triId * 3 + 0][1], 0.0f);
	//			const TexCoord3f& uv1 = TexCoord3f(uvs[triId * 3 + 1][0], uvs[triId * 3 + 1][1], 0.0f);
	//			const TexCoord3f& uv2 = TexCoord3f(uvs[triId * 3 + 2][0], uvs[triId * 3 + 2][1], 0.0f);
	//			/* calc face normal */
	//			Vec3f p0p1   = p1 - p0;
	//			Vec3f p0p2   = p2 - p0;
	//			Vec3f normal = Cross(p0p1, p0p2).normalized(); /* face normal */
	//
	//			/* calc barycentric coord */
	//			float alpha = randAlpha(randAlphaRng);
	//			float beta  = randBeta(randBetaRng);
	//			if(alpha + beta >= 1.0f)
	//			{
	//				alpha = 1.0f - alpha;
	//				beta  = 1.0f - beta;
	//			}
	//			/* calc position */
	//			Vec3f p  = alpha * p0 + beta * p1 + (1.0f - alpha - beta) * p2;
	//			/* calc uv */
	//			TexCoord3f uv = alpha * uv0 + beta * uv1 + (1.0f - alpha - beta) * uv2;
	//			/* -------------------------------------------- apply density map */
	//			if(applyDensityMap && !testDensityMap_(densityMap, uv, randDensity(randDensityRng), static_cast<float>(shiftDensity)))
	//			{
	//				(*pOutIsDeleted)[pid] = true;
	//				continue;
	//			}
	//			/* -------------------------------------------- apply remove map */
	//			if(applyRemoveMap && !testRemoveMap_(removeMap, uv, static_cast<float>(removeEpsilon)))
	//			{
	//				(*pOutIsDeleted)[pid] = true;
	//				continue;
	//			}
	//			/* -------------------------------------------- apply face angle */
	//			float eps = 0.001f;
	//			if(useFaceAngle && !testFaceAngle_(p0, p1, p2, faceAngleRange[0] - eps, faceAngleRange[1] + eps))
	//			{
	//				(*pOutIsDeleted)[pid] = true;
	//				continue;
	//			}
	//			/* set point */
	//			positionArray[pid] = p;
	//
	//			/*  set texture coord */
	//			uvArray[pid] = uv;
	//
	//			/* set local axis */
	//			Vec3f right  = Vec3f(1.0f, 0.0f, 0.0f);
	//			Vec3f up     = Vec3f(0.0f, 1.0f, 0.0f);
	//			Vec3f target = Vec3f(0.0f, 0.0f, 1.0f);
	//			if(!useUpVector)
	//			{
	//				calcLocalAxis_(p0, p1, p2, &target, &up, &right);
	//			}
	//			localAxisArray[pid].col(0) = right;
	//			localAxisArray[pid].col(1) = up;
	//			localAxisArray[pid].col(2) = target;
	//
	//			/* set Orientation variation */
	//			float radx = Deg2Rad(randRotateX(randRotXRng));
	//			float rady = Deg2Rad(randRotateY(randRotYRng));
	//			float radz = Deg2Rad(randRotateZ(randRotZRng));
	//
	//			rotateLocalAxis_(radx, rady, radz, &target, &up, &right);
	//
	//			float rx, ry, rz;
	//			CalcEulerAngles(right, up, target, &rx, &ry, &rz);
	//			eulerAngleArray[pid] = Float3(rx, ry, rz);        ///euler angles, unit : radian
	//			/* set Scale variation */
	//			if(useUniformScale)
	//			{
	//				float s = randUnifScale(randUnifScaleRng);
	//				if(applyScaleMap)
	//				{
	//					s *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
	//				}
	//				//logger.message("pid = %d, scaleOffset from map = %5.2f", pid, scaleOffset);
	//				//logger.message("(sx, sy, sz) = (%5.2f, %5.2f, %5.2f)", s, s, s);
	//				scaleArray[pid] = Float3(s, s, s);
	//			}
	//			else
	//			{
	//				float sx = randScaleX(randScaleXRng);
	//				float sy = randScaleY(randScaleYRng);
	//				float sz = randScaleZ(randScaleZRng);
	//				if(applyScaleMap)
	//				{
	//					sx *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
	//					sy *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
	//					sz *= getScaleFromMap_(scaleMap, uv) + shiftScaleValue;
	//				}
	//				//logger.message("(sx, sy, sz) = (%5.2f, %5.2f, %5.2f)", sx, sy, sz);
	//				scaleArray[pid] = Float3(sx, sy, sz);
	//			}
	//			/* set translationY variation */
	//			float transX = (randTranslateX(randTranslateXRng));
	//			float transY = (randTranslateY(randTranslateYRng));
	//			float transZ = (randTranslateZ(randTranslateZRng));
	//			positionArray[pid] += transY * up;
	//		}
	//	}
	//}
	//LOGGER_OUT();
	return stat;
}





MStatus EnvironmentMPxNode::applySourceIDRatio_(MDataBlock& datablock, const std::shared_ptr<Points> pOutPoints)
{
	//DECL_LOGGER(logging::maya->EnvironmentMPxNode);
	DECL_LOGGER(false);
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
		const int& targetPoints = datablock.inputValue(npointsObj, &stat).asInt();
		LOGGER_DEBUG("targetPoints = %d", targetPoints);
		
		IntArray& pidArray = pOutPoints->get<int>(sementic::attribute::pid);
		IntArray& sidArray = pOutPoints->get<int>(sementic::attribute::sid);
		
		const int& seed = datablock.inputValue(seedObj, &stat).asInt();
		MCHECKERROR(stat, "Failed to get seed attribute");
		
		const bool& applySidRatio = datablock.inputValue(applySidRatioObj, &stat).asBool();
		
		//thrust::for_each(thrust::omp::par,
        thrust::for_each(thrust::host,
		                 thrust::counting_iterator<size_t>(0),
		                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
		                 [&](const size_t idx)
		                 {
			                 thrust::default_random_engine seedRng(seed);
			                 thrust::uniform_int_distribution<int> randomSeed(0, std::numeric_limits<int>::max());
			                 seedRng.discard(idx);
			
			                 ///-------------------------------------------- set pid
			                 std::mt19937                 randSourceIDsRng(randomSeed(seedRng));
			                 std::discrete_distribution<> sourceIDsDist(ratios.begin(), ratios.end());
			                 randSourceIDsRng.discard(idx);
			
			                 pidArray[idx] = idx;
			                 
			                 if(idx < targetPoints)
			                 {
				                 ///-------------------------------------------- set sid
				                 if(elemCount > 0)
					                 sidArray[idx] = sourceIDs[sourceIDsDist(randSourceIDsRng)];
				                 else
					                 sidArray[idx] = 0;
			                 }
			                 else if(applySidRatio)
			                 {
				                 if (elemCount > 0)
					                 sidArray[idx] = sourceIDs[sourceIDsDist(randSourceIDsRng)];
				                 else
					                 sidArray[idx] = 0;
			                 }
		                 	
		                 });
	}
	
	
	return stat;
}

MStatus EnvironmentMPxNode::openZImageMap_(const bool use, const MString& path, bool* sucess, Zelos::ZImageMap* map)
{
	MStatus stat;
	if(use)
	{
		if(path == "")
		{
			MGlobal::displayWarning("Map path is .empty");
			return MS::kFailure;
		}
		else
		{
			*sucess = map->load(path.asChar());
			if(!(*sucess))
			{
				//MGlobal::displayWarning("Failed to open map");
				return MS::kFailure;
			}
		}
	}
	return MS::kSuccess;
}

bool EnvironmentMPxNode::testFaceAngle_(const Point3f& p0, const Point3f& p1, const Point3f& p2, const float& minAngle, const float& maxAngle)
{
	Vec3f yAxis(0.0f, 1.0f, 0.0f);
	Vec3f p0p1   = p1 - p0;
	Vec3f p0p2   = p2 - p0;
	Vec3f  normal = cross(p0p1, p0p2).normalized();
	float angle = Rad2Deg(acos(dot(normal, yAxis)));
	if(((minAngle < angle) && (angle < maxAngle)))
	{
		return true;
	}
	return false;
}

bool EnvironmentMPxNode::testDensityMap_(const Zelos::ZImageMap& densityMap, const TexCoord3f& uv, const float& prob, const float& shiftDensity)
{
	//const Zelos::ZColor c = densityMap.color(uv[0], uv[1], false);
	//
	//float val       = c.intensity() + shiftDensity;
	//val = boost::algorithm::clamp(val, 0.0f, 1.0f);
	//
	//if(prob <= val)
	//	return true;
	//return false;
	
	float val = densityMap.fastValue(uv[0], uv[1], 4 /*rgba*/) + static_cast<float>(shiftDensity);
	val = boost::algorithm::clamp(val, 0.0f, 1.0f);
	if(prob <= (val))
	{
		return true;
	}
	return false;
	
}
bool EnvironmentMPxNode::testRemoveMap_(const Zelos::ZImageMap& removeMap, const TexCoord3f& uv, const float& removeEps)
{
	//const Zelos::ZColor c = removeMap.color(uv[0], uv[1], false);
	//
	//float val       = c.intensity();
	//val       = boost::algorithm::clamp(val, 0.0f, 1.0f);
	//
	//if((std::abs<float>(val - 1.0f)) < static_cast<float>(removeEps))
	//	return true;
	//return false;
	
	float val = removeMap.fastValue(uv[0], uv[1], 4 /*rgba*/);
	val = boost::algorithm::clamp(val, 0.0f, 1.0f);
	if((std::abs<float>(val - 1.0f)) < static_cast<float>(removeEps))
	{
		return true;
	}
	return false;
	
}
float EnvironmentMPxNode::getScaleFromMap_(const Zelos::ZImageMap& scaleMap, const TexCoord3f& uv)
{
	//const Zelos::ZColor c = scaleMap.color(uv[0], uv[1], false);
	//float val = c.intensity();
	//val = boost::algorithm::clamp(val, 0.0f, 1.0f);
	//return val;

	float val = scaleMap.fastValue(uv[0], uv[1], 4 /*rgba*/);
	val = boost::algorithm::clamp(val, 0.0f, 1.0f);
	return val;
	
}


void EnvironmentMPxNode::calcLocalAxis_(const Point3f& p0, const Point3f& p1, const Point3f& p2, Vec3f* target, Vec3f* up, Vec3f* right)
{
	Vec3f p0p1 = p1 - p0;
	Vec3f p0p2 = p2 - p0;
	
	*target = Vec3f(0.0f, 0.0f, 1.0f).normalized();  //front vector(z-axis)
	*up     = cross(p0p1, p0p2).normalized();
	*right  = cross(*up, *target).normalized();
	*target = cross(*right, *up).normalized();
}
void EnvironmentMPxNode::rotateLocalAxis_(const float radx, const float rady, const float radz, Vec3f* target, Vec3f* up, Vec3f* right)
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

//MStatus getInBodyData_(MDataBlock& datablock,
//                       std::shared_ptr<Int3Array>          pTriangleIndices,
//                       std::shared_ptr<Int2Array>          pWireIndices,
//                       std::shared_ptr<Point3fArray>       pPositions,
//                       std::shared_ptr<Normal3fArray>      pNormals,
//                       std::shared_ptr<TexCoord2fArray>    pUVs,
//                       float* pTotalArea
//)
//{
//	DECL_LOGGER(logging::maya->EnvironmentMPxNode);
//
//	MStatus stat;
//	const MString& uvSetName   = datablock.inputValue(uvSetNameObj, &stat).asString();
//
//
//	LOGGER_DEBUG("get bodyMesh data");
//	{
//		MObject                meshObj = datablock.inputValue(inBodyObj).asMesh();
//		MayaMeshExtractor<int> extractor(meshObj, true);
//		extractor.compute();
//
//		const std::shared_ptr<Int3Array>      & pTriangleIndices = extractor.triangleIndices();
//		const std::shared_ptr<Int2Array>      & pWireIndices     = extractor.wireIndices();
//		const std::shared_ptr<Point3fArray>   & pPositions       = extractor.positions();
//		const std::shared_ptr<Normal3fArray>  & pNormals         = extractor.normals();
//		const std::shared_ptr<TexCoord2fArray>& pUvs             = extractor.uvs();
//
//		float totalArea = 0.0f;
//		const int ntriangle = pTriangleIndices->size();
//
//		FloatArray areaArray(ntriangle, 0.0f);
//
//		/* calc area */
//		thrust::for_each(thrust::system::omp::par,
//		                 thrust::counting_iterator<size_t>(0),
//		                 thrust::counting_iterator<size_t>(static_cast<size_t>(ntriangle)),
//		                 [&](const size_t idx)
//		                 {
//			                 const Int3& vIndices = (*pTriangleIndices)[idx];
//
//			                 const Point3f& p0 = (*pPositions)[vIndices[0]];
//			                 const Point3f& p1 = (*pPositions)[vIndices[1]];
//			                 const Point3f& p2 = (*pPositions)[vIndices[2]];
//
//			                 /* push_back triangle area */
//			                 float area = 0.5f * cross((p1 - p0), (p2 - p0)).norm();
//			                 areaArray[idx] = area;
//		                 });
//		/* calc total Area */
//
//	}
//	LOGGER_OUT();
//
//
//	MObject meshObj = datablock.inputValue(inBodyObj).asMesh();
//	MFnMesh meshFn(meshObj, &stat);
//	MCHECKERROR(stat, "Getting meshFn is failed");
//	LOGGER_DEBUG("name = %s", meshFn.name().asChar());
//
//	/* get Mesh Data  */
//	int npolygons     = meshFn.numPolygons(&stat);
//	int nuvs          = meshFn.numUVs(uvSetName, &stat);
//	int nvertices     = meshFn.numVertices(&stat);
//	int nfacevertcies = meshFn.numFaceVertices(&stat);
//	if(nuvs == 0)
//	{
//		MGlobal::displayError("body mesh has no uv");
//		return MS::kFailure;
//	}
//	MFloatPointArray pointsArray;
//	MFloatArray      uArray;
//	MFloatArray      vArray;
//	stat = meshFn.getPoints(pointsArray, MSpace::kWorld);
//	MCHECKERROR(stat, "failed to get points");
//	stat = meshFn.getUVs(uArray, vArray, &uvSetName);
//	MCHECKERROR(stat, "failed to get uvs");
//
//	pointsArrayPtr->clear();
//	pointsArrayPtr->reserve(npolygons * 3);
//
//	uvsArrayPtr->clear();
//	uvsArrayPtr->reserve(npolygons * 3);
//
//	areaArrayPtr->clear();
//	areaArrayPtr->reserve(npolygons * 3);
//
//	LOGGER_DEBUG("pointsArray.length() = %u", pointsArray.length());
//	LOGGER_DEBUG("uArray.length()      = %u", uArray.length());
//	LOGGER_DEBUG("vArray.length()      = %u", vArray.length());
//
//	MItMeshPolygon fIter(meshObj, &stat);
//	MCHECKERROR(stat, "failed to get fIter");
//
//	int     ntriangle = 0;
//	float   areaSum = 0.0f;
//	LOGGER_IN("get bodyMesh data");
//	for(fIter.reset(); !fIter.isDone(); fIter.next())
//	{
//		MIntArray vIndices, uvIndices;
//		fIter.getVertices(vIndices);
//		const int vCount = vIndices.length();
//		if(vCount < 3)
//			continue;
//
//		uvIndices.setLength(vCount);
//		for (int i = 0; i < vCount; ++i)
//		{
//			//cout << "i = " << i << endl;
//			fIter.getUVIndex(i, uvIndices[i], &uvSetName);
//		}
//		for (int i = 0; i < vCount - 2; ++i)
//		{
//			//cout << "j = " << i << endl;
//			const int vid0 = vIndices[0    ];
//			const int vid1 = vIndices[i + 1];
//			const int vid2 = vIndices[i + 2];
//
//			/* push_back point */
//			const Vec3f p0 = Vec3f(pointsArray[vIndices[0    ]].x, pointsArray[vIndices[0    ]].y, pointsArray[vIndices[0    ]].z);
//			const Vec3f p1 = Vec3f(pointsArray[vIndices[i + 1]].x, pointsArray[vIndices[i + 1]].y, pointsArray[vIndices[i + 1]].z);
//			const Vec3f p2 = Vec3f(pointsArray[vIndices[i + 2]].x, pointsArray[vIndices[i + 2]].y, pointsArray[vIndices[i + 2]].z);
//			pointsArrayPtr->push_back(p0);
//			pointsArrayPtr->push_back(p1);
//			pointsArrayPtr->push_back(p2);
//
//			/* push_back uv */
//			//const Vec2f uv0 = Vec2f(0.0f, 0.0f);//Vec2f(uArray[uvIndices[0    ]], vArray[uvIndices[0    ]]);
//			//const Vec2f uv1 = Vec2f(0.0f, 0.0f);//Vec2f(uArray[uvIndices[i + 1]], vArray[uvIndices[i + 1]]);
//			//const Vec2f uv2 = Vec2f(0.0f, 0.0f);//Vec2f(uArray[uvIndices[i + 2]], vArray[uvIndices[i + 2]]);
//
//			const Vec2f uv0 = Vec2f(uArray[uvIndices[0    ]], vArray[uvIndices[0    ]]);
//			const Vec2f uv1 = Vec2f(uArray[uvIndices[i + 1]], vArray[uvIndices[i + 1]]);
//			const Vec2f uv2 = Vec2f(uArray[uvIndices[i + 2]], vArray[uvIndices[i + 2]]);
//
//
//			uvsArrayPtr->push_back(uv0);
//			uvsArrayPtr->push_back(uv1);
//			uvsArrayPtr->push_back(uv2);
//
//			/* push_back triangle area */
//			float area = 0.5f * cross((p1 - p0), (p2 - p0)).norm();
//			areaArrayPtr->push_back(area);
//			areaSum += area;
//			ntriangle++;
//		}
//	}
//	LOGGER_OUT();
//
//	/* set totalArea */
//	*totalAreaPtr = areaSum;
//
//	/* set ntriangles */
//	*ntrianglePtr = ntriangle;
//
//	return stat;
//}
bool EnvironmentMPxNode::getIndices_(const int polyId, const MFnMesh& meshFn, int* ntriangles, MIntArray* objectRelativeID, MIntArray* faceRelativeID)
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