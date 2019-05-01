#pragma once
#include "maya/pch.h"

#include "maya/createUtil.h"

#include "baseMaya/util.h"

///------------------------------------------------------------------ CreateEnvironmentNetworkCmd
NAMESPACE_BASE_BEGIN
class createEnvironmentNetworkCmd: public MPxCommand
{
public:
	static MString      Cmd;
private:
	MString      inBodyName_;

public:
	static void* Creator()
	{
		return new createEnvironmentNetworkCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-ib", "-inBody",   MSyntax::kString);
		return syntax;
	}

public:
	createEnvironmentNetworkCmd()
	{
	}
	virtual ~createEnvironmentNetworkCmd()
	{
	}

public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList &args)
	{
		DECL_LOGGER(logging::maya->CreateEnvironmentNetworkCmd);
		MStatus stat;
		stat = parseArgs_(args);
		MCHECKERROR(stat, "Failed to parse args");

		LOGGER_DEBUG("inBodyName = %s", inBodyName_.asChar());
		MDagPath inBodyDagPath;
		stat = GetDagPathByName(inBodyName_, &inBodyDagPath);
		MCHECKERROR(stat, "Failed to get %s DagPath", inBodyName_.asChar());

		/* Create TN_EnvironmentMPxNode */
		MObject envObj = CreateDGNode(RegisteredNodes->EnvironmentMPxNode.name);
		if(envObj.isNull())
		{
			STDOUT_ERROR("Failed to create envObj");
			return MS::kFailure;
		}
		stat = ConnectOutTime(envObj);
		MCHECKERROR(stat, "Failed to connect time1.outTime");
		
		/* Create TN_Tane */
		MObject taneXformObj, taneShapeObj;
		stat = CreateTN_Nodes(RegisteredNodes->TaneMPxShape.name, "", &taneXformObj, &taneShapeObj);
		MCHECKERROR(stat, "Failed to create TN_Tane");
		
		/* connect inBody.worldMesh -> TN_Environment.inBody */
		{
			/* Get InBody.worldMesh Plug */
			MFnDependencyNode inBodyDepFn(inBodyDagPath.node(&stat), &stat);
			MCHECKERROR(stat, "Failed to get %s depFn", inBodyName_.asChar());
			LOGGER_DEBUG("inBodyDepFn.absoluteName = %s", inBodyDepFn.absoluteName().asChar());

			MPlug worldMeshPlugArray = inBodyDepFn.findPlug("worldMesh", &stat);        MCHECKERROR(stat, "Failed to get %s.worldMesh plug array", inBodyName_.asChar());
			MPlug worldMeshPlug =  worldMeshPlugArray.elementByLogicalIndex(0, &stat);  MCHECKERROR(stat, "Failed to get %s.worldMesh[0] plug", inBodyName_.asChar());

			/* Get TN_Environment.inBody Plug */
			MFnDependencyNode envDepFn(envObj, &stat);
			MPlug inBodyPlug = envDepFn.findPlug("inBody", &stat);
			MCHECKERROR(stat, "Failed to get TN_Environment.inBody plug");
			
			LOGGER_DEBUG("worldMeshPlug.name() = %s", worldMeshPlug.name().asChar());
			
			stat = Connect(worldMeshPlug, inBodyPlug);
			MCHECKERROR(stat, "Failed to connect inBody.worldMesh -> TN_Environment.inBody");
		}
		/* connect TN_Environment.output -> TN_TaneShape.input */
		{
			stat = Connect(envObj, "output", taneShapeObj, "input");
			MCHECKERROR(stat, "Failed to connect TN_Environment.output -> TN_TaneShape.input ");
		}
		
		return stat;
	}

private:
	MStatus parseArgs_(const MArgList& args)
	{
		DECL_LOGGER(logging::maya->CreateEnvironmentNetworkCmd);
		MStatus stat;
		
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDatabase");
		
		MString inBodyName;
		stat = argDatabase.getFlagArgument("-ib", 0, inBodyName_);
		MCHECKERROR(stat, "Getting argument ib is failed");
		
		return stat;
	}
	
};
MString createEnvironmentNetworkCmd::Cmd(RegisteredCmd->CreateEnvironmentNetworkCmd.name);



NAMESPACE_BASE_END