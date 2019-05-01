#pragma once

#include "base/pch.h"
#include "base/array.h"
#include "base/ray.h"

NAMESPACE_BASE_BEGIN
template<typename T>
struct BBoxRayIntersection3
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "BBoxRayIntersection3  only can be instantiated with floating point types");
public:
	bool isIntersecting = false;
	T    tNear          = Infinity<T>();
	T    tFar           = Infinity<T>();
};

using BBoxRayIntersection3f = BBoxRayIntersection3<float>;
using BBoxRayIntersection3d = BBoxRayIntersection3<double>;

template<typename T>
class BBox3 final
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "BBox3 only can be instantiated with floating point types");

private:
	Vec3<T> lower_;
	Vec3<T> upper_;

public:
	BBox3()
	{
		reset();
	}
	BBox3(const Vec3<T>& lower, const Vec3<T>& upper)
	{
		lower_[0] = std::min(lower[0], upper[0]);
		lower_[1] = std::min(lower[1], upper[1]);
		lower_[2] = std::min(lower[2], upper[2]);
		
		upper_[0] = std::max(lower[0], upper[0]);
		upper_[1] = std::max(lower[1], upper[1]);
		upper_[2] = std::max(lower[2], upper[2]);
	}
	BBox3(const BBox3& other) = default;
	BBox3& operator=(const BBox3& other)    = default;
	~BBox3() = default;

public:
	const Point3<T>& lower() const
	{
		return lower_;
	}
	Point3<T>& lower()
	{
		return lower_;
	}
	const Point3<T>& upper() const
	{
		return upper_;
	}
	Point3<T>& upper()
	{
		return upper_;
	}

