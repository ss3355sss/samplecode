#pragma once
#include "baseGl/pch.h"

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"
NAMESPACE_BASE_BEGIN
template<typename T>
inline const GLenum GlDataType()
{
	const std::type_info& typeInfo = typeid(T);
	
	if(typeInfo == typeid(char) || (typeInfo == typeid(uint64_t)))
	{
		return GL_BOOL;
	}
	else if(typeInfo == typeid(uint32_t) || (typeInfo == typeid(uint64_t)))
	{
		return GL_UNSIGNED_INT;
	}
	else if(typeInfo == typeid(int32_t) || (typeInfo == typeid(int64_t)))
	{
		return GL_INT;
	}
	else if(typeInfo == typeid(float))
	{
		return GL_FLOAT;
	}
	else if(typeInfo == typeid(double))
	{
		return GL_DOUBLE;
	}
	else if(typeInfo == typeid(Vec2i) || typeInfo == typeid(Vec3i) || typeInfo == typeid(Vec4i))
	{
		return GL_INT;
	}
	else if(typeInfo == typeid(Vec2f) || typeInfo == typeid(Vec3f) || typeInfo == typeid(Vec4f))
	{
		return GL_FLOAT;
	}
	else if(typeInfo == typeid(Vec2d) || typeInfo == typeid(Vec3d) || typeInfo == typeid(Vec4d)) ///use Point3d, Normal3d
	{
		return GL_DOUBLE;
	}
	else if(typeInfo == typeid(Mat2x2f) || typeInfo == typeid(Mat3x3f) || typeInfo == typeid(Mat4x4f))
	{
		return GL_FLOAT;
	}
	else if(typeInfo == typeid(Mat2x2d) || typeInfo == typeid(Mat3x3d) || typeInfo == typeid(Mat4x4d))
	{
		return GL_DOUBLE;
	}
	else if(typeInfo == typeid(Quatf))
	{
		return GL_FLOAT;
	}
	else if(typeInfo == typeid(Quatd))
	{
		return GL_DOUBLE;
	}
	else
	{
		STDOUT_ERROR("Failed to valid gl data type");
		return 0u;
	}
}

NAMESPACE_BASE_END