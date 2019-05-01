#pragma once
#include "baseGl/pch.h"

#include "base/renderItem.h"
#include "base/instancingItem.h"

#include "baseGl/glShader.h"
#include "baseGl/glBuffer.h"
#include "baseGl/instancingTask.h"

NAMESPACE_BASE_BEGIN
/* ----------------------------------------------------- RenderTask Base */
class RenderTask
{
protected:
	bool isPopulated_;
	
	int                             instanceID_;
	std::shared_ptr<InstancingTask> pInstancingTask_;   /* used for instance render */
	int                             primCount_;
	int                             baseInstance_;
	
	/* icr data */
	GLuint              xfdID_;
	GlVertexArrayObject icrvao_;
	GLuint           queryIDs_[LOD::maxLevel];
	int              visibles_[LOD::maxLevel];
	GlIcrArrayBuffer icrBuffer_[LOD::maxLevel];


public:
	static const std::shared_ptr<RenderTask> Create(const std::shared_ptr<RenderItem> pRenderItem,
	                                                const size_t instanceId,
	                                                const std::shared_ptr<InstancingTask> pInstancingTask);


protected:
	RenderTask(const int instanceID, const std::shared_ptr<InstancingTask> pInstancingTask)
		: isPopulated_(false),
		  instanceID_(instanceID),
		  pInstancingTask_(pInstancingTask),
		  xfdID_(0xffffffff),
		  queryIDs_({0xffffffff, 0xffffffff, 0xffffffff}),
		  visibles_({0, 0, 0})
	{
		const std::shared_ptr<InstancingItem> pItem     = pInstancingTask_->item();
		const Partition                       partition = pItem->partition(instanceID_);
		primCount_    = partition.primCount;
		baseInstance_ = partition.baseInstance;
	}
	virtual ~RenderTask()
	{
	};

public:
	virtual const RenderItem::Type itemType() const = 0;
	virtual const std::shared_ptr<RenderItem> item() const = 0;

public:
	virtual void populate()
	{
		DECL_LOGGER(base::RenderTask);
		if(!pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("pInstancingTask is not populated");
			return;
		}
		
		const int npoints = pInstancingTask_->item()->npoints();
		GlIntArrayBuffer      & rbo  = pInstancingTask_->instancingBuffers().renderableBuffer;
		GlMat4x4fArrayBuffer  & xbo  = pInstancingTask_->instancingBuffers().xformBuffer;
		GlRGBAColorArrayBuffer& cbo  = pInstancingTask_->instancingBuffers().colorBuffer;
		GlRGBAColorArrayBuffer& wcbo = pInstancingTask_->instancingBuffers().wireColorBuffer;
		
		/* set icr vertex array object */
		icrvao_.generate();
		icrvao_.bind();
		{
			rbo.bind();
			rbo.enableVertexAttribIPointer(0);
			xbo.bind();
			xbo.enableVertexAttribPointer(1);
			cbo.bind();
			cbo.enableVertexAttribPointer(5);
			wcbo.bind();
			wcbo.enableVertexAttribPointer(6);
		}
		icrvao_.unbind();
		
		/* set transform feedback buffer for icr */
		glGenTransformFeedbacks(1, &xfdID_);
		
		/* set icr buffer */
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, xfdID_);
		for (int lod = 0; lod < LOD::maxLevel; ++lod)
		{
			icrBuffer_[lod].generate();
			icrBuffer_[lod].bind();
			icrBuffer_[lod].bufferData(npoints, nullptr, GL_DYNAMIC_COPY);
			icrBuffer_[lod].bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, lod /*output stream index */);
			icrBuffer_[lod].unbind();
		}
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		
		/* create querys */
		glGenQueries(LOD::maxLevel, queryIDs_);
	}
	virtual void update()
	{
		DECL_LOGGER(base::RenderTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("RenderTask is not populated");
			return;
		}
	}
	
	virtual void drawICR(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::RenderTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("RenderTask is not populated");
			return;
		}
		
		const std::shared_ptr<GlIcrShader> pIcrShader = GlIcrShader::Get();
		LOGGER_IN("drawIcr");
		{
			if(!shaderOverrided)
			{
				pIcrShader->use();
				
				LOGGER_DEBUG("baseInstance = %d", baseInstance_);
				LOGGER_DEBUG("primCount    = %d", primCount_);
				
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, xfdID_);
				GLCHECKERRORNORET("Failed to bind transformfeedback, %u", xfdID_);
				glEnable(GL_RASTERIZER_DISCARD);
				GLCHECKERRORNORET("Failed to enable GL_RASTERIZER_DISCARD");
				{
					icrvao_.bind();
					{
						for (int i = 0; i < LOD::maxLevel; i++)
						{
							glBeginQueryIndexed(GL_PRIMITIVES_GENERATED, i, queryIDs_[i]);
							GLCHECKERRORNORET("Failed to begin query %d", i);
						}
						
						glBeginTransformFeedback(GL_POINTS);
						GLCHECKERRORNORET("Failed to begin transform feedback");
						glDrawArrays(GL_POINTS, baseInstance_, primCount_);
						GLCHECKERRORNORET("Failed to draw array");
						glEndTransformFeedback();
						GLCHECKERRORNORET("Failed to end transform feedback");
						
						
						for (int i = 0; i < LOD::maxLevel; i++)
						{
							glEndQueryIndexed(GL_PRIMITIVES_GENERATED, i);
							GLCHECKERRORNORET("Failed to end query %d", i);
						}
					}
					icrvao_.unbind();
				}
				glDisable(GL_RASTERIZER_DISCARD);
				GLCHECKERRORNORET("Failed to disable GL_RASTERIZER_DISCARD");
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
				GLCHECKERRORNORET("Failed to unbind transform feedback");
				pIcrShader->unuse();
			}
			
			
		}
		LOGGER_OUT();
	}
	virtual void drawShaded(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::RenderTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("RenderTask is not populated");
			return;
		}
	}
	virtual void drawWire(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::RenderTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("RenderTask is not populated");
			return;
		}
	}
	
	virtual void depopulate()
	{
		DECL_LOGGER(base::RenderTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("RenderTask is not populated");
			return;
		}
		
		for (int lod = 0; lod < LOD::maxLevel; ++lod)
		{
			visibles_[lod] = 0;
			icrBuffer_[lod].clear();
		}
		icrvao_.clear();
		
		glDeleteTransformFeedbacks(1, &xfdID_);
		GLCHECKERRORNORET("Failed to delete transform feedback");
		glDeleteQueries(LOD::maxLevel, queryIDs_);
		GLCHECKERRORNORET("Failed to delete queries");
	}
