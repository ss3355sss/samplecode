#pragma once
#include "baseUsd/pch.h"

#include "base/bbox.h"
#include "base/frame.h"
#include "base/mathUtil.h"
#include "base/points.h"

#include <pxr/pxr.h>

#include <pxr/usd/ar/resolver.h>

#include <pxr/usd/sdf/layer.h>

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/stageCache.h>
#include <pxr/usd/usd/stageCacheContext.h>
#include <pxr/usd/usd/variantSets.h>

#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/matrix4d.h>

#include <pxr/usd/usdGeom/points.h>
#include <pxr/usd/usdGeom/pointInstancer.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xform.h>

#include <pxr/usd/usdUtils/pipeline.h>
#include <pxr/usd/usdUtils/stageCache.h>

PXR_NAMESPACE_USING_DIRECTIVE

NAMESPACE_BASE_BEGIN

typedef std::vector<std::pair<std::string, std::string> > VariantSelects;
typedef Array1<UsdPrim> UsdPrimArray;

class UsdImporterBase
{
protected:
	UsdStageRefPtr  stage_;
	UsdStageCache   stageCache_;
	UsdPrim         defaultPrim_;
	
	GfBBox3d worldBnd_;
	
public:
	UsdImporterBase()
	{
	}
	~UsdImporterBase()
	{
	}

public:
	bool open(const std::string fileName,
	          const std::string primPath = "",
	          const VariantSelects variantSelects = VariantSelects(0))
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		SdfLayerRefPtr rootLayer = SdfLayer::FindOrOpen(fileName);
		if (!rootLayer)
		{
			STDOUT_ERROR("Failed to open %s:%s", fileName.c_str(), primPath.c_str());
			return false;
		}
		
		TfToken modelName = UsdUtilsGetModelNameFromRootLayer(rootLayer);
		UsdStageRefPtr usdStage;
		SdfLayerRefPtr sessionLayer;
		
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
		
		defaultPrim_ = primPath.empty() ? stage_->GetDefaultPrim() : stage_->GetPrimAtPath(SdfPath(primPath));
		if (!defaultPrim_ && !(primPath.empty() || primPath == "/"))
		{
			STDOUT_WARRNING("Unable to set root prim to <%s> when reading USD file '%s'; " "using the pseudo-root </> instead", primPath.c_str(), fileName.c_str());
			defaultPrim_ = stage_->GetPseudoRoot();
		}
		
		if (!defaultPrim_)
		{
			STDOUT_ERROR("No default prim found in USD file '%s'", fileName.c_str());
			return false;
		}
		//LOGGER_DEBUG("usdRootPrim = %s", defaultPrim_.GetPath().GetText());
		
		for (int i = 0; i < variantSelects.size(); ++i)
		{
			LOGGER_DEBUG("variantSelects[i] = %s, value = %s", variantSelects[i].first.c_str(), variantSelects[i].second.c_str());
			UsdVariantSet variantSet = defaultPrim_.GetVariantSet(variantSelects[i].first);
			if (!variantSet.SetVariantSelection(variantSelects[i].second))
			{
				STDOUT_ERROR("Failed to set variant, %s", variantSelects[i].first.c_str());
			}
		}
		return true;
	}

public:
	const UsdStageRefPtr stage() const
	{
		return stage_;
	}
	const UsdPrim& defaultPrim() const
	{
		return defaultPrim_;
	}
	

