#pragma once
#include "maya/pch.h"
#include "baseMaya/util.h"

///------------------------------------------------------------------ CreateNodeCmd
NAMESPACE_BASE_BEGIN
inline MStatus CreateTN_Nodes(const MString nodeType,
                              const MString nodeName,
                              MObject* xformObj,
                              MObject* shapeObj)
{
	static std::mt19937 rng(1024);
	MStatus stat;
	if(nodeType == RegisteredNodes->TaneMPxShape.name)
	{
		/* create TaneMPxShape */
		*xformObj = CreateDAGNode(RegisteredNodes->TaneMPxTransform.name,
		                              MObject::kNullObj,
		                              "",
		                              false,
		                              &stat
		);
		MCHECKERROR(stat, "Failed to create %s", RegisteredNodes->TaneMPxTransform.name.asChar());
		
		*shapeObj = CreateDAGNode(RegisteredNodes->TaneMPxShape.name,
		                              *xformObj,
		                              "",
		                              false,
		                              &stat
		);
		MCHECKERROR(stat, "Failed to create %s", RegisteredNodes->TaneMPxShape.name.asChar());
		
		/* rock transform*/
		MFnDagNode dagFn(*xformObj, &stat);
		MPlug txPlug(*xformObj, dagFn.attribute("tx")); txPlug.setLocked(true);
		MPlug tyPlug(*xformObj, dagFn.attribute("ty")); tyPlug.setLocked(true);
		MPlug tzPlug(*xformObj, dagFn.attribute("tz")); tzPlug.setLocked(true);
		MPlug rxPlug(*xformObj, dagFn.attribute("rx")); rxPlug.setLocked(true);
		MPlug ryPlug(*xformObj, dagFn.attribute("ry")); ryPlug.setLocked(true);
		MPlug rzPlug(*xformObj, dagFn.attribute("rz")); rzPlug.setLocked(true);
		MPlug sxPlug(*xformObj, dagFn.attribute("sx")); sxPlug.setLocked(true);
		MPlug syPlug(*xformObj, dagFn.attribute("sy")); syPlug.setLocked(true);
		MPlug szPlug(*xformObj, dagFn.attribute("sz")); szPlug.setLocked(true);
	}
	else
	{
		MString name = nodeType;
		*xformObj = CreateDAGNode("transform",
		                          MObject::kNullObj,
		                          "",
		                          false,
		                          &stat);
		MCHECKERROR(stat, "Failed to create transform");
		
		*shapeObj = CreateDAGNode(nodeType,
		                              *xformObj,
		                              "",
		                              false,
		                              &stat);
		MCHECKERROR(stat, "Failed to create %s", nodeType.asChar());
		
		/* set NodeName */
		stat = SetNodeName(*xformObj, MString(name) + MString("#"), false);
		
		/* set solidColor randomly*/
		std::uniform_real_distribution<float> randomColor = std::uniform_real_distribution<float>(0.0f, std::nextafter(1.0f, 2.0f));
		const float r = randomColor(rng);
		const float g = randomColor(rng);
		const float b = randomColor(rng);
		
		MFnDagNode dagFn(*shapeObj, &stat);
		MPlug colorPlug(*shapeObj, dagFn.attribute("color"));
		colorPlug.child(0).setFloat(r);
		colorPlug.child(1).setFloat(g);
		colorPlug.child(2).setFloat(b);
	}
	
	///* rename node */
	if(nodeName != "")
	{
		stat = SetNodeName(*xformObj, MString(nodeName) + MString("#"), false);
	}
	
	MDagPath dagPath = GetDagPath(*shapeObj, &stat);
	MCHECKERROR(stat, "Failed to get dagPath");
	
	/* connect time1.outTime to tane.inTime */
	stat = ConnectOutTime(dagPath.node());
	MCHECKERROR(stat, "Failed to connect inTime");
	
	/* set tane initialShadingGroup */
	stat = SetInitialShadingGroup(*shapeObj);
	MCHECKERROR(stat, "Failed to set initialShadingGroup");
	
	/* set renderman Visibility */
	//stat = SetRendermanVisibility(shapeObj);
	//MCHECKERROR(stat, "Failed to connect inTime");
	
	/* add renderman procedural command */
	//stat = SetRendermanPreShapeScript(shapeObj, "TaneUtil.TaneMPxSurfaceShapeRenderCmd()");
	//if(!stat) return MS::kFailure;
	
	
	return stat;
}
NAMESPACE_BASE_END