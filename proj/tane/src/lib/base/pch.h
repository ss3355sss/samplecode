#pragma once
/* ----------------------------------------------- include */
/* ---------------------------- gcc */
#include <algorithm>
#include <atomic>
#include <cfloat>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include <functional>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <inttypes.h>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <stack>
#include <random>
#include <queue>
#include <bitset>

#include <functional>
#include <future>
#include <thread>

/* ---------------------------- omp */
#include <omp.h>

/* ---------------------------- cuda */
#define CUDA_NO_HALF
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

/* ----------------------------------------------- using */
using std::cout;
using std::endl;
using std::cerr;

/* ----------------------------------------------- namespace */
#define BASE_NAME_SPACE base

#define NAMESPACE_BASE_BEGIN namespace BASE_NAME_SPACE{
#define NAMESPACE_BASE_END }

#define USING_NAMESPACE_BASE using namespace BASE_NAME_SPACE;

/* ----------------------------------------------- macro */
#define UNUSED_VARIABLE(x) ((void)x)

/* Clone Support */
#define DECL_CLONE_SUPPORT(Type)                                        virtual Type *clone() const = 0;
#define IMPL_CLONE_SUPPORT(Type)                                        virtual Type *clone() const { return new Type(*this); }

#define DECL_CLONE_SUPPORT_WITH_SMART_POINTER(BaseType)                 virtual std::shared_ptr<BaseType> clone() = 0;
#define IMPL_CLONE_SUPPORT_WITH_SMART_POINTER(BaseType, DerivedType)    virtual std::shared_ptr<BaseType> clone(){ return std::shared_ptr<BaseType>((new DerivedType(static_cast<DerivedType const&>(*this)))); }

/* Static Assert for floating-pointing */
#define STATIC_ASSSERT_TEMPLATE_PARAM(CLASS_NAME, TEMPLATE_PARAM)\
    static_assert(std::is_floating_point<TEMPLATE_PARAM>::value, #CLASS_NAME " only can be instantiated with floating point types");

#define STATIC_ASSSERT_FLOATING_POINT(TYPE, MSG)\
    static_assert(std::is_floating_point<TYPE>::value, #MSG);

/* FILE NAME */
/*linux only*/
#define __PRETTY_FILENAME__ (strrchr(__FILE__, '/')+1)
#define __FILE_NAME__ (strrchr(__FILE__, '/')+1)

/* FUNCTION NAME */
#ifndef __FUNCTION_NAME__
#define __FUNCTION_NAME__ std::string(std::string(__func__) + "()").c_str()
#endif

/* STRINGIFY */
#define STRINGIFY(s) #s

/* ENUM TO STRING */
#include <boost/tokenizer.hpp>
inline std::string _EnumToString(const std::string value)
{
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
	boost::char_separator<char>                           sep{"::"};
	Tokenizer                                             tokenizer(value, sep);
	std::vector<std::string>                              tokens;
	for (Tokenizer::iterator                              iter = tokenizer.begin(); iter != tokenizer.end(); ++iter)
		tokens.push_back(*iter);
	return tokens[tokens.size() - 1];
}
#define ENUM_TO_STRING(ENUM) _EnumToString(#ENUM)

/* STDOUT MESSAGE HELPER */
inline void __stdout_set_color__(const int color)
{
	switch (color)
	{
		case 0: //message
			fprintf(stdout, "\033[0m");
			break;
		case 1: //info
			fprintf(stdout, "\033[1;32m");
			break;
		case 2: //warnig
			fprintf(stdout, "\033[1;33m");
			break;
		case 3: //error
			fprintf(stdout, "\033[1;31m");
			break;
	}
}
inline void __stdout_clear_color__()
{
	fprintf(stdout, "\033[0m");
	
}
inline void __stdout_message__(const int color, const char* fmt, ...)
{
	//__stdout_set_color__(color);
	
	va_list args;
	va_start(args, fmt);
	int result   = -1, length = 256;
	char* buffer = 0;
	while (result == -1)
	{
		if(buffer) delete[] buffer;
		buffer = new char[length + 1];
		memset(buffer, 0, length + 1);
		result = vsnprintf(buffer, length, fmt, args);
		length *= 2;
	}
	std::string s(buffer);
	delete[] buffer;
	va_end(args);
	
	std::cout << s << "\n";
	
	//__stdout_clear_color__();
	
}

/*----------------------------------------------------------------- STDOUT_DEBUG */
#define STDOUT_DEBUG0(msg)                            if(ENABLE_LOGGER) { __stdout_set_color__(0); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(0, msg);                         __stdout_clear_color__();}
#define STDOUT_DEBUG1(msg, arg1)                    if(ENABLE_LOGGER) { __stdout_set_color__(0); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(0, msg, arg1);                   __stdout_clear_color__();}
#define STDOUT_DEBUG2(msg, arg1, arg2)                if(ENABLE_LOGGER) { __stdout_set_color__(0); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(0, msg, arg1, arg2);             __stdout_clear_color__();}
#define STDOUT_DEBUG3(msg, arg1, arg2, arg3)        if(ENABLE_LOGGER) { __stdout_set_color__(0); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(0, msg, arg1, arg2, arg3);       __stdout_clear_color__();}
#define STDOUT_DEBUG4(msg, arg1, arg2, arg3, arg4)    if(ENABLE_LOGGER) { __stdout_set_color__(0); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(0, msg, arg1, arg2, arg3, arg4); __stdout_clear_color__();}
#define GET_MACRO_STDOUT_DEBUG(_1, _2, _3, _4, _5, NAME, ...) NAME

#ifdef DEBUG
#define STDOUT_DEBUG(...) GET_MACRO_STDOUT_DEBUG(__VA_ARGS__,  \
                                                     STDOUT_DEBUG4,\
                                                     STDOUT_DEBUG3,\
                                                     STDOUT_DEBUG2,\
                                                     STDOUT_DEBUG1,\
                                                     STDOUT_DEBUG0) (__VA_ARGS__)
#else
#define STDOUT_DEBUG(...)
#endif

/*----------------------------------------------------------------- STDOUT_INFO */
#define STDOUT_INFO0(msg)                           if(ENABLE_LOGGER) { __stdout_set_color__(1); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(1, msg);                         __stdout_clear_color__();}
#define STDOUT_INFO1(msg, arg1)                     if(ENABLE_LOGGER) { __stdout_set_color__(1); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(1, msg, arg1);                   __stdout_clear_color__();}
#define STDOUT_INFO2(msg, arg1, arg2)               if(ENABLE_LOGGER) { __stdout_set_color__(1); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(1, msg, arg1, arg2);             __stdout_clear_color__();}
#define STDOUT_INFO3(msg, arg1, arg2, arg3)         if(ENABLE_LOGGER) { __stdout_set_color__(1); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(1, msg, arg1, arg2, arg3);       __stdout_clear_color__();}
#define STDOUT_INFO4(msg, arg1, arg2, arg3, arg4)   if(ENABLE_LOGGER) { __stdout_set_color__(1); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(1, msg, arg1, arg2, arg3, arg4); __stdout_clear_color__();}
#define GET_MACRO_STDOUT_INFO(_1, _2, _3, _4, _5, NAME, ...) NAME

#ifdef DEBUG
#define STDOUT_INFO(...) GET_MACRO_STDOUT_INFO(__VA_ARGS__,  \
                                                   STDOUT_INFO4, \
                                                   STDOUT_INFO3, \
                                                   STDOUT_INFO2, \
                                                   STDOUT_INFO1, \
                                                   STDOUT_INFO0) (__VA_ARGS__)
