#pragma once
#include "base/pch.h"

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"
NAMESPACE_BASE_BEGIN
template<typename T>
inline const size_t TypeDimension()
{
	const std::type_info& typeInfo = typeid(T);
	
	if(typeInfo == typeid(int32_t))
	{
		return 1u;
	}
	else if(typeInfo == typeid(int64_t))
	{
		return 1u;
	}
	if(typeInfo == typeid(float))
	{
		return 1u;
	}
	else if(typeInfo == typeid(double))
	{
		return 1u;
	}
	else if(typeInfo == typeid(Vec2i) || typeInfo == typeid(Vec2f) || typeInfo == typeid(Vec2d))
	{
		return 2u;
	}
	else if(typeInfo == typeid(Vec3i) || typeInfo == typeid(Vec3f) || typeInfo == typeid(Vec3d)) ///use Point3f, Normal3f
	{
		return 3u;
	}
	else if(typeInfo == typeid(Vec4i) || typeInfo == typeid(Vec4f) || typeInfo == typeid(Vec4d)) ///use Point3d, Normal3d
	{
		return 4u;
	}
	else if(typeInfo == typeid(Mat2x2f) || typeInfo == typeid(Mat2x2d))
	{
		return 4u;
	}
	else if(typeInfo == typeid(Mat3x3f) || typeInfo == typeid(Mat3x3d))
	{
		return 9u;
	}
	else if(typeInfo == typeid(Mat4x4f) || typeInfo == typeid(Mat4x4d))
	{
		return 16u;
	}
	else if(typeInfo == typeid(Quatf))
	{
		return 4u;
	}
	else if(typeInfo == typeid(Quatd))
	{
		return 4u;
	}
	else
	{
		STDOUT_ERROR("Failed to valid data dim");
		return 0u;
	}
}


NAMESPACE_BASE_END