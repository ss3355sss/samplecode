#pragma once
#include "maya/pch.h"

#include "maya/abcProxyMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxSurfaceShape
MTypeId AbcProxyMPxShape::Id(RegisteredNodes->AbcProxyMPxShape.id);
MString AbcProxyMPxShape::NodeName(RegisteredNodes->AbcProxyMPxShape.name);
MString AbcProxyMPxShape::DrawDbClassification("drawdb/geometry/" + RegisteredNodes->AbcProxyMPxShape.name);
MString AbcProxyMPxShape::DrawOverrideRegistrantId(RegisteredNodes->AbcProxyMPxShape.name + "DrawOverride");

MObject AbcProxyMPxShape::renderFileObj;
MObject AbcProxyMPxShape::renderObjectPathObj;
MObject AbcProxyMPxShape::useProxyFileObj;
MObject AbcProxyMPxShape::proxyFileObj;
MObject AbcProxyMPxShape::useProxyObjectPathObj;
MObject AbcProxyMPxShape::proxyObjectPathObj;

MObject AbcProxyMPxShape::flipvObj;

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeUI
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeDrawOverride
NAMESPACE_BASE_END
