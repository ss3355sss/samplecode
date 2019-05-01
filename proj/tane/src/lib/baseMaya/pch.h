#pragma once
/* ----------------------------------------------- include */
/* ---------------------------- base */
#include "base/pch.h"
#include "base/logger.h"
#include "base/staticData.h"

/* workaround Cuda, Maya */
#define short2  CUDA_short2
#define short3  CUDA_short3
#define long2   CUDA_long2
#define long3   CUDA_long3
#define int2    CUDA_int2
#define int3    CUDA_int3
#define float2  CUDA_float2
#define float3  CUDA_float3
#define double2 CUDA_double2
#define double3 CUDA_double3
#define double4 CUDA_double4

/* ---------------------------- maya api */
#include "inc_maya.h"

/* workaround usd, X */
#undef Always
/* workaround usd, Xlib */
#undef Bool

/* ----------------------------------------------- macro */
#define MCHECKERROR0(_STAT, _MSG)			  	if ( MS::kSuccess != _STAT ) { STDOUT_ERROR(_MSG); return MS::kFailure; };
#define MCHECKERROR1(_STAT, _MSG, _1)			if ( MS::kSuccess != _STAT ) { STDOUT_ERROR(_MSG, _1); return MS::kFailure; };
#define MCHECKERROR2(_STAT, _MSG, _1, _2)		if ( MS::kSuccess != _STAT ) { STDOUT_ERROR(_MSG, _1, _2); return MS::kFailure; };
#define MCHECKERROR3(_STAT, _MSG, _1, _2, _3)	if ( MS::kSuccess != _STAT ) { STDOUT_ERROR(_MSG, _1, _2, _3); return MS::kFailure; };
#define GET_MACRO_MCHECKERROR(_1, _2, _3, _4, _5, NAME, ...) NAME
#define MCHECKERROR(...) \
	GET_MACRO_MCHECKERROR(__VA_ARGS__, \
                          MCHECKERROR3,\
                          MCHECKERROR2,\
                          MCHECKERROR1,\
                          MCHECKERROR0)\
                          (__VA_ARGS__)

#define MCHECKERRORNORET0(_STAT, _MSG)			  	if ( MS::kSuccess != _STAT ) { STDOUT_WARRNING(_MSG); };
#define MCHECKERRORNORET1(_STAT, _MSG, _1)			if ( MS::kSuccess != _STAT ) { STDOUT_WARRNING(_MSG, _1); };
#define MCHECKERRORNORET2(_STAT, _MSG, _1, _2)		if ( MS::kSuccess != _STAT ) { STDOUT_WARRNING(_MSG, _1, _2); };
#define MCHECKERRORNORET3(_STAT, _MSG, _1, _2, _3) 	if ( MS::kSuccess != _STAT ) { STDOUT_WARRNING(_MSG, _1, _2, _3); };
#define GET_MACRO_MCHECKERRORNORET(_1, _2, _3, _4, _5, NAME, ...) NAME
#define MCHECKERRORNORET(...) \
	GET_MACRO_MCHECKERRORNORET(__VA_ARGS__, \
                               MCHECKERRORNORET3,\
                               MCHECKERRORNORET2,\
                               MCHECKERRORNORET1,\
                               MCHECKERRORNORET0)\
                               (__VA_ARGS__)

///---------------------------------------------------------------------- DECL_DG_LOGGER
/* GET CALLER FROM MOBJECT HELPER*/
inline MString __get_absolute_name__(const MObject& obj)
{
	MStatus stat;
	MFnDependencyNode nodeFn(obj, &stat);
	if(stat != MS::kSuccess)
		return "invalid object";
	
	MString absName = nodeFn.absoluteName(&stat);
	if(stat != MS::kSuccess)
		return "invalid object";
	
	return absName;
}

#ifdef ENABLE_LOGGER
#define GET_CALLER_FROM_MOBJECT(OBJECT) std::string __caller_name__= std::string(__get_absolute_name__(OBJECT).asChar());
#else
#define GET_CALLER_FROM_MOBJECT(OBJECT)
#endif

