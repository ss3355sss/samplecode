#pragma once
#include "maya/pch.h"

#include "baseMaya/util.h"

#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
class ConnectionInfoCmd : public MPxCommand
{
public:
	struct _TargetNames
	{
		const std::string pid      = "pid";
		const std::string lid      = "lid";
		const std::string srcShape = "srcShape";
	};
public:
	static StaticData<_TargetNames> TargetNames;
	static MString                  Cmd;

private:
	std::string target_;
	MObject     obj_;

public:
	static void* Creator()
	{
		return new ConnectionInfoCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-nn", "-nodeName", MSyntax::kString);
		syntax.addFlag("-tg", "-target", MSyntax::kString);
		return syntax;
		
	}
public:
	ConnectionInfoCmd()
	{
	}
	virtual ~ConnectionInfoCmd()
	{
	}
public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList& args)
	{
		DECL_LOGGER(RegisteredCmd->ConnectionInfoCmd.logging);
		MStatus stat;
		stat = parseArg_(args);
		MCHECKERROR(stat, "Failed to parseArg_(args)");
		
		MFnDependencyNode depFn(obj_, &stat);
		MPlug inSourceArrayPlug = depFn.findPlug("inSource", &stat);
		
		unsigned int numConnectedElements = inSourceArrayPlug.numConnectedElements();
		
		MIntArray    pidArray(numConnectedElements, 0);
		MIntArray    lidArray(numConnectedElements, 0);
		MStringArray shapeArray(numConnectedElements, "");
		for(int pid = 0; pid < numConnectedElements; ++pid)
		{
			MPlug   inSourcePlug = inSourceArrayPlug.connectionByPhysicalIndex(pid, &stat);
			MObject srcObj       = inSourcePlug.source().node();
			
			MFnDagNode srcFn(srcObj, &stat);
			MString fullPath = srcFn.fullPathName();
			
			unsigned int lid = inSourcePlug.logicalIndex(&stat);
			LOGGER_DEBUG("logicalID[%d] = %u", pid, lid);
			
			pidArray[pid] = pid;
			lidArray[pid] = lid;
			shapeArray[pid] = fullPath;
		}
		
		if(target_ == TargetNames->pid)
			setResult(pidArray);
		if(target_ == TargetNames->lid)
			setResult(lidArray);
		if(target_ == TargetNames->srcShape)
			setResult(shapeArray);
		return stat;
	}
private:
	MStatus parseArg_(const MArgList& args)
	{
		DECL_LOGGER(RegisteredCmd->ConnectionInfoCmd.logging);
		MStatus stat;
		
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDatabase");
		
		MString nodeName;
		stat = argDatabase.getFlagArgument("-nn", 0, nodeName);
		MCHECKERROR(stat, "Failed to get -nn argument");
		
		MSelectionList selectionList;
		stat = selectionList.add(nodeName);
		MCHECKERROR(stat, "Failed to add nodeName to selectionList");
		
		stat = selectionList.getDependNode(0, obj_);
		MCHECKERROR(stat, "Failed to get depend node by name, %s", nodeName.asChar());
		
		MString target;
		stat = argDatabase.getFlagArgument("-tg", 0, target);
		MCHECKERROR(stat, "Failed to get -tg argument");
		target_ = target.asChar();
		
		return stat;
	}
};
StaticData<ConnectionInfoCmd::_TargetNames>  ConnectionInfoCmd::TargetNames;
MString                                      ConnectionInfoCmd::Cmd(RegisteredCmd->ConnectionInfoCmd.name);
NAMESPACE_BASE_END

