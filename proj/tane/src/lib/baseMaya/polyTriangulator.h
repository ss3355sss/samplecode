#pragma once

#include "baseMaya/pch.h"

#include <boost/shared_array.hpp>

NAMESPACE_BASE_BEGIN
template<class INDEX_TYPE>
class PolyTriangulator
{
public:
	typedef INDEX_TYPE index_type;

private:
	// Input
	size_t                  fNumFaceCounts;
	const unsigned int*     fFaceCounts;
	const index_type*       fFaceIndices;
	const bool              fFaceIndicesCW;
	const float*            fPositions;
	const float*            fNormals;
	
	// Output
	size_t fNumTriangles;
	boost::shared_array<index_type> fTriangleIndices;

public:
	PolyTriangulator(const size_t           numFaceCounts,
	                 const unsigned int*    faceCounts,
	                 const index_type*      faceIndices,
	                 const bool             faceIndicesCW,
	                 const float*           positions,
	                 const float*           normals
	)
			: fNumFaceCounts(numFaceCounts),
			  fFaceCounts(faceCounts),
			  fFaceIndices(faceIndices),
			  fFaceIndicesCW(faceIndicesCW),
			  fPositions(positions),
			  fNormals(normals),
			  fNumTriangles(0)
	{
	}

public:
	void compute()
	{
		// empty mesh
		if (fNumFaceCounts == 0)
		{
			STDOUT_ERROR("Failed to get valid mesh");
			return;
		}
		
		// scan the polygons to estimate the buffer size
		size_t maxPoints = 0;  // the max number of vertices in one polygon
		size_t totalTriangles = 0;  // the number of triangles in the mesh
		
		for (size_t i = 0; i < fNumFaceCounts; i++)
		{
			size_t numPoints = fFaceCounts[i];
			
			// ignore degenerate polygon
			if (numPoints < 3)
			{
				continue;
			}
			
			// max number of points in a polygon
			maxPoints = std::max(numPoints, maxPoints);
			
			// the number of triangles expected in the polygon
			size_t numTriangles = numPoints - 2;
			totalTriangles += numTriangles;
		}
		
		size_t maxTriangles = maxPoints - 2;  // the max number of triangles in a polygon
		
		// allocate buffers for the worst case
		boost::shared_array<index_type> indices(new index_type[maxPoints]);
		boost::shared_array<unsigned short> triangles(new unsigned short[maxTriangles * 3]);
		boost::shared_array<float> aPosition(new float[maxPoints * 2]);
		boost::shared_array<float> aNormal;
		if (fNormals)
		{
			aNormal.reset(new float[maxPoints * 3]);
		}
		
		boost::shared_array<index_type> triangleIndices(new index_type[totalTriangles * 3]);
		
		// triangulate each polygon
		size_t triangleCount = 0;  // the triangles
		for (size_t i = 0, polyVertOffset = 0; i < fNumFaceCounts; polyVertOffset += fFaceCounts[i], i++)
		{
			size_t numPoints = fFaceCounts[i];
			
			// ignore degenerate polygon
			if (numPoints < 3)
			{
				continue;
			}
			
			// no need to triangulate a triangle
			if (numPoints == 3)
			{
				if (fFaceIndicesCW)
				{
					triangleIndices[triangleCount * 3 + 0] = fFaceIndices[polyVertOffset + 2];
					triangleIndices[triangleCount * 3 + 1] = fFaceIndices[polyVertOffset + 1];
					triangleIndices[triangleCount * 3 + 2] = fFaceIndices[polyVertOffset + 0];
				}
				else
				{
					triangleIndices[triangleCount * 3 + 0] = fFaceIndices[polyVertOffset + 0];
					triangleIndices[triangleCount * 3 + 1] = fFaceIndices[polyVertOffset + 1];
					triangleIndices[triangleCount * 3 + 2] = fFaceIndices[polyVertOffset + 2];
				}
				triangleCount++;
				continue;
			}
			
			// 1) correct the polygon winding from CW to CCW
			if (fFaceIndicesCW)
			{
				for (size_t j = 0; j < numPoints; j++)
				{
					size_t jCCW = numPoints - j - 1;
					indices[j] = fFaceIndices[polyVertOffset + jCCW];
				}
			}
			else
			{
				for (size_t j = 0; j < numPoints; j++)
				{
					indices[j] = fFaceIndices[polyVertOffset + j];
				}
			}
			
			// 2) compute the face normal (Newell's Method)
			MFloatVector faceNormal(0.0f, 0.0f, 0.0f);
			for (size_t j = 0; j < numPoints; j++)
			{
				const float *thisPoint = &fPositions[indices[j] * 3];
				const float *nextPoint = &fPositions[indices[(j + numPoints - 1) % numPoints] * 3];
				faceNormal.x += (thisPoint[1] - nextPoint[1]) * (thisPoint[2] + nextPoint[2]);
				faceNormal.y += (thisPoint[2] - nextPoint[2]) * (thisPoint[0] + nextPoint[0]);
				faceNormal.z += (thisPoint[0] - nextPoint[0]) * (thisPoint[1] + nextPoint[1]);
			}
			faceNormal.normalize();
			
			// 3) project the vertices to 2d plane by faceNormal
			float cosa, sina, cosb, sinb, cacb, sacb;
			sinb = -sqrtf(faceNormal[0] * faceNormal[0] + faceNormal[1] * faceNormal[1]);
			if (sinb < -1e-5)
			{
				cosb = faceNormal[2];
				sina = faceNormal[1] / sinb;
				cosa = -faceNormal[0] / sinb;
				
				cacb = cosa * cosb;
				sacb = sina * cosb;
			}
			else
			{
				cacb = 1.0f;
				sacb = 0.0f;
				sinb = 0.0f;
				sina = 0.0f;
				if (faceNormal[2] > 0.0f)
				{
					cosa = 1.0f;
					cosb = 1.0f;
				}
				else
				{
					cosa = -1.0f;
					cosb = -1.0f;
				}
			}
			
			for (size_t j = 0; j < numPoints; j++)
			{
				const float *point = &fPositions[indices[j] * 3];
				aPosition[j * 2 + 0] = cacb * point[0] - sacb * point[1] + sinb * point[2];
				aPosition[j * 2 + 1] = sina * point[0] + cosa * point[1];
			}
			
			// 4) copy normals
			if (fNormals)
			{
				for (size_t j = 0; j < numPoints; j++)
				{
					aNormal[j * 3 + 0] = fNormals[indices[j] * 3 + 0];
					aNormal[j * 3 + 1] = fNormals[indices[j] * 3 + 1];
					aNormal[j * 3 + 2] = fNormals[indices[j] * 3 + 2];
				}
			}
			
			// 5) do triangulation
			int numResultTriangles = 0;
			MFnMesh::polyTriangulate(
					aPosition.get(),
					(unsigned int) numPoints,
					(unsigned int) numPoints,
					0,
					fNormals != NULL,
					aNormal.get(),
					triangles.get(),
					numResultTriangles);
			
			if (numResultTriangles == int(numPoints - 2))
			{
				// triangulation success
				for (size_t j = 0; j < size_t(numResultTriangles); j++)
				{
					triangleIndices[triangleCount * 3 + 0] = indices[triangles[j * 3 + 0]];
					triangleIndices[triangleCount * 3 + 1] = indices[triangles[j * 3 + 1]];
					triangleIndices[triangleCount * 3 + 2] = indices[triangles[j * 3 + 2]];
					triangleCount++;
				}
			}
			else
			{
				// triangulation failure, use the default triangulation
				for (size_t j = 1; j < numPoints - 1; j++)
				{
					triangleIndices[triangleCount * 3 + 0] = indices[0];
					triangleIndices[triangleCount * 3 + 1] = indices[j];
					triangleIndices[triangleCount * 3 + 2] = indices[j + 1];
					triangleCount++;
				}
			}
		}
		
		fNumTriangles = totalTriangles;
		fTriangleIndices = triangleIndices;
	}
public:
	size_t numTriangles()
	{
		return fNumTriangles;
	}
	boost::shared_array<index_type>& triangleIndices()
	{
		return fTriangleIndices;
	}

private:
	// Prohibited and not implemented.
	PolyTriangulator(const PolyTriangulator&);
	const PolyTriangulator& operator=(const PolyTriangulator&);
	
};

NAMESPACE_BASE_END
