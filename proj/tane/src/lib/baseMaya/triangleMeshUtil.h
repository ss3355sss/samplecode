#pragma once
#include "baseMaya/pch.h"

#include "base/triangleMesh.h"

NAMESPACE_BASE_BEGIN



template<typename T>
std::shared_ptr<TriangleMesh3<T>> CreateTriangleMesh3(const MObject& meshObj, const MString& uvSetName)
{
	MStatus stat;
	MFnMesh meshFn(meshObj, &stat);
	
	int npolygons     = meshFn.numPolygons(&stat);
	int nuvs          = meshFn.numUVs(uvSetName, &stat);
	int nvertices     = meshFn.numVertices(&stat);
	int nfacevertcies = meshFn.numFaceVertices(&stat);
	if(nuvs == 0)
	{
		STDOUT_ERROR("Failed to get valid mesh, no uv");
		return nullptr;
	}
	
	MFloatPointArray pointsArray;
	MFloatArray      uArray;
	MFloatArray      vArray;
	stat = meshFn.getPoints(pointsArray, MSpace::kWorld);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get points from MeshFn");
		return nullptr;
	}
	stat = meshFn.getUVs(uArray, vArray, &uvSetName);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get uvs from MeshFn");
		return nullptr;
	}
	
	std::shared_ptr<TriangleMesh3<T>> pTriMesh = std::make_shared<TriangleMesh3<T>>();
	
	
	
	
	return pTriMesh;
}

NAMESPACE_BASE_END