public:
	bool isPopulated() const
	{
		return isPopulated_;
	}
	bool valid()
	{
		if(!pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("pInstancingTask is not populated");
			return false;
		}
		return true;
	}
	
};

/* ----------------------------------------------------- RenderItem Proxy */
template<typename T>
class RenderTaskProxy : public RenderTask
{
public:
	typedef RenderTask Base;
	typedef T          RenderItemT;

protected:
	const std::shared_ptr<RenderItemT> pRenderItem_;

protected:
	RenderTaskProxy(const std::shared_ptr<RenderItem> pItem,
	                const int32_t instanceID,
	                const std::shared_ptr<InstancingTask> pInstancingTask
	)
		: RenderTask(instanceID, pInstancingTask),
		  pRenderItem_(std::dynamic_pointer_cast<RenderItemT>(pItem))
	{
	}
	virtual ~RenderTaskProxy()
	{
	};
public:
	virtual const RenderItem::Type itemType() const
	{
		return pRenderItem_->type();
	}
	virtual const std::shared_ptr<RenderItem> item() const
	{
		return pRenderItem_;
	}
	
};
NAMESPACE_BASE_END

/* ----------------------------------------------------- BBoxTask */
NAMESPACE_BASE_BEGIN
class BBoxTask : public RenderTaskProxy<BBoxItem>
{
public:
	typedef RenderTaskProxy<BBoxItem>   Base;
	typedef typename Base::RenderItemT  RenderItemT;
	typedef typename RenderItemT::ItemT ItemT;
private:
	GlVertexArrayObject     vao_[LOD::maxLevel];
	GlPoint3fArrayBuffer    vbo_;
	GlNormal3fArrayBuffer   nbo_;
	GlTexCoord3fArrayBuffer tbo_;
	
