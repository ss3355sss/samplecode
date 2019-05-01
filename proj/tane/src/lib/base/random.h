#pragma once
#include "baseCu/pch.h"

#include "base/array.h"

#include <thrust/execution_policy.h>
#include <thrust/random.h>
#include <thrust/system/omp/execution_policy.h>
#include <thrust/transform.h>

NAMESPACE_BASE_BEGIN
namespace internal
{
	template <typename T>
	class GenRandom
	{
	private:
		int seed_;
		T   min_;
		T   max_;
	public:
		GenRandom(int seed, const T min, const T max)
				: seed_(seed),
				  min_(min),
				  max_(max)
		{
		}
	public:
		T operator()(int idx)
		{
			if(std::is_floating_point<T>::value)
			{
				thrust::default_random_engine rng(seed_);
				thrust::uniform_real_distribution<T> dist(min_, std::nextafter(max_, max_+1.0));
				rng.discard(idx);
				return dist(rng);
			}
			else
			{
				thrust::default_random_engine rng(seed_);
				thrust::uniform_int_distribution<T> dist(min_, std::nextafter(max_, max_+1.0));
				rng.discard(idx);
				return dist(rng);
			}
		}
	};
}

template <typename T>
inline Array1<T> RandomSequence(const int sz, const T min, const T max, const int seed = 0)
{
	Array1<T> ret(sz);
	thrust::transform(thrust::system::omp::par,
	                  thrust::make_counting_iterator(0),
	                  thrust::make_counting_iterator(sz),
	                  ret.begin(),
	                  internal::GenRandom<T>(seed, min, max)
	);
	return ret;
}



NAMESPACE_BASE_END