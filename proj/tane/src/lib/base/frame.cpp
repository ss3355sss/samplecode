#include "base/frame.h"
#include <boost/algorithm/clamp.hpp>

NAMESPACE_BASE_BEGIN

int Time::GetSampleIndex(const Time& inTime, const double& minTime, const double& maxTime, const int& nsamples, const Sampling& sampling)
{
	double inFps 	= inTime.fps;
	double inSecond = inTime.sec;
	double targetFrame = (inSecond - minTime) * inFps;

	int sampleIndex = -1;
	switch (sampling)
	{
		case Sampling::kFLOOR:
			sampleIndex = std::floor(targetFrame);
		case Sampling::kCEIL:
			sampleIndex = std::ceil(targetFrame);
		case Sampling::kROUND:
			sampleIndex = std::round(targetFrame);
	}
	sampleIndex = boost::algorithm::clamp(sampleIndex, 0, nsamples - 1);
	return sampleIndex ;
}
double Time::ClampTime(const Time& inTime, const double& minTime, const double& maxTime)
{
	return boost::algorithm::clamp(inTime.sec, minTime, maxTime);
}
double Time::LoopedTime(const Time& inTimeCode, const double& minTime, const double& maxTime)
{
	double inTime = inTimeCode.sec;

	double range  	= maxTime - minTime;
	double ratio 	= (inTime - minTime) / range;
	double state 	= range * (ratio - std::floor(ratio));

	if(state > range)
	{
		state = 0;
	}
	double val = (state + minTime);
	return boost::algorithm::clamp(val, minTime, maxTime);

}
double Time::OsciilateTime(const Time& inTimeCode, const double& minTime, const double& maxTime)
{
	double inTime = inTimeCode.sec;

	double range  	= maxTime - minTime;
	double length 	= range * 2.0;
	double ratio 	= (inTime - minTime) / length;
	double state 	= length * (ratio - std::floor(ratio));

	if(state > range)
	{
		state = length - state;
	}
	double val = (state + minTime);
	return boost::algorithm::clamp(val, minTime, maxTime);
}

NAMESPACE_BASE_END