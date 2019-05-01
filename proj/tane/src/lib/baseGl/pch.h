#pragma once
/* ----------------------------------------------- include */
/* ---------------------------- base */
#include "base/pch.h"

/*----------------------------- opengl */
#include <GL/glew.h>

/* ----------------------------------------------- macro */
NAMESPACE_BASE_BEGIN
template <typename... Args>
inline bool _GlStateValid(const char *fmt, Args... args)
{
	static const size_t bufferSize = 1024 * 10;
	
	const GLubyte* errString;
	GLenum err = glGetError();
	if((err == GL_NO_ERROR))
	{
		return true;
	}
	else
	{
		char msg[bufferSize];
		sprintf(msg, fmt, args...);
		STDOUT_ERROR("%s - %s", gluErrorString(err), msg);
		return false;
	}
	
}

#define GLCHECKERROR0(MSG)              { if(_GlStateValid(MSG)             != true) { return false; }}
#define GLCHECKERROR1(MSG, _1)          { if(_GlStateValid(MSG, _1)         != true) { return false; }}
#define GLCHECKERROR2(MSG, _1, _2)      { if(_GlStateValid(MSG, _1, _2)     != true) { return false; }}
#define GLCHECKERROR3(MSG, _1, _2, _3)  { if(_GlStateValid(MSG, _1, _2, _3) != true) { return false; }}
#define GET_MACRO_GLCHECKERROR(_1, _2, _3, _4, NAME, ...) NAME

#define GLCHECKERRORNORET0(MSG)              { _GlStateValid(MSG)            ;}
#define GLCHECKERRORNORET1(MSG, _1)          { _GlStateValid(MSG, _1)        ;}
#define GLCHECKERRORNORET2(MSG, _1, _2)      { _GlStateValid(MSG, _1, _2)    ;}
#define GLCHECKERRORNORET3(MSG, _1, _2, _3)  { _GlStateValid(MSG, _1, _2, _3);}
#define GET_MACRO_GLCHECKERRORNORET(_1, _2, _3, _4, NAME, ...) NAME

#ifdef DEBUG
#define GLCHECKERROR(...) \
		GET_MACRO_GLCHECKERROR(__VA_ARGS__, \
	                           GLCHECKERROR3,\
	                           GLCHECKERROR2,\
	                           GLCHECKERROR1,\
	                           GLCHECKERROR0)\
	                           (__VA_ARGS__)
#define GLCHECKERRORNORET(...) \
		GET_MACRO_GLCHECKERRORNORET(__VA_ARGS__, \
	                           GLCHECKERRORNORET3,\
	                           GLCHECKERRORNORET2,\
	                           GLCHECKERRORNORET1,\
	                           GLCHECKERRORNORET0)\
	                           (__VA_ARGS__)
#else
#define GLCHECKERROR(...)
	#define GLCHECKERRORNORET(...)
#endif

NAMESPACE_BASE_END
/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
namespace logging
{
	struct _basegl
	{
	};
}
NAMESPACE_BASE_END



/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
/* ---------------------------- attribute */
namespace sementic
{

}
NAMESPACE_BASE_END

