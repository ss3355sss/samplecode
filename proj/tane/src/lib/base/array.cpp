#include "base/array.h"

#include "base/hasher.h"

#include <boost/make_shared.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>



///* internal */
//namespace {
//	USING_NAMESPACE_BASE
//	/*--------------------------------------------------------------- ArrayBaseImp Impl */
//	class ArrayBaseImpl
//	{
//	public:
//		typedef ArrayBase::Callback Callback;
//		typedef ArrayBase::Key      Key;
//
//		static void registerCreationCallback(Callback callback)
//		{
//			creationCallbacks.push_back(callback);
//		}
//
//		static void unregisterCreationCallback(Callback callback)
//		{
//			Callbacks::iterator it = std::find(creationCallbacks.begin(), creationCallbacks.end(), callback);
//			if (it != creationCallbacks.end())
//			{
//				creationCallbacks.erase(it);
//			}
//		}
//
//		static void invokeCreationCallback(const Key& key)
//		{
//			BOOST_FOREACH(const Callback& callback, creationCallbacks)
//						{
//							(*callback)(key);
//						}
//		}
//
//		static void registerDestructionCallback(Callback callback)
//		{
//			destructionCallbacks.push_back(callback);
//		}
//
//		static void unregisterDestructionCallback(Callback callback)
//		{
//			Callbacks::iterator it = std::find(destructionCallbacks.begin(), destructionCallbacks.end(), callback);
//			if (it != destructionCallbacks.end())
//			{
//				destructionCallbacks.erase(it);
//			}
//		}
//
//		static void invokeDestructionCallback(const Key& key)
//		{
//			BOOST_FOREACH(const Callback& callback, destructionCallbacks)
//			{
//							(*callback)(key);
//			}
//		}
//
//	private:
//		typedef std::vector<ArrayBase::Callback> Callbacks;
//
//		static Callbacks creationCallbacks;
//		static Callbacks destructionCallbacks;
//	};
//
//	ArrayBaseImpl::Callbacks ArrayBaseImpl::creationCallbacks;
//	ArrayBaseImpl::Callbacks ArrayBaseImpl::destructionCallbacks;
//
//	/*--------------------------------------------------------------- ArrayRegistry Impl */
//	template <typename T>
//	class ArrayRegistryImpl
//	{
//	public:
//		typedef ArrayBase::Key        Key;
//		typedef ArrayBase::KeyHash    KeyHash;
//		typedef ArrayBase::KeyEqualTo KeyEqualTo;
//
//	public:
//		typedef boost::unordered_map<Key, boost::weak_ptr<Array<T> >, KeyHash, KeyEqualTo>          Map;
//		typedef boost::unordered_map<Key, boost::weak_ptr<ReadableArray<T>>,KeyHash, KeyEqualTo>    MapReadable;
//
//	public:
//		static ArrayRegistryImpl<T>& singleton()
//		{
//			return fsSingleton;
//		}
//
//	private:
//		static ArrayRegistryImpl fsSingleton;
//
//	private:
//		tbb::mutex  fMutex;
//		Map         fMapNonReadable;
//		MapReadable fMapReadable;
//
//	public:
//		ArrayRegistryImpl()
//		{
//		}
//		~ArrayRegistryImpl()
//		{
//		}
//	public:
//		tbb::mutex& mutex()
//		{
//			return fMutex;
//		}
//		boost::shared_ptr<Array<T> > lookup(const Digest& digest, size_t size)
//		{
//			// Caller will accept either readable or non-readable.  First look for non-readable.
//			boost::shared_ptr<Array<T> > ret = lookupNonReadable(digest, size);
//			if (!ret)
//			{
//				ret = lookupReadable(digest, size);
//			}
//			return ret;
//		}
//
//		boost::shared_ptr<Array<T> > lookupNonReadable(const Digest& digest,size_t size)
//		{
//			typename Map::const_iterator it = fMapNonReadable.find(Key(size * sizeof(T), digest));
//			if (it != fMapNonReadable.end()) {
//				// Might return null if the weak_ptr<> is now dangling
//				// but not yet removed from the map...
//				boost::shared_ptr<Array<T> > ret = it->second.lock();
//				if (!ret) {
//					fMapNonReadable.erase(it);
//				}
//				return ret;
//			}
//			else {
//				return boost::shared_ptr<Array<T> >();
//			}
//		}
//
//		boost::shared_ptr<ReadableArray<T> > lookupReadable(const Digest& digest,size_t size)
//		{
//			typename MapReadable::const_iterator it = fMapReadable.find(Key(size * sizeof(T), digest));
//			if (it != fMapReadable.end()) {
//				// Might return null if the weak_ptr<> is now dangling
//				// but not yet removed from the map...
//				boost::shared_ptr<ReadableArray<T> > ret = it->second.lock();
//				if (!ret) {
//					fMapReadable.erase(it);
//				}
//				return ret;
//			}
//			else {
//				return boost::shared_ptr<ReadableArray<T> >();
//			}
//		}
//
//		void insert(boost::shared_ptr<Array<T> > array)
//		{
//			if (array->isReadable())
//			{
//				fMapReadable.insert(std::make_pair(array->key(), array->getReadableArray()));
//			}
//			else
//			{
//				fMapNonReadable.insert(std::make_pair(array->key(), array));
//			}
//		}
//		void removeIfStaled(const Key& key, bool readable)
//		{
//			if (readable)
//			{
//				typename MapReadable::const_iterator it = fMapReadable.find(key);
//				if (it != fMapReadable.end())
//				{
//					boost::shared_ptr<Array<T> > ret = it->second.lock();
//					if (!ret)
//					{
//						fMapReadable.erase(it);
//					}
//				}
//			}
//			else
//			{
//				typename Map::const_iterator it = fMapNonReadable.find(key);
//				if (it != fMapNonReadable.end())
//				{
//					boost::shared_ptr<Array<T> > ret = it->second.lock();
//					if (!ret)
//					{
//						fMapNonReadable.erase(it);
//					}
//				}
//			}
//		}
//
//	};
//
//	template <typename T>
//	ArrayRegistryImpl<T> ArrayRegistryImpl<T>::fsSingleton;
//}
//
///*--------------------------------------------------------------- ArrayBase */
//NAMESPACE_BASE_BEGIN
//void ArrayBase::registerCreationCallback(Callback callback)
//{
//	ArrayBaseImpl::registerCreationCallback(callback);
//}
//
//void ArrayBase::unregisterCreationCallback(Callback callback)
//{
//	ArrayBaseImpl::unregisterCreationCallback(callback);
//}
//
//void ArrayBase::registerDestructionCallback(Callback callback)
//{
//	ArrayBaseImpl::registerDestructionCallback(callback);
//}
//
//void ArrayBase::unregisterDestructionCallback(Callback callback)
//{
//	ArrayBaseImpl::unregisterDestructionCallback(callback);
//}
//
//ArrayBase::ArrayBase(size_t bytes, const Digest& digest, bool isReadable)
//		: fKey(bytes, digest), fIsReadable(isReadable)
//{
//	ArrayBaseImpl::invokeCreationCallback(fKey);
//}
//
//ArrayBase::~ArrayBase()
//{
//	ArrayBaseImpl::invokeDestructionCallback(fKey);
//}
//
//
///*--------------------------------------------------------------- ArrayReadInterface */
//
///*--------------------------------------------------------------- Array */
//template <typename T>
//Array<T>::~Array()
//{
//	tbb::mutex::scoped_lock lock(ArrayRegistryImpl<T>::singleton().mutex());
//	ArrayRegistryImpl<T>::singleton().removeIfStaled(key(), isReadable());
//}
//
///*--------------------------------------------------------------- ReadableArray */
///*--------------------------------------------------------------- ArrayRegistry */
//template <typename T>
//tbb::mutex& ArrayRegistry<T>::mutex()
//{
//	return ArrayRegistryImpl<T>::singleton().mutex();
//}
//template <typename T>
//boost::shared_ptr<Array<T> > ArrayRegistry<T>::lookup(const Digest& digest, size_t size)
//{
//	boost::shared_ptr<Array<T> > result = ArrayRegistryImpl<T>::singleton().lookup(digest, size);
//
//	assert(!result || result->digest() == digest);
//	assert(!result || result->bytes()  == size * sizeof(T));
//
//	return result;
//}
//
//template <typename T>
//boost::shared_ptr<Array<T> > ArrayRegistry<T>::lookupNonReadable(const Digest& digest, size_t size)
//{
//	boost::shared_ptr<Array<T> > result = ArrayRegistryImpl<T>::singleton().lookupNonReadable(digest, size);
//
//	assert(!result || result->digest() == digest);
//	assert(!result || result->bytes()  == size * sizeof(T));
//
//	return result;
//}
//
//template <typename T>
//boost::shared_ptr<ReadableArray<T> >  ArrayRegistry<T>::lookupReadable(const Digest& digest, size_t size)
//{
//	boost::shared_ptr<ReadableArray<T> > result = ArrayRegistryImpl<T>::singleton().lookupReadable(digest, size);
//
//	assert(!result || result->digest() == digest);
//	assert(!result || result->bytes()  == size * sizeof(T));
//
//	return result;
//}
//
//template <typename T>
//void ArrayRegistry<T>::insert(boost::shared_ptr<Array<T> > array)
//{
//	ArrayRegistryImpl<T>::singleton().insert(array);
//}
//
//NAMESPACE_BASE_END

