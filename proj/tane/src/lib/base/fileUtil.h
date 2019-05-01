#pragma once
#include "base/pch.h"

#include <boost/filesystem.hpp>
namespace boost { namespace filesystem
{
template<>
inline path& path::append<path::iterator>(path::iterator begin, path::iterator end, const codecvt_type& cvt)
{
	for (; begin != end; ++begin)
		*this /= *begin;
	return *this;
}

// Return path when appended to a_From will resolve to same as a_To
inline boost::filesystem::path make_relative(boost::filesystem::path a_From, boost::filesystem::path a_To)
{
	a_From = boost::filesystem::absolute(a_From);
	a_To   = boost::filesystem::absolute(a_To);
	
	boost::filesystem::path                 ret;
	boost::filesystem::path::const_iterator itrFrom(a_From.begin()), itrTo(a_To.begin());
	
	// Find common base
	for (boost::filesystem::path::const_iterator toEnd(a_To.end()), fromEnd(a_From.end()); itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo);
	// Navigate backwards in directory to reach previously found base
	for (boost::filesystem::path::const_iterator fromEnd(a_From.end()); itrFrom != fromEnd; ++itrFrom)
	{
		if( (*itrFrom) != "." )
			ret /= "..";
	}
	// Now navigate down the directory branch
	ret.append(itrTo, a_To.end());
	return ret;
}
}}
NAMESPACE_BASE_BEGIN


inline bool IsExists(const boost::filesystem::path p)
{
	return boost::filesystem::exists(p);
}

inline bool IsDirectory(const boost::filesystem::path p)
{
	return boost::filesystem::is_directory(p);
}
inline bool IsFile(const boost::filesystem::path p)
{
	return boost::filesystem::is_regular_file(p);
}

inline const std::string GetBaseName(const boost::filesystem::path p)
{
	return p.stem().string();
}
inline const std::string GetFileName(const boost::filesystem::path p)
{
	return p.filename().string();
}
inline const std::string GetFileExtension(const boost::filesystem::path p)
{
	return p.extension().string();
}
inline const std::string GetRelativePath(const boost::filesystem::path from, const boost::filesystem::path to)
{
	return boost::filesystem::make_relative(from, to).string();
}

NAMESPACE_BASE_END