#pragma once
#include "base/pch.h"

#include "base/vector.h"

NAMESPACE_BASE_BEGIN
template <typename ItemT, typename T>
struct NearestNeighborQueryResult3
{
	const ItemT* 	item = nullptr;
	T 				dist = Infinity<T>();
};
template <typename ItemT> using NearestNeighborQueryResult3f = NearestNeighborQueryResult3<ItemT, float>;
template <typename ItemT> using NearestNeighborQueryResult3d = NearestNeighborQueryResult3<ItemT, double>;

template <typename ItemT, typename T>
using NearestNeighborDistanceFunc3 = std::function<double(const ItemT&, const Vec3<T>&)>;

template <typename ItemT, typename T>
class NearestNeighborQueryEngine3
{
public:
	virtual NearestNeighborQueryResult3<ItemT, T> nearest(const Point3<T>& p, const NearestNeighborDistanceFunc3<ItemT, T>& distanceFunc) const = 0;
};


NAMESPACE_BASE_END
