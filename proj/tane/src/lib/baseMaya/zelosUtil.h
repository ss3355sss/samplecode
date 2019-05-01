#pragma once
#include "maya/pch.h"

#include <ZelosBase.h>

NAMESPACE_BASE_BEGIN
template<typename T>
inline Vec3<T> GetVec3(const Zelos::ZVector& v)
{
	return Vec3<T>(static_cast<T>(v[0]), static_cast<T>(v[1]), static_cast<T>(v[1]));
}
template<typename T>
inline Point3<T> GetPoint3(const Zelos::ZVector& v)
{
	return GetVec3<T>(v);
}


inline bool GetWorldMatrix(const MObject& dagNodeObj, MMatrix& worldMat)
{
	MStatus status = MS::kSuccess;
	
	MFnDagNode dagFn(dagNodeObj, &status);
	if(status != MS::kSuccess)
	{ return false; } // Don't print any error messages here!
	
	MDagPath dagPath;
	status = dagFn.getPath(dagPath);
	if(status != MS::kSuccess)
	{ return false; } // Don't print any error messages here!
	
	worldMat = dagPath.inclusiveMatrix();
	
	return true;
}

inline void ApplyXForm(const MMatrix& M, const MPoint& p, MPoint& q)
{
	const double (* matrix)[4] = M.matrix;
	const double& x = p.x, & y = p.y, & z = p.z;
	
	q.x = matrix[0][0] * x + matrix[1][0] * y + matrix[2][0] * z + matrix[3][0];
	q.y = matrix[0][1] * x + matrix[1][1] * y + matrix[2][1] * z + matrix[3][1];
	q.z = matrix[0][2] * x + matrix[1][2] * y + matrix[2][2] * z + matrix[3][2];
}

inline void Copy(Zelos::ZVectorArray& to, const MPointArray& from)
{
	const unsigned int n = from.length();
	if(!n)
	{
		to.clear();
		return;
	}
	to.setLength(n);
	for (unsigned int i = 0; i < n; ++i)
	{
		Zelos::ZVector& q = to[i];
		const MPoint  & p = from[i];
		q.x = (float) p.x;
		q.y = (float) p.y;
		q.z = (float) p.z;
	}
}
inline MStatus Convert(Zelos::ZTriMesh& mesh, MObject& meshObj, bool vPosOnly, const char* uvSetName = nullptr)
{
	MStatus status      = MS::kSuccess;
	
	if(!vPosOnly)
	{
		mesh.reset();
	}
	
	MFnMesh meshFn(meshObj, &status);
	if(status != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get MFnMesh.");
		mesh.reset();
		return MS::kFailure;
	}
	
	MItMeshVertex vItr(meshObj, &status);
	if(status != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get MItMeshVertex.");
		mesh.reset();
		return MS::kFailure;
	}
	
	MItMeshPolygon fItr(meshObj, &status);
	if(status != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get MItMeshPolygon.");
		mesh.reset();
		return MS::kFailure;
	}
	
	const int numVertices = meshFn.numVertices();
	
	//////////////////////
	// vertex positions //
	int         vIdx = 0;
	MPoint      localPos;
	MMatrix     localToWorld;
	MPointArray vP(numVertices); // vertex positions
	
	if(GetWorldMatrix(meshObj, localToWorld))
	{
		for (vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx)
		{
			localPos = vItr.position(MSpace::kObject);
			ApplyXForm(localToWorld, localPos, vP[vIdx]);
		}
	}
	else
	{
		for (vItr.reset(); !vItr.isDone(); vItr.next(), ++vIdx)
		{
			vP[vIdx] = vItr.position(MSpace::kWorld);
		}
	}
	Copy(mesh.p, vP); // mesh.p <- vP
	
	if(vPosOnly)
	{
		return MS::kSuccess;
	}
	
	/////////////////
	// UV-set name //
	
	bool    toConvertUV = true;
	MString     uvSetNameStr;
	MFloatArray vU, vV;
	
	if(!meshFn.numUVSets() || !meshFn.numUVs())
	{
		toConvertUV = false;
	}
	else
	{
		MString inUVSetName(uvSetName);
		
		if(inUVSetName.length() == 0)
		{
			toConvertUV = false;
		}
		else if(inUVSetName == MString("currentUVSet"))
		{
			uvSetNameStr = meshFn.currentUVSetName();
		}
		else
		{
			MStringArray uvSetNames;
			meshFn.getUVSetNames(uvSetNames);
			const int numUVSets = (int) uvSetNames.length();
			
			FOR(i, 0, numUVSets)
			{
				if(inUVSetName == uvSetNames[i])
				{
					uvSetNameStr = inUVSetName;
					break;
				}
			}
		}
	}
	
	if(toConvertUV)
	{
		if(!meshFn.getUVs(vU, vV, &uvSetNameStr))
		{ toConvertUV = false; }
	}
	
	//////////////////////
	// triangle indices //
	
	Zelos::ZInt3Array& v012 = mesh.v012;
	v012.reserve(meshFn.numPolygons() * 2);
	
	Zelos::ZPointArray& uv = mesh.uv;
	uv.reserve(meshFn.numPolygons() * 3);
	
	if(toConvertUV)
	{
		MIntArray vList, uvIndices;
		
		for (fItr.reset(); !fItr.isDone(); fItr.next())
		{
			fItr.getVertices(vList);
			const int vCount = (int) vList.length();
			
			if(vCount < 3) // invalid case
			{
				continue;
			}
			
			uvIndices.setLength(vCount);
			FOR(i, 0, vCount)
			{ fItr.getUVIndex(i, uvIndices[i], &uvSetNameStr); }
			
			FOR(i, 0, vCount - 2)
			{
				v012.push_back(Zelos::ZInt3(vList[0], vList[i + 1], vList[i + 2]));
				
				uv.push_back(Zelos::ZPoint(vU[uvIndices[0]], vV[uvIndices[0]], 0.f));
				uv.push_back(Zelos::ZPoint(vU[uvIndices[i + 1]], vV[uvIndices[i + 1]], 0.f));
				uv.push_back(Zelos::ZPoint(vU[uvIndices[i + 2]], vV[uvIndices[i + 2]], 0.f));
			}
		}
	}
	else
	{
		MIntArray vList;
		
		for (fItr.reset(); !fItr.isDone(); fItr.next())
		{
			fItr.getVertices(vList);
			const int vCount = (int) vList.length();
			
			if(vCount < 3) // invalid case
			{
				continue;
			}
			
			FOR(i, 0, vCount - 2)
			{
				v012.push_back(Zelos::ZInt3(vList[0], vList[i + 1], vList[i + 2]));
			}
		}
	}
	
	// This code may generate per frame different connections when the mesh is deforming.
	//MIntArray triangleCounts, triangleVertices;
	//meshFn.getTriangles( triangleCounts, triangleVertices );
	//const int numTriangles = (int)(triangleVertices.length()/3);
	//mesh.v012.setLength( numTriangles );
	//int idx = 0;
	//FOR( i, 0, numTriangles )
	//{
	//	ZInt3& v012 = mesh.v012[i];
	//	v012[0] = triangleVertices[idx++];
	//	v012[1] = triangleVertices[idx++];
	//	v012[2] = triangleVertices[idx++];
	//}
	
	return MS::kSuccess;
}


NAMESPACE_BASE_END