#else
#define STDOUT_INFO(...)
#endif

/* ----------------------------------------------------------------- STDOUT_WARNING */
#define STDOUT_WARRNING0(msg)                        if(ENABLE_LOGGER) { __stdout_set_color__(2); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(2, msg);                   __stdout_clear_color__();}
#define STDOUT_WARRNING1(msg, arg1)                    if(ENABLE_LOGGER) { __stdout_set_color__(2); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(2, msg, arg1);             __stdout_clear_color__();}
#define STDOUT_WARRNING2(msg, arg1, arg2)            if(ENABLE_LOGGER) { __stdout_set_color__(2); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(2, msg, arg1, arg2);       __stdout_clear_color__();}
#define STDOUT_WARRNING3(msg, arg1, arg2, arg3)        if(ENABLE_LOGGER) { __stdout_set_color__(2); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::";  __stdout_message__(2, msg, arg1, arg2, arg3); __stdout_clear_color__();}
#define GET_MACRO_STD_WARRNING(_1, _2, _3, _4, NAME, ...) NAME
#ifdef DEBUG
#define STDOUT_WARRNING(...) GET_MACRO_STD_WARRNING(__VA_ARGS__,      \
                                                        STDOUT_WARRNING3, \
                                                        STDOUT_WARRNING2, \
                                                        STDOUT_WARRNING1, \
                                                        STDOUT_WARRNING0)(__VA_ARGS__)
#else
#define STDOUT_WARRNING(...)
#endif

/* ----------------------------------------------------------------- STDOUT_ERROR */
#define STDOUT_ERROR0(msg)                        if(ENABLE_LOGGER) { __stdout_set_color__(3); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(3, msg);                     __stdout_clear_color__();}
#define STDOUT_ERROR1(msg, arg1)                if(ENABLE_LOGGER) { __stdout_set_color__(3); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(3, msg, arg1);               __stdout_clear_color__();}
#define STDOUT_ERROR2(msg, arg1, arg2)            if(ENABLE_LOGGER) { __stdout_set_color__(3); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(3, msg, arg1, arg2);         __stdout_clear_color__();}
#define STDOUT_ERROR3(msg, arg1, arg2, arg3)    if(ENABLE_LOGGER) { __stdout_set_color__(3); std::cout << __FILE_NAME__ << "::" << __FUNCTION_NAME__ << "::"; __stdout_message__(3, msg, arg1, arg2, arg3);   __stdout_clear_color__();}
#define GET_MACRO_STD_OUTERROR(_1, _2, _3, _4, NAME, ...) NAME
#ifdef DEBUG
#define STDOUT_ERROR(...) GET_MACRO_STD_OUTERROR(__VA_ARGS__,    \
                                                     STDOUT_ERROR3,  \
                                                     STDOUT_ERROR2,  \
                                                     STDOUT_ERROR1,  \
                                                     STDOUT_ERROR0) (__VA_ARGS__)
#else
#define STDOUT_ERROR(...)
#endif

#define CHECKERROR0(_STAT, _MSG)                if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG);              return false; };
#define CHECKERROR1(_STAT, _MSG, _1)            if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1);          return false; };
#define CHECKERROR2(_STAT, _MSG, _1, _2)        if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1, _2);      return false; };
#define CHECKERROR3(_STAT, _MSG, _1, _2, _3)    if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1, _2, _3);  return false; };
#define GET_MACRO_CHECKERROR(_1, _2, _3, _4, _5, NAME, ...) NAME
#define CHECKERROR(...) GET_MACRO_CHECKERROR(__VA_ARGS__, \
                                                 CHECKERROR3,\
                                                 CHECKERROR2,\
                                                 CHECKERROR1,\
                                                 CHECKERROR0)\
                                                 (__VA_ARGS__)
#ifdef DEBUG
#else
#define CHECKERROR(...)
#endif

