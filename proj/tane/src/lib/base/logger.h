#pragma once
#include "base/pch.h"

#include <boost/noncopyable.hpp>
#include <boost/format.hpp>

/* DECL LOGGER */
#define DECL_LOGGER0()                 			Logger __logger__(__PRETTY_FILENAME__, __FUNCTION_NAME__);
#define DECL_LOGGER1(_FLAG)               		Logger __logger__(__PRETTY_FILENAME__, __FUNCTION_NAME__, _FLAG);
#define DECL_LOGGER2(_CALLER, _FLAG)           	Logger __logger__(_CALLER, __FUNCTION_NAME__, _FLAG);
#define DECL_LOGGER3(_CALLER, _WORK, _FLAG)     Logger __logger__(_CALLER, _WORK, _FLAG);
#define GET_MACRO_DECL_LOGGER(_0, _1, _2, _3, NAME, ...) NAME

/* LOGGER IN */
#define LOGGER_IN0(MSG)                       __logger__.in(MSG);
#define LOGGER_IN1(MSG, _1)                   __logger__.in(MSG, _1);
#define LOGGER_IN2(MSG, _1, _2)               __logger__.in(MSG, _1, _2);
#define LOGGER_IN3(MSG, _1, _2, _3)           __logger__.in(MSG, _1, _2, _3);
#define LOGGER_IN4(MSG, _1, _2, _3, _4)       __logger__.in(MSG, _1, _2, _3, _4);
#define GET_MACRO_LOGGER_IN(_0, _1, _2, _3, _4, NAME, ...) NAME


/* LOGGER DEBUG */
#define LOGGER_DEBUG0(MSG)                       __logger__.debug(MSG);
#define LOGGER_DEBUG1(MSG, _1)                   __logger__.debug(MSG, _1);
#define LOGGER_DEBUG2(MSG, _1, _2)               __logger__.debug(MSG, _1, _2);
#define LOGGER_DEBUG3(MSG, _1, _2, _3)           __logger__.debug(MSG, _1, _2, _3);
#define LOGGER_DEBUG4(MSG, _1, _2, _3, _4)       __logger__.debug(MSG, _1, _2, _3, _4);
#define GET_MACRO_LOGGER_DEBUG(_0, _1, _2, _3, _4, NAME, ...) NAME

/* LOGGER ERROR */
#define LOGGER_ERROR0(MSG)                       __logger__.error(MSG);
#define LOGGER_ERROR1(MSG, _1)                   __logger__.error(MSG, _1);
#define LOGGER_ERROR2(MSG, _1, _2)               __logger__.error(MSG, _1, _2);
#define LOGGER_ERROR3(MSG, _1, _2, _3)           __logger__.error(MSG, _1, _2, _3);
#define LOGGER_ERROR4(MSG, _1, _2, _3, _4)       __logger__.error(MSG, _1, _2, _3, _4);
#define GET_MACRO_LOGGER_ERROR(_0, _1, _2, _3, _4, NAME, ...) NAME

/* LOGGER WARNING */
#define LOGGER_WARNING0(MSG)                     __logger__.warning(MSG);
#define LOGGER_WARNING1(MSG, _1)                 __logger__.warning(MSG, _1);
#define LOGGER_WARNING2(MSG, _1, _2)             __logger__.warning(MSG, _1, _2);
#define LOGGER_WARNING3(MSG, _1, _2, _3)         __logger__.warning(MSG, _1, _2, _3);
#define LOGGER_WARNING4(MSG, _1, _2, _3, _4)     __logger__.warning(MSG, _1, _2, _3, _4);
#define GET_MACRO_LOGGER_WARNING(_0, _1, _2, _3, _4, NAME, ...) NAME

