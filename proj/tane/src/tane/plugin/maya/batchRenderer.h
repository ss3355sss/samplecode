#pragma once

#include "maya/pch.h"
#include <pxr/pxr.h>
#include <pxr/base/arch/hash.h>

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/vec4d.h>
#include <pxr/base/gf/gamma.h>

#include <pxr/base/tf/debug.h>
#include <pxr/base/tf/staticTokens.h>
#include <pxr/base/tf/registryManager.h>

#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/stage.h>

#include <pxr/imaging/glf/simpleLightingContext.h>

#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/enums.h>
#include <pxr/imaging/hd/camera.h>
#include <pxr/imaging/hd/renderIndex.h>
#include <pxr/imaging/hd/sceneDelegate.h>
#include <pxr/imaging/hd/tokens.h>
#include <pxr/imaging/hd/version.h>

#include <pxr/imaging/glf/glew.h>

#include <pxr/imaging/hdSt/renderDelegate.h>
#include <pxr/imaging/hdSt/light.h>

#include <pxr/imaging/hdx/intersector.h>
#include <pxr/imaging/hdx/renderTask.h>
#include <pxr/imaging/hdx/selectionTask.h>
#include <pxr/imaging/hdx/simpleLightTask.h>

#include <pxr/usdImaging/usdImaging/delegate.h>
#include <pxr/usdImaging/usdImaging/tokens.h>

#include <pxrUsdMayaGL/softSelectHelper.h>
#include <px_vp20/utils.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEBUG_CODES(
    PXRUSDMAYAGL_QUEUE_INFO
);
TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    //(idRenderTask)
    (renderTask)
    (selectionTask)
    (simpleLightTask)
    (camera)
    (render)
    
    ((MayaEndRenderNotificationName, "TN_UsdMayaEndRenderNotification"))
);
PXR_NAMESPACE_CLOSE_SCOPE

NAMESPACE_BASE_BEGIN
PXR_NAMESPACE_USING_DIRECTIVE

inline MHWRender::DisplayStatus _ToMHWRenderDisplayStatus(const M3dView::DisplayStatus& displayStatus)
{
    // these enums are equivalent, but statically checking just in case.
    static_assert(((int) M3dView::kActive == (int) MHWRender::kActive)
                  && ((int) M3dView::kLive == (int) MHWRender::kLive)
                  && ((int) M3dView::kDormant == (int) MHWRender::kDormant)
                  && ((int) M3dView::kInvisible == (int) MHWRender::kInvisible)
                  && ((int) M3dView::kHilite == (int) MHWRender::kHilite)
                  && ((int) M3dView::kTemplate == (int) MHWRender::kTemplate)
                  && ((int) M3dView::kActiveTemplate == (int) MHWRender::kActiveTemplate)
                  && ((int) M3dView::kActiveComponent == (int) MHWRender::kActiveComponent)
                  && ((int) M3dView::kLead == (int) MHWRender::kLead)
                  && ((int) M3dView::kIntermediateObject == (int) MHWRender::kIntermediateObject)
                  && ((int) M3dView::kActiveAffected == (int) MHWRender::kActiveAffected)
                  && ((int) M3dView::kNoStatus == (int) MHWRender::kNoStatus),
                  "M3dView::DisplayStatus == MHWRender::DisplayStatus");
    return MHWRender::DisplayStatus((int) displayStatus);
}

inline bool _GetWireframeColor(const UsdMayaGLSoftSelectHelper& softSelectHelper,
                               const MDagPath& objPath,
                               const MHWRender::DisplayStatus& displayStatus,
                               MColor* mayaWireColor)
{
    // dormant objects may be included in soft selection.
    if(displayStatus == MHWRender::kDormant)
    {
        return softSelectHelper.GetFalloffColor(objPath, mayaWireColor);
    }
    else if((displayStatus == MHWRender::kActive) ||
            (displayStatus == MHWRender::kLead) ||
            (displayStatus == MHWRender::kHilite))
    {
        *mayaWireColor = MHWRender::MGeometryUtilities::wireframeColor(objPath);
        return true;
    }

    return false;
}


class _BatchDrawUserData : public MUserData
{
public:
	bool                            _drawShape;
	boost::scoped_ptr<MBoundingBox> _bounds;
	boost::scoped_ptr<GfVec4f>      _wireframeColor;
public:
	_BatchDrawUserData()
		: MUserData(/* deleteAfterUse = */ false), _drawShape(true)
	{
	}
	
	_BatchDrawUserData(const bool drawShape,
	                   const MBoundingBox& bounds,
	                   const GfVec4f& wireFrameColor)
		: MUserData(/* deleteAfterUse = */ false),
		  _drawShape(drawShape),
		  _bounds(new MBoundingBox(bounds)),
		  _wireframeColor(new GfVec4f(wireFrameColor))
	{
	}
	virtual ~_BatchDrawUserData()
	{
	}
};

typedef boost::shared_ptr<class HdxIntersector> HdxIntersectorSharedPtr;
class UsdMayaGLBatchRenderer : private boost::noncopyable
{
public:
    struct RenderParams
    {
        // USD Params
        UsdTimeCode frame       = UsdTimeCode::Default();
        uint8_t     refineLevel = 0;
        TfToken     geometryCol = HdTokens->geometry;
        
