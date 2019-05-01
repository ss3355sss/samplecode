#pragma once

/* ----------------------------------------------- include */
/* ---------------------------- base */
#include "base/pch.h"
#include "base/logger.h"

/* ---------------------------- baseGl */
#include "baseGl/pch.h"

/* ---------------------------- baseMaya */
#include "baseMaya/pch.h"

/* ---------------------------- tane */
#include "tane/pch.h"

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

/* ---------------------------- zelos base */
//#include "inc_zelos.h"

/* ---------------------------- maya api */
#include "inc_maya.h"

/* workaround usd, X */
#undef Always
/* workaround usd, Xlib */
#undef Bool

///----------------------------------------------------------------------Common Internal Attributes Define
///----------------------------------------------------BoundingBox
#define DECL_BOUNDINGBOX_ATTRIBUTE\
	static MObject minCornerObj;		\
	static MObject maxCornerObj;
#define IMPL_BOUNDINGBOX_ATTRIBUTE(NODE)\
	MObject NODE::minCornerObj;         \
    MObject NODE::maxCornerObj;
#define INITIALIZE_BOUNDINGBOX_ATTRIBUTE\
	minCornerObj = nAttr.create("minCorner", "minCorner", MFnNumericData::k3Double, 0, &stat);\
	MCHECKERROR(stat, "Failed to create minCorner attribute");\
	SET_ATTRIBUTE_STATUS(nAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(minCornerObj);\
	\
	maxCornerObj = nAttr.create("maxCorner", "maxCorner", MFnNumericData::k3Double, 0, &stat);\
	MCHECKERROR(stat, "Failed to create maxCorner attribute error");\
	SET_ATTRIBUTE_STATUS(nAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(maxCornerObj);

///----------------------------------------------------Miscellaneous
#define DECL_MISCELLANEOUS_ATTRIBUTE	\
    static MObject recomputeObj;
#define IMPL_MISCELLANEOUS_ATTRIBUTE(NODE) \
    MObject NODE::recomputeObj;
#define INITIALIZE_MISCELLANEOUS_ATTRIBUTE(FLAG) \
	recomputeObj = nAttr.create("recompute", "recompute", MFnNumericData::kBoolean, true, &stat);\
	MCHECKERROR(stat, "Failed to create recompute attribute");\
	SET_ATTRIBUTE_STATUS(nAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(recomputeObj);

///----------------------------------------------------------------------Common Attributes Define
///----------------------------------------------------------------------Common Input Attributes Define
///----------------------------------------------------InBody
#define DECL_IN_BODY_ATTRIBUTE\
	static MObject inBodyObj;
#define IMPL_IN_BODY_ATTRIBUTE(NODE)\
	MObject NODE::inBodyObj;
#define INITIALIZE_IN_BODY_ATTRIBUTE\
	inBodyObj = tAttr.create("inBody", "inBody", MFnData::kAny, MObject::kNullObj, &stat);\
	MCHECKERROR(stat, "Failed to create inBody attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,     /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(inBodyObj);

///----------------------------------------------------InSource
#define DECL_IN_SOURCE_ATTRIBUTE\
	static MObject inSourceObj;
#define IMPL_IN_SOURCE_ATTRIBUTE(NODE)\
	MObject NODE::inSourceObj;
#define INITIALIZE_IN_SOURCE_ATTRIBUTE\
	MFnPluginData pluginDataFn_IN_SOURCE;\
	inSourceObj = tAttr.create("inSource", "inSource", SourceMPxData::id, pluginDataFn_IN_SOURCE.create(MTypeId(SourceMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create inSource attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 true ,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kDelete,     /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	tAttr.setIndexMatters(false);\
	ADD_ATTRIBUTE(inSourceObj);

///----------------------------------------------------InSources
#define DECL_IN_SOURCES_ATTRIBUTE\
	static MObject inSourcesObj;
#define IMPL_IN_SOURCES_ATTRIBUTE(NODE)\
	MObject NODE::inSourcesObj;
#define INITIALIZE_IN_SOURCES_ATTRIBUTE\
	MFnPluginData pluginDataFn_IN_SOURCES;\
	inSourcesObj = tAttr.create("inSources", "inSources", SourcesMPxData::id, pluginDataFn_IN_SOURCES.create(MTypeId(SourcesMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create inSources attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(inSourcesObj);

///----------------------------------------------------InPoints
#define DECL_IN_POINTS_ATTRIBUTE\
	static MObject inPointsObj;
#define IMPL_IN_POINTS_ATTRIBUTE(NODE)\
	MObject NODE::inPointsObj;
#define INITIALIZE_IN_POINTS_ATTRIBUTE\
	MFnPluginData pluginDataFn_IN_POINTS;\
	inPointsObj = tAttr.create("inPoints", "inPoints", PointsMPxData::id, pluginDataFn_IN_POINTS.create(MTypeId(PointsMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create inPoints attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(inPointsObj);

///----------------------------------------------------InMessage
#define DECL_IN_MESSAGE_ATTRIBUTE\
	static MObject inMessageObj;
#define IMPL_IN_MESSAGE_ATTRIBUTE(NODE)\
	MObject NODE::inMessageObj;
#define INITIALIZE_IN_MESSAGE_ATTRIBUTE\
	MFnPluginData pluginDataFn_IN_MESSAGE;\
	inMessageObj = tAttr.create("inMessage", "inMessage", MessageMPxData::id, pluginDataFn_IN_MESSAGE.create(MTypeId(MessageMPxData::id), &stat), &stat);\
	MCHECKERROR(stat, "Failed to create inMessage attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(inMessageObj);

///----------------------------------------------------Input
#define DECL_INPUT_ATTRIBUTE\
	DECL_IN_BODY_ATTRIBUTE;\
	DECL_IN_SOURCES_ATTRIBUTE;\
	DECL_IN_POINTS_ATTRIBUTE;\
	DECL_IN_MESSAGE_ATTRIBUTE;\
	static MObject inputObj;
#define IMPL_INPUT_ATTRIBUTE(NODE)\
	IMPL_IN_BODY_ATTRIBUTE(NODE);\
	IMPL_IN_POINTS_ATTRIBUTE(NODE);\
	IMPL_IN_SOURCES_ATTRIBUTE(NODE);\
	IMPL_IN_MESSAGE_ATTRIBUTE(NODE);\
	MObject NODE::inputObj;
#define INITIALIZE_INPUT_ATTRIBUTE\
	inputObj = cAttr.create("input", "input", &stat);\
	MCHECKERROR(stat, "Failed to create input attribute");\
		INITIALIZE_IN_BODY_ATTRIBUTE;\
		INITIALIZE_IN_SOURCES_ATTRIBUTE;\
		INITIALIZE_IN_POINTS_ATTRIBUTE;\
		INITIALIZE_IN_MESSAGE_ATTRIBUTE;\
	cAttr.addChild(inBodyObj);\
	cAttr.addChild(inSourcesObj);\
	cAttr.addChild(inPointsObj);\
	cAttr.addChild(inMessageObj);\
	SET_ATTRIBUTE_STATUS(cAttr,\
						 false,                     /* Readable              */\
						 true ,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 true ,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 true ,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 true ,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(inputObj);
///----------------------------------------------------------------------Common Output Attributes Define

///----------------------------------------------------outBody
#define DECL_OUT_BODY_ATTRIBUTE\
	static MObject outBodyObj;
#define IMPL_OUT_BODY_ATTRIBUTE(NODE)\
	MObject NODE::outBodyObj;
#define INITIALIZE_OUT_BODY_ATTRIBUTE\
	outBodyObj = tAttr.create("outBody", "outBody", MFnData::kMesh, MObject::kNullObj, &stat);\
	MCHECKERROR(stat, "Failed to create outBody attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outBodyObj);


///----------------------------------------------------outSource
#define DECL_OUT_SOURCE_ATTRIBUTE\
	static MObject outSourceObj;
#define IMPL_OUT_SOURCE_ATTRIBUTE(NODE)\
	MObject NODE::outSourceObj;
#define INITIALIZE_OUT_SOURCE_ATTRIBUTE\
	MFnPluginData pluginDataFn_OUT_SOURCE;\
	outSourceObj = tAttr.create("outSource", "outSource", SourceMPxData::id, pluginDataFn_OUT_SOURCE.create(MTypeId(SourceMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create outSource attribute error");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outSourceObj);

///----------------------------------------------------outSources
#define DECL_OUT_SOURCES_ATTRIBUTE\
	static MObject outSourcesObj;
#define IMPL_OUT_SOURCES_ATTRIBUTE(NODE)\
	MObject NODE::outSourcesObj;
#define INITIALIZE_OUT_SOURCES_ATTRIBUTE\
	MFnPluginData pluginDataFn_OUT_SOURCES;\
	outSourcesObj = tAttr.create("outSources", "outSources", SourcesMPxData::id, pluginDataFn_OUT_SOURCES.create(MTypeId(SourcesMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create outSources attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outSourcesObj);

///----------------------------------------------------outPoints
#define DECL_OUT_POINTS_ATTRIBUTE\
	static MObject outPointsObj;
#define IMPL_OUT_POINTS_ATTRIBUTE(NODE)\
	MObject NODE::outPointsObj;
#define INITIALIZE_OUT_POINTS_ATTRIBUTE\
	MFnPluginData pluginDataFn_OUT_POINTS;\
	outPointsObj = tAttr.create("outPoints", "outPoints", PointsMPxData::id, pluginDataFn_OUT_POINTS.create(MTypeId(PointsMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create outPoints attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outPointsObj);

///----------------------------------------------------outMessage
#define DECL_OUT_MESSAGE_ATTRIBUTE\
	static MObject outMessageObj;
#define IMPL_OUT_MESSAGE_ATTRIBUTE(NODE)\
	MObject NODE::outMessageObj;
#define INITIALIZE_OUT_MESSAGE_ATTRIBUTE\
	MFnPluginData pluginDataFn_OUT_MESSAGE;\
	outMessageObj = tAttr.create("outMessage", "outMessage", MessageMPxData::id, pluginDataFn_OUT_MESSAGE.create(MTypeId(MessageMPxData::id)), &stat);\
	MCHECKERROR(stat, "Failed to create outMessage attribute");\
	SET_ATTRIBUTE_STATUS(tAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outMessageObj);

///----------------------------------------------------output
#define DECL_OUTPUT_ATTRIBUTE\
	DECL_OUT_BODY_ATTRIBUTE;\
	DECL_OUT_SOURCES_ATTRIBUTE;\
	DECL_OUT_POINTS_ATTRIBUTE;\
	DECL_OUT_MESSAGE_ATTRIBUTE;\
	static MObject outputObj;

#define IMPL_OUTPUT_ATTRIBUTE(NODE)\
	IMPL_OUT_BODY_ATTRIBUTE(NODE);\
	IMPL_OUT_SOURCES_ATTRIBUTE(NODE);\
	IMPL_OUT_POINTS_ATTRIBUTE(NODE)\
	IMPL_OUT_MESSAGE_ATTRIBUTE(NODE);\
	MObject NODE::outputObj;

#define INITIALIZE_OUTPUT_ATTRIBUTE\
	outputObj = cAttr.create("output", "output", &stat);\
	MCHECKERROR(stat, "Failed to create output attribute");\
		INITIALIZE_OUT_BODY_ATTRIBUTE;\
		INITIALIZE_OUT_POINTS_ATTRIBUTE;\
		INITIALIZE_OUT_SOURCES_ATTRIBUTE;\
		INITIALIZE_OUT_MESSAGE_ATTRIBUTE;\
	cAttr.addChild(outBodyObj);\
	cAttr.addChild(outSourcesObj);\
	cAttr.addChild(outPointsObj);\
	cAttr.addChild(outMessageObj);\
	SET_ATTRIBUTE_STATUS(cAttr,\
						 true ,                     /* Readable              */\
						 false,                     /* Writable              */\
						 true ,                     /* Connectable           */\
						 false,                     /* Stroable              */\
						 true ,                     /* Cached                */\
						 false,                     /* ChannelBox            */\
						 false,                     /* Array                 */\
						 false,                     /* Use Array DataBuilder */\
						 false,                     /* Keyable               */\
						 false,                     /* Hidden                */\
						 MFnAttribute::kNothing,    /* Disconnect Behavior   */\
						 false                      /* Affect Appearance     */\
						 );\
	ADD_ATTRIBUTE(outputObj);

	
	
///*------------------------------------------------------------------------ Define NodeId, NodeName */
///----------------------------------------------- MPxData


///----------------------------------------------- MPxManipContainer
#define TN_TANE_MPXSURFACESHAPE_MANIP_ID                        0x44000000
//#define TN_TANE_MPXSURFACESHAPE_MANIP_NAME                    "TN_TaneMPxSurfaceShape"

///----------------------------------------------- MPxMPxContext
#define TN_PLACER_MPXCONTEXT_ID                                 0x45000000
#define TN_PLACER_MPXCONTEXT_NAME                               "TN_PlacerMPxContext"

#define TN_PICKING_MPXTOOL_COMMAND                				"TN_PickingToolCmd"
#define TN_PICKING_MPXCONTEXT_COMMAND                     		"TN_PickingContextCmd"

///----------------------------------------------- MPxCommand
#define TN_CREATE_TANE_MPXSURFACESHAPE                          "TN_CreateTaneMPxSurfaceShape"
#define TN_CREATE_ABCPROXY_MPXSURFACESHAPE                      "TN_CreateAbcProxyMPxSurfaceShape"

#define TN_TANE_MPXSURFACESHAPE_CLEARTWEAKS_CMD                 "TN_TaneMPxSurfaceShapeClearTweaksCmd"
#define TN_TANE_MPXSURFACESHAPE_CLEAR_RENDERABLE_TWEAKS_CMD     "TN_TaneMPxSurfaceShapeClearRenderableTweaksCmd"
#define TN_TANE_MPXSURFACESHAPE_RENDER_CMD     			        "TN_TaneMPxSurfaceShapeRenderCmd"

#define TN_ZENV_TO_TANE_CMD     			        			"TN_ZEnvToTaneCmd"
#define TN_IMPORT_ZENN_FEATHER_CMD                              "TN_ImportZennFeatherCmd"
#define TN_IMPORT_ZFEATHER_DATA_CMD                             "TN_ImportZFeatherDataCmd"
#define TN_EXPORT_ZFEATHER_DATA_CMD                             "TN_ExportZFeatherDataCmd"

#define TN_FRUSTUM_CULLING_CMD     			        			"TN_FrustumCullingCmd"

#define TN_PINPOINTS_CMD     			        				"TN_PinPointsCmd"
#define TN_UNPINPOINTS_CMD     			        				"TN_UnPinPointsCmd"
#define TN_IMPORTCACHE_CMD     			        				"TN_ImportCacheCmd"
#define TN_EXPORTCACHE_CMD     			        				"TN_ExportCacheCmd"

#define TN_IMPORT_ZENVCACHE_CMD     			        		"TN_ImportZEnvCacheCmd"

#define TN_IMPORT_TANEUSD_CMD     			        		    "TN_ImportTaneUsdCmd"
#define TN_EXPORT_TANEUSD_CMD     			        		    "TN_ExportTaneUsdCmd"

#define TN_POINTS_INFO_CMD     			        				"TN_PointsInfoCmd"
#define TN_SOURCES_INFO_CMD     			        			"TN_SourcesInfoCmd"

#define TN_CREATE_NODE_CMD     			        			    "TN_CreateNode"

#define TN_IMPORT_TANE_CMD     			        			    "TN_ImportTaneCmd"
#define TN_EXPORT_TANE_CMD     			        			    "TN_ExportTaneCmd"

#define TN_TANE_INFO_CMD     			        			    "TN_TaneInfo"

NAMESPACE_BASE_BEGIN

/* ----------------------------------------------- register identifier */
struct _registeredRenderItems
{
	typedef MString value_type;
	const value_type DormantPointsRenderItem = "TN_PointsRenderItem";
	const value_type ActivePointsRenderItem  = "TN_ActivePointsRenderItemName";
	const value_type DeletedPointsRenderItem = "TN_deletedPointsRenderItemName";
};
extern StaticData<_registeredRenderItems> RegisteredRenderItems;

struct _registeredNodes
{
	struct _nodeInfo
	{
		const MTypeId id;
		const MString name;
		bool          logging;
	};
	typedef _nodeInfo value_type;
	///----------------------------------------------- MPxData
	value_type PointsMPxData                = { MTypeId(0x40000000), MString("TN_PointsMPxData"),    false};
	value_type SourceMPxData                = { MTypeId(0x40000001), MString("TN_SourceMPxData"),    false};
	value_type SourcesMPxData               = { MTypeId(0x40000003), MString("TN_SourcesMPxData"),   false};
	value_type MessageMPxData               = { MTypeId(0x40000005), MString("TN_MessageMPxData"),   false};
	
	///----------------------------------------------- MPxNode
	value_type EnvironmentMPxNode           = { MTypeId(0x41000000), MString("TN_Environment"),      false};
	value_type BindMPxNode                  = { MTypeId(0x41000001), MString("TN_Bind"),             false};
	value_type FetchAttributeMPxNode        = { MTypeId(0x41000002), MString("TN_FetchAttribute"),   false};
	
	///----------------------------------------------- MPxTransformationMatrix, MPxTransform
	value_type TaneMPxTransformMtx          = { MTypeId(0x42000000), MString("TN_TaneTransformMtx"), false};
	value_type TaneMPxTransform             = { MTypeId(0x42000001), MString("TN_TaneTransform"),    false};
	value_type TaneMPxTransformDrawOverride = { MTypeId(),           MString(""),                    false};
	
	///----------------------------------------------- MPxSurfaceShape
	value_type TaneMPxShape                 = { MTypeId(0x43000001), MString("TN_Tane"),             false};
	value_type TaneMPxShapeGeometryOverride = { MTypeId(),           MString(""),                    false};
	value_type AbcProxyMPxShape             = { MTypeId(0x43000002), MString("TN_AbcProxy"),         false};
	value_type Abc2ProxyMPxShape            = { MTypeId(0x43000003), MString("TN_Abc2Proxy"),        false};
	value_type UsdProxyMPxShape             = { MTypeId(0x43000004), MString("TN_UsdProxy"),         false};
	value_type UsdHdProxyMPxShape           = { MTypeId(0x43000005), MString("TN_UsdHdProxy"),       false};
	value_type VdbProxyMPxShape             = { MTypeId(0x43000006), MString("TN_VdbProxy"),         false};
};
extern StaticData<_registeredNodes> RegisteredNodes;
void RegisteredNodesLogging();


struct _registeredCommand
{
	struct _nodeInfo
	{
		const MString name;
		bool  logging;
	};
	typedef _nodeInfo value_type;
	value_type CreateNodeCmd                  = { "TN_CreateNode",                  false };
	value_type CreateEnvironmentNetworkCmd    = { "TN_CreateEnvironmentNetwork",    false };

	value_type ConnectionInfoCmd              = { "TN_ConnectionInfo",              false };

	value_type ImportTaneCmd                  = { "TN_ImportTane",                  false };
    value_type ExportTaneCmd                  = { "TN_ExportTane",                  false };

	value_type ClearTransformTweakCmd         = { "TN_ClearTransformTweak",         false };
	value_type ClearRenderableTweakCmd        = { "TN_ClearRenderableTweak",        false };

	value_type GetSourceCmd                   = { "TN_GetSource",                  false };
	value_type SetSourceCmd                   = { "TN_SetSource",                  false };

	value_type GetPointCmd                    = { "TN_GetPoint",                   false };
	value_type SetPointCmd                    = { "TN_SetPoint",                   false };
};
extern StaticData<_registeredCommand> RegisteredCmd;
void SetRegisteredCmdLogging();


NAMESPACE_BASE_END

/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
namespace logging
{
	struct _maya
	{
	public:
		static const bool DrawOverride;
	
	public:
		static const bool PointsIterator;
		static const bool PointsMPxData;
	
	public:
		static const bool EnvironmentMPxNode;
		static const bool BindMPxNode;
		static const bool FetchAttributeMPxNode;
	public:
		static const bool ProxyMPxShape;
		static const bool ProxyMPxShapeDrawOverride;

		static const bool AbcProxyMPxShape;
		static const bool AbcProxyMPxShapeDrawOverride;

		static const bool Abc2ProxyMPxShape;
		static const bool Abc2ProxyMPxShapeDrawOverride;

		static const bool UsdProxyMPxShape;
		static const bool UsdProxyMPxShapeUI;
		static const bool UsdProxyMPxShapeDrawOverride;
		
		static const bool UsdHdBatchRenderer;
		static const bool UsdHdProxyMPxShape;
		static const bool UsdHdProxyMPxShapeDrawOverride;

		static const bool VdbProxyMPxShape;
		static const bool VdbProxyMPxShapeDrawOverride;

		static const bool TaneMPxTransform;
		static const bool TaneMPxTransformDrawOverride;

		static const bool TaneMPxShape;
		static const bool TaneMPxShapeGeometryOverride;
	
	public:
		static const bool CreateNodeCmd;
		static const bool CreateEnvironmentNetworkCmd;
		
		static const bool ConnectionInfoCmd;
		
		static const bool ImportTaneCmd;
		static const bool ExportTaneCmd;
	};
	extern StaticData<_maya>  maya;
}
NAMESPACE_BASE_END
/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
namespace sementic
{
}
NAMESPACE_BASE_END