#define CHECKERRORNORET0(_STAT, _MSG)                if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG); };
#define CHECKERRORNORET1(_STAT, _MSG, _1)            if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1); };
#define CHECKERRORNORET2(_STAT, _MSG, _1, _2)        if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1, _2); };
#define CHECKERRORNORET3(_STAT, _MSG, _1, _2, _3)    if ( _STAT <= 0 ) { STDOUT_ERROR(_MSG, _1, _2, _3); };
#define GET_MACRO_CHECKERRORNORET(_1, _2, _3, _4, _5, NAME, ...) NAME
#ifdef DEBUG
#define CHECKERRORNORET(...) GET_MACRO_CHECKERRORNORET(__VA_ARGS__, \
                                                           CHECKERRORNORET3,\
                                                           CHECKERRORNORET2,\
                                                           CHECKERRORNORET1,\
                                                           CHECKERRORNORET0)\
                                                           (__VA_ARGS__)
#else
#define CHECKERRORNORET(...)
#endif

//template <typename... Args>
//inline bool _GlStateValid(const char *fmt, Args... args)
//{
//	static const size_t bufferSize = 1024;
//
//	const GLubyte* errString;
//	GLenum err = glGetError();
//	if((err == GL_NO_ERROR))
//	{
//		return true;
//	}
//	else
//	{
//		char msg[bufferSize];
//		sprintf(msg, fmt, args...);
//		STDOUT_ERROR("%s - %s", gluErrorString(err), msg);
//		return false;
//	}
//
//}
//
//#define GLCHECKERROR0(MSG)              { if(_GlStateValid(MSG)             != true) { return false; }}
//#define GLCHECKERROR1(MSG, _1)          { if(_GlStateValid(MSG, _1)         != true) { return false; }}
//#define GLCHECKERROR2(MSG, _1, _2)      { if(_GlStateValid(MSG, _1, _2)     != true) { return false; }}
//#define GLCHECKERROR3(MSG, _1, _2, _3)  { if(_GlStateValid(MSG, _1, _2, _3) != true) { return false; }}
//#define GET_MACRO_GLCHECKERROR(_1, _2, _3, _4, NAME, ...) NAME
//
//#define GLCHECKERRORNORET0(MSG)              { _GlStateValid(MSG)            ;}
//#define GLCHECKERRORNORET1(MSG, _1)          { _GlStateValid(MSG, _1)        ;}
//#define GLCHECKERRORNORET2(MSG, _1, _2)      { _GlStateValid(MSG, _1, _2)    ;}
//#define GLCHECKERRORNORET3(MSG, _1, _2, _3)  { _GlStateValid(MSG, _1, _2, _3);}
//#define GET_MACRO_GLCHECKERRORNORET(_1, _2, _3, _4, NAME, ...) NAME
//
//#ifdef DEBUG
//#define GLCHECKERROR(...) \
//		GET_MACRO_GLCHECKERROR(__VA_ARGS__, \
//	                           GLCHECKERROR3,\
//	                           GLCHECKERROR2,\
//	                           GLCHECKERROR1,\
//	                           GLCHECKERROR0)\
//	                           (__VA_ARGS__)
//#define GLCHECKERRORNORET(...) \
//		GET_MACRO_GLCHECKERRORNORET(__VA_ARGS__, \
//	                           GLCHECKERRORNORET3,\
//	                           GLCHECKERRORNORET2,\
//	                           GLCHECKERRORNORET1,\
//	                           GLCHECKERRORNORET0)\
//	                           (__VA_ARGS__)
//#else
//#define GLCHECKERROR(...)
//	#define GLCHECKERRORNORET(...)
//#endif