	Point3fArray    pointArray_;
	Normal3fArray   normalArray_;
	TexCoord3fArray uvArray_;
	Int32Array      indexArray_;

public:
	BBoxTask(const std::shared_ptr<RenderItem> pItem,
	         const size_t instanceId,
	         const std::shared_ptr<InstancingTask> pInstancingTask)
		: Base(pItem, instanceId, pInstancingTask)
	{
	}
	virtual ~BBoxTask()
	{
	}
public:
	virtual void populate()
	{
		DECL_LOGGER(base::BBoxTask);
		if(!valid())
		{
			STDOUT_ERROR("Failed to get valid BoxTask");
			return;
		}
		
		/* create icr buffers */
		Base::populate();
		
		/* create geometry buffers */
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		const Point3f& lower = pItem->lower();
		const Point3f& upper = pItem->upper();
		LOGGER_DEBUG("lower = (%5.2f, %5.2f, %5.2f)", lower[0], lower[1], lower[2]);
		LOGGER_DEBUG("upper = (%5.2f, %5.2f, %5.2f)", upper[0], upper[1], upper[2]);
		
		BuildExpandedBBoxGeom(*pItem,
		                      &pointArray_,
		                      &normalArray_,
		                      &uvArray_,
		                      &indexArray_
		);
		
		vbo_.generate();
		vbo_.bind();
		vbo_.bufferData(pointArray_.size(), pointArray_.data(), GL_STATIC_DRAW);
		vbo_.unbind();
		
		nbo_.generate();
		nbo_.bind();
		nbo_.bufferData(normalArray_.size(), normalArray_.data(), GL_STATIC_DRAW);
		nbo_.unbind();
		
		tbo_.generate();
		tbo_.bind();
		tbo_.bufferData(uvArray_.size(), uvArray_.data(), GL_STATIC_DRAW);
		tbo_.unbind();
		
		
		/* set geometry lod vao */
		for (int lod = 0; lod < LOD::maxLevel; ++lod)
		{
			vao_[lod].generate();
			vao_[lod].bind();
			{
				vbo_.bind();
				vbo_.enableVertexAttribPointer(0);
				nbo_.bind();
				nbo_.enableVertexAttribPointer(1);
				tbo_.bind();
				tbo_.enableVertexAttribPointer(2);
				
				/* connect icr buffer to geometry vao */
				icrBuffer_[lod].bind();
				icrBuffer_[lod].enableVertexAttribPointer(3, 1);
			}
			vao_[lod].unbind();
		}
		
		isPopulated_ = true;
		
	}
	virtual void update()
	{
		DECL_LOGGER(base::BBoxTask);
		
		if(!isPopulated_)
			return;
		
		const std::shared_ptr<ItemT> pItem = pRenderItem_->item();
		const Point3f& lower = pItem->lower();
		const Point3f& upper = pItem->upper();
		
		LOGGER_DEBUG("lower = (%5.2f, %5.2f, %5.2f)", lower[0], lower[1], lower[2]);
		LOGGER_DEBUG("upper = (%5.2f, %5.2f, %5.2f)", upper[0], upper[1], upper[2]);
		
		/// ------------------------------ make buffer data
		//BuildExpandedBBoxGeom(lower, upper, &pointArray_, &normalArray_, &uvArray_);
		vbo_.bind();
		vbo_.bufferSubData(0, pointArray_.size(), pointArray_.data());
		vbo_.unbind();
		
		nbo_.bind();
		nbo_.bufferSubData(0, normalArray_.size(), normalArray_.data());
		nbo_.unbind();
		
		tbo_.bind();
		tbo_.bufferSubData(0, uvArray_.size(), uvArray_.data());
		tbo_.unbind();
	}
	virtual void drawShaded(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::BBoxTask);
		
