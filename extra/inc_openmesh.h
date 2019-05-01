#pragma once

#include <inc_eigen.h>
#define OM_STATIC_BUILD

#include <OpenMesh/Core/Utils/vector_traits.hh>
/* Eigen Interface for OpenMesh */
namespace OpenMesh
{
	template<typename _Scalar, int _Rows, int _Cols, int _Options>
	struct vector_traits<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options>>
	{
		static_assert(_Rows != Eigen::Dynamic && _Cols != Eigen::Dynamic, "Should not use dynamic vectors.");
		static_assert(_Rows == 1 || _Cols == 1,                           "Should not use matrices."       );
		
		using vector_type 			= Eigen::Matrix<_Scalar, _Rows, _Cols, _Options>;
		using value_type 			= _Scalar;
		static const size_t size_ 	= _Rows * _Cols;
		//const size_t size_ 	= _Rows * _Cols;
		static size_t size()		{ return size_; }
	};
	template<typename Derived> typename Derived::Scalar dot(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
	{
		return v1.dot(v2);
	}
	
	template<typename Derived> typename Derived::template cross_product_return_type<Derived>::type cross(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
	{
		return v1.cross(v2);
	}
	template<typename Derived> Eigen::MatrixBase<Derived> normalize(Eigen::MatrixBase<Derived> const& v)
	{
		return v.normalized();
	}
	template<typename Derived> typename Derived::Scalar norm(Eigen::MatrixBase<Derived> const& v)
	{
		return v.norm();
	}
}

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

//#include <OpenMesh/Core/IO/MeshIO.hh>
//#include <OpenMesh/Core/IO/reader/OBJReader.hh>
//#include <OpenMesh/Core/IO/writer/OBJWriter.hh>

/* back up */
//struct TestTrait : public OpenMesh::DefaultTraits
//{
//public:
//	typedef vec3f  	Point;
//	typedef vec3f 	Normal;
//	typedef vec3uc  Color;
//	VertexTraits
//	{
//	public:
//		VertexT() : local_coord(0.0f, 0.0f, 0.0f), mass(1.0f), w(1.0 / mass), velocity(0.0f, 0.0f, 0.0f){};
//		Point	local_coord;
//		Point	p;
//		float	mass;
//		float	w;
//		vec3f	velocity;
//	};
//	FaceTraits
//	{
//	public:
//		FaceT() : barycenter_(Point(0.0f, 0.0f, 0.0f)){ }
//		Point  barycenter_;
//	};
//	VertexAttributes(OpenMesh::Attributes::Color | OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status);
//	FaceAttributes(OpenMesh::Attributes::Color | OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status);
//	EdgeAttributes(OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
//	HalfedgeAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status | OpenMesh::Attributes::PrevHalfedge);
//};
//typedef OpenMesh::TriMesh_ArrayKernelT<TestTrait> TestTrait_Mesh;

