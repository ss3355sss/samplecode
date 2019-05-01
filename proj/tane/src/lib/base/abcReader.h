#pragma once
#include "base/pch.h"


#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>

#include <boost/variant.hpp>

#include "base/frame.h"
#include "base/logger.h"

#include "base/matrixStack.h"

#include "base/bbox.h"
#include "base/renderItem.h"

NAMESPACE_BASE_BEGIN

namespace Abc   = ::Alembic::Abc;
namespace AbcA  = ::Alembic::AbcCoreAbstract;
namespace AbcF  = ::Alembic::AbcCoreFactory;
namespace AbcG  = ::Alembic::AbcGeom;

class AbcReaderBase
{
protected:
	MatrixStack<float> matrixStack_;
	AbcG::IObject      iObject_;

	BBox3f             bbox_;

protected:
	AbcReaderBase()
	{
	}
	virtual ~AbcReaderBase()
	{
	}

public:
	bool open(const std::string& filePath, const std::string& objectPath)
	{
		DECL_LOGGER(base::abc);
		/* get iObject */
		if(!getObject_(filePath, objectPath))
		{
			LOGGER_ERROR("Failed to open %s", filePath.c_str());
			return false;
		}
		
		return true;
	}
	
public:
	const BBox3f bbox(const short animCycle, const double time) const
	{
		if(!iObject_.valid())
		{
			STDOUT_ERROR("Failed to get valid iObject");
			return BBox3f();
		}
		
		AbcReaderBase* nonConstThis = (AbcReaderBase*)(this);

		/* clear bbox_ */
		nonConstThis->bbox_ = BBox3f();
		
		nonConstThis->abcTree_(animCycle, time, iObject_, true, true);
		
		return bbox_;
	}
	const std::string info(const std::string prefix = "", const bool showProps = false) const
	{
		if(!iObject_.valid())
		{
			STDOUT_ERROR("Failed to get valid iObject");
			return std::string("");
		}
		
		std::stringstream ss;
		tree_(iObject_, &ss, showProps, prefix);
		return ss.str();
	}
	const bool valid() const
	{
		return iObject_.valid();
	}
protected:
	virtual void processXform_(const Abc::ISampleSelector& ss, const AbcG::IXformSchema& schema)
	{
		Abc::M44d tmp = schema.getValue(ss).getMatrix().transpose();
		Mat4x4f   mat = Mat4x4f::Identity();
		for (int  i   = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				mat(i, j) = tmp[i][j];
		matrixStack_.transform(mat);
	}
	virtual void processSubD_(const Abc::ISampleSelector& ss, const AbcG::ISubDSchema& schema)
	{
	}
	virtual void processPolyMesh_(const Abc::ISampleSelector& ss, const AbcG::IPolyMeshSchema& schema)
	{
	}
	virtual void processNuPatch_(const Abc::ISampleSelector& ss, const AbcG::INuPatchSchema& schema)
	{
	}
	virtual void processPoints_(const Abc::ISampleSelector& ss, const AbcG::IPointsSchema& schema)
	{
	}
	virtual void processCurves_(const Abc::ISampleSelector& ss, const AbcG::ICurvesSchema& schema)
	{
	}

protected:
	void abcTree_(const short animCycle, const double time, const Abc::IObject& obj, bool visible, const bool bboxOnly)
	{
		DECL_LOGGER(base::abc);
		matrixStack_.push();
		
		const AbcA::ObjectHeader ohead = obj.getHeader();
		LOGGER_DEBUG("%s", ohead.getName().c_str());
		
		if(AbcG::IXform::matches(ohead))
		{
			const AbcG::IXform xform(obj, Abc::kWrapExisting);
			const AbcG::IXformSchema& schema = xform.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			getVisibility_(obj, ss, &visible);
			///------------------------------------------------------------------------ set xform data
			if(visible)
			{
				processXform_(ss, schema);
			}
		}
		else if(AbcG::ISubD::matches(ohead))
		{
			const AbcG::ISubD subD(obj, Abc::kWrapExisting);
			const AbcG::ISubDSchema& schema = subD.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			
			getVisibility_(obj, ss, &visible);
			///------------------------------------------------------------------------ set xform data
			if(visible)
			{
				processSelfBnd_(ss, schema);
				if(!bboxOnly)
					processSubD_(ss, schema);
			}
			
		}
		else if(AbcG::IPolyMesh::matches(ohead))
		{
			const AbcG::IPolyMesh polyMesh(obj, Abc::kWrapExisting);
			const AbcG::IPolyMeshSchema& schema = polyMesh.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			LOGGER_DEBUG("ss.index = %d, ss2.reqTime = %5.2f", ss.getRequestedIndex(), ss.getRequestedTime());
			
			getVisibility_(obj, ss, &visible);
			if(visible)
			{
				processSelfBnd_(ss, schema);
				if(!bboxOnly)
					processPolyMesh_(ss, schema);
			}
		}
		else if(AbcG::INuPatch::matches(ohead))
		{
			const AbcG::INuPatch nuPatch(obj, Abc::kWrapExisting);
			const AbcG::INuPatchSchema& schema = nuPatch.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			LOGGER_DEBUG("ss.index = %d, ss2.reqTime = %5.2f", ss.getRequestedIndex(), ss.getRequestedTime());
			getVisibility_(obj, ss, &visible);
			if(visible)
			{
				processSelfBnd_(ss, schema);
				if(!bboxOnly)
					processNuPatch_(ss, schema);
			}
		}
		else if(AbcG::IPoints::matches(ohead))
		{
			const AbcG::IPoints points(obj, Abc::kWrapExisting);
			const AbcG::IPointsSchema& schema = points.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			LOGGER_DEBUG("ss.index = %d, ss2.reqTime = %5.2f", ss.getRequestedIndex(), ss.getRequestedTime());
			getVisibility_(obj, ss, &visible);
			if(visible)
			{
				processSelfBnd_(ss, schema);
				if(!bboxOnly)
					processPoints_(ss, schema);
			}
		}
		else if(AbcG::ICurves::matches(ohead))
		{
			const AbcG::ICurves curves(obj, Abc::kWrapExisting);
			const AbcG::ICurvesSchema& schema = curves.getSchema();
			
			Abc::ISampleSelector ss;
			if(!schema.isConstant())
			{
				ss = getSampleSelector_(animCycle, time, schema);
			}
			LOGGER_DEBUG("ss.index = %d, ss2.reqTime = %5.2f", ss.getRequestedIndex(), ss.getRequestedTime());
			getVisibility_(obj, ss, &visible);
			if(visible)
			{
				processSelfBnd_(ss, schema);
				if(!bboxOnly)
					processCurves_(ss, schema);
			}
		}
		else if(AbcG::IFaceSet::matches(ohead))
		{
			/* not supported */
		}
		else
		{
			//Don't complain but don't walk beneath other types
		}
		if(visible)
		{
			/// object tree traversing
			for (size_t i = 0; i < obj.getNumChildren(); i++)
			{
				abcTree_(animCycle, time, obj.getChild(i), visible, bboxOnly);
			};
		}
		
		matrixStack_.pop();
	}
	
private:
	bool isFile_(const std::string& filename)
	{
		struct stat buf;
		if(stat(filename.c_str(), &buf) == 0 && !S_ISDIR(buf.st_mode))
		{
			return true;
		}
		return false;
	}
	bool getObject_(const std::string& filePath, const std::string& objectPath)
	{
		DECL_LOGGER(base::abc);
		std::string              path = filePath + objectPath;
		std::stringstream        ss(path.c_str());
		std::stringstream        fp;
		std::string              segment;
		std::vector<std::string> seglist;
		
		int j = 0;
		while (std::getline(ss, segment, '/'))
		{
			if(!isFile_(fp.str()))
			{
				if(j != 0)
					fp << "/";
				fp << segment;
			}
			else
			{
				seglist.push_back(segment);
			}
			++j;
		}
		
		Abc::IArchive  archive;
		AbcF::IFactory factory;
		factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
		AbcF::IFactory::CoreType coreType;
		
		archive = factory.getArchive(std::string(fp.str()), coreType);
		if(!archive.valid())
		{
			LOGGER_ERROR("Failed to open alembic, filePath = %s", filePath.c_str());
			return false;
		}
		
		// walk object hierarchy and find valid objects
		AbcG::IObject test = archive.getTop();
		AbcG::IObject iObj = test;
		while (test.valid() && seglist.size() > 0)
		{
			test = test.getChild(seglist.front());
			if(test.valid())
			{
				iObj = test;
				seglist.erase(seglist.begin());
			}
		}
		
		if(!iObj.valid() || iObj.getFullName() != std::string(objectPath.c_str()))
		{
			STDOUT_ERROR("object path is not valid, objectPath = %s", objectPath.c_str());
			return false;
		}
		iObject_ = iObj;
		return true;
	}
	void getVisibility_(const Abc::IObject& obj, const Abc::ISampleSelector& ss, bool* pOutVisible)
	{
		Abc::IObject* nonConst = (Abc::IObject*) &obj;
		switch (AbcG::GetVisibility(*nonConst, ss))
		{
			case AbcG::kVisibilityVisible:
				*pOutVisible = true;
				break;
			case AbcG::kVisibilityHidden:
				*pOutVisible = false;
				break;
			default:
				break;
		}
		
	}
	template<class SchemaT>
	void processSelfBnd_(const Abc::ISampleSelector& ss, const SchemaT& schema)
	{
		Abc::Box3d selfBnds = schema.getSelfBoundsProperty().getValue(ss);
		
		BBox3f bound(Point3f(selfBnds.min[0], selfBnds.min[1], selfBnds.min[2]),
		             Point3f(selfBnds.max[0], selfBnds.max[1], selfBnds.max[2])
		);
		
		const Mat4x4f& xformMtx = matrixStack_.top();
		for(int i = 0; i < 8; ++i)
		{
			Point3f corner0 = bound.corner(i);
			Vec4f   corner1 = xformMtx * Vec4f(corner0[0], corner0[1], corner0[2], 1.0);
			bbox_.merge(Point3f(corner1[0], corner1[1], corner1[2]));
		}
	}
	template<class SchemaT>
	Abc::ISampleSelector getSampleSelector_(const short animCycle, const double time, const SchemaT& schema)
	{
		DECL_LOGGER(base::abc);
		const int             nsamples      = schema.getNumSamples();
		const Abc::chrono_t   minTime       = schema.getTimeSampling()->getSampleTime(0);
		const Abc::chrono_t   maxTime       = schema.getTimeSampling()->getSampleTime(schema.getNumSamples() - 1);
		
		LOGGER_DEBUG("minTime = %5.2f", minTime);
		LOGGER_DEBUG("maxTime = %5.2f", maxTime);
		
		double targetTime = 0.0;
		switch (animCycle)
		{
			case mode::animCycle::none:
				LOGGER_DEBUG("animCycle = none");
				targetTime = Time::ClampTime(time, minTime, maxTime);
				break;
			case mode::animCycle::looped:
				LOGGER_DEBUG("animCycle = looped");
				targetTime = Time::LoopedTime(time, minTime, maxTime);
				break;
			case mode::animCycle::osciilated:
				LOGGER_DEBUG("animCycle = osciilated");
				targetTime = Time::OsciilateTime(time, minTime, maxTime);
				break;
		}
		
		
		return Abc::ISampleSelector(static_cast<Abc::chrono_t>(targetTime));
	}
	bool isLeaf_(AbcG::IObject iObj) const
	{
		if(!iObj.getParent().valid())
		{
			return true;
		}
		Abc::IObject parent = iObj.getParent();
		int numChildren = parent.getNumChildren();
		Abc::IObject test = parent.getChild(numChildren - 1);
		if(test.valid() && test.getName() != iObj.getName())
		{
			return false;
		}
		return true;
	}
	bool isLeaf_(Abc::ICompoundProperty iProp, Abc::PropertyHeader iHeader) const
	{
		if(!iProp.valid())
		{
			return true;
		}
		int last = iProp.getNumProperties() - 1;
		Abc::PropertyHeader header = iProp.getPropertyHeader(last);
		if(header.getName() == iHeader.getName())
			return true;
		return false;
	}
	int index_(Abc::ICompoundProperty iProp, Abc::PropertyHeader iHeader) const
	{
		for (size_t i = 0; i < iProp.getNumProperties(); i++)
		{
			Abc::PropertyHeader header = iProp.getPropertyHeader(i);
			if(header.getName() == iHeader.getName())
			{
				return i;
			}
		}
		return -1;
	}
	void tree_(Abc::IScalarProperty iProp, std::stringstream *ss, std::string prefix) const
	{
		if(iProp.getObject().getFullName() != "/")
		{
			prefix = prefix + " ";
		}
		if(isLeaf_(iProp.getParent(), iProp.getHeader()) && (iProp.getObject().getNumChildren() == 0 || iProp.getParent().getName() != ""))
		{
			(*ss) << prefix << " `--";
			
		}
		else
		{
			(*ss) << prefix << " :--";
			prefix = prefix + " :";
		}
		(*ss) << iProp.getName() << "\r" << std::endl;
	}
	void tree_(Abc::IArrayProperty iProp, std::stringstream *ss, std::string prefix) const
	{
		if(iProp.getObject().getFullName() != "/")
		{
			prefix = prefix + " ";
		}
		
		if(isLeaf_(iProp.getParent(), iProp.getHeader()) && (iProp.getObject().getNumChildren() == 0 || iProp.getParent().getName() != ""))
		{
			(*ss) << prefix << " `--";
		}
		else
		{
			(*ss) << prefix << " :--";
			prefix = prefix + " :";
		}
		
		(*ss) << iProp.getName() << "\r" << std::endl;
	}
	void tree_(Abc::ICompoundProperty iProp, std::stringstream *ss, std::string prefix) const
	{
		if(iProp.getObject().getFullName() != "/")
		{
			prefix = prefix + " ";
		}
		if(isLeaf_(iProp.getParent(), iProp.getHeader()) && iProp.getObject().getNumChildren() == 0)
		{
			(*ss) << prefix << " `--";
			prefix = prefix + " ";
		}
		else
		{
			if(isLeaf_(iProp.getParent(), iProp.getHeader()))
			{
				(*ss) << prefix << " | `--";
				prefix = prefix + " |";
			}
			else if(iProp.getObject().getNumChildren() == 0)
			{
				(*ss) << prefix << " :--";
				prefix = prefix + " :";
			}
			else if(isLeaf_(iProp, iProp.getHeader()))
			{
				(*ss) << prefix << " | `--";
				prefix = prefix + " |";
			}
			else
			{
				(*ss) << prefix << " | :--";
				prefix = prefix + " | :";
			}
		}
		
		(*ss) << iProp.getName() << "\r" << "\n";
		
		for (size_t i = 0; i < iProp.getNumProperties(); i++)
		{
			Abc::PropertyHeader header = iProp.getPropertyHeader(i);
			if(header.isScalar())
			{
				tree_(Abc::IScalarProperty(iProp, header.getName()), ss, prefix);
			}
			else if(header.isArray())
			{
				tree_(Abc::IArrayProperty(iProp, header.getName()), ss, prefix);
			}
			else
			{
				tree_(Abc::ICompoundProperty(iProp, header.getName()), ss, prefix);
			}
		}
	}
	void tree_(AbcG::IObject iObj, std::stringstream *ss, bool showProps, std::string prefix) const
	{
		std::string path = iObj.getFullName();
		if(path == "/")
		{
			prefix = "";
		}
		else
		{
			if(iObj.getParent().getFullName() != "/")
			{
				prefix = prefix + " ";
			}
			if(isLeaf_(iObj))
			{
				(*ss) << prefix << " `--";
				prefix = prefix + " ";
			}
			else
			{
				(*ss) << prefix << " |--";
				prefix = prefix + " |";
			}
		};
		
		if(iObj.getName() == "ABC")
			(*ss) << "/";
		else
			(*ss) << iObj.getName();
		(*ss) << "\r" << "\n";
		
		
		// property tree
		if(showProps)
		{
			Abc::ICompoundProperty props = iObj.getProperties();
			for (size_t i = 0; i < props.getNumProperties(); i++)
			{
				Abc::PropertyHeader header = props.getPropertyHeader(i);
				if(header.isScalar())
				{
					tree_(Abc::IScalarProperty(props, header.getName()), ss, prefix);
				}
				else if(header.isArray())
				{
					tree_(Abc::IArrayProperty(props, header.getName()), ss, prefix);
				}
				else
				{
					tree_(Abc::ICompoundProperty(props, header.getName()), ss, prefix);
				}
			}
		}
		
		// object tree
		for (size_t i = 0; i < iObj.getNumChildren(); i++)
		{
			tree_(AbcG::IObject(iObj, iObj.getChildHeader(i).getName()), ss, showProps, prefix);
		};
	}

	
};

class AbcReader : public AbcReaderBase
{
private:
	typedef AbcReaderBase Base;

private:
	using Geom = boost::variant<
		std::shared_ptr<IndexedGeom>,
		std::shared_ptr<ExpandedGeom>
	>;

private:
	Geom    geom_;
	int     indexOffset_;
public:
	AbcReader() : Base()
	{
	}
	virtual ~AbcReader()
	{
	}
public:
	const std::shared_ptr<ExpandedGeom> expandedSample(const short animCycle, const double sec, const int indexOffset = 0)
	{
		if(!iObject_.valid())
		{
			STDOUT_ERROR("Failed to get sample");
			return nullptr;
		}
		geom_        = std::make_shared<ExpandedGeom>();
		indexOffset_ = indexOffset;
		
		/* fill var_ */
		abcTree_(animCycle, sec, iObject_, true, false);
		
		return boost::get<std::shared_ptr<ExpandedGeom>>(geom_);
	}
	
protected:
	virtual void processPolyMesh_(const Abc::ISampleSelector& ss, const AbcG::IPolyMeshSchema& ps)
	{
		DECL_LOGGER(base::abc);
		switch (geom_.which())
		{
			case 0:
				LOGGER_DEBUG("geomType is IndexedGeom");
				if(!setIndexedGeom_(ss, ps))
				{
					STDOUT_ERROR("Failed to set ExpandedGeom");
					return;
				}
				break;
			case 1:
				LOGGER_DEBUG("geomType is ExpandedGeom");
				if(!setExpandedGeom_(ss, ps))
				{
					STDOUT_ERROR("Failed to set ExpandedGeom");
					return;
				}
				break;
		}
	}

private:
	bool setIndexedGeom_(const Abc::ISampleSelector& ss, const AbcG::IPolyMeshSchema& ps)
	{
		return false;
	}
	bool setExpandedGeom_(const Abc::ISampleSelector& ss, const AbcG::IPolyMeshSchema& ps)
	{
		DECL_LOGGER(base::abc);
		std::shared_ptr<ExpandedGeom> pGeom = boost::get<std::shared_ptr<ExpandedGeom>>(geom_);
		if(!pGeom)
		{
			STDOUT_ERROR("Failed to get pGeom");
			return false;
		}
		/* get positions */
		const Mat4x4f& xformMtx = matrixStack_.top();
		Abc::P3fArraySamplePtr pPoints = ps.getPositionsProperty().getValue(ss);
		
		/* get normals */
		Abc::N3fArraySamplePtr    pNormals       = nullptr;
		Abc::UInt32ArraySamplePtr pNormalIndices = nullptr;
		{
			AbcG::IN3fGeomParam normalParam = ps.getNormalsParam();
			if(normalParam.valid())
			{
				AbcG::IN3fGeomParam::Sample normalSample = normalParam.getIndexedValue(ss);
				pNormals       = normalSample.getVals();
				pNormalIndices = normalSample.getIndices();
			}
			else
			{
				STDOUT_WARRNING("Failed to get normal");
			}
			
		}
		
		
		/* get uvs */
		Abc::V2fArraySamplePtr    pUVs       = nullptr;
		Abc::UInt32ArraySamplePtr pUVIndices = nullptr;
		{
			AbcG::IV2fGeomParam uvParam = ps.getUVsParam();
			if(uvParam.valid())
			{
				AbcG::IV2fGeomParam::Sample uvSample = uvParam.getIndexedValue(ss);
				pUVs       = uvSample.getVals();
				pUVIndices = uvSample.getIndices();
			}
			else
			{
				STDOUT_WARRNING("Failed to get uv");
			}
		}
		
		
		/* set Index */
		int pointOffset = pGeom->pointArray.size();
		Abc::Int32ArraySamplePtr pFaceIndices = ps.getFaceIndicesProperty().getValue(ss);
		Abc::Int32ArraySamplePtr pFaceCounts  = ps.getFaceCountsProperty().getValue(ss);
		
		int nfaces       = pFaceCounts->size();
		int baseIndex    = 0;
		int triangleBase = 0;
		for (int fid = 0; fid < nfaces; ++fid)
		{
			int nvertices = (*pFaceCounts)[fid];
			//LOGGER_DEBUG("nvertices[%d] = %d", fid, nvertices);
			int             ntri = nvertices - 2;
			Point3fArray    pointArray(ntri * 3);
			Normal3fArray   normalArray(ntri * 3);
			TexCoord3fArray uvArray(ntri * 3);
			Int32Array      indexArray(ntri * 4);
			
			/* triangulate */
			for (int vid = 0; vid < nvertices - 2; ++vid)
			{
				/* winding = CW */
				int id0 = triangleBase + (nvertices - 1);
				int id1 = triangleBase + (vid + 1);
				int id2 = triangleBase + (vid + 0);
				
				
				//LOGGER_DEBUG("f[%d].id = (%d, %d, %d)", fid, id0, id1, id2);
				indexArray[vid * 4 + 0] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 1] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 2] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 3] = 0xffffffff;
				
