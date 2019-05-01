#include "maya/drawOverride.h"

#include "baseGl/glutil.h"
#include "baseGl/taskManager.h"

#include "lib/baseMaya/util.h"


NAMESPACE_BASE_BEGIN

/* ------------------------------------------------------------ decl singleton */
DrawOverride::MapT DrawOverride::kDrawOverrideMap;

/* ------------------------------------------------------------ static functions for singleton */
const std::shared_ptr<DrawOverride> DrawOverride::GetDrawOverride(const MDagPath& objPath)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	if(kDrawOverrideMap.find(objPath) == kDrawOverrideMap.end())
	{
		LOGGER_DEBUG("Failed to get pDrawOverride");
	}
	return kDrawOverrideMap[objPath];
}
void DrawOverride::Register(const MDagPath& objPath, const std::shared_ptr<DrawOverride>& pDerived)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	
	if(kDrawOverrideMap.find(objPath) != kDrawOverrideMap.end())
	{
		LOGGER_DEBUG("Failed to insert %s,", objPath.fullPathName().asChar());
		return;
	}
	
	bool success = kDrawOverrideMap.insert(DrawOverride::MapT::value_type(objPath, pDerived)).second;
	if(!success)
	{
		LOGGER_DEBUG("Failed to insert %s,", objPath.fullPathName().asChar());
	}
}
void DrawOverride::Deregister(const MDagPath& objPath)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	
	if(kDrawOverrideMap.find(objPath) == kDrawOverrideMap.end())
	{
		LOGGER_DEBUG("Failed to insert %s,", objPath.fullPathName().asChar());
		return;
	}
	
	kDrawOverrideMap.erase(objPath);
}
DrawOverride::iterator DrawOverride::Begin()
{
	return kDrawOverrideMap.begin();
}
DrawOverride::iterator DrawOverride::End()
{
	return kDrawOverrideMap.end();
}

/* ------------------------------------------------------------ MUserData */
struct DrawOverride::UserData : public MUserData
{
	DrawOverride* pDrawOverride;
public:
	UserData(DrawOverride* pOvr)
		: MUserData(false /* no delete after use */),
		  pDrawOverride(pOvr)
	{
	}
	virtual ~UserData()
	{
	}
	
	
};

