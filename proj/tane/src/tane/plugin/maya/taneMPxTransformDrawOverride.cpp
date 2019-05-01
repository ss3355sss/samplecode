#include "maya/taneMPxTransformDrawOverride.h"

#include "lib/baseMaya/util.h"

#include "base/renderItem.h"

#include "baseGl/renderTask.h"
#include "baseGl/taskManager.h"

#include "maya/drawOverride.h"

#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

#include "maya/taneMPxTransform.h"
#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
///--------------------------------------------------------------------------- TaneMPxTransformDrawOverride */
class TaneTransformDrawOverride : public DrawOverride
{
public:
	typedef DrawOverride                        Base;
public:
	typedef TaneMPxShape::State                 State;
	typedef TaneMPxShape::StateQueue            StateQueue;
	
	typedef std::map<int, Mat4x4f>              XFormMap;
	typedef std::map<int, RGBColor>             ColorMap;
	typedef std::map<int, short>                DisplayModeMap;
	
private:
	std::shared_ptr<StateQueue>                 pStateQueue_;
	std::shared_ptr<SourceManager>              pSourceMgr_;
	std::shared_ptr<Points>                     pPoints_;
	std::shared_ptr<InstancingItem>             pInstancingItem_;

	TaskManager                                 taskMgr_;
	
	XFormMap                                    xformMap_;
	short                                       colorMode_;
	ColorMap                                    colorMap_;
	DisplayModeMap                              displayModeMap_;
	
	MIntArray           					    selectedIDs_;
public:
	TaneTransformDrawOverride(const std::shared_ptr<InstancingItem> pInstancingItem) : Base()
	{
	}
	virtual ~TaneTransformDrawOverride()
	{
	}
protected:
	virtual MStatus prepareForDrawImpl_(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		MStatus stat;
		MFnDagNode shapeFn(objPath.node());
		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			LOGGER_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		
		pStateQueue_     = pShape->stateQueue();
		pPoints_         = pShape->outPoints();
		pSourceMgr_      = pShape->outSources();
		pInstancingItem_ = pShape->instancingItem();
		
		if(!pStateQueue_)       { LOGGER_ERROR("Failed to get pStateQueue");     return MS::kFailure; }
		if(!pPoints_)           { LOGGER_ERROR("Failed to get pPoints");         return MS::kFailure; }
		if(!pSourceMgr_)        { LOGGER_ERROR("Failed to get pSourceMgr_");     return MS::kFailure; }
		if(!pInstancingItem_)   { LOGGER_ERROR("Failed to get pInstancingItem"); return MS::kFailure; }
		
		/* update seletedIDs*/
		stat = updateSeletedIDs_(objPath);
		MCHECKERROR(stat, "Failed to update seletedIDs");
		
		/* update map (per source data) */
		stat = updateMap_(objPath);
		MCHECKERROR(stat, "Failed to update map");
		
		return stat;
	}
	virtual void drawImpl_(const MHWRender::MDrawContext& context, const MHWRender::MShaderInstance* pOverridedShader)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		
		
		if(!pStateQueue_->empty() && !processMessage_())
		{
			LOGGER_ERROR("Failed to process message");
			return;
		}
		
		/* update color buffer */
		if(!updateColorBuffer_())
		{
			LOGGER_ERROR("Failed to update color buffer");
			return;
		}
		
		/* update wire color buffer */
		if(!updateWireColorBuffer_())
		{
			LOGGER_ERROR("Failed to update wire color buffer");
			return;
		}
		