/* -----------------------------------------------------------------  IMPL_SET */
#define IMPL_SET(CLASS, CONTAINER, VAR)\
	public:\
		typedef typename CONTAINER::key_type                  key_type;                                                                             \
		typedef typename CONTAINER::value_type                value_type;                                                                           \
		typedef typename CONTAINER::key_compare               key_compare;                                                                          \
		typedef typename CONTAINER::value_compare             value_compare;                                                                        \
		typedef typename CONTAINER::allocator_type            allocator_type;                                                                       \
	    typedef typename CONTAINER::pointer                   pointer;                                                                              \
		typedef typename CONTAINER::const_pointer             const_pointer;                                                                        \
		typedef typename CONTAINER::reference                 reference;                                                                            \
		typedef typename CONTAINER::const_reference           const_reference;                                                                      \
		typedef typename CONTAINER::const_iterator            iterator;                                                                             \
		typedef typename CONTAINER::const_iterator            const_iterator;                                                                       \
		typedef typename CONTAINER::const_reverse_iterator    reverse_iterator;                                                                     \
		typedef typename CONTAINER::const_reverse_iterator    const_reverse_iterator;                                                               \
		typedef typename CONTAINER::size_type                 size_type;                                                                            \
		typedef typename CONTAINER::difference_type           difference_type;                                                                      \
	public:                                                                                                                                         \
		CLASS()                                                                        : VAR()                              {}                      \
		CLASS(const key_compare& __comp, const allocator_type& __a = allocator_type()) : VAR(__comp, __a)                   {}                      \
		template<typename _InputIterator>                                                                                                           \
		CLASS(_InputIterator __first, _InputIterator __last)                           : VAR(__first, __last)               {}                      \
		template<typename _InputIterator>                                                                                                           \
		CLASS(_InputIterator __first,                                                                                                               \
              _InputIterator __last,                                                                                                                \
              const key_compare& __comp,                                                                                                            \
              const allocator_type& __a = allocator_type())                            : VAR(__first, __last, __comp, __a)  {}                      \
		CLASS(std::initializer_list<value_type> __l,                                                                                                \
              const key_compare& __comp = key_compare(),                                                                                            \
              const allocator_type& __a = allocator_type())                            : VAR(__l, __comp, __a)              {};                     \
		CLASS(const CLASS& __x)                                                        : VAR(__x.VAR)                       {};                     \
		~CLASS()                                                                                                            {};                     \
	public:                                                                                                                                         \
		CLASS& operator=(const CLASS& __x)                                                          { VAR.operator=(__x.VAR); return *this;     }   \
		CLASS& operator=(CLASS&& __x)                                                               { VAR.operator=(__x.VAR); return *this;     }   \
		CLASS& operator=(std::initializer_list<value_type> __l)                                     { VAR.operator=(__l); return *this;         }   \
	public:                                                                                                                                         \
		key_compare         key_comp() const                                                        { return VAR.key_comp();                    }   \
		value_compare       value_comp() const                                                      { return VAR.value_comp();                  }   \
		allocator_type      get_allocator() const                                                   { return VAR.get_allocator();               }   \
		iterator            begin() const                                                           { return VAR.begin();                       }   \
		iterator            end() const                                                             { return VAR.end();                         }   \
		reverse_iterator    rbegin() const                                                          { return VAR.rbegin();                      }   \
		reverse_iterator    rend() const                                                            { return VAR.rend();                        }   \
		iterator            cbegin() const                                                          { return VAR.cbegin();                      }   \
		iterator            cend() const                                                            { return VAR.cend();                        }   \
		reverse_iterator    crbegin() const                                                         { return VAR.crbegin();                     }   \
		reverse_iterator    crend() const                                                           { return VAR.crend();                       }   \
	public:                                                                                                                                         \
		bool        empty()     const                                                               { return VAR.empty();                       }   \
		size_type   size()      const                                                               { return VAR.size();                        }   \
		size_type   max_size()  const                                                               { return VAR.max_size();                    }   \
		void        swap(CLASS& __x)                                                                { VAR.swap(__x.VAR);                        }   \
	public:                                                                                                                                         \
		template<typename... _Args>                                                                                                                 \
		std::pair<iterator, bool> emplace(_Args&&... __args)                                        { return VAR.emplace(__args...);            }   \
		template<typename... _Args>                                                                                                                 \
		iterator emplace_hint(const_iterator __pos, _Args&&... __args)                              { return VAR.emplace_hint(__pos, __args...);}   \
	public:                                                                                                                                         \
		std::pair<iterator, bool> insert(const value_type& __x)                                     { return VAR.insert(__x);                   }   \
		std::pair<iterator, bool> insert(value_type&& __x)                                          { return VAR.insert(__x);                   }   \
		iterator insert(const_iterator __position, const value_type& __x)                           { return VAR.insert(__position, __x);       }   \
		iterator insert(const_iterator __position, value_type&& __x)                                { return VAR.insert(__position, __x);       }   \
		template<typename _InputIterator>                                                                                                           \
		void insert(_InputIterator __first, _InputIterator __last)                                  { VAR.insert(__first, __last);              }   \
		void insert(std::initializer_list<value_type> __l)                                          { VAR.insert(__l);                          }   \
	public:                                                                                                                                         \
		iterator    erase(const_iterator __position)                                                {return VAR.erase(__position);              }   \
		size_type   erase(const key_type& __x)                                                      {return VAR.erase(__x);                     }   \
		iterator    erase(const_iterator __first, const_iterator __last)                            {return VAR.erase(__first, __last);         }   \
	public:                                                                                                                                         \
		void        clear()                                                                         { VAR.clear();                              }   \
		size_type   count(const key_type& __x) const                                                { return VAR.count(__x);                    }   \
	public:                                                                                                                                         \
		iterator        find(const key_type& __x)                                                   { return VAR.find(__x);                     }   \
		const_iterator  find(const key_type& __x) const                                             { return VAR.find(__x);                     }   \
	public:                                                                                                                                         \
		iterator        lower_bound(const key_type& __x)                                            { return VAR.lower_bound(__x);              }   \
		const_iterator  lower_bound(const key_type& __x) const                                      { return VAR.lower_bound(__x);              }   \
		iterator        upper_bound(const key_type& __x)                                            { return VAR.upper_bound(__x);              }   \
		const_iterator  upper_bound(const key_type& __x) const                                      { return VAR.upper_bound(__x);              }   \
	public:                                                                                                                                         \
		std::pair<iterator, iterator>               equal_range(const key_type& __x)                { return VAR.equal_range(__x);              }   \
		std::pair<const_iterator, const_iterator>   equal_range(const key_type& __x) const          { return VAR.equal_range(__x);              }   \
	public:                                                                                                                                         \
		friend bool operator==(const CLASS& lhs, const CLASS& rhs)                                  { return lhs.VAR == rhs.VAR;                }   \
		friend bool operator< (const CLASS& lhs, const CLASS& rhs)                                  { return lhs.VAR <  rhs.VAR;                }

