#pragma once

#include "baseMaya/pch.h"

#include "base/array.h"

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

NAMESPACE_BASE_BEGIN
template<class INDEX_TYPE>
class WireIndicesGenerator
{
public:
	typedef INDEX_TYPE index_type;

private:
	// This class represents an unordered pair for wireframe indices
	struct WirePair
	{
	public:
		index_type fV1, fV2;
		index_type fMappedV1, fMappedV2;
	public:
		WirePair(index_type v1,
		         index_type v2,
		         index_type mappedV1,
		         index_type mappedV2)
				: fV1(v1),
				  fV2(v2),
				  fMappedV1(mappedV1),
				  fMappedV2(mappedV2)
		{
		}
	
	public:
		struct Hash : std::unary_function<WirePair, std::size_t>
		{
			std::size_t operator()(const WirePair& pair) const
			{
				std::size_t seed = 0;
				if (pair.fV1 < pair.fV2) {
					boost::hash_combine(seed, pair.fV1);
					boost::hash_combine(seed, pair.fV2);
				}
				else {
					boost::hash_combine(seed, pair.fV2);
					boost::hash_combine(seed, pair.fV1);
				}
				return seed;
			}
		};
	public:
		struct EqualTo : std::binary_function<WirePair, WirePair, bool>
		{
			bool operator()(const WirePair& x, const WirePair& y) const
			{
				if (x.fV1 < x.fV2) {
					if (y.fV1 < y.fV2) {
						return (x.fV1 == y.fV1 && x.fV2 == y.fV2);
					}
					else {
						return (x.fV1 == y.fV2 && x.fV2 == y.fV1);
					}
				}
				else {
					if (y.fV1 < y.fV2) {
						return (x.fV2 == y.fV1 && x.fV1 == y.fV2);
					}
					else {
						return (x.fV2 == y.fV2 && x.fV1 == y.fV1);
					}
				}
			}
		};
		
	};

private:
	// Input
	size_t              fNumFaceCounts;
	const unsigned int* fFaceCounts;
	
	size_t              fNumFaceIndices;
	const index_type*   fFaceIndices;
	const index_type*   fMappedFaceIndices;
	
	// Output
	size_t                          fNumWires;
	boost::shared_array<index_type> fWireIndices;
public:
	WireIndicesGenerator(size_t numFaceCounts,
	                     const unsigned int* faceCounts,
	                     size_t numFaceIndices,
	                     const index_type* faceIndices,
	                     const index_type*
	                     mappedFaceIndices)
			: fNumFaceCounts(numFaceCounts),
			  fFaceCounts(faceCounts),
			  fNumFaceIndices(numFaceIndices),
			  fFaceIndices(faceIndices),
			  fMappedFaceIndices(mappedFaceIndices),
			  fNumWires(0)
	{
	}
private:
	WireIndicesGenerator(const WireIndicesGenerator&);                   // Prohibited and not implemented.
	const WireIndicesGenerator& operator= (const WireIndicesGenerator&); // Prohibited and not implemented.

public:
	void compute()
	{
		if (fNumFaceCounts == 0 || fNumFaceIndices == 0)
		{
			STDOUT_ERROR("Failed to get valid mesh data");
			return;
		}

		// pre-allocate buffers for the worst case
		size_t maxNumWires = fNumFaceIndices;
		boost::unordered_set<WirePair, typename WirePair::Hash, typename WirePair::EqualTo> wireSet(size_t(maxNumWires / 0.75f));

		// insert all wires to the set
		size_t      polyIndex = 0;
		size_t      endOfPoly = fFaceCounts[polyIndex];
		for (size_t i         = 0; i < fNumFaceIndices; i++)
		{
			// find the two vertices of the wireframe
			// v1 and v2 (face indices before splitting vertices) are hashed to
			// remove duplicated wireframe lines.
			// mappedV1 and mappedV2 are the actual indices to remapped
			// positions/normals/UVs
			index_type v1, v2, mappedV1, mappedV2;
			v1       = fFaceIndices[i];
			mappedV1 = fMappedFaceIndices[i];

			size_t v2Index;
			if (i + 1 == endOfPoly)
			{
				// wrap at the end of the polygon
				v2Index = i + 1 - fFaceCounts[polyIndex];
				if (++polyIndex < fNumFaceCounts)
				{
					endOfPoly += fFaceCounts[polyIndex];
				}
			}
			else
			{
				v2Index = i + 1;
			}

			v2       = fFaceIndices[v2Index];
			mappedV2 = fMappedFaceIndices[v2Index];

			// insert
			wireSet.insert(WirePair(v1, v2, mappedV1, mappedV2));
		}

		// the number of wireframes
		size_t numWires                           = wireSet.size();

		// allocate buffers for wireframe indices
		boost::shared_array<index_type> wireIndices(new index_type[numWires * 2]);
		size_t                          wireCount = 0;
		BOOST_FOREACH(const WirePair& pair, wireSet)
		{
			wireIndices[wireCount * 2 + 0] = pair.fMappedV1;
			wireIndices[wireCount * 2 + 1] = pair.fMappedV2;
			wireCount++;
		}

		fNumWires    = numWires;
		fWireIndices = wireIndices;
	}
	size_t                           numWires()
	{
		return fNumWires;
	}
	boost::shared_array<index_type>& wireIndices()
	{
		return fWireIndices;
	}



};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
template<class INDEX_TYPE, size_t MAX_NUM_STREAMS = 16>
class MultiIndexedStreamsConverter
{
public:
	typedef INDEX_TYPE index_type;
private:
	// This class represents a multi-index combination
	class IndexTuple
	{
	public:
		IndexTuple(index_type* indices, unsigned int size, unsigned int faceIndex)
				: fIndices(indices), fSize(size), fFaceIndex(faceIndex)
		{}
		