		static const bool iterated = false;
		const TaskManager::TaskArray& taskArray = taskMgr_.taskArray();
		for(size_t i = 0; i < taskArray.size(); ++i)
		{
			const int                         instanceID  = taskArray[i].first;
			const std::shared_ptr<RenderTask> pRenderTask = taskArray[i].second;
			if(!pRenderTask || !pRenderTask->isPopulated())
			{
				LOGGER_ERROR("Failed to get valid pRenderTask[%d]", instanceID);
				return;
			}
			
			const BBox3f & bbox     = pRenderTask->item()->bbox();
			const Mat4x4f& xformMtx = transformMtx().xformMtx * xformMap_[instanceID];

			LOGGER_IN("Draw RenderItem[%d], type = %s", instanceID, RenderItem::GetTypeStr(pRenderTask->itemType()).c_str());
			{
				LOGGER_DEBUG("bbox[%d].lower() = (%5.2f, %5.2f, %5.2f)", instanceID, bbox.lower()[0], bbox.lower()[1], bbox.lower()[2]);
				LOGGER_DEBUG("bbox[%d].upper() = (%5.2f, %5.2f, %5.2f)", instanceID, bbox.upper()[0], bbox.upper()[1], bbox.upper()[2]);
				///==============================================================================
				/// Draw ICR
				///==============================================================================
				LOGGER_IN("Draw ICR[%d]", instanceID);
				{
					const std::shared_ptr<GlIcrShader> pShader = GlIcrShader::Get();
					if(!isPassShaderOverrided())
					{
						pShader->use();
						{
							pShader->setUniform(GlIcrShader::kXformMtx,  xformMtx);
							pShader->setUniform(GlIcrShader::kEnableIcr, parameter().enableIcr);
							pShader->setUniform(GlIcrShader::kMinCorner, bbox.lower());
							pShader->setUniform(GlIcrShader::kMaxCorner, bbox.upper());
							pShader->setUniform(GlIcrShader::kEnableLOD,   parameter().enableLOD);
							pShader->setUniform(GlIcrShader::kMinDistance, parameter().minDistance);
							pShader->setUniform(GlIcrShader::kMaxDistance, parameter().maxDistance);
						}
						pShader->unuse();
					}
					pRenderTask->drawICR(isPassShaderOverrided());
				}
				LOGGER_OUT();
				///==============================================================================
				/// Draw In Front Buffer
				///==============================================================================
				LOGGER_IN("Draw In Front Buffer[%d]", instanceID);
				{
					const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
					if(!isPassShaderOverrided())
					{
						pShader->use();
						{
							pShader->setUniform(GlIcrPhongShader::kXformMtx,        xformMtx);
							pShader->setUniform(GlIcrPhongShader::kOverrideMask,    parameter().overrideMask);
							pShader->setUniform(GlIcrPhongShader::kOverrideTexture, drawTexture());
							pShader->setUniform(GlIcrPhongShader::kNoLighting,      parameter().noLighting);
						}
						pShader->unuse();
					}
					if(drawShaded())
					{
						pRenderTask->drawShaded(isPassShaderOverrided(), iterated);
					}
					if(drawWire())
					{
						pRenderTask->drawWire(isPassShaderOverrided(), iterated);
					}
					
				}
				LOGGER_OUT();
				
				///==============================================================================
				/// Draw In Back Buffer
				///==============================================================================
				LOGGER_IN("Draw In Back Buffer[%d]", instanceID);
				{
				}
				LOGGER_OUT();
			}
			LOGGER_OUT();
		}
	}