/* -----------------------------------------------------------------  IMPL_VECTOR */
#define IMPL_VECTOR(_Class, _ContainerT, _Variable)\
    public:\
        typedef typename _ContainerT::value_type                   value_type;\
        typedef typename _ContainerT::pointer                      pointer;\
        typedef typename _ContainerT::const_pointer                const_pointer;\
        typedef typename _ContainerT::reference                    reference;\
        typedef typename _ContainerT::const_reference              const_reference;\
        typedef typename _ContainerT::const_iterator               const_iterator;\
        typedef typename _ContainerT::iterator                     iterator;\
        typedef typename _ContainerT::const_reverse_iterator       const_reverse_iterator;\
        typedef typename _ContainerT::reverse_iterator             reverse_iterator;\
        typedef typename _ContainerT::size_type                    size_type;\
        typedef typename _ContainerT::difference_type              difference_type;\
        typedef typename _ContainerT::allocator_type               allocator_type;\
        \
        _Class() : _Variable() {}\
        explicit _Class(const allocator_type& __a) : _Variable(__a) {}\
        explicit _Class(size_type __n, const allocator_type& __a = allocator_type()) : _Variable(__n, __a) {} \
        _Class(size_type __n, const value_type& __value, const allocator_type& __a = allocator_type()) : _Variable(__n, __value, __a) {}\
        _Class(const _Class& __x) : _Variable(__x._Variable) {}\
        _Class(_Class&& __x) : _Variable(std::move(__x._Variable)) {} \
        _Class(const _Class& __x, const allocator_type& __a) : _Variable(__x._Variable, __a) {}\
        _Class(_Class&& __rv, const allocator_type& __m) : _Variable(std::move(__rv._Variable), __m) {}\
        _Class(std::initializer_list<value_type> __l, const allocator_type& __a = allocator_type()) : _Variable(__l, __a) {}\
        template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>\
        _Class(_InputIterator __first, _InputIterator __last, const allocator_type& __a = allocator_type()) : _Variable(__first, __last, __a){}\
        ~_Class(){}\
        \
        _Class& operator=(const _Class& __x)                        { _Variable.operator=(__x._Variable); return *this; }\
        _Class& operator=(_Class&& __x)                             { _Variable.operator=(__x._Variable); return *this; }\
        _Class& operator=(std::initializer_list<value_type> __l)    { _Variable.operator=(__l); return *this;}\
        \
        template<typename _InputIterator,typename = std::_RequireInputIter<_InputIterator>>\
        void assign(_InputIterator __first, _InputIterator __last)  { _Variable.assign(__first, __last);        }\
        void assign(size_type __n, const value_type& __val)        { _Variable.assign(__n, __val);             }\
        void assign(std::initializer_list<value_type> __l)        { _Variable.assign(__l.begin(), __l.end()); }\
        \
        iterator begin()                                            { return _Variable.begin(); }\
        const_iterator begin() const                                { return _Variable.cbegin(); }\
        iterator end()                                            { return _Variable.end(); }\
        const_iterator end() const                                { return _Variable.cend(); }\
        reverse_iterator rbegin()                                    { return _Variable.rbegin(); }\
        const_reverse_iterator rbegin() const                        { return _Variable.crbegin(); }\
        reverse_iterator rend()                                    { return _Variable.rend(); }\
        const_reverse_iterator rend() const                        { return _Variable.crend(); }\
        const_iterator cbegin() const                                { return _Variable.cbegin(); }\
        const_iterator cend() const                                { return _Variable.cend(); }\
        const_reverse_iterator crbegin() const                    { return _Variable.crbegin(); }\
        const_reverse_iterator crend() const                        { return _Variable.crend(); }\
        \
        size_type size() const                                    { return _Variable.size(); }\
        size_type max_size() const                                { return _Variable.max_size(); }\
        \
        void resize(size_type __new_size)                            { _Variable.resize(__new_size); }\
        void resize(size_type __new_size, const value_type& __x)    { _Variable.resize(__new_size, __x); }\
        void shrink_to_fit()                                        { _Variable.shrink_to_fit();}\
        size_type capacity() const                                { return _Variable.capacity();}\
        bool empty() const                                        { return _Variable.empty();}\
        void reserve(size_type __n)                                    { _Variable.reserve(__n);}\
        \
        reference operator[](size_type __n)                        { return _Variable[__n];}\
        const_reference operator[](size_type __n) const            { return _Variable[__n];}\
        reference at(size_type __n)                                { return _Variable.at(__n); }\
        const_reference at(size_type __n) const                    { return _Variable.at(__n); }\
        \
        reference front()                                        { return _Variable.front(); }\
        const_reference front() const                            { return _Variable.front(); }\
        reference back()                                            { return _Variable.back(); }\
        const_reference back() const                                { return _Variable.back(); }\
        \
        pointer data()                                            { return _Variable.data(); }\
        const_pointer data() const                                    { return _Variable.data(); }\
        \
        void push_back(const value_type& __x)                                            { _Variable.push_back(__x);}\
        void push_back(value_type&& __x)                                                { _Variable.push_back(__x);}\
        template<typename... _Args>\
        void emplace_back(_Args&&... __args)                                            { _Variable.emplace_back(__args...);}\
        void pop_back()                                                                { _Variable.pop_back();}\
        template<typename... _Args>\
        iterator emplace(iterator __position, _Args&&... __args)                        { return _Variable.emplace(__position, __args...);    }\
        iterator insert(iterator __position, const value_type& __x)                    { return _Variable.insert(__position, __x);        }\
        iterator insert(iterator __position, value_type&& __x)                            { return _Variable.insert(__position, __x);            }\
        void insert(iterator __position, std::initializer_list<value_type> __l)        { _Variable.insert(__position, __l);}\
        void insert(iterator __position, size_type __n, const value_type& __x)            { _Variable.insert(__position, __n, __x);}\
        template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>\
        void insert(iterator __position, _InputIterator __first, _InputIterator __last) { _Variable.insert(__position, __first, __last);}\
        iterator erase(iterator __position)                                                { return _Variable.erase(__position);}\
        iterator erase(iterator __first, iterator __last)                                { return _Variable.erase(__first, __last);}\
        \
        void swap(_Class& __x)                                                            { _Variable.swap(__x._Variable); }\
        void clear()                                                                    { _Variable.clear(); }\


