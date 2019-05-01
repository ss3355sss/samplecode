#pragma once
#include "base/pch.h"

#include <inc_eigen.h>
NAMESPACE_BASE_BEGIN
/*-----------------------------------------------------------------------  Aliasing */
/*-----------------------------aliasing for matrix*/
#ifndef EIGEN_MATRIX_STORAGE_ROW_MAJOR_ORDER
template <typename T, int R = Eigen::Dynamic, int C = Eigen::Dynamic>   using Mat = Eigen::Matrix< T, R, C, Eigen::ColMajor>;
#else
template <typename T, int R = Eigen::Dynamic, int C = Eigen::Dynamic>   using Mat = Eigen::Matrix< T, R, C, Eigen::RowMajor>;
#endif

template <typename T> 													using Mat2x2 = Mat<T, 2, 2>;
template <typename T> 													using Mat3x3 = Mat<T, 3, 3>;
template <typename T> 													using Mat4x4 = Mat<T, 4, 4>;
typedef Mat2x2<float> Mat2x2f; typedef Mat2x2<double> Mat2x2d;
typedef Mat3x3<float> Mat3x3f; typedef Mat3x3<double> Mat3x3d;
typedef Mat4x4<float> Mat4x4f; typedef Mat4x4<double> Mat4x4d;

/* TODO::remove row */
/* TODO::insert row */

/*----------------------------------------------------------------------- Helper */
/*----------------------------------------- Mat2x2 extension */
/*----------------------------------------- Mat3x3 extension */
template <typename T>
inline bool IsEqual(const Mat3x3<T>& m1, const Mat3x3<T>& m2, T eps = Epsilon<T>())
{
	return std::fabs(m1.data()[0] - m2.data()[0]) < eps &&
		   std::fabs(m1.data()[1] - m2.data()[1]) < eps &&
		   std::fabs(m1.data()[2] - m2.data()[2]) < eps &&
		   std::fabs(m1.data()[3] - m2.data()[3]) < eps &&
		   std::fabs(m1.data()[4] - m2.data()[4]) < eps &&
		   std::fabs(m1.data()[5] - m2.data()[5]) < eps &&
		   std::fabs(m1.data()[6] - m2.data()[6]) < eps &&
		   std::fabs(m1.data()[7] - m2.data()[7]) < eps &&
		   std::fabs(m1.data()[8] - m2.data()[8]) < eps;

}

/*----------------------------------------- Mat4x4 extension */
NAMESPACE_BASE_END