private:
	bool processMessage_()
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);

		int cnt = 0;
		while (!pStateQueue_->empty())
		{
			const TaneMPxShape::State& state = pStateQueue_->top();
			switch (state)
			{
				case State::kNODE_DELECETED:
				LOGGER_IN("kNODE_DELECETED");
				{
					TaskManager::TaskArray taskArray = taskMgr_.taskArray();
					for (int i = 0; i < taskArray.size(); ++i)
					{
						const std::shared_ptr<RenderTask> pRenderTask = taskArray[i].second;
						if(pRenderTask && pRenderTask->isPopulated())
						{
							pRenderTask->depopulate();
						}
					}
					const std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
					if(pInstancingTask && pInstancingTask->isPopulated())
					{
						pInstancingTask->depopulate();
					}
				}
				LOGGER_OUT();
				break;
				case State::kNODE_ADDED:
				case State::kINPUT_DIRTIED:
				case State::kDISPLAY_MODE_CHANGED:
				LOGGER_IN("kNODE_ADDED || kINPUT_DIRTIED || kDISPLAY_MODE_CHANGED");
				{
					LOGGER_IN("populate instancing task");
					{
						/* depopulate previous instancingTask if exists */
						taskMgr_.clearInstancingTask();
						
						/* create new instancingTask */
						std::shared_ptr<InstancingTask> pInstancingTask = InstancingTask::Create(pInstancingItem_);
						pInstancingTask->populate();
						taskMgr_.setInstancingTask(pInstancingTask);
						
					}
					LOGGER_OUT();
					LOGGER_IN("populate render task ");
					{
						/* depopulate previous renderTask if exists */
						taskMgr_.clearRenderTasks();

						std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
			
						for (SourceManager::iterator iter = pSourceMgr_->begin(); iter != pSourceMgr_->end(); ++iter)
						{
							const int                         instanceId  = iter->first;
							const std::shared_ptr<Source>     pSource     = iter->second;
							const std::shared_ptr<RenderItem> pRenderItem = pSource->renderItem();
							if(!pRenderItem)
							{
								LOGGER_ERROR("Failed to get renderItem[%d]", instanceId);
								continue;
							}
							LOGGER_DEBUG("source[%d] = %s", instanceId, pSource->objPath().fullPathName().asChar());
			
							std::shared_ptr<RenderTask> pRenderTask = nullptr;
							/* create new renderTask by display mode*/
							switch (parameter().displayMode)
							{
								case mode::display::none:
								{
									pRenderTask = nullptr;
								}
								break;
								case mode::display::points:
								{
									pRenderTask = nullptr;
								}
								break;
								case mode::display::boundingbox:
								{
									LOGGER_DEBUG("make BBoxTask");
									std::shared_ptr<BBoxItem> pBBoxItem = std::make_shared<BBoxItem>(pRenderItem->bbox());
									pRenderTask = BBoxTask::Create(pBBoxItem, instanceId, pInstancingTask);
									
								}
								break;
								case mode::display::proxy:
								{
									LOGGER_DEBUG("make GeomTask");
									pRenderTask = RenderTask::Create(pRenderItem, instanceId, pInstancingTask);
								}
								break;
								case mode::display::foreach:
								{
									LOGGER_DEBUG("make Foreach Task");
									const short dispMode = displayModeMap_[instanceId];
									LOGGER_DEBUG("dispMode = %u", dispMode);
									if(dispMode == mode::display::none)
									{
									}
									else if(dispMode == mode::display::boundingbox)
									{
										std::shared_ptr<BBoxItem> pBBoxItem = std::make_shared<BBoxItem>(pRenderItem->bbox());
										pRenderTask = BBoxTask::Create(pBBoxItem, instanceId, pInstancingTask);
									}
									else if(dispMode == mode::display::proxy)
									{
										pRenderTask = RenderTask::Create(pRenderItem, instanceId, pInstancingTask);
									}
								}
								break;
							}
							if(pRenderTask)
							{
								pRenderTask->populate();
								taskMgr_.setRenderTask(instanceId, pRenderTask);
							}
						}
					}
					LOGGER_OUT();
				}
				LOGGER_OUT();
				break;
				case State::kMANIPULATING_SOURCE_ID:
				LOGGER_IN("kMANIPULATING_SOURCE_ID");
				{
				}
				LOGGER_OUT();
				break;
				case State::kMANIPULATING_RENDERABLE:
				LOGGER_IN("kMANIPULATING_RENDERABLE");
				{
					updateRenderableBuffer_();
				}
				LOGGER_OUT();
				break;
				case State::kMANIPULATING_TRANSFORM:
				LOGGER_IN("kMANIPULATING_TRANSFORM");
				{
					updateInstancingMtxBuffer_();
				}
				LOGGER_OUT();
				break;
				case State::kSHADER_ATTRIBUTE_CHANGED:
				LOGGER_IN("kSHADER_ATTRIBUTE_CHANGED");
				{
				}
				LOGGER_OUT();
				break;
				case State::kTIME_CHANGED:
				LOGGER_IN("kTIME_CHANGED");
				{
					/* update instancingTask */
					{
						updateInstancingMtxBuffer_();
					}
					/* update renderTask */
					{
						TaskManager::TaskArray taskArray = taskMgr_.taskArray();
						for(int i = 0; i < taskArray.size(); ++i)
						{
							const std::shared_ptr<RenderTask> pRenderTask = taskArray[i].second;
							if(pRenderTask && pRenderTask->isPopulated())
							{
								pRenderTask->update();
							}
						}
					}
				}
				LOGGER_OUT();
				break;
			}
			pStateQueue_->pop();
		}
		return true;
	}

	MStatus updateMap_(const MDagPath& objPath)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		MStatus stat;
		MFnDagNode shapeFn(objPath.node());
		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));
		if(!pShape)
		{
			LOGGER_ERROR("Failed to get pShape");
			return MS::kFailure;
		}
		
		/* get ColorMode */
		MPlug colorModePlug(objPath.node(), shapeFn.attribute("colorMode", &stat));
		colorMode_ = colorModePlug.asShort();

		xformMap_.clear();
		colorMap_.clear();
		displayModeMap_.clear();
		for(SourceManager::iterator iter = pSourceMgr_->begin(); iter != pSourceMgr_->end(); ++iter)
		{
			const int                     elemIndex = iter->first;
			const std::shared_ptr<Source> pSource   = iter->second;

			const MDagPath objPath = pSource->objPath();
			LOGGER_DEBUG("objPath[%d] = %s", elemIndex, objPath.fullPathName().asChar())

			MFnDagNode dagFn(objPath.node(), &stat);
			MCHECKERROR(stat, "Failed to create dagFn");
			/* get xform of source */
			Mat4x4f xformMtx = Mat4x4f::Identity();
			{
				stat= GetXformMtx(objPath.transform(), &xformMtx);
				MCHECKERROR(stat, "Failed to get xform, %s", dagFn.fullPathName().asChar());
			}
			/* get color of source */
			RGBColor color;
			{
				MPlug colorPlug(dagFn.object(), dagFn.attribute("color"));
				color = RGBColor(colorPlug.child(0).asFloat(), colorPlug.child(1).asFloat(), colorPlug.child(2).asFloat());
			}
			/* get displayMode of source */
			short displayMode;
			{
				MPlug displayModePlug(dagFn.object(), dagFn.attribute("displayMode"));
				displayMode = displayModePlug.asShort();
			}
			
			
			xformMap_[elemIndex]       = xformMtx;
			colorMap_[elemIndex]       = color;
			displayModeMap_[elemIndex] = displayMode;
		}
		return stat;
	}
	MStatus updateSeletedIDs_(const MDagPath& objPath)
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		MStatus stat;

		MFnDagNode shapeFn(objPath.node());
		TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));

		selectedIDs_.clear();
		if(pShape->hasActiveComponents())
		{
			MObjectArray activeComponents = pShape->activeComponents();
			if(activeComponents.length())
			{
				MFnSingleIndexedComponent fnComponent(activeComponents[0]);
				if(fnComponent.elementCount())
				{
					MFn::Type compType = fnComponent.componentType(&stat);
					if(compType == MFn::kMeshVertComponent)
					{
						stat = fnComponent.getElements(selectedIDs_);
					}
				}
			}
		}
		return stat;
	}
	bool updateInstancingMtxBuffer_()
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
		if(!pInstancingTask || !pInstancingTask->isPopulated())
		{
			LOGGER_ERROR("pInstancingTask is not valid");
			return false;
		}
		
		const std::shared_ptr<InstancingItem> pInstancingItem = pInstancingTask->item();
		const Mat4x4fArray& instancingMtxArray = pInstancingItem->getInstancingMtxArray();
		
		/* update instancingMtx buffer */
		GlMat4x4fArrayBuffer& instancingMtxBuffer = pInstancingTask->instancingBuffers().xformBuffer;
		instancingMtxBuffer.bind();
		{
			instancingMtxBuffer.bufferSubData(0, instancingMtxArray.size(), instancingMtxArray.data());
		}
		instancingMtxBuffer.unbind();
		return true;
	}
	bool updateRenderableBuffer_()
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
		if(!pInstancingTask || !pInstancingTask->isPopulated())
		{
			LOGGER_ERROR("pInstancingTask is not valid");
			return false;
		}
		const std::shared_ptr<InstancingItem> pInstancingItem = pInstancingTask->item();
		const Int32Array& renderableArray = pInstancingItem->getRenderableArray();
		
		/* update renderableArray buffer */
		GlIntArrayBuffer& renderableBuffer = pInstancingTask->instancingBuffers().renderableBuffer;
		
		renderableBuffer.bind();
		{
			renderableBuffer.bufferSubData(0, renderableArray.size(), renderableArray.data());
		}
		renderableBuffer.unbind();
	}
	bool updateColorBuffer_()
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		
		std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
		if(!pInstancingTask || !pInstancingTask->isPopulated())
		{
			LOGGER_ERROR("pInstancingTask is not valid");
			return false;
		}
		const std::shared_ptr<InstancingItem> pInstancingItem = pInstancingTask->item();
		const int                             npoints         = pInstancingItem->npoints();
		
		/* get color buffer */
		GlRGBAColorArrayBuffer& cbo = pInstancingTask->instancingBuffers().colorBuffer;
		cbo.bind();
		{
			RGBAColor* pColorArray = cbo.mapBuffer(GL_WRITE_ONLY);
			switch (colorMode_)
			{
				case mode::color::solid:
				{
					std::fill(pColorArray,
					          pColorArray  + npoints,
					          RGBAColor(parameter().color[0], parameter().color[1], parameter().color[2], parameter().transparency));
				}
				break;
				case mode::color::foreach:
				{
					for (ColorMap::iterator iter = colorMap_.begin(); iter != colorMap_.end(); ++iter)
					{
						const int & instanceId = iter->first;
						const RGBColor& color  = iter->second;
						LOGGER_DEBUG("color[%d] = (%5.2f, %5.2f, %5.2f)", instanceId, color[0], color[1], color[2]);
						const Partition& partition = pInstancingItem->partition(instanceId);
						
						RGBAColor* base = pColorArray + partition.baseInstance;
						RGBAColor* end  = base + partition.primCount;
						std::fill(base,
						          end,
						          RGBAColor(color[0],color[1], color[2], parameter().transparency));
					}
				}
				break;
				case mode::color::random:
				{
					/* not supproted */
					std::fill(pColorArray,
					          pColorArray + npoints,
					          RGBAColor(parameter().color[0], parameter().color[1], parameter().color[2], parameter().transparency));
				}
				break;
			}
			cbo.unmapBuffer();
		}
		cbo.unbind();
		return true;
	}
	bool updateWireColorBuffer_()
	{
		DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
		std::shared_ptr<InstancingTask> pInstancingTask = taskMgr_.getInstancingTask();
		if(!pInstancingTask || !pInstancingTask->isPopulated())
		{
			LOGGER_ERROR("pInstancingTask is not valid");
			return false;
		}
		
		const std::shared_ptr<InstancingItem> pInstancingItem = pInstancingTask->item();
		const int                             npoints         = pInstancingItem->npoints();
		
		/* get wire color buffer */
		GlRGBAColorArrayBuffer& wcbo = pInstancingTask->instancingBuffers().wireColorBuffer;
		
		wcbo.bind();
		RGBAColor* pWireColorArray = wcbo.mapBuffer(GL_WRITE_ONLY);
		std::fill(pWireColorArray ,
		          pWireColorArray  + npoints,
		          RGBAColor(parameter().wireColor[0], parameter().wireColor[1], parameter().wireColor[2], parameter().transparency));

		if(selectedIDs_.length() > 0)
		{
			Int32Array& pidArray = pInstancingItem->getPidArray();

			static bool useOpenMP = true;
#pragma omp parallel for if( useOpenMP )
			for(int i = 0; i < selectedIDs_.length(); ++i)
			{
				const int searchID = selectedIDs_[i];
				Int32Array::iterator iter = std::find(pidArray.begin(), pidArray.end(), searchID);
				if(iter != pidArray.end())
				{
					pWireColorArray[boost::algorithm::clamp(iter - pidArray.begin(), 0, npoints - 1)] = RGBAColor(1.0f, 1.0f, 0.0f, parameter().transparency);
				}
			}
		}
		wcbo.unmapBuffer();
		wcbo.unbind();
		return true;
	}
};

