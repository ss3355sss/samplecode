#pragma once
#include "base/pch.h"

#include "base/ray.h"
#include "base/bbox.h"
#include "base/transform.h"

///* --------------------------------------------------------------------- OpenMesh Trait for supporting Eigen*/
//NAMESPACE_BASE_BEGIN
//template<typename T>
//struct EigenTrait3 : public OpenMesh::DefaultTraits
//{
//private:
//	STATIC_ASSSERT_FLOATING_POINT(T, "EigenTrait3 only can be instantiated with floating point types");
//
//public:
//	using Point         = Point3<T>;
//	using Normal        = Normal3<T>;
//	using TexCoord1D    = TexCoord1<T>;
//	using TexCoord2D    = TexCoord2<T>;
//	using TexCoord3D    = TexCoord3<T>;
//	using Scalar        = T;
//	//using TextureIndex  = int;
//
//#ifdef Status
//#define Status_WORKAROUND Status //workaround Xlib.h Status macro
//#undef Status
//#endif
//	VertexAttributes(
//		OpenMesh::Attributes::TextureIndex |
//		OpenMesh::Attributes::TexCoord2D |
//		OpenMesh::Attributes::Normal |
//		OpenMesh::Attributes::Status          //for delete
//	);
//	FaceAttributes(
//		OpenMesh::Attributes::Normal |
//		OpenMesh::Attributes::Status          //for delete
//	);
//	EdgeAttributes(
//		OpenMesh::Attributes::Status          //for delete
//	);
//	HalfedgeAttributes(
//		//OpenMesh::Attributes::Normal |
//		OpenMesh::Attributes::Status |        //for delete
//		OpenMesh::Attributes::PrevHalfedge
//	);
//#ifdef Status_WORKAROUND
//#define Status Status_workaround
//#endif
//};
//
//template<typename T>
//using PolyMeshKernel3 = OpenMesh::PolyMesh_ArrayKernelT<EigenTrait3<T>>;
//template<typename T>
//using TriMeshKernel3 = OpenMesh::TriMesh_ArrayKernelT<EigenTrait3<T>>;
//
//#define DECL_OPENMESH_TYPE_ALIASING(KERNEL)                                             \
//	/* Handle Alias typedef */                                                          \
//	typedef typename KERNEL::VertexHandle                  VertexHandle                ;\
//	typedef typename KERNEL::HalfedgeHandle                HalfedgeHandle              ;\
//	typedef typename KERNEL::EdgeHandle                    EdgeHandle                  ;\
//	typedef typename KERNEL::FaceHandle                    FaceHandle                  ;\
//	/* Iterator Alias typedef */                                                        \
//	typedef typename KERNEL::VertexIter                    VertexIter                  ;\
//	typedef typename KERNEL::HalfedgeIter                  HalfedgeIter                ;\
//	typedef typename KERNEL::EdgeIter                      EdgeIter                    ;\
//	typedef typename KERNEL::FaceIter                      FaceIter                    ;\
//	typedef typename KERNEL::ConstVertexIter               ConstVertexIter             ;\
//	typedef typename KERNEL::ConstHalfedgeIter             ConstHalfedgeIter           ;\
//	typedef typename KERNEL::ConstEdgeIter                 ConstEdgeIter               ;\
//	typedef typename KERNEL::ConstFaceIter                 ConstFaceIter               ;\
//	typedef typename KERNEL::VertexVertexIter              VertexVertexIter            ;\
//	typedef typename KERNEL::VertexVertexCWIter            VertexVertexCWIter          ;\
//	typedef typename KERNEL::VertexVertexCCWIter           VertexVertexCCWIter         ;\
//	typedef typename KERNEL::VertexOHalfedgeIter           VertexOHalfedgeIter         ;\
//	typedef typename KERNEL::VertexOHalfedgeCWIter         VertexOHalfedgeCWIter       ;\
//	typedef typename KERNEL::VertexOHalfedgeCCWIter        VertexOHalfedgeCCWIter      ;\
//	typedef typename KERNEL::VertexIHalfedgeIter           VertexIHalfedgeIter         ;\
//	typedef typename KERNEL::VertexIHalfedgeCWIter         VertexIHalfedgeCWIter       ;\
//	typedef typename KERNEL::VertexIHalfedgeCCWIter        VertexIHalfedgeCCWIter      ;\
//	typedef typename KERNEL::VertexEdgeIter                VertexEdgeIter              ;\
//	typedef typename KERNEL::VertexEdgeCWIter              VertexEdgeCWIter            ;\
//	typedef typename KERNEL::VertexEdgeCCWIter             VertexEdgeCCWIter           ;\
//	typedef typename KERNEL::VertexFaceIter                VertexFaceIter              ;\
//	typedef typename KERNEL::VertexFaceCWIter              VertexFaceCWIter            ;\
//	typedef typename KERNEL::VertexFaceCCWIter             VertexFaceCCWIter           ;\
//	typedef typename KERNEL::FaceVertexIter                FaceVertexIter              ;\
//	typedef typename KERNEL::FaceVertexCWIter              FaceVertexCWIter            ;\
//	typedef typename KERNEL::FaceVertexCCWIter             FaceVertexCCWIter           ;\
//	typedef typename KERNEL::FaceHalfedgeIter              FaceHalfedgeIter            ;\
//	typedef typename KERNEL::FaceHalfedgeCWIter            FaceHalfedgeCWIter          ;\
//	typedef typename KERNEL::FaceHalfedgeCCWIter           FaceHalfedgeCCWIter         ;\
//	typedef typename KERNEL::FaceEdgeIter                  FaceEdgeIter                ;\
//	typedef typename KERNEL::FaceEdgeCWIter                FaceEdgeCWIter              ;\
//	typedef typename KERNEL::FaceEdgeCCWIter               FaceEdgeCCWIter             ;\
//	typedef typename KERNEL::FaceFaceIter                  FaceFaceIter                ;\
//	typedef typename KERNEL::ConstVertexVertexIter         ConstVertexVertexIter       ;\
//	typedef typename KERNEL::ConstVertexVertexCWIter       ConstVertexVertexCWIter     ;\
//	typedef typename KERNEL::ConstVertexVertexCCWIter      ConstVertexVertexCCWIter    ;\
//	typedef typename KERNEL::ConstVertexOHalfedgeIter      ConstVertexOHalfedgeIter    ;\
//	typedef typename KERNEL::ConstVertexOHalfedgeCWIter    ConstVertexOHalfedgeCWIter  ;\
//	typedef typename KERNEL::ConstVertexOHalfedgeCCWIter   ConstVertexOHalfedgeCCWIter ;\
//	typedef typename KERNEL::ConstVertexIHalfedgeIter      ConstVertexIHalfedgeIter    ;\
//	typedef typename KERNEL::ConstVertexIHalfedgeCWIter    ConstVertexIHalfedgeCWIter  ;\
//	typedef typename KERNEL::ConstVertexIHalfedgeCCWIter   ConstVertexIHalfedgeCCWIter ;\
//	typedef typename KERNEL::ConstVertexEdgeIter           ConstVertexEdgeIter         ;\
//	typedef typename KERNEL::ConstVertexEdgeCWIter         ConstVertexEdgeCWIter       ;\
//	typedef typename KERNEL::ConstVertexEdgeCCWIter        ConstVertexEdgeCCWIter      ;\
//	typedef typename KERNEL::ConstVertexFaceIter           ConstVertexFaceIter         ;\
//	typedef typename KERNEL::ConstVertexFaceCWIter         ConstVertexFaceCWIter       ;\
//	typedef typename KERNEL::ConstVertexFaceCCWIter        ConstVertexFaceCCWIter      ;\
//	typedef typename KERNEL::ConstFaceVertexIter           ConstFaceVertexIter         ;\
//	typedef typename KERNEL::ConstFaceVertexCWIter         ConstFaceVertexCWIter       ;\
//	typedef typename KERNEL::ConstFaceVertexCCWIter        ConstFaceVertexCCWIter      ;\
//	typedef typename KERNEL::ConstFaceHalfedgeIter         ConstFaceHalfedgeIter       ;\
//	typedef typename KERNEL::ConstFaceHalfedgeCWIter       ConstFaceHalfedgeCWIter     ;\
//	typedef typename KERNEL::ConstFaceHalfedgeCCWIter      ConstFaceHalfedgeCCWIter    ;\
//	typedef typename KERNEL::ConstFaceEdgeIter             ConstFaceEdgeIter           ;\
//	typedef typename KERNEL::ConstFaceEdgeCWIter           ConstFaceEdgeCWIter         ;\
//	typedef typename KERNEL::ConstFaceEdgeCCWIter          ConstFaceEdgeCCWIter        ;\
//	typedef typename KERNEL::ConstFaceFaceIter             ConstFaceFaceIter           ;\
//	typedef typename KERNEL::ConstFaceFaceCWIter           ConstFaceFaceCWIter         ;\
//	typedef typename KERNEL::ConstFaceFaceCCWIter          ConstFaceFaceCCWIter        ;\
//
//
//NAMESPACE_BASE_END

