#pragma once
#include "maya/pch.h"

#include "baseMaya/util.h"

#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
class GetSourceCmd : public MPxCommand
{
public:
	struct _TargetNames
	{
		const std::string xform     = "xform";
		const std::string sourceIDs = "sourceIDs";
		
		const std::string type      = "nodeType";
		const std::string attrNames = "attrNames";
		const std::string attr      = "attr";
	};
public:
	static StaticData<_TargetNames> TargetNames;
	static MString                  Cmd;

private:
	int         sid_;
	std::string target_;
	MObject     obj_;

public:
	static void* Creator()
	{
		return new GetSourceCmd;
	}
	static MSyntax NewSyntax()
	{
		MSyntax syntax;
		syntax.addFlag("-h",  "-help");
		syntax.addFlag("-nn", "-nodeName", MSyntax::kString);
		syntax.addFlag("-tg", "-target", MSyntax::kString);
		syntax.addFlag("-i",  "-index", MSyntax::kLong);
		return syntax;
		
	}
public:
	GetSourceCmd() : sid_(-1)
	{
	}
	virtual ~GetSourceCmd()
	{
	}
public:
	virtual bool isUndoable() const
	{
		return false;
	}
	virtual MStatus doIt(const MArgList& args)
	{
		DECL_LOGGER(RegisteredCmd->GetSourceCmd.logging);
		MStatus stat;
		
		MArgDatabase argDatabase(syntax(), args, &stat);
		MCHECKERROR(stat, "Failed to get argDatabase");
		
		if(argDatabase.isFlagSet("-h"))
		{
			const char* helpMsg =
					          "\nUsage : "
			;
			setResult(helpMsg);
			return MStatus::kSuccess;
		}
		
		stat = parseArg_(argDatabase);
		MCHECKERROR(stat, "Failed to parseArg_(pargs)");
		
		MFnDagNode taneFn(obj_, &stat);
		MCHECKERROR(stat, "Failed to get shapeFn");
		
		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(taneFn.userNode(&stat));
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		const std::shared_ptr<SourceManager> pSourceMgr = pShape->outSources();
		if(!pSourceMgr)
		{
			STDOUT_ERROR("Failed to get pSourceMgr");
			return MS::kFailure;
		}
		const int nsources = pSourceMgr->size();
		if(target_ == TargetNames->xform)
		{
			MDagPath dagPath = GetDagPath(obj_, &stat);
			MCHECKERROR(stat, "Failed to get dagPath");
			MMatrix xformMtx;
			stat           = GetXformMtx(dagPath.transform(), &xformMtx);
			MDoubleArray ret;
			for (int     r = 0; r < 4; ++r)
				for (int c = 0; c < 4; ++c)
					ret.append(xformMtx[r][c]);
			setResult(ret);
		}
		else if(target_ == TargetNames->sourceIDs)
		{
			const Int32Array& sourceIDs = pSourceMgr->sourceIDs();
			MIntArray ret(sourceIDs.size(), 0);
			for (int  i                 = 0; i < sourceIDs.size(); ++i)
				ret[i] = sourceIDs[i];
			setResult(ret);
		}
		else
		{
			if(!isValidIndex_(pSourceMgr, sid_))
			{
				STDOUT_ERROR("Failed to get valid index");
				return MS::kFailure;
			}
			
			if(target_ == TargetNames->type)
			{
				MFnDagNode srcFn((*pSourceMgr)[sid_]->objPath().node(&stat));
				setResult(srcFn.typeName(&stat));
			}
			else if(target_ == TargetNames->attrNames)
			{
				MFnDagNode srcFn((*pSourceMgr)[sid_]->objPath().node(&stat));
				MString    typeName = srcFn.typeName(&stat);
				
				nodeAttr::container attrNames;
				if(typeName == MString(RegisteredNodes->AbcProxyMPxShape.name))
				{
					attrNames = nodeAttr::abc->data();
				}
				else if(typeName == MString(RegisteredNodes->Abc2ProxyMPxShape.name))
				{
					attrNames = nodeAttr::abc2->data();
				}
				else if(typeName == MString(RegisteredNodes->UsdProxyMPxShape.name))
				{
					attrNames = nodeAttr::usd->data();
				}
				else if(typeName == MString(RegisteredNodes->VdbProxyMPxShape.name))
				{
					attrNames = nodeAttr::vdb->data();
				}
				else
				{
					STDOUT_ERROR("Failed to get valid typeName");
					return MS::kFailure;
				}
				MStringArray ret;
				for (int     i      = 0; i < attrNames.size(); ++i)
					ret.append(attrNames[i]);
				setResult(ret);
			}
			else
			{
				MFnDagNode srcFn((*pSourceMgr)[sid_]->objPath().node(&stat));
				if(!srcFn.hasAttribute(target_.c_str()))
				{
					MString msg;
					msg += "attribute ";
					msg += target_.c_str();
					msg += "is not exists in ";
					msg += srcFn.partialPathName();
					STDOUT_ERROR(msg.asChar());
					MGlobal::displayError(msg);
				}
				MPlug      plug = srcFn.findPlug(target_.c_str());
				stat = setResultForTarget_(plug);
				MCHECKERROR(stat, "Failed to set result for target");
			}
		}
		return stat;
	}
private:
	MStatus parseArg_(const MArgDatabase& argDatabase)
	{
		DECL_LOGGER(RegisteredCmd->GetSourceCmd.logging);
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
			stat = argDatabase.getFlagArgument("-i", 0, sid_);
			MCHECKERROR(stat, "Failed to get -i argument");
		}
		
