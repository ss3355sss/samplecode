#pragma once
#include "base/pch.h"

#include "base/array.h"
#include "base/surface.h"

NAMESPACE_BASE_BEGIN
template <typename T>
inline Point3<T> ClosestPointOnLine(const Point3<T>& p0, const Point3<T>& p1, const Point3<T>& p)
{
	const T lenSqr = Point3<T>(p1 - p0).lengthSquared();
	if (lenSqr < Epsilon<T>())
	{
		return p0;
	}
	const T t = (Dot((p - p0), (p1 - p0))) / lenSqr;
	if (t < static_cast<T>(0))
	{
		return p0;
	}
	else if (t > static_cast<T>(1))
	{
		return p1;
	}

	return p0 + t * (p1 - p0);
}
template <typename T>
inline Normal3<T> ClosestNormalOnLine(const Point3<T>& p0, const Point3<T>& p1, const Normal3<T>& n0, const Normal3<T>& n1, const Point3<T>& p)
{
	const T lenSqr = Point3<T>(p1 - p0).lengthSquared();

	if (lenSqr < Epsilon<T>())
	{
		return n0;
	}

	const T t = (Dot((p - p0), (p1 - p0))) / lenSqr;
	if (t < 0.0f)
	{
		return n0;
	}
	else if (t > 1.0f)
	{
		return n1;
	}

	return (n0 + t * (n1 - n0)).normalized();
}

