#pragma once
#include "base/pch.h"
#include "base/bvh.h"
#include "base/surface.h"
#include "base/triangle.h"

NAMESPACE_BASE_BEGIN
template<typename T>
class TriangleMesh3 final : public Surface3<T>
{
public:
	typedef Triangle3<T>                                        Triangle3T;
	typedef Surface3<T>                                         Base;
	typedef Bvh3<int      /* Item Type (Triangle Index) */, T>  Bvh3T;
	
private:
	Point3Array<T>      pointArray_;
	Normal3Array<T>     normalArray_;
	TexCoord2Array<T>   uvArray_;
	Int3Array           indexArray_;
	
	mutable bool    bvhInvalidated_ = true;
	mutable Bvh3T   bvh_;
public:
	TriangleMesh3(const Point3Array<T>&     pointArray      = Point3Array<T>(),
	              const Normal3Array<T>&    normalArray     = Normal3Array<T>(), /* per-vertex normals */
	              const TexCoord2Array<T>&  uvArray         = TexCoord2Array<T>(),
				  const Int3Array&          indexArray      = Int3Array(),
	              const Transform3<T>&      transform       = Transform3<T>(),
	              const bool                isNormalFlipped = false
	): Base(transform, isNormalFlipped),
	   pointArray_(pointArray),
	   normalArray_(normalArray),
	   uvArray_(uvArray),
       indexArray_(indexArray)
	{
	}
	virtual ~TriangleMesh3()
	{
	
	}
public:
	const Point3Array<T>& pointArray() const
	{
		return pointArray_;
	}
	const Normal3Array<T>& normalArray() const
	{
		return normalArray_;
	}
	const TexCoord2Array<T>& uvArray() const
	{
		return uvArray_;
	}
	const Int3Array& indexArray() const
	{
		return indexArray_;
	}

