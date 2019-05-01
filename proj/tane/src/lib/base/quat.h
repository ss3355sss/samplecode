#pragma once
#include "base/pch.h"

#include <inc_eigen.h>
///------------------------------------------------------------------ deprecated version
NAMESPACE_BASE_BEGIN
/*-----------------------------------------------------------------------  Aliasing */
/*-----------------------------aliasing for quaternion*/
template <typename T > using Quat = Eigen::Quaternion<T>;
using Quatf = Quat<float>;
using Quatd = Quat<double>;
/*-----------------------------aliasing for AngleAxis*/
template <typename T> using AngleAxis3 = Eigen::AngleAxis<T>;
using AngleAxis3d = AngleAxis3<double>;
using AngleAxis3f = AngleAxis3<float>;

/*----------------------------------------------------------------------- Helper */
/* Quaternion Dot */
template<typename T>
T Dot(const Quat<T>& q1, const Quat<T>& q2) { return q1.dot(q2); }
NAMESPACE_BASE_END
///------------------------------------------------------------------ deprecated version end