///*--------------------------------------------------------------- SharedArray */
//template <typename T>
//boost::shared_ptr<ReadableArray<T>> SharedArray<T>::create(const boost::shared_array<T>& data, size_t size)
//{
//	// Compute the Murmur3 cryptographic hash-key.
//	Digest digest;
//	MurmurHash3_x64_128(data.get(), size * sizeof(T), sizeof(T), digest.words);
//	return create(data, digest, size);
//}
//
//
//template <typename T>
//boost::shared_ptr<ReadableArray<T>> SharedArray<T>::create(const boost::shared_array<T>& data, Digest digest, size_t size)
//{
//	// We first look if a similar array already exists in the
//	// cache. If so, we return the cached array to promote sharing as
//	// much as possible.
//	boost::shared_ptr<ReadableArray<T> > ret;
//	{
//		tbb::mutex::scoped_lock lock(ArrayRegistry<T>::mutex());
//
//		ret = ArrayRegistry<T>::lookupReadable(digest, size);
//
//		if (!ret)
//		{
//			ret = boost::make_shared<SharedArray<T> >(data, size, digest);
//			ArrayRegistry<T>::insert(ret);
//		}
//	}
//	return ret;
//}
//
//template <typename T>
//SharedArray<T>::~SharedArray()
//{
//}
//
//template <typename T>
//const T* SharedArray<T>::get() const
//{
//	return fData.get();
//}
//NAMESPACE_BASE_END