/* -----------------------------------------------------------------  IMPL_MAP */
#define IMPL_MAP(_Class, _ContainerT, _Variable)                                                                                                                                                                                 \
    public:                                                                                                                                                                                                                 \
    typedef typename _ContainerT::key_type                 key_type;\
    typedef typename _ContainerT::mapped_type             mapped_type;\
    typedef typename _ContainerT::value_type             value_type;\
    typedef typename _ContainerT::key_compare            key_compare;\
    typedef typename _ContainerT::allocator_type         allocator_type;\
    typedef typename _ContainerT::pointer                 pointer;\
    typedef typename _ContainerT::const_pointer         const_pointer;\
    typedef typename _ContainerT::reference             reference;\
    typedef typename _ContainerT::const_reference         const_reference;\
    typedef typename _ContainerT::iterator               iterator;\
    typedef typename _ContainerT::const_iterator         const_iterator;\
    typedef typename _ContainerT::size_type              size_type;\
    typedef typename _ContainerT::difference_type        difference_type;\
    typedef typename _ContainerT::reverse_iterator       reverse_iterator;\
    typedef typename _ContainerT::const_reverse_iterator const_reverse_iterator;\
    \
    _Class() : _Variable() {}\
    explicit _Class(const key_compare& __comp, const allocator_type& __a = allocator_type()) : _Variable(__comp, __a) {}\
    _Class(const _Class& __x) : _Variable(__x._Variable) {}\
    _Class(_Class&& __x) noexcept(std::is_nothrow_copy_constructible<key_compare>::value) : _Variable(std::move(__x._Variable)){}\
    _Class(std::initializer_list<value_type> __l, const key_compare& __comp = key_compare(), const allocator_type& __a = allocator_type()) : _Variable(__l, __comp, __a){}\
    template<typename _InputIterator>\
     _Class(_InputIterator __first, _InputIterator __last) : _Variable(__first, __last){}\
    template<typename _InputIterator>\
     _Class(_InputIterator __first, _InputIterator __last, const key_compare& __comp, const allocator_type& __a = allocator_type()) : _Variable(__first, __last, __comp, __a) {}\
    virtual ~_Class() { _Variable.clear();}\
    \
    _Class& operator=(const _Class& __x)                        { _Variable.operator=(__x._Variable);   return *this;}\
    _Class& operator=(_Class&& __x)                             { _Variable.operator=(__x._Variable);   return *this;}\
    _Class& operator=(std::initializer_list<value_type> __l)    { _Variable.operator=(__l);             return *this;}\
    \
    allocator_type get_allocator() const    { return _Variable.get_allocator(); }\
    \
    iterator begin()                        { return _Variable.begin(); }\
    const_iterator begin() const            { return _Variable.begin(); }\
    iterator end()                            { return _Variable.end(); }\
    const_iterator end() const                { return _Variable.end(); }\
    reverse_iterator rbegin()                { return _Variable.rbegin(); }\
    const_reverse_iterator rbegin() const    { return _Variable.rbegin(); }\
    reverse_iterator rend()                { return _Variable.rend(); }\
    const_reverse_iterator rend() const    { return _Variable.rend(); }\
    const_iterator cbegin() const            { return _Variable.cbegin(); }\
    const_iterator cend() const            { return _Variable.cend(); }\
    const_reverse_iterator crbegin() const    { return _Variable.crbegin(); }\
    const_reverse_iterator crend() const    { return _Variable.crend(); }\
    \
    bool empty() const                        { return _Variable.empty(); }\
    size_type size() const                    { return _Variable.size(); }\
    size_type max_size() const                { return _Variable.max_size(); }\
    \
    mapped_type& operator[](const key_type& __k)        { return _Variable.operator[](__k);}\
    mapped_type& operator[](key_type&& __k)            { return _Variable.operator[](__k);}\
    mapped_type& at(const key_type& __k)                { _Variable.at(__k);}\
    const mapped_type& at(const key_type& __k) const    { _Variable.at(__k);}\
    \
    template<typename... _Args>\
    std::pair<iterator, bool> emplace(_Args&&... __args)                                { return _Variable.emplace(__args...); }\
    template<typename... _Args> \
    iterator emplace_hint(const_iterator __pos, _Args&&... __args)                        { return _Variable.emplace_hint(__pos, __args...); }\
    \
    std::pair<iterator, bool> insert(const value_type& __x)                                { return _Variable.insert(__x); }\
    template<typename _Pair, typename = typename std::enable_if<std::is_constructible<value_type, _Pair&&>::value>::type>\
    std::pair<iterator, bool> insert(_Pair&& __x)                                        { return _Variable.insert(std::forward<_Pair>(__x)); }\
    void insert(std::initializer_list<value_type> __list)                                { _Variable.insert(__list.begin(), __list.end()); }\
    iterator insert(const_iterator __position, const value_type& __x)                    { return _Variable.insert(__position, __x); }\
    template<typename _Pair, typename = typename std::enable_if<std::is_constructible<value_type, _Pair&&>::value>::type>\
    iterator insert(const_iterator __position, _Pair&& __x)                            { return _Variable.insert(__position, std::forward<_Pair>(__x)); }\
    template<typename _InputIterator>\
    void insert(_InputIterator __first, _InputIterator __last)                            { _Variable.insert(__first, __last); }\
    \
    iterator erase(const_iterator __position)                                            { return _Variable.erase(__position); }\
    iterator erase(iterator __position)                                                { return _Variable.erase(__position); }\
    size_type erase(const key_type& __x)                                                { return _Variable.erase(__x);}\
    iterator erase(const_iterator __first, const_iterator __last)                    { return _Variable.erase(__first, __last); }\
    \
    void swap(_Class& __x)                                                            { _Variable.swap(__x._Variable); }\
    void clear()                                                                        { _Variable.clear(); }\
    \
    key_compare key_comp() const                                                        { return _Variable.key_comp(); }\
    /*typename*/ _ContainerT::value_compare value_comp() const                        { return _Variable.value_comp(); /*typename _ContainerT::value_compare(_Variable.key_comp());*/ }\
    \
    iterator find(const key_type& __x)                                                { return _Variable.find(__x); }\
    const_iterator find(const key_type& __x) const                                    { return _Variable.find(__x); }\
    \
    size_type count(const key_type& __x) const                                        { return _Variable.find(__x) == _Variable.end() ? 0 : 1; }\
    \
    iterator lower_bound(const key_type& __x)                                        { return _Variable.lower_bound(__x); }\
    const_iterator lower_bound(const key_type& __x) const                            { return _Variable.lower_bound(__x); }\
    iterator upper_bound(const key_type& __x)                                        { return _Variable.upper_bound(__x); }\
    const_iterator upper_bound(const key_type& __x) const                            { return _Variable.upper_bound(__x); }\
    \
    std::pair<iterator, iterator> equal_range(const key_type& __x)                    { return _Variable.equal_range(__x); }\
    std::pair<const_iterator, const_iterator> equal_range(const key_type& __x) const    { return _Variable.equal_range(__x); }\

