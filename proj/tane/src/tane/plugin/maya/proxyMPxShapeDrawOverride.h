#pragma once
#include "maya/pch.h"

#include "maya/drawOverride.h"
#include "maya/proxyMPxShape.h"

NAMESPACE_BASE_BEGIN
class ProxyDrawOverride : public DrawOverride
{
public:
	typedef DrawOverride Base;
private:
	std::shared_ptr<StateQueue>     pMsgQueue_;
	std::shared_ptr<Source>         pSource_;
	
	std::shared_ptr<InstancingTask> pInstancingTask_;
	std::shared_ptr<RenderTask>     pRenderTask_;
public:
	ProxyDrawOverride() : DrawOverride()
	{
	
	}
	virtual ~ProxyDrawOverride()
	{
	}
protected:
	virtual MStatus prepareForDrawImpl_(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext)
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		
		MStatus    stat;
		MFnDagNode shapeFn(objPath.node());
		ProxyMPxShape* pShape = dynamic_cast<ProxyMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		
		//LOGGER_DEBUG("isSelected      = %d", isSelected_);
		//
		//LOGGER_DEBUG("forceStatic     = %d", forceStatic_);
		//LOGGER_DEBUG("animCycle       = %d", animCycle_);
		//LOGGER_DEBUG("time            = (%5.2f, %5.2f)", time_.fps, time_.sec);
		//
		//LOGGER_DEBUG("enableLight     = %d", enableLight_);
		//LOGGER_DEBUG("enableDrawBound = %d", enableDrawBound_);
		//LOGGER_DEBUG("enableWireFrame = %d", enableWireFrame_);
		//
		//LOGGER_DEBUG("displayMode     = %d", displayMode_);
		//LOGGER_DEBUG("drawShaded      = %d", drawShaded_);
		//LOGGER_DEBUG("drawWire        = %d", drawWire_);
		//LOGGER_DEBUG("drawTexture     = %d", drawTexture_);
		
		pSource_   = pShape->outSource();
		if(!pSource_ || !pSource_->renderItem())
		{
			STDOUT_ERROR("Failed to get pSource");
			return MS::kFailure;
		}
		pMsgQueue_ = pShape->stateQueue();
		if(!pMsgQueue_)
		{
			STDOUT_ERROR("Failed to get MsgQueue");
			return MS::kFailure;
		}
		