		if(!isPopulated_)
		{
			STDOUT_ERROR("task is not populated");
			return;
		}
		
		const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		LOGGER_IN("draw");
		{
			
			static const RGBAColor mask0(1.0f, 0.0f, 0.0, 1.0f);
			static const RGBAColor mask1(0.0f, 1.0f, 0.0, 1.0f);
			static const RGBAColor mask2(0.0f, 0.0f, 1.0, 1.0f);
			
			for (int lod = 0; lod < LOD::maxLevel; ++lod)
			{
				glGetQueryObjectiv(queryIDs_[lod], GL_QUERY_RESULT, &visibles_[lod]);
				GLCHECKERRORNORET("Failed to get query[%d]", lod);
				LOGGER_DEBUG("visible_[%d] = %d", lod, visibles_[lod]);
				
				if(!shaderOverrided)
				{
					pShader->use();
					switch (lod)
					{
						case 0:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask0);
							break;
						case 1:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask1);
							break;
						case 2:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask2);
							break;
					}
					pShader->setUniform(GlIcrPhongShader::kDrawMode, 0);
				}
				
				if(visibles_[lod] > 0)
				{
					vao_[lod].bind();
					glDrawElementsInstancedBaseInstance(GL_TRIANGLES,       //mode
					                                    indexArray_.size(), //count
					                                    GL_UNSIGNED_INT,    //type
					                                    indexArray_.data(), //indices
					                                    visibles_[lod],     //primcount
					                                    0                   //baseinstance
					);
					
					vao_[lod].unbind();
				}
				if(!shaderOverrided)
					pShader->unuse();
			}
		}
		LOGGER_OUT();
	}
	virtual void drawWire(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::BBoxTask);
		
		if(!isPopulated_)
		{
			STDOUT_ERROR("task is not populated");
			return;
		}
		
		const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
		LOGGER_IN("draw");
		{
			static const RGBAColor mask0(1.0f, 0.0f, 0.0, 1.0f);
			static const RGBAColor mask1(0.0f, 1.0f, 0.0, 1.0f);
			static const RGBAColor mask2(0.0f, 0.0f, 1.0, 1.0f);
			
			for (int lod = 0; lod < LOD::maxLevel; ++lod)
			{
				glGetQueryObjectiv(queryIDs_[lod], GL_QUERY_RESULT, &visibles_[lod]);
				GLCHECKERRORNORET("Failed to get query[%d]", lod);
				LOGGER_DEBUG("visible_[%d] = %d", lod, visibles_[lod]);
				
				if(!shaderOverrided)
				{
					pShader->use();
					pShader->setUniform(GlIcrPhongShader::kDrawMode, 1);
				}
				
				if(visibles_[lod] > 0)
				{
					vao_[lod].bind();
					glDrawElementsInstancedBaseInstance(GL_LINE_LOOP,        //mode
					                                    indexArray_.size(),  //count
					                                    GL_UNSIGNED_INT,     //type
					                                    indexArray_.data(),  //indices
					                                    visibles_[lod],      //primcount
					                                    0                    //baseinstance
					);
					
					vao_[lod].unbind();
				}
				if(!shaderOverrided)
					pShader->unuse();
			}
		}
		LOGGER_OUT();
	}
	virtual void depopulate()
	{
		DECL_LOGGER(base::BBoxTask);
		if(!isPopulated_)
			return;
		
		vbo_.clear();
		nbo_.clear();
		tbo_.clear();
		for (int i = 0; i < LOD::maxLevel; ++i)
		{
			vao_[i].clear();
		}
		
		isPopulated_ = false;
	}

private:
};
NAMESPACE_BASE_END


