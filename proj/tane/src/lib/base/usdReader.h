#pragma once

#include "base/pch.h"

#include "base/frame.h"
#include "base/logger.h"

#include "base/matrixStack.h"

#include "base/bbox.h"
#include "base/renderItem.h"

#include "pxr/pxr.h"
#include "pxr/usd/ar/resolver.h"

#include "pxr/usd/sdf/layer.h"

#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/stageCache.h"
#include "pxr/usd/usd/stageCacheContext.h"
#include "pxr/usd/usd/variantSets.h"

#include "pxr/usd/usdGeom/points.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/xform.h"

#include "pxr/usd/usdUtils/pipeline.h"
#include "pxr/usd/usdUtils/stageCache.h"


PXR_NAMESPACE_USING_DIRECTIVE
NAMESPACE_BASE_BEGIN
class UsdReader
{
public:
	typedef std::vector<std::pair<std::string /* variant name */, std::string /*varaint select */>> VariantSelects;
	using Geom = boost::variant<
			std::shared_ptr<IndexedGeom>,
			std::shared_ptr<ExpandedGeom>
	>;

private:
	UsdStageRefPtr stage_;
	UsdStageCache stageCache_;
	UsdPrim usdRootPrim_;
	MatrixStack<float> matrixStack_;
	
	BBox3f bbox_;
	
	/* for mesh */
	int texLayer_;
	Geom geom_;
	int indexOffset_;

public:
	UsdReader()
	{
	}
	
	~UsdReader()
	{
	}

public:
	const UsdPrim& usdPrim() const
	{
		return usdRootPrim_;
	}
	
	UsdPrim& usdPrim()
	{
		return usdRootPrim_;
	}
	
	const std::string info() const
	{
		return std::string("");
	}
	
	const bool valid() const
	{
		return usdRootPrim_.IsValid();
	}

public:
	const BBox3f bbox(const TfToken purpose, const short animCycle, const UsdTimeCode timeCode) const
	{
		if (!usdRootPrim_)
		{
			STDOUT_ERROR("Failed to get valid usdPrim");
			return BBox3f();
		}
		
		UsdReader *nonConstThis = (UsdReader *) (this);
		
		/* clear bbox_ */
		nonConstThis->bbox_ = BBox3f();
		
		/* fill bbox_ */
		nonConstThis->usdTree_(purpose, animCycle, timeCode, usdRootPrim_, true, true);
		return bbox_;
	}
	