template <typename T>
class Triangle3 final : public Surface3<T>
{
public:
	typedef T value_type;
	Point3Array<T> 		points_;
	Normal3Array<T> 	normals_;
	TexCoord2Array<T>   uvs_;

public:
	Triangle3(const Point3Array<T>& points 		= Point3Array<T>(3, Point3<T>::Zero()),
			  const Normal3Array<T>& normals 	= Normal3Array<T>(3, Normal3<T>::Zero()),
			  const TexCoord2Array<T>& uvs 		= TexCoord2Array<T>(3, TexCoord2<T>::Zero()),
			  const Transform3<T>& transform 	= Transform3<T>(),
			  const bool isNormalFlipped 		= false
	)
		: Surface3<T>(transform, isNormalFlipped),
		  points_(points),
		  normals_(normals),
		  uvs_(uvs)
	{
	}
	Triangle3(const Triangle3& other) 				= default;
	Triangle3& operator=(const Triangle3& other) 	= default;
	virtual ~Triangle3() 							= default;

public:
	//bool operator ==(const Triangle3<T>& other) const
	//bool operator !=(const Triangle3<T>& other) const
public:
	const Point3Array<T>& points() const
	{
		return points_;
	}
	Point3Array<T>& points()
	{
		return points_;
	}
	const Normal3Array<T>& normals() const
	{
		return normals_;
	}
	Normal3Array<T>& normals()
	{
		return normals_;
	}
	const TexCoord2Array<T>& uvs() const
	{
		return uvs_;
	}
	TexCoord2Array<T>& uvs()
	{
		return uvs_;
	}

public:
	T area() const
	{
		return static_cast<T>(0.5) * Cross((points_[1] - points_[0]), (points_[2] - points_[0])).norm();
	}
	Normal3<T> faceNormal() const
	{
		Normal3<T> ret = Cross((points_[1] - points_[0]), (points_[2] - points_[0]));
		return ret.normalized();
	}
	void getBarycentricCoords(const Point3<T>& p, T* b0, T* b1, T* b2) const
	{
		Vec3<T> q01 = Cross((points_[1] - points_[0]), (p - points_[0]));
		Vec3<T> q12 = Cross((points_[2] - points_[1]), (p - points_[1]));
		Vec3<T> q02 = Cross((points_[0] - points_[2]), (p - points_[2]));

		 T a = area();
		*b0 = static_cast<T>(0.5) * q12.length() / a;
		*b1 = static_cast<T>(0.5) * q02.length() / a;
		*b2 = static_cast<T>(0.5) * q01.length() / a;
	}

protected:
	virtual Point3<T> closestPointLocal(const Point3<T>& pointInLocal) const
	{
		Normal3<T> n 	= faceNormal();
		T       nd 		= Dot(n, n);
		T       d  		= Dot(n, points_[0]);
		T       t  		= (d - Dot(n, pointInLocal)) / nd;

		Point3<T> q = t * n + pointInLocal;

		Vec3<T> q01 = Cross(points_[1] - points_[0], (q - points_[0]));
		if(Dot(n, q01) < static_cast<T>(0))
		{
			return ClosestPointOnLine(points_[0], points_[1], q);
		}

		Vec3<T> q12 = Cross((points_[2] - points_[1]), (q - points_[1]));
		if(Dot(n, q12) < static_cast<T>(0))
		{
			return ClosestPointOnLine(points_[1], points_[2], q);
		}

		Vec3<T> q02 = Cross((points_[0] - points_[2]), (q - points_[2]));
		if(Dot(n, q02) < static_cast<T>(0))
		{
			return ClosestPointOnLine(points_[0], points_[2], q);
		}

		T a  = area();
		T b0 = 0.5 * q12.length() / a;
		T b1 = 0.5 * q02.length() / a;
		T b2 = 0.5 * q01.length() / a;

		return b0 * points_[0] + b1 * points_[1] + b2 * points_[2];

	}
	virtual BBox3<T> bboxLocal() const
	{
		BBox3<T> box(points_[0], points_[1]);
		box.merge(points_[2]);
		return box;
	}
	virtual SurfaceRayIntersection3<T> closestIntersectionLocal(const Ray3<T>& rayInLocal) const
	{
		SurfaceRayIntersection3<T> intersection;
		Normal3<T>                n  = faceNormal();
		T                          nd = Dot(n, rayInLocal.direction());

		if(nd < Epsilon<T>())
		{
			intersection.isIntersecting = false;
			return intersection;
		}

		T d = Dot(n, points_[0]);
		T t = (d - Dot(n, rayInLocal.origin())) / nd;

		if(t < static_cast<T>(0))
		{
			intersection.isIntersecting = false;
			return intersection;
		}

		Point3<T> q = rayInLocal.pointAt(t);

		Vec3<T> q01 = Cross((points_[1] - points_[0]), (q - points_[0]));
		if(Dot(n, q01) <= static_cast<T>(0))
		{
			intersection.isIntersecting = false;
			return intersection;
		}

		Vec3<T> q12 = Cross((points_[2] - points_[1]), (q - points_[1]));
		if(Dot(n, q12) <= static_cast<T>(0))
		{
			intersection.isIntersecting = false;
			return intersection;
		}

		Vec3<T> q02 = cross((points_[0] - points_[2]), (q - points_[2]));
		if(Dot(n, q02) <= static_cast<T>(0))
		{
			intersection.isIntersecting = false;
			return intersection;
		}

		T a  = area();
		T b0 = 0.5 * q12.length() / a;
		T b1 = 0.5 * q02.length() / a;
		T b2 = 0.5 * q01.length() / a;

		Normal3<T> normal = b0 * normals_[0] + b1 * normals_[1] + b2 * normals_[2];

		intersection.isIntersecting = true;
		intersection.dist            = t;
		intersection.p               = q;
		intersection.n               = normal.normalized();

		return intersection;

	}
	virtual Normal3<T> closestNormalLocal(const Point3<T>& pointInLocal) const
	{
		Normal3<T> n  	= faceNormal();
		T       nd 		= Dot(n, n);
		T       d  		= Dot(n, points_[0]);
		T       t  		= (d - Dot(n, pointInLocal)) / nd;

		Point3<T> q = t * n + pointInLocal;

		Vec3<T> q01 = Cross((points_[1] - points_[0]), (q - points_[0]));
		if(Dot(n, q01) < 0)
		{
			return ClosestNormalOnLine(points_[0], points_[1], normals_[0], normals_[1], q);
		}

		Vec3<T> q12 = cross((points_[2] - points_[1]), (q - points_[1]));
		if(Dot(n, q12) < 0)
		{
			return ClosestNormalOnLine(points_[1], points_[2], normals_[1], normals_[2], q);
		}

		Vec3<T> q02 = cross((points_[0] - points_[2]), (q - points_[2]));
		if(Dot(n, q02) < 0)
		{
			return ClosestNormalOnLine(points_[0], points_[2], normals_[0], normals_[2], q);
		}

		double a  = area();
		double b0 = 0.5 * q12.length() / a;
		double b1 = 0.5 * q02.length() / a;
		double b2 = 0.5 * q01.length() / a;

		return (b0 * normals_[0] + b1 * normals_[1] + b2 * normals_[2]).normalized();
	}

protected:
	virtual bool intersectsLocal(const Ray3<T>& rayInLocal) const
	{
		Normal3<T> n  	= faceNormal();
		T       nd 		= Dot(n, rayInLocal.direction());

		if(nd < Epsilon<T>())
		{
			return false;
		}

		T d = Dot(n, points_[0]);
		T t = (d - Dot(n, rayInLocal.origin())) / nd;

		if(t < static_cast<T>(0))
		{
			return false;
		}

		Vec3<T> q = rayInLocal.pointAt(t);

		Vec3<T> q01 = Cross((points_[1] - points_[0]), (q - points_[0]));
		if(Dot(n, q01) <= static_cast<T>(0))
		{
			return false;
		}

		Vec3<T> q12 = Cross((points_[2] - points_[1]), (q - points_[1]));
		if(Dot(n, q12) <= static_cast<T>(0))
		{
			return false;
		}

		Vec3<T> q02 = Cross((points_[0] - points_[2]), (q - points_[2]));
		if(Dot(n, q02) <= static_cast<T>(0))
		{
			return false;
		}

		return true;
	}
};

using Triangle3f = Triangle3<float>;
using Triangle3d = Triangle3<double>;

NAMESPACE_BASE_END