		return stat;
	}
	virtual void drawImpl_(const MHWRender::MDrawContext& context, const MHWRender::MShaderInstance* pOverridedShader)
	{
		//DECL_LOGGER(log::ProxyMPxShapeDrawOverride);
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		
		if(!pMsgQueue_->empty() && !processMessage_())
		{
			STDOUT_ERROR("Failed to process message");
			return;
		}
		
		if(!pInstancingTask_ || !pInstancingTask_->isPopulated())
		{
			LOGGER_ERROR("Failed to get pInstancingTask_");
			return;
		}
		
		if(!pRenderTask_ || !pRenderTask_->isPopulated())
		{
			LOGGER_ERROR("Failed to get pRenderTask");
			return;
		}
		
		/* update color buffer */
		if(!updateColorBuffer_())
		{
			STDOUT_ERROR("Failed to update color buffer");
			return;
		}
		/* update wire color buffer */
		if(!updateWireColorBuffer_())
		{
			STDOUT_ERROR("Failed to update wire color buffer");
			return;
		}
		
		
		///==============================================================================
		/// Draw ICR
		///==============================================================================
		LOGGER_IN("Draw In Front Buffer");
		{
			const std::shared_ptr<GlIcrShader> pShader = GlIcrShader::Get();
			if(!isPassShaderOverrided())
			{
				pShader->use();
				{
					pShader->setUniform(GlIcrShader::kXformMtx,  transformMtx().xformMtx);
					pShader->setUniform(GlIcrShader::kEnableIcr, parameter().enableIcr);
					pShader->setUniform(GlIcrShader::kMinCorner, bound().lower());
					pShader->setUniform(GlIcrShader::kMaxCorner, bound().upper());
					
					pShader->setUniform(GlIcrShader::kEnableLOD,   parameter().enableLOD);
					pShader->setUniform(GlIcrShader::kMinDistance, parameter().minDistance);
					pShader->setUniform(GlIcrShader::kMaxDistance, parameter().maxDistance);
				}
				pShader->unuse();
			}
			pRenderTask_->drawICR(isPassShaderOverrided());
		}
		LOGGER_OUT();
		
		///==============================================================================
		/// Draw In Front Buffer
		///==============================================================================
		LOGGER_IN("Draw In Front Buffer");
		{
			static const bool iterated = false;
			
			if(!isPassShaderOverrided())
			{
				const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
				pShader->use();
				{
					pShader->setUniform(GlIcrPhongShader::kXformMtx,        transformMtx().xformMtx);
					pShader->setUniform(GlIcrPhongShader::kOverrideMask,    parameter().overrideMask);
					pShader->setUniform(GlIcrPhongShader::kOverrideTexture, drawTexture());
					pShader->setUniform(GlIcrPhongShader::kNoLighting,      parameter().noLighting);
				}
				pShader->unuse();
			}
			
			if(drawShaded())
			{
				pRenderTask_->drawShaded(isPassShaderOverrided(), iterated);
			}
			if(drawWire())
			{
				pRenderTask_->drawWire(isPassShaderOverrided(), iterated);
			}
		}
		LOGGER_OUT();
		///==============================================================================
		/// Draw In Back Buffer
		///==============================================================================
		LOGGER_IN("Draw In Back Buffer");
		{
		}
		LOGGER_OUT();
	}