/* ------------------------------------------------------------ MPxDrawOverride Draw callback Wrap */
void DrawOverride::DrawCallback(const MHWRender::MDrawContext& context, const MUserData* pUserData)
{
	//DECL_LOGGER(maya::DrawOverride);
	DECL_LOGGER(false);
	
	/* ========================================================*/
	/*  Validate Data                                          */
	/* ========================================================*/
	if(SkipDraw(context, pUserData))
	{
		LOGGER_DEBUG("SkipDraw() is true");
		return;
	}
	if(!GlewInit())
	{
		LOGGER_ERROR("Failed to initialize glew");
		return;
	}
	DrawOverride* pDrawOverride = dynamic_cast<const DrawOverride::UserData*>(pUserData)->pDrawOverride;
	if(!pDrawOverride)
	{
		LOGGER_ERROR("Failed to get pDrawOverride in draw callback");
		return;
	}
	if(pDrawOverride->parameter().displayMode == mode::display::none ||
	   pDrawOverride->parameter().displayMode == mode::display::points
	)
	{
		LOGGER_ERROR("displayMode is not valid, %s", mode::display::ToString(pDrawOverride->parameter().displayMode).c_str());
		return;
	}
	
	
	MHWRender::MRenderer    * theRenderer = MHWRender::MRenderer::theRenderer();
	MHWRender::MStateManager* stateMgr    = context.getStateManager();
	const unsigned int displayStyle       = context.getDisplayStyle();
	
	bool inSelection        = false;
	bool inBeautyPass       = false;
	bool inTransparencyPass = false;
	const MString      beautyPassSemantic = "shadedBeautyGraphSemantic";
	const MHWRender::MPassContext& passCtx              = context.getPassContext();
	const MStringArray           & passSem              = passCtx.passSemantics();
	MHWRender::MShaderInstance   * pPassShaderOverrided = nullptr;
	
	bool requireBlending = false;
	const MHWRender::MBlendState* pOldBlendState = NULL;
	
	const MHWRender::MRasterizerState* pOldRasterState = NULL;
	const MHWRender::MRasterizerState* rasterState     = NULL;
	
	/* ========================================================*/
	/*  Pre Draw Work                                          */
	/* ========================================================*/
	{
		/* set passShader */
		for (unsigned int i = 0; i < passSem.length(); i++)
		{
			if(passSem[i] == MHWRender::MPassContext::kSelectionPassSemantic)
			{
				inSelection = true;
			}
			else if(passSem[i] == beautyPassSemantic)
			{
				inBeautyPass = true;
			}
			
			if(passCtx.hasShaderOverride())
			{
				pPassShaderOverrided = passCtx.shaderOverrideInstance();
				
				// Update single-sided state and normal multiplier as needed
				pPassShaderOverrided->setParameter("isSingleSided", (displayStyle & MHWRender::MFrameContext::kTwoSidedLighting) == 0);
				pPassShaderOverrided->setParameter("mayaNormalMultiplier", -1.0f);
			}
			
			if(passSem[i] == MHWRender::MPassContext::kCullBackSemantic || passSem[i] == MHWRender::MPassContext::kCullFrontSemantic)
			{
				inTransparencyPass = true;
			}
		}
		/* set blend and raster state */
		if(stateMgr && ((displayStyle & MHWRender::MFrameContext::kGouraudShaded) || (displayStyle & MHWRender::MFrameContext::kFlatShaded)))
		{
			///---------------------------------------------------set Blend State
			// draw filled, and with blending if required
			if(stateMgr && requireBlending)
			{
				if(pDrawOverride->pBlendState_)
				{
					pOldBlendState = stateMgr->getBlendState();
					stateMgr->setBlendState(pDrawOverride->pBlendState_);
				}
			}
			
			///---------------------------------------------------set Raster State
			// If the object is not draw as transparency, override culling mode since we always want double-sided
			if(!inTransparencyPass)
			{
				pOldRasterState = stateMgr ? stateMgr->getRasterizerState() : NULL;
				if(pOldRasterState)
				{
					
					MHWRender::MRasterizerStateDesc       desc(pOldRasterState->desc());
					// It's also possible to change this to kCullFront or kCullBack if we wanted to set it to that.
					MHWRender::MRasterizerState::CullMode cullMode = MHWRender::MRasterizerState::kCullNone;
					if(desc.cullMode != cullMode)
					{
						// Just override the cullmode
						desc.cullMode = cullMode;
						rasterState = stateMgr->acquireRasterizerState(desc);
						if(rasterState)
						{
							stateMgr->setRasterizerState(rasterState);
						}
					}
				}
			}
		}
	}
	/* ========================================================*/
	/*  Draw Work       	                                   */
	/* ========================================================*/
	{
		glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
		{
			/* set gl state */
			pDrawOverride->setGlState_(context);
			/* set display state */
			pDrawOverride->setDisplayState_(context);
			/* set transformMtx state */
			pDrawOverride->setTransformMtx_(context);
			/* set lighting state */
			pDrawOverride->setLighting_(context);
			
			pDrawOverride->bindContext_(&context, pPassShaderOverrided);
			{
				if(pDrawOverride->parameter().drawBound)
					pDrawOverride->drawBound_(context, pPassShaderOverrided);
				
				/*draw in derived class */
				pDrawOverride->drawImpl_(context, pPassShaderOverrided);
			}
			pDrawOverride->unbindContext_(&context, pPassShaderOverrided);
			
			/* unset gl state */
			pDrawOverride->unsetGlState_(context);
			/* unset lighting state */
			pDrawOverride->unsetLighting_(context);
			/* usset transformMtx state */
			pDrawOverride->unsetTransformMtx_(context);
			/* usset display state */
			pDrawOverride->unsetDisplayState_(context);
			
		}
		glPopAttrib(); // GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT
	}
	
	/* ========================================================*/
	/*  Post Draw Work                                         */
	/* ========================================================*/
	{
		/* Restore old blend state and old raster state */
		if(stateMgr && (displayStyle & MHWRender::MFrameContext::kGouraudShaded || displayStyle & MHWRender::MFrameContext::kFlatShaded))
		{
			///---------------------------------------------------set Blend State
			if(stateMgr && pOldBlendState)
			{
				stateMgr->setBlendState(pOldBlendState);
				stateMgr->releaseBlendState(pOldBlendState);
			}
			
			///---------------------------------------------------set Raster State
			if(pOldRasterState)
			{
				stateMgr->setRasterizerState(pOldRasterState);
				stateMgr->releaseRasterizerState(pOldRasterState);
			}
			if(rasterState)
			{
				stateMgr->releaseRasterizerState(rasterState);
				rasterState = NULL;
			}
		}
	}
}
bool DrawOverride::SkipDraw(const MHWRender::MDrawContext& context, const MUserData* pUserData)
{
	///-------------------------------------------------- Get renderer
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer == nullptr)
	{
		MGlobal::displayInfo("theRenderer is null, drawing skipped");
		return true;
	}
	///--------------------------------------------------  Just return and draw nothing, if drawApi isn't OpenGL
	if (!(theRenderer->drawAPI() == MHWRender::kOpenGL || theRenderer->drawAPI() == MHWRender::kOpenGLCoreProfile))
	{
		MGlobal::displayInfo("drawAPI is not openGL; drawing skipped");
		return true;
	}
	if(MGlobal::mayaState() == MGlobal::kBatch)
	{
		MGlobal::displayInfo("maya state is kBatch; drawing skipped");
		return true;
	}
	if(MGlobal::mayaState() == MGlobal::kLibraryApp)
	{
		MGlobal::displayInfo("maya state is kLibraryApp; drawing skipped");
		return true;
	}
	if (pUserData == nullptr)
	{
		return true;
	}
	
	return false;
}

