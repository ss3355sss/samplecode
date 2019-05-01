#include "maya/pch.h"

NAMESPACE_BASE_BEGIN
/* ----------------------------------------------- register node */
StaticData<_registeredRenderItems>  RegisteredRenderItems;
StaticData<_registeredNodes>        RegisteredNodes;
StaticData<_registeredCommand>      RegisteredCmd;

void RegisteredNodesLogging()
{
	///----------------------------------------------- MPxData
	RegisteredNodes->PointsMPxData.logging                  = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->SourceMPxData.logging                  = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->SourcesMPxData.logging                 = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->MessageMPxData.logging                 = (ENABLE_LOGGER ? false : false);
	///----------------------------------------------- MPxNode
	RegisteredNodes->EnvironmentMPxNode.logging             = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->BindMPxNode.logging                    = (ENABLE_LOGGER ? true  : false);
	RegisteredNodes->FetchAttributeMPxNode.logging          = (ENABLE_LOGGER ? true  : false);
	
	///----------------------------------------------- MPxTransformationMatrix, MPxTransform
	RegisteredNodes->TaneMPxTransformMtx.logging            = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->TaneMPxTransform.logging               = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->TaneMPxTransformDrawOverride.logging   = (ENABLE_LOGGER ? false : false);
	
	///----------------------------------------------- MPxSurfaceShape
	RegisteredNodes->TaneMPxShape.logging                   = (ENABLE_LOGGER ? false  : false);
	RegisteredNodes->TaneMPxShapeGeometryOverride.logging   = (ENABLE_LOGGER ? false  : false);
	RegisteredNodes->AbcProxyMPxShape.logging               = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->Abc2ProxyMPxShape.logging              = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->UsdProxyMPxShape.logging               = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->UsdHdProxyMPxShape.logging             = (ENABLE_LOGGER ? false : false);
	RegisteredNodes->VdbProxyMPxShape.logging               = (ENABLE_LOGGER ? false : false);
}
void SetRegisteredCmdLogging()
{
	RegisteredCmd->CreateNodeCmd.logging               = (ENABLE_LOGGER ? false : false);
	RegisteredCmd->CreateEnvironmentNetworkCmd.logging = (ENABLE_LOGGER ? false : false);

	RegisteredCmd->ConnectionInfoCmd.logging           = (ENABLE_LOGGER ? false : false);

	RegisteredCmd->ImportTaneCmd.logging               = (ENABLE_LOGGER ? false : false);
	RegisteredCmd->ExportTaneCmd.logging               = (ENABLE_LOGGER ? true : false);

	RegisteredCmd->ClearTransformTweakCmd.logging      = (ENABLE_LOGGER ? true : false);
	RegisteredCmd->ClearRenderableTweakCmd.logging     = (ENABLE_LOGGER ? false : false);

	RegisteredCmd->GetSourceCmd.logging                = (ENABLE_LOGGER ? false : false);
	RegisteredCmd->SetSourceCmd.logging                = (ENABLE_LOGGER ? false : false);

	RegisteredCmd->GetPointCmd.logging                 = (ENABLE_LOGGER ? true : false);
	RegisteredCmd->SetPointCmd.logging                 = (ENABLE_LOGGER ? false : false);
}



NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
/* ----------------------------------------------- logging */
namespace logging
{
	const bool _maya::DrawOverride                      = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::PointsIterator                    = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::PointsMPxData                     = (ENABLE_LOGGER ? false : false);

	const bool _maya::EnvironmentMPxNode                = (ENABLE_LOGGER ? true  : false);
	const bool _maya::BindMPxNode                       = (ENABLE_LOGGER ? true   : false);
	const bool _maya::FetchAttributeMPxNode             = (ENABLE_LOGGER ? true   : false);
 
	const bool _maya::ProxyMPxShape                     = (ENABLE_LOGGER ? false : false);
	const bool _maya::ProxyMPxShapeDrawOverride         = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::AbcProxyMPxShape                  = (ENABLE_LOGGER ? false : false);
	const bool _maya::AbcProxyMPxShapeDrawOverride      = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::Abc2ProxyMPxShape                 = (ENABLE_LOGGER ? false : false);
	const bool _maya::Abc2ProxyMPxShapeDrawOverride     = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::UsdProxyMPxShape                  = (ENABLE_LOGGER ? false : false);
	const bool _maya::UsdProxyMPxShapeUI                = (ENABLE_LOGGER ? false : false);
	const bool _maya::UsdProxyMPxShapeDrawOverride      = (ENABLE_LOGGER ? false : false);

	const bool _maya::UsdHdBatchRenderer                = (ENABLE_LOGGER ? false : false);
	const bool _maya::UsdHdProxyMPxShape                = (ENABLE_LOGGER ? false : false);
	const bool _maya::UsdHdProxyMPxShapeDrawOverride    = (ENABLE_LOGGER ? false : false);

	const bool _maya::VdbProxyMPxShape                  = (ENABLE_LOGGER ? false : false);
	const bool _maya::VdbProxyMPxShapeDrawOverride      = (ENABLE_LOGGER ? false : false);
 
	const bool _maya::TaneMPxTransform                  = (ENABLE_LOGGER ? false : false);
	const bool _maya::TaneMPxTransformDrawOverride      = (ENABLE_LOGGER ? false  : false);
 
	const bool _maya::TaneMPxShape                      = (ENABLE_LOGGER ? true  : false);
	const bool _maya::TaneMPxShapeGeometryOverride      = (ENABLE_LOGGER ? false : false);

	const bool _maya::CreateNodeCmd                     = (ENABLE_LOGGER ? false : false);
	const bool _maya::CreateEnvironmentNetworkCmd       = (ENABLE_LOGGER ? false : false);

	const bool _maya::ConnectionInfoCmd                 = (ENABLE_LOGGER ? false : false);

	const bool _maya::ImportTaneCmd                     = (ENABLE_LOGGER ? true : false);
	const bool _maya::ExportTaneCmd                     = (ENABLE_LOGGER ? true : false);

	StaticData<_maya> maya;
}
NAMESPACE_BASE_END

