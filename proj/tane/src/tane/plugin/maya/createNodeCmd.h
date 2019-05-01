#pragma once
#include "maya/pch.h"

#include "maya/createUtil.h"

#include "baseMaya/util.h"

///------------------------------------------------------------------ CreateNodeCmd
NAMESPACE_BASE_BEGIN

class CreateNodeCmd: public MPxCommand
{
public:
	static MString      Cmd;
private:
	MString      nodeType_;
	MString      nodeName_;
	
public:
	static void* Creator()
	{
		return new CreateNodeCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-nt", "-nodeType", MSyntax::kString);
		syntax.addFlag("-nn", "-nodeName", MSyntax::kString);
		return syntax;
	}

public:
	CreateNodeCmd()
	{
	}
	virtual ~CreateNodeCmd()
	{
	}
public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList &args)
	{
		DECL_LOGGER(logging::maya->CreateNodeCmd);
		MStatus stat;
		stat = parseArgs_(args);
		MCHECKERROR(stat, "Failed to parse args");
		
		LOGGER_DEBUG("nodeType = %s", nodeType_.asChar());
		LOGGER_DEBUG("nodeName = %s", nodeName_.asChar());
		
		/* create TaneMPxShape */
		MObject xformObj;
		MObject shapeObj;
		
		stat = CreateTN_Nodes(nodeType_,
			                  nodeName_,
			                  &xformObj,
			                  &shapeObj);
		MCHECKERROR(stat, "Failed to create %s", nodeType_.asChar());
		
		MFnDagNode shapeFn(shapeObj, &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");
		MString result = shapeFn.fullPathName(&stat);
		setResult(result);
		
		return stat;
	}
private:
	MStatus parseArgs_(const MArgList& args)
	{
		DECL_LOGGER(logging::maya->CreateNodeCmd);
		MStatus stat;
		
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDatabase");
		
		stat = argDatabase.getFlagArgument("-nt", 0, nodeType_);
		MCHECKERROR(stat, "Getting argument nt is failed");
		
		if(argDatabase.isFlagSet("-nn", &stat))
		{
			stat = argDatabase.getFlagArgument("-nn", 0, nodeName_);
			MCHECKERROR(stat, "Getting argument nn is failed");
		}
		return stat;
	}
};
MString CreateNodeCmd::Cmd(RegisteredCmd->CreateNodeCmd.name);

NAMESPACE_BASE_END
