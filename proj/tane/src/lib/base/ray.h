#pragma once
#include "base/pch.h"
#include "base/vector.h"

NAMESPACE_BASE_BEGIN
template<typename T>
class Ray3 final
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "Ray3 only can be instantiated with floating point types");

private:
	Point3 <T> origin_;
	Vec3 <T> direction_;

public:
	Ray3(const Point3 <T>& o = Point3<T>::Zero(), const Vec3 <T>& d = Vec3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)))
		: origin_(o), direction_(d.normalized())
	{
	}
	Ray3(const Ray3& other) = default;
	~Ray3()                 = default;

public:
	const Point3 <T>& origin() const
	{
		return origin_;
	}
	Point3 <T>& origin()
	{
		return origin_;
	}
	const Vec3 <T>& direction() const
	{
		return direction_;
	}
	Vec3 <T>& direction()
	{
		return direction_;
	}

public:
	Point3 <T> pointAt(T t) const
	{
		return origin_ + t * direction_;
	}

private:
	friend class boost::serialization::access;
	template<typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		ar & origin_;
		ar & direction_;
	}
};

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Ray3<T>& r)
{
	os << "Ray3(origin = [";
	os << r.origin()[0] << ", ";
	os << r.origin()[1] << ", ";
	os << r.origin()[2] << "], ";
	
	os << "direction = [";
	os << r.direction()[0] << ", ";
	os << r.direction()[1] << ", ";
	os << r.direction()[2] << "])";
	return os;
}

using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;

NAMESPACE_BASE_END