/* ------------------------------------------------------------ constructor / destructor*/
DrawOverride::DrawOverride()
{
}
DrawOverride::~DrawOverride()
{
	if(pBlendState_)
	{
		MHWRender::MStateManager::releaseBlendState(pBlendState_);
		pBlendState_ = nullptr;
	}
	
}


/* ------------------------------------------------------------ public helpers*/
MUserData* DrawOverride::prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* pOldUserData)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	
	DrawOverride::UserData* pUserData = dynamic_cast<DrawOverride::UserData*>(pOldUserData);
	if(!pUserData)
	{
		pUserData = new DrawOverride::UserData(this);
	}
	
	/* -------------------------------------------- prepare extra render param */
	/* ---------------------- get MDagDrawOverridedInfo */
	drawOverrideInfo_ = objPath.getDrawOverrideInfo();
	
	/* ---------------------- get attribute */
	getRenderParam_(objPath);
	/* ---------------------- get transform mtx*/
	getWorldMtx_(objPath);
	/* ---------------------- set bound*/
	setBound_(objPath);
	
	/* ---------------------- get display state*/
	MHWRender::DisplayStatus displayStatus = MHWRender::MGeometryUtilities::displayStatus(objPath);
	if((displayStatus == MHWRender::kLead) || (displayStatus == MHWRender::kActive) || (displayStatus == MHWRender::kHilite) || (displayStatus == MHWRender::kActiveComponent))
	{
		MColor color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
		param_.wireColor = RGBColor(color.r, color.g, color.b);
		isSelected_ = true;
	}
	else
	{
		MColor color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
		param_.wireColor = RGBColor(color.r, color.g, color.b);
		isSelected_ = false;
	}
	LOGGER_DEBUG("isSelected = %d, wireColor = (%5.2f, %5.2f, %5.2f)", isSelected_, param_.wireColor[0], param_.wireColor[1], param_.wireColor[2]);
	
	
	/* ---------------------- get blend state override*/
	if(!pBlendState_)
	{
		MHWRender::MBlendStateDesc blendStateDesc;
		for (int i = 0; i < (blendStateDesc.independentBlendEnable ? MHWRender::MBlendState::kMaxTargets : 1); ++i)
		{
			blendStateDesc.targetBlends[i].blendEnable           = true;
			blendStateDesc.targetBlends[i].sourceBlend           = MHWRender::MBlendState::kSourceAlpha;
			blendStateDesc.targetBlends[i].destinationBlend      = MHWRender::MBlendState::kInvSourceAlpha;
			blendStateDesc.targetBlends[i].blendOperation        = MHWRender::MBlendState::kAdd;
			blendStateDesc.targetBlends[i].alphaSourceBlend      = MHWRender::MBlendState::kOne;
			blendStateDesc.targetBlends[i].alphaDestinationBlend = MHWRender::MBlendState::kInvSourceAlpha;
			blendStateDesc.targetBlends[i].alphaBlendOperation   = MHWRender::MBlendState::kAdd;
		}
		
		blendStateDesc.blendFactor[0] = 1.0f;
		blendStateDesc.blendFactor[1] = 1.0f;
		blendStateDesc.blendFactor[2] = 1.0f;
		blendStateDesc.blendFactor[3] = 1.0f;
		pBlendState_ = MHWRender::MStateManager::acquireBlendState(blendStateDesc);
	}
	
	
	
	/* ----------------------------------------------------------- prepare for draw in derived class */
	if(prepareForDrawImpl_(objPath, cameraPath, frameContext) != MS::kSuccess)
	{
		LOGGER_ERROR("Failed to prepareForDraw()");
		return nullptr;
	}
	
	
	return pUserData;
}