		const index_type& operator[](unsigned int index) const
		{
			assert(index < fSize);
			return fIndices[index];
		}
		
		unsigned int faceIndex() const
		{
			return fFaceIndex;
		}
		
		struct Hash : std::unary_function<IndexTuple, std::size_t>
		{
			std::size_t operator()(const IndexTuple& tuple) const
			{
				std::size_t seed = 0;
				for (unsigned int i = 0; i < tuple.fSize; i++) {
					boost::hash_combine(seed, tuple.fIndices[i]);
				}
				return seed;
			}
		};
		
		struct EqualTo : std::binary_function<IndexTuple, IndexTuple, bool>
		{
			bool operator()(const IndexTuple& x, const IndexTuple& y) const
			{
				if (x.fSize == y.fSize) {
					return memcmp(x.fIndices, y.fIndices, sizeof(index_type) * x.fSize) == 0;
				}
				return false;
			}
		};
	
	private:
		index_type*  fIndices;
		unsigned int fFaceIndex;
		unsigned int fSize;
	};

private:
	// Input
	size_t            fNumFaceIndices;
	const index_type* fFaceIndices;
	
	const index_type* fStreams[MAX_NUM_STREAMS];
	unsigned int      fNumStreams;
	
	// Output
	size_t                            fNumVertices;
	boost::shared_array<unsigned int> fVertAttribsIndices;
	boost::shared_array<index_type>   fMappedFaceIndices;
	
public:
	MultiIndexedStreamsConverter(size_t numFaceIndices,
	                             const index_type* faceIndices)
			: fNumFaceIndices(numFaceIndices),
			  fFaceIndices(faceIndices),
			  fNumStreams(0),
			  fNumVertices(0)
	{
		// position stream
		addMultiIndexedStream(faceIndices);
	}
private:
	MultiIndexedStreamsConverter(const MultiIndexedStreamsConverter&);                      // Prohibited and not implemented.
	const MultiIndexedStreamsConverter& operator= (const MultiIndexedStreamsConverter&);    // Prohibited and not implemented.

public:
	void addMultiIndexedStream(const index_type* indices)
	{
		// indices can be NULL, sequence 0,1,2,3,4,5... is assumed
		fStreams[fNumStreams++] = indices;
		assert(fNumStreams <= MAX_NUM_STREAMS);
	}
	void compute()
	{
		// pre-allocate buffers for the worst case
		boost::shared_array<index_type> indicesRegion(
				new index_type[fNumStreams * fNumFaceIndices]);

		// the hash map to find unique combination of multi-indices
		typedef boost::unordered_map<IndexTuple,size_t,typename IndexTuple::Hash,typename IndexTuple::EqualTo> IndicesMap;
		IndicesMap indicesMap(size_t(fNumFaceIndices / 0.75f));

		// fill the hash map with multi-indices
		size_t vertexAttribIndex = 0;  // index to the remapped vertex attribs
		boost::shared_array<index_type> mappedFaceIndices(new index_type[fNumFaceIndices]);

		for (size_t i = 0; i < fNumFaceIndices; i++) {
			// find the location in the region to copy multi-indices
			index_type* indices = &indicesRegion[i * fNumStreams];

			// make a tuple consists of indices for positions, normals, UVs..
			for (unsigned int j = 0; j < fNumStreams; j++) {
				indices[j] = fStreams[j] ? fStreams[j][i] : (index_type)i;
			}

			// try to insert the multi-indices tuple to the hash map
			IndexTuple tuple(indices, fNumStreams, (unsigned int)i);
			std::pair<typename IndicesMap::iterator,bool> ret = indicesMap.insert(std::make_pair(tuple, 0));

			if (ret.second) {
				// a success insert, allocate a vertex attrib index to the multi-index combination
				ret.first->second = vertexAttribIndex++;
			}

			// remap face indices
			mappedFaceIndices[i] = (index_type)ret.first->second;
		}

		// the number of unique combination is the size of vertex attrib arrays
		size_t numVertex = vertexAttribIndex;
		assert(vertexAttribIndex == indicesMap.size());

		// allocate memory for the indices into faceIndices
		boost::shared_array<unsigned int> vertAttribsIndices(new unsigned int[numVertex]);

		// build the indices (how the new vertex maps to the poly vert)
		BOOST_FOREACH(const typename IndicesMap::value_type& pair, indicesMap)
		{
			vertAttribsIndices[pair.second] = pair.first.faceIndex();
		}

		fMappedFaceIndices  = mappedFaceIndices;
		fVertAttribsIndices = vertAttribsIndices;
		fNumVertices        = numVertex;
	}
	unsigned int                       numStreams()
	{
		return fNumStreams;
	}
	size_t                             numVertices()
	{
		return fNumVertices;
	}
	boost::shared_array<unsigned int>& vertAttribsIndices()
	{
		return fVertAttribsIndices;
	}
	boost::shared_array<index_type>&   mappedFaceIndices()
	{
		return fMappedFaceIndices;
	}
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
template<class INDEX_TYPE, size_t MAX_NUM_STREAMS = 16>
class MultiIndexedStreamsRemapper
{
public:
	typedef INDEX_TYPE index_type;
private:
	// Input
	const index_type*   fFaceIndices;
	size_t              fNumNewVertices;
	const unsigned int* fVertAttribsIndices;
	
