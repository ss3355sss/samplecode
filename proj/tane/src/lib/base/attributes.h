#pragma once
#include "base/pch.h"

#include <boost/variant.hpp>

#include "base/array.h"

#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"
NAMESPACE_BASE_BEGIN
/*------------------------------------------------------ Attributes */
namespace internal
{
	namespace attributes
	{
		struct KeyPred
		{
			template<typename Pair>
			bool operator()(Pair const& lhs, Pair const& rhs) const
			{
				return lhs.first == rhs.first;
			}
		};
	}
}
class Attributes final
{
public:
	///* decl support attribute type (maximum count = 20) */
	//using variant = boost::variant<
	//    Int32Array, UInt32Array,
	//    Int64Array, UInt64Array,
	//
	//    FloatArray, DoubleArray,
	//
	//
	//
	//    Vec2fArray, Vec2dArray,
	//    Vec3fArray, Vec3dArray,
	//    Vec4fArray, Vec4dArray,
	//
	//    Mat2x2fArray, Mat2x2dArray,
	//    Mat3x3fArray, Mat3x3dArray,
	//    Mat4x4fArray, Mat4x4dArray,
	//
	//    QuatfArray,   QuatdArray
	//>;
	
	using variant = boost::variant<
			IntArray,
			Int2Array,
			Int3Array,
			
			FloatArray,
			Float2Array,
			Float3Array,
			
			Vec2fArray,
			Vec3fArray,
			Vec4fArray,
			
			Mat2x2fArray,
			Mat3x3fArray,
			Mat4x4fArray,
			
			QuatfArray
	>;

public:
	typedef size_t KeyT;
	typedef std::map<KeyT /*key_type*/, variant /*mapped_type*/> MapT;

private:
	MapT map_;

public:
	IMPL_MAP(Attributes, MapT, map_);

public:
	template <typename AttrT> const Array1<AttrT>& get(const key_type& key) const
	{
		return boost::get<Array1<AttrT>>(map_.at(key));
	}
	template <typename AttrT> Array1<AttrT>& get(const KeyT& key)
	{
		return boost::get<Array1<AttrT>>(map_[key]);
	}
	template <typename AttrT> typename Array1<AttrT>::const_iterator cbegin(const key_type& key) const
	{
		return boost::get<Array1<AttrT>>(map_.at(key)).cbegin();
	}
	template <typename AttrT> typename Array1<AttrT>::const_iterator cend(const key_type& key) const
	{
		return boost::get<Array1<AttrT>>(map_.at(key)).cend();
	}
	template <typename AttrT> typename Array1<AttrT>::iterator begin(const key_type& key)
	{
		return boost::get<Array1<AttrT>>(map_[key]).begin();
	}
	template <typename AttrT> typename Array1<AttrT>::iterator end(const key_type& key)
	{
		return boost::get<Array1<AttrT>>(map_[key]).end();
	}

private:

public:
	bool operator ==(const Attributes& other) const
	{
		bool equal = true;
		equal = map_.size() == other.map_.size();
		if(!equal)
			return false;

		equal = std::equal(map_.begin(),
                    	   map_.end(),
	                       other.map_.begin(),
	                       internal::attributes::KeyPred());

		if(!equal)
			return false;

		return true;
	}
	bool operator !=(const Attributes& other) const
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