/* --------------------------------------------------------------------- Surface (Base)*/
NAMESPACE_BASE_BEGIN
template <typename T>
struct SurfaceRayIntersection3
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "SurfaceRayIntersection3 only can be instantiated with floating point types");

public:
	bool 		isIntersecting 		= false;
	T 			dist 				= Infinity<T>();
	Point3<T>	p;
	Normal3<T>	n;
};
using SurfaceRayIntersection3f = SurfaceRayIntersection3<float>;
using SurfaceRayIntersection3d = SurfaceRayIntersection3<double>;

template <typename T>
class Surface3
{
private:
	STATIC_ASSSERT_FLOATING_POINT(T, "Surface3 only can be instantiated with floating point types");

private:
	bool 			isNormalFlipped_;
	Transform3<T>	transform_;

public:
	Surface3(const Transform3<T>& transform = Transform3<T>(), const bool isNormalFlipped = false)
		: transform_(transform), isNormalFlipped_(isNormalFlipped)
	{
	}
	Surface3(const Surface3& other) 			= default;
	Surface3& operator=(const Surface3& other)	= default;
	virtual ~Surface3() 						= default;

public:
	const bool& isNormalFlipped() const
	{
		return isNormalFlipped_;
	}
	bool& isNormalFlipped()
	{
		return isNormalFlipped_;
	}
	const Transform3<T>& transform() const
	{
		return transform_;
	}
	Transform3<T>& transform()
	{
		return transform_;
	}

public:
	virtual void updateQueryEngine()
	{
		/* do nothing */
	}

public:
	Point3<T> closestPoint(const Point3<T>& p) const
	{
		return transform_.toWorld(closestPointLocal(transform_.toLocal(p)));
	}
	BBox3<T> bbox() const
	{
		return transform_.toWorld(bboxLocal());
	}
	bool intersects(const Ray3<T>& rayInWorld) const
	{
		return intersectsLocal(transform_.toLocal(rayInWorld));
	}
	T closestDistance(const Vec3<T>& p) const
	{
		return closestDistanceLocal(transform_.toLocal(p));
	}
	SurfaceRayIntersection3<T> closestIntersection(const Ray3<T>& rayInWorld) const
	{
		SurfaceRayIntersection3<T> result = closestIntersectionLocal(transform_.toLocal(rayInWorld));
		result.p = transform_.toWorld(result.p);
		result.n = transform_.toWorldDirection(result.n);
		result.n *= (isNormalFlipped_) ? -1.0f : +1.0f;
		return result;
	}
	Vec3<T> closestNormal(const Point3<T>& pointInWorld) const
	{
		Vec3<T> result = transform_.toWorldDirection(closestNormalLocal(transform_.toLocal(pointInWorld)));
		result *= (isNormalFlipped_) ? -1.0f : +1.0f;
		return result;
	}

protected:
	virtual Point3<T> 					closestPointLocal(const Point3<T>& pointInLocal) const 	= 0;
	virtual BBox3<T> 					bboxLocal() const 											= 0;
	virtual SurfaceRayIntersection3<T> closestIntersectionLocal(const Ray3<T>& rayInLocal) const 	= 0;
	virtual Normal3<T> 				closestNormalLocal(const Point3<T>& pointInLocal) const 	= 0;

protected:
	virtual bool intersectsLocal(const Ray3<T>& rayInLocal) const
	{
		auto result = closestIntersectionLocal(rayInLocal);
		return result.isIntersecting;
	}
	virtual T closestDistanceLocal(const Point3<T>& pointInLocal) const
	{
		return pointInLocal.distanceTo(closestPointLocal(pointInLocal));
	};
};

using Surface3d = Surface3<double>;
using Surface3f = Surface3<float>;

NAMESPACE_BASE_END
