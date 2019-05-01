#pragma once
#include "maya/pch.h"

#include "baseMaya/util.h"

#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
class SetSourceCmd : public MPxCommand
{
public:
	static MString                  Cmd;

public:
	static void* Creator()
	{
		return new SetSourceCmd ;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		return syntax;
	}
public:
	SetSourceCmd()
	{
	}
	virtual ~SetSourceCmd()
	{
	}
public:
	virtual MStatus doIt(const MArgList& args)
	{
		MStatus stat;
		return stat;
	}
private:
};

MString                                SetSourceCmd::Cmd(RegisteredCmd->SetSourceCmd.name);
NAMESPACE_BASE_END