				pointArray[vid * 3 + 0] = transformPoint_(xformMtx, (*pPoints)[(*pFaceIndices)[id0]]);
				pointArray[vid * 3 + 1] = transformPoint_(xformMtx, (*pPoints)[(*pFaceIndices)[id1]]);
				pointArray[vid * 3 + 2] = transformPoint_(xformMtx, (*pPoints)[(*pFaceIndices)[id2]]);
				
				if(pNormals && pNormalIndices)
				{
					normalArray[vid * 3 + 0] = transformNormal_(xformMtx, (*pNormals)[(*pNormalIndices)[id0]]);
					normalArray[vid * 3 + 1] = transformNormal_(xformMtx, (*pNormals)[(*pNormalIndices)[id1]]);
					normalArray[vid * 3 + 2] = transformNormal_(xformMtx, (*pNormals)[(*pNormalIndices)[id2]]);
				}
				
				if(pUVs && pUVIndices)
				{
					static float layer = 0.0;
					uvArray[vid * 3 + 0] = TexCoord3f((*pUVs)[(*pUVIndices)[id0]][0], (*pUVs)[(*pUVIndices)[id0]][1], layer);
					uvArray[vid * 3 + 1] = TexCoord3f((*pUVs)[(*pUVIndices)[id1]][0], (*pUVs)[(*pUVIndices)[id1]][1], layer);
					uvArray[vid * 3 + 2] = TexCoord3f((*pUVs)[(*pUVIndices)[id2]][0], (*pUVs)[(*pUVIndices)[id2]][1], layer);
				}
			}
			pGeom->pointArray .insert(pGeom->pointArray .end(), pointArray .begin(), pointArray .end());
			pGeom->normalArray.insert(pGeom->normalArray.end(), normalArray.begin(), normalArray.end());
			pGeom->uvArray    .insert(pGeom->uvArray    .end(), uvArray    .begin(), uvArray    .end());
			pGeom->indexArray .insert(pGeom->indexArray .end(), indexArray .begin(), indexArray .end());
			