const DrawOverride::TransformMtx&  DrawOverride::transformMtx() const
{
	return transformMtx_;
}
const DrawOverride::Parameter& DrawOverride::parameter() const
{
	return param_;
}

const bool  DrawOverride::isSelected() const
{
	return isSelected_;
}
const bool  DrawOverride::isPassShaderOverrided() const
{
	return isPassShaderOverrided_;
}
const bool  DrawOverride::isTransparent() const
{
	return isTransparent_;
}

const BBox3f& DrawOverride::bound() const
{
	return bound_;
}
const bool  DrawOverride::drawShaded() const
{
	return drawShaded_;
}
const bool  DrawOverride::drawWire() const
{
	return drawWire_;
}
const bool  DrawOverride::drawTexture() const
{
	return drawTexture_;
}


/* ------------------------------------------------------------ private helpers*/
MStatus DrawOverride::getRenderParam_(const MDagPath& objPath)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	
	MStatus    stat;
	MObject    obj = objPath.node(&stat);
	MFnDagNode shapeFn(obj, &stat);
	MCHECKERROR(stat, "Failed to get shapeFn");

	if(shapeFn.hasAttribute("doubleSided", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("doubleSided", &stat));
		MCHECKERROR(stat, "Failed to get doubleSided attrib");
		param_.doubleSided = plug.asBool();
	}
	if(shapeFn.hasAttribute("displayMode", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("displayMode", &stat));
		MCHECKERROR(stat, "Failed to get displayMode attrib");
		param_.displayMode = plug.asShort();
	}
	if(shapeFn.hasAttribute("color", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("color", &stat));
		MCHECKERROR(stat, "Failed to get color attrib");
		param_.color[0] = plug.child(0).asFloat();
		param_.color[1] = plug.child(1).asFloat();
		param_.color[2] = plug.child(2).asFloat();
	}
	if(shapeFn.hasAttribute("transparency", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("transparency", &stat));
		MCHECKERROR(stat, "Failed to get transparency attrib");
		param_.transparency = plug.asFloat();
	}
	
	if(shapeFn.hasAttribute("animCycle", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("animCycle", &stat));
		MCHECKERROR(stat, "Failed to get animCycle attrib");
		param_.animCycle = plug.asShort();
	}
	
	bool forceStatic = false;
	if(shapeFn.hasAttribute("forceStatic", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("forceStatic", &stat));
		MCHECKERROR(stat, "Failed to get forceStatic attrib");
		forceStatic = plug.asBool();
	}
	if(forceStatic)
	{
		double frameOffset;
		MTime  mTime;

		if(shapeFn.hasAttribute("frameOffset", &stat))
		{
			MPlug plug(obj, shapeFn.attribute("frameOffset", &stat));
			MCHECKERROR(stat, "Failed to get frameOffset attrib");
			frameOffset = plug.asDouble();
		}
		if(shapeFn.hasAttribute("inTime", &stat))
		{
			MPlug plug(obj, shapeFn.attribute("inTime", &stat));
			MCHECKERROR(stat, "Failed to get inTime attrib");
			mTime = plug.asMTime();
		}
		Time inTime = GetTime(mTime);
		param_.time = Time(inTime.sec + (frameOffset / inTime.fps), inTime.fps);
	}
	else
	{
		param_.time = Time();
	}
	
	
	if(shapeFn.hasAttribute("drawBound", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("drawBound", &stat));
		MCHECKERROR(stat, "Failed to get drawBound attrib");
		param_.drawBound = plug.asBool();
	}
	if(shapeFn.hasAttribute("enableIcr", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("enableIcr", &stat));
		MCHECKERROR(stat, "Failed to get enableIcr attrib");
		param_.enableIcr = plug.asBool();
	}
	if(shapeFn.hasAttribute("enableLOD", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("enableLOD", &stat));
		MCHECKERROR(stat, "Failed to get enableLOD attrib");
		param_.enableLOD = plug.asBool();
	}
	if(shapeFn.hasAttribute("minDistance", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("minDistance", &stat));
		MCHECKERROR(stat, "Failed to get minDistance attrib");
		param_.minDistance = plug.asDouble();
	}
	if(shapeFn.hasAttribute("maxDistance", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("maxDistance", &stat));
		MCHECKERROR(stat, "Failed to get maxDistance attrib");
		param_.maxDistance = plug.asDouble();
	}
	if(shapeFn.hasAttribute("noLighting", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("noLighting", &stat));
		MCHECKERROR(stat, "Failed to get noLighting attrib");
		param_.noLighting = plug.asBool();
	}
	if(shapeFn.hasAttribute("noWire", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("noWire", &stat));
		MCHECKERROR(stat, "Failed to get noWire attrib");
		param_.noWire = plug.asBool();
	}
	if(shapeFn.hasAttribute("overrideMask", &stat))
	{
		MPlug plug(obj, shapeFn.attribute("overrideMask", &stat));
		MCHECKERROR(stat, "Failed to get overrideMask attrib");
		param_.overrideMask = plug.asBool();
	}
	
	return stat;
}
MStatus DrawOverride::getWorldMtx_(const MDagPath& objPath)
{
	MStatus    stat;
	MFnDagNode shapeFn(objPath.node(), &stat);
	
	stat = GetWorldMtx(objPath.transform(), &transformMtx_.xformMtx);
	MCHECKERRORNORET(stat, "Failed to get xformMtx, %s", shapeFn.fullPathName().asChar());
}