        // Raster Params
        bool enableLighting = true;
        
        // Geometry Params
        HdCullStyle   cullStyle = HdCullStyleNothing;
        TfToken       drawRepr  = HdReprTokens->refined;
        TfTokenVector renderTags;
        
        // Color Params
        GfVec4f overrideColor  = {.0f, .0f, .0f, .0f};
        GfVec4f wireframeColor = {.0f, .0f, .0f, .0f};
        
        /// \brief Helper function to find a batch key for the render params
        size_t Hash() const
        {
            size_t hash = (refineLevel << 1) + enableLighting;
            boost::hash_combine(hash, frame);
            boost::hash_combine(hash, geometryCol);
            boost::hash_combine(hash, cullStyle);
            boost::hash_combine(hash, drawRepr);
            boost::hash_combine(hash, overrideColor);
            boost::hash_combine(hash, wireframeColor);
            
            return hash;
        }
    };
    class ShapeRenderer
    {
    public:
        friend class UsdMayaGLBatchRenderer;
    private:
        SdfPath                             _sharedId;
        UsdPrim                             _rootPrim;
        SdfPathVector                       _excludedPaths;
        GfMatrix4d                          _rootXform;
        RenderParams                        _baseParams;
        bool                                _isPopulated;
        std::shared_ptr<UsdImagingDelegate> _delegate;
        UsdMayaGLBatchRenderer*             _batchRenderer;
    public:
        ShapeRenderer() : _isPopulated(false), _batchRenderer(NULL)
        {
        }
        void Init(HdRenderIndex* renderIndex, const SdfPath& sharedId, const UsdPrim& rootPrim, const SdfPathVector& excludedPaths)
        {
            _sharedId      = sharedId;
            _rootPrim      = rootPrim;
            _excludedPaths = excludedPaths;
            
            _delegate.reset(new UsdImagingDelegate(renderIndex, _sharedId));
            
        }
        void PrepareForQueue(const MDagPath& objPath,
                             UsdTimeCode time,
                             uint8_t refineLevel,
                             bool showGuides,
                             bool showRenderGuides,
                             short purpose,
                             bool tint,
                             GfVec4f tintColor
        )
        {
            // Initialization of default parameters go here. These parameters get used
            // in all viewports and for selection.
            //
            _baseParams.frame       = time;
            _baseParams.refineLevel = refineLevel;
            
            // XXX Not yet adding ability to turn off display of proxy geometry, but
            // we should at some point, as in usdview
            _baseParams.renderTags.clear();
            _baseParams.renderTags.push_back(HdTokens->geometry);
            switch (purpose)
            {
                case 0:     //proxy
                    _baseParams.renderTags.push_back(HdTokens->proxy);
                    break;
                case 1:     //render
                    _baseParams.renderTags.push_back(_tokens->render);
                    break;
            }
            
            if(tint)
            {
                _baseParams.overrideColor = tintColor;
            }
            
            if(_delegate)
            {
                MStatus status;
                MMatrix transform = objPath.inclusiveMatrix(&status);
                if(status)
                {
                    _rootXform = GfMatrix4d(transform.matrix);
                    _delegate->SetRootTransform(_rootXform);
                }
                
                _delegate->SetRefineLevelFallback(refineLevel);
                
                // Will only react if time actually changes.
                _delegate->SetTime(time);
                
                /* TODO:: */
                //_delegate->SetRootCompensation(_rootPrim.GetPath());
                
                if(!_isPopulated)
                {
                    _batchRenderer->_populateQueue.insert(this);
                }
            }
        }
        
        RenderParams GetRenderParams(const MDagPath& objPath, const M3dView::DisplayStyle& displayStyle, const M3dView::DisplayStatus& displayStatus, bool* drawShape, bool* drawBoundingBox)
        {
            // VP 1.0 Implementation
            //
            RenderParams params(_baseParams);
            
            // VP 1.0 deos not allow shapes and bounding boxes to be drawn at the
            // same time...
            //
            *drawBoundingBox       = (displayStyle == M3dView::kBoundingBox);
            *drawShape             = !*drawBoundingBox;
            
            // If obj is selected, we set selected and the wireframe color.
            // QueueShapeForDraw(...) will later break this single param set into
            // two, to perform a two-pass render.
            //
            MColor mayaWireframeColor;
            bool         needsWire = _GetWireframeColor(_batchRenderer->GetSoftSelectHelper(),
                                                        objPath,
                                                        _ToMHWRenderDisplayStatus(displayStatus),
                                                        &mayaWireframeColor);
            
            if(needsWire)
            {
                params.wireframeColor = GfConvertDisplayToLinear(GfVec4f(mayaWireframeColor.r, mayaWireframeColor.g, mayaWireframeColor.b,1.f));
            }
            
            switch (displayStyle)
            {
                case M3dView::kWireFrame:
                {
                    params.drawRepr       = HdReprTokens->refinedWire;
                    params.enableLighting = false;
                    break;
                }
                case M3dView::kGouraudShaded:
                {
                    if(needsWire)
                        params.drawRepr = HdReprTokens->refinedWireOnSurf;
                    else
                        params.drawRepr = HdReprTokens->refined;
                    break;
                }
                case M3dView::kFlatShaded:
                {
                    if(needsWire)
                        params.drawRepr = HdReprTokens->refinedWireOnSurf;
                    else
                        params.drawRepr = HdReprTokens->hull;
                    break;
                }
                case M3dView::kPoints:
                {
                    // Points mode is not natively supported by Hydra, so skip it...
                }
                default:
                {
                    *drawShape = false;
                }
            };
            
            return params;
        }
        
