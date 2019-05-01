#pragma once

#include "base/pch.h"

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"
#include "base/ray.h"
#include "base/bbox.h"

NAMESPACE_BASE_BEGIN

template<typename T>
class Transform3 final
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "Transform3 only can be instantiated with floating point types");

private:
	Vec3<T>   translation_;
	Quat<T>   orientation_;
	Mat3x3<T> orientationMtx_;
	Mat3x3<T> inverseOrientationMtx_;

public:
	Transform3(const Vec3<T>& t = Vec3<T>::Zero(), const Quat<T>& o = Quat<T>::Identity() )
	{
		setTranslation_(t);
		setOrientation_(o);
	}
	Transform3(const Mat4x4<T>& mtx)
	{
		const Vec3<T>& t = mtx.col(3);
		const Mat3x3<T>& o = mtx.block(0, 0, 3, 3);
		
		setTranslation_(t);
		setOrientation_(Quat<T>(o));
	}
	~Transform3()
	{
	}
public:
	const Vec3<T>& translation() const
	{
		return translation_;
	}
	Vec3<T>& translation()
	{
		return translation_;
	}
	const Quat<T>& orientation() const
	{
		return orientation_;
	}
	Quat<T>& orientation()
	{
		return orientation_;
	}

public:
	Point3<T> toLocal(const Point3<T>& pointInWorld) const
	{
		return inverseOrientationMtx_ * (pointInWorld - translation_);
	}
	Ray3<T> toLocal(const Ray3<T>& rayInWorld) const
	{
		return Ray3<T>(toLocal(rayInWorld.origin()), toLocalDirection(rayInWorld.direction()));
	}
	BBox3<T> toLocal(const BBox3<T>& bboxInWorld) const
	{
		Vec3<T> lower;
		Vec3<T> upper;
		for (int i = 0; i < 8; ++i)
		{
			Vec3<T> corner_in_local = toLocal(bboxInWorld.corner(i));
			lower = Min(lower, corner_in_local);
			upper = Max(upper, corner_in_local);
		}
		return BBox3<T>(lower, upper);
	}
	Vec3<T> toLocalDirection(const Vec3<T>& dirInWorld) const
	{
		return inverseOrientationMtx_ * dirInWorld;
	}
	Mat4x4<T> toLocalMtx() const
	{
		return Mat4x4<T>(
			inverseOrientationMtx_(0, 0), inverseOrientationMtx_(0, 1), inverseOrientationMtx_(0, 2), -translation_[0],
			inverseOrientationMtx_(1, 0), inverseOrientationMtx_(1, 1), inverseOrientationMtx_(1, 2), -translation_[1],
			inverseOrientationMtx_(2, 0), inverseOrientationMtx_(2, 1), inverseOrientationMtx_(2, 2), -translation_[2],
			                         0.0,                          0.0,                          0.0,              1.0
		);
	}

	Point3<T> toWorld(const Point3<T>& pointInLocal) const
	{
		return (orientationMtx_ * pointInLocal) + translation_;
	}
	Ray3<T> toWorld(const Ray3<T>& rayInLocal) const
	{
		return Ray3<T>(toWorld(rayInLocal.origin()), toWorldDirection(rayInLocal.direction()));
	}
	BBox3<T> toWorld(const BBox3<T>& bboxInLocal)  const
	{
		BBox3<T> bboxInWorld;
		for (int i = 0; i < 8; ++i)
		{
			Point3<T> cornerInWorld = toWorld(bboxInLocal.corner(i));
			bboxInWorld.lower() = Min(bboxInWorld.lower(), cornerInWorld);
			bboxInWorld.upper() = Max(bboxInWorld.upper(), cornerInWorld);
		}
		return bboxInWorld;
	}
	Vec3<T> toWorldDirection(const Vec3<T>& dirInLocal) const
	{
		return orientationMtx_ * dirInLocal;
	}
	Mat4x4<T> toWorldMtx() const
	{
		return Mat4x4<T>(
			orientationMtx_(0, 0), orientationMtx_(0, 1), orientationMtx_(0, 2), translation_[0],
			orientationMtx_(1, 0), orientationMtx_(1, 1), orientationMtx_(1, 2), translation_[1],
			orientationMtx_(2, 0), orientationMtx_(2, 1), orientationMtx_(2, 2), translation_[2],
	 	                      0.0,                   0.0,                   0.0,             1.0
		);
	}

private:
	void setTranslation_(const Vec3<T>& t)
	{
		translation_ = t;
	}
	void setOrientation_(const Quat<T>& o)
	{
		orientation_           = o;
		orientationMtx_        = o.matrix3();
		inverseOrientationMtx_ = o.inverse().matrix3();
	}

private:
	friend class boost::serialization::access;
	template <typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		ar & translation_;
		ar & orientation_;
		ar & orientationMtx_;
		ar & inverseOrientationMtx_;
	}
};
using Transform3f 	= Transform3<float>;
using Transform3d 	= Transform3<double>;

NAMESPACE_BASE_END