MStatus DrawOverride::setBound_(const MDagPath& objPath)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	
	MStatus stat;
	
	LOGGER_DEBUG("objPaath = %s", objPath.fullPathName(&stat).asChar());
	MObject    obj = objPath.node(&stat);
	MFnDagNode shapeFn(obj, &stat);
	
	MPlug minCorerPlug(obj, shapeFn.attribute("minCorner", &stat));
	MPlug maxCorerPlug(obj, shapeFn.attribute("maxCorner", &stat));
	
	Point3f minCorner = Point3f(minCorerPlug.child(0).asDouble(), minCorerPlug.child(1).asDouble(), minCorerPlug.child(2).asDouble());
	Point3f maxCorner = Point3f(maxCorerPlug.child(0).asDouble(), maxCorerPlug.child(1).asDouble(), maxCorerPlug.child(2).asDouble());
	
	LOGGER_DEBUG("bound_.lower() = (%5.2f, %5.2f, %5.2f)", bound_.lower()[0], bound_.lower()[1], bound_.lower()[2]);
	LOGGER_DEBUG("bound_.upper() = (%5.2f, %5.2f, %5.2f)", bound_.upper()[0], bound_.upper()[1], bound_.upper()[2]);
	bound_ = BBox3f(minCorner, maxCorner);
	
	return stat;
}