/* ----------------------------------------------------- ExpandedGeomTask */
NAMESPACE_BASE_BEGIN
class ExpandedGeomTask : public RenderTaskProxy<ExpandedGeomItem>
{
public:
	typedef RenderTaskProxy<ExpandedGeomItem> Base;
	typedef typename Base::RenderItemT        RenderItemT;
	typedef typename RenderItemT::ItemT       ItemT;
private:
	GlVertexArrayObject     vao_[LOD::maxLevel];
	GlPoint3fArrayBuffer    vbo_;
	GlNormal3fArrayBuffer   nbo_;
	GlTexCoord3fArrayBuffer tbo_;

public:
	ExpandedGeomTask(const std::shared_ptr<RenderItem> pItem,
	                 const size_t instanceId,
	                 const std::shared_ptr<InstancingTask> pInstancingTask)
		: Base(pItem, instanceId, pInstancingTask)
	{
	}
	virtual ~ExpandedGeomTask()
	{
	}
public:
	virtual void populate()
	{
		DECL_LOGGER(base::ExpandedGeomTask);
		if(!pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("pInstancingTask is not populated");
			return;
		}
		/* create icr buffers */
		Base::populate();
		
		/* create geometry buffers */
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		const Point3fArray   & pointArray  = pItem->pointArray;
		const Normal3fArray  & normalArray = pItem->normalArray;
		const TexCoord3fArray& uvArray     = pItem->uvArray;
		
		vbo_.generate();
		vbo_.bind();
		vbo_.bufferData(pointArray.size(), pointArray.data(), GL_STATIC_DRAW);
		vbo_.unbind();
		
		nbo_.generate();
		nbo_.bind();
		nbo_.bufferData(normalArray.size(), normalArray.data(), GL_STATIC_DRAW);
		nbo_.unbind();
		
		tbo_.generate();
		tbo_.bind();
		tbo_.bufferData(uvArray.size(), uvArray.data(), GL_STATIC_DRAW);
		tbo_.unbind();
		
		/* set geometry vao */
		for (int lod = 0; lod < LOD::maxLevel; ++lod)
		{
			vao_[lod].generate();
			vao_[lod].bind();
			{
				vbo_.bind();
				vbo_.enableVertexAttribPointer(0);
				nbo_.bind();
				nbo_.enableVertexAttribPointer(1);
				tbo_.bind();
				tbo_.enableVertexAttribPointer(2);
				
				/* connect icr buffer to geometry vao */
				icrBuffer_[lod].bind();
				icrBuffer_[lod].enableVertexAttribPointer(3, 1);
			}
			vao_[lod].unbind();
		}
		
		isPopulated_ = true;
	}
	virtual void update()
	{
		DECL_LOGGER(base::ExpandedGeomTask);
		
		if(!isPopulated())
		{
			STDOUT_ERROR("render task is not poluated");
			return;
		}
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		const Point3fArray   & pointArray  = pRenderItem_->item()->pointArray;
		const Normal3fArray  & normalArray = pRenderItem_->item()->normalArray;
		const TexCoord3fArray& uvArray     = pRenderItem_->item()->uvArray;
		
		/* update geometry buffer */
		vbo_.bind();
		vbo_.bufferSubData(0, pointArray.size(), pointArray.data());
		vbo_.unbind();
		
		nbo_.bind();
		nbo_.bufferSubData(0, normalArray.size(), normalArray.data());
		nbo_.unbind();
		
		tbo_.bind();
		tbo_.bufferSubData(0, uvArray.size(), uvArray.data());
		tbo_.unbind();
	}
	
