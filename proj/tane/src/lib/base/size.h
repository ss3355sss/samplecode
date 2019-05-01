#pragma once

#include "base/pch.h"
NAMESPACE_BASE_BEGIN

template <int N = Eigen::Dynamic>
class Size final
{
private:
	Vec<size_t, N> data_;
	
public:
	Size() : data_(Vec<size_t, N>::Zero())
	{
	}
	template <typename U>
	Size(const std::initializer_list<U>& lst)
	{
		assert(lst.size() >= N);

		size_t i = 0;
		typename std::initializer_list<U>::iterator iter;
		for (iter = lst.begin(); iter != lst.end(); ++iter)
		{
			data_[i] = *iter;
			++i;
		}
	}
	template <typename... Params>
	Size(Params... params)
	{
		static_assert(sizeof...(params) == N, "Invalid number of parameters.");
		setAt_(0, params...);
	}
	~Size()
	{
	}
public:
	size_t& operator[](const size_t i)
	{
		return data_[i];
	}
	const size_t& operator[](const size_t i) const
	{
		return data_[i];
	}
	
private:
	template <typename... Params>
	void setAt_(size_t i, size_t v, Params... params)
	{
		data_[i] = v;
		setAt_(i+1, params...);
	}
	void setAt_(size_t i, size_t v)
	{
		data_[i] = v;
	}
	
};
typedef Size<1>   Size1;
typedef Size<2>   Size2;
typedef Size<3>   Size3;

NAMESPACE_BASE_END
