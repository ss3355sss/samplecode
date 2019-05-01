#pragma once
#include "maya/pch.h"

#include "lib/baseMaya/util.h"
#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
class SetPointCmd : public MPxCommand
{
public:
	enum Mode
	{
		kInvalid,
		kSourceID,
		kRenderable,
		kPosition,
		kRotation,
		kScale,
	};
public:
	static MString                  Cmd;

private:
	MObject     obj_;
	Mode        mode_;
	int         pid_;

	/* target params */
	int     sid_;
	int     ren_;
	Point3d pos_;
	Vec3d   rot_;
	Vec3d   scl_;
	
public:
	static void* Creator()
	{
		return new SetPointCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-h",   "-help");
		syntax.addFlag("-nn",  "-nodeName",   MSyntax::kString);
		syntax.addFlag("-pid", "-pointId",    MSyntax::kLong);
		syntax.addFlag("-sid", "-sourceId",   MSyntax::kLong);
		syntax.addFlag("-ren", "-renderable", MSyntax::kLong);
		syntax.addFlag("-pos", "-position",   MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
		syntax.addFlag("-rot", "-rotation",   MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
		syntax.addFlag("-scl", "-scale",      MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);

		return syntax;

	}
public:
	SetPointCmd() : mode_(kInvalid), pid_(-1)
	{
	}
	virtual ~SetPointCmd()
	{
	}
public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList& args)
	{
		DECL_LOGGER(RegisteredCmd->SetPointCmd.logging);

		MStatus stat;
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDataBase");
		
		if(argDatabase.isFlagSet("-h"))
		{
			const char* helpMsg =
					          "\nUsage : "
			;
			setResult(helpMsg);
			return MStatus::kSuccess;
		}

		stat = parseArg_(argDatabase);
		MCHECKERROR(stat, "Failed to parseArg_(args)");

		MFnDagNode shapeFn(obj_, &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");

		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MS::kFailure;
		}

		std::shared_ptr<Points> pInPoints  = pShape->cachedPoints();
		if(!pInPoints)
		{
			STDOUT_ERROR("Failed to get pInPoints");
			return MS::kFailure;
		}

		std::shared_ptr<Points> pOutPoints = pShape->outPoints();
		if(!pOutPoints)
		{
			STDOUT_ERROR("Failed to get pOutPoints");
			return MS::kFailure;
		}

		const int npoints = pOutPoints->size();
		if(pid_ >= npoints)
		{
			STDOUT_ERROR("Failed to get valid pid");
			return MS::kFailure;
		}

		/* pInPoints */
		const Int32Array  & sidArray0        = pInPoints->get<int>(sementic::attribute::sid);
		const Int32Array  & renderableArray0 = pInPoints->get<int>(sementic::attribute::renderable);
		const Point3fArray& pointArray0      = pInPoints->get();
		const Vec3fArray  & eulerAngleArray0 = pInPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		const Vec3fArray  & scaleArray0      = pInPoints->get<Vec3f>(sementic::attribute::scale);

		switch(mode_)
		{
			case kRenderable:
			{
				LOGGER_DEBUG("sid = %s", ren_ ? "true" : "false");
				MPlug arrayPlug = shapeFn.findPlug("mRenderables", &stat);
				MPlug plug      = arrayPlug.elementByPhysicalIndex(pid_);
				stat = plug.setValue(ren_);
				MCHECKERROR(stat, "Failed to set renderable");
			}
			break;
			case kSourceID:
			{
				LOGGER_DEBUG("sid = %d", sid_);

				MPlug arrayPlug = shapeFn.findPlug("mSourceIDs", &stat);
				MPlug plug      = arrayPlug.elementByPhysicalIndex(pid_);

				int offset = sid_ - sidArray0[pid_];

				stat = plug.setValue(offset);
				MCHECKERROR(stat, "Failed to set sourceId");
			}
			break;

			case kPosition:
			{
				LOGGER_DEBUG("pos = (%5.2f, %5.2f, %5.2f)", pos_[0], pos_[1], pos_[2]);
				MPlug arrayPlug = shapeFn.findPlug("controlPoints", &stat);
				MPlug plug      = arrayPlug.elementByPhysicalIndex(pid_);

				double offset0 = pos_[0] - pointArray0[pid_][0];
				double offset1 = pos_[1] - pointArray0[pid_][1];
				double offset2 = pos_[2] - pointArray0[pid_][2];

				stat = plug.child(0).setValue(offset0);
				MCHECKERROR(stat, "Failed to set translateX");
				stat = plug.child(1).setValue(offset1);
				MCHECKERROR(stat, "Failed to set translateY");
				stat = plug.child(2).setValue(offset2);
				MCHECKERROR(stat, "Failed to set translateZ");
			}
			break;

			case kRotation:
			{
				LOGGER_DEBUG("rot = (%5.2f, %5.2f, %5.2f)", rot_[0], rot_[1], rot_[2]);
				MPlug arrayPlug = shapeFn.findPlug("mEulerAngles", &stat);
				MPlug plug      = arrayPlug.elementByPhysicalIndex(pid_);

				double offset0 = rot_[0] - eulerAngleArray0[pid_][0];
				double offset1 = rot_[1] - eulerAngleArray0[pid_][1];
				double offset2 = rot_[2] - eulerAngleArray0[pid_][2];

				stat = plug.child(0).setValue(offset0);
				MCHECKERROR(stat, "Failed to set eulerAngleX");
				stat = plug.child(1).setValue(offset1);
				MCHECKERROR(stat, "Failed to set eulerAngleY");
				stat = plug.child(2).setValue(offset2);
				MCHECKERROR(stat, "Failed to set eulerAngleZ");

			}
			break;
			case kScale:
			{
				LOGGER_DEBUG("scl = (%5.2f, %5.2f, %5.2f)", scl_[0], scl_[1], scl_[2]);
				MPlug arrayPlug = shapeFn.findPlug("mScales", &stat);
				MPlug plug      = arrayPlug.elementByPhysicalIndex(pid_);

				double offset0 = scl_[0] - scaleArray0[pid_][0];
				double offset1 = scl_[1] - scaleArray0[pid_][1];
				double offset2 = scl_[2] - scaleArray0[pid_][2];

				stat = plug.child(0).setValue(offset0);
				MCHECKERROR(stat, "Failed to set scaleX");
				stat = plug.child(1).setValue(offset1);
				MCHECKERROR(stat, "Failed to set scaleY");
				stat = plug.child(2).setValue(offset2);
				MCHECKERROR(stat, "Failed to set scaleZ");
			}
			break;

		}
		return stat;
	}
private:
	MStatus parseArg_(const MArgDatabase& argDataBase)
	{
		DECL_LOGGER(RegisteredCmd->SetPointCmd.logging);
		MStatus stat;

		MString nodeName;
		stat = argDataBase.getFlagArgument("-nn", 0, nodeName);
		MCHECKERROR(stat, "Failed to get -nn argument");

		stat = argDataBase.getFlagArgument("-pid", 0, pid_);
		MCHECKERROR(stat, "Failed to get -pid argument");

		if(argDataBase.isFlagSet("-sid", &stat))
		{
			mode_ = Mode::kSourceID;
			argDataBase.getFlagArgument("-sid", 0, sid_);
		}
		else if(argDataBase.isFlagSet("-ren", &stat))
		{
			mode_ = Mode::kRenderable;
			argDataBase.getFlagArgument("-ren", 0, ren_);
		}
		else if(argDataBase.isFlagSet("-pos"), &stat)
		{
			mode_ = Mode::kPosition;
			argDataBase.getFlagArgument("-pos", 0, pos_[0]);
			argDataBase.getFlagArgument("-pos", 1, pos_[1]);
			argDataBase.getFlagArgument("-pos", 2, pos_[2]);
		}
		else if(argDataBase.isFlagSet("-rot"), &stat)
		{
			mode_ = Mode::kPosition;
			argDataBase.getFlagArgument("-rot", 0, rot_[0]);
			argDataBase.getFlagArgument("-rot", 1, rot_[1]);
			argDataBase.getFlagArgument("-rot", 2, rot_[2]);
		}
		else if(argDataBase.isFlagSet("-scl"), &stat)
		{
			mode_ = Mode::kPosition;
			argDataBase.getFlagArgument("-scl", 0, scl_[0]);
			argDataBase.getFlagArgument("-scl", 1, scl_[1]);
			argDataBase.getFlagArgument("-scl", 2, scl_[2]);
		}
		else
		{
			LOGGER_ERROR("Failed to get valid flag");
			return MS::kFailure;
		}

		MDagPath dagPath;
		stat = GetDagPathByName(nodeName, &dagPath);
		MCHECKERROR(stat, "Failed to get dagPath by nodeName");

		MFnDagNode shapeFn(dagPath.node(), &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");

		if(shapeFn.typeId() == RegisteredNodes->TaneMPxShape.id)
		{
			obj_ = dagPath.node();
		}
		else
		{
			STDOUT_ERROR("Failed to get %s type", RegisteredNodes->TaneMPxShape.name.asChar());
			return MS::kFailure;
		}
		return stat;
	}

};
MString SetPointCmd::Cmd(RegisteredCmd->SetPointCmd.name);
NAMESPACE_BASE_END