	const std::shared_ptr<ExpandedGeom> expandedSample(const TfToken purpose,
	                                                   const short animCycle,
	                                                   const UsdTimeCode timeCode,
	                                                   const int indexOffset = 0)
	{
		DECL_LOGGER(base::usd);
		if (!usdRootPrim_)
		{
			STDOUT_ERROR("Failed to get valid usdPrim");
			return nullptr;
		}
		
		/* clear geom_ */
		geom_ = std::make_shared<ExpandedGeom>();
		texLayer_ = 0;
		indexOffset_ = indexOffset;
		
		/* fill geom_ data */
		usdTree_(purpose, animCycle, timeCode, usdRootPrim_, true, false);
		return boost::get<std::shared_ptr<ExpandedGeom>>(geom_);
	}

public:
	bool open(const std::string& fileName,
	          const std::string& primPath,
	          const VariantSelects& variantSelects = VariantSelects(0))
	{
		DECL_LOGGER(base::usd);
		SdfLayerRefPtr rootLayer = SdfLayer::FindOrOpen(fileName);
		if (!rootLayer)
		{
			STDOUT_ERROR("Failed to open %s:%s", fileName.c_str(), primPath.c_str());
			return false;
		}
		
		TfToken modelName = UsdUtilsGetModelNameFromRootLayer(rootLayer);
		UsdStageRefPtr usdStage;
		SdfLayerRefPtr sessionLayer;
		
		
		//for(int i = 0; i < variantSelects.size(); ++i)
		//{
		//	LOGGER_DEBUG("variant = (%s, %s)", variantSelects[i].first.c_str(), variantSelects[i].second.c_str());
		//}
		sessionLayer = UsdUtilsStageCache::GetSessionLayerForVariantSelections(modelName, variantSelects);
		
		
		UsdStageCacheContext stageCacheContext(stageCache_);
		if (sessionLayer)
		{
			usdStage = UsdStage::Open(rootLayer,
			                          sessionLayer,
			                          ArGetResolver().GetCurrentContext());
		}
		else
		{
			usdStage = UsdStage::Open(rootLayer,
			                          ArGetResolver().GetCurrentContext());
		}
		usdStage->SetEditTarget(usdStage->GetSessionLayer());
		
		if (!usdStage)
		{
			STDOUT_ERROR("Failed to get stage %s", fileName.c_str());
			return false;
		}
		
		
		stage_ = usdStage;
		
		usdRootPrim_ = primPath.empty() ? stage_->GetDefaultPrim() : stage_->GetPrimAtPath(SdfPath(primPath));
		if (!usdRootPrim_ && !(primPath.empty() || primPath == "/"))
		{
			STDOUT_ERROR("Unable to set root prim to <%s> when reading USD file '%s'; " "using the pseudo-root </> instead", primPath.c_str(), fileName.c_str());
			usdRootPrim_ = stage_->GetPseudoRoot();
		}
		usdRootPrim_.GetVariantSets().GetNames();
		if (!usdRootPrim_)
		{
			STDOUT_ERROR("No default prim found in USD file '%s'", fileName.c_str());
			return false;
		}
		LOGGER_DEBUG("usdRootPrim = %s", usdRootPrim_.GetPath().GetText());
		
		for (int i = 0; i < variantSelects.size(); ++i)
		{
			LOGGER_DEBUG("variantSelects[i] = %s, value = %s", variantSelects[i].first.c_str(), variantSelects[i].second.c_str());
			
			UsdVariantSet variantSet = usdRootPrim_.GetVariantSet(variantSelects[i].first);
			if (!variantSet.SetVariantSelection(variantSelects[i].second))
			{
				STDOUT_ERROR("Failed to set variant, %s", variantSelects[i].first.c_str());
			}
		}
		return true;
	}

private:
	void usdTree_(const TfToken& targetPurpose, const short animCycle, const UsdTimeCode timeCode, const UsdPrim& usdPrim, bool visible, const bool extentOnly)
	{
		DECL_LOGGER(base::usd);
		LOGGER_IN("%s", usdPrim.GetName().GetText());
		matrixStack_.push();
		
		TfToken typeName = usdPrim.GetTypeName();
		LOGGER_DEBUG("typeName = %s", typeName.GetText());
		if (typeName == "Xform")
		{
			UsdGeomXform schema(usdPrim);
			UsdTimeCode targetTimeCode = getTargetTimeCode_(animCycle, timeCode, schema);
			getVisibility(targetTimeCode, &visible);
			if (visible)
			{
				processXform_(targetTimeCode, usdPrim, schema);
			}
		}
		else if (typeName == "Mesh")
		{
			UsdGeomMesh schema(usdPrim);
			UsdTimeCode targetTimeCode = getTargetTimeCode_(animCycle, timeCode, schema);
			LOGGER_DEBUG("targetTimeCode = %5.2f", targetTimeCode.GetValue());
			getVisibility(targetTimeCode, &visible);
			if (visible)
			{
				TfToken purpose = UsdGeomImageable(usdPrim).ComputePurpose();
				LOGGER_DEBUG("targetPurpose = %s", targetPurpose.GetText());
				LOGGER_DEBUG("purpose       = %s", purpose.GetText());
				if (purpose == UsdGeomTokens->default_ || purpose == targetPurpose)
				{
					GfMatrix4d xformGfMtx = schema.ComputeLocalToWorldTransform(timeCode);

					GfMatrix4d gfXformMtx = schema.ComputeLocalToWorldTransform(timeCode);
					Mat4x4f xformMtx(gfXformMtx[0][0], gfXformMtx[1][0], gfXformMtx[2][0], gfXformMtx[3][0],
					                 gfXformMtx[0][1], gfXformMtx[1][1], gfXformMtx[2][1], gfXformMtx[3][1],
					                 gfXformMtx[0][2], gfXformMtx[1][2], gfXformMtx[2][2], gfXformMtx[3][2],
					                 gfXformMtx[0][3], gfXformMtx[1][3], gfXformMtx[2][3], gfXformMtx[3][3]
					);
					
					processExtent_(xformMtx, targetTimeCode, schema);
					if (!extentOnly)
					{
						processMesh_(xformMtx, targetTimeCode, usdPrim, schema);
					}
				}
			}
		}
		else
		{
			STDOUT_ERROR("Failed to get supporting primType");
			return;
		}
		
		UsdPrimSiblingRange siblingRange = usdPrim.GetChildren();
		for (auto iter = siblingRange.begin(); iter != siblingRange.end(); ++iter)
		{
			usdTree_(targetPurpose, animCycle, timeCode, *iter, visible, extentOnly);
		}
		
		matrixStack_.pop();
		LOGGER_OUT();
		
	}

private:
	template<class SchemaT>
	void processExtent_(const Mat4x4f& xformMtx, const UsdTimeCode timeCode, const SchemaT& schema)
	{
		DECL_LOGGER(base::usd);
		VtArray<pxr::GfVec3f> extent(2);
		if (!schema.GetExtentAttr().Get(&extent, timeCode))
		{
			STDOUT_ERROR("Failed to get extent attr");
			return;
		}
		
		if (bbox_.isEmpty())
		{
			const Point3f minCorner = transformPoint_(xformMtx, extent[0]);
			const Point3f maxCorner = transformPoint_(xformMtx, extent[1]);
			bbox_ = BBox3f(minCorner, maxCorner);
		}
		else
		{
			bbox_.merge(transformPoint_(xformMtx, extent[0]));
			bbox_.merge(transformPoint_(xformMtx, extent[1]));
		}
	}

private:
	template<typename SCHEMA>
	void processXform_(const UsdTimeCode timeCode, const UsdPrim& usdPrim, const SCHEMA& schema)
	{
		DECL_LOGGER(base::usd);
		GfMatrix4d xformMtx = schema.ComputeLocalToWorldTransform(timeCode);
		matrixStack_.transform(Mat4x4f{
				xformMtx[0][0], xformMtx[1][0], xformMtx[2][0], xformMtx[3][0],
				xformMtx[0][1], xformMtx[1][1], xformMtx[2][1], xformMtx[3][1],
				xformMtx[0][2], xformMtx[1][2], xformMtx[2][2], xformMtx[3][2],
				xformMtx[0][3], xformMtx[1][3], xformMtx[2][3], xformMtx[3][3],
		});
	}
	
