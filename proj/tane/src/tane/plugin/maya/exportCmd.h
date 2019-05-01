#pragma once

#include "maya/pch.h"

#include <boost/filesystem.hpp>

#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/stageCache.h>
#include <pxr/usd/usd/stageCacheContext.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/pointInstancer.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/kind/api.h>
#include <pxr/usd/kind/registry.h>
#include <thrust/system/omp/detail/par.h>

#include "base/fileUtil.h"
#include "base/usdUtil.h"
#include "base/usdReader.h"

#include "baseMaya/usdUtil.h"

#include "tane/pipelineUtil.h"

#include "maya/abcProxyMPxShape.h"
#include "maya/taneMPxShape.h"


NAMESPACE_BASE_BEGIN
class ExportTaneCmd : public MPxCommand
{
public:
	typedef MDagPathMap<pxr::UsdStageRefPtr> StageMap;
public:
	static MString Cmd;

private:
    MString     fileName_           = "";
    bool        isDynamic_          = false;
    double      startFrame_         = 0.0;
    double      endFrame_           = 0.0;
    double      fps_                = 24.0;
    bool        exportCustomData_   = false;
    MString     fileFormat_         = "usdc";
    
    StageMap    stageMap_           = StageMap();
public:
	static void* Creator()
	{
		return new ExportTaneCmd;
	}
	static MSyntax NewSyntax()
    {

        MSyntax syntax;
	    syntax.addFlag("-h" ,  "-help");
	    
	    syntax.addFlag("-fn", "-fileName"        , MSyntax::kString);
	    syntax.addFlag("-nn", "-nodeName"        , MSyntax::kString);
	    syntax.addFlag("-ec", "-exportCustomData", MSyntax::kBoolean);

        syntax.addFlag("-sf", "-startFrame"      , MSyntax::kDouble);
        syntax.addFlag("-ef", "-endFrame"        , MSyntax::kDouble);
	    syntax.addFlag("-f" , "-framePerSecond"  , MSyntax::kDouble);
	
	    syntax.addFlag("-fm" , "-fileFormat"     , MSyntax::kString);
	    
	    return syntax;
    }

public:
    ExportTaneCmd()
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);

    }

    virtual ~ExportTaneCmd()
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);
    }

