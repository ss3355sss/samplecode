#pragma once
#include "maya/common/pch.h"

NAMESPACE_MAYA_BEGIN
class SourcesInfoCmd : public MPxCommand
{
public:
	static MString cmd;

private:
	MString  			nodeName_;
	bool 				sidDefined_;
	unsigned int 	 	sid_;
	MString  			attrName_;

	MDagPath 			shapeDagPath_;

public:
	static void* creator();
	static MSyntax newSyntax();
public:
	SourcesInfoCmd();
	virtual ~SourcesInfoCmd();
public:
	virtual bool isUndoable() const;
	virtual MStatus doIt(const MArgList &args);
private:
	MStatus parseArg_(const MArgList& args);
};
NAMESPACE_MAYA_END