	virtual void processMesh_(const Mat4x4f& xformMtx, const UsdTimeCode timeCode, const UsdPrim& usdPrim, const UsdGeomMesh& schema)
	{
		DECL_LOGGER(base::usd);
		switch (geom_.which())
		{
			case 0:
			{
				/* set indexed geom */
				if (!setIndexedGeom_(xformMtx, timeCode, usdPrim, schema))
				{
					STDOUT_ERROR("Failed to set IndexedGeom");
					return;
				}
			}
				break;
			case 1:
			{
				/* set expanded geom */
				if (!setExpandedGeom_(xformMtx, timeCode, usdPrim, schema))
				{
					STDOUT_ERROR("Failed to set ExpandedGeom");
					return;
				}
			}
				break;
		}
	}

private:
	bool setIndexedGeom_(const Mat4x4f& xformMtx, const UsdTimeCode timeCode, const UsdPrim& usdPrim, const UsdGeomMesh& schema)
	{
		return false;
	}
	
	bool setExpandedGeom_(const Mat4x4f& xformMtx, const UsdTimeCode timeCode, const UsdPrim& usdPrim, const UsdGeomMesh& schema)
	{
		DECL_LOGGER(base::usd);
		
		std::shared_ptr<ExpandedGeom> pGeom = boost::get<std::shared_ptr<ExpandedGeom>>(geom_);
		if (!pGeom)
		{
			STDOUT_ERROR("Failed to get pGeom");
			return false;
		}
		VtVec3fArray p;
		VtVec3fArray n;
		VtIntArray fvc;
		VtIntArray fvi;
		VtVec2fArray uv;
		VtIntArray uvIndices;
		
		UsdTimeCode pointsTimeSample = UsdTimeCode::EarliestTime();
		UsdTimeCode normalsTimeSample = UsdTimeCode::EarliestTime();
		std::vector<double> pointsTimeSamples;
		//size_t pointsNumTimeSamples = 0u;
		//if (!args.GetTimeInterval().IsEmpty()) {
		//	mesh.GetPointsAttr().GetTimeSamplesInInterval(args.GetTimeInterval(),&pointsTimeSamples);
		//	if (!pointsTimeSamples..empty()) {
		//		pointsNumTimeSamples = pointsTimeSamples.size();
		//		pointsTimeSample = pointsTimeSamples.front();
		//	}
		//
		//	std::vector<double> normalsTimeSamples;
		//	mesh.GetNormalsAttr().GetTimeSamplesInInterval(args.GetTimeInterval(),
		//	                                               &normalsTimeSamples);
		//	if (!normalsTimeSamples..empty()) {
		//		normalsTimeSample = normalsTimeSamples.front();
		//	}
		//}
		
		
		
		if (!schema.GetPointsAttr().Get(&p, timeCode))
		{
			STDOUT_ERROR("Failed to get points attr");
			return false;
		}
		if (!schema.GetFaceVertexCountsAttr().Get(&fvc, timeCode))
		{
			STDOUT_ERROR("Failed to get face vertex count attr");
			return false;
		}
		if (!schema.GetFaceVertexIndicesAttr().Get(&fvi, timeCode))
		{
			STDOUT_ERROR("Failed to get face vertex indices attr");
			return false;
		}
		if (!schema.GetNormalsAttr().Get(&n, timeCode))
		{
			STDOUT_WARRNING("Failed to get normal attr");
		}
		if (!usdPrim.GetAttribute(TfToken("primvars:st")).Get(&uv, timeCode))
		{
			STDOUT_WARRNING("Failed to get primvars:st attr");
		}
		if (!usdPrim.GetAttribute(TfToken("primvars:st:indices")).Get(&uvIndices, timeCode))
		{
			STDOUT_WARRNING("Failed to get primvars:st:indices attr");
		}
		
		/* set index */
		int pointOffset = pGeom->pointArray.size();
		int nfaces = fvc.size();
		int baseIndex = 0;
		int triangleBase = 0;
		for (int fid = 0; fid < nfaces; ++fid)
		{
			int nvertices = fvc[fid];
			int ntri = nvertices - 2;
			Point3fArray pointArray(ntri * 3);
			Normal3fArray normalArray(ntri * 3);
			TexCoord3fArray uvArray(ntri * 3);
			Int32Array indexArray(ntri * 4);
			/* triangulate */
			for (int vid = 0; vid < nvertices - 2; ++vid)
			{
				/* winding = CCW */
				int id0 = triangleBase + (vid + 0);
				int id1 = triangleBase + (vid + 1);
				int id2 = triangleBase + (nvertices - 1);
				//LOGGER_DEBUG("f[%d].id = (%d, %d, %d)", fid, id0, id1, id2);
				indexArray[vid * 4 + 0] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 1] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 2] = baseIndex++ + (pointOffset + indexOffset_);
				indexArray[vid * 4 + 3] = 0xffffffff;
				
				pointArray[vid * 3 + 0] = transformPoint_(xformMtx, p[fvi[id0]]);
				pointArray[vid * 3 + 1] = transformPoint_(xformMtx, p[fvi[id1]]);
				pointArray[vid * 3 + 2] = transformPoint_(xformMtx, p[fvi[id2]]);
				
				if (!n.empty())
				{
					normalArray[vid * 3 + 0] = transformNormal_(xformMtx, n[id0]);
					normalArray[vid * 3 + 1] = transformNormal_(xformMtx, n[id1]);
					normalArray[vid * 3 + 2] = transformNormal_(xformMtx, n[id2]);
				}
				else
				{
					/*calc normal */
					const Point3f& p0 = pointArray[vid * 3 + 0];
					const Point3f& p1 = pointArray[vid * 3 + 1];
					const Point3f& p2 = pointArray[vid * 3 + 2];
					
					const Vec3f& v01 = p1 - p0;
					const Vec3f& v02 = p2 - p0;
					
					const Normal3f n = Cross(v01, v02).normalized();
					const Vec4f& transformed = xformMtx * Vec4f(n[0], n[1], n[2], 0.0);
					
					normalArray[vid * 3 + 0] = Normal3f(transformed[0], transformed[1], transformed[2]);
					normalArray[vid * 3 + 1] = Normal3f(transformed[0], transformed[1], transformed[2]);
					normalArray[vid * 3 + 2] = Normal3f(transformed[0], transformed[1], transformed[2]);
					
				}
				
				if (!uv.empty() && !uvIndices.empty())
				{
					uvArray[vid * 3 + 0] = TexCoord3f(uv[uvIndices[fvi[id0]]][0], uv[uvIndices[fvi[id0]]][0], texLayer_);
					uvArray[vid * 3 + 1] = TexCoord3f(uv[uvIndices[fvi[id1]]][0], uv[uvIndices[fvi[id1]]][1], texLayer_);
					uvArray[vid * 3 + 2] = TexCoord3f(uv[uvIndices[fvi[id2]]][0], uv[uvIndices[fvi[id2]]][2], texLayer_);
					
				}
			}
			pGeom->pointArray.insert(pGeom->pointArray.end(), pointArray.begin(), pointArray.end());
			pGeom->normalArray.insert(pGeom->normalArray.end(), normalArray.begin(), normalArray.end());
			pGeom->uvArray.insert(pGeom->uvArray.end(), uvArray.begin(), uvArray.end());
			pGeom->indexArray.insert(pGeom->indexArray.end(), indexArray.begin(), indexArray.end());
			triangleBase += nvertices;
		}
		return true;
	}
	
	const Vec3f transformPoint_(const Mat4x4f& xformMtx, const GfVec3f& p)
	{
		const Vec4f& transformed = xformMtx * Vec4f(p[0], p[1], p[2], 1.0);
		return Point3f(transformed[0], transformed[1], transformed[2]);
	}
	
	const Vec3f transformNormal_(const Mat4x4f& xformMtx, const GfVec3f& v)
	{
		const Vec4f& transformed = xformMtx * Vec4f(v[0], v[1], v[2], 0.0);
		return Point3f(transformed[0], transformed[1], transformed[2]);
	}