	const float*      fAttribs[MAX_NUM_STREAMS];
	const index_type* fIndices[MAX_NUM_STREAMS];
	bool              fFaceVarying[MAX_NUM_STREAMS];
	int               fStride[MAX_NUM_STREAMS];
	unsigned int      fNumStreams;
	
	// Output, NULL means no change
	boost::shared_array<float> fMappedVertAttribs[MAX_NUM_STREAMS];
	
public:
	MultiIndexedStreamsRemapper(const index_type* faceIndices,
	                            size_t numNewVertices, const unsigned int* vertAttribsIndices)
			: fFaceIndices(faceIndices), fNumNewVertices(numNewVertices),
			  fVertAttribsIndices(vertAttribsIndices), fNumStreams(0)
	{
	}
private:
	MultiIndexedStreamsRemapper(const MultiIndexedStreamsRemapper&);                   // Prohibited and not implemented.
	const MultiIndexedStreamsRemapper& operator= (const MultiIndexedStreamsRemapper&); // Prohibited and not implemented.

public:
	void addMultiIndexedStream(const float* attribs, const index_type* indices, bool faceVarying, int stride)
	{
		fAttribs[fNumStreams]     = attribs;
		fIndices[fNumStreams]     = indices;
		fFaceVarying[fNumStreams] = faceVarying;
		fStride[fNumStreams]      = stride;
		fNumStreams++;
	}
	void compute()
	{
		// remap vertex attribs
		for (unsigned int i = 0; i < fNumStreams; i++) {
			const float*      attribs     = fAttribs[i];
			const index_type* indices     = fIndices[i];
			bool              faceVarying = fFaceVarying[i];
			int               stride      = fStride[i];

			// allocate memory for remapped vertex attrib arrays
			boost::shared_array<float> mappedVertAttrib(
					new float[fNumNewVertices * stride]);

			for (size_t j = 0; j < fNumNewVertices; j++) {
				// new j-th vertices maps to polyVertIndex-th poly vert
				unsigned int polyVertIndex = fVertAttribsIndices[j];

				// if the scope is varying/vertex, need to convert poly vert
				// index to vertex index
				index_type pointOrPolyVertIndex = faceVarying ?
				                                  polyVertIndex : fFaceIndices[polyVertIndex];

				// look up the vertex attrib index
				index_type attribIndex = indices ?
				                         indices[pointOrPolyVertIndex] : pointOrPolyVertIndex;

				if (stride == 3) {
					mappedVertAttrib[j * 3 + 0] = attribs[attribIndex * 3 + 0];
					mappedVertAttrib[j * 3 + 1] = attribs[attribIndex * 3 + 1];
					mappedVertAttrib[j * 3 + 2] = attribs[attribIndex * 3 + 2];
				}
				else if (stride == 2) {
					mappedVertAttrib[j * 2 + 0] = attribs[attribIndex * 2 + 0];
					mappedVertAttrib[j * 2 + 1] = attribs[attribIndex * 2 + 1];
				}
				else {
					assert(0);
				}
			}

			fMappedVertAttribs[i] = mappedVertAttrib;
		}
	}
	boost::shared_array<float>& mappedVertAttribs(unsigned int index)
	{
		assert(index < fNumStreams);
		return fMappedVertAttribs[index];
	}

	
	
};
NAMESPACE_BASE_END

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
private:
	PolyTriangulator(const PolyTriangulator&);                  	// Prohibited and not implemented.
	const PolyTriangulator& operator=(const PolyTriangulator&); 	// Prohibited and not implemented.
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
	size_t numTriangles()
	{
		return fNumTriangles;
	}
	boost::shared_array<index_type>& triangleIndices()
	{
		return fTriangleIndices;
	}


};

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
template<class INDEX_TYPE>
class MayaMeshExtractor
{
public:
	typedef INDEX_TYPE index_type;

