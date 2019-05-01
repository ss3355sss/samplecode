#include "maya/usdProxyMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxSurfaceShape
MTypeId UsdProxyMPxShape::Id(RegisteredNodes->UsdProxyMPxShape.id);
MString UsdProxyMPxShape::NodeName(RegisteredNodes->UsdProxyMPxShape.name);
MString UsdProxyMPxShape::DrawOverrideRegistrantId(RegisteredNodes->UsdProxyMPxShape.name + "DrawOverride");
MString UsdProxyMPxShape::DrawDbClassification("drawdb/geometry/" + RegisteredNodes->UsdProxyMPxShape.name );

MObject UsdProxyMPxShape::renderFileObj;
MObject UsdProxyMPxShape::primPathObj;
MObject UsdProxyMPxShape::purposeObj;
MObject UsdProxyMPxShape::variantKeyObj;


NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxShapeUI
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ UsdProxyMPxShapeDrawOverride
NAMESPACE_BASE_END