        RenderParams GetRenderParams(const MDagPath& objPath, const unsigned int& displayStyle, const MHWRender::DisplayStatus& displayStatus, bool* drawShape, bool* drawBoundingBox)
        {
            // VP 2.0 Implementation
            //
            RenderParams params(_baseParams);
            
            *drawShape       = true;
            *drawBoundingBox = (displayStyle & MHWRender::MFrameContext::DisplayStyle::kBoundingBox);
            
            // If obj is selected, we set the wireframe color.
            // QueueShapeForDraw(...) will later break this single param set into
            // two, to perform a two-pass render.
            //
            MColor mayaWireframeColor;
            bool needsWire   = _GetWireframeColor(_batchRenderer->GetSoftSelectHelper(),
                                                  objPath,
                                                  displayStatus,
                                                  &mayaWireframeColor);
            
            if(needsWire)
            {
                params.wireframeColor = GfVec4f(mayaWireframeColor.r,
                                                mayaWireframeColor.g,
                                                mayaWireframeColor.b,
                                                1.f
                );
            }

// Maya 2015 lacks MHWRender::MFrameContext::DisplayStyle::kFlatShaded for whatever reason...
            bool         flatShaded =
#if MAYA_API_VERSION >= 201600
                             displayStyle & MHWRender::MFrameContext::DisplayStyle::kFlatShaded;
#else
            false;
#endif
            
            if(flatShaded)
            {
                if(needsWire)
                    params.drawRepr = HdReprTokens->wireOnSurf;
                else
                    params.drawRepr = HdReprTokens->hull;
            }
            else if(displayStyle & MHWRender::MFrameContext::DisplayStyle::kGouraudShaded)
            {
                if(needsWire || (displayStyle & MHWRender::MFrameContext::DisplayStyle::kWireFrame))
                    params.drawRepr = HdReprTokens->refinedWireOnSurf;
                else
                    params.drawRepr = HdReprTokens->refined;
            }
            else if(displayStyle & MHWRender::MFrameContext::DisplayStyle::kWireFrame)
            {
                params.drawRepr       = HdReprTokens->refinedWire;
                params.enableLighting = false;
            }
            else
            {
                *drawShape = false;
            }

// Maya 2016 SP2 lacks MHWRender::MFrameContext::DisplayStyle::kBackfaceCulling for whatever reason...
            params.cullStyle = HdCullStyleNothing;
#if MAYA_API_VERSION >= 201603
            if(displayStyle & MHWRender::MFrameContext::DisplayStyle::kBackfaceCulling)
                params.cullStyle = HdCullStyleBackUnlessDoubleSided;
#endif
            
            return params;
        }
        
        void QueueShapeForDraw(MUserData*& userData, const RenderParams& params, bool drawShape, MBoundingBox* boxToDraw = NULL)
        {
            if(boxToDraw)
                userData = new _BatchDrawUserData(drawShape, *boxToDraw, params.wireframeColor);
            else if(drawShape)
                userData = new _BatchDrawUserData();
            else
                userData = NULL;
            
            if(drawShape && _batchRenderer)
                _batchRenderer->_QueueShapeForDraw(_sharedId, params);
        }
        
        const RenderParams& GetBaseParams()
        {
            return _baseParams;
        }
        
    };
    class TaskDelegate : public HdSceneDelegate
    {
    public:
        typedef std::unordered_map<size_t, SdfPath>                _RenderTaskIdMap;
        typedef TfHashMap <TfToken, VtValue, TfToken::HashFunctor> _ValueCache;
        typedef TfHashMap <SdfPath, _ValueCache, SdfPath::Hash>    _ValueCacheMap;
    private:
        _RenderTaskIdMap               _renderTaskIdMap;
        SdfPath                        _rootId;
        SdfPath                        _simpleLightTaskId;
        SdfPathVector                  _lightIds;
        SdfPath                        _cameraId;
        GfVec4d                        _viewport;
        GlfSimpleLightingContextRefPtr _lightingContext;
        _ValueCacheMap                 _valueCacheMap;
    public:
        TaskDelegate(HdRenderIndex* renderIndex, SdfPath const& delegateID)
            : HdSceneDelegate(renderIndex, delegateID)
        {
            _lightingContext = GlfSimpleLightingContext::New();
    
            // populate tasks in renderindex
    
            // create an unique namespace
            _rootId = delegateID.AppendChild(TfToken(TfStringPrintf("_UsdImaging_%p", this)));
    
            _simpleLightTaskId = _rootId.AppendChild(_tokens->simpleLightTask);
            _cameraId          = _rootId.AppendChild(_tokens->camera);
    
            // camera
            {
                // Since we're hardcoded to use HdStRenderDelegate, we expect to
                // have camera Sprims.
                TF_VERIFY(renderIndex->IsSprimTypeSupported(HdPrimTypeTokens->camera));
        
                renderIndex->InsertSprim(HdPrimTypeTokens->camera, this, _cameraId);
                _ValueCache& cache = _valueCacheMap[_cameraId];
                cache[HdCameraTokens->worldToViewMatrix] = VtValue(GfMatrix4d(1.0));
                cache[HdCameraTokens->projectionMatrix]  = VtValue(GfMatrix4d(1.0));
                cache[HdCameraTokens->windowPolicy]      = VtValue();  // no window policy.
            }
    
            // simple lighting task (for Hydra native)
            {
                renderIndex->InsertTask<HdxSimpleLightTask>(this, _simpleLightTaskId);
                _ValueCache& cache = _valueCacheMap[_simpleLightTaskId];
                HdxSimpleLightTaskParams taskParams;
                taskParams.cameraPath = _cameraId;
                taskParams.viewport   = GfVec4f(_viewport);
                cache[HdTokens->params]   = VtValue(taskParams);
                //cache[HdTokens->children] = VtValue(SdfPathVector());
            }
        }
    public:
        virtual VtValue Get(SdfPath const& id, TfToken const& key)
        {
            _ValueCache* vcache = TfMapLookupPtr(_valueCacheMap, id);
            VtValue    ret;
            if(vcache && TfMapLookup(*vcache, key, &ret))
                return ret;

            TF_CODING_ERROR("%s:%s doesn't exist in the value cache\n", id.GetText(), key.GetText());
            return VtValue();
        }