MStatus DrawOverride::setGlState_(const MHWRender::MDrawContext& context)
{
	MStatus stat;
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xffffffff);
	glEnable(GL_DEPTH_TEST);
	
	if(!param_.doubleSided)
	{
		/* enable GL_CULL_FACE */
		glEnable(GL_CULL_FACE);
	}
	
	return stat;
}
MStatus DrawOverride::unsetGlState_(const MHWRender::MDrawContext& context)
{
	MStatus stat;
	
	
	if(!param_.doubleSided)
	{
		/* enable GL_CULL_FACE */
		glDisable(GL_CULL_FACE);
	}
	
	return stat;
}
MStatus DrawOverride::setDisplayState_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	MStatus stat;
	if(drawOverrideInfo_.fOverrideEnabled && !drawOverrideInfo_.fEnableVisible)
	{
		param_.displayMode = mode::display::none;
	}
	else
	{
		if(param_.displayMode == mode::display::none)
		{
			param_.displayMode = mode::display::none;
		}
		else if(param_.displayMode == mode::display::points)
		{
			param_.displayMode = mode::display::points;
		}
		else
		{
			const unsigned int displayStyle = ctx.getDisplayStyle();
			bool drawAsBoundingBox          = false;
			bool drawShaded                 = false;
			bool drawWireframe              = false;
			
			///set by maya status
			drawAsBoundingBox = (displayStyle & MHWRender::MFrameContext::kBoundingBox) || (drawOverrideInfo_.fLOD == MDAGDrawOverrideInfo::kLODBoundingBox);
			
			drawShaded = (displayStyle & MHWRender::MFrameContext::kGouraudShaded) || (displayStyle & MHWRender::MFrameContext::kFlatShaded);
			
			drawWireframe = (displayStyle & MHWRender::MFrameContext::kWireFrame) > 0;
			
			// Templated, only draw wirefame and it is not selectale
			bool overideTemplated = (drawOverrideInfo_.fOverrideEnabled) && (drawOverrideInfo_.fDisplayType == MDAGDrawOverrideInfo::kDisplayTypeTemplate);
			if(overideTemplated)
			{
				drawWireframe = true;
			}
			
			// Override no shaded, only show wireframe
			bool overrideNoShaded = (drawOverrideInfo_.fOverrideEnabled && !drawOverrideInfo_.fEnableShading);
			
			if(overideTemplated || overrideNoShaded)
			{
				drawShaded = false;
			}
			
			///set by node status
			drawAsBoundingBox = drawAsBoundingBox || (param_.displayMode == mode::display::boundingbox);
			drawWireframe     = drawWireframe || (isSelected_);
			
			if(drawWireframe && drawShaded && param_.noWire == true)
			{
				drawWireframe = false;
			}
			
			if(drawAsBoundingBox)
			{
				param_.displayMode = mode::display::boundingbox;
			}
			else if(drawShaded || drawWireframe)
			{
				param_.displayMode /*= displayMode_*/;
			}
			else
			{
				param_.displayMode = mode::display::none;
			}
			
			///-------------------------------------------------------------------- set required drawing mode
			drawShaded_ = drawShaded;
			drawWire_   = drawWireframe;
		}
	}
	
	
	return stat;
}
MStatus DrawOverride::unsetDisplayState_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	MStatus stat;
	
	param_ = Parameter();
	
	isSelected_            = false;
	isPassShaderOverrided_ = false;
	isTransparent_         = false;
	bound_                 = BBox3f();
	drawShaded_            = false;
	drawWire_              = false;
	drawTexture_           = false;
	
	
	return stat;
}
MStatus DrawOverride::setTransformMtx_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	MStatus stat;
	MMatrix mProjectionMtx  = ctx.getMatrix(MHWRender::MDrawContext::kProjectionMtx, &stat);              MCHECKERROR(stat, "Failed to get projection matrix");
	MMatrix mViewMtx        = ctx.getMatrix(MHWRender::MDrawContext::kViewMtx, &stat);                    MCHECKERROR(stat, "Failed to get view matrix");
	MMatrix mModelviewMtx   = ctx.getMatrix(MHWRender::MFrameContext::kWorldViewMtx, &stat);              MCHECKERROR(stat, "Failed to get modleview matrix");
	MMatrix mWorldMtx       = ctx.getMatrix(MHWRender::MFrameContext::kWorldMtx, &stat);                  MCHECKERROR(stat, "Failed to get world matrix");
	MMatrix mNormalMtx      = ctx.getMatrix(MHWRender::MFrameContext::kWorldViewTranspInverseMtx, &stat); MCHECKERROR(stat, "Failed to get normal matrix");
	
	int width, height;
	stat = ctx.getRenderTargetSize(width, height);
	float w2 = width / 2.0f;
	float h2 = height / 2.0f;
	
	//transformMtx_.normalMtx       ; //used glsl built-in uniform
	//transformMtx_.modelViewMtx    ; //used glsl built-in uniform
	//transformMtx_.projectionMtx   ; //used glsl built-in uniform
	//transformMtx_.modelViewProjMtx; //used glsl built-in uniform
	
	/* for drawing wireframe */
	transformMtx_.viewportMtx = Mat4x4f(w2, 0.0f, 0.0f, 0.0f,
	                                    0.0f, h2, 0.0f, 0.0f,
	                                    0.0f, 0.0f, 1.0f, 0.0f,
	                                    w2 + 0, h2 + 0, 0.0f, 1.0f
	);
	
	return stat;
	
}
MStatus DrawOverride::unsetTransformMtx_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	MStatus stat;
	
	transformMtx_ = TransformMtx();
	
	return stat;
	
}
MStatus DrawOverride::setLighting_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	//DECL_LOGGER(true);
	MStatus stat;
	
	const unsigned int nbLights = std::min(ctx.numberOfActiveLights(&stat), 8u);
	MCHECKERRORNORET(stat, "Failed to get # of active light");
	
	const MMatrix worldToView = ctx.getMatrix(MHWRender::MFrameContext::kViewMtx, &stat);
	MCHECKERRORNORET(stat, "Failed to get kViewMtx");
	
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadMatrixd(worldToView.matrix[0]);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	{
		const GLfloat ambient[4]  = {0.0f, 0.0f, 0.0f, 1.0f};
		const GLfloat specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	}
	
	for (unsigned int i = 0u; i < nbLights; ++i)
	{
		MFloatVector direction;
		float        intensity;
		MColor       color;
		bool hasDirection;
		bool hasPosition;
		
		MFloatPointArray positions;
		stat = ctx.getLightInformation(i,
			                           positions,
		                               direction,
		                               intensity,
		                               color,
		                               hasDirection,
		                               hasPosition);
		if(stat != MS::kSuccess)
			continue;
		
		const MFloatPoint& position = positions[0];
		
		if(hasDirection)
		{
			if(hasPosition)
			{
				LOGGER_DEBUG("Assumes a Maya Spot Light");
				const GLfloat ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
				const GLfloat diffuse[4] = {intensity * color[0],
				                            intensity * color[1],
				                            intensity * color[2],
				                            1.0f
				};
				const GLfloat pos[4]     = {position[0],
				                            position[1],
				                            position[2],
				                            1.0f
				};
				const GLfloat dir[3]     = {direction[0],
				                            direction[1],
				                            direction[2]
				};
				
				glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
				glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
				glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
				glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, dir);
				
				// Maya's default values for spot lights.
				glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 0.0);
				glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 20.0);
			}
			else
			{
				LOGGER_DEBUG("Assumes a Maya Directional Light");
				const GLfloat ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
				const GLfloat diffuse[4] = {intensity * color[0],
				                            intensity * color[1],
				                            intensity * color[2],
				                            1.0f
				};
				const GLfloat pos[4]     = {-direction[0],
				                            -direction[1],
				                            -direction[2],
				                            0.0f
				};
				
				glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
				glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
				glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
				glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.0f);
			}
		}
		else if(hasPosition)
		{
			LOGGER_DEBUG("Assumes a Maya Point Light!");
			const GLfloat ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			const GLfloat diffuse[4] = {intensity * color[0],
			                            intensity * color[1],
			                            intensity * color[2],
			                            1.0f};
			const GLfloat pos[4]     = {position[0],
			                            position[1],
			                            position[2],
			                            1.0f
			};
			
			glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.0f);
		}
		else
		{
			LOGGER_DEBUG("Assumes a Maya Ambient Light");
			const GLfloat ambient[4] = {intensity * color[0],
			                            intensity * color[1],
			                            intensity * color[2],
			                            1.0f
			};
			const GLfloat diffuse[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			const GLfloat pos[4]     = {0.0f, 0.0f, 0.0f, 1.0f};
			
			glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.0f);
		}
		glEnable(GL_LIGHT0 + i);
	}
	
	glDisable(GL_LIGHTING);
	
	//glPopMatrix();
	
	
	return stat;
	
}
MStatus DrawOverride::unsetLighting_(const MHWRender::MDrawContext& ctx)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	MStatus stat;
	
	// Take into account only the 8 lights supported by the basic OpenGL profile.
	const unsigned int nbLights = std::min(ctx.numberOfActiveLights(&stat), 8u);
	MCHECKERRORNORET(stat, "Failed to get # of active light");
	
	if (nbLights == 0u)
	{
		return MS::kSuccess;
	}
	// Restore OpenGL default values for anything that we have modified.
	for (unsigned int i = 0u; i < nbLights; ++i)
	{
		glDisable(GL_LIGHT0 + i);
		
		const GLfloat ambient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
		
		if(i == 0u)
		{
			const GLfloat diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			
			const GLfloat spec[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, spec);
		}
		else
		{
			const GLfloat diffuse[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
			
			const GLfloat spec[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, spec);
		}
		
		const GLfloat pos[4] = {0.0f, 0.0f, 1.0f, 0.0f};
		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
		
		const GLfloat dir[3] = {0.0f, 0.0f, -1.0f};
		glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, dir);
		
		glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 0.0);
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180.0);
	}
	
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	
	const GLfloat ambient[4]  = {0.2f, 0.2f, 0.2f, 1.0f};
	const GLfloat specular[4] = {0.8f, 0.8f, 0.8f, 1.0f};
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	return stat;
	
}

