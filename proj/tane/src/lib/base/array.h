#pragma once
#include "base/pch.h"

#include "base/digest.h"


#include "base/vector.h"
#include "base/matrix.h"
#include "base/quat.h"

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>
#include <boost/enable_shared_from_this.hpp>

//#include <tbb/mutex.h>

NAMESPACE_BASE_BEGIN
template <typename T>
class Array1 final
{
public:
	typedef std::vector<T> 	ContainerT;
private:
	ContainerT 	data_;
public:
	IMPL_VECTOR(Array1, ContainerT, data_);

public:
	//template <typename Callback> void forEach(Callback func) const;
	//template <typename Callback> void forEachIndex(Callback func) const;
	//template <typename Callback> void parallelForEach(Callback func);
	//template <typename Callback> void parallelForEachIndex(Callback func) const;
public:
	//operator ArrayAccessor1<T>();
	//operator ConstArrayAccessor1<T>() const;
	//ArrayAccessor1<T> accessor();
	//ConstArrayAccessor1<T> constAccessor() const;

private:
	friend class boost::serialization::access;
	template <typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		ar & data_;
	}
	
	
};

//bool        /* 8bit */
//
//int8_t;     /* [signed] char  8  bit */
//int16_t;    /* [signed] short 16 bit */
//int32_t;    /* [signed] int   32 bit */
//int64_t;    /* [signed] long  64 bit */
//
//uint8_t;    /* unsigned char  8  bit */
//uint16_t;   /* unsigned short 16 bit */
//uint32_t;   /* unsigned int   32 bit */
//uint64_t;   /* unsigned long  64 bit */
typedef Array1<bool>            BoolArray;
typedef Array1<char>            CharArray;
typedef Array1<unsigned char>   UCharArray;

typedef Array1<int>             IntArray;
typedef Array1<int8_t>          Int8Array;
typedef Array1<int16_t>         Int16Array;
typedef Array1<int32_t>         Int32Array;
typedef Array1<int64_t>         Int64Array;

typedef Array1<uint8_t>         UInt8Array;
typedef Array1<uint32_t>        UInt16Array;
typedef Array1<uint32_t>        UInt32Array;
typedef Array1<uint64_t>        UInt64Array;

typedef Array1<float>           FloatArray;
typedef Array1<double>          DoubleArray;

typedef Array1<std::string>     StringArray;

template <typename T> using Vec2Array           = Array1<Vec2<T>>;
template <typename T> using Vec3Array           = Array1<Vec3<T>>;
template <typename T> using Vec4Array           = Array1<Vec4<T>>;

template <typename T> using Mat2x2Array         = Array1<Mat2x2<T>>;
template <typename T> using Mat3x3Array         = Array1<Mat3x3<T>>;
template <typename T> using Mat4x4Array         = Array1<Mat4x4<T>>;

template <typename T> using Point3Array         = Array1<Point3<T>>;
template <typename T> using Normal3Array        = Array1<Normal3<T>>;
template <typename T> using TexCoord1Array      = Array1<TexCoord1<T>>;
template <typename T> using TexCoord2Array      = Array1<TexCoord2<T>>;
template <typename T> using TexCoord3Array      = Array1<TexCoord3<T>>;

template <typename T> using QuatArray           = Array1<Quat<T>>;

typedef Vec2Array<int>          Vec2iArray;
typedef Vec3Array<int>          Vec3iArray;
typedef Vec4Array<int>          Vec4iArray;

typedef Vec2Array<int>          Int2Array;
typedef Vec3Array<int>          Int3Array;
typedef Vec4Array<int>          Int4Array;

typedef Vec2Array<float>        Float2Array;
typedef Vec3Array<float>        Float3Array;
typedef Vec4Array<float>        Float4Array;

typedef Vec2Array<double>       Double2Array;
typedef Vec3Array<double>       Double3Array;
typedef Vec4Array<double>       Double4Array;

typedef Vec2Array<float>        Vec2fArray;         typedef Vec2Array<double>       Vec2dArray;
typedef Vec3Array<float>        Vec3fArray;         typedef Vec3Array<double>       Vec3dArray;
typedef Vec4Array<float>        Vec4fArray;         typedef Vec4Array<double>       Vec4dArray;

typedef Mat2x2Array<float>      Mat2x2fArray;       typedef Mat2x2Array<double>     Mat2x2dArray;
typedef Mat3x3Array<float>      Mat3x3fArray;       typedef Mat3x3Array<double>     Mat3x3dArray;
typedef Mat4x4Array<float>      Mat4x4fArray;       typedef Mat4x4Array<double>     Mat4x4dArray;

typedef Point3Array<float>      Point3fArray;       typedef Point3Array<double>     Point3dArray;
typedef Normal3Array<float>     Normal3fArray;      typedef Normal3Array<double>    Normal3dArray;
typedef TexCoord1Array<float>   TexCoord1fArray;    typedef TexCoord1Array<double>  TexCoord1dArray;
typedef TexCoord2Array<float>   TexCoord2fArray;    typedef TexCoord2Array<double>  TexCoord2dArray;
typedef TexCoord3Array<float>   TexCoord3fArray;    typedef TexCoord3Array<double>  TexCoord3dArray;
typedef QuatArray<float>        QuatfArray;         typedef TexCoord3Array<double>  QuatdArray;

