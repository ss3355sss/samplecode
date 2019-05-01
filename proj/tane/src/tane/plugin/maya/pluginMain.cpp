#include "maya/pch.h"

#include "baseGl/glutil.h"
#include "baseGl/glShader.h"
#include "baseGl/glBuffer.h"

#include "baseMaya/pluginRegister.h"

///----------------------------------------------------------------DrawOverride wrapper
#include "maya/drawOverride.h"

///----------------------------------------------------------------MPxData
#include "maya/messageMPxData.h"
#include "maya/pointsMPxData.h"
#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

///----------------------------------------------------------------MPxNode
#include "maya/environmentMPxNode.h"
#include "maya/bindMPxNode.h"
#include "maya/fetchAttributeMPxNode.h"

///----------------------------------------------------------------MPxTransform
#include "maya/taneMPxTransform.h"
#include "maya/taneMPxTransformDrawOverride.h"

///----------------------------------------------------------------MPxSurfaceShape
#include "maya/abcProxyMPxShape.h"
#include "maya/abc2ProxyMPxShape.h"
#include "maya/usdProxyMPxShape.h"

#include "maya/taneMPxShape.h"
#include "maya/taneMPxShapeGeometryOverride.h"

///----------------------------------------------------------------MPxManipContainer

///----------------------------------------------------------------MPxContext

///----------------------------------------------------------------MPxCommand
#include "maya/createNodeCmd.h"
#include "maya/createEnvironmentNetworkCmd.h"

#include "maya/clearRenderableTweakCmd.h"
#include "maya/clearTransformTweakCmd.h"

#include "maya/connectionInfoCmd.h"

#include "maya/getSourceCmd.h"
#include "maya/setSourceCmd.h"

#include "maya/getPointCmd.h"
#include "maya/setPointCmd.h"

#include "maya/exportCmd.h"
#include "maya/importCmd.h"

#include <maya/MFnPlugin.h>
#include <ctime>
#include <ratio>
#include <chrono>


bool InitializeTaneEnv()
{
	USING_NAMESPACE_BASE
	STDOUT_INFO("InitializeTaneEnv");

	if(!GlewInit())
	{
		STDOUT_ERROR("Failed to initialize glew");
		return false;
	}
	/* set gl options */
	/* ------------------------------------ configure shaders */
	/* GlIcrShader */
	STDOUT_INFO("Build IcrShader");
	{
		const std::shared_ptr<GlIcrShader>& pShader = GlIcrShader::Get();
		if(!pShader)
		{
			STDOUT_ERROR("Failed to build IcrShader");
			return false;
		}
	}
	/* GlIcrPhongShader */
	STDOUT_INFO("Build IcrPhongShader");
	{
		const std::shared_ptr<GlIcrPhongShader>& pShader = GlIcrPhongShader::Get();
		if(!pShader)
		{
			STDOUT_ERROR("Failed to build IcrPhongShader");
			return false;
		}
	}
	
	RegisteredNodesLogging();
	SetRegisteredCmdLogging();
	
	return true;
}
bool ReleaseTaneEnv()
{
	USING_NAMESPACE_BASE
	STDOUT_INFO("ReleaseTaneEnv");

	/* ------------------------------------ release shaders */
	/* GlIcrShader */
	{
		std::shared_ptr<GlIcrShader>& pShader = GlIcrShader::Get();
		if(!pShader)
		{
			STDOUT_ERROR("Failed to get GlIcrShader");
			return false;
		}
		pShader->clear();
		pShader.reset();
	}
	/* GlIcrPhongShader */
	{
		std::shared_ptr<GlIcrPhongShader>& pShader = GlIcrPhongShader::Get();
		if(!pShader)
		{
			STDOUT_ERROR("Failed to get GlIcrPhongShader");
			return false;
		}
		pShader->clear();
		pShader.reset();
	}

	return true;
}