			triangleBase += nvertices;
		}
		
		return true;
	}

protected:
	const Vec3f transformPoint_(const Mat4x4f& xformMtx, const Imath::Vec3<float>& v)
	{
		const Vec4f& transformed = xformMtx * Vec4f(v[0], v[1], v[2], 1.0);
		return Point3f(transformed[0], transformed[1], transformed[2]);
	}
	const Vec3f transformNormal_(const Mat4x4f& xformMtx, const Imath::Vec3<float>& v)
	{
		const Vec4f& transformed = xformMtx * Vec4f(v[0], v[1], v[2], 0.0);
		return Point3f(transformed[0], transformed[1], transformed[2]);
	}
	
};

inline std::shared_ptr<ExpandedGeom> CreateExpandedLODGeom(const short animCycle,
                                                           const double sec,
                                                           const BBox3f& bbox,
                                                           const AbcReader& reader0,
                                                           const AbcReader& reader1 = AbcReader(),
                                                           const AbcReader& reader2 = AbcReader()
)
{
	DECL_LOGGER(base::abc);
	if(!reader0.valid())
	{
		STDOUT_ERROR("Failed to get valid abcReader0");
		return nullptr;
	}
	AbcReader* pNonConstReader0 = (AbcReader*) (&reader0);
	
	int pointOffset = 0;
	/* set lod0 */
	std::shared_ptr<ExpandedGeom> pGeom = pNonConstReader0->expandedSample(animCycle, sec, pointOffset);
	pGeom->drawCmd[0].indexOffset = 0;
	pGeom->drawCmd[0].indexCount  = pGeom->indexArray.size();
	pointOffset += pGeom->pointArray.size();
	
	std::shared_ptr<ExpandedGeom> pGeom1 = nullptr;
	if(reader1.valid())
	{
		AbcReader* pNonConstReader1 = (AbcReader*) (&reader1);
		pGeom1 = pNonConstReader1->expandedSample(animCycle, sec, pointOffset);
	}
	else
	{
		pGeom1 = std::make_shared<ExpandedGeom>();
		BuildExpandedBBoxGeom(bbox,
		                      &pGeom1->pointArray,
		                      &pGeom1->normalArray,
		                      &pGeom1->uvArray,
		                      &pGeom1->indexArray,
		                      pointOffset
		);
	}
	pGeom->drawCmd[1].indexOffset = pGeom->indexArray.size();
	pGeom->drawCmd[1].indexCount  = pGeom1->indexArray.size();
	pGeom->pointArray .insert(pGeom->pointArray .end(), pGeom1->pointArray .begin(), pGeom1->pointArray .end());
	pGeom->normalArray.insert(pGeom->normalArray.end(), pGeom1->normalArray.begin(), pGeom1->normalArray.end());
	pGeom->uvArray    .insert(pGeom->uvArray    .end(), pGeom1->uvArray    .begin(), pGeom1->uvArray    .end());
	pGeom->indexArray .insert(pGeom->indexArray .end(), pGeom1->indexArray .begin(), pGeom1->indexArray .end());
	pointOffset += pGeom1->pointArray.size();

	std::shared_ptr<ExpandedGeom> pGeom2 = nullptr;
	if(reader2.valid())
	{
		AbcReader* pNonConstReader2 = (AbcReader*) (&reader2);
		pGeom2 = pNonConstReader2->expandedSample(animCycle, sec, pointOffset);
	}
	else
	{
		pGeom2 = std::make_shared<ExpandedGeom>();
		BuildExpandedBBoxGeom(bbox,
		                      &pGeom2->pointArray,
		                      &pGeom2->normalArray,
		                      &pGeom2->uvArray,
		                      &pGeom2->indexArray,
		                      pointOffset
		);
	}
	pGeom->drawCmd[2].indexOffset = pGeom->indexArray.size();
	pGeom->drawCmd[2].indexCount  = pGeom2->indexArray.size();
	pGeom->pointArray .insert(pGeom->pointArray .end(), pGeom2->pointArray .begin(), pGeom2->pointArray .end());
	pGeom->normalArray.insert(pGeom->normalArray.end(), pGeom2->normalArray.begin(), pGeom2->normalArray.end());
	pGeom->uvArray    .insert(pGeom->uvArray    .end(), pGeom2->uvArray    .begin(), pGeom2->uvArray    .end());
	pGeom->indexArray .insert(pGeom->indexArray .end(), pGeom2->indexArray .begin(), pGeom2->indexArray .end());
	pointOffset += pGeom2->pointArray.size();
	
	return pGeom;
}

NAMESPACE_BASE_END