///------------------------------------------------------------ static functions
MHWRender::MPxDrawOverride* TaneMPxTransformDrawOverride::Creator(const MObject &obj)
{
	return new TaneMPxTransformDrawOverride(obj);
}

///------------------------------------------------------------ constructor / destructor
TaneMPxTransformDrawOverride::TaneMPxTransformDrawOverride(const MObject &obj)
	: MHWRender::MPxDrawOverride(obj, DrawOverride::DrawCallback, true /*isAlwaysDirty*/)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
	MDagPath shapePath = getShapePath(obj);
	
	MFnDagNode shapeFn(shapePath.node());
	TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode());
	if(!pShape)
	{
		LOGGER_ERROR("Failed to get pShape");
		return;
	}
	DrawOverride::Register(getShapePath(obj), std::make_shared<TaneTransformDrawOverride>(pShape->instancingItem()));
}

TaneMPxTransformDrawOverride::~TaneMPxTransformDrawOverride()
{
}

///------------------------------------------------------------ virtual functions
MHWRender::DrawAPI TaneMPxTransformDrawOverride::supportedDrawAPIs() const
{
	return (MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile);
}
MUserData* TaneMPxTransformDrawOverride::prepareForDraw(const MDagPath &objPath, const MDagPath &cameraPath, const MHWRender::MFrameContext &frameContext, MUserData* oldData)
{
	DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
	MDagPath shapePath = getShapePath(objPath.node());
	
	LOGGER_DEBUG("shapePath = %s", shapePath.fullPathName().asChar());
	const std::shared_ptr<DrawOverride>& pDrawOverride = DrawOverride::GetDrawOverride(shapePath);
	if(!pDrawOverride)
	{
		LOGGER_ERROR("Failed to get pDrawOverride");
		return nullptr;
	}
	return pDrawOverride->prepareForDraw(shapePath, cameraPath, frameContext, oldData);
}
bool TaneMPxTransformDrawOverride::isTransparent() const
{
	return false;
}
bool TaneMPxTransformDrawOverride::isBounded(const MDagPath &objPath, const MDagPath &cameraPath) const
{
	return true;
}
MBoundingBox TaneMPxTransformDrawOverride::boundingBox(const MDagPath &objPath, const MDagPath &cameraPath) const
{
	DECL_LOGGER(RegisteredNodes->TaneMPxTransformDrawOverride.logging);
	
	MStatus stat;
	MDagPath   shapePath = getShapePath(objPath.node());
	MFnDagNode shapeFn(shapePath.node(&stat));
	TaneMPxShape* pShape = dynamic_cast<TaneMPxShape*>(shapeFn.userNode(&stat));
	if(!pShape)
	{
		LOGGER_ERROR("Failed to get pShape");
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
	
	//const std::shared_ptr<DrawOverride>& pDrawOverride = DrawOverride::GetDrawOverride(shapePath);
	//if(!pDrawOverride)
	//{
	//	LOGGER_ERROR("Failed to get pDrawOverride");
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
//MMatrix TaneMPxTransformDrawOverride::transform(const MDagPath &objPath, const MDagPath &cameraPath) const
//{
//	DECL_LOGGER(log::TaneMPxTransformDrawOverride);
//	MMatrix mtx = MMatrix::identity;
//	mtx[3][1] = 10.0;
//
//	return mtx;
//	//return MPxDrawOverride::transform(objPath, cameraPath);
//}

///------------------------------------------------------------ private helper functions
MObject TaneMPxTransformDrawOverride::getShape(const MObject& obj) const
{
	MDagPath objPath = GetDagPath(obj);
	objPath.extendToShape();
	return objPath.node();
}
MDagPath  TaneMPxTransformDrawOverride::getShapePath(const MObject& obj) const
{
	MDagPath objPath = GetDagPath(obj);
	objPath.extendToShape();
	return objPath;
}

NAMESPACE_BASE_END