protected:
	void usdTree_(const bool&        detail,
	              const TfToken&     targetPurpose,
	              const short&       animCycle,
	              const UsdTimeCode& timeCode,
	              const UsdPrim&     usdPrim,
	              const bool&        inheritedVisible
	)
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		if(!inheritedVisible)
			return;
		
		UsdTimeCode targetTimeCode    = UsdTimeCode::EarliestTime();
		bool visible                  = true;
		
		TfToken typeName = usdPrim.GetTypeName();
		LOGGER_IN("%s, (%s)", usdPrim.GetName().GetText(), typeName.GetText());
		{
			if(usdPrim.IsA<UsdGeomXform>())
			{
				targetTimeCode = computeTargetTimeCode_(animCycle, timeCode, usdPrim);
				visible        = computeVisible_(targetTimeCode, usdPrim);
				if(!visible)
					return;
				
				//TfToken purpose = computePurpose(targetTimeCode, usdPrim);
				//if(purpose == UsdGeomTokens->default_ || purpose == targetPurpose)
				//{
				//	if (!processWorldBound_(targetTimeCode, usdPrim))
				//		STDOUT_WARRNING("Failed to process world bound");
				//
				//	if (detail && !processUsdGeomXform_(targetTimeCode, UsdGeomXform(usdPrim)))
				//		STDOUT_WARRNING("Failed to process UsdGeomXform Schema");
				//}
			}
			else if(usdPrim.IsA<UsdGeomMesh>())
			{
				targetTimeCode = computeTargetTimeCode_(animCycle, timeCode, usdPrim);
				visible        = computeVisible_(targetTimeCode, usdPrim);
				if(!visible)
					return;
				
				TfToken purpose = computePurpose_(targetTimeCode, usdPrim);
				if(purpose == UsdGeomTokens->default_ || purpose == targetPurpose)
				{
					if (detail && !processUsdGeomMesh_(targetTimeCode, UsdGeomMesh(usdPrim)))
						STDOUT_WARRNING("Failed to process UsdGeomXform Schema");
				}
			}
			else if(usdPrim.IsA<UsdGeomPointInstancer>())
			{
				targetTimeCode = computeTargetTimeCode_(animCycle, timeCode, usdPrim);
				visible        = computeVisible_(targetTimeCode, usdPrim);
				if(!visible)
					return;
				
				TfToken purpose = computePurpose_(targetTimeCode, usdPrim);
				if(purpose == UsdGeomTokens->default_ || purpose == targetPurpose)
				{
					if (!processUsdGeomProtoTypeRelationship_(targetTimeCode, UsdGeomPointInstancer(usdPrim)))
						STDOUT_WARRNING("Failed to process processUsdGeomProtoTypeRelationship Schema");
					
					if (detail && !processUsdGeomPointInstancer_(targetTimeCode, UsdGeomPointInstancer(usdPrim)))
						STDOUT_WARRNING("Failed to process UsdGeomPointInstancer Schema");
				}
			}
			
			UsdPrimSiblingRange siblingRange = usdPrim.GetChildren();
			for (auto iter = siblingRange.begin(); iter != siblingRange.end(); ++iter)
			{
				usdTree_(detail, targetPurpose, animCycle, timeCode, *iter, visible);
			}
		}
		LOGGER_OUT();
	}

protected:
	virtual bool processUsdGeomXform_(const UsdTimeCode&    timeCode,
	                                  const UsdGeomXform&   schema)
	{
		return true;
	}
	virtual bool processUsdGeomMesh_(const UsdTimeCode& timeCode,
	                                 const UsdGeomMesh& schema)
	{
		return true;
	}
	virtual bool processUsdGeomProtoTypeRelationship_(const UsdTimeCode&               timeCode,
	                                                  const UsdGeomPointInstancer&     schema)
	{
		return true;
	}
	virtual bool processUsdGeomPointInstancer_(const UsdTimeCode&               timeCode,
	                                           const UsdGeomPointInstancer&     schema)
	{
		return true;
	}
protected:
	bool processWorldBound_(const UsdTimeCode&  timeCode,
	                        const UsdPrim&      usdPrim,
	                        const TfToken&      purpose1=TfToken(),
	                        const TfToken&      purpose2=TfToken(),
	                        const TfToken&      purpose3=TfToken(),
	                        const TfToken&      purpose4=TfToken())
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		
		if(!usdPrim.IsA<UsdGeomBoundable>())
		{
			STDOUT_WARRNING("Failed to get UsdGeomBoundable Schema");
			return false;
		}
		
		UsdGeomBoundable schema(usdPrim);
		GfBBox3d worldBnd = schema.ComputeWorldBound(timeCode,
		                                             purpose1,
		                                             purpose2,
		                                             purpose3,
		                                             purpose4
		);
		if(worldBnd.GetVolume() == 0.0)
		{
			STDOUT_WARRNING("Failed to get valid world bound");
			return false;
		}

		if(worldBnd_.GetVolume() == 0.0)
		{
			worldBnd_= worldBnd;
		}
		else
		{
			worldBnd_ = GfBBox3d::Combine(worldBnd_, worldBnd);
		}
		return true;
	}