/* LOGGER INFO */
#define LOGGER_INFO0(MSG)                        __logger__.info(MSG);
#define LOGGER_INFO1(MSG, _1)                    __logger__.info(MSG, _1);
#define LOGGER_INFO2(MSG, _1, _2)                __logger__.info(MSG, _1, _2);
#define LOGGER_INFO3(MSG, _1, _2, _3)            __logger__.info(MSG, _1, _2, _3);
#define LOGGER_INFO4(MSG, _1, _2, _3, _4)        __logger__.info(MSG, _1, _2, _3, _4);
#define GET_MACRO_LOGGER_INFO(_0, _1, _2, _3, _4, NAME, ...) NAME

#ifdef ENABLE_LOGGER
#define DECL_LOGGER(...)    GET_MACRO_DECL_LOGGER   (_0,          ##__VA_ARGS__  , DECL_LOGGER3   , DECL_LOGGER2   , DECL_LOGGER1   , DECL_LOGGER0) (__VA_ARGS__);
#define LOGGER_IN(...)      GET_MACRO_LOGGER_IN     (__VA_ARGS__, LOGGER_IN4     , LOGGER_IN3     , LOGGER_IN2     , LOGGER_IN1     , LOGGER_IN0) (__VA_ARGS__)
#define LOGGER_DEBUG(...)   GET_MACRO_LOGGER_DEBUG  (__VA_ARGS__, LOGGER_DEBUG4  , LOGGER_DEBUG3  , LOGGER_DEBUG2  , LOGGER_DEBUG1  , LOGGER_DEBUG0) (__VA_ARGS__)
#define LOGGER_ERROR(...)   GET_MACRO_LOGGER_ERROR  (__VA_ARGS__, LOGGER_ERROR4  , LOGGER_ERROR3  , LOGGER_ERROR2  , LOGGER_ERROR1  , LOGGER_ERROR0) (__VA_ARGS__)
#define LOGGER_WARNING(...) GET_MACRO_LOGGER_WARNING(__VA_ARGS__, LOGGER_WARNING4, LOGGER_WARNING3, LOGGER_WARNING2, LOGGER_WARNING1, LOGGER_WARNING0) (__VA_ARGS__)
#define LOGGER_INFO(...)    GET_MACRO_LOGGER_INFO   (__VA_ARGS__, LOGGER_INFO4   , LOGGER_INFO3   , LOGGER_INFO2   , LOGGER_INFO1   , LOGGER_INFO0) (__VA_ARGS__)
#define LOGGER_OUT()		__logger__.out();
#else
#define DECL_LOGGER(...)
	#define LOGGER_IN(MSG)
	#define LOGGER_DEBUG(...)
	#define LOGGER_INFO(...)
	#define LOGGER_WARNING(...)
	#define LOGGER_ERROR(...)
	#define LOGGER_OUT()
#endif

NAMESPACE_BASE_BEGIN
class Logger final : boost::noncopyable
{
public:
	/*------------------------------------------------------ enum*/
	enum Color
	{
		kWHITE,
		kRED,
		kGREEN,
		kBLUE,
		kMAGENTA,
		kYELLOW,
		kCYAN,
	};
private:
	static const size_t& GetBufferSize_()
	{
		static size_t bufferSize = 1024 * 10;
		return bufferSize;
	}
	static const size_t& GetTabSize_()
	{
		static size_t tabSize = 2;
		return tabSize;
	}
	static std::stack<std::string,  std::vector<std::string>>& GetWorkStack_()
	{
		static std::stack<std::string,  std::vector<std::string>>   workStack;
		return workStack;
	}
	static std::stack<std::clock_t, std::vector<std::clock_t>>& GetTimeStack_()
	{
		static std::stack<std::clock_t, std::vector<std::clock_t>>  timeStack;
		return timeStack;
	}
	
private:
	/*------------------------------------------------------ private variables */
	bool		on_;
	std::string caller_;
	std::string prevWork_;

public:
	/*------------------------------------------------------ defaults functions*/
	Logger(const std::string& caller, const std::string& work, const bool on = false)
		: caller_(caller), on_(on)
	{
		in(work.c_str());
	}
	~Logger()
	{
		out();
	}
public:
	/*------------------------------------------------------ public functions */
	template <typename... Args> void in(const char *fmt, Args... args)
	{
		
		if(!on_)
			return;
		
		std::clock_t inTime = std::clock();
		
		char work[GetBufferSize_()];
		sprintf(work, fmt, args...);
		
		printTab_(GetTabSize_() * GetWorkStack_().size());
		
		GetWorkStack_().push(work);
		GetTimeStack_().push(inTime);
		
		prevWork_ = work;
		
		printf_("[+] %s - %s\n", caller_.c_str(), work);
		
	}
	template <typename... Args> void debug(const char *fmt, Args... args)
	{
		if(!on_)
			return;
		print(fmt, args...);
	}
	template <typename... Args> void error(const char *fmt, Args... args)
	{
		if(!on_)
			return;
		setTextColor_(kRED);
		print(fmt, args...);
		setTextColor_(kWHITE);
	}
	template <typename... Args> void warning(const char *fmt, Args... args)
	{
		if(!on_)
			return;
		
		setTextColor_(kYELLOW);
		print(fmt, args...);
		setTextColor_(kWHITE);
	}
	template <typename... Args> void info(const char *fmt, Args... args)
	{
		if(!on_)
			return;
		
		setTextColor_(kGREEN);
		print(fmt, args...);
		setTextColor_(kWHITE);
	}
	void out()
	{
		if(!on_)
			return;
		
		if(GetWorkStack_().size() < 1 || GetTimeStack_().size() < 1)
		{
			printf_("stack is empty\n");
			return;
		}
		std::string work = GetWorkStack_().top();
		GetWorkStack_().pop();
		std::clock_t inTime = GetTimeStack_().top();
		GetTimeStack_().pop();
		
		std::clock_t outTime = clock();
		
		long double elapsed = (long double) (outTime - inTime) / (long double) (CLOCKS_PER_SEC);

		if(elapsed > 0.01)
		{
			setTextColor_(kYELLOW);
		}
		if(elapsed > 0.1)
		{
			setTextColor_(kRED);
		}
		printTab_(GetTabSize_() * GetWorkStack_().size());
		if(prevWork_ == work)
		{
			printf_("[-] ");
			for (int i = 0; i < 65; ++i)
			{
				printf_("-");
			}
		}
		else
		{
			printf_("[-] ");
			
			for (int i = 0; i < 65; ++i)
			{
				printf_("-");
				
			}
		}
		printf_(" %10.4Lfs.\n", elapsed);
		setTextColor_(kWHITE);
	}

private:
	/*------------------------------------------------------ private functions */
	void setTextColor_(const Color& color)
	{
		switch (color)
		{
			case kWHITE:
				printf_("\033[0m");
				break;
			case kRED:
				printf_("\033[1;31m");
				break;
			case kGREEN:
				printf_("\033[1;32m");
				break;
			case kYELLOW:
				printf_("\033[1;33m");
				break;
			case kBLUE:
				printf_("\033[1;34m");
				break;
			case kMAGENTA:
				printf_("\033[1;35m");
				break;
			case kCYAN:
				printf_("\033[1;36m");
				break;
		}
	}
	template <typename... Args> void print(const char *fmt, Args... args)
	{
		char msg[GetBufferSize_()];
		sprintf(msg, fmt, args...);
		printTab_(GetTabSize_() * GetWorkStack_().size());
		printf_("- %s\n", msg);
	}
	
	void printTab_(size_t tab_size)
	{
		for (size_t i = 0; i < tab_size; i++)
		{
			printf_(" ");
		}
	}
	template <typename... Args> void printf_(const char *fmt, Args... args)
	{
		//static const int32_t bufferSize = 1024;
		char buffer[GetBufferSize_()];
		sprintf(buffer, fmt, args...);
		printf("%s", buffer);
	}
	
	
};


NAMESPACE_BASE_END