public:
    virtual bool isUndoable() const
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);
        return false;
    }

    virtual MStatus doIt(const MArgList &args)
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);
        MStatus stat;
	
	    MArgDatabase argDatabase(syntax(), args, &stat);
	    MCHECKERROR(stat, "Failed to get argDataBase");
	    
	    if(argDatabase.isFlagSet("-h"))
	    {
		    const char* helpMsg =
				    "\nUsage : \"TN_ExportTane -fn \"${fileName}\" (string)"
				    "\n                        -nn \"${shape}\" (string)"
				    "\n                        [-ec \"${value}\"] (bool)"
				    "\n                        [-sf \"${startFrame}\" -ef \"${endFrame}\" -f \"${fps}\"] (double)"
				    "\n                        [-fm \"${fileFormat}\"] (string)"
		    ;
		    setResult(helpMsg);
		    return MStatus::kSuccess;
	    }
	    
	    stat = parseArg_(argDatabase);
        MCHECKERROR(stat, "Failed to parse args");

        MStringArray ret;
        for (StageMap::iterator iter = stageMap_.begin(); iter != stageMap_.end(); ++iter)
        {
        	const MDagPath& dagPath = iter->first;
        	LOGGER_INFO("fullPath = %s", dagPath.fullPathName().asChar());
            MFnDagNode shapeFn(iter->first.node());
            TaneMPxShape *pShape = dynamic_cast<TaneMPxShape *>(shapeFn.userNode());
            if (!pShape)
            {
                STDOUT_ERROR("Failed to get pShape");
                return MS::kFailure;
            }
            std::shared_ptr<Message> pMsg = pShape->outMessage();
            if (!pMsg)
            {
                STDOUT_ERROR("Failed to get pMsg");
                return MS::kFailure;
            }
            std::shared_ptr<SourceManager> pSourceMgr = pShape->outSources();
            if (!pSourceMgr)
            {
                STDOUT_ERROR("Failed to get pSourceMgr");
                return MS::kFailure;
            }
            
            pxr::UsdStageRefPtr pStage = iter->second;
            const pxr::UsdPrim &rootPrim = pStage->GetDefaultPrim();
            LOGGER_DEBUG("rootPrim = %s", rootPrim.GetName().GetText());

            /* write pointInstancer */
            pxr::UsdPrim instancerPrim = pStage->DefinePrim(rootPrim.GetPath().AppendChild(pxr::TfToken("scatter")), pxr::TfToken("PointInstancer"));
            pxr::UsdGeomPointInstancer instancerGeomSchema(instancerPrim);
            {
	            const UsdAttribute& pidUsdAttr         = instancerGeomSchema.CreateIdsAttr();
	            const UsdAttribute& sidUsdAttr         = instancerGeomSchema.CreateProtoIndicesAttr();
	            const UsdAttribute& invisUsdAttr       = instancerGeomSchema.CreateInvisibleIdsAttr();

	            const UsdAttribute& positionUsdAttr    = instancerGeomSchema.CreatePositionsAttr();
	            const UsdAttribute& orientationUsdAttr = instancerGeomSchema.CreateOrientationsAttr();
	            const UsdAttribute& scaleUsdAttr       = instancerGeomSchema.CreateScalesAttr();
	
	            if(exportCustomData_)
	            {
		            const UsdAttribute& localAxisUsdAttr = CreateOrGetAttribute(instancerPrim, "localAxis", SdfValueTypeNames->Matrix3dArray, true);
		
		            const UsdAttribute& rootSUsdAttr = CreateOrGetAttribute(instancerPrim, "rootS", SdfValueTypeNames->FloatArray, true);
		            const UsdAttribute& rootTUsdAttr = CreateOrGetAttribute(instancerPrim, "rootT", SdfValueTypeNames->FloatArray, true);
		            const UsdAttribute& rootWUsdAttr = CreateOrGetAttribute(instancerPrim, "rootW", SdfValueTypeNames->FloatArray, true);
		
		            const UsdAttribute& radUsdAttr = CreateOrGetAttribute(instancerPrim, "radius", SdfValueTypeNames->FloatArray, true);
	            }
	            
	            LOGGER_DEBUG("write point instancer data");
                {
                    if (isDynamic_)
                    {
	                    LOGGER_DEBUG("startFrame = %5.2f", startFrame_);
                        LOGGER_DEBUG("endFrame   = %5.2f", endFrame_);
                        for (double frame = startFrame_; frame <= endFrame_; frame += 1.0)
                        {
                            /* shift frame */
	                        MGlobal::viewFrame(frame);
	                        
	                        std::shared_ptr<Points> pPoints = pShape->outPoints();
	                        if (!pPoints)
	                        {
		                        STDOUT_ERROR("Failed to get pPoints");
		                        return MS::kFailure;
	                        }
	                        stat = setPointInstancerAttr_(instancerPrim, pPoints, pxr::UsdTimeCode(frame));
                        }
                    }
                    else
                    {
	                    std::shared_ptr<Points> pPoints = pShape->outPoints();
	                    if (!pPoints)
	                    {
		                    STDOUT_ERROR("Failed to get pPoints");
		                    return MS::kFailure;
	                    }
	                    stat = setPointInstancerAttr_(instancerPrim, pPoints);
                    }
                }
            }

            /* write protoTypes */
            const std::string &identifier = pStage->GetRootLayer()->GetIdentifier();
            LOGGER_DEBUG("identifier = %s", identifier.c_str());

            pxr::SdfPathVector protoTargets;
            pxr::UsdPrim protoTypesPrim = pStage->DefinePrim(instancerPrim.GetPath().AppendChild(pxr::TfToken("Prototypes")));
            {
            	
            	Int32Array sourceIDs = pSourceMgr->sourceIDs();
	
	            LOGGER_DEBUG("pSourceMgr->size() = %d", pSourceMgr->size());
	            LOGGER_DEBUG("sourceIDs.size() = %d", sourceIDs.size());
            	if(!(sourceIDs.size() > 0))
	            {
		            STDOUT_ERROR("Failed to get valid sourceIDs");
		            return MS::kFailure;
	            }
	            
            	
            	int lastId = sourceIDs[sourceIDs.size() - 1];
            	
            	LOGGER_DEBUG("lastId = %d", lastId);
            	for(int protoId = 0; protoId <= lastId; ++protoId)
	            {
		            SourceManager::iterator iter = pSourceMgr->find(protoId);
		            pxr::UsdPrim protoTargetPrim;
	            	if(iter == pSourceMgr->end())
		            {
			            protoTargetPrim = makeDummyProtoType(protoId, pStage, protoTypesPrim);
		            }
		            else
		            {
			            const int srcId = iter->first;
			            LOGGER_DEBUG("srcId = %d", srcId);
			            const std::shared_ptr<Source> pSource = iter->second;
			            if (!pSource)
			            {
				            STDOUT_ERROR("Failed to get pSource[%d]", srcId);
				            return MS::kFailure;
			            }
			
			            MString renderFile;
			            const MDagPath srcDagPath = pSource->objPath();
			            LOGGER_DEBUG("source[%d].dagPath = %s", srcId, srcDagPath.fullPathName().asChar());
			
			            const MObject &srcObj = srcDagPath.node(&stat);
			            MFnDagNode srcFn(srcObj, &stat);

			            LOGGER_DEBUG("make protoType");
			            pxr::UsdPrim srcPrim;
			            const MTypeId typeId = srcFn.typeId(&stat);
			            if (typeId == RegisteredNodes->AbcProxyMPxShape.id)
			            {
				            srcPrim = makeAbcProtoType(srcId, pStage, srcObj, protoTypesPrim);
			            }
			            else if (typeId == RegisteredNodes->Abc2ProxyMPxShape.id)
			            {
				            srcPrim = makeDummyProtoType(srcId, pStage, protoTypesPrim);
			            }
			            else if (typeId == RegisteredNodes->UsdProxyMPxShape.id)
			            {
				            srcPrim = makeUsdProtoType(srcId, pStage, srcObj, protoTypesPrim);
			            }
			            else if (typeId == RegisteredNodes->VdbProxyMPxShape.id)
			            {
				            srcPrim = makeVdbProtoType(srcId, pStage, srcObj, protoTypesPrim);
			            }
			            else
			            {
				            STDOUT_ERROR("Failed to get supporting proxy type");
				            return MS::kFailure;
			            }
			            
			            if (!srcPrim)
			            {
				            STDOUT_ERROR("Failed to make proto prim")
				            return MS::kFailure;
			            }
			
			            LOGGER_DEBUG("make xformMtx");
			            Mat4x4f xformMtx = Mat4x4f::Identity();
			            stat = GetXformMtx(srcDagPath.transform(&stat), &xformMtx);
			            MCHECKERROR(stat, "Failed to get xform, %s", srcDagPath.fullPathName().asChar());
			
			
			            LOGGER_DEBUG("make xformSchema");
			            UsdGeomMesh xformSchema(srcPrim);
			            GfMatrix4d xformGfMtx = {
					            xformMtx(0, 0), xformMtx(1, 0), xformMtx(2, 0), xformMtx(3, 0),
					            xformMtx(0, 1), xformMtx(1, 1), xformMtx(2, 1), xformMtx(3, 1),
					            xformMtx(0, 2), xformMtx(1, 2), xformMtx(2, 2), xformMtx(3, 2),
					            xformMtx(0, 3), xformMtx(1, 3), xformMtx(2, 3), xformMtx(3, 3),
			            };
			
			            LOGGER_DEBUG("set xformSchema");
			            xformSchema.MakeMatrixXform().Set(xformGfMtx);

			            LOGGER_DEBUG("set protoTargetPrim");
			            protoTargetPrim = srcPrim;
		            }
		            protoTargets.push_back(protoTargetPrim.GetPath());
	            }
                if (!instancerGeomSchema.GetPrototypesRel().SetTargets(protoTargets))
                {
                    STDOUT_ERROR("Failed to set proto targets");
                    return MS::kFailure;
                }

            }
            pStage->Save();
        	ret.append(identifier.c_str());
        }
        
        setResult(ret);
        return stat;
    }