typedef Array1<RGBColor>        RGBColorArray;
typedef Array1<RGBAColor>       RGBAColorArray;

NAMESPACE_BASE_END

/* TODO::Class Array# */
//template<typename ElemT>
//class Array2;
//template<typename ElemT>
//class Array3;
//template<typename ElemT>
//using Array2 = std::vector<std::vector<ElemT>>;
//template<typename ElemT>
//using Array3 = std::vector<std::vector<std::vector<ElemT>>>;




//NAMESPACE_BASE_END
//
//NAMESPACE_BASE_BEGIN
//template <typename T>
//class Array2<T> final
//{
//public:
//	typedef size2_t 		size_type;
//	typedef std::vector<T> 	ContainerT;
//
//public:
//	typedef typename ContainerT::value_type value_type;
//	typedef typename ContainerT::pointer pointer;
//	typedef typename ContainerT::const_pointer const_pointer;
//	typedef typename ContainerT::reference reference;
//	typedef typename ContainerT::const_reference const_reference;
//	typedef typename ContainerT::const_iterator const_iterator;
//	typedef typename ContainerT::iterator iterator;
//	typedef typename ContainerT::const_reverse_iterator const_reverse_iterator;
//	typedef typename ContainerT::reverse_iterator reverse_iterator;
//	typedef typename ContainerT::difference_type difference_type;
//	typedef typename ContainerT::allocator_type allocator_type;
//
//private:
//	size_type 	size_;
//	ContainerT 	data_;
//
//public:
//	Array2()
//	{
//	}
//	explicit Array2(size_type size, const T& value = T()) : data_(size, value)
//	{
//	}
//	Array2(const Array2& other) : data_(other.data_)
//	{
//	}
//	Array2(Array2&& other) : data_(std::move(other.data_))
//	{
//	}
//	Array2(const std::initializer_list<T>& lst) : data_(lst)
//	{
//	}
//	template<typename InputIterator>
//	Array2(InputIterator first, InputIterator last) : data_(first, last)
//	{
//	}
//	~Array2()
//	{
//	}
//
//public:
//	Array2& operator=(const Array2& other)
//	{
//		return data_.operator=(other);
//	}
//	Array2& operator=(Array2&& other)
//	{
//		return data_.operator=(other);
//	}
//	Array2& operator=(const std::initializer_list<T>& lst)
//	{
//		return data_.operator=(lst);
//	}
//
//public:
//	reference operator[](size_type i)
//	{
//		return data_[i];
//	}
//	const_reference  operator[](size_type i) const
//	{
//		return data_[i];
//	}
//
//public:
//	template <typename Callback> void forEach(Callback func) const;
//	template <typename Callback> void forEachIndex(Callback func) const;
//
//	template <typename Callback> void parallelForEach(Callback func);
//	template <typename Callback> void parallelForEachIndex(Callback func) const;
//
//public:
//	reference at(size_type i)
//	{
//		return data_.at(i);
//	}
//	const_reference at(size_type i) const
//	{
//		return data_.at(i);
//	}
//
//	void resize(size_type size, const T& val = T())
//	{
//		data_.resize(size, val);
//	}
//	void clear()
//	{
//		data_.clear();
//	}
//
//	size_type size() const
//	{
//		return data_.size();
//	}
//
//	iterator begin()
//	{
//		return data_.begin();
//	}
//	iterator end()
//	{
//		return data_.end();
//	}
//	const_iterator begin() const
//	{
//		return data_.cbegin();
//	}
//	const_iterator end() const
//	{
//		return data_.cend();
//	}
//
//	void append(const T& value)
//	{
//		data_.push_back(value);
//	}
//	void append(const Array2& other)
//	{
//		data_.insert(data_.end(), other.data_.begin(), other.data_.end());
//	}
//
//	void swap(Array2& other)
//	{
//		std::swap(other.data_, data_);
//	}
//	const T* const data() const
//	{
//		return data_.data();
//	}
//	T* data()
//	{
//		return data_.data();
//	}
//
//public:
//	//operator ArrayAccessor1<T>();
//	//operator ConstArrayAccessor1<T>() const;
//	//ArrayAccessor1<T> accessor();
//	//ConstArrayAccessor1<T> constAccessor() const;
//
//private:
//	//void set_(const T& value)
//	//void set_(const Array& other);
//	//void set_(const std::initializer_list<T>& lst);
//
//private:
//	friend class boost::serialization::access;
//	template <typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
//	{
//		ar & data_;
//	}
//
//
//};
//
//NAMESPACE_BASE_END
//



