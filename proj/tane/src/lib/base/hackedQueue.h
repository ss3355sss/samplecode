#pragma once
#include "base/pch.h"
NAMESPACE_BASE_BEGIN
template<class T, class S, class C>
S& Container(std::priority_queue<T, S, C> &q)
{
	struct HackedQueue : private std::priority_queue<T, S, C>
	{
		static S &Container(std::priority_queue<T, S, C> &q)
		{
			return q.*&HackedQueue::c;
		}
	};
	return HackedQueue::Container(q);
}

template<class T, class S, class C>
S& HackedQueue(std::priority_queue<T, S, C> &q)
{
	struct _HackedQueue : private std::priority_queue<T, S, C>
	{
		static S &Container(std::priority_queue<T, S, C> &q)
		{
			return q.*&_HackedQueue ::c;
		}
	};
	return _HackedQueue ::Container(q);
}

NAMESPACE_BASE_END