    public:
        void SetCameraState(const GfMatrix4d& worldToViewMatrix,
                            const GfMatrix4d& projectionMatrix,
                            const GfVec4d& viewport)
        {
            // cache the camera matrices
            _ValueCache& cache = _valueCacheMap[_cameraId];
            cache[HdCameraTokens->worldToViewMatrix] = VtValue(worldToViewMatrix);
            cache[HdCameraTokens->projectionMatrix]  = VtValue(projectionMatrix);
            cache[HdCameraTokens->windowPolicy]      = VtValue(); // no window policy.

            // invalidate the camera to be synced
            GetRenderIndex().GetChangeTracker().MarkSprimDirty(_cameraId, HdCamera::AllDirty);

            if(_viewport != viewport)
            {
                _viewport = viewport;

                // Update the simple light task.
                HdxSimpleLightTaskParams simpleLightTaskParams = _GetValue<HdxSimpleLightTaskParams>(_simpleLightTaskId, HdTokens->params);

                simpleLightTaskParams.viewport = GfVec4f(_viewport);
                _SetValue(_simpleLightTaskId, HdTokens->params, simpleLightTaskParams);

                GetRenderIndex().GetChangeTracker().MarkTaskDirty(_simpleLightTaskId, HdChangeTracker::DirtyParams);

                // Update all render tasks.
                for (const auto& it : _renderTaskIdMap)
                {
                    const SdfPath& renderTaskId = it.second;

                    HdxRenderTaskParams renderTaskParams = _GetValue<HdxRenderTaskParams>(renderTaskId, HdTokens->params);

                    renderTaskParams.viewport = _viewport;
                    _SetValue(renderTaskId, HdTokens->params, renderTaskParams);

                    GetRenderIndex().GetChangeTracker().MarkTaskDirty(renderTaskId,HdChangeTracker::DirtyParams);
                }
            }
        }

        void SetLightingStateFromVP1(const GfMatrix4d& worldToViewMatrix,
                                     const GfMatrix4d& projectionMatrix)
        {
            // This function should only be called in a VP1.0 context. In VP2.0, we can
            // translate the lighting state from the MDrawContext directly into Glf,
            // but there is no draw context in VP1.0, so we have to transfer the state
            // through OpenGL.

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadMatrixd(worldToViewMatrix.GetArray());
            _lightingContext->SetStateFromOpenGL();
            glPopMatrix();

            _lightingContext->SetCamera(worldToViewMatrix, projectionMatrix);

            _SetLightingStateFromLightingContext();
        }

        void SetLightingStateFromMayaDrawContext(const MHWRender::MDrawContext& context)
        {
            _lightingContext = px_vp20Utils::GetLightingContextFromDrawContext(context);
            _SetLightingStateFromLightingContext();
        }

