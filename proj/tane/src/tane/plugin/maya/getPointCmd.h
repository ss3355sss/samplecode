#pragma once
#include "maya/pch.h"
#include "baseMaya/util.h"
#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
class GetPointCmd : public MPxCommand
{
public:
	enum Mode
	{
		kAll,
		kPid,
	};
public:
	struct _TargetNames
	{
		const std::string count      = "count";
		const std::string xform      = "xform";
		const std::string pid        = "pid";
		const std::string sid        = "sid";
		const std::string renderable = "vis";
		const std::string pos        = "pos";
		const std::string eulerAngle = "rot";
		const std::string scale      = "scl";
		const std::string uvCoord    = "tex";
		
	};
public:
	static StaticData<_TargetNames> TargetNames;
	static MString                  Cmd;

private:
	Mode        mode_;
	int         pid_;
	std::string target_;
	MObject     obj_;
public:
	static void* Creator()
	{
		return new GetPointCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-nn", "-nodeName", MSyntax::kString);
		syntax.addFlag("-tg", "-target", MSyntax::kString);
		syntax.addFlag("-i",  "-index", MSyntax::kLong);
		return syntax;
		
	}
public:
	GetPointCmd() : mode_(kAll), pid_(-1)
	{
	}
	virtual ~GetPointCmd()
	{
	}
public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList& args)
	{
		DECL_LOGGER(RegisteredCmd->GetPointCmd.logging);
		MStatus stat;

		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDataBase");

		if(argDatabase.isFlagSet("-h"))
		{
			const char* helpMsg =
					          "\nUsage : \"TN_ExportTane -fn \"${fileName}\" (string)"
			;
			setResult(helpMsg);
			return MStatus::kSuccess;
		}
		
		stat = parseArg_(argDatabase);
		MCHECKERROR(stat, "Failed to parseArg_(pargs)");
		
		MFnDagNode shapeFn(obj_, &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");
		
		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		const std::shared_ptr<Points> pPoints = pShape->outPoints();
		if(!pPoints)
		{
			STDOUT_ERROR("Failed to get pPoints");
			return MS::kFailure;
		}
		const int npoints = pPoints->size();
		LOGGER_DEBUG("npoints = %d", npoints);
		
		if(target_ == TargetNames->count)
		{
			setResult(npoints);
		}
		else if(target_ == TargetNames->xform)
		{
			MDagPath dagPath = GetDagPath(obj_, &stat);
			MCHECKERROR(stat, "Failed to get dagPath");
			MMatrix xformMtx;
			stat = GetXformMtx(dagPath.transform(), &xformMtx);
			MDoubleArray ret;
			for(int r = 0; r < 4; ++r)
				for(int c = 0; c < 4; ++c)
					ret.append(xformMtx[r][c]);
			setResult(ret);
		}
		else
		{
			switch (mode_)
			{
				case kPid:
				{
					if(!checkPointCount_(npoints))
					{
						LOGGER_ERROR("Failed to get valid pid");
						return MS::kFailure;
					}
					if(target_ == TargetNames->pid)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::pid);
						setResult(static_cast<int>(attrArray[pid_]));
					}
					else if(target_ == TargetNames->sid)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::sid);
						setResult(static_cast<int>(attrArray[pid_]));
					}
					else if(target_ == TargetNames->renderable)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::renderable);
						setResult(static_cast<int>(attrArray[pid_]));
					}
					else if(target_ == TargetNames->pos)
					{
						const float* attrArray = pPoints->get().data()->data();
						MDoubleArray ret(3);
						ret[0] = attrArray[pid_ * 3 + 0];
						ret[1] = attrArray[pid_ * 3 + 1];
						ret[2] = attrArray[pid_ * 3 + 2];
						setResult(ret);
					}
					else if(target_ == TargetNames->eulerAngle)
					{
						const float* attrArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle).data()->data();
						MDoubleArray ret(3);
						ret[0] = attrArray[pid_ * 3 + 0];
						ret[1] = attrArray[pid_ * 3 + 1];
						ret[2] = attrArray[pid_ * 3 + 2];
						setResult(ret);
					}
					else if(target_ == TargetNames->scale)
					{
						const float* attrArray = pPoints->get<Vec3f>(sementic::attribute::scale).data()->data();
						MDoubleArray ret(3);
						ret[0] = attrArray[pid_ * 3 + 0];
						ret[1] = attrArray[pid_ * 3 + 1];
						ret[2] = attrArray[pid_ * 3 + 2];
						setResult(ret);
					}
					else if(target_ == TargetNames->uvCoord)
					{
						const float* attrArray = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord).data()->data();
						MDoubleArray ret(3);
						ret[0] = attrArray[pid_ * 3 + 0];
						ret[1] = attrArray[pid_ * 3 + 1];
						ret[2] = attrArray[pid_ * 3 + 2];
						setResult(ret);
					}
					else
					{
						LOGGER_ERROR("Failed to get valid target");
						return MS::kFailure;
					}
					
				}
				break;
				case kAll:
				{
					if(target_ == TargetNames->pid)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::pid);
						MIntArray ret(npoints);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->sid)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::sid);
						MIntArray ret(npoints);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->renderable)
					{
						const Int32Array& attrArray = pPoints->get<int>(sementic::attribute::renderable);
						MIntArray ret(npoints);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->pos)
					{
						const float* attrArray = pPoints->get().data()->data();
						MDoubleArray ret(npoints * 3);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints * 3)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->eulerAngle)
					{
						const float* attrArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle).data()->data();
						MDoubleArray ret(npoints * 3);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints * 3)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->scale)
					{
						const float* attrArray = pPoints->get<Vec3f>(sementic::attribute::scale).data()->data();
						MDoubleArray ret(npoints * 3);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints * 3)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else if(target_ == TargetNames->uvCoord)
					{
						const float* attrArray = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord).data()->data();
						MDoubleArray ret(npoints * 3);
						thrust::for_each(thrust::host,
						                 thrust::counting_iterator<size_t>(0),
						                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints * 3)),
						                 [&](const size_t i)
						                 {
							                 ret[i] = attrArray[i];
						                 });
						setResult(ret);
					}
					else
					{
						STDOUT_ERROR("Failed to get valid target");
						return MS::kFailure;
					}
				}
				break;
			}
		}
		return stat;
	}

