#pragma once
#include "base/pch.h"
NAMESPACE_BASE_BEGIN
template<typename T>
struct _cmpFloating
{
	double eps_;
	
	_cmpFloating(const T eps = static_cast<T>(0.001)) : eps_(eps)
	{
	}
	~_cmpFloating()
	{
	}
	
	bool operator()(const T& lhs, const T& rhs) const
	{
		if(std::fabs<T>(lhs - rhs) < static_cast<T>(eps_))
			return false;
		return lhs < rhs;
	}
};

template<typename ElemT> using FloatMap  = std::map<float,  ElemT, _cmpFloating<float>>;
template<typename ElemT> using DoubleMap = std::map<double, ElemT, _cmpFloating<double>>;

NAMESPACE_BASE_END