private:
	bool processMessage_()
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		
		const std::shared_ptr<RenderItem> pRenderItem = pSource_->renderItem();
		LOGGER_DEBUG("RenderItem Type = %s", RenderItem::GetTypeStr(pRenderItem->type()).c_str());
		LOGGER_DEBUG("pMsgQueue_->size() = %u", pMsgQueue_->size());
		while (!pMsgQueue_->empty())
		{
			switch (pMsgQueue_->top())
			{
				case State::kNODE_DELECETED:
					LOGGER_IN("kNODE_DELECETED");
					{
						if(pInstancingTask_ && pInstancingTask_->isPopulated())
							pInstancingTask_->depopulate();
						
						if(pRenderTask_ && pRenderTask_->isPopulated())
							pRenderTask_->depopulate();
					}
					LOGGER_OUT();
					break;
				
				case State::kNODE_ADDED:
				case State::kARCHIVE_ATTRIBUTE_CHANGED:
				case State::kDISPLAY_MODE_ATTRIBUTE_CHANGED:
					LOGGER_IN("kNODE_ADDED || kARCHIVE_ATTRIBUTE_CHANGED || kDISPLAY_MODE_ATTRIBUTE_CHANGED");
					{
						LOGGER_IN("populate new instancing task");
						{
							if(pInstancingTask_ && pInstancingTask_->isPopulated())
							{
								pInstancingTask_->depopulate();
							}
							
							const std::shared_ptr<InstancingItem> pInstancingItem = InstancingItem::Create();
							pInstancingTask_ = InstancingTask::Create(pInstancingItem);
							pInstancingTask_->populate();
						}
						LOGGER_OUT();
						
						LOGGER_IN("populating new render task");
						{
							/* depopulate and remove previous renderTask */
							if(pRenderTask_ && pRenderTask_->isPopulated())
							{
								pRenderTask_->depopulate();
							}
							pRenderTask_ = nullptr;
							
							
							/* create new renderTask */
							switch (parameter().displayMode)
							{
								case mode::display::none:
								{
									pRenderTask_ = nullptr;
								}
									break;
								case mode::display::points:
								{
									pRenderTask_ = nullptr;
								}
									break;
								case mode::display::boundingbox:
								{
									LOGGER_DEBUG("make BBoxTask");
									const std::shared_ptr<RenderItem> pRenderItem = pSource_->renderItem();
									std::shared_ptr<BBoxItem> pBBoxItem = std::make_shared<BBoxItem>(pRenderItem->bbox());
									pRenderTask_ = RenderTask::Create(pBBoxItem, 0, pInstancingTask_);
								}
									break;
								case mode::display::proxy:
								{
									LOGGER_DEBUG("make GeomTask");
									pRenderTask_ = RenderTask::Create(pSource_->renderItem(), 0, pInstancingTask_);
								}
									break;
							}
							if(pRenderTask_)
							{
								pRenderTask_->populate();
							}
						}
						LOGGER_OUT();
					}
					LOGGER_OUT();
					break;
				case State::kSHADER_ATTRIBUTE_CHANGED:
					LOGGER_IN("kSHADER_ATTRIBUTE_CHANGED");
					{
					}
					LOGGER_OUT();
					break;
				case State::kRENDER_ATTRIBUTE_CHAGNED:
					LOGGER_IN("kRENDER_ATTRIBUTE_CHAGNED");
					{
					}
					LOGGER_OUT();
					break;
				case State::kTIME_CHAGNED:
					LOGGER_IN("kTIME_CHAGNED");
					{
						if(pRenderTask_ && pRenderTask_->isPopulated())
							pRenderTask_->update();
					}
					LOGGER_OUT();
					break;
			}
			pMsgQueue_->pop();
		}
		
		return true;
	}
	bool updateColorBuffer_()
	{
		//DECL_LOGGER(log::ProxyMPxShapeDrawOverride);
		DECL_LOGGER(false);
		if(!pInstancingTask_ || !pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("pInstancingTask is not valid");
			return false;
		}
		
		/* update color buffer */
		const int npoints = pInstancingTask_->item()->npoints();
		GlRGBAColorArrayBuffer& cbo  = pInstancingTask_->instancingBuffers().colorBuffer;
		
		cbo.bind();
		{
			RGBAColor* pColorArray = cbo.mapBuffer(GL_WRITE_ONLY);
			std::fill(pColorArray,
			          pColorArray  + npoints,
			          RGBAColor(parameter().color[0], parameter().color[1], parameter().color[2], parameter().transparency));
			cbo.unmapBuffer();
		}
		cbo.unbind();
		return true;
	}
	bool updateWireColorBuffer_()
	{
		//DECL_LOGGER(log::ProxyMPxShapeDrawOverride);
		DECL_LOGGER(false);
		if(!pInstancingTask_ || !pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("pInstancingTask is not valid");
			return false;
		}
		/* update color buffer */
		const int npoints = pInstancingTask_->item()->npoints();
		GlRGBAColorArrayBuffer& wcbo = pInstancingTask_->instancingBuffers().wireColorBuffer;
		
		wcbo.bind();
		{
			RGBAColor* pWireColorArray = wcbo.mapBuffer(GL_WRITE_ONLY);
			std::fill(pWireColorArray,
			          pWireColorArray + npoints,
			          RGBAColor(parameter().wireColor[0], parameter().wireColor[1], parameter().wireColor[2], parameter().transparency));
			wcbo.unmapBuffer();
		}
		wcbo.unbind();
		
		return true;
	}

};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------------------------ ProxyMPxShapeDrawOverride
class ProxyMPxShapeDrawOverride : public MHWRender::MPxDrawOverride
{
public:
	typedef MHWRender::MPxDrawOverride Base;

protected:
	ProxyMPxShapeDrawOverride(const MObject &obj) : Base(obj, DrawOverride::DrawCallback, /*isAlwaysDirty*/ true )
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		
		LOGGER_DEBUG("getShapePath_(obj).fullPathName() = %s", getShapePath_(obj).fullPathName().asChar());
		
		DrawOverride::Register(getShapePath_(obj), std::make_shared<ProxyDrawOverride>());
		
	}
	virtual ~ProxyMPxShapeDrawOverride ()
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
	}