        HdTaskSharedPtrVector GetSetupTasks()
        {
            HdTaskSharedPtrVector tasks;
            tasks.push_back(GetRenderIndex().GetTask(_simpleLightTaskId));
            return tasks;
        }
        HdTaskSharedPtr GetRenderTask(size_t               hash,
                                      RenderParams const&  renderParams,
                                      SdfPathVector const& roots)
        {
            // select bucket
            SdfPath renderTaskId;
            if(!TfMapLookup(_renderTaskIdMap, hash, &renderTaskId))
            {
                // create new render task if not exists
                renderTaskId = _rootId.AppendChild(TfToken(TfStringPrintf("renderTask%zx", hash)));
                _InsertRenderTask(renderTaskId);
                _renderTaskIdMap[hash] = renderTaskId;
            }

            TfToken           colName = renderParams.geometryCol;
            HdRprimCollection rprims(colName, HdReprSelector(renderParams.drawRepr));
            rprims.SetRootPaths(roots);
            rprims.SetRenderTags(renderParams.renderTags);
            GetRenderIndex().GetChangeTracker().MarkCollectionDirty(colName);

            // update value cache
            _SetValue(renderTaskId, HdTokens->collection, rprims);

            // invalidate
            GetRenderIndex().GetChangeTracker().MarkTaskDirty(renderTaskId, HdChangeTracker::DirtyCollection);

            // update render params in the value cache
            HdxRenderTaskParams taskParams = _GetValue<HdxRenderTaskParams>(renderTaskId, HdTokens->params);

            // update params
            taskParams.overrideColor  = renderParams.overrideColor;
            taskParams.wireframeColor = renderParams.wireframeColor;
            taskParams.enableLighting = renderParams.enableLighting;
            taskParams.enableIdRender = false;
            taskParams.alphaThreshold = 0.1;
            //taskParams.tessLevel      = 32.0;
            const float tinyThreshold = 0.9f;
            //taskParams.drawingRange          = GfVec2f(tinyThreshold, -1.0f);
            taskParams.depthBiasUseDefault   = true;
            taskParams.depthFunc             = HdCmpFuncLess;
            taskParams.cullStyle             = renderParams.cullStyle;
            //taskParams.geomStyle             = HdGeomStylePolygons;
            //taskParams.enableHardwareShading = true;

            // note that taskParams.rprims and taskParams.viewport are not updated
            // in this function, and needed to be preserved.

            // store into cache
            _SetValue(renderTaskId, HdTokens->params, taskParams);

            // invalidate
            GetRenderIndex().GetChangeTracker().MarkTaskDirty(renderTaskId, HdChangeTracker::DirtyParams);

            return GetRenderIndex().GetTask(renderTaskId);
        }

    protected:
        void _InsertRenderTask(SdfPath const& id)
        {
            GetRenderIndex().InsertTask<HdxRenderTask>(this, id);
            _ValueCache         & cache = _valueCacheMap[id];
            HdxRenderTaskParams taskParams;
            taskParams.camera   = _cameraId;
            
            taskParams.viewport = _viewport;
            cache[HdTokens->params]     = VtValue(taskParams);
            //cache[HdTokens->children]   = VtValue(SdfPathVector());
            cache[HdTokens->collection] = VtValue();
        }

        void _SetLightingStateFromLightingContext()
        {
            const GlfSimpleLightVector& lights = _lightingContext->GetLights();

            bool hasNumLightsChanged = false;

            // Insert light Ids into the render index for those that do not yet exist.
            while (_lightIds.size() < lights.size())
            {
                SdfPath lightId(TfStringPrintf("%s/light%d", _rootId.GetText(), (int) _lightIds.size()));
                _lightIds.push_back(lightId);

                // Since we're hardcoded to use HdStRenderDelegate, we expect to have light Sprims.
                TF_VERIFY(GetRenderIndex().IsSprimTypeSupported(HdPrimTypeTokens->simpleLight));

                GetRenderIndex().InsertSprim(HdPrimTypeTokens->simpleLight, this, lightId);
                hasNumLightsChanged = true;
            }

            // Remove unused light Ids from HdRenderIndex
            while (_lightIds.size() > lights.size())
            {
                GetRenderIndex().RemoveSprim(HdPrimTypeTokens->simpleLight, _lightIds.back());
                _lightIds.pop_back();
                hasNumLightsChanged = true;
            }

            // invalidate HdLights
            for (size_t i = 0; i < lights.size(); ++i)
            {
                _ValueCache& cache = _valueCacheMap[_lightIds[i]];
                // store GlfSimpleLight directly.
                cache[HdLightTokens->params]           = VtValue(lights[i]);
                cache[HdLightTokens->transform]        = VtValue();
                cache[HdLightTokens->shadowParams]     = VtValue(HdxShadowParams());
                cache[HdLightTokens->shadowCollection] = VtValue();

                // Only mark as dirty the parameters to avoid unnecessary invalidation
                // specially marking as dirty lightShadowCollection will trigger
                // a collection dirty on geometry and we don't want that to happen
                // always
                GetRenderIndex().GetChangeTracker().MarkSprimDirty(_lightIds[i], HdStLight::AllDirty);
            }

            // sadly the material also comes from lighting context right now...
            HdxSimpleLightTaskParams taskParams = _GetValue<HdxSimpleLightTaskParams>(_simpleLightTaskId, HdTokens->params);
            taskParams.sceneAmbient = _lightingContext->GetSceneAmbient();
            taskParams.material     = _lightingContext->GetMaterial();

            // invalidate HdxSimpleLightTask too
            if(hasNumLightsChanged)
            {
                _SetValue(_simpleLightTaskId, HdTokens->params, taskParams);

                GetRenderIndex().GetChangeTracker().MarkTaskDirty(_simpleLightTaskId, HdChangeTracker::DirtyParams);
            }
        }


        template<typename T> T const& _GetValue(SdfPath const& id, TfToken const& key)
        {
            VtValue vParams = _valueCacheMap[id][key];
            TF_VERIFY(vParams.IsHolding<T>());
            return vParams.Get<T>();
        }