///*--------------------------------------------------------------- ArrayBase */
//NAMESPACE_BASE_BEGIN
//class ArrayBase
//{
//public:
//	struct Key
//	{
//	public:
//		const size_t fBytes;
//		const Digest fDigest;
//	public:
//		Key(const size_t  bytes, const Digest& digest)
//				: fBytes(bytes),fDigest(digest)
//		{
//		}
//	};
//	struct KeyHash : std::unary_function<Key, std::size_t>
//	{
//	public:
//		std::size_t operator()(Key const& key) const
//		{
//			std::size_t seed = 0;
//			boost::hash_combine(seed, key.fBytes);
//			boost::hash_combine(seed, key.fDigest.words[0]);
//			boost::hash_combine(seed, key.fDigest.words[1]);
//			return seed;
//		}
//	};
//	struct KeyEqualTo : std::binary_function<Key, Key, bool>
//	{
//	public:
//		bool operator()(Key const& x, Key const& y) const
//		{
//			return (x.fBytes  == y.fBytes && x.fDigest == y.fDigest);
//		}
//	};
//
//public:
//	typedef void (*Callback)(const Key& array);
//	static void registerCreationCallback     (Callback callback);
//	static void unregisterCreationCallback   (Callback callback);
//	static void registerDestructionCallback  (Callback callback);
//	static void unregisterDestructionCallback(Callback callback);
//
//private:
//	const Key   fKey;
//	const bool  fIsReadable;
//
//protected:
//	ArrayBase(size_t bytes, const Digest& digest, bool isReadable);
//
//private:
//	ArrayBase(const ArrayBase&);        	        // Prohibited and not implemented.
//	const ArrayBase& operator= (const ArrayBase&);  // Prohibited and not implemented.
//
//public:
//	virtual ~ArrayBase();
//
//public:
//	size_t bytes() const
//	{
//		return fKey.fBytes;
//	}
//	Digest digest() const
//	{
//		return fKey.fDigest;
//	}
//	const Key& key() const
//	{
//		return fKey;
//	}
//	bool isReadable() const
//	{
//		return fIsReadable;
//	}
//};
//
///*--------------------------------------------------------------- ArrayReadInterface */
//template<typename T>
//class ArrayReadInterface
//{
//public:
//	ArrayReadInterface()
//	{
//	}
//	virtual ~ArrayReadInterface()
//	{
//	}
//public:
//	virtual const T* get() const = 0;
//};
//
///*--------------------------------------------------------------- Array */
//template <class T>
//class ReadableArray; // forward declearation ReadableArray
//
//template <class T>
//class Array : public ArrayBase
//{
//protected:
//	Array(size_t size, const Digest& digest, bool isReadable) : ArrayBase(size * sizeof(T), digest, isReadable)
//	{
//	}
//public:
//	virtual ~Array();
//public:
//	virtual boost::shared_ptr<const ArrayReadInterface<T>> getReadable() const      = 0;
//	virtual boost::shared_ptr<ReadableArray<T> >           getReadableArray() const = 0;
//
//public:
//	size_t size() const
//	{
//		return bytes() / sizeof(T);
//	}
//
//};
//
///*--------------------------------------------------------------- ReadableArray */
//template <typename T>
//class ReadableArray : public Array<T>,
//		              public ArrayReadInterface<T>,
//		              public boost::enable_shared_from_this<ReadableArray<T>>
//{
//protected:
//	ReadableArray(size_t size, const Digest& digest) : Array<T>(size, digest, true)
//	{
//	}
//public:
//	virtual ~ReadableArray()
//	{
//	}
//
//public:
//	virtual boost::shared_ptr<const ArrayReadInterface<T> > getReadable() const
//	{
//		return this->shared_from_this();
//	}
//
//	virtual boost::shared_ptr<ReadableArray<T> > getReadableArray() const
//	{
//		return const_cast<ReadableArray<T> *>(this)->shared_from_this();
//	}
//
//};
//
///*--------------------------------------------------------------- ArrayRegistry */
//template <typename T>
//class ArrayRegistry
//{
//public:
//	static tbb::mutex& mutex();
//public:
//	static boost::shared_ptr<Array<T> >         lookup(const Digest& digest, size_t size);
//	static boost::shared_ptr<Array<T> >         lookupNonReadable(const Digest& digest,size_t size);
//	static boost::shared_ptr<ReadableArray<T> > lookupReadable(const Digest& digest,size_t size);
//public:
//	static void insert(boost::shared_ptr<Array<T> > array);
//};


///*--------------------------------------------------------------- SharedArray */
//template <class T>
//class SharedArray : public ReadableArray<T>
//{
//public:
//	static boost::shared_ptr<ReadableArray<T> > create(const boost::shared_array<T>& data, size_t size);
//	static boost::shared_ptr<ReadableArray<T> > create(const boost::shared_array<T>& data, Digest digest, size_t size);
//
//private:
//	const boost::shared_array<T> fData;
//
//private:
//	DECL_MAKE_SHARED_FRIEND_3;
//	SharedArray(const boost::shared_array<T>& data,size_t size,const Digest& digest)
//			: ReadableArray<T>(size, digest),
//			  fData(data)
//	{
//	}
//public:
//	virtual ~SharedArray();
//
//public:
//	virtual const T* get() const;
//
//};
//NAMESPACE_BASE_END