#include <boost/smart_ptr.hpp>
#define DECL_MAKE_SHARED_FRIEND_0 template< class AA > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared();
#define DECL_MAKE_SHARED_FRIEND_1 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_2 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_3 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_4 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_5 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_6 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_7 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_8 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );
#define DECL_MAKE_SHARED_FRIEND_9 template< class AA, class Arg1, class... Args > friend typename boost::detail::sp_if_not_array< AA >::type boost::make_shared( Arg1 && arg1, Args && ... args );

/* ----------------------------------------------- constant */
NAMESPACE_BASE_BEGIN
/* ---------------------------- types */
inline const std::type_info& BoolType()
{
	const std::type_info& type = typeid(bool);
	return type;
}

inline const std::type_info& CharType()
{
	const std::type_info& type = typeid(char);
	return type;
}
inline const std::type_info& Int8Type()
{
	const std::type_info& type = typeid(int8_t);
	return type;
}

inline const std::type_info& ShortType()
{
	const std::type_info& type = typeid(short);
	return type;
}
inline const std::type_info& Int16Type()
{
	const std::type_info& type = typeid(int16_t);
	return type;
}

inline const std::type_info& IntType()
{
	const std::type_info& type = typeid(int);
	return type;
}
inline const std::type_info& Int32Type()
{
	const std::type_info& type = typeid(int32_t);
	return type;
}

inline const std::type_info& LongIntType()
{
	const std::type_info& type = typeid(long int);
	return type;
}
inline const std::type_info& Int64Type()
{
	const std::type_info& type = typeid(int64_t);
	return type;
}

inline const std::type_info& UCharType()
{
	const std::type_info& type = typeid(unsigned char);
	return type;
}
inline const std::type_info& UInt8Type()
{
	const std::type_info& type = typeid(uint8_t);
	return type;
}

inline const std::type_info& UShortType()
{
	const std::type_info& type = typeid(unsigned short);
	return type;
}
inline const std::type_info& UInt16Type()
{
	const std::type_info& type = typeid(uint16_t);
	return type;
}

inline const std::type_info& UIntType()
{
	const std::type_info& type = typeid(unsigned int);
	return type;
}
inline const std::type_info& UInt32Type()
{
	const std::type_info& type = typeid(uint32_t);
	return type;
}

inline const std::type_info& ULongIntType()
{
	const std::type_info& type = typeid(unsigned long int);
	return type;
}
inline const std::type_info& UInt64Type()
{
	const std::type_info& type = typeid(uint64_t);
	return type;
}

inline const std::type_info& FloatType()
{
	const std::type_info& type = typeid(float);
	return type;
}
inline const std::type_info& DoubleType()
{
	const std::type_info& type = typeid(double);
	return type;
}

/* ----------------------------  kilobyte */
constexpr long double kKiloByte = 1024.0;
template<typename T>
constexpr inline T KiloByte()
{
	return static_cast<T>(kKiloByte);
}
/* ---------------------------- megabyte */
constexpr long double kMegaByte = 1024.0 * 1024.0;
template<typename T>
constexpr inline T MegaByte()
{
	return static_cast<T>(kMegaByte);
}
/* ---------------------------- gigabyte */
constexpr long double kGigaByte = 1024.0 * 1024.0 * 1024.0;
template<typename T>
constexpr inline T GigaByte()
{
	return static_cast<T>(kGigaByte);
}

/* ---------------------------- Zero */
template<typename T>
constexpr inline T Zero()
{
	return static_cast<T>(0);
}

/* ---------------------------- One */
template<typename T>
constexpr inline T One()
{
	return static_cast<T>(1);
}

/* ---------------------------- Epsilon */
template<typename T>
constexpr inline T Epsilon()
{
	return std::numeric_limits<T>::epsilon();
}

/* ---------------------------- Infinity */
template<typename T>
constexpr inline T Infinity()
{
	return std::numeric_limits<T>::infinity();
}

/* ---------------------------- Max */
template<typename T>
constexpr inline T Max()
{
	return std::numeric_limits<T>::max();
}

/* ---------------------------- Max Size */
constexpr inline size_t MaxSize()
{
	return std::numeric_limits<size_t>::max();
}
/* ---------------------------- Max SSize */
constexpr inline ssize_t MaxSSize()
{
	return std::numeric_limits<ssize_t>::max();
}

/* ---------------------------- Pi */
constexpr float  kPiF = 3.14159265358979323846264338327950288f;
constexpr double kPiD = 3.14159265358979323846264338327950288;
template<typename T>
constexpr inline T Pi()
{
	return static_cast<T>(kPiD);
}

/* ---------------------------- HalfPi */
constexpr float  kHalfPiF = 1.57079632679489661923132169163975144f;
constexpr double kHalfPiD = 1.57079632679489661923132169163975144;
template<typename T>
constexpr inline T HalfPi()
{
	return static_cast<T>(kHalfPiD);
}

/* ---------------------------- QuaterPi */
constexpr float  kQuarterPiF = 0.785398163397448309615660845819875721f;
constexpr double kQuarterPiD = 0.785398163397448309615660845819875721;
template<typename T>
constexpr inline T QuarterPi()
{
	return static_cast<T>(kQuarterPiD);
}
/* ---------------------------- 2.0 * Pi */
constexpr float  kTwoPiF     = static_cast<float>(2.0 * kPiD);
constexpr double kTwoPiD     = 2.0 * kPiD;
template<typename T>
constexpr inline T TwoPi()
{
	return static_cast<T>(kTwoPiD);
}

/* ---------------------------- 1.0 / Pi */
constexpr float  kInvPiF = static_cast<float>(1.0 / kPiD);
constexpr double kInvPiD = 1.0 / kPiD;
template<typename T> constexpr inline T InvPi()
{
	return static_cast<T>(kInvPiD);
}