	virtual void drawShaded(const bool shaderOverrided, const bool iterated)
	{
		DECL_LOGGER(base::ExpandedGeomTask);
		
		if(!isPopulated_)
		{
			STDOUT_ERROR("task is not populated");
			return;
		}
		
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
		LOGGER_IN("draw");
		{
			static const RGBAColor mask0(1.0f, 0.0f, 0.0, 1.0f);
			static const RGBAColor mask1(0.0f, 1.0f, 0.0, 1.0f);
			static const RGBAColor mask2(0.0f, 0.0f, 1.0, 1.0f);
			
			for (int lod = 0; lod < LOD::maxLevel; ++lod)
			{
				glGetQueryObjectiv(queryIDs_[lod], GL_QUERY_RESULT, &visibles_[lod]);
				GLCHECKERRORNORET("Failed to get query[%d]", lod);
				LOGGER_DEBUG("visible_[%d] = %d", lod, visibles_[lod]);
				
				if(!shaderOverrided)
				{
					pShader->use();
					switch (lod)
					{
						case 0:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask0);
							break;
						case 1:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask1);
							break;
						case 2:
							pShader->setUniform(GlIcrPhongShader::kMaskColor, mask2);
							break;
					}
					pShader->setUniform(GlIcrPhongShader::kDrawMode, 0);
				}
				
				if(visibles_[lod] > 0)
				{
					const Int32Array& indexArray = pItem->indexArray;
					const GLint   indexCount  = pItem->drawCmd[lod].indexCount;
					const GLsizei indexOffset = pItem->drawCmd[lod].indexOffset;
					
					LOGGER_DEBUG("indexCount = %d, indexOffset = %d", indexCount, indexOffset);

					vao_[lod].bind();
					glDrawElementsInstancedBaseInstance(GL_TRIANGLES,        //mode
					                                    indexCount,          //count
					                                    GL_UNSIGNED_INT,     //type
					                                    &indexArray[indexOffset],
					                                    visibles_[lod],      //primcount
					                                    0                    //baseinstance
					);
					
					vao_[lod].unbind();
				}
				if(!shaderOverrided)
					pShader->unuse();
			}
		}
		LOGGER_OUT();
	}
	virtual void drawWire(const bool shaderOverrided, const bool iterated = false)
	{
		DECL_LOGGER(base::ExpandedGeomTask);
		
		if(!isPopulated_)
		{
			STDOUT_ERROR("task is not populated");
			return;
		}
		
		const std::shared_ptr<ItemT>& pItem = pRenderItem_->item();
		const std::shared_ptr<GlIcrPhongShader> pShader = GlIcrPhongShader::Get();
		LOGGER_IN("draw");
		{
			for (int lod = 0; lod < LOD::maxLevel; ++lod)
			{
				glGetQueryObjectiv(queryIDs_[lod], GL_QUERY_RESULT, &visibles_[lod]);
				GLCHECKERRORNORET("Failed to get query[%d]", lod);
				LOGGER_DEBUG("visible_[%d] = %d", lod, visibles_[lod]);
				
				if(!shaderOverrided)
				{
					pShader->use();
					pShader->setUniform(GlIcrPhongShader::kDrawMode, 1);
				}
				
				if(visibles_[lod] > 0)
				{
					const Int32Array& indexArray = pItem->indexArray;
					const GLint   indexCount  = pItem->drawCmd[lod].indexCount;
					const GLsizei indexOffset = pItem->drawCmd[lod].indexOffset;
					
					LOGGER_DEBUG("indexCount = %d, indexOffset = %d", indexCount, indexOffset);
					vao_[lod].bind();
					glDrawElementsInstancedBaseInstance(GL_LINE_LOOP,        //mode
					                                    indexCount,          //count
					                                    GL_UNSIGNED_INT,     //type
					                                    &indexArray[indexOffset],
					                                    visibles_[lod],      //primcount
					                                    0                    //baseinstance
					);
					
					vao_[lod].unbind();
				}
				if(!shaderOverrided)
					pShader->unuse();
			}
		}
		LOGGER_OUT();
	}
	virtual void depopulate()
	{
		DECL_LOGGER(base::ExpandedGeomTask);
		if(!isPopulated())
		{
			STDOUT_ERROR("render task is not poluated");
			return;
		}
		
		Base::depopulate();
		
		vbo_.clear();
		nbo_.clear();
		tbo_.clear();
		
		for (int lod = 0; lod < LOD::maxLevel; ++lod)
		{
			vao_[lod].clear();
		}
		
		isPopulated_ = false;
	}

private:
};
NAMESPACE_BASE_END


