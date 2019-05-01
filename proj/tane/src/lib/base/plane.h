#pragma once

#include "base/pch.h"

#include "base/surface.h"

NAMESPACE_BASE_BEGIN
template<typename T>
class Plane3 final : public Surface3<T>
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "Plane3 only can be instantiated with floating point types");

private:
	Normal3<T> n_;
	Point3<T>  p_;

public:
	Plane3(const Normal3<T>& n = Normal3<T>(0, 1, 0), const Point3<T>& p = Point3<T>::Zero(), const Transform3<T>& transform = Transform3<T>(), bool isNormalFlipped = false)
		: Surface3<T>(transform, isNormalFlipped), p_(p), n_(n)
	{
	}
	Plane3(const Point3<T>& p0, const Point3<T>& p1, const Point3<T>& p2, const Transform3<T>& transform = Transform3<T>(), bool isNormalFlipped = false)
		: Surface3<T>(transform, isNormalFlipped)
	{
		n_ = Cross((p1 - p0), (p2 - p0)).normalized();
		p_ = p0;
	}
	Plane3(const Plane3& other) 			= default;
	Plane3& operator=(const Plane3& other)  = default;
	virtual ~Plane3() 						= default;

public:
	const Point3<T>& p() const
	{
		return p_;
	}
	Point3<T>& p()
	{
		return p_;
	}
	const Normal3<T>& n() const
	{
		return n_;
	}
	Normal3<T>& n()
	{
		return n_;
	}


protected:
	virtual Point3<T> closestPointLocal(const Point3<T>& p) const
	{
		Vec3<T> r = p - p_;
		return r - Dot(n_, r) * n_ + p_;

	};
	virtual BBox3<T> bboxLocal() const
	{
		if(std::fabs(Dot(n_, Vec3<T>(1, 0, 0)) - 1) < Epsilon<T>())
		{
			return BBox3<T>(p_ - Vec3<T>(0, Max<T>(), Max<T>()), p_ + Vec3<T>(0, Max<T>(), Max<T>()));
		}
		if(std::fabs(Dot(n_, Vec3<T>(0, 1, 0)) - 1) < Epsilon<T>())
		{
			return BBox3<T>(p_ - Vec3<T>(Max<T>(), 0, Max<T>()), p_ + Vec3<T>(Max<T>(), 0, Max<T>()));
		}
		if(std::fabs(Dot(n_, Vec3<T>(0, 0, 1)) - 1) < Epsilon<T>())
		{
			return BBox3<T>(p_ - Vec3<T>(Max<T>(), Max<T>(), 0), p_ + Vec3<T>(Max<T>(), Max<T>(), 0));
		}
		return BBox3<T>(Vec3<T>(Max<T>(), Max<T>(), Max<T>()), Vec3<T>(Max<T>(), Max<T>(), Max<T>()));
	};
	virtual bool intersectsLocal(const Ray3<T>& ray) const
	{
		return std::fabs(Dot(ray.direction(), n_)) > static_cast<T>(0);
	};
	virtual T closestDistanceLocal(const Point3<T>& p) const
	{
	};
	virtual SurfaceRayIntersection3<T> closestIntersectionLocal(const Ray3<T>& ray) const
	{
		SurfaceRayIntersection3<T> intersection;

		T dDotN = Dot(ray.direction(), n_);

		/* Check if not parallel */
		if (std::fabs(dDotN) > 0)
		{
			Vec3<T> diff = p_ - ray.origin();
			T t = Dot(n_, diff) / dDotN;
			if (t >= static_cast<T>(0))
			{
				intersection.isIntersecting = true;
				intersection.dist 	= t;
				intersection.p 		= ray.pointAt(t);
				intersection.n 		= n_;
			}
		}
	};
	virtual Normal3<T> closestNormalLocal(const Point3<T>& p) const
	{
		UNUSED_VARIABLE(p);
		return n_;
	};

};

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Plane3<T>& plane)
{
	os << "Plane3(";
	os << "p=[" << plane.p()[0] << " " << plane.p()[1] <<" " << plane.p()[2] << "]";
	os << ", ";
	os << "n=[" << plane.n()[0] << " " << plane.n()[1] <<" " << plane.n()[2] << "]";
	os << ")";


	return os;
}

using Plane3f = Plane3<float>;
using Plane3d = Plane3<double>;


NAMESPACE_BASE_END