	Point3Array<T>& pointArray()
	{
		return pointArray_;
	}
	Normal3Array<T>& normalArray()
	{
		return normalArray_;
	}
	TexCoord2Array<T>& uvArray()
	{
		return uvArray_;
	}
	Int3Array& indexArray()
	{
		return indexArray_;
	}


public:
	int npoints() const
	{
		return pointArray_.size();
	}
	int nfaces() const
	{
		return indexArray_.size();
	}
	bool hasNormal() const
	{
		return normalArray_.size() > 0;
	}
	bool hasUV() const
	{
		return uvArray_.size() > 0;
	}
	T area() const
	{
		//total area
		T sum = 0;
		for(int fid = 0; fid < nfaces(); ++fid)
		{
			sum += triangle(fid).area();
		}
		return sum;
	}
	T volume() const
	{
		//total volume
		T sum   = 0;
		for(int fid = 0; fid < nfaces(); ++fid)
		{
			const Triangle3<T>& tri = triangle(fid).area();
			sum += Dot(tri.points()[0], Cross(tri.points()[1], tri.points()[2])) / static_cast<T>(6);
		}
		return sum;
	}
	Triangle3<T> triangle(const int fid) const
	{
		
		Triangle3<T> tri;

		tri.points()[0] = pointArray_[indexArray_[fid][0]];
		tri.points()[1] = pointArray_[indexArray_[fid][1]];
		tri.points()[2] = pointArray_[indexArray_[fid][2]];

		if(hasNormal())
		{
			tri.normals()[0] = normalArray_[indexArray_[fid][0]];
			tri.normals()[1] = normalArray_[indexArray_[fid][1]];
			tri.normals()[2] = normalArray_[indexArray_[fid][2]];
		}
		else
		{
			const Normal3<T> faceNormal = tri.faceNormal();
			tri.normals()[0] = faceNormal;
			tri.normals()[1] = faceNormal;
			tri.normals()[2] = faceNormal;
		}
		
		if(hasUV())
		{
			tri.uvs()[0] = uvArray_[indexArray_[fid][0]];
			tri.uvs()[1] = uvArray_[indexArray_[fid][1]];
		}
		else
		{
		
			tri.uvs()[0] = TexCoord2<T>(0.0, 0.0);
			tri.uvs()[1] = TexCoord2<T>(0.0, 0.0);
		}
		return tri;
	}
public:
	void clear()
	{
		/* clear array */
		pointArray_ .clear();
		normalArray_.clear();
		uvArray_    .clear();
		indexArray_ .clear();
		
		/* clear bvh */
		invalidateBvh_();
	}


public:
	virtual void updateQueryEngine()
	{
		buildBvh_();
	}


protected:
	virtual Point3<T> closestPointLocal(const Point3<T>& pointInLocal) const
	{
		buildBvh_();
		const auto distanceFunc = [this](const int32_t& triIdx, const Point3<T>& pt)
		{
			Triangle3<T> tri = triangle(triIdx);
			return tri.closestDistance(pt);
		};

		const auto queryResult = bvh_.nearest(pointInLocal, distanceFunc);

		return triangle(*queryResult.item).closestPoint(pointInLocal);
	}
	virtual BBox3<T> bboxLocal() const
	{
		buildBvh_();
		return bvh_.bound();
	}
	virtual SurfaceRayIntersection3<T> closestIntersectionLocal(const Ray3<T>& rayInLocal) const
	{
		buildBvh_();

		const auto testFunc = [this](const int32_t& triIdx, const Ray3<T>& ray)
		{
			Triangle3<T>               tri    = triangle(triIdx);
			SurfaceRayIntersection3<T> result = tri.closestIntersection(ray);
			return result.dist;
		};

		const auto queryResult = bvh_.closestIntersection(rayInLocal, testFunc);

		SurfaceRayIntersection3<T> result;
		result.dist           = queryResult.dist;
		result.isIntersecting = queryResult.item != nullptr;

		if(queryResult.item != nullptr)
		{
			result.p = rayInLocal.pointAt(queryResult.dist);
			result.n = triangle(*queryResult.item).closestNormal(result.p);
		}
		return result;

	}
	virtual Normal3<T> closestNormalLocal(const Point3<T>& pointInLocal) const
	{
		buildBvh_();

		const auto distanceFunc = [this](const int32_t& triIdx, const Vec3<T>& pt)
		{
			Triangle3<T> tri = triangle(triIdx);
			return tri.closestDistance(pt);
		};

		const auto queryResult = bvh_.nearest(pointInLocal, distanceFunc);
		return triangle(*queryResult.item).closestNormal(pointInLocal);
	}

protected:
	virtual bool intersectsLocal(const Ray3<T>& rayInLocal) const
	{
		buildBvh_();
		const auto testFunc = [this](const int32_t& triIdx, const Ray3<T>& ray)
		{
			Triangle3<T> tri = triangle(triIdx);
			return tri.intersects(ray);
		};

		return bvh_.intersects(rayInLocal, testFunc);

	}
	virtual T closestDistanceLocal(const Point3<T>& pointInLocal) const
	{
		buildBvh_();

		const auto distanceFunc = [this](const int32_t& triIdx, const Vec3<T>& pt)
		{
			Triangle3<T> tri = triangle(triIdx);
			return tri.closestDistance(pt);
		};

		const auto queryResult = bvh_.nearest(pointInLocal, distanceFunc);
		return queryResult.dist;
	}

private:
	void invalidateBvh_()
	{
		bvhInvalidated_ = true;
	}
	void buildBvh_() const
	{
		if(bvhInvalidated_)
		{
			size_t ntriangles = nfaces();

			typename Bvh3T::ItemContainerT  ids(ntriangles);
			typename Bvh3T::BoundContainerT bounds(ntriangles);

			/* TODO::do parallel */
			for (size_t i = 0; i < ntriangles; ++i)
			{
				ids[i]    = i;
				bounds[i] = triangle(i).bbox();
			}
			bvh_.build(ids, bounds);
			bvhInvalidated_ = false;
		}

	}
private:
	friend class boost::serialization::access;
	template<typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		//TODO::Serialize TriangleMesh3
	}
};

using TriangleMesh3f = TriangleMesh3<float>;
using TriangleMesh3d = TriangleMesh3<double>;

NAMESPACE_BASE_END