	typedef Array1<index_type>  TriangleIndexArray;
	typedef Array1<index_type>  WireIndexArray;
	typedef Array1<Point3f>     PositionArray;
	typedef Array1<Normal3f>    NormalArray;
	typedef Array1<TexCoord2f>  TexCoordArray;
	
private:
	// Input
	MFnMesh     fPolyMesh;
	bool        fWantUVs;

	// Output
	std::shared_ptr<Int3Array>          fTriangleIndices;
	std::shared_ptr<Int2Array>          fWireIndices;
	std::shared_ptr<Point3fArray>       fPositions;
	std::shared_ptr<Normal3fArray>      fNormals;
	std::shared_ptr<TexCoord2fArray>    fUVs;
	
public:
	MayaMeshExtractor(const MObject& meshObj, const bool wantUVs = true) : fPolyMesh(meshObj), fWantUVs(wantUVs)
	{
	}
private:
	MayaMeshExtractor(const MayaMeshExtractor&);    	            // Prohibited and not implemented.
	const MayaMeshExtractor& operator=(const MayaMeshExtractor&);   // Prohibited and not implemented.

public:
	void setWantUVs(bool wantUVs)
	{
		fWantUVs = wantUVs;
	}
	void compute()
	{
		MStatus status;
		bool    needTriangulate = false;
		
		// Topology
		size_t                            numFaceCounts;
		boost::shared_array<unsigned int> faceCounts;
		
		size_t                          numFaceIndices;
		boost::shared_array<index_type> faceIndices;
		
		{
			MIntArray mayaVertexCount, mayaVertexList;
			status = fPolyMesh.getVertices(mayaVertexCount, mayaVertexList);
			assert(status == MS::kSuccess);
			
			// Copy Maya arrays
			numFaceCounts = mayaVertexCount.length();
			const int* srcVertexCount = &mayaVertexCount[0];
			faceCounts.reset(new unsigned int[numFaceCounts]);
			for (size_t i = 0; i < numFaceCounts; i++)
			{
				faceCounts[i] = srcVertexCount[i];
				if (faceCounts[i] != 3) needTriangulate = true;
			}
			
			numFaceIndices = mayaVertexList.length();
			const int* srcVertexList = &mayaVertexList[0];
			faceIndices.reset(new index_type[numFaceIndices]);
			for (size_t i = 0; i < numFaceIndices; i++)
			{
				faceIndices[i] = srcVertexList[i];
			}
		}
		
		// Positions
		boost::shared_array<float> positions;
		{
			MFloatPointArray mayaPositions;
			status = fPolyMesh.getPoints(mayaPositions);
			assert(status == MS::kSuccess);
			
			// Allocate memory for positions
			unsigned int numPositions = mayaPositions.length();
			positions.reset(new float[numPositions * 3]);
			
			// just copy the positions to shared_array
			for (unsigned int i = 0; i < numPositions; i++)
			{
				MFloatPoint& point = mayaPositions[i];
				positions[i * 3 + 0] = point.x;
				positions[i * 3 + 1] = point.y;
				positions[i * 3 + 2] = point.z;
			}
		}
		
		// Normals
		boost::shared_array<float>      normals;
		boost::shared_array<index_type> normalIndices;
		{
			MFloatVectorArray mayaNormals;
			status = fPolyMesh.getNormals(mayaNormals);
			assert(status == MS::kSuccess);
			
			MIntArray mayaNormalIdCounts, mayaNormalIds;
			status = fPolyMesh.getNormalIds(mayaNormalIdCounts, mayaNormalIds);
			assert(status == MS::kSuccess);
			
			// Allocate memory for normals
			unsigned int numNormals   = mayaNormals.length();
			unsigned int numNormalIds = mayaNormalIds.length();
			normals.reset(new float[numNormals * 3]);
			normalIndices.reset(new index_type[numNormalIds]);
			
			// just copy the normals and normalIds to shared_array
			for (unsigned int i = 0; i < numNormals; i++)
			{
				MFloatVector& normal = mayaNormals[i];
				normals[i * 3 + 0] = normal.x;
				normals[i * 3 + 1] = normal.y;
				normals[i * 3 + 2] = normal.z;
			}
			for (unsigned int i = 0; i < numNormalIds; i++)
			{
				normalIndices[i] = mayaNormalIds[i];
			}
		}
		
		// UVs
		boost::shared_array<float>      UVs;
		boost::shared_array<index_type> uvIndices;
		if (fWantUVs)
		{
			MFloatArray mayaUArray, mayaVArray;
			status = fPolyMesh.getUVs(mayaUArray, mayaVArray);
			assert(status == MS::kSuccess);
			
			MIntArray mayaUVCounts, mayaUVIds;
			status = fPolyMesh.getAssignedUVs(mayaUVCounts, mayaUVIds);
			assert(status == MS::kSuccess);
			
			// Allocate memory for normals
			unsigned int numUVs   = mayaUArray.length();
			unsigned int numUVIds = mayaUVIds.length();
			if (numUVs > 0 && numUVIds > 0)
			{
				UVs.reset(new float[numUVs * 2]);
				uvIndices.reset(new index_type[numUVIds]);
				
				// just copy the UVs and uvIds to shared_array
				for (unsigned int i = 0; i < numUVs; i++)
				{
					UVs[i * 2 + 0] = mayaUArray[i];
					UVs[i * 2 + 1] = mayaVArray[i];
				}
				for (unsigned int i = 0; i < numUVIds; i++)
				{
					uvIndices[i] = mayaUVIds[i];
				}
			}
		}
		
		// Convert multi-indexed streams
		size_t                            numVertices = 0;
		boost::shared_array<index_type>   mappedFaceIndices;
		boost::shared_array<unsigned int> vertAttribsIndices;
		{
			MultiIndexedStreamsConverter<index_type> converter(numFaceIndices, faceIndices.get());
			
			converter.addMultiIndexedStream(normalIndices.get());
			if (fWantUVs && uvIndices)
			{
				converter.addMultiIndexedStream(uvIndices.get());
			}
			
			converter.compute();
			
			numVertices        = converter.numVertices();
			mappedFaceIndices  = converter.mappedFaceIndices();
			vertAttribsIndices = converter.vertAttribsIndices();
		}
		
		// Remap vertex streams
		boost::shared_array<float> mappedPositions;
		boost::shared_array<float> mappedNormals;
		boost::shared_array<float> mappedUVs;
		{
			MultiIndexedStreamsRemapper<index_type> remapper(faceIndices.get(), numVertices, vertAttribsIndices.get());
			
			remapper.addMultiIndexedStream(positions.get(), NULL, false, 3);
			remapper.addMultiIndexedStream(normals.get(), normalIndices.get(), true, 3);
			if (fWantUVs && UVs && uvIndices)
			{
				remapper.addMultiIndexedStream(UVs.get(), uvIndices.get(), true, 2);
			}
			
			remapper.compute();
			
			mappedPositions = remapper.mappedVertAttribs(0);
			mappedNormals   = remapper.mappedVertAttribs(1);
			if (fWantUVs && UVs && uvIndices)
			{
				mappedUVs = remapper.mappedVertAttribs(2);
			}
		}
		
		// Wireframe indices
		size_t                          numWires = 0;
		boost::shared_array<index_type> wireIndices;
		{
			WireIndicesGenerator<index_type> wireIndicesGenerator(numFaceCounts,
			                                                      faceCounts.get(),
			                                                      numFaceIndices,
			                                                      faceIndices.get(),
			                                                      mappedFaceIndices.get()
			);
			
			wireIndicesGenerator.compute();
			
			numWires    = wireIndicesGenerator.numWires();
			wireIndices = wireIndicesGenerator.wireIndices();
		}
		
		// Triangle indices
		size_t                          numTriangles = 0;
		boost::shared_array<index_type> triangleIndices;
		if (needTriangulate)
		{
			PolyTriangulator<index_type> polyTriangulator(numFaceCounts,
			                                              faceCounts.get(),
			                                              mappedFaceIndices.get(),
			                                              false,
			                                              mappedPositions.get(),
			                                              mappedNormals.get()
			);
			
			polyTriangulator.compute();
			
			numTriangles    = polyTriangulator.numTriangles();
			triangleIndices = polyTriangulator.triangleIndices();
		}
		else
		{
			assert(numFaceIndices % 3 == 0);
			numTriangles    = numFaceIndices / 3;
			triangleIndices = mappedFaceIndices;
		}
		
		/* ------------------------------------ done */
		fWireIndices = std::make_shared<Int2Array>(numWires);
		std::copy(wireIndices.get(), wireIndices.get() + (numWires * 2), (*fWireIndices)[0].data());
		
		fTriangleIndices = std::make_shared<Int3Array>(numTriangles);
		std::copy(triangleIndices.get(), triangleIndices.get() + (numTriangles * 3), (*fTriangleIndices)[0].data());
		
		fPositions = std::make_shared<Point3fArray>(numVertices);
		std::copy(mappedPositions.get(), mappedPositions.get() + (numVertices * 3), (*fPositions)[0].data());
		
		fNormals = std::make_shared<Normal3fArray>(numVertices);
		std::copy(mappedNormals.get(), mappedNormals.get() + (numVertices * 3), (*fNormals)[0].data());
		
		if (fWantUVs && mappedUVs)
		{
			fUVs = std::make_shared<TexCoord2fArray>(numVertices);
			std::copy(mappedUVs.get(), mappedUVs.get() + (numVertices * 2), (*fUVs)[0].data());
		}
	}

	const std::shared_ptr<Int3Array>& triangleIndices()
	{
		return fTriangleIndices;
	}
	const std::shared_ptr<Int2Array>& wireIndices()
	{
		return fWireIndices;
	}
	const std::shared_ptr<Point3fArray>& positions()
	{
		return fPositions;
	}
	const std::shared_ptr<Normal3fArray>& normals()
	{
		return fNormals;
	}
	const std::shared_ptr<TexCoord2fArray>& uvs()
	{
		return fUVs;
	}

	
};
NAMESPACE_BASE_END