private:
	template<class SchemaT>
	UsdTimeCode getTargetTimeCode_(const short animCycle, const UsdTimeCode timeCode, const SchemaT& schema)
	{
		DECL_LOGGER(base::usd);
		
		std::vector<double> timeSamples;
		schema.GetTimeSamples(&timeSamples);
		
		if (!timeSamples.size() > 0)
			return timeCode;
		
		const double time = timeCode.GetValue();
		const int nsamples = timeSamples.size();
		const double minTime = timeSamples[0];
		const double maxTime = timeSamples[timeSamples.size() - 1];
		
		LOGGER_DEBUG("minTime = %5.2f", minTime);
		LOGGER_DEBUG("maxTime = %5.2f", maxTime);
		
		double target = 0.0;
		switch (animCycle)
		{
			case mode::animCycle::none:
				LOGGER_DEBUG("animCycle = none");
				target = Time::ClampTime(time, minTime, maxTime);
				break;
			case mode::animCycle::looped:
				LOGGER_DEBUG("animCycle = looped");
				target = Time::LoopedTime(time, minTime, maxTime);
				break;
			case mode::animCycle::osciilated:
				LOGGER_DEBUG("animCycle = osciilated");
				target = Time::OsciilateTime(time, minTime, maxTime);
				break;
		}
		return UsdTimeCode(target);
	}
	
	void getVisibility(const UsdTimeCode timeCode, const bool *pOutVisible)
	{
	}