///* ----------------------------------------------------- IndexedGeomTask */
//NAMESPACE_BASE_BEGIN
//class IndexedGeomTask : public RenderTaskProxy<IndexedGeomItem>
//{
//public:
//	typedef RenderTaskProxy<IndexedGeomItem> Base;
//	typedef typename Base::RenderItemT       RenderItemT;
//	typedef typename RenderItemT::ItemT      ItemT;
//private:
//	gl::GlVertexArrayObject     vao_[LOD::maxLevel];
//	gl::GlPoint3fArrayBuffer    vbo_;
//	gl::GlNormal3fArrayBuffer   nbo_;
//	gl::GlTexCoord2fArrayBuffer tbo_;
//
//public:
//	IndexedGeomTask(const std::shared_ptr<RenderItem> pItem,
//	                const size_t instanceId,
//	                const std::shared_ptr<InstancingTask> pInstancingTask)
//		: Base(pItem, instanceId, pInstancingTask)
//	{
//	}
//	virtual ~IndexedGeomTask()
//	{
//	}
//public:
//	virtual void populate()
//	{
//		DECL_LOGGER(log::IndexedGeomTask);
//		if(!pInstancingTask_->isPopulated())
//		{
//			STDOUT_ERROR("pInstancingTask is not populated");
//			return;
//		}
//		/* create icr buffers */
//		Base::populate();
//
//		/* create geometry buffers */
//		//LOGGER_DEBUG("minTime  = %5.2f", pRenderItem_->minTime());
//		//LOGGER_DEBUG("maxTime  = %5.2f", pRenderItem_->maxTime());
//		//LOGGER_DEBUG("nsamples = %u",    pRenderItem_->nsamples());
//
//		const Point3fArray   & pointArray    = pRenderItem_->item()->pointArray;
//		const Int32Array     & pointIndices  = pRenderItem_->item()->pointIndices;
//		const Normal3fArray  & normalArray   = pRenderItem_->item()->normalArray;
//		const Int32Array     & normalIndices = pRenderItem_->item()->pointIndices;
//		const TexCoord2fArray& uvArray       = pRenderItem_->item()->uvArray;
//		const Int32Array     & uvIndices     = pRenderItem_->item()->pointIndices;
//
//		vbo_.generate();
//		vbo_.bind();
//		vbo_.bufferData(pointArray.size(), pointArray.data(), GL_STATIC_DRAW);
//		vbo_.unbind();
//
//		nbo_.generate();
//		nbo_.bind();
//		nbo_.bufferData(normalArray.size(), normalArray.data(), GL_STATIC_DRAW);
//		nbo_.unbind();
//
//		tbo_.generate();
//		tbo_.bind();
//		tbo_.bufferData(uvArray.size(), uvArray.data(), GL_STATIC_DRAW);
//		tbo_.unbind();
//
//		/* set geometry vao */
//		for (int lod = 0; lod < levelOfDetail::maxLevel; ++lod)
//		{
//			vao_[lod].generate();
//			vao_[lod].bind();
//			{
//				vbo_.bind();
//				vbo_.enableVertexAttribPointer(0);
//				nbo_.bind();
//				nbo_.enableVertexAttribPointer(1);
//				tbo_.bind();
//				tbo_.enableVertexAttribPointer(2);
//
//				/* connect icr buffer to geometry vao */
//				icrBuffer_[lod].bind();
//				icrBuffer_[lod].enableVertexAttribPointer(3, 1);
//			}
//			vao_[lod].unbind();
//		}
//
//		isPopulated_ = true;
//	}
//	virtual void update()
//	{
//		DECL_LOGGER(log::IndexedGeomTask);
//
//		if(!isPopulated())
//		{
//			STDOUT_ERROR("render task is not poluated");
//			return;
//		}
//
//		const Point3fArray   & pointArray  = pRenderItem_->item()->pointArray;
//		const Normal3fArray  & normalArray = pRenderItem_->item()->normalArray;
//		const TexCoord2fArray& uvArray     = pRenderItem_->item()->uvArray;
//
//		/* update geometry buffer */
//		vbo_.bind();
//		vbo_.bufferSubData(0, pointArray.size(), pointArray.data());
//		vbo_.unbind();
//
//		nbo_.bind();
//		nbo_.bufferSubData(0, normalArray.size(), normalArray.data());
//		nbo_.unbind();
//
//		tbo_.bind();
//		tbo_.bufferSubData(0, uvArray.size(), uvArray.data());
//		tbo_.unbind();
//	}
//
//	virtual void drawShaded(const bool shaderOverrided, const bool iterated)
//	{
//		DECL_LOGGER(log::IndexedGeomTask);
//		if(!isPopulated())
//		{
//			STDOUT_ERROR("render task is not poluated");
//			return;
//		}
//
//		const DrawElementCmd& drawCmd = pRenderItem_->drawElementCmd();
//		const std::shared_ptr<GlIcrPhongShader> pIcrPhongShader = GlIcrPhongShader::Get();
//
//		LOGGER_IN("draw");
//		{
//			for (int lod = 0; lod < levelOfDetail::maxLevel; ++lod)
//			{
//				glGetQueryObjectiv(queryIDs_[lod], GL_QUERY_RESULT, &visibles_[lod]);
//				LOGGER_DEBUG("visible_[%d] = %d", lod, visibles_[lod]);
//				if(!shaderOverrided)
//				{
//					pIcrPhongShader->use();
//					switch (lod)
//					{
//						case 0:
//							pIcrPhongShader->setMaskColor(1.0f, 0.0f, 0.0f);
//							break;
//						case 1:
//							pIcrPhongShader->setMaskColor(0.0f, 1.0f, 0.0f);
//							break;
//						case 2:
//							pIcrPhongShader->setMaskColor(0.0f, 0.0f, 1.0f);
//							break;
//					}
//				}
//
//				if(visibles_[lod] > 0)
//				{
//					const Int32Array& pointIndices = pRenderItem_->item()->pointIndices;
//					const GLint   indexCount  = pRenderItem_->drawElementCmd()[lod].indexCount;
//					const GLsizei indexOffset = pRenderItem_->drawElementCmd()[lod].indexOffset;
//
//					LOGGER_DEBUG("indexCount = %d, indexOffset = %d", indexCount, indexOffset);
//
//					LOGGER_IN("glDrawElements");
//					vao_[lod].bind();
//					//glDrawElementsInstancedBaseInstance(GL_TRIANGLES,        //mode
//					//                                    indexCount,          //count
//					//                                    GL_UNSIGNED_INT,     //type
//					//									&pointIndices[indexOffset],
//					//						            visibles_[lod],      //primcount
//					//                                    0                    //baseinstance
//					//);
//					glDrawElementsInstancedBaseInstance(GL_POLYGON,          //mode
//					                                    indexCount,          //count
//					                                    GL_UNSIGNED_INT,     //type
//					                                    &pointIndices[indexOffset],
//					                                    visibles_[lod],      //primcount
//					                                    0                    //baseinstance
//					);
//
//					vao_[lod].unbind();
//					LOGGER_OUT();
//				}
//				if(!shaderOverrided)
//					pIcrPhongShader->unuse();
//			}
//		}
//		LOGGER_OUT();
//
//	}
//	virtual void drawWire(const bool shaderOverrided, const bool iterated = false)
//	{
//		DECL_LOGGER(log::IndexedGeomTask);
//		if(!isPopulated())
//		{
//			STDOUT_ERROR("render task is not poluated");
//			return;
//		}
//	}
//	virtual void depopulate()
//	{
//		DECL_LOGGER(log::IndexedGeomTask);
//		if(!isPopulated())
//		{
//			STDOUT_ERROR("render task is not poluated");
//			return;
//		}
//
//		Base::depopulate();
//
//		vbo_.clear();
//		nbo_.clear();
//		tbo_.clear();
//
//		for (int lod = 0; lod < LOD::maxLevel; ++lod)
//		{
//			vao_[lod].clear();
//		}
//
//		isPopulated_ = false;
//	}
//
//private:
//};
//NAMESPACE_BASE_END