protected:
	TfToken computePurpose_(const UsdTimeCode&  timeCode,
	                        const UsdPrim& usdPrim)
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		
		if(!usdPrim.IsA<UsdGeomImageable>())
		{
			STDOUT_WARRNING("Failed to get UsdGeomImageable Schema");
			return UsdGeomTokens->default_;
		}
		UsdGeomImageable schema(usdPrim);
		return schema.ComputePurpose();
	}
	GfMatrix4d  computeLocalToWorldTransform_(const UsdTimeCode&  timeCode,
	                                          const UsdPrim&      usdPrim)
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		
		if(!usdPrim.IsA<UsdGeomXformable>())
		{
			STDOUT_WARRNING("Failed to get UsdGeomXformable Schema");
			return GfMatrix4d(1.0);
		}
		UsdGeomXformable schema(usdPrim);
		return schema.ComputeLocalToWorldTransform(timeCode);
		
	}

	bool computeVisible_(const UsdTimeCode&  timeCode,
	                     const UsdPrim&      usdPrim)
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		LOGGER_DEBUG("TypeName = %s", usdPrim.GetTypeName().GetText());
		
		if(!usdPrim.IsA<UsdGeomImageable>())
		{
			STDOUT_WARRNING("Failed to get UsdGeomImageable Schema");
			return false;
		}
		UsdGeomXformable schema(usdPrim);
		return (schema.ComputeVisibility(timeCode) == UsdGeomTokens->invisible) ? false : true;
	}
	UsdTimeCode computeTargetTimeCode_(const short&         animCycle,
	                                   const UsdTimeCode&   timeCode,
	                                   const UsdPrim&       usdPrim)
	{
		//DECL_LOGGER(logging::baseUsd->UsdImporter)
		DECL_LOGGER(false)
		
		if(!usdPrim.IsA<UsdGeomXformable>())
		{
			STDOUT_WARRNING("Failed to get UsdGeomXformable Schema!");
			return UsdTimeCode::EarliestTime();
		}
		//UsdGeomXformable schema(usdPrim);
		//
		//std::vector<double> timeSamples;
		//schema.GetTimeSamples(&timeSamples);
		//if (!timeSamples.size() > 0)
		//	return timeCode;
		//
		//const double time = timeCode.GetValue();
		//const int nsamples = timeSamples.size();
		//const double minTime = timeSamples[0];
		//const double maxTime = timeSamples[timeSamples.size() - 1];
		
		//LOGGER_DEBUG("minTime = %5.2f", minTime);
		//LOGGER_DEBUG("maxTime = %5.2f", maxTime);
		
		double target = 0.0;
		//switch (animCycle)
		//{
		//	case mode::animCycle::none:
		//		LOGGER_DEBUG("animCycle = none");
		//		target = Time::ClampTime(time, minTime, maxTime);
		//		break;
		//	case mode::animCycle::looped:
		//		LOGGER_DEBUG("animCycle = looped");
		//		target = Time::LoopedTime(time, minTime, maxTime);
		//		break;
		//	case mode::animCycle::osciilated:
		//		LOGGER_DEBUG("animCycle = osciilated");
		//		target = Time::OsciilateTime(time, minTime, maxTime);
		//		break;
		//}
		return UsdTimeCode(target);
	}
	
	
};
NAMESPACE_BASE_END

//class UsdGeomMeshImporter : public UsdImporterBase;



NAMESPACE_BASE_BEGIN
class UsdGeomPointInstancerImporter : public UsdImporterBase
{
public:
	typedef UsdImporterBase Base;
private:
	std::shared_ptr<Points>         pPoints_;
	std::shared_ptr<UsdPrimArray>   pProtoPrims_;
	
	bool hasLocalAxis_;
	bool hasRootS_;
	bool hasRootT_;
	bool hasRadius_;


public:
	UsdGeomPointInstancerImporter() : Base(),
	                                  pPoints_(nullptr),
	                                  pProtoPrims_(nullptr),
	                                  hasLocalAxis_(false),
	                                  hasRootS_(false),
	                                  hasRootT_(false),
	                                  hasRadius_(false)
	{
	}
	virtual ~UsdGeomPointInstancerImporter()
	{
	}

public:
	const std::shared_ptr<UsdPrimArray> getProtoPrims()
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		if (!defaultPrim_)
		{
			STDOUT_ERROR("Failed to get valid usdPrim");
			return nullptr;
		}
		
