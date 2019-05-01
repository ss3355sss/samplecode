#pragma once
#include "baseGl/pch.h"
///----------------------------------------------------------------------------------------------- depreacated version
NAMESPACE_BASE_BEGIN

inline bool GlewInit()
{
	static bool initialized = false;
	
	
	GLenum stat;
	if(!initialized)
	{
		stat = ::glewInit();
		if(stat != GLEW_OK)
		{
			STDOUT_ERROR("Failed to initialize glew, %s", glewGetErrorString(stat));
			return false;
		}
		
		/* set gl options */
		initialized = true;
	}
	return initialized;
}

NAMESPACE_BASE_END






















