private:
};

inline std::shared_ptr<ExpandedGeom> CreateExpandedLODGeom(const TfToken purpose,
                                                           const short animCycle,
                                                           const double sec,
                                                           const BBox3f& bbox,
                                                           const UsdReader& reader0,
                                                           const UsdReader& reader1 = UsdReader(),
                                                           const UsdReader& reader2 = UsdReader()
)
{
	DECL_LOGGER(base::usd);
	if (!reader0.valid())
	{
		STDOUT_ERROR("Failed to get valid usdReader0");
		return nullptr;
	}
	UsdReader *pNonConstReader0 = (UsdReader *) (&reader0);
	
	int pointOffset = 0;
	/* set lod0 */
	std::shared_ptr<ExpandedGeom> pGeom = pNonConstReader0->expandedSample(purpose, animCycle, sec, pointOffset);
	pGeom->drawCmd[0].indexOffset = 0;
	pGeom->drawCmd[0].indexCount = pGeom->indexArray.size();
	pointOffset += pGeom->pointArray.size();
	
	std::shared_ptr<ExpandedGeom> pGeom1 = nullptr;
	if (reader1.valid())
	{
		UsdReader *pNonConstReader1 = (UsdReader *) (&reader1);
		pGeom1 = pNonConstReader1->expandedSample(purpose, animCycle, sec, pointOffset);
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
	pGeom->drawCmd[1].indexCount = pGeom1->indexArray.size();
	pGeom->pointArray.insert(pGeom->pointArray.end(), pGeom1->pointArray.begin(), pGeom1->pointArray.end());
	pGeom->normalArray.insert(pGeom->normalArray.end(), pGeom1->normalArray.begin(), pGeom1->normalArray.end());
	pGeom->uvArray.insert(pGeom->uvArray.end(), pGeom1->uvArray.begin(), pGeom1->uvArray.end());
	pGeom->indexArray.insert(pGeom->indexArray.end(), pGeom1->indexArray.begin(), pGeom1->indexArray.end());
	pointOffset += pGeom1->pointArray.size();
	
	std::shared_ptr<ExpandedGeom> pGeom2 = nullptr;
	if (reader2.valid())
	{
		UsdReader *pNonConstReader2 = (UsdReader *) (&reader2);
		pGeom2 = pNonConstReader2->expandedSample(purpose, animCycle, sec, pointOffset);
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
	pGeom->drawCmd[2].indexCount = pGeom2->indexArray.size();
	pGeom->pointArray.insert(pGeom->pointArray.end(), pGeom2->pointArray.begin(), pGeom2->pointArray.end());
	pGeom->normalArray.insert(pGeom->normalArray.end(), pGeom2->normalArray.begin(), pGeom2->normalArray.end());
	pGeom->uvArray.insert(pGeom->uvArray.end(), pGeom2->uvArray.begin(), pGeom2->uvArray.end());
	pGeom->indexArray.insert(pGeom->indexArray.end(), pGeom2->indexArray.begin(), pGeom2->indexArray.end());
	pointOffset += pGeom2->pointArray.size();
	
	return pGeom;
}


NAMESPACE_BASE_END