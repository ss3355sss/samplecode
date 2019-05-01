#pragma once
#include "base/pch.h"

#include <boost/variant.hpp>

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"
///-------------------------------------------------------Uniform
NAMESPACE_BASE_BEGIN
/* nothing to impl for Uniform */
NAMESPACE_BASE_END

///-------------------------------------------------------Uniforms
NAMESPACE_BASE_BEGIN
namespace internal
{
	namespace uniforms
	{
		struct KeyPred
		{
		template <typename Pair>
		bool operator() (Pair const &lhs, Pair const &rhs) const
		{
			return lhs.first == rhs.first;
		}
	};
	}
}

class Uniforms final
{
public:
	/* decl support uniform type (maximum count = 20) */
	using variant = boost::variant<
		bool,
		char,
		unsigned char,
		short,
		unsigned short,
		int,
		unsigned int,
		float,
		double,
		Quatf,
		Vec2f,
		Vec3f,
		Vec4f,
		Mat2x2f,
		Mat3x3f,
		Mat4x4f
	>;
public:
	typedef size_t 												KeyT;
	typedef std::map<KeyT /*key_type*/, variant /*value_type*/> MapT;

public:
	MapT map_;

public:
	IMPL_MAP(Uniforms, MapT, map_);

public:
	template <typename T> const T& get(const KeyT& key) const
	{
		return boost::get<T>(map_.at(key));
	}
	template <typename T> T& get(const KeyT& key)
	{
		return boost::get<T>(map_[key]);
	}

private:
	struct KeyPred_
	{
		template <typename Pair>
		bool operator() (Pair const &lhs, Pair const &rhs) const
		{
			return lhs.first == rhs.first;
		}
	};
public:
	bool operator ==(const Uniforms& other) const
	{
		bool equal = true;
		equal = (map_.size() == other.map_.size());
		if(!equal)
			return false;

		equal = std::equal(map_.begin(), map_.end(),
			               other.map_.begin(),
			               internal::uniforms::KeyPred());
		if(!equal)
			return false;
		return true;
	}
	bool operator !=(const Uniforms& other) const
	{
		return !(operator==(other));
	}

private:
	friend class boost::serialization::access;
	template <typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		ar & map_;
	}
};
NAMESPACE_BASE_END