void DrawOverride::bindContext_(const MHWRender::MDrawContext* pContext, const MHWRender::MShaderInstance* pOverridedShader)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	if(pOverridedShader)
	{
		isPassShaderOverrided_ = true;
		
		MHWRender::MVertexBufferDescriptorList bufferList;
		pOverridedShader->requiredVertexBuffers(bufferList);
		//for (int i = 0; i < bufferList.length(); i++)
		//{
		//	MHWRender::MVertexBufferDescriptor desc;
		//	bufferList.getDescriptor(i, desc);
		//	LOGGER_DEBUG("Buffer[%d][name=%s]", i, desc.name().asChar());
		//	LOGGER_DEBUG("[semantic = %s", desc.semanticName().asChar());
		//	LOGGER_DEBUG("[dataType = %d", desc.dataType());
		//	LOGGER_DEBUG("[dataTyleSize = %d", desc.dataTypeSize());
		//	LOGGER_DEBUG("[dimension = %d", desc.dimension());
		//	LOGGER_DEBUG("[offset = %d", desc.offset());
		//	LOGGER_DEBUG("[stride = %d", desc.stride());
		//}
		
		pOverridedShader->bind(*pContext);
		pOverridedShader->updateParameters(*pContext);
		pOverridedShader->activatePass(*pContext, 0);
	}
	else
	{
		isPassShaderOverrided_ = false;
	}
	
}
void DrawOverride::unbindContext_(const MHWRender::MDrawContext* pContext, const MHWRender::MShaderInstance* pOverridedShader)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	if(pOverridedShader)
	{
		isPassShaderOverrided_ = false;
		pOverridedShader->unbind(*pContext);
	}
	
}
void DrawOverride::drawBound_(const MHWRender::MDrawContext& context, const MHWRender::MShaderInstance* pOverridedShader)
{
	DECL_LOGGER(logging::maya->DrawOverride);
	float minx = bound_.lower()[0];
	float miny = bound_.lower()[1];
	float minz = bound_.lower()[2];
	float maxx = bound_.upper()[0];
	float maxy = bound_.upper()[1];
	float maxz = bound_.upper()[2];
	Vec4fArray pointArray(8);
	pointArray[0] = transformMtx_.xformMtx * Vec4f(minx, miny, maxz, 1.0f); //v0, front-bottom-left
	pointArray[1] = transformMtx_.xformMtx * Vec4f(maxx, miny, maxz, 1.0f); //v1	front-bottom-right
	pointArray[2] = transformMtx_.xformMtx * Vec4f(maxx, maxy, maxz, 1.0f); //v2	front-top-right
	pointArray[3] = transformMtx_.xformMtx * Vec4f(minx, maxy, maxz, 1.0f); //v3	front-top-left
	pointArray[4] = transformMtx_.xformMtx * Vec4f(minx, miny, minz, 1.0f); //v4  back-bottom-left
	pointArray[5] = transformMtx_.xformMtx * Vec4f(maxx, miny, minz, 1.0f); //v5  back-bottom-right
	pointArray[6] = transformMtx_.xformMtx * Vec4f(maxx, maxy, minz, 1.0f); //v6  back-top-right
	pointArray[7] = transformMtx_.xformMtx * Vec4f(minx, maxy, minz, 1.0f); //v7  back-top-left
	
	glPushAttrib(GL_LIGHTING_BIT);
	{
		glDisable(GL_LIGHTING);
		//front
		glBegin(GL_LINE_LOOP);
		glVertex3f(pointArray[0][0], pointArray[0][1], pointArray[0][2]);
		glVertex3f(pointArray[1][0], pointArray[1][1], pointArray[1][2]);
		glVertex3f(pointArray[2][0], pointArray[2][1], pointArray[2][2]);
		glVertex3f(pointArray[3][0], pointArray[3][1], pointArray[3][2]);
		glEnd();
		
		//back
		glBegin(GL_LINE_LOOP);
		glVertex3f(pointArray[4][0], pointArray[4][1], pointArray[4][2]);
		glVertex3f(pointArray[5][0], pointArray[5][1], pointArray[5][2]);
		glVertex3f(pointArray[6][0], pointArray[6][1], pointArray[6][2]);
		glVertex3f(pointArray[7][0], pointArray[7][1], pointArray[7][2]);
		glEnd();
		
		//top
		
		//bottom
		
		//left
		glBegin(GL_LINE_LOOP);
		glVertex3f(pointArray[0][0], pointArray[0][1], pointArray[0][2]);
		glVertex3f(pointArray[3][0], pointArray[3][1], pointArray[3][2]);
		glVertex3f(pointArray[7][0], pointArray[7][1], pointArray[7][2]);
		glVertex3f(pointArray[4][0], pointArray[4][1], pointArray[4][2]);
		glEnd();
		
		//right
		glBegin(GL_LINE_LOOP);
		glVertex3f(pointArray[5][0], pointArray[5][1], pointArray[5][2]);
		glVertex3f(pointArray[6][0], pointArray[6][1], pointArray[6][2]);
		glVertex3f(pointArray[2][0], pointArray[2][1], pointArray[2][2]);
		glVertex3f(pointArray[1][0], pointArray[1][1], pointArray[1][2]);
		glEnd();
	}
	glPopAttrib();
}


NAMESPACE_BASE_END




