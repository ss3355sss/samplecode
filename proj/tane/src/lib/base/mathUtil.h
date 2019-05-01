#pragma once
#include "base/pch.h"

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"

NAMESPACE_BASE_BEGIN
template <typename T> inline bool IsZero(const T& value, const T& eps = std::numeric_limits<T>::epsilon())
{
	if(std::abs<T>(value) < eps)
		return true;
	return false;
}
template <typename T> inline bool IsEqual(const T& lhs, const T& rhs, const T& eps = std::numeric_limits<T>::epsilon())
{
	if(std::abs<T>(lhs - rhs) < eps)
		return true;
	return false;
}
template <typename T> inline T Deg2Rad(const T& deg)
{
	return deg * Pi<T>() / static_cast<T>(180.0);
}
template <typename T> inline T Rad2Deg(const T& rad)
{
	return rad * static_cast<T>(180.0) / Pi<T>();

}

template <typename T>
Quat<T> EulerAngleToQuternion(const Vec3<T>& eulerAngle)
{
	
	const T& rx = eulerAngle[0]; // radian
	const T& ry = eulerAngle[1]; // radian
	const T& rz = eulerAngle[2]; // radian
	
	Quat<T> q =
	        AngleAxis3<T>(rz, Vec3<T>::UnitZ()) *
			AngleAxis3<T>(ry, Vec3<T>::UnitY()) *
			AngleAxis3<T>(rx, Vec3<T>::UnitX());
	
	return q;
}

template <typename T>
Vec3<T> QuternionToEulerAngle(const Quat<T>& q)
{
	const T& w = q.w();
	const T& x = q.x();
	const T& y = q.y();
	const T& z = q.z();
	
	const T Nq = w*w + x*x + y*y + z*z;
	const T s = ( Nq > 0 ) ? (2/Nq) : 0;
	const T xs = x*s,  ys = y*s,  zs = z*s;
	const T wx = w*xs, wy = w*ys, wz = w*zs;
	const T xx = x*xs, xy = x*ys, xz = x*zs;
	const T yy = y*ys, yz = y*zs, zz = z*zs;
	const T m00 = 1-(yy+zz), m01 = xy-wz,     m02 = xz+wy;
	const T m10 = xy+wz,     m11 = 1-(xx+zz), m12 = yz-wx;
	const T m20 = xz-wy,     m21 = yz+wx,     m22 = 1-(xx+yy);
	const T cy = sqrt( m00*m00 + m10*m10 );
	
	T Rx, Ry, Rz;
	if( cy > std::numeric_limits<T>::epsilon() )
	{
		Rx = atan2(  m21, m22 );
		Ry = atan2( -m20, cy  );
		Rz = atan2(  m10, m00 );
	}
	else
	{
		Rx = atan2( -m12, m11 );
		Ry = atan2( -m20, cy  );
		Rz = 0;
	}
	return Vec3<T>(Rx, Ry, Rz);
}

template<typename T>
void CalcEulerAngles(const Vec3<T>& right, const Vec3<T>& up, const Vec3<T>& target, T* rx, T* ry, T* rz)
{
	Mat4x4<T> rotation = Mat4x4<T>::Identity();
	rotation(0, 0) = right[0]; rotation(0, 1) = up[0]; rotation(0, 2) = target[0];
	rotation(1, 0) = right[1]; rotation(1, 1) = up[1]; rotation(1, 2) = target[1];
	rotation(2, 0) = right[2]; rotation(2, 1) = up[2]; rotation(2, 2) = target[2];

	const T c2 = std::sqrt( rotation(0,0)*rotation(0,0) + rotation(1,0) * rotation(1,0));

	if( c2 < 1e-30f) // singular case
	{
		*rx = *ry = *rz = static_cast<T>(0);
	}
	else // regular case
	{
		*rx = std::atan2( rotation(2,1), rotation(2,2) );
		*ry = std::atan2(-rotation(2,0), c2 );

		const T s = std::sin( *rx );
		const T c = std::cos( *rx );
		*rz = std::atan2( s*rotation(0,2)-c*rotation(0,1), c*rotation(1,1)-s*rotation(1,2));
	}

}
template<typename T>
Mat4x4<T> Rotation(const Vec3<T>& axis, const T rad)
{
	const T c = std::cos(rad);
	const T s = std::sin(rad);
	float t = static_cast<T>(1.0) - c;

	// intermediate values
	T tx = t*axis[0];  T ty = t*axis[1];    T tz = t*axis[2];
	T sx = s*axis[0];  T sy = s*axis[1];    T sz = s*axis[2];
	T txy = tx*axis[1]; T tyz = tx*axis[2]; T txz = tx*axis[2];

	Mat4x4<T> ret = Mat4x4<T>::Identity();
	ret(0, 0) = tx*axis[0] + c; ret(0, 1) = txy - sz;       ret(0, 2) = txz + sy;       ret(0, 3) = 0.0f;
	ret(1, 0) = txy + sz;       ret(1, 1) = ty*axis[1] + c; ret(1, 2) = tyz - sx;       ret(1, 3) = 0.0f;
	ret(2, 0) = txz - sy;       ret(2, 1) = tyz + sx;       ret(2, 2) = tz*axis[2] + c; ret(2, 3) = 0.0f;
	ret(3, 0) = 0.0f;           ret(3, 1) = 0.0f;           ret(3, 2) = 0.0f;           ret(3, 3) = 1.0f;

	return ret;
}

template<typename T>
Vec3<T> RotateByAxis(const Vec3<T>& v, const Vec3<T>& unitAxis, const T rad)
{
	const T c = std::cos(rad);
	const T s = std::sin(rad);

	const T alpha = (1 - c) * dot(unitAxis, v);

	Vec3<T> ret = c * v + s * cross(unitAxis, v) + alpha * unitAxis;
	return ret;
}

template<typename T>
void RotateLocalAxis(const T radx, const T rady, const T radz, Vec3<T>* target, Vec3<T>* up, Vec3<T>* right)
{
	//---------------- local x axis
	*up     = RotateByAxis(*up, *right, radx).normalized();
	*target = RotateByAxis(*target, *right, radx).normalized();
	//---------------- local y axis
	*right  = RotateByAxis(*right, *up, rady).normalized();
	*target = RotateByAxis(*target, *up, rady).normalized();
	//---------------- local z axis
	*right  = RotateByAxis(*right, *target, radz).normalized();
	*up     = RotateByAxis(*up, *target, radz).normalized();

}

NAMESPACE_BASE_END