		pProtoPrims_ = std::make_shared<UsdPrimArray>();
		
		usdTree_(false,
		         UsdGeomTokens->default_,
		         mode::animCycle::none,
		         UsdTimeCode::EarliestTime(),
		         defaultPrim(),
		         true
		);
		return pProtoPrims_;
	}
	const std::shared_ptr<Points> getPoints(const short animCycle, const UsdTimeCode timeCode)
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		if (!defaultPrim_)
		{
			STDOUT_ERROR("Failed to get valid usdPrim");
			return nullptr;
		}
		pPoints_ = Points::Create();
		
		usdTree_(true,
		         UsdGeomTokens->default_,
		         animCycle,
		         timeCode,
		         defaultPrim(),
		         true
		);
		
		return pPoints_;
	}
	
	const std::shared_ptr<Points> getPoints(const short animCycle,
	                                        const UsdTimeCode timeCode,
	                                        const std::initializer_list<Uniforms::value_type>& ulst,
	                                        const std::initializer_list<Attributes::value_type>& alst)
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		if (!defaultPrim_)
		{
			STDOUT_ERROR("Failed to get valid usdPrim");
			return nullptr;
		}
		pPoints_ = Points::Create(0, ulst, alst);
		
		usdTree_(true,
		         UsdGeomTokens->default_,
		         animCycle,
		         timeCode,
		         defaultPrim(),
		         true
		);
		
		return pPoints_;
	}
	
	void getInstancerSet()
	{
	}
public:
	bool hasLocalAxis() const
	{
		return hasLocalAxis_;
	};
	bool hasRootS() const
	{
		return hasRootS_;
	};
	bool hasRootT() const
	{
		return hasRootT_;
	};
	bool hasRadius() const
	{
		return hasRadius_;
	};



