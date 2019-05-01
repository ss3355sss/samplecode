#pragma once
#include "base/pch.h"
NAMESPACE_BASE_BEGIN
struct TransformMtx
{
public:
	Mat4x4f viewportMtx         = Mat4x4f::Identity();
	Mat3x3f normalMtx           = Mat3x3f::Identity();
	Mat4x4f modelViewMtx        = Mat4x4f::Identity();
	Mat4x4f projectionMtx       = Mat4x4f::Identity();
	Mat4x4f modelViewProjMtx    = Mat4x4f::Identity();
};

NAMESPACE_BASE_END



