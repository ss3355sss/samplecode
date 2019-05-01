#pragma once
#include "maya/pch.h"

NAMESPACE_BASE_BEGIN
///---------------------------------------------- TaneMPxTransformDrawOverride
class TaneMPxTransformDrawOverride : public MHWRender::MPxDrawOverride
{
public:
	static MHWRender::MPxDrawOverride* Creator(const MObject &obj);

private:
	TaneMPxTransformDrawOverride(const MObject &objPath);
public:
	virtual ~TaneMPxTransformDrawOverride();

public:
	virtual MHWRender::DrawAPI 	supportedDrawAPIs() const;
	virtual MUserData* 			prepareForDraw(const MDagPath &objPath, const MDagPath &cameraPath, const MHWRender::MFrameContext &frameContext, MUserData* oldData);
	virtual bool 				isBounded(const MDagPath &objPath, const MDagPath &cameraPath) const;
	virtual bool 				isTransparent() const;
	virtual MBoundingBox 		boundingBox(const MDagPath &objPath, const MDagPath &cameraPath) const;
	//virtual MMatrix 			transform(const MDagPath &objPath, const MDagPath &cameraPath) const;

private:
	MObject  getShape(const MObject& obj) const;
	MDagPath getShapePath(const MObject& obj) const;
};
NAMESPACE_BASE_END