private:
	MStatus parseArg_(const MArgDatabase& argDatabase)
	{
		DECL_LOGGER(RegisteredCmd->GetPointCmd.logging);
		MStatus stat;
		
		MString nodeName;
		stat = argDatabase.getFlagArgument("-nn", 0, nodeName);
		MCHECKERROR(stat, "Failed to get -nn argument");
		
		MDagPath dagPath;
		stat = GetDagPathByName(nodeName, &dagPath);
		MCHECKERROR(stat, "Failed to get dagPath by nodeName, %s", nodeName.asChar());
		
		obj_ = dagPath.node(&stat);
		MCHECKERROR(stat, "Failed to get MObject");
		
		MString target;
		stat = argDatabase.getFlagArgument("-tg", 0, target);
		MCHECKERROR(stat, "Failed to get -tg argument");
		target_ = target.asChar();
		
		if(argDatabase.isFlagSet("-i", &stat))
		{
			mode_ = kPid;
			stat = argDatabase.getFlagArgument("-i", 0, pid_);
			MCHECKERROR(stat, "Failed to get -i argument");
		}
		
		return stat;
	}
	
	bool checkPointCount_(const int npoints)
	{
		if((0 <= pid_ && pid_ < npoints))
			return true;
		return false;
	}
	
};
StaticData<GetPointCmd ::_TargetNames>   GetPointCmd::TargetNames;
MString                                  GetPointCmd::Cmd(RegisteredCmd->GetPointCmd.name);
NAMESPACE_BASE_END