        template<typename T> void _SetValue(SdfPath const& id, TfToken const& key, T const& value)
        {
            _valueCacheMap[id][key] = value;
        }
    
    };
public:
    static void Init()
    {
        GlfGlewInit();
        GetGlobalRenderer();
    }
    static UsdMayaGLBatchRenderer& GetGlobalRenderer()
    {
        if(!_sGlobalRendererPtr)
        {
            Reset();
        }
        return *_sGlobalRendererPtr;
    
    }
    static void Reset()
    {
        if(_sGlobalRendererPtr)
        {
            MGlobal::displayInfo("Resetting USD Batch Renderer");
        }
        _sGlobalRendererPtr.reset(new UsdMayaGLBatchRenderer());
    }
    
    
    static void _OnMayaSceneUpdateCallback(void* clientData)
    {
        UsdMayaGLBatchRenderer::Reset();
    }
private:
    static size_t _ShapeHash(const UsdPrim& usdPrim, const SdfPathVector& excludePrimPaths, const MDagPath& objPath)
    {
	    size_t hash(MObjectHandle(objPath.transform()).hashCode());
	    boost::hash_combine(hash, usdPrim);
	    boost::hash_combine(hash, excludePrimPaths);
	
	    return hash;
    }
	
	static void _OnMayaEndRenderCallback(MHWRender::MDrawContext& context, void* clientData)
	{
		if(_sGlobalRendererPtr)
		{
			_sGlobalRendererPtr->_MayaRenderDidEnd();
		}
	}


private:
    static std::unique_ptr<UsdMayaGLBatchRenderer> _sGlobalRendererPtr;

private:
    typedef std::unordered_map<size_t, ShapeRenderer>                       _ShapeRendererMap;
    typedef std::unordered_set<SdfPath, SdfPath::Hash>                      _SdfPathSet;
    typedef std::pair<RenderParams, _SdfPathSet>                            _RenderParamSet;
    typedef std::unordered_map<size_t, _RenderParamSet>                     _RendererQueueMap;
    typedef std::unordered_map<SdfPath, HdxIntersector::Hit, SdfPath::Hash> HitBatch;
    typedef std::shared_ptr<TaskDelegate>                                   TaskDelegateSharedPtr;
private:
    _ShapeRendererMap                                                       _shapeRendererMap;
    std::unordered_set<ShapeRenderer*>                                      _populateQueue;
    _RendererQueueMap                                                       _renderQueue;
    std::unordered_set<std::string>                                         _drawnMayaRenderPasses;
    _RendererQueueMap                                                       _selectQueue;

    HitBatch                                                                _selectResults;
    HdEngine                                                                _hdEngine;
    HdStRenderDelegate                                                      _renderDelegate;
    std::unique_ptr<HdRenderIndex>                                          _renderIndex;

    TaskDelegateSharedPtr                                                   _taskDelegate;
    HdxIntersectorSharedPtr                                                 _intersector;
    UsdMayaGLSoftSelectHelper                                               _softSelectHelper;

public:
    UsdMayaGLBatchRenderer()
    {
        DECL_LOGGER(logging::maya->UsdHdBatchRenderer);

        _renderIndex.reset(HdRenderIndex::New(&_renderDelegate));
        if(!TF_VERIFY(_renderIndex))
        {
            return;
        }
        _taskDelegate.reset(new TaskDelegate(_renderIndex.get(), SdfPath("/TN_mayaTask")));
        _intersector.reset(new HdxIntersector(_renderIndex.get()));
    
    
        static MCallbackId sceneUpdateCallbackId = 0;
        if(sceneUpdateCallbackId == 0)
        {
            sceneUpdateCallbackId = MSceneMessage::addCallback(MSceneMessage::kSceneUpdate, _OnMayaSceneUpdateCallback);
        }
    
        MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
        if(!renderer)
        {
            MGlobal::displayError("Viewport 2.0 renderer not initialized.");
        }
        else
        {
            renderer->addNotification(UsdMayaGLBatchRenderer::_OnMayaEndRenderCallback,
                                      _tokens->MayaEndRenderNotificationName.GetText(),
                                      MHWRender::MPassContext::kEndRenderSemantic,
                                      nullptr);
        }
    }
    
