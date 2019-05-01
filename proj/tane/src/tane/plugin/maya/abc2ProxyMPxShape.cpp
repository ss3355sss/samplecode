#pragma once
#include "maya/pch.h"

#include "maya/abc2ProxyMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxSurfaceShape
MTypeId Abc2ProxyMPxShape::id(RegisteredNodes->Abc2ProxyMPxShape.id);
MString Abc2ProxyMPxShape::nodeName(RegisteredNodes->Abc2ProxyMPxShape.name);
MString Abc2ProxyMPxShape::drawOverrideRegistrantId(RegisteredNodes->Abc2ProxyMPxShape.name + "DrawOverride");
MString Abc2ProxyMPxShape::drawDbClassification("drawdb/geometry/" + RegisteredNodes->Abc2ProxyMPxShape.name);
MObject Abc2ProxyMPxShape::filePath0Obj;
MObject Abc2ProxyMPxShape::objectPath0Obj;
MObject Abc2ProxyMPxShape::proxyPath0Obj;
MObject Abc2ProxyMPxShape::proxyObjectPath0Obj;
MObject Abc2ProxyMPxShape::useLOD1Obj;
MObject Abc2ProxyMPxShape::filePath1Obj;
MObject Abc2ProxyMPxShape::objectPath1Obj;
MObject Abc2ProxyMPxShape::proxyPath1Obj;
MObject Abc2ProxyMPxShape::proxyObjectPath1Obj;
MObject Abc2ProxyMPxShape::useLOD2Obj;
MObject Abc2ProxyMPxShape::filePath2Obj;
MObject Abc2ProxyMPxShape::objectPath2Obj;
MObject Abc2ProxyMPxShape::proxyPath2Obj;
MObject Abc2ProxyMPxShape::proxyObjectPath2Obj;

NAMESPACE_BASE_END


NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ AbcProxyMPxShapeUI
NAMESPACE_BASE_END

