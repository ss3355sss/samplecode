#pragma once
#include "base/pch.h"

#if defined(ARCH_COMPILER_GCC) || defined(ARCH_COMPILER_CLANG)

#define BASE_ARCH_LIKELY(x) (__builtin_expect((bool)(x), true))
#define BASE_ARCH_UNLIKELY(x) (__builtin_expect((bool)(x), false))

#else

#define BASE_ARCH_LIKELY(x) (x)
#define BASE_ARCH_UNLIKELY(x) (x)

#endif


NAMESPACE_BASE_BEGIN
template <class T>
class StaticData
{
public:
	inline T* operator-> () const { return  StaticData<T>::Get(); }
	inline T& operator* () const  { return *StaticData<T>::Get(); }

public:
	inline static T* Get()
	{
		static std::shared_ptr<T> pData_ = nullptr;
		if(!pData_ )
			pData_ = std::make_shared<T>();
		return pData_.get();
	}
};


//template <class T>
//struct StaticData
//{
//public:
//	inline T* operator-> () const { return  Get(); }
//	inline T& operator* () const  { return *Get(); }
//
//public:
//	inline T* Get() const
//	{
//		static std::shared_ptr<T> pData_ = nullptr;
//		if(!pData_ )
//			pData_ = std::make_shared<T>();
//		return pData_.get();
//	}
//};


//template <class T>
//struct Tf_StaticDataDefaultFactory {
//	static T *New() { return new T; }
//};
//
//template <class T, class Factory = Tf_StaticDataDefaultFactory<T> >
//class TfStaticData {
//public:
//	/// Return a pointer to the underlying data object. It is created and
//	/// initialized if necessary.
//	inline T* operator-> () const { return Get(); }
//
//	/// Member lookup. The underlying data object is created and initialized
//	/// if necessary.
//	inline T& operator* () const { return *Get(); }
//
//	/// Return a pointer to the underlying object, creating and initializing
//	/// it if necessary.
//	inline T* Get() const {
//		T *p = _data;
//		return BASE_ARCH_LIKELY(p) ? p : _TryToCreateData();
//	}
//
//	/// Return true if the underlying data object is created and initialized.
//	/// Return false otherwise.
//	inline bool IsInitialized() const { return _data.load() != nullptr; }
//
//private:
//	T *_TryToCreateData() const {
//		// Allocate an instance.
//		T *tmp = Factory::New();
//
//		// Try to atomically set the pointer from null to tmp.
//		T *n = nullptr;
//		if (BASE_ARCH_LIKELY(_data.compare_exchange_strong(n, tmp)))
//			return tmp;
//
//		// Another thread won the initialization race.
//		delete tmp;
//		return _data;
//	}
//
//	mutable std::atomic<T *> _data;
//};
NAMESPACE_BASE_END