    ~UsdMayaGLBatchRenderer()
    {
        DECL_LOGGER(logging::maya->UsdHdBatchRenderer);
        _intersector.reset();
        _taskDelegate.reset();
        _shapeRendererMap.clear();
    }

public:
    ShapeRenderer* GetShapeRenderer(const UsdPrim& usdPrim, const SdfPathVector& excludePrimPaths, const MDagPath& objPath)
    {
        DECL_LOGGER(logging::maya->UsdHdBatchRenderer);
        const size_t hash = _ShapeHash(usdPrim, excludePrimPaths, objPath);
    
        // We can get away with this because the spec for std::unordered_map
        // guarantees that data pairs remain valid even if other objects are inserted.
        //
        ShapeRenderer* toReturn = &_shapeRendererMap[hash];
    
        if(!toReturn->_batchRenderer)
        {
            // Create a simple hash string to put into a flat SdfPath "hierarchy".
            // This is much faster than more complicated pathing schemes.
            //
            std::string idString = TfStringPrintf("/x%zx", hash);
        
            toReturn->Init(_renderIndex.get(),
                           SdfPath(idString),
                           usdPrim,
                           excludePrimPaths);
        
            toReturn->_batchRenderer = this;
        }
    
        return toReturn;
    }
    const UsdMayaGLSoftSelectHelper& GetSoftSelectHelper()
    {
        _softSelectHelper.Populate();
        return _softSelectHelper;
    }
    void Draw(const MDrawRequest& request, M3dView& view)
    {
        // VP 1.0 Implementation
        MDrawData drawData = request.drawData();
        
        _BatchDrawUserData* batchData = static_cast<_BatchDrawUserData*>(drawData.geometry());
        if(!batchData)
        {
            return;
        }
        
        MMatrix projectionMat;
        view.projectionMatrix(projectionMat);
        const GfMatrix4d projectionMatrix(projectionMat.matrix);
        
        if(batchData->_bounds)
        {
            MMatrix modelViewMat;
            view.modelViewMatrix(modelViewMat);
            
            px_vp20Utils::RenderBoundingBox(*(batchData->_bounds),
                                            *(batchData->_wireframeColor),
                                            modelViewMat,
                                            projectionMat);
        }
        
        if(batchData->_drawShape && !_renderQueue.empty())
        {
            MDagPath cameraDagPath;
            view.getCamera(cameraDagPath);
            const GfMatrix4d cameraMatrix(cameraDagPath.inclusiveMatrix().matrix);
            const GfMatrix4d worldToViewMatrix(cameraMatrix.GetInverse());
            
            unsigned int viewX, viewY, viewWidth, viewHeight;
            view.viewport(viewX, viewY, viewWidth, viewHeight);
            const GfVec4d viewport(viewX, viewY, viewWidth, viewHeight);
            
            // Only the first call to this will do anything... After that the batch
            // queue is cleared.
            //
            _RenderBatches(nullptr, worldToViewMatrix, projectionMatrix, viewport);
        }
        
        // Clean up the _BatchDrawUserData!
        delete batchData;
    }
    void Draw(const MHWRender::MDrawContext& context, const MUserData* userData)
    {
        DECL_LOGGER(logging::maya->UsdHdBatchRenderer);
        // VP 2.0 Implementation
        MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
        if(!theRenderer || !theRenderer->drawAPIIsOpenGL())
        {
            STDOUT_ERROR("Failed to get theRenderer");
            return;
        }
    
        const _BatchDrawUserData* batchData = static_cast<const _BatchDrawUserData*>(userData);
        if(!batchData)
        {
            STDOUT_ERROR("Failed to get batchData");
            return;
        }
    
        MStatus status;
    
        const MMatrix    projectionMat = context.getMatrix(MHWRender::MFrameContext::kProjectionMtx, &status);
        const GfMatrix4d projectionMatrix(projectionMat.matrix);
    
        if(batchData->_bounds)
        {
            const MMatrix worldViewMat = context.getMatrix(MHWRender::MFrameContext::kWorldViewMtx, &status);
        
            px_vp20Utils::RenderBoundingBox(*(batchData->_bounds),
                                            *(batchData->_wireframeColor),
                                            worldViewMat,
                                            projectionMat);
        }
    
        const MHWRender::MPassContext& passContext = context.getPassContext();
        const MString                & passId      = passContext.passIdentifier();
        LOGGER_DEBUG("passId = %s", passId.asChar());
    
        const auto inserted = _drawnMayaRenderPasses.insert(passId.asChar());
        if(!inserted.second)
        {
            // We've already done a Hydra draw for this Maya render pass, so we
            // don't do another one.
        
            STDOUT_ERROR("Failed to insert render pass");
            return;
        }
    
        if(batchData->_drawShape && !_renderQueue.empty())
        {
            const MMatrix    viewMat = context.getMatrix(MHWRender::MFrameContext::kViewMtx, &status);
            const GfMatrix4d worldToViewMatrix(viewMat.matrix);
        
            // Extract camera settings from maya view
            int viewX, viewY, viewWidth, viewHeight;
            context.getViewportDimensions(viewX, viewY, viewWidth, viewHeight);
            const GfVec4d viewport(viewX, viewY, viewWidth, viewHeight);
        
            // Only the first call to this will do anything... After that the batch queue is cleared.
            _RenderBatches(&context, worldToViewMatrix, projectionMatrix, viewport);
        }
    }

private:
    void _QueueShapeForDraw(const SdfPath& sharedId, const RenderParams& params)
    {
        _QueuePathForDraw(sharedId, params);
    }
    