public:
	virtual MUserData* prepareForDraw(const MDagPath &objPath, const MDagPath &cameraPath, const MHWRender::MFrameContext &frameContext, MUserData* oldData)
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		
		MDagPath shapePath = getShapePath_(objPath.node());
		LOGGER_DEBUG("shapePath = %s", shapePath.fullPathName().asChar());
		const std::shared_ptr<DrawOverride> pDrawOverride = DrawOverride::GetDrawOverride(shapePath);
		if(!pDrawOverride)
		{
			STDOUT_ERROR("Failed to get pDrawOverride");
			return nullptr;
		}
		return pDrawOverride->prepareForDraw(shapePath, cameraPath, frameContext, oldData);
	}
	virtual MHWRender::DrawAPI supportedDrawAPIs() const
	{
		return (MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile);
	}
	
	virtual bool isBounded(const MDagPath &objPath, const MDagPath &cameraPath) const
	{
		return true;
	}
	virtual MBoundingBox boundingBox(const MDagPath &objPath, const MDagPath &cameraPath) const
	{
		DECL_LOGGER(logging::maya->ProxyMPxShapeDrawOverride);
		//DECL_LOGGER(false);

		MStatus stat;
		MFnDagNode shapeFn(objPath.node(&stat));
		ProxyMPxShape* pShape = dynamic_cast<ProxyMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			STDOUT_ERROR("Failed to get pShape");
			return MBoundingBox();
		}

#ifdef DEBUG
		const MBoundingBox& boundingBox = pShape->boundingBox();
		const Point3f minCorner = Point3f(boundingBox.min()[0], boundingBox.min()[1], boundingBox.min()[2]);
		const Point3f maxCorner = Point3f(boundingBox.min()[0], boundingBox.min()[1], boundingBox.min()[2]);
		LOGGER_DEBUG("bound.lower() = (%5.2f, %5.2f, %5.2f)", minCorner[0], minCorner[1], minCorner[2]);
		LOGGER_DEBUG("bound.upper() = (%5.2f, %5.2f, %5.2f)", maxCorner[0], maxCorner[1], maxCorner[2]);
#endif

		return pShape->boundingBox();


		//DECL_LOGGER(log::ProxyMPxShapeDrawOverride);
		//
		//MDagPath shapePath = getShapePath_(objPath.node());
		//const std::shared_ptr<DrawOverride>& pDrawOverride = DrawOverride::GetDrawOverride(shapePath);
		//if(!pDrawOverride)
		//{
		//	STDOUT_ERROR("Failed to get pDrawOverride");
		//	return MPxDrawOverride::boundingBox(objPath, cameraPath);
		//}
		//
		//const BBox3f& bound = pDrawOverride->bound();
		//LOGGER_DEBUG("bound.lower() = (%5.2f, %5.2f, %5.2f)", bound.lower()[0], bound.lower()[1], bound.lower()[2]);
		//LOGGER_DEBUG("bound.upper() = (%5.2f, %5.2f, %5.2f)", bound.upper()[0], bound.upper()[1], bound.upper()[2]);
		//return MBoundingBox(MPoint(bound.lower()[0], bound.lower()[1], bound.lower()[2]),
		//                    MPoint(bound.upper()[0], bound.upper()[1], bound.upper()[2])
		//);
		
	}
	virtual MMatrix transform(const MDagPath& objPath, const MDagPath& cameraPath) const
	{
		//DECL_LOGGER(log::ProxyMPxShapeDrawOverride);
		DECL_LOGGER(false);
		return MPxDrawOverride::transform(objPath, cameraPath);
	}
	virtual bool isTransparent() const
	{
		return false;
	}

private:
	MObject  getShape_(const MObject& obj) const
	{
		MDagPath objPath = GetDagPath(obj);
		return objPath.node();
	}
	MDagPath getShapePath_(const MObject& obj) const
	{
		MDagPath objPath = GetDagPath(obj);
		return objPath;
	}
};



NAMESPACE_BASE_END

