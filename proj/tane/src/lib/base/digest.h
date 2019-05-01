#pragma once
#include "base/pch.h"

#include "base/operator.h"

#include <iomanip>

NAMESPACE_BASE_BEGIN

struct Digest : public totally_ordered<Digest>
{
public:
	union
	{
		uint8_t     d[16];
		uint64_t    words[2];
	};

public:
	Digest()
	{
		words[0] = words[1] = 0;
	}
	Digest( const Digest &copy )
	{
		words[0] = copy.words[0];
		words[1] = copy.words[1];
	}
public:
	Digest &operator=( const Digest &copy )
	{
		words[0] = copy.words[0];
		words[1] = copy.words[1];
		return *this;
	}
public:
	uint8_t& operator[]( size_t i )
	{
		return d[i];
	}
	uint8_t operator[]( size_t i ) const
	{
		return d[i];
	}
public:
	bool operator==( const Digest &iRhs ) const
	{
		return (( words[0] == iRhs.words[0]) && (words[1] == iRhs.words[1]));
	}
	bool operator<( const Digest &iRhs ) const
	{
		return (words[0] < iRhs.words[0] ? true : (words[0] > iRhs.words[0] ? false : (words[1] < iRhs.words[1])));
	}
public:
	void print( std::ostream &ostr ) const
	{
		for ( int i = 0; i < 16; ++i )
		{
			ostr << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::noshowbase << ( int ) d[i];
		}
	}
	std::string str() const
	{
		std::stringstream sstr;
		print( sstr );
		return sstr.str();
	}
};

////-*****************************************************************************
//inline std::ostream &operator<<( std::ostream &ostr, const Digest &a )
//{
//	a.print( ostr );
//	return ostr;
//}

NAMESPACE_BASE_END
