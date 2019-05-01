#pragma once
#include "base/pch.h"

#include <inc_eigen.h>
#include <boost/algorithm/clamp.hpp>
NAMESPACE_BASE_BEGIN
/*-----------------------------------------------------------------------  Aliasing */
/*-----------------------------aliasing for vector*/
template <typename T, int M = Eigen::Dynamic>   using Vec	    = Eigen::Matrix< T, M, 1, Eigen::ColMajor>;
template <typename T>                           using Vec2    = Vec<T, 2>;
template <typename T>                           using Vec3    = Vec<T, 3>;
template <typename T>                           using Vec4    = Vec<T, 4>;
typedef Vec2<float> Vec2f; typedef Vec2<double> Vec2d;
typedef Vec3<float> Vec3f; typedef Vec3<double> Vec3d;
typedef Vec4<float> Vec4f; typedef Vec4<double> Vec4d;

typedef Vec2<int>   Vec2i; typedef Vec2<u_int>   Vec2ui;
typedef Vec3<int>   Vec3i; typedef Vec3<u_int>   Vec3ui;
typedef Vec4<int>   Vec4i; typedef Vec4<u_int>   Vec4ui;

template <typename T, int M = Eigen::Dynamic>   using RowVec	= Eigen::Matrix< T, 1, M, Eigen::RowMajor>;
template <typename T>                           using RowVec2 = RowVec<T, 2>;
template <typename T>                           using RowVec3 = RowVec<T, 3>;
template <typename T>                           using RowVec4 = RowVec<T, 4>;
typedef RowVec2<float> RowVec2f; typedef RowVec2<double> RowVec2d;
typedef RowVec3<float> RowVec3f; typedef RowVec3<double> RowVec3d;
typedef RowVec4<float> RowVec4f; typedef RowVec4<double> RowVec4d;

/*-----------------------------aliasing for Int */
typedef Vec2<int>           Int2;
typedef Vec3<int>           Int3;
typedef Vec4<int>           Int4;

/*-----------------------------aliasing for UInt */
typedef Vec2<unsigned int>  UInt2;
typedef Vec3<unsigned int>  UInt3;
typedef Vec4<unsigned int>  UInt4;

/*-----------------------------aliasing for float */
typedef Vec2<float>         Float2;
typedef Vec3<float>         Float3;
typedef Vec4<float>         Float4;

/*-----------------------------aliasing for doube */
typedef Vec2<double>        Double2;
typedef Vec3<double>        Double3;
typedef Vec4<double>        Double4;


/*-----------------------------aliasing for normal*/
template <typename T> using Normal2 = Vec2<T>;
template <typename T> using Normal3 = Vec3<T>;
typedef Normal2<float> Normal2f; typedef Normal2<double> Normal2d ;
typedef Normal3<float> Normal3f; typedef Normal3<double> Normal3d ;
/*-----------------------------aliasing for point*/
template <typename T> using Point2 = Vec2<T>;
template <typename T> using Point3 = Vec3<T>;
typedef Point2<float> Point2f; typedef Point2<double> Point2d ;
typedef Point3<float> Point3f; typedef Point3<double> Point3d ;

/*-----------------------------aliasing for texCoord*/
template <typename T> using TexCoord1 = T;
template <typename T> using TexCoord2 = Vec2<T>;
template <typename T> using TexCoord3 = Vec3<T>;
typedef TexCoord1<float> TexCoord1f; typedef TexCoord1<double> TexCoord1d ;
typedef TexCoord2<float> TexCoord2f; typedef TexCoord2<double> TexCoord2d ;
typedef TexCoord3<float> TexCoord3f; typedef TexCoord3<double> TexCoord3d ;
/*-----------------------------aliasing for Color*/
typedef Vec3<float> RGBColor;
typedef Vec4<float> RGBAColor;

/*-----------------------------aliasing for semetic */