		return stat;
	}
	bool isValidIndex_(const std::shared_ptr<SourceManager>& pSourceMgr, const int index)
	{
		if(pSourceMgr->find(index) == pSourceMgr->end())
			return false;
		return true;
	}
	MStatus setResultForTarget_(const MPlug& plug)
	{
		MStatus       stat;
		MayaValueType valueType = GetPlugValueType(plug);
		if(valueType == MayaValueType::kInvalid)
		{
			MString msg("Failed to get valid valueType in GetPlugValueType()");
			STDOUT_ERROR(msg.asChar());
			MGlobal::displayError(msg);
			return MS::kFailure;
		}
		switch (valueType)
		{
			case MayaValueType::kString:
			{
				setResult(plug.asString());
			}
				break;
			case MayaValueType::kMatrix:
			case MayaValueType::kStringArray:
			case MayaValueType::kDoubleArray:
			case MayaValueType::kFloatArray:
			case MayaValueType::kIntArray:
			case MayaValueType::kPointArray:
			case MayaValueType::kVectorArray:
			{
				MString msg("need to be impl");
				STDOUT_ERROR(msg.asChar());
				MGlobal::displayError(msg);
				return MS::kFailure;
			}
				break;
			case MayaValueType::kBoolean    :
			{
				setResult(plug.asBool());
			}
				break;
			case MayaValueType::kByte       :
			{
				setResult(static_cast<int>(plug.asChar()));
			}
				break;
			case MayaValueType::kChar       :
			{
				setResult(static_cast<int>(plug.asChar()));
			}
				break;
			case MayaValueType::kShort      :
			{
				setResult(plug.asShort());
			}
				break;
			case MayaValueType::kInt        :
			{
				setResult(plug.asInt());
			}
				break;
			case MayaValueType::k2Short     :
			{
				MIntArray ret(2, 0);
				ret[0] = static_cast<int>(plug.child(0).asShort());
				ret[1] = static_cast<int>(plug.child(1).asShort());
				setResult(ret);
			}
				break;
			case MayaValueType::k2Int       :
			{
				MIntArray ret(2, 0);
				ret[0] = static_cast<int>(plug.child(0).asInt());
				ret[1] = static_cast<int>(plug.child(1).asInt());
				setResult(ret);
			}
				break;
			case MayaValueType::k3Short     :
			{
				MIntArray ret(3, 0);
				ret[0] = static_cast<int>(plug.child(0).asShort());
				ret[1] = static_cast<int>(plug.child(1).asShort());
				ret[2] = static_cast<int>(plug.child(2).asShort());
				setResult(ret);
			}
				break;
			case MayaValueType::k3Int       :
			{
				MIntArray ret(3, 0);
				ret[0] = static_cast<int>(plug.child(0).asInt());
				ret[1] = static_cast<int>(plug.child(1).asInt());
				ret[2] = static_cast<int>(plug.child(2).asInt());
				setResult(ret);
			}
				break;
			case MayaValueType::kFloat      :
			{
				setResult(plug.asFloat());
			}
				break;
			case MayaValueType::k2Float     :
			{
				MDoubleArray ret(2, 0.0);
				ret[0] = static_cast<double>(plug.child(0).asFloat());
				ret[1] = static_cast<double>(plug.child(1).asFloat());
				setResult(ret);
			}
				break;
			case MayaValueType::k3Float     :
			{
				MDoubleArray ret(3, 0.0);
				ret[0] = static_cast<double>(plug.child(0).asFloat());
				ret[1] = static_cast<double>(plug.child(1).asFloat());
				ret[2] = static_cast<double>(plug.child(2).asFloat());
				setResult(ret);
			}
				break;
			case MayaValueType::kDouble     :
			{
				setResult(plug.asDouble());
			}
				break;
			case MayaValueType::k2Double    :
			{
				MDoubleArray ret(2, 0.0);
				ret[0] = plug.child(0).asDouble();
				ret[1] = plug.child(1).asDouble();
				setResult(ret);
			}
				break;
			case MayaValueType::k3Double    :
			{
				MDoubleArray ret(3, 0.0);
				ret[0] = plug.child(0).asDouble();
				ret[1] = plug.child(1).asDouble();
				ret[2] = plug.child(2).asDouble();
				setResult(ret);
			}
				break;
			case MayaValueType::k4Double    :
			{
				MDoubleArray ret(4, 0.0);
				ret[0] = plug.child(0).asDouble();
				ret[1] = plug.child(1).asDouble();
				ret[2] = plug.child(2).asDouble();
				ret[3] = plug.child(3).asDouble();
				setResult(ret);
			}
				break;
			case MayaValueType::kAngle      :
			{
				MString msg("need to be impl");
				STDOUT_ERROR(msg.asChar());
				MGlobal::displayError(msg);
				return MS::kFailure;
				
			}
				break;
			case MayaValueType::kDistance   :
			{
				MString msg("need to be impl");
				STDOUT_ERROR(msg.asChar());
				MGlobal::displayError(msg);
				return MS::kFailure;
			}
				break;
			default:
				MString msg;
				msg += "Faild to valid attribute, ";
				msg += plug.partialName();
				STDOUT_ERROR(msg.asChar());
				MGlobal::displayError(msg);
				return MS::kFailure;
		}
		return stat;
	}
};

StaticData<GetSourceCmd::_TargetNames> GetSourceCmd::TargetNames;
MString                                GetSourceCmd::Cmd(RegisteredCmd->GetSourceCmd.name);
NAMESPACE_BASE_END