MStatus initializePlugin(MObject obj)
{
	USING_NAMESPACE_BASE

	std::string vendor 				= "Dexter Studio";
	std::string version 			= "0.0";
	std::string requiredApiVersion 	= "MAYA_VER=" + std::string(MAYA_VER) + ", RMAN_VER=" + std::string(RMAN_VER);
	
	MFnPlugin pluginFn(obj, vendor.c_str(), version.c_str(), "Any");
	MStatus stat;
	
	if(!InitializeTaneEnv())
	{
		STDOUT_ERROR("Failed to initialize TaneEnv");
		return MS::kFailure;
	}

	///------------------------------------- Load UsdMaya Plugin
	{
		int loaded = 0;
		stat = MGlobal::executeCommand("pluginInfo -q -loaded  \"pxrUsd\"", loaded);
		MCHECKERROR(stat, "Failed to query pxrUsd plugin state");
		if(!loaded)
		{
			stat = MGlobal::executeCommand("loadPlugin \"pxrUsd\"");
			MCHECKERROR(stat, "Failed to load pxrUsd plugin");
		}
	}

	std::shared_ptr<PluginRegister> pPluginRegister = PluginRegister::Get();
	///----------------------------------------------------------------MPxData
	///------------------------------------- MessageMPxData
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxData>(MessageMPxData::typeName,
	                                                                                           MessageMPxData::id,
	                                                                                           &MessageMPxData::creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- PointsMPxData
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxGemoetryData>(PointsMPxData::typeName,
                                                                                                	   PointsMPxData::id,
	                                                                                                   &PointsMPxData::creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- SourceMPxData
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxData>(SourceMPxData::typeName,
	                                                                                           SourceMPxData::id,
	                                                                                           &SourceMPxData::creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- SourcesMPxData
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxData>(SourcesMPxData::typeName,
	                                                                                           SourcesMPxData::id,
	                                                                                           &SourcesMPxData::creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///----------------------------------------------------------------MPxNode
	///------------------------------------- EnvironmentMPxNode
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxNode>(EnvironmentMPxNode::nodeName,
	                                                                                           EnvironmentMPxNode::id,
	                                                                                           EnvironmentMPxNode::Creator,
	                                                                                           EnvironmentMPxNode::Initialize
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- BindMPxNode
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxNode>(BindMPxNode::nodeName,
	                                                                                           BindMPxNode::id,
	                                                                                           BindMPxNode::Creator,
	                                                                                           BindMPxNode::Initialize
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- FetchAttributeMPxNode
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxNode>(FetchAttributeMPxNode::nodeName,
	                                                                                           FetchAttributeMPxNode::id,
	                                                                                           FetchAttributeMPxNode::Creator,
	                                                                                           FetchAttributeMPxNode::Initialize
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	
	///----------------------------------------------------------------MPxTransform
	///------------------------------------- TaneMPxTransform
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxTransformNode>(TaneMPxTransform::nodeName,
		                                                                                                TaneMPxTransform::id,
	                                                                                                    TaneMPxTransform::Creator,
	                                                                                                    TaneMPxTransform::Initialize,
		                                                                                                TaneMPxTransformationMatrix::Creator,
		                                                                                                TaneMPxTransformationMatrix::id,
		                                                                                                TaneMPxTransform::drawDbClassification
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxDrawOverride>(TaneMPxTransform::drawDbClassification,
	                                                                                                   TaneMPxTransform::drawOverrideRegistrantId,
	                                                                                                   TaneMPxTransformDrawOverride::Creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///----------------------------------------------------------------MPxSurfaceShape
	///------------------------------------- AbcProxyMPxShape
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxShape>(AbcProxyMPxShape::NodeName,
	                                                                                            AbcProxyMPxShape::Id,
									                                                            AbcProxyMPxShape::Creator,
									                                                            AbcProxyMPxShape::Initialize,
									                                                            AbcProxyMPxShapeUI::Creator,
									                                                            AbcProxyMPxShape::DrawDbClassification
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxDrawOverride>(AbcProxyMPxShape::DrawDbClassification,
	                                                                                                   AbcProxyMPxShape::DrawOverrideRegistrantId,
		                                                                                               AbcProxyMPxShapeDrawOverride::Creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///------------------------------------- Abc2ProxyMPxSurfaceShape
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxShape>(Abc2ProxyMPxShape::nodeName,
	                                                                                            Abc2ProxyMPxShape::id,
	                                                                                            Abc2ProxyMPxShape::Creator,
	                                                                                            Abc2ProxyMPxShape::Initialize,
	                                                                                            Abc2ProxyMPxShapeUI::Creator,
	                                                                                            Abc2ProxyMPxShape::drawDbClassification
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxDrawOverride>(Abc2ProxyMPxShape::drawDbClassification,
	                                                                                                   Abc2ProxyMPxShape::drawOverrideRegistrantId,
	                                                                                                   Abc2ProxyMPxShapeDrawOverride::Creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- UsdProxyMPxShape
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxShape>(UsdProxyMPxShape::NodeName,
	                                                                                            UsdProxyMPxShape::Id,
	                                                                                            UsdProxyMPxShape::Creator,
	                                                                                            UsdProxyMPxShape::Initialize,
	                                                                                            UsdProxyMPxShapeUI::Creator,
	                                                                                            UsdProxyMPxShape::DrawDbClassification
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxDrawOverride>(UsdProxyMPxShape::DrawDbClassification,
	                                                                                                   UsdProxyMPxShape::DrawOverrideRegistrantId,
	                                                                                                   UsdProxyMPxShapeDrawOverride::Creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- UsdHdProxyMPxShape (deprecated)
	//stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxShape>(UsdHdProxyMPxShape::TypeName,
	//	                                                                                        UsdHdProxyMPxShape::TypeId,
	//                                                                                            UsdHdProxyMPxShape::Creator,
	//                                                                                            UsdHdProxyMPxShape::Initialize,
	//                                                                                            UsdHdProxyMPxShapeUI::Creator,
	//                                                                                            UsdHdProxyMPxShape::DrawDbClassification
	//));
	//CHECK_MSTATUS_AND_RETURN_IT(stat);
	//stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxDrawOverride>(UsdHdProxyMPxShape::DrawDbClassification,
	//                                                                                                   UsdHdProxyMPxShape::DrawOverrideRegistrantId,
	//                                                                                                   UsdHdProxyMPxShapeDrawOverride::Creator
	//));
	//CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- TaneMPxShape
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxShape>(TaneMPxShape::nodeName,
		                                                                                        TaneMPxShape::id,
								                                                                TaneMPxShape::Creator,
								                                                                TaneMPxShape::Initialize,
								                                                                TaneMPxShapeUI::Creator,
								                                                                TaneMPxShape::drawDbClassification
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxGeometryOverride>(TaneMPxShape::drawDbClassification,
		                                                                                                   TaneMPxShape::drawOverrideRegistrantId,
	                                                                                                       TaneMPxShapeGeometryOverride::Creator
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxComponentConverter>(TaneMPxShapeGeometryOverride::DormantPointsRenderItem,
		                                                                                                     TaneMPxShapeComponentConverter::Creator
		));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///----------------------------------------------------------------MPxSurfaceShapeManip
	///----------------------------------------------------------------MPxContext
	///------------------------------------- PickingMPxContext
	///----------------------------------------------------------------MPxCommand
	///------------------------------------- TaneMPxShapeClearRenderableTweaksCmd
	///------------------------------------- TaneMPxShapeClearTweaksCmd
	///------------------------------------- TaneMPxShapeRenderCmd
	///------------------------------------- FrustumCullingCmd

	///------------------------------------- CreateNodeCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(CreateNodeCmd::Cmd,
		                                                                                          CreateNodeCmd::Creator,
		                                                                                          CreateNodeCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- CreateEnvironmentNetworkCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(createEnvironmentNetworkCmd::Cmd,
	                                                                                              createEnvironmentNetworkCmd::Creator,
	                                                                                              createEnvironmentNetworkCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- ConnectedInfoCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(ConnectionInfoCmd::Cmd,
	                                                                                              ConnectionInfoCmd::Creator,
	                                                                                              ConnectionInfoCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	
	///------------------------------------- GetSourceCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(GetSourceCmd::Cmd,
	                                                                                              GetSourceCmd::Creator,
	                                                                                              GetSourceCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- GetSourceCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(SetSourceCmd::Cmd,
	                                                                                              SetSourceCmd::Creator,
	                                                                                              SetSourceCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	
	///------------------------------------- GetPointCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(GetPointCmd::Cmd,
	                                                                                              GetPointCmd::Creator,
	                                                                                              GetPointCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- SetPointCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(SetPointCmd::Cmd,
	                                                                                              SetPointCmd::Creator,
	                                                                                              SetPointCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);

	///------------------------------------- ClearTransformTweakCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(ClearTransformTweakCmd::Cmd,
	                                                                                              ClearTransformTweakCmd::Creator,
	                                                                                              ClearTransformTweakCmd::NewSyntax
	));
	
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	///------------------------------------- ClearRenderableTweakCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(ClearRenderableTweakCmd::Cmd,
	                                                                                              ClearRenderableTweakCmd::Creator,
	                                                                                              ClearRenderableTweakCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	
	///------------------------------------- ImportTaneCmd
	stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(ImportTaneCmd::Cmd,
	                                                                                              ImportTaneCmd::Creator,
	                                                                                              ImportTaneCmd::NewSyntax
	));
	CHECK_MSTATUS_AND_RETURN_IT(stat);
	
	///------------------------------------- ExportTaneCmd
    stat = pPluginRegister->registerPlugin(pluginFn, std::make_shared<PluginRegister::MPxCommand>(ExportTaneCmd::Cmd,
                                                                                                  ExportTaneCmd::Creator,
                                                                                                  ExportTaneCmd::NewSyntax
    ));
    CHECK_MSTATUS_AND_RETURN_IT(stat);

	stat = MGlobal::sourceFile("TaneInit.mel");
	MCHECKERROR(stat, "Failed to source TaneInit.mel");

	stat = MGlobal::sourceFile("TaneUtil.mel");
	MCHECKERROR(stat, "Failed to source TanuUtil.mel");

	stat = MGlobal::sourceFile("DagMenuProcs.mel");
	MCHECKERROR(stat, "Failed to source DagMenuProcs.mel");

	stat = pluginFn.registerUI( "TN_CreateTaneMenu", "TN_DeleteTaneMenu" );
	MCHECKERROR(stat, "Failed to registerUI Tane");

	std::string msg;
	msg += "TaneForMaya ";
	#ifdef DEBUG
	msg += "(DEBUG), ";
	#elif RELEASE
	msg += "(RELEASE), ";
	#endif
	msg += " is loaded, 190308 19:37 ";
	msg += (boost::format("MAYA_VER = %s, USD_VER = %s") % std::string(MAYA_VER) % std::string(USD_VER)).str();

	fprintf(stdout, "\033[1;32m");
	fprintf(stdout, "%s\n", msg.c_str());
	if(ENABLE_LOGGER)
	{
		fprintf(stdout, "Logger is enabled\n");
	}
	else
	{
		fprintf(stdout, "Logger is disabled\n");
	}
	fprintf(stdout, "\033[0m");

	
	return stat;
}

MStatus uninitializePlugin(MObject obj)
{
	USING_NAMESPACE_BASE
	MStatus stat;
	MFnPlugin plugin(obj);
	std::shared_ptr<PluginRegister> pPluginRegister = PluginRegister::Get();
	pPluginRegister->deregisterPlugin(plugin);
	
	for(DrawOverride::iterator iter = DrawOverride::Begin(); iter != DrawOverride::End(); ++iter)
	{
		const MDagPath objPath  = iter->first;
		DrawOverride::Deregister(objPath);
	}
	
	if(!ReleaseTaneEnv())
	{
		STDOUT_ERROR("Failed to release TaneEnv");
		return MS::kFailure;
	}
	return stat;
}