    void _QueuePathForDraw(const SdfPath& sharedId, const RenderParams& params)
    {
        const size_t paramKey = params.Hash();
    
        auto renderSetIter = _renderQueue.find(paramKey);
        if(renderSetIter == _renderQueue.end())
        {
            // If we had no _SdfPathSet for this particular RenderParam combination,
            // create a new one.
            _renderQueue[paramKey] = _RenderParamSet(params, _SdfPathSet({sharedId}));
        }
        else
        {
            _SdfPathSet& renderPaths = renderSetIter->second.second;
            renderPaths.insert(sharedId);
        }
    }
    
    
    void _RenderBatches(const MHWRender::MDrawContext* vp2Context, const GfMatrix4d& worldToViewMatrix, const GfMatrix4d& projectionMatrix, const GfVec4d& viewport)
    {
        DECL_LOGGER(logging::maya->UsdHdBatchRenderer);
        //DECL_LOGGER(true);
        if(_renderQueue.empty())
        {
            LOGGER_ERROR("renderQueue is empty");
            return;
        }
    
        if(!_populateQueue.empty())
        {
            LOGGER_INFO("populate stage");
            TF_DEBUG(PXRUSDMAYAGL_QUEUE_INFO).Msg(
                    "____________ POPULATE STAGE START ______________ (%zu)\n", _populateQueue.size());
        
            std::vector<UsdImagingDelegate*> delegates;
            UsdPrimVector                    rootPrims;
            std::vector<SdfPathVector>       excludedPrimPaths;
            std::vector<SdfPathVector>       invisedPrimPaths;
        
            for (ShapeRenderer* shapeRenderer : _populateQueue)
            {
                delegates.push_back(shapeRenderer->_delegate.get());
                rootPrims.push_back(shapeRenderer->_rootPrim);
                excludedPrimPaths.push_back(shapeRenderer->_excludedPaths);
                invisedPrimPaths.push_back(SdfPathVector());
            
                shapeRenderer->_isPopulated = true;
            }
        
            UsdImagingDelegate::Populate(delegates,
                                         rootPrims,
                                         excludedPrimPaths,
                                         invisedPrimPaths
            );
        
            _populateQueue.clear();
        
            TF_DEBUG(PXRUSDMAYAGL_QUEUE_INFO).Msg("^^^^^^^^^^^^ POPULATE STAGE FINISH ^^^^^^^^^^^^^ (%zu)\n", _populateQueue.size());
        }
    
        TF_DEBUG(PXRUSDMAYAGL_QUEUE_INFO).Msg("____________ RENDER STAGE START ______________ (%zu)\n", _renderQueue.size());
    
        // A new display refresh signifies that the cached selection data is no
        // longer valid.
        _selectQueue.clear();
        _selectResults.clear();
    
        // We've already populated with all the selection info we need.  We Reset
        // and the first call to GetSoftSelectHelper in the next render pass will
        // re-populate it.
        _softSelectHelper.Reset();
    
        _taskDelegate->SetCameraState(worldToViewMatrix,
                                      projectionMatrix,
                                      viewport);
    
        // save the current GL states which hydra may reset to default
        glPushAttrib(GL_LIGHTING_BIT |
                     GL_ENABLE_BIT |
                     GL_POLYGON_BIT |
                     GL_DEPTH_BUFFER_BIT |
                     GL_VIEWPORT_BIT);
    
        // hydra orients all geometry during topological processi))ng so that
        // front faces have ccw winding. We disable culling because culling
        // is handled by fragment shader discard.
        glFrontFace(GL_CCW); // < State is pushed via GL_POLYGON_BIT
        glDisable(GL_CULL_FACE);
    
        // note: to get benefit of alpha-to-coverage, the target framebuffer
        // has to be a MSAA buffer.
        glDisable(GL_BLEND);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    
        if(vp2Context)
        {
            _taskDelegate->SetLightingStateFromMayaDrawContext(*vp2Context);
        }
        else
        {
            _taskDelegate->SetLightingStateFromVP1(worldToViewMatrix, projectionMatrix);
        }
    
        // The legacy viewport does not support color management,
        // so we roll our own gamma correction by GL means (only in
        // non-highlight mode)
        bool gammaCorrect = !vp2Context;
    
        if(gammaCorrect)
            glEnable(GL_FRAMEBUFFER_SRGB_EXT);
    
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
        // render task setup
        HdTaskSharedPtrVector tasks = _taskDelegate->GetSetupTasks(); // lighting etc
    
        for (const auto& renderSetIter : _renderQueue)
        {
            const size_t      hash          = renderSetIter.first;
            const RenderParams& params      = renderSetIter.second.first;
            const _SdfPathSet & renderPaths = renderSetIter.second.second;
        
            TF_DEBUG(PXRUSDMAYAGL_QUEUE_INFO).Msg("*** renderQueue, batch %zx, size %zu\n", renderSetIter.first, renderPaths.size());
        
            SdfPathVector roots(renderPaths.begin(), renderPaths.end());
            tasks.push_back(_taskDelegate->GetRenderTask(hash, params, roots));
        }
    
        LOGGER_INFO("hdEngine.Execute");
        _hdEngine.Execute(*_renderIndex, tasks);
    
        if(gammaCorrect)
            glDisable(GL_FRAMEBUFFER_SRGB_EXT);
    
        glPopAttrib(); // GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT
    
        // Viewport 2 may be rendering in multiple passes, and we want to make sure
        // we draw once (and only once) for each of those passes, so we delay
        // swapping the render queue into the select queue until we receive a
        // notification that all rendering has ended.
        // For the legacy viewport, rendering is done in a single pass and we will
        // not receive a notification at the end of rendering, so we do the swap
        // now.
        if(!vp2Context)
        {
            _MayaRenderDidEnd();
        }
    
        TF_DEBUG(PXRUSDMAYAGL_QUEUE_INFO).Msg("^^^^^^^^^^^^ RENDER STAGE FINISH ^^^^^^^^^^^^^ (%zu)\n", _renderQueue.size());
    }
    
    void _MayaRenderDidEnd()
    {
        // Selection is based on what we have last rendered to the display. The
        // selection queue is cleared during drawing, so this has the effect of
        // resetting the render queue and prepping the selection queue without any
        // significant memory hit.
        _renderQueue.swap(_selectQueue);
    
        _drawnMayaRenderPasses.clear();
    }
    
};

NAMESPACE_BASE_END
