#pragma once
#include "base/pch.h"

NAMESPACE_BASE_BEGIN

template < class T >
class totally_ordered
{
	friend bool operator > ( const T& x, const T& y )
	{
		return y < x;
	}
	
	friend bool operator <= ( const T& x, const T& y )
	{
		return !( y < x );
	}
	
	friend bool operator >= ( const T& x, const T& y )
	{
		return !( x < y );
	}
	
	friend bool operator != ( const T& x, const T& y )
	{
		return !( x == y );
	}
};


NAMESPACE_BASE_END