public:
	T width() const
	{
		return upper_[0] - lower_[0];
	}
	T height() const
	{
		return upper_[1] - lower_[1];
		
	}
	T depth() const
	{
		return upper_[2] - lower_[2];
	}
	
	T length(size_t axis)
	{
		return upper_[axis] - lower_[axis];
	}
	bool overlaps(const BBox3& other) const
	{
		if(upper_[0] < other.lower_[0] || lower_[0] > other.upper_[0])
		{
			return false;
		}
		if(upper_[1] < other.lower_[1] || lower_[1] > other.upper_[1])
		{
			return false;
		}
		if(upper_[2] < other.lower_[2] || lower_[2] > other.upper_[2])
		{
			return false;
		}
		return true;
	}
	bool contains(const Point3<T>& point) const
	{
		if(upper_[0] < point[0] || lower_[0] > point[0])
		{
			return false;
		}
		if(upper_[1] < point[1] || lower_[1] > point[1])
		{
			return false;
		}
		if(upper_[2] < point[2] || lower_[2] > point[2])
		{
			return false;
		}
		return true;
	}
	bool intersects(const Ray3<T>& ray) const
	{
		T tMin = 0;
		T tMax = Infinity<T>();
		
		const Vec3<T>& rayDir    = ray.direction();
		const Vec3<T>& rayInvDir = Vec3<T>(static_cast<T>(1) / rayDir[0], static_cast<T>(1) / rayDir[1], static_cast<T>(1) / rayDir[2]);
		for (int i = 0; i < 3; ++i)
		{
			T tNear = (lower_[i] - ray.origin()[i]) * rayInvDir[i];
			T tFar  = (upper_[i] - ray.origin()[i]) * rayInvDir[i];
			
			if(tNear > tFar)
			{
				std::swap(tNear, tFar);
			}
			tMin = tNear > tMin ? tNear : tMin;
			tMax = tFar < tMax ? tFar : tMax;
			
			if(tMin > tMax)
			{
				return false;
			}
		}
		return true;
	}
	
	BBoxRayIntersection3<T> closestIntersection(const Ray3<T>& ray) const
	{
		BBoxRayIntersection3<T> intersection;
		
		T tMin = 0;
		T tMax = Infinity<T>();
		const Vec3<T>& rayDir    = ray.direction();
		const Vec3<T>& rayInvDir = Vec3<T>(static_cast<T>(1) / rayDir[0], static_cast<T>(1) / rayDir[1], static_cast<T>(1) / rayDir[2]);
		
		for (int i = 0; i < 3; ++i)
		{
			T tNear = (lower_[i] - ray.origin()[i]) * rayInvDir[i];
			T tFar  = (upper_[i] - ray.origin()[i]) * rayInvDir[i];
			
			if(tNear > tFar)
			{
				std::swap(tNear, tFar);
			}
			tMin = tNear > tMin ? tNear : tMin;
			tMax = tFar < tMax ? tFar : tMax;
			
			if(tMin > tMax)
			{
				intersection.isIntersecting = false;
				return intersection;
			}
		}
		
		intersection.isIntersecting = true;
		
		if(contains(ray.origin()))
		{
			intersection.tNear = tMax;
			intersection.tFar  = Infinity<T>();
		}
		else
		{
			intersection.tNear = tMin;
			intersection.tFar  = tMax;
		}
		
		return intersection;
	}
	
	Point3<T> midPoint() const
	{
		return (upper_ + lower_) / static_cast<T>(2);
	}
	T diagonalLength() const
	{
		return (upper_ - lower_).norm();
	}
	T diagonalLengthSquared() const
	{
		return (upper_ - lower_).squaredNorm();
	}
	void reset()
	{
		//lower_[0] = +Infinity<T>();
		//lower_[1] = +Infinity<T>();
		//lower_[2] = +Infinity<T>();
		//upper_[0] = -Infinity<T>();
		//upper_[1] = -Infinity<T>();
		//upper_[2] = -Infinity<T>();
		
		lower_[0] = lower_[1] = lower_[2] = static_cast<T>(0);
		upper_[0] = upper_[1] = upper_[2] = static_cast<T>(0);
	}
	
	void merge(const Point3<T>& p)
	{
		lower_[0] = std::min(lower_[0], p[0]);
		lower_[1] = std::min(lower_[1], p[1]);
		lower_[2] = std::min(lower_[2], p[2]);
		upper_[0] = std::max(upper_[0], p[0]);
		upper_[1] = std::max(upper_[1], p[1]);
		upper_[2] = std::max(upper_[2], p[2]);
	}
	void merge(const BBox3<T>& other)
	{
		lower_[0] = std::min(lower_[0], other.lower_[0]);
		lower_[1] = std::min(lower_[1], other.lower_[1]);
		lower_[2] = std::min(lower_[2], other.lower_[2]);
		upper_[0] = std::max(upper_[0], other.upper_[0]);
		upper_[1] = std::max(upper_[1], other.upper_[1]);
		upper_[2] = std::max(upper_[2], other.upper_[2]);
		
	}
	void expand(T delta)
	{
		lower_[0] -= delta;
		lower_[1] -= delta;
		lower_[2] -= delta;
		
		upper_[0] += delta;
		upper_[1] += delta;
		upper_[2] += delta;
	}
	
	Point3<T> corner(size_t idx) const
	{
		static const T       h         = static_cast<T>(1) / 2;
		static const Vec3<T> offset[8] = {
			{-h, -h, -h}, {+h, -h, -h}, {-h, +h, -h}, {+h, +h, -h},
			{-h, -h, +h}, {+h, -h, +h}, {-h, +h, +h}, {+h, +h, +h}
		};
		
		return Point3<T>(width() * offset[idx][0],
		                 height() * offset[idx][1],
		                 depth() * offset[idx][2]
		) + midPoint();
	}
	Point3<T> clamp(const Point3<T>& point) const
	{
		return Clamp(point, lower_, upper_);
	}
	bool isEmpty() const
	{
		return (
			lower_[0] >= upper_[0] ||
			lower_[1] >= upper_[1] ||
			lower_[2] >= upper_[2]
		);
		
	}

private:
	friend class boost::serialization::access;
	template<typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		ar & lower_;
		ar & upper_;
	}
	
};

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const BBox3<T>& bbox)
{
	os << "BBox3(lower=[";
	os << bbox.lower()[0] << ", ";
	os << bbox.lower()[1] << ", ";
	os << bbox.lower()[2] << "], ";
	
	os << "upper=[";
	os << bbox.upper()[0] << ", ";
	os << bbox.upper()[1] << ", ";
	os << bbox.upper()[2] << "])";
	return os;
}

