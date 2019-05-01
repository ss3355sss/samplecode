#pragma once
#include "base/pch.h"
#include "base/matrix.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------- MatrixStack
template <typename T>
class MatrixStack
{
private:
	Mat4x4<T> curr_;
	std::stack<Mat4x4<T>, std::vector<Mat4x4<T>>> stack_;
public:
	MatrixStack() : curr_(Mat4x4<T>::Identity())
	{
	}
	MatrixStack(const Mat4x4<T> &m) : curr_(m)
	{
	}
	MatrixStack(const MatrixStack &other)
	{
		MatrixStack::operator=(other);
	}
	MatrixStack &operator=(const MatrixStack &other)
	{
		if(&other != this)
		{
			curr_  = other.curr_;
			stack_ = other.stack_;
		}
		return *this;
	}
	~MatrixStack()
	{

	}
	MatrixStack &operator*=(const Mat4x4f& m)
	{
		//ApplyMatrix(m);
		return *this;
	}
public:
	void push()
	{
		stack_.push(curr_);
	}
	void pop()
	{
		curr_ = stack_.top();
		stack_.pop();
	}
	void reset()
	{
		curr_ = stack_.top();
	}
	const Mat4x4<T>& top()
	{
		return curr_;
	}
	const Mat4x4<T>& top() const
	{
		return curr_;
	}

	//void rotateX(const float deg)
	//{
	//	Mat4x4<T> tmp = Mat4x4<T>::Identity();
	//
	//	const float s = sin(deg2rad(deg));
	//	const float c = cos(deg2rad(deg));
	//
	//	tmp(1, 1) = c;
	//	tmp(1, 2) = -s;
	//	tmp(2, 1) = s;
	//	tmp(2, 2) = c;
	//
	//	curr_ *= tmp;
	//}
	//void rotateY(const float deg)
	//{
	//	Mat4x4<T> tmp = Mat4x4<T>::Identity();
	//
	//	const float s = sin(deg2rad(deg));
	//	const float c = cos(deg2rad(deg));
	//
	//	tmp(0, 0) = c;
	//	tmp(0, 2) = s;
	//	tmp(2, 0) = -s;
	//	tmp(2, 2) = c;
	//
	//	curr_ *= tmp;
	//
	//}
	//void rotateZ(const float deg)
	//{
	//	Mat4x4<T> tmp = Mat4x4<T>::Identity();
	//
	//	const float s = sin(deg2rad(deg));
	//	const float c = cos(deg2rad(deg));
	//
	//	tmp(0, 0) = c;
	//	tmp(0, 1) = -s;
	//	tmp(0, 1) = s;
	//	tmp(1, 1) = c;
	//
	//	curr_ *= tmp;
	//}
	//
	//void scale(float sx, float sy, float sz)
	//{
	//	Mat4x4<T> tmp = Mat4x4<T>::Identity();
	//	tmp(0, 0) = sx;
	//	tmp(1, 1) = sy;
	//	tmp(2, 2) = sz;
	//
	//	curr_ *= tmp;
	//
	//}
	//void scale(float s)
	//{
	//	scale(s, s, s);
	//}
	//
	//void translate(float tx, float ty, float tz)
	//{
	//	Mat4x4<T> tmp = Mat4x4<T>::Identity();
	//	tmp(0, 3) = tx;
	//	tmp(1, 3) = ty;
	//	tmp(2, 3) = tz;
	//
	//	curr_ *= tmp;
	//}

	void transform(const Mat4x4<T> &m)
	{
		curr_ *= m;
	}

	void setIdentity()
	{
		curr_ = Mat4x4<T>::Identity();
	}

	void clear()
	{
		curr_      = Mat4x4<T>::Identity();
		for (int i = 0; i < stack_.size(); ++i)
			stack_.pop();
	}

private:
	//float deg2rad(const float deg)
	//{
	//	return deg * M_PI / 180.0f;
	//}
	//float rad2deg(const float rad)
	//{
	//	return rad * 180.0f / M_PI;
	//}

	///TODO :: need to impl
	//void Rotate(const vec3f& axis, const float rad);
	//void Rotate(const vec3f& axis, const float degree)
	//void SetMatrix(const mat4x4f& m)

};

NAMESPACE_BASE_END