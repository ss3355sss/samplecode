#pragma once
#include "maya/pch.h"

#include "base/fileUtil.h"
#include "baseUsd/usdImporter.h"

#include "baseMaya/usdUtil.h"

#include "maya/createUtil.h"
#include <boost/filesystem.hpp>

NAMESPACE_BASE_BEGIN
class ImportTaneCmd : public MPxCommand
{
public:
	static MString Cmd;

private:
	short   type_     = templetType::invalid;
	MString fileName_ = "";

	MObject bodyObj_;
	
public:
	static void* Creator()
	{
		return new ImportTaneCmd;
	}
	static MSyntax NewSyntax()
    {
        MSyntax syntax;
	    syntax.addFlag("-h" , "-help");
	    syntax.addFlag("-it", "-importingType", MSyntax::kString);
	    syntax.addFlag("-fn", "-fileName",      MSyntax::kString);
	
	    syntax.addFlag("-bn", "-bodyName",      MSyntax::kString);
	    
	    return syntax;
    }

public:
	ImportTaneCmd()
    {
        DECL_LOGGER(logging::maya->ImportTaneCmd);

    }
    virtual ~ImportTaneCmd()
    {
        DECL_LOGGER(logging::maya->ImportTaneCmd);
    }

public:
    virtual bool isUndoable() const
    {
	    DECL_LOGGER(logging::maya->ImportTaneCmd);
        return false;
    }
    virtual MStatus doIt(const MArgList &args)
    {
	    DECL_LOGGER(logging::maya->ImportTaneCmd);
        MStatus stat;
	
	    MArgDatabase argDatabase(syntax(), args, &stat);
	    MCHECKERROR(stat, "Failed to get argDataBase");
	    
	    if(argDatabase.isFlagSet("-h"))
	    {
		    const char* helpMsg = "";
		    ;
		    setResult(helpMsg);
		    return MStatus::kSuccess;
	    }
	    
	    stat = parseArg_(argDatabase);
        MCHECKERROR(stat, "Failed to parse args");
        
        UsdGeomPointInstancerImporter importer;
        if(!importer.open(fileName_.asChar(), "/"))
        {
        	MString msg = "Failed to open file, " + fileName_;
        	MGlobal::displayError(msg);
        	LOGGER_ERROR("%s", msg.asChar());
        	return MS::kFailure;
        }
        const std::string name = GetFileName(fileName_.asChar());
        LOGGER_INFO("name = %s", name.c_str());

        /* create Tane Node */
	    MObject taneXformObj;
	    MObject taneShapeObj;
	    stat = CreateTN_Nodes(RegisteredNodes->TaneMPxShape.name, name.c_str(), &taneXformObj, &taneShapeObj);
	    MCHECKERROR(stat, "Failed to create Tane node");
	
	    MObject interObj;
	    switch (type_)
	    {
	    	case templetType::environment:
		    {
			    /* create Environment Node */
			    interObj = CreateDGNode(RegisteredNodes->EnvironmentMPxNode.name);
			    if(interObj.isNull())
			    {
				    STDOUT_ERROR("Failed to create interObj");
				    return MS::kFailure;
			    }
			
			    /* set Environment Params */
			    MFnDependencyNode depFn(interObj, &stat);
			    MPlug             importCachePlug = depFn.findPlug("importCache");
			    stat = importCachePlug.setBool(true);
			    MPlug cacheFilePlug = depFn.findPlug("cacheFile");
			    stat = cacheFilePlug.setString(fileName_);
			
			    if(!bodyObj_.isNull())
			    {
				    MFnDependencyNode depFn;
				
				    stat = depFn.setObject(bodyObj_);
				    MPlug outWorldMeshArrayPlug = depFn.findPlug("worldMesh", &stat);
				    MPlug outWorldMeshPlug      = outWorldMeshArrayPlug.elementByLogicalIndex(0, &stat);
				
				    stat = depFn.setObject(interObj);
				    MPlug inBodyPlug = depFn.findPlug("inBody", &stat);
				
				    stat = Connect(outWorldMeshPlug, inBodyPlug);
				    MCHECKERROR(stat, "Failed to connect wolrdMesh[0] TN_Environment.inBodyPlug");
			    }
			    else
			    {
				    STDOUT_WARRNING("Failed to get valid bodyObj");
			    }
			    
		    }
		    break;
		    case templetType::bind:
		    {
			    /* create Bind Node */
			    interObj = CreateDGNode(RegisteredNodes->BindMPxNode.name);
			    if(interObj.isNull())
			    {
				    STDOUT_ERROR("Failed to create interObj");
				    return MS::kFailure;
			    }
			    
			    /* set Bind Params */
			    MFnDependencyNode depFn(interObj, &stat);
			
			    MPlug fileNamePlug = depFn.findPlug("fileName");
			    stat = fileNamePlug.setString(fileName_);
			    
			    if(!bodyObj_.isNull())
			    {
				    stat = depFn.setObject(bodyObj_);
				    MPlug outWorldMeshArrayPlug = depFn.findPlug("worldMesh", &stat);
				    MPlug outWorldMeshPlug      = outWorldMeshArrayPlug.elementByLogicalIndex(0, &stat);
				
				    stat = depFn.setObject(interObj);
				    MPlug inBodyPlug = depFn.findPlug("inBody", &stat);
				
				    stat = Connect(outWorldMeshPlug, inBodyPlug);
				    MCHECKERROR(stat, "Failed to connect wolrdMesh[0] TN_Environment.inBodyPlug");
				    
				    /* trigger calc rest state */
				    MPlug calcRestStatePlug = depFn.findPlug("calcRestState");
				    calcRestStatePlug.setValue(true);
			    }
			    
		    }
			break;
	    }
	    stat = ConnectOutTime(interObj);
	    MCHECKERROR(stat, "Failed to connect time1.outTime");

	    stat = Connect(interObj, "output", taneShapeObj, "input");
	    MCHECKERROR(stat, "Failed to connect TN_Environment.output -> TaneMPxShape.input");
	
	    std::shared_ptr<UsdPrimArray> pProtoArray = importer.getProtoPrims();
	    LOGGER_DEBUG("pProtoArray->size() = %d", pProtoArray->size());
	    for(int protoId = 0; protoId < pProtoArray->size(); ++protoId)
	    {
		    const UsdPrim& protoPrim = (*pProtoArray)[protoId];
		    
		    SdfReferenceListOp             refsOp;
		    SdfReferenceListOp::ItemVector refs;
		    protoPrim.GetMetadata(SdfFieldKeys->References, &refsOp);
		    refsOp.ApplyOperations(&refs);
		    
		    if(refs.empty())
		    {
		    	LOGGER_WARNING("Proto[%d] does not have references", protoId);
		    	continue;
		    }
		    const SdfReference& ref       = refs[0];
		    const std::string & assetPath = ref.GetAssetPath();
		    const SdfPath     & primPath  = ref.GetPrimPath();
		
		    const std::string& protoName  = GetFileName(assetPath);
		    const std::string& extenstion = GetFileExtension(assetPath);
		
		    MObject protoXformObj;
		    MObject protoShapeObj;
		    if(extenstion == ".abc")
		    {
			    stat = CreateTN_Nodes(RegisteredNodes->AbcProxyMPxShape.name, protoName.c_str(), &protoXformObj, &protoShapeObj);
			    MCHECKERROR(stat, "Failed to create AbcProxyMPxShape");
			
			    MFnDagNode shapeFn(protoShapeObj);
			    shapeFn.findPlug(nodeAttr::abc->renderFile, &stat).setValue(assetPath.c_str());
			
			    stat = setCustomData_(nodeAttr::abc->data(), protoShapeObj, protoPrim);
			    MCHECKERROR(stat, "Failed to set CustomData");
		    }
		    else if(extenstion == ".usd")
		    {
			    stat = CreateTN_Nodes(RegisteredNodes->UsdProxyMPxShape.name, protoName.c_str(), &protoXformObj, &protoShapeObj);
			    MCHECKERROR(stat, "Failed to create UsdProxyMPxShape");
			
			    MFnDagNode shapeFn(protoShapeObj);
			    shapeFn.findPlug(nodeAttr::usd->renderFile, &stat).setValue(assetPath.c_str());
			
			    stat = setCustomData_(nodeAttr::usd->data(), protoShapeObj, protoPrim);
			    MCHECKERROR(stat, "Failed to set CustomData");
		    }
		
		    /* reparent proto to TaneMPxShape */
		    stat = ReparentNode(protoXformObj, taneXformObj);

		    /* set protoPrim Transform */
		    GfMatrix4d xformGfMtx(1.0);
			if(protoPrim.HasAttribute(TfToken("xformOp:transform")))
				protoPrim.GetAttribute(TfToken("xformOp:transform")).Get(&xformGfMtx);
		    MMatrix      xformMtx;
		    for (int r = 0; r < 4; ++r)
			    for (int c = 0; c < 4; ++c)
				    xformMtx(r, c) = xformGfMtx[r][c];
		    
		    MFnTransform xformFn(protoXformObj, &stat);
		    stat = xformFn.set(MTransformationMatrix(xformMtx));
		
		    /* connect outSource TN_Environment.inSource[%d] */
		    MFnDependencyNode depFn;
		    {
			    stat                = depFn.setObject(protoShapeObj);
			    MPlug outSourcePlug = depFn.findPlug("outSource", &stat);
			
			    stat = depFn.setObject(interObj);
			    MPlug inSourceArrayPlug = depFn.findPlug("inSource", &stat);
			    MPlug inSourcePlug      = inSourceArrayPlug.elementByLogicalIndex(protoId, &stat);
			    stat = Connect(outSourcePlug, inSourcePlug);
			    MCHECKERROR(stat, "Failed to connect outSource TN_Environment.inSource[%d]", protoId);
		    }

		    /* set display mode */
		    {
			    stat = depFn.setObject(protoShapeObj);
			    MPlug displayModePlug = depFn.findPlug("displayMode", &stat);
			    displayModePlug.setValue(mode::display::none);
		    }
	    }
        return stat;
    }
private:
    MStatus parseArg_(const MArgDatabase& argDatabase)
    {
	    DECL_LOGGER(logging::maya->ImportTaneCmd);
        MStatus stat;
	
	    stat = argDatabase.getFlagArgument("-fn", 0, fileName_);
	    MCHECKERROR(stat, "Failed to get -fn argument");

	    MString type;
	    stat = argDatabase.getFlagArgument("-it", 0, type);
	    MCHECKERROR(stat, "Failed to get -it argument");
	    
	    if(type == "kEnvironment")
	    {
	    	type_ = templetType::environment;
	    }
	    else if(type == "kBind")
	    {
		    type_ = templetType::bind;
		    MString bodyName;
		    stat = argDatabase.getFlagArgument("-bn", 0, bodyName);
		    MCHECKERROR(stat, "Failed to get -bn argument");
		
		    MDagPath dagPath;
		    stat = GetDagPathByName(bodyName, &dagPath);
		    MCHECKERROR(stat, "Failed to get dagPath by nodeName");
		
		    bodyObj_ = dagPath.node();
	    }
	    else
	    {
	    	STDOUT_ERROR("Failed to get valid import type");
	    	return MS::kFailure;
	    }
	    return stat;
    }
	MStatus setCustomData_(const nodeAttr::container& data, const MObject& shapeObj, const UsdPrim& prim)
	{
		//DECL_LOGGER(logging::maya->ImportTaneCmd);
		DECL_LOGGER(false);
		MStatus stat;
		MFnDagNode shapeFn(shapeObj, &stat);
		MCHECKERROR(stat, "Failed to create shapeFn");
		
		LOGGER_DEBUG("prim = %s", prim.GetPath().GetText());
		for(const nodeAttr::value_type& attrname : data)
		{
			if(!prim.HasCustomDataKey(TfToken(attrname)))
				continue;
			
			MPlug                  plug      = shapeFn.findPlug(attrname, &stat);
			const SdfValueTypeName valueType = GetUsdValueType(plug);
			LOGGER_DEBUG("attrname = %s, [%s]", attrname, valueType.GetCPPTypeName().c_str());
			
			if (!valueType)
			{
				STDOUT_ERROR("Failed to get valid valueType");
				continue;
			}
			
			if (valueType == pxr::SdfValueTypeNames->String)
			{
				std::string val = prim.GetCustomDataByKey(TfToken(attrname)).Get<std::string>();
				LOGGER_DEBUG("%s = %s", attrname, val.c_str());
				stat = plug.setValue(val.c_str());
			}
			else if (valueType == pxr::SdfValueTypeNames->Matrix4d)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->StringArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->DoubleArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->FloatArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->IntArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->Point3dArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->Vector3dArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if (valueType == pxr::SdfValueTypeNames->Bool)
			{
				bool val = prim.GetCustomDataByKey(TfToken(attrname)).Get<bool>();
				LOGGER_DEBUG("%s = %s", attrname, val ? "true" : "false");
				stat = plug.setValue(val);
			}
			else if (valueType == pxr::SdfValueTypeNames->Int)
			{
				int val = prim.GetCustomDataByKey(TfToken(attrname)).Get<int>();
				LOGGER_DEBUG("%s = %d", attrname, val);
				stat = plug.setValue(val);
			}
			else if (valueType == pxr::SdfValueTypeNames->Int2)
			{
				GfVec2i val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec2i>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Int3)
			{
				GfVec3i val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3i>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Float)
			{
				float val = prim.GetCustomDataByKey(TfToken(attrname)).Get<float>();
				LOGGER_DEBUG("%s = %5.2f", attrname, val);
				plug.setValue(val);
			}
			else if (valueType == pxr::SdfValueTypeNames->Float2)
			{
				GfVec3f val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3f>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Float3)
			{
				GfVec3f val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3f>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Color3f)
			{
				GfVec3f val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3f>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Double)
			{
				double val = prim.GetCustomDataByKey(TfToken(attrname)).Get<double>();
				LOGGER_DEBUG("%s = %5.2f", attrname, val);
				plug.setValue(val);
			}
			else if (valueType == pxr::SdfValueTypeNames->Double2)
			{
				GfVec2d val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec2d>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Double3)
			{
				GfVec3f val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3f>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Color3d)
			{
				GfVec3f val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec3f>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
			}
			else if (valueType == pxr::SdfValueTypeNames->Double4)
			{
				GfVec4d val = prim.GetCustomDataByKey(TfToken(attrname)).Get<GfVec4d>();
				stat = plug.child(0).setValue(val[0]);
				stat = plug.child(1).setValue(val[1]);
				stat = plug.child(2).setValue(val[2]);
				stat = plug.child(3).setValue(val[3]);
			}
			else
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
		}
		return stat;
	}
};
MString ImportTaneCmd::Cmd(RegisteredCmd->ImportTaneCmd.name);
NAMESPACE_BASE_END