private:
    MStatus parseArg_(const MArgDatabase& argDatabase)
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);
        MStatus stat;

	    stat = argDatabase.getFlagArgument("-fn", 0, fileName_);
	    MCHECKERROR(stat, "Failed to get -fn argument");
	
	    if (argDatabase.isFlagSet("-ec"))
	    {
		    stat = argDatabase.getFlagArgument("-ec", 0, exportCustomData_);
		    MCHECKERROR(stat, "Failed to get -ea argument");
	    }
        if (argDatabase.isFlagSet("-sf") && argDatabase.isFlagSet("-ef") && argDatabase.isFlagSet("-f"))
        {
            isDynamic_ = true;
            stat = argDatabase.getFlagArgument("-sf", 0, startFrame_);
            MCHECKERROR(stat, "Failed to get -sf argument");
            stat = argDatabase.getFlagArgument("-ef", 0, endFrame_);
            MCHECKERROR(stat, "Failed to get -ef argument");
            stat = argDatabase.getFlagArgument("-f", 0, fps_);
            MCHECKERROR(stat, "Failed to get -f argument");
        }
        else
        {
            isDynamic_ = false;
        }
	    if (argDatabase.isFlagSet("-fm"))
	    {
		    stat = argDatabase.getFlagArgument("-fm", 0, fileFormat_);
		    MCHECKERROR(stat, "Failed to get -fm argument");
	    }

	    MString nodeName;
	    stat = argDatabase.getFlagArgument("-nn", 0, nodeName);
	    MCHECKERROR(stat, "Failed to get -nn argument");
	
	    MDagPath dagPath;
	    stat = GetDagPathByName(nodeName, &dagPath);
	    MCHECKERROR(stat, "Failed to get dagPath by nodeName");
	
	    MObject xformObj = dagPath.transform();
	    MObject shapeObj = dagPath.node();
	
	    pxr::UsdStageRefPtr pStage = openUsdFile_(xformObj, fileName_);
	    if (!pStage)
	    {
		    STDOUT_ERROR("Failed to get valid pStage");
		    return MS::kFailure;
	    }
	    bool success = stageMap_.insert(StageMap::value_type(dagPath, pStage)).second;
	    if (!success)
	    {
		    STDOUT_ERROR("Failed to insert stage");
		    return MS::kFailure;
	    }
	    
        if (stageMap_.empty())
        {
            STDOUT_ERROR("Failed to get TaneMPxShape nodes!");
            return MS::kFailure;
        }

        return stat;
    }
    
	pxr::UsdStageRefPtr openUsdFile_(const MObject &xformObj, const MString& fileName)
	{
		MStatus stat;


		/* make initial layer */
		pxr::SdfLayerRefPtr pRootLayer = pxr::SdfLayer::FindOrOpen(fileName.asChar());
		if (!pRootLayer)
		{
			pxr::SdfLayer::FileFormatArguments fileFormatArguments = {
					{ "format", fileFormat_.asChar() }
			};
			pRootLayer = pxr::SdfLayer::CreateNew(fileName.asChar(), fileName.asChar(), fileFormatArguments);
		}
		else
		{
			STDOUT_WARRNING("%s is already exists", fileName.asChar());
			pRootLayer->Clear();
		}
		pxr::UsdStageRefPtr pStage = pxr::UsdStage::Open(fileName.asChar(), pxr::UsdStage::LoadNone);
		if (!pStage)
		{
			STDOUT_ERROR("Failed to pStage, %s", fileName.asChar());
			return nullptr;
		}
		
		
		/* set stage status */
		{
			pxr::UsdGeomSetStageUpAxis(pStage, pxr::UsdGeomTokens->y);
			if (isDynamic_)
			{
				pStage->SetStartTimeCode(startFrame_);
				pStage->SetEndTimeCode(endFrame_);
				pStage->SetFramesPerSecond(fps_);
			}
		}
		
		
		
		/* set root prim */
		MFnDagNode xformFn(xformObj, &stat);
		const std::string xformName = xformFn.name().asChar();
		pxr::UsdPrim rootPrim = pStage->DefinePrim(pxr::SdfPath("/" + std::string(xformName)), pxr::TfToken("Xform"));
		pStage->SetDefaultPrim(rootPrim);
		
		pxr::UsdModelAPI modelAPI(rootPrim);
		modelAPI.SetKind(pxr::KindTokens->component);
		modelAPI.SetAssetName(xformName);
		
		return pStage;
	}
	
	pxr::UsdPrim makeAbcProtoType(const int id, const pxr::UsdStageRefPtr &pStage, const MObject& shapeObj, const pxr::UsdPrim &parent)
    {
        DECL_LOGGER(logging::maya->ExportTaneCmd);

	    MStatus stat;
	    MFnDagNode shapeFn(shapeObj);
	
	    const MString nodeName = shapeFn.name(&stat);
	    if(stat != MS::kSuccess)
	    {
		    STDOUT_ERROR("Failed to get nodeName");
		    return pxr::UsdPrim();
	    }
	    const MString renderFile = shapeFn.findPlug(nodeAttr::abc->renderFile, &stat).asString();
	    if(stat != MS::kSuccess)
	    {
		    STDOUT_ERROR("Failed to get %s", nodeAttr::abc->renderFile);
		    return pxr::UsdPrim();
	    }
	    const MString renderObjectPath = shapeFn.findPlug(nodeAttr::abc->renderObjectPath, &stat).asString();
	    if(stat != MS::kSuccess)
	    {
		    STDOUT_ERROR("Failed to get %s", nodeAttr::abc->renderObjectPath);
		    return pxr::UsdPrim();
	    }
	    const short animCycle = shapeFn.findPlug(nodeAttr::abc->animCycle, &stat).asShort();
	    if(stat != MS::kSuccess)
	    {
		    STDOUT_ERROR("Failed to get %s", nodeAttr::abc->animCycle);
		    return pxr::UsdPrim();
	    }
	
	    SdfLayerOffset layerOffset(0.0);
	    if(animCycle == mode::animCycle::looped)
	    {
		    const double frameOffset = shapeFn.findPlug(nodeAttr::usd->frameOffset, &stat).asDouble();
		    if(stat != MS::kSuccess)
		    {
			    STDOUT_ERROR("Failed to get %s", nodeAttr::usd->frameOffset);
			    return pxr::UsdPrim();
		    }
		    layerOffset = SdfLayerOffset(frameOffset);
	    }
	
	    /* create usdPrim */
	    pxr::UsdPrim prim = pStage->DefinePrim(parent.GetPath().AppendChild(pxr::TfToken(nodeName.asChar())));
	
	    /* set sdfReference */
	    UsdReferences usdReferences = prim.GetReferences();
	    if(renderObjectPath.length() > 1 /* skip default value, "/" */)
	    {
		    usdReferences.AddReference(renderFile.asChar(), SdfPath(renderObjectPath.asChar()), layerOffset);
	    }
	    else
	    {
		    usdReferences.AddReference(renderFile.asChar(), layerOffset);
	    }

	    /* set custom data */
	    stat = setCustomData_(nodeAttr::abc->data(), shapeObj, prim);
	    if (stat != MS::kSuccess)
	    {
		    STDOUT_ERROR("Failed to write attribute");
		    return pxr::UsdPrim();
	    }
	    return prim;
	}
	pxr::UsdPrim makeUsdProtoType(const int id, const pxr::UsdStageRefPtr& pStage, const MObject& shapeObj, const pxr::UsdPrim& parent)
	{
		DECL_LOGGER(logging::maya->ExportTaneCmd);
		
		MStatus stat;
		MFnDagNode shapeFn(shapeObj);
		
		const MString nodeName = shapeFn.name(&stat);
		if(stat != MS::kSuccess)
		{
			STDOUT_ERROR("Failed to get nodeName");
			return pxr::UsdPrim();
		}
		const MString renderFile = shapeFn.findPlug(nodeAttr::usd->renderFile, &stat).asString();
		if(stat != MS::kSuccess)
		{
			STDOUT_ERROR("Failed to get %s", nodeAttr::usd->renderFile);
			return pxr::UsdPrim();
		}
		const MString primPath = shapeFn.findPlug(nodeAttr::usd->primPath, &stat).asString();
		if(stat != MS::kSuccess)
		{
			STDOUT_ERROR("Failed to get %s", nodeAttr::usd->primPath);
			return pxr::UsdPrim();
		}
		const short animCycle = shapeFn.findPlug(nodeAttr::usd->animCycle, &stat).asShort();
		if(stat != MS::kSuccess)
		{
			STDOUT_ERROR("Failed to get %s", nodeAttr::usd->animCycle);
			return pxr::UsdPrim();
		}
		
		SdfLayerOffset layerOffset(0.0);
		if(animCycle == mode::animCycle::looped)
		{
			const double frameOffset = shapeFn.findPlug(nodeAttr::usd->frameOffset, &stat).asDouble();
			if(stat != MS::kSuccess)
			{
				STDOUT_ERROR("Failed to get %s", nodeAttr::usd->frameOffset);
				return pxr::UsdPrim();
			}
			layerOffset = SdfLayerOffset(frameOffset);
		}
		
		/* set sdfReference */
		std::set<std::string>   variantSetNames   = GetVariantSetNamesForStageCache(shapeFn);
		VariantSelects          variantSelects    = GetVariantSelects(variantSetNames, shapeFn);
		
		/* create usdPrim */
		pxr::UsdPrim prim = pStage->DefinePrim(parent.GetPath().AppendChild(pxr::TfToken(nodeName.asChar())));
		
		/* set sdfReference */
		/* TODO::Check if sdf reference exists */
		UsdReferences usdReferences = prim.GetReferences();
		if(primPath.length() > 0)
		{
			usdReferences.AddReference(renderFile.asChar(), SdfPath(primPath.asChar()), layerOffset);
		}
		else
		{
			usdReferences.AddReference(renderFile.asChar(), layerOffset);
		}
		
		/* set variant selections */
		if(!variantSelects.empty())
		{
			for(VariantSelects::const_iterator iter = variantSelects.cbegin(); iter != variantSelects.cend(); ++iter)
			{
				prim.GetVariantSet(iter->first).SetVariantSelection(iter->second);
			}
		}
		
		/* set custom data */
		stat = setCustomData_(nodeAttr::usd->data(), shapeObj, prim);
		if (stat != MS::kSuccess)
		{
			STDOUT_ERROR("Failed to write attribute");
			return pxr::UsdPrim();
		}
		return prim;
	}
	pxr::UsdPrim makeVdbProtoType(const int id, const pxr::UsdStageRefPtr& pStage, const MObject& shapeObj, const pxr::UsdPrim& parent)
	{
		std::string name = "dummy_" + std::to_string(id);
		pxr::UsdPrim dummy = pStage->DefinePrim(parent.GetPath().AppendChild(pxr::TfToken(name)), pxr::TfToken("Xform"));
		return dummy;
	}
	pxr::UsdPrim makeDummyProtoType(const int id, const pxr::UsdStageRefPtr& pStage, const pxr::UsdPrim& parent)
	{
		std::string name = "dummy_" + std::to_string(id);
		pxr::UsdPrim dummy = pStage->DefinePrim(parent.GetPath().AppendChild(pxr::TfToken(name)), pxr::TfToken("Xform"));
		return dummy;
	}
	MStatus setCustomData_(const nodeAttr::container& data, const MObject& shapeObj, const UsdPrim& prim)
	{
		DECL_LOGGER(logging::maya->ExportTaneCmd);
		MStatus stat;
		MFnDagNode shapeFn(shapeObj, &stat);
		
		for(const nodeAttr::value_type& attrname : data)
		{
			LOGGER_DEBUG("attrname = %s", attrname);
			if(!shapeFn.hasAttribute(attrname, &stat))
				continue;
			
			MPlug                 plug      = shapeFn.findPlug(attrname, &stat);
			pxr::SdfValueTypeName valueType = GetUsdValueType(plug);
			LOGGER_DEBUG("attrname = %s, [%s]", attrname, valueType.GetCPPTypeName().c_str());

			if(!valueType)
			{
				STDOUT_ERROR("Failed to get valid valueType");
				continue;
			}
			
			if(valueType == pxr::SdfValueTypeNames->String)
			{
				LOGGER_DEBUG("%s = %s", attrname, plug.asString().asChar());
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(plug.asString().asChar()));
			}
			else if(valueType == pxr::SdfValueTypeNames->Matrix4d)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->StringArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->DoubleArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->FloatArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->IntArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->Point3dArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->Vector3dArray)
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			else if(valueType == pxr::SdfValueTypeNames->Bool)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(plug.asBool()));
			}
			else if(valueType == pxr::SdfValueTypeNames->Int)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(plug.asInt()));
			}
			else if(valueType == pxr::SdfValueTypeNames->Int2)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec2i(plug.child(0).asInt(),
				                                                                 plug.child(1).asInt())
				));
			}
			else if(valueType == pxr::SdfValueTypeNames->Int3)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec3i(plug.child(0).asInt(),
				                                                                 plug.child(1).asInt(),
				                                                                 plug.child(2).asInt()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Float)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(plug.asFloat()));
			}
			else if(valueType == pxr::SdfValueTypeNames->Float2)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec2f(plug.child(0).asFloat(),
				                                                                 plug.child(1).asFloat()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Float3)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec3f(plug.child(0).asFloat(),
				                                                                 plug.child(1).asFloat(),
				                                                                 plug.child(2).asFloat()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Color3f)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec3f(plug.child(0).asFloat(),
				                                                                 plug.child(1).asFloat(),
				                                                                 plug.child(2).asFloat()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Double)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(plug.asDouble()));
			}
			else if(valueType == pxr::SdfValueTypeNames->Double2)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec2d(plug.child(0).asDouble(),
				                                                                 plug.child(1).asDouble()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Double3)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec3d(plug.child(0).asDouble(),
				                                                                 plug.child(1).asDouble(),
				                                                                 plug.child(2).asDouble()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Color3d)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec3d(plug.child(0).asDouble(),
				                                                                 plug.child(1).asDouble(),
				                                                                 plug.child(2).asDouble()
				)));
			}
			else if(valueType == pxr::SdfValueTypeNames->Double4)
			{
				prim.SetCustomDataByKey(pxr::TfToken(attrname), VtValue(GfVec4d(plug.child(0).asDouble(),
				                                                                 plug.child(1).asDouble(),
				                                                                 plug.child(2).asDouble(),
				                                                                 plug.child(3).asDouble()
				)));
			}
			else
			{
				STDOUT_ERROR("Failed to get supporting valueType");
				continue;
			}
			
		}
		return stat;
	}
	MStatus setPointInstancerAttr_(const pxr::UsdPrim& instancerPrim, const std::shared_ptr<Points> pPoints, const UsdTimeCode timeCode = UsdTimeCode::Default())
	{
		DECL_LOGGER(logging::maya->ExportTaneCmd);
		MStatus stat;
		const size_t npoints = pPoints->size();
		
		const IntArray&       pidArray        = pPoints->get<int>(sementic::attribute::pid);
		const IntArray&       sidArray        = pPoints->get<int>(sementic::attribute::sid);
		const IntArray&       renderableArray = pPoints->get<int>(sementic::attribute::renderable);
		
		const Point3fArray&     pointArray      = pPoints->get();
		const Vec3fArray&       eulerAngleArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		const Vec3fArray&       scaleArray      = pPoints->get<Vec3f>(sementic::attribute::scale);
		
		const Mat3x3fArray&     localAxisArray  = pPoints->get<Mat3x3f>(sementic::attribute::localAxis);
		
		const TexCoord3fArray&  uvCoordArray    = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		const FloatArray&       radArray        = pPoints->get<float>(sementic::attribute::radius);
		
		
		VtInt64Array     vtPidArray(npoints, 0);
		VtIntArray       vtSidArray(npoints, 0);
		VtInt64Array     vtInvisArray;
		
		VtVec3fArray     vtPointArray(npoints, GfVec3f(0.0f, 0.0f, 0.0f));
		VtQuathArray     vtOrientationArray(npoints, GfQuath(1.0f, 0.0f, 0.0f, 0.0f));
		VtVec3fArray     vtScaleArray(npoints, GfVec3f(1.0f, 1.0f, 1.0f));
		
		VtMatrix3dArray  vtLocalAxisArray(npoints, GfMatrix3d(1.0));
		
		VtFloatArray     vtRootSArray(npoints, 0.0f);
		VtFloatArray     vtRootTArray(npoints, 0.0f);
		VtFloatArray     vtRootWArray(npoints, 0.0f);
		
		VtFloatArray     vtRadArray(npoints, 0.0f);
		
		VtFloatArray     vtUidArray(npoints, 0.0f);
		
		bool useOpenMP = true;
#pragma omp parallel for if( useOpenMP )
		for(size_t i = 0; i < npoints; ++i)
		{
			const int&        pid        = pidArray[i];
			const int&        sid        = sidArray[i];
			const int&        renderable = renderableArray[i];
			const Point3f&    point      = pointArray[i];
			const Quatf&      orient     = EulerAngleToQuternion(eulerAngleArray[i]);
			const Vec3f&      scale      = scaleArray[i];
			const Mat3x3f&    localAxis  = localAxisArray[i];
			const TexCoord3f& uvCoord    = uvCoordArray[i];
			const float&      rad        = radArray[i];
			
			vtPidArray[i]          = pid;
			vtSidArray[i]          = sid;
			
			if(!renderable)
			{
				#pragma omp critical(push_back_invisId)
				{
					vtInvisArray.push_back(vtPidArray[i]);
				}
			}
			
			vtPointArray[i]        = GfVec3f(point[0], point[1], point[2]);
			vtOrientationArray[i]  = GfQuath(orient.w(), orient.x(), orient.y(), orient.z());
			vtScaleArray[i]        = GfVec3f(scale[0], scale[1], scale[2]);
			
			vtLocalAxisArray[i] = GfMatrix3d(localAxis(0, 0), localAxis(1, 0), localAxis(2, 0),
			                                 localAxis(0, 1), localAxis(1, 1), localAxis(2, 1),
			                                 localAxis(0, 2), localAxis(1, 2), localAxis(2, 2)
			);
			
			
			vtRootSArray[i] = uvCoord[0];
			vtRootTArray[i] = uvCoord[1];
			vtRootWArray[i] = uvCoord[2];
			
			vtRadArray[i]   = rad;
			
			vtUidArray[i]   = pid;
			
		}


		pxr::UsdGeomPointInstancer schema(instancerPrim);
		schema.GetIdsAttr()         .Set(vtPidArray,   timeCode);
		schema.GetProtoIndicesAttr().Set(vtSidArray,   timeCode);
		schema.GetInvisibleIdsAttr().Set(vtInvisArray, timeCode);
		
		schema.GetPositionsAttr()   .Set(vtPointArray,       timeCode);
		schema.GetOrientationsAttr().Set(vtOrientationArray, timeCode);
		schema.GetScalesAttr()      .Set(vtScaleArray,       timeCode);
		
		//if(exportCustomData_)
		{
			/* create GeomPrimvar */
			CreateOrGetGeomPrimvar(instancerPrim, "uids",  SdfValueTypeNames->FloatArray, UsdGeomTokens->vertex).Set(vtUidArray, timeCode);
			CreateOrGetGeomPrimvar(instancerPrim, "rootS", SdfValueTypeNames->FloatArray, UsdGeomTokens->vertex).Set(vtRootSArray, timeCode);
			CreateOrGetGeomPrimvar(instancerPrim, "rootT", SdfValueTypeNames->FloatArray, UsdGeomTokens->vertex).Set(vtRootTArray, timeCode);

			/* create Attribute*/
			CreateOrGetAttribute(instancerPrim, "localAxis", SdfValueTypeNames->Matrix3dArray).Set(vtLocalAxisArray, timeCode);
			CreateOrGetAttribute(instancerPrim, "rootS", SdfValueTypeNames->FloatArray).Set(vtRootSArray, timeCode);
			CreateOrGetAttribute(instancerPrim, "rootT", SdfValueTypeNames->FloatArray).Set(vtRootTArray, timeCode);
			CreateOrGetAttribute(instancerPrim, "rootW", SdfValueTypeNames->FloatArray).Set(vtRootWArray, timeCode);
			
			CreateOrGetAttribute(instancerPrim, "radius", SdfValueTypeNames->FloatArray).Set(vtRadArray, timeCode);
		}
		
		//for(int i = 0; i < vtPointArray.size(); ++i)
		//{
		//	cout << "vtPointArray[" << i << "] = " << vtPointArray[i][0] << ", " << vtPointArray[i][1] << ", " << vtPointArray[i][2] << endl;
		//}
		
		return stat;
	}
};

MString ExportTaneCmd::Cmd(RegisteredCmd->ExportTaneCmd.name);

NAMESPACE_BASE_END