/*----------------------------------------------------------------------- eigen extension */
/*------------------------------------------------------------- Common extension */
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Sum(Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().sum();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Average (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().sum() / m.array().size();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Min (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().minCoeff();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Max (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().maxCoeff();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar AbsMin (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().cwiseAbs().minCoeff();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar AbsMax (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().cwiseAbs().maxCoeff();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Abs2Min (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().cwiseAbs2().minCoeff();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> _Scalar Abs2Max (Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().cwiseAbs2().maxCoeff();
}

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> Ceil(Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().ceil();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> Floor(Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().floor();
}
template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> Round(Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const& m)
{
	return m.array().round();
}

//template <typename Derived>
//inline std::ostream& operator<<(std::ostream& os, const Eigen::MatrixBase<Derived>& m)
//{
//	std::streamsize prec = os.precision();
//	//os.setf(std::ios::fixed);
//	os.precision(2);
//
//	const typename Eigen::MatrixBase<Derived>::Index&   c = m.cols();
//	const typename Eigen::MatrixBase<Derived>::Index&   r = m.rows();
//	const std::type_info&                               typeInfo = typeid(typename Eigen::MatrixBase<Derived>::value_type);
//
//	std::string typePrefix;
//	if(typeid(int) == typeInfo)     { typePrefix = "i"; }
//	if(typeid(float) == typeInfo)   { typePrefix = "f"; }
//	if(typeid(double) == typeInfo)  { typePrefix = "d"; }
//
//	if(c == 1 || r == 1) /// vector case
//	{
//		int n = 0;
//
//		if(c == 1) { n = r; }
//		if(r == 1) { n = c; }
//
//		os << "(";
//		for(size_t i = 0; i < n; ++i)
//		{
//			(i == (n - 1) ? os << std::ios::fixed << m[i] : os << std::ios::fixed << m[i] << ", ");
//		}
//		os << ")";
//
//	}
//	else
//	{
//		os << m;
//	}
//
//	os.precision(prec);
//	return os;
//}


/*------------------------------------------------------------- Vec3 extension
 /* Dot */
template<typename Derived> typename Derived::Scalar Dot(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
{
	return v1.dot(v2);
}
/* Cross */
template<typename Derived> typename Derived::template cross_product_return_type<Derived>::type Cross(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
{
	return v1.cross(v2);
}

///* TODO::DominantAxis */
//template <typename T> Vec3<T> DominantAxis(const Vec3<T>& v)
//{
//	return Vec3<T>();
//}
///* TODO::SubminantAxis */
//template <typename T> Vec3<T> SubminantAxis(const Vec3<T>& v)
//{
//	return Vec3<T>();
//}
/* Clamp */
template <typename T> Vec3<T> Clamp(const Vec3<T>& v, const Vec3<T>& l, const Vec3<T>& h)
{
	return Vec3<T>(boost::algorithm::clamp(v[0], l[0], h[0]),
					boost::algorithm::clamp(v[1], l[1], h[1]),
					boost::algorithm::clamp(v[2], l[2], h[2]));
}
/* Min */
template <typename T> Vec3<T> Min(const Vec3<T>& a, const Vec3<T>& b)
{
	return Vec3<T>(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]));
}
/* Max */
template <typename T> Vec3<T> Max(const Vec3<T>& a, const Vec3<T>& b)
{
	return Vec3<T>(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]));
}

/* Distance */
template <typename T> T Distance(const Vec3<T>& u, const Vec3<T>& v)
{
	return (v - u).norm();
}
/* DistanceSquared*/
template <typename T> T DistanceSquared(const Vec3<T>& u, const Vec3<T>& v)
{
	return (v - u).squaredNorm();
}
/* Reflect */
template <typename T> Vec3<T> Reflect(const Vec3<T>& v, const Normal3<T>& n /* unit vector */)
{
	// this - 2(this.n)n
	return v - static_cast<T>(2) * Dot(v, n) * n;
}
/* Project */
template <typename T> Vec3<T> Project(const Vec3<T>& v, const Normal3<T>& n /* unit vector */)
{
	return v - Dot(v, n) * n;
}
/* Tangential */
template <typename T> std::tuple<Vec3<T>, Vec3<T>> Tangential(const Normal3<T>& n)
{
	Vec3<T> u = Cross(((std::fabs(n[1]) > 0 || std::fabs(n[2]) > 0) ? Vec3<T>(1, 0, 0) : Vec3<T>(0, 1, 0)), n).normalized();
	Vec3<T> v = Cross(n, u);
	return std::make_tuple(u, v);

}
template <typename T> bool IsEqual(const Vec3<T>& v1, const Vec3<T>& v2, T eps = Epsilon<T>())
{
	return std::fabs(v1.data()[0] - v2.data()[0]) < eps &&
		   std::fabs(v1.data()[1] - v2.data()[1]) < eps &&
		   std::fabs(v1.data()[2] - v2.data()[2]) < eps;
}
template <typename T> bool IsZero(const Vec3<T>& v1, T eps = Epsilon<T>())
{
	static Vec3<T> zero = Vec3<T>::Zero();
	return std::fabs(v1.data()[0] - zero.data()[0]) < eps &&
		   std::fabs(v1.data()[1] - zero.data()[1]) < eps &&
		   std::fabs(v1.data()[2] - zero.data()[2]) < eps;
}
template <typename T> bool IsUnit(const Vec3<T>& v, T eps = Epsilon<T>())
{
	return IsZero<T>(v.norm() - static_cast<T>(1.0), eps);
}
template <typename T> bool IsParallel(const Vec3<T>& u, const Vec3<T>& v, T eps = Epsilon<T>())
{
	return IsZero<T>(u.dot(v) - static_cast<T>(1.0), eps);
}
template <typename T> bool IsOrthogonal(const Vec3<T>& u, const Vec3<T>& v, T eps = Epsilon<T>())
{
	return IsZero<T>(u.dot(v), eps);
}
template <typename T> bool IsOrthonormal(const Vec3<T>& u, const Vec3<T>& v, T eps = Epsilon<T>())
{
	if(!IsUnit(u, eps))
		return false;
	if(!IsUnit(v, eps))
		return false;
	return IsZero<T>(u.dot(v), eps);
}

/*------------------------------------------------------------- Vec4 extension */



NAMESPACE_BASE_END

