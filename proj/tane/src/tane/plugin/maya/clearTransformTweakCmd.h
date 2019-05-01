#pragma once
#include "maya/pch.h"

#include "lib/baseMaya/util.h"
#include "maya/taneMPxShape.h"
NAMESPACE_BASE_BEGIN
class ClearTransformTweakCmd : public MPxCommand
{
public:
	static MString Cmd;
private:
	MObject         shapeObj_;
	MDagModifier    dagMod_;

public:
	static void* Creator()
	{
		return new ClearTransformTweakCmd;
	}
	static MSyntax NewSyntax()
	{
		
		MSyntax syntax;
		syntax.addFlag("-nn", "-nodeName", MSyntax::kString);
		syntax.useSelectionAsDefault(true);
		syntax.setObjectType(MSyntax::kSelectionList, 0);
		
		return syntax;
	}

public:
	ClearTransformTweakCmd()
	{
		DECL_LOGGER(RegisteredCmd->ClearTransformTweakCmd.logging);
		
	}
	virtual ~ClearTransformTweakCmd()
	{
		DECL_LOGGER(RegisteredCmd->ClearTransformTweakCmd.logging);
	}

public:
	virtual bool isUndoable() const
	{
		return true;
	}
	
	virtual MStatus undoIt()
	{
		return dagMod_.undoIt();
	}
	virtual MStatus redoIt()
	{
		return dagMod_.doIt();
	}
	virtual MStatus doIt(const MArgList &args)
	{
		DECL_LOGGER(RegisteredCmd->ClearTransformTweakCmd.logging);
		MStatus stat;
		
		stat = parseArg_(args);
		MCHECKERROR(stat, "Failed to parse args");
		
		MFnDagNode shapeFn(shapeObj_, &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");
		
		TaneMPxShape *pShape = dynamic_cast<TaneMPxShape *>(shapeFn.userNode());
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		
		const std::shared_ptr<Points> pPoints = pShape->outPoints();
		if(!pPoints || !(pPoints->size() > 0))
		{
			STDOUT_ERROR("Failed to get pPoints");
			return MS::kFailure;
		}
		
		MString nodeName = shapeFn.dagPath().fullPathName();
		MString msg;
		if(pShape->hasActiveComponents())
		{
			/* clear selected points tweaks */
			MObjectArray activeComponents = pShape->activeComponents();
			MFnSingleIndexedComponent fnComponent(activeComponents[0]);
			if (fnComponent.elementCount())
			{
				MIntArray activeIds;
				fnComponent.getElements(activeIds);
				
				if (fnComponent.componentType() == MFn::kMeshVertComponent)
				{
					unsigned int activePointsCount = activeIds.length();
					
					for (int i = 0; i < activePointsCount; ++i)
					{
						const int pid = activeIds[i];
						msg += MString((boost::format("setAttr \"%s.controlPoints[%d]\" 0 0 0;") % nodeName.asChar() % pid).str().c_str());
						msg += MString((boost::format("setAttr \"%s.mEulerAngles[%d]\" 0 0 0;")  % nodeName.asChar() % pid).str().c_str());
						msg += MString((boost::format("setAttr \"%s.mScales[%d]\" 0 0 0;")       % nodeName.asChar() % pid).str().c_str());
					}
				}
			}
		}
		else
		{
			/* clear all tweaks */
			const size_t npoints = pPoints->size();
			for (size_t i = 0; i < npoints; ++i)
			{
				msg += MString((boost::format("setAttr \"%s.controlPoints[%d]\" 0 0 0;") % nodeName.asChar() % i).str().c_str());
				msg += MString((boost::format("setAttr \"%s.mEulerAngles[%d]\" 0 0 0;")  % nodeName.asChar() % i).str().c_str());
				msg += MString((boost::format("setAttr \"%s.mScales[%d]\" 0 0 0;")       % nodeName.asChar() % i).str().c_str());
			}
		}
		
		stat = dagMod_.commandToExecute(msg);
		MCHECKERROR(stat, "Failed to insert commandToExecute");
		return dagMod_.doIt();
	}

private:
	MStatus parseArg_(const MArgList &args)
	{
		DECL_LOGGER(RegisteredCmd->ClearTransformTweakCmd.logging);
		MStatus stat;
		
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDataBase");
		
		MString nodeName;
		stat = argDatabase.getFlagArgument("-nn", 0, nodeName);
		MCHECKERROR(stat, "Failed to get -nn argument");
		
		MDagPath dagPath;
		stat = GetDagPathByName(nodeName, &dagPath);
		MCHECKERROR(stat, "Failed to get dagPath by nodeName");
		shapeObj_ = dagPath.node(&stat);
		
		if(shapeObj_.isNull())
		{
			STDOUT_ERROR("Failed to get valid shapeObj");
			return MS::kFailure;
		}
		
		return stat;
	}
	
};
MString ClearTransformTweakCmd::Cmd(RegisteredCmd->ClearTransformTweakCmd.name);
NAMESPACE_BASE_END