using BBox3f = BBox3<float>;
using BBox3d = BBox3<double>;
inline void BuildExpandedBBoxGeom(const BBox3f&     bbox,
                                  Point3fArray*     pOutPointArray,
                                  Normal3fArray*    pOutNormalArray,
                                  TexCoord3fArray*  pOutUvArray,
                                  Int32Array*       pOutIndexArray,
                                  const int         indexOffset = 0
)
{
	enum
	{
		kFRONT, kBACK, kTOP, kBOTTOM, kLEFT, kRIGHT, nState
	};
	
	float minx = bbox.lower()[0];
	float miny = bbox.lower()[1];
	float minz = bbox.lower()[2];
	float maxx = bbox.upper()[0];
	float maxy = bbox.upper()[1];
	float maxz = bbox.upper()[2];
	
	Point3fArray points(8);
	
	points[0] = Point3f(minx, miny, maxz); //v0, front-bottom-left
	points[1] = Point3f(maxx, miny, maxz); //v1	front-bottom-right
	points[2] = Point3f(maxx, maxy, maxz); //v2	front-top-right
	points[3] = Point3f(minx, maxy, maxz); //v3	front-top-left
	points[4] = Point3f(minx, miny, minz); //v4  back-bottom-left
	points[5] = Point3f(maxx, miny, minz); //v5  back-bottom-right
	points[6] = Point3f(maxx, maxy, minz); //v6  back-top-right
	points[7] = Point3f(minx, maxy, minz); //v7  back-top-left
	
	(*pOutPointArray) = Point3fArray(36);
	
	/* front */
	(*pOutPointArray)[0]  = points[0];
	(*pOutPointArray)[1]  = points[1];
	(*pOutPointArray)[2]  = points[2];
	(*pOutPointArray)[3]  = points[2];
	(*pOutPointArray)[4]  = points[3];
	(*pOutPointArray)[5]  = points[0];
	
	/* back */
	(*pOutPointArray)[6]  = points[7];
	(*pOutPointArray)[7]  = points[6];
	(*pOutPointArray)[8]  = points[5];
	(*pOutPointArray)[9]  = points[5];
	(*pOutPointArray)[10] = points[4];
	(*pOutPointArray)[11] = points[7];
	
	/* bottom */
	(*pOutPointArray)[12] = points[1];
	(*pOutPointArray)[13] = points[5];
	(*pOutPointArray)[14] = points[4];
	(*pOutPointArray)[15] = points[4];
	(*pOutPointArray)[16] = points[0];
	(*pOutPointArray)[17] = points[1];
	
	/* top */
	(*pOutPointArray)[18] = points[3];
	(*pOutPointArray)[19] = points[2];
	(*pOutPointArray)[20] = points[6];
	(*pOutPointArray)[21] = points[6];
	(*pOutPointArray)[22] = points[7];
	(*pOutPointArray)[23] = points[3];
	
	/* right */
	(*pOutPointArray)[24] = points[1];
	(*pOutPointArray)[25] = points[5];
	(*pOutPointArray)[26] = points[6];
	(*pOutPointArray)[27] = points[6];
	(*pOutPointArray)[28] = points[2];
	(*pOutPointArray)[29] = points[1];
	
	/* left */
	(*pOutPointArray)[30] = points[4];
	(*pOutPointArray)[31] = points[0];
	(*pOutPointArray)[32] = points[3];
	(*pOutPointArray)[33] = points[3];
	(*pOutPointArray)[34] = points[7];
	(*pOutPointArray)[35] = points[4];
	
	Normal3fArray normals(nState);
	
	normals[kFRONT]  = Normal3f(+0.0f, +0.0f, +1.0f);
	normals[kBACK]   = Normal3f(+0.0f, +0.0f, -1.0f);
	normals[kTOP]    = Normal3f(+0.0f, +1.0f, +0.0f);
	normals[kBOTTOM] = Normal3f(+0.0f, -1.0f, +0.0f);
	normals[kLEFT]   = Normal3f(-1.0f, +0.0f, +0.0f);
	normals[kRIGHT]  = Normal3f(+1.0f, +0.0f, +0.0f);
	
	*pOutNormalArray = Normal3fArray(36);
	
	/* front */
	(*pOutNormalArray)[0]  = normals[kFRONT];
	(*pOutNormalArray)[1]  = normals[kFRONT];
	(*pOutNormalArray)[2]  = normals[kFRONT];
	(*pOutNormalArray)[3]  = normals[kFRONT];
	(*pOutNormalArray)[4]  = normals[kFRONT];
	(*pOutNormalArray)[5]  = normals[kFRONT];
	
	/* back */
	(*pOutNormalArray)[6]  = normals[kBACK];
	(*pOutNormalArray)[7]  = normals[kBACK];
	(*pOutNormalArray)[8]  = normals[kBACK];
	(*pOutNormalArray)[9]  = normals[kBACK];
	(*pOutNormalArray)[10] = normals[kBACK];
	(*pOutNormalArray)[11] = normals[kBACK];
	
	/* bottom */
	(*pOutNormalArray)[12] = normals[kBOTTOM];
	(*pOutNormalArray)[13] = normals[kBOTTOM];
	(*pOutNormalArray)[14] = normals[kBOTTOM];
	(*pOutNormalArray)[15] = normals[kBOTTOM];
	(*pOutNormalArray)[16] = normals[kBOTTOM];
	(*pOutNormalArray)[17] = normals[kBOTTOM];
	
	/* top */
	(*pOutNormalArray)[18] = normals[kTOP];
	(*pOutNormalArray)[19] = normals[kTOP];
	(*pOutNormalArray)[20] = normals[kTOP];
	(*pOutNormalArray)[21] = normals[kTOP];
	(*pOutNormalArray)[22] = normals[kTOP];
	(*pOutNormalArray)[23] = normals[kTOP];
	
	/* right */
	(*pOutNormalArray)[24] = normals[kRIGHT];
	(*pOutNormalArray)[25] = normals[kRIGHT];
	(*pOutNormalArray)[26] = normals[kRIGHT];
	(*pOutNormalArray)[27] = normals[kRIGHT];
	(*pOutNormalArray)[28] = normals[kRIGHT];
	(*pOutNormalArray)[29] = normals[kRIGHT];
	
	/* left */
	(*pOutNormalArray)[30] = normals[kLEFT];
	(*pOutNormalArray)[31] = normals[kLEFT];
	(*pOutNormalArray)[32] = normals[kLEFT];
	(*pOutNormalArray)[33] = normals[kLEFT];
	(*pOutNormalArray)[34] = normals[kLEFT];
	(*pOutNormalArray)[35] = normals[kLEFT];
	
	*pOutUvArray = TexCoord3fArray(36, TexCoord3f(0.0f, 0.0f, 0.0f));
	
	*pOutIndexArray = Int32Array(48);
	/* front */
	(*pOutIndexArray)[0]  = 0 + indexOffset;
	(*pOutIndexArray)[1]  = 1 + indexOffset;
	(*pOutIndexArray)[2]  = 2 + indexOffset;
	(*pOutIndexArray)[3]  = 0xffffffff;
	(*pOutIndexArray)[4]  = 3 + indexOffset;
	(*pOutIndexArray)[5]  = 4 + indexOffset;
	(*pOutIndexArray)[6]  = 5 + indexOffset;
	(*pOutIndexArray)[7]  = 0xffffffff;
	
	/* back */
	(*pOutIndexArray)[8]  = 6 + indexOffset;
	(*pOutIndexArray)[9]  = 7 + indexOffset;
	(*pOutIndexArray)[10] = 8 + indexOffset;
	(*pOutIndexArray)[11] = 0xffffffff;
	(*pOutIndexArray)[12] = 9 + indexOffset;
	(*pOutIndexArray)[13] = 10 + indexOffset;
	(*pOutIndexArray)[14] = 11 + indexOffset;
	(*pOutIndexArray)[15] = 0xffffffff;
	
	/* bottom */
	(*pOutIndexArray)[16] = 12 + indexOffset;
	(*pOutIndexArray)[17] = 13 + indexOffset;
	(*pOutIndexArray)[18] = 14 + indexOffset;
	(*pOutIndexArray)[19] = 0xffffffff;
	(*pOutIndexArray)[20] = 15 + indexOffset;
	(*pOutIndexArray)[21] = 16 + indexOffset;
	(*pOutIndexArray)[22] = 17 + indexOffset;
	(*pOutIndexArray)[23] = 0xffffffff;
	
	/* top */
	(*pOutIndexArray)[24] = 18 + indexOffset;
	(*pOutIndexArray)[25] = 19 + indexOffset;
	(*pOutIndexArray)[26] = 20 + indexOffset;
	(*pOutIndexArray)[27] = 0xffffffff;
	(*pOutIndexArray)[28] = 21 + indexOffset;
	(*pOutIndexArray)[29] = 22 + indexOffset;
	(*pOutIndexArray)[30] = 23 + indexOffset;
	(*pOutIndexArray)[31] = 0xffffffff;
	
	/* right */
	(*pOutIndexArray)[32] = 24 + indexOffset;
	(*pOutIndexArray)[33] = 25 + indexOffset;
	(*pOutIndexArray)[34] = 26 + indexOffset;
	(*pOutIndexArray)[35] = 0xffffffff;
	(*pOutIndexArray)[36] = 27 + indexOffset;
	(*pOutIndexArray)[37] = 28 + indexOffset;
	(*pOutIndexArray)[38] = 29 + indexOffset;
	(*pOutIndexArray)[39] = 0xffffffff;
	
	/* left */
	(*pOutIndexArray)[40] = 30 + indexOffset;
	(*pOutIndexArray)[41] = 31 + indexOffset;
	(*pOutIndexArray)[42] = 32 + indexOffset;
	(*pOutIndexArray)[43] = 0xffffffff;
	(*pOutIndexArray)[44] = 33 + indexOffset;
	(*pOutIndexArray)[45] = 34 + indexOffset;
	(*pOutIndexArray)[46] = 35 + indexOffset;
	(*pOutIndexArray)[47] = 0xffffffff;
	
	
}




NAMESPACE_BASE_END
