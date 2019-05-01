#pragma once
#include "base/pch.h"

#include "base/ray.h"
#include "base/bbox.h"

NAMESPACE_BASE_BEGIN
template <typename ItemT, typename T>
struct ClosestIntersectionQueryResult3
{
	const ItemT* 	item = nullptr;
	T 				dist = Infinity<T>();
};
template <typename ItemT> using ClosestIntersectionQueryResult3f = ClosestIntersectionQueryResult3<ItemT, float>;
template <typename ItemT> using ClosestIntersectionQueryResult3d = ClosestIntersectionQueryResult3<ItemT, double>;

template <typename ItemT, typename T>
using ClosestIntersectionDistanceFunc3 = std::function<T(const ItemT&, const Vec3<T>&)>;
template <typename ItemT, typename T>
using BoxIntersectionTestFunc3 = std::function<bool(const ItemT&, const BBox3<T>&)>;
template <typename ItemT, typename T>
using RayIntersectionTestFunc3 = std::function<bool(const ItemT&, const Ray3<T>&)>;
template <typename ItemT, typename T>
using GetRayIntersectionFunc3 = std::function<T(const ItemT&, const Ray3<T>&)>;
template <typename ItemT, typename T>
using IntersectionVisitorFunc3 = std::function<void(const ItemT&)>;

template <typename ItemT, typename T>
class IntersectionQueryEngine3
{
public:
	virtual bool intersects(const BBox3<T>& box, const BoxIntersectionTestFunc3<ItemT, T>& testFunc) const = 0;
	virtual bool intersects(const Ray3<T>& ray, const RayIntersectionTestFunc3<ItemT, T>& testFunc) const = 0;

	virtual void forEachIntersectingItem(const BBox3<T>& box, const BoxIntersectionTestFunc3<ItemT, T>& testFunc, const IntersectionVisitorFunc3<ItemT, T>& visitorFunc) const = 0;
	virtual void forEachIntersectingItem(const Ray3<T>& ray, const RayIntersectionTestFunc3<ItemT, T>& testFunc, const IntersectionVisitorFunc3<ItemT, T>& visitorFunc) const = 0;

	virtual ClosestIntersectionQueryResult3<ItemT, T> closestIntersection(const Ray3<T>& ray, const GetRayIntersectionFunc3<ItemT, T>& testFunc) const = 0;
};

NAMESPACE_BASE_END