/* ---------------------------- 1.0 / (2.0 * Pi) */
constexpr float  kInvTwoPiF = static_cast<float>(0.5 / kPiD);
constexpr double kInvTwoPiD = 0.5 / kPiD;
template<typename T> constexpr inline T InvTwoPi()
{
	return static_cast<T>(kInvTwoPiD);
}

NAMESPACE_BASE_END
/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
struct base
{
	static bool gl;
	static bool abc;
	static bool usd;
	
	static bool Texture;
	
	static bool RenderItem;
	static bool BBoxItem;
	static bool GeomItem;
	
	static bool InstanceItem;

	static bool RenderTask;
	static bool BBoxTask;
	static bool IndexedGeomTask;
	static bool ExpandedGeomTask;
	
	static bool InstancingTask;
	static bool TaskMananger;
};
NAMESPACE_BASE_END



/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
/* ---------------------------- cppType*/
namespace cppType
{
	enum : int
	{
		kInvalid,
		kBool,
		
		kChar,
		kUnsignedChar,
		
		kShort,
		kUnsignedShort,
		
		kInt,
		kUnsignedInt,
		
		kLongInt,
		kUnsignedLongInt,
		
		kFloat,
		kDouble,
		kString,
		
		kVec2i,
		kVec3i,
		kVec4i,
		
		kVec2f,
		kVec3f,
		kVec4f,
		kVec2d,
		kVec3d,
		kVec4d,
		
		kPoint2f,
		kPoint3f,
		kNormal2f,
		kNormal3f,
		kTexCoord2f,
		kTexCoord3f,
		
		kPoint2d,
		kPoint3d,
		kNormal2d,
		kNormal3d,
		kTexCoord2d,
		kTexCoord3d,
		
		kRGBColor,
		kRGBAColor,

		kQuatf,
		kQuatd,

		kMat2x2f,
		kMat3x3f,
		kMat4x4f,
		kMat2x2d,
		kMat3x3d,
		kMat4x4d,

		kBoolArray,
		
		kCharArray,
		kShortArray,
		kInt32Array,
		kInt64Array,

		kUCharArray,
		kUShortArray,
		kUInt32Array,
		kUInt64Array,

		kFloatArray,
		kDoubleArray,

		kStringArray,

		kVec2iArray,
		kVec3iArray,
		kVec4iArray,
		
		kVec2fArray,
		kVec3fArray,
		kVec4fArray,
		
		kVec2dArray,
		kVec3dArray,
		kVec4dArray,
		
		kPoint2fArray,
		kPoint3fArray,
		kNormal2fArray,
		kNormal3fArray,
		kTexCoord2fArray,
		kTexCoord3fArray,
		
		kPoint2dArray,
		kPoint3dArray,
		kNormal2dArray,
		kNormal3dArray,
		kTexCoord2dArray,
		kTexCoord3dArray,
		
		kRGBColorArray,
		kRGBAColorArray,

		kQuatfArray,
		kQuatdArray,

		kMat2x2fArray,
		kMat2x2dArray,

		kMat3x3fArray,
		kMat3x3dArray,

		kMat4x4fArray,
		kMat4x4dArray,
	};
}

/* ---------------------------- mode */
namespace mode
{
namespace color
{
enum : short
{
	solid,
	foreach,
	random,
};
inline const std::string ToString(const short e)
{
	switch (e)
	{
		case solid:
			return ENUM_TO_STRING(solid);
		case foreach:
			return ENUM_TO_STRING(foreach);
		case random:
			return ENUM_TO_STRING(random);
	}
}
}
namespace animCycle
{
enum : short
{
	none,
	looped,
	osciilated,
};
inline const std::string ToString(const short e)
{
	switch (e)
	{
		case none:
			return ENUM_TO_STRING(none);
		case looped:
			return ENUM_TO_STRING(looped);
		case osciilated:
			return ENUM_TO_STRING(osciilated);
	}
}
}
namespace display
{
enum : short
{
	none,
	points,
	boundingbox,
	proxy,
	foreach,
};
inline const std::string ToString(const short e)
{
	switch (e)
	{
		case none:
			return ENUM_TO_STRING(none);
		case points:
			return ENUM_TO_STRING(points);
		case boundingbox:
			return ENUM_TO_STRING(boundingbox);
		case foreach:
			return ENUM_TO_STRING(foreach);
	}
}
}
namespace style
{
enum : short
{
	shaded,
	wireframe,
};
inline const std::string ToString(const short e)
{
	switch (e)
	{
		case shaded:
			return ENUM_TO_STRING(shaded);
		case wireframe:
			return ENUM_TO_STRING(wireframe);
	}
}
}
}

/* ---------------------------- level of detail */
namespace LOD
{
	enum : int
	{
		level0   = 0,
		level1   = 1,
		level2   = 2,
		maxLevel = 3,
	};
}
/* ---------------------------- attribute */
namespace sementic
{
	namespace uniform
	{
	/* ------------------------------ key */
	enum : int
	{
		gid,
	};
	}
	namespace attribute
	{
	/* ------------------------------ key */
	enum : int
	{
		pid,
		lid,
		sid,
		
		polygonIDs,
		triangleIDs,
		
		renderable,
		culled,
		visible,
		selected,
		
		position,
		eulerAngle,
		scale,
		
		instancingMtx,
		transformMtx,
		localAxis,
		inversedLocalAxis,
		//localAxisX,
		//localAxisY,
		//localAxisZ,
		
		orientation,
		orientationMtx,
		
		color,
		wirecolor,
		transparency,
		
		frameoffset,
		timeoffset,

		uCoord,
		vCoord,
		uvCoord,
		baryCentricCoord,
		
		radius,
		
	};
	}
}
NAMESPACE_BASE_END