#define DECL_DG_LOGGER0()                 			GET_CALLER_FROM_MOBJECT(thisMObject());   	DECL_LOGGER3(__caller_name__, __FUNCTION_NAME__, false);
#define DECL_DG_LOGGER1(_FLAG)               		GET_CALLER_FROM_MOBJECT(thisMObject());   	DECL_LOGGER3(__caller_name__, __FUNCTION_NAME__, _FLAG);
#define DECL_DG_LOGGER2(_CALLER, _FLAG)           												DECL_LOGGER3(_CALLER,         __FUNCTION_NAME__, _FLAG);
#define DECL_DG_LOGGER3(_CALLER, _WORK, _FLAG)      											DECL_LOGGER3(_CALLER, _WORK, _FLAG);

#define GET_MACRO_DECL_DG_LOGGER(_0, _1, _2, _3, NAME, ...) NAME

#ifdef ENABLE_LOGGER
#define DECL_DG_LOGGER(...) GET_MACRO_DECL_DG_LOGGER(_0, ##__VA_ARGS__, \
                                DECL_DG_LOGGER3,\
                                DECL_DG_LOGGER2,\
                                DECL_DG_LOGGER1,\
                                DECL_DG_LOGGER0)\
                                (__VA_ARGS__);
#else
#define DECL_DG_LOGGER(...)
#endif

#define ADD_ATTRIBUTE(ATTR)            MStatus ATTR##_stat;    ATTR##_stat    = addAttribute( ATTR );        MCHECKERROR(ATTR##_stat   , "addAttribute: " #ATTR );
#define ATTRIBUTE_AFFECTS(IN, OUT)     MStatus IN##OUT##_stat; IN##OUT##_stat = attributeAffects( IN, OUT ); MCHECKERROR(IN##OUT##_stat, "attributeAffects:" #IN "->" #OUT);

///---------------------------------------------------------------------- DECL_DG_LOGGER
#define SET_ATTRIBUTE_STATUS(FN_ATTR, READABLE, WRITABLE, CONNECTABLE, STORABLE, CACHED, CHANNELBOX, ARRAY, USES_ARRAYDATA_BUILDER, KEYABLE, HIDDEN, DISCONNECT_BEHAVIOR, AFFECTS_APPEARANCE)\
	FN_ATTR.setReadable(READABLE);\
	FN_ATTR.setWritable(WRITABLE);\
	FN_ATTR.setConnectable(CONNECTABLE);\
	FN_ATTR.setStorable(STORABLE);\
	FN_ATTR.setCached(CACHED);\
	FN_ATTR.setChannelBox(CHANNELBOX);\
	FN_ATTR.setArray(ARRAY);\
	FN_ATTR.setUsesArrayDataBuilder(USES_ARRAYDATA_BUILDER);\
	FN_ATTR.setKeyable(KEYABLE);\
	FN_ATTR.setHidden(HIDDEN);\
	FN_ATTR.setDisconnectBehavior(DISCONNECT_BEHAVIOR);\
	FN_ATTR.setAffectsAppearance(AFFECTS_APPEARANCE);
	
///----------------------------------------------------------------------Post Constructor
#define DEFAULT_DAG_POST_CONSTRUCTOR(NODE_PRETTY_NAME)\
	MPxNode::postConstructor();\
	MFnDependencyNode nodeFn;\
	nodeFn.setObject(thisMObject());\
	nodeFn.setName(NODE_PRETTY_NAME +"Shape#");

#define DEFAULT_DG_POST_CONSTRUCTOR(NODE_PRETTY_NAME)\
	MPxNode::postConstructor();\
	MFnDependencyNode nodeFn;\
	nodeFn.setObject(thisMObject());\
	nodeFn.setName(NODE_PRETTY_NAME + "#");



/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
namespace logging
{
	struct _baseMaya
	{
	};
	extern StaticData<_baseMaya> baseMaya;
}

NAMESPACE_BASE_END

/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
enum class MayaValueType : int
{
	kInvalid    ,
	kString     ,
	kMatrix     ,
	kStringArray,
	kDoubleArray,
	kFloatArray ,
	kIntArray   ,
	kPointArray ,
	kVectorArray,
	kBoolean    ,
	kByte       ,
	kChar       ,
	kShort      ,
	kInt        ,
	k2Short     ,
	k2Int       ,
	k3Short     ,
	k3Int       ,
	kFloat      ,
	k2Float     ,
	k3Float     ,
	kDouble     ,
	k2Double    ,
	k3Double    ,
	k4Double    ,
	kAngle      ,
	kDistance   ,
};
namespace sementic
{
}
NAMESPACE_BASE_END
