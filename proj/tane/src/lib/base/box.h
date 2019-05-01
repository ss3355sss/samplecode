#pragma once

#include "base/pch.h"

#include "base/ray.h"
#include "base/bbox.h"
#include "base/transform.h"
#include "base/plane.h"
#include "base/surface.h"

NAMESPACE_BASE_BEGIN
template<typename T>
class Box3 final : public Surface3<T>
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "Box3 only can be instantiated with floating point types");

private:
	BBox3<T> bound_;

public:
	Box3(const BBox3<T>& bbox           = BBox3<T>(Vec3<T>(0, 0, 0), Vec3<T>(1, 1, 1)),
	     const Transform3<T>& transform = Transform3<T>(),
	     bool isNormalFlipped           = false)
		: Surface3<T>(transform, isNormalFlipped), bound_(bbox)
	{

	}
	Box3(const Point3<T>& lower,
	     const Point3<T>& upper,
	     const Transform3<T>& transform = Transform3<T>(),
	     bool isNormalFlipped = false)
		: Surface3<T>(transform, isNormalFlipped), bound_(BBox3<T>(lower, upper))
	{

	}

	Box3(const Box3& other)             = default;
	Box3& operator=(const Box3& other)  = default;
	virtual ~Box3() = default;

public:
	const BBox3<T>& bound() const
	{
		return bound_;
	}
	BBox3<T>& bound()
	{
		return bound_;
	}
protected:
	virtual Point3<T> closestPointLocal(const Point3<T>& p) const
	{
		if(bound_.contains(p))
		{
			Plane3<T> planes[] = {
				Plane3<T>(Vec3<T>(1, 0, 0), bound_.upper()),
				Plane3<T>(Vec3<T>(0, 1, 0), bound_.upper()),
				Plane3<T>(Vec3<T>(0, 0, 1), bound_.upper()),
				Plane3<T>(Vec3<T>(-1, 0, 0), bound_.lower()),
				Plane3<T>(Vec3<T>(0, -1, 0), bound_.lower()),
				Plane3<T>(Vec3<T>(0, 0, -1), bound_.lower())
			};

			Vec3<T> result = planes[0].closestPoint(p);
			T distanceSquared = result.distanceSquaredTo(p);

			for (int i = 1; i < 6; ++i)
			{
				Vec3<T> localResult = planes[i].closestPoint(p);
				T localDistanceSquared = localResult.distanceSquaredTo(p);

				if(localDistanceSquared < distanceSquared)
				{
					result = localResult;
					distanceSquared = localDistanceSquared;
				}
			}

			return result;
		}
		else
		{
			return Clamp(p, bound_.lower(), bound_.upper());
		}
	};
	virtual BBox3<T> bboxLocal() const
	{
		return bound_;
	}
	virtual bool intersectsLocal(const Ray3<T>& rayInLocal) const
	{
		return bound_.intersects(rayInLocal);
	};
	virtual SurfaceRayIntersection3<T> closestIntersectionLocal(const Ray3<T>& ray) const
	{
		SurfaceRayIntersection3<T> intersection;
		BBoxRayIntersection3<T> bbRayIntersection = bound_.closestIntersection(ray);
		intersection.isIntersecting = bbRayIntersection.isIntersecting;
		if(intersection.isIntersecting)
		{
			intersection.dist = bbRayIntersection.tNear;
			intersection.p = ray.pointAt(bbRayIntersection.tNear);
			intersection.n = Surface3<T>::closestNormal(intersection.p);
		}
		return intersection;
	};
	virtual Normal3<T> closestNormalLocal(const Point3<T>& p) const
	{
		Plane3<T> planes[6] = {
			Plane3<T>(Vec3<T>(1, 0, 0), bound_.upper()),
			Plane3<T>(Vec3<T>(0, 1, 0), bound_.upper()),
			Plane3<T>(Vec3<T>(0, 0, 1), bound_.upper()),
			Plane3<T>(Vec3<T>(-1, 0, 0), bound_.lower()),
			Plane3<T>(Vec3<T>(0, -1, 0), bound_.lower()),
			Plane3<T>(Vec3<T>(0, 0, -1), bound_.lower())
		};

		if(bound_.contains(p))
		{
			Vec3<T> closestNormal = planes[0].n();
			Vec3<T> closestPoint = planes[0].closestPoint(p);
			T minDistanceSquared = Vec3<T>(closestPoint - p).lengthSquared();

			for (int i = 1; i < 6; ++i)
			{
				Vec3 <T> localClosestPoint = planes[i].closestPoint(p);
				T localDistanceSquared = Vec3<T>(localClosestPoint - p).lengthSquared();

				if(localDistanceSquared < minDistanceSquared)
				{
					closestNormal = planes[i].n();
					minDistanceSquared = localDistanceSquared;
				}
			}

			return closestNormal;
		}
		else
		{
			Vec3<T> closestPoint = Clamp(p, bound_.lower(), bound_.upper());
			Vec3<T> closestPointToInputPoint = p - closestPoint;
			Vec3<T> closestNormal = planes[0].n();
			T maxCosineAngle = Dot(closestNormal, closestPointToInputPoint);

			for (int i = 1; i < 6; ++i)
			{
				T cosineAngle = Dot(planes[i].n(), closestPointToInputPoint);
				if(cosineAngle > maxCosineAngle)
				{
					closestNormal = planes[i].n();
					maxCosineAngle = cosineAngle;
				}
			}

			return closestNormal;
		}
	};


};
using Box3f = Box3<float>;
using Box3d = Box3<double>;


NAMESPACE_BASE_END