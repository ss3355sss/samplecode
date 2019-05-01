#pragma once
#include "base/pch.h"

NAMESPACE_BASE_BEGIN
/*deprecated*/
class Time final
{
public:
	enum Sampling{
		kFLOOR,
		kCEIL,
		kROUND,
	};

public:
	double fps;
	double sec;

public:
	static int 		GetSampleIndex(const Time& inTimeCode, const double& minTime, const double& maxTime, const int& nsamples, const Sampling& sampling = kROUND);
	static double 	ClampTime(const Time& inTimeCode, const double& minTime, const double& maxTime);
	static double 	LoopedTime(const Time& inTimeCode, const double& minTime, const double& maxTime);
	static double 	OsciilateTime(const Time& inTimeCode, const double& minTime, const double& maxTime);

public:
	Time(const double s = 0.0, const double f = 24.0) : sec(s), fps(f)
	{
	}
	~Time()
	{
		sec = 0.0;
		fps = 24.0;
	}
public:


};

class Frame final
{
public:
	enum Sampling{
		kFLOOR,
		kCEIL,
		kROUND,
	};

public:
	double value_;
	
public:
	//static int 		GetSampleIndex(const Time& inTimeCode, const double& minTime, const double& maxTime, const int& nsamples, const Sampling& sampling = kROUND);
	static Frame Clamp    (const Frame& inTimeCode, const Frame& minFrame, const Frame& maxFrame);
	static Frame Looped   (const Frame& inTimeCode, const Frame& minFrame, const Frame& maxFrame);
	static Frame Osciilate(const Frame& inTimeCode, const Frame& minFrame, const Frame& maxFrame);

public:
	static Frame EarliestFrame()
	{
		return Frame(std::numeric_limits<double>::lowest());
	}
	
public:
	Frame(const double value = 0.0) : value_(value)
	{
	}
	~Frame()
	{
		value_ = 0.0;
	}
public:
	//operator ++;
	//operator --;
	//operator +=;
	//operator -=;
	//operator ==;
	//operator !=;
	//operator <;
	//operator <=;
	//operator >=;
	//operator >;
public:
	const double& value() const
	{
		return value_;
	}
	double& value()
	{
		return value_;
	}
public:
	const double timeInSecond(const double& fps) const
	{
		return (value_ - 1.0) / fps;
	}
	
};


NAMESPACE_BASE_END