protected:
	virtual bool processUsdGeomProtoTypeRelationship_(const UsdTimeCode&               timeCode,
	                                                  const UsdGeomPointInstancer&     schema)
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		UsdRelationship prototypesRel = schema.GetPrototypesRel();
		
		SdfPathVector targets;
		if(!prototypesRel.GetTargets(&targets))
		{
			STDOUT_ERROR("Failed to get protoType targets");
			return false;
		}
		
		pProtoPrims_ = std::make_shared<UsdPrimArray>(targets.size());
		for(int i = 0; i < targets.size(); ++i)
		{
			(*pProtoPrims_)[i] = stage_->GetPrimAtPath(targets[i]);
		}
		return true;
	}
	virtual bool processUsdGeomPointInstancer_(const UsdTimeCode&           timeCode,
	                                           const UsdGeomPointInstancer& schema)
	{
		DECL_LOGGER(logging::baseUsd->UsdImporter)
		
		VtInt64Array     vtPidArray;
		VtIntArray       vtSidArray;
		VtInt64Array     vtInvisArray;
		
		VtVec3fArray     vtPointArray;
		VtQuathArray     vtOrientationArray;
		VtVec3fArray     vtScaleArray;
		VtMatrix3dArray  vtLocalAxisArray;
		
		VtFloatArray     vtRootSArray;
		VtFloatArray     vtRootTArray;

		VtFloatArray     vtRadiusArray;
		
		if(!schema.GetIdsAttr().Get(&vtPidArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtPidArray");
			return false;
		}
		if(!schema.GetProtoIndicesAttr().Get(&vtSidArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtSidArray");
			return false;
		}
		if(!schema.GetInvisibleIdsAttr().Get(&vtInvisArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtInvisArray");
			return false;
		}
		if(!schema.GetPositionsAttr().Get(&vtPointArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtPointArray");
			return false;
		}
		if(!schema.GetOrientationsAttr().Get(&vtOrientationArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtOrientationArray");
			return false;
		}
		if(!schema.GetScalesAttr().Get(&vtScaleArray, timeCode))
		{
			STDOUT_ERROR("Failed to get vtScaleArray");
			return false;
		}

		const UsdPrim& usdPrim = schema.GetPrim();
		if(usdPrim.HasAttribute(TfToken("localAxis")))
		{
			hasLocalAxis_ = true;
			if(!usdPrim.GetAttribute(TfToken("localAxis")).Get(&vtLocalAxisArray, timeCode))
				STDOUT_WARRNING("Failed to get vtLocalAxisArray");
		}
		if(usdPrim.HasAttribute(TfToken("rootS")))
		{
			hasRootS_ = true;
			if(!usdPrim.GetAttribute(TfToken("rootS")).Get(&vtRootSArray, timeCode))
				STDOUT_WARRNING("Failed to get vtRootSArray");
		}
		if(usdPrim.HasAttribute(TfToken("rootT")))
		{
			hasRootT_ = true;
			if(!usdPrim.GetAttribute(TfToken("rootT")).Get(&vtRootTArray, timeCode))
				STDOUT_WARRNING("Failed to get vtRootTArray");
		}
		if(usdPrim.HasAttribute(TfToken("radius")))
		{
			hasRadius_ = true;
			if(!usdPrim.GetAttribute(TfToken("radius")).Get(&vtRadiusArray, timeCode))
				STDOUT_WARRNING("Failed to get vtRadiusArray");
		}


		const size_t                    npoints = vtPidArray.size();
		const std::shared_ptr<Points>   pPoints = Points::Create(npoints);

		Int32Array&       pidArray        = pPoints->get<int>(sementic::attribute::pid);
		Int32Array&       sidArray        = pPoints->get<int>(sementic::attribute::sid);
		Int32Array&       renderableArray = pPoints->get<int>(sementic::attribute::renderable);
		
		
		Point3fArray&     pointArray      = pPoints->get();
		Vec3fArray&       eulerAngleArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle);
		Vec3fArray&       scaleArray      = pPoints->get<Vec3f>(sementic::attribute::scale);

		Mat3x3fArray&     localAxisArray  = pPoints->get<Mat3x3f>(sementic::attribute::localAxis);

		TexCoord3fArray&  uvCoordArray    = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		FloatArray&       radiusArray     = pPoints->get<float>(sementic::attribute::radius);
		
		bool useOpenMP = false; //segmentation fault when useOpenMP is true, why? no race condition..why!!!!!!!!!!!!!!
#pragma omp parallel for if( useOpenMP )
		for(size_t i = 0; i < npoints; ++i)
		{
			//cout << "i = " << i << ", ";
			pidArray[i] = vtPidArray[i];
			sidArray[i] = vtSidArray[i];
			
			pointArray[i]       = Point3f(vtPointArray[i][0],vtPointArray[i][1], vtPointArray[i][2]);
			eulerAngleArray[i]  = QuternionToEulerAngle(Quatf(vtOrientationArray[i].GetReal(),
			                                                  vtOrientationArray[i].GetImaginary()[0],
			                                                  vtOrientationArray[i].GetImaginary()[1],
			                                                  vtOrientationArray[i].GetImaginary()[2]
					));
			scaleArray[i]       = Vec3f(vtScaleArray[i][0],vtScaleArray[i][1], vtScaleArray[i][2]);
			
			if(vtLocalAxisArray.size() > 0)
			{
				GfMatrix3d localAxisGfMtx = vtLocalAxisArray[i];
				localAxisArray[i] = Mat3x3f(localAxisGfMtx[0][0], localAxisGfMtx[1][0], localAxisGfMtx[2][0],
				                            localAxisGfMtx[0][1], localAxisGfMtx[1][1], localAxisGfMtx[2][1],
				                            localAxisGfMtx[0][2], localAxisGfMtx[1][2], localAxisGfMtx[2][2]
				);
			}
			if(vtRootSArray.size() > 0)
			{
				uvCoordArray[i][0] = vtRootSArray[i];
			}
			if(vtRootTArray.size() > 0)
			{
				uvCoordArray[i][1] = vtRootTArray[i];
			}
			if(vtRadiusArray.size() > 0)
			{
				radiusArray[i] = vtRadiusArray[i];
			}
		}
		if(vtInvisArray.size() > 0)
		{
#pragma omp parallel for if( useOpenMP )
			for(size_t i = 0; i < vtInvisArray.size(); ++i)
			{
				const int64_t pid = vtInvisArray[i];
				renderableArray[pid] = false;
			}
		}
		pPoints_->append(*pPoints.get());
		return true;
	}
private:
	void setPoints_(const std::shared_ptr<Points> pPoints)
	{
		pPoints_ = pPoints;
	}

};


NAMESPACE_BASE_END
