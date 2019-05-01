#pragma once
#include "baseGl/pch.h"

#include "base/frame.h"
#include "base/instancingItem.h"

#include "baseGl/glutil.h"
#include "baseGl/glBuffer.h"

NAMESPACE_BASE_BEGIN
struct InstancingBuffers
{
	GlIntArrayBuffer       renderableBuffer;
	GlMat4x4fArrayBuffer   xformBuffer;
	GlRGBAColorArrayBuffer colorBuffer;
	GlRGBAColorArrayBuffer wireColorBuffer;
};

class InstancingTask final
{
private:
	InstancingBuffers                       instancingBuffers_;
	bool                                    isPopulated_;
	const std::shared_ptr<InstancingItem>   pItem_;

public:
	static const std::shared_ptr<InstancingTask> Create (const std::shared_ptr<InstancingItem> pItem)
	{
		return std::make_shared<InstancingTask>(pItem);
	}

public:
	InstancingTask(const std::shared_ptr<InstancingItem> pItem)
		: isPopulated_(false), pItem_(pItem)
	{
	}
	~InstancingTask()
	{
	}
public:
	const std::shared_ptr<InstancingItem> item() const
	{
		return pItem_;
	}

public:
	const InstancingBuffers& instancingBuffers() const
	{
		return instancingBuffers_;
	}
	InstancingBuffers& instancingBuffers()
	{
		return instancingBuffers_;
	}

public:
	void populate()
	{
		DECL_LOGGER(base::InstancingTask);
		
		if(isPopulated_)
		{
			STDOUT_ERROR("Failed to populate instancing task");
			return;
		}
		
		const int npoints = pItem_->npoints();
		const Int32Array     & pidArray           = pItem_->getPidArray();
		const Int32Array     & sidArray           = pItem_->getSidArray();
		const Int32Array     & renderableArray    = pItem_->getRenderableArray();
		const Mat4x4fArray   & instancingMtxArray = pItem_->getInstancingMtxArray();
		const TexCoord3fArray& uvArray            = pItem_->getUVArray();
		const FloatArray     & radiusArray        = pItem_->getRadiusArray();
		
		GlIntArrayBuffer      & rbo   = instancingBuffers_.renderableBuffer;
		GlMat4x4fArrayBuffer  & xbo   = instancingBuffers_.xformBuffer;
		GlRGBAColorArrayBuffer& cbo  = instancingBuffers_.colorBuffer;
		GlRGBAColorArrayBuffer& wcbo = instancingBuffers_.wireColorBuffer;
		
		rbo.generate();
		rbo.bind();
		rbo.bufferData(npoints, renderableArray.data(), GL_STATIC_DRAW);
		rbo.unbind();
		
		xbo.generate();
		xbo.bind();
		xbo.bufferData(npoints, instancingMtxArray.data(), GL_STATIC_DRAW);
		xbo.unbind();
		
		cbo.generate();
		cbo.bind();
		cbo.bufferData(npoints, nullptr, GL_STATIC_DRAW);
		cbo.unbind();

		wcbo.generate();
		wcbo.bind();
		wcbo.bufferData(npoints, nullptr, GL_STATIC_DRAW);
		wcbo.unbind();
		
		isPopulated_ = true;
	}
	void update()
	{
		DECL_LOGGER(base::InstancingTask);
		
		if(!isPopulated_)
			return;
		
		const Int32Array     & pidArray           = pItem_->getPidArray();
		const Int32Array     & sidArray           = pItem_->getSidArray();
		const Int32Array     & renderableArray    = pItem_->getRenderableArray();
		const Mat4x4fArray   & instancingMtxArray = pItem_->getInstancingMtxArray();
		const TexCoord3fArray& uvArray            = pItem_->getUVArray();
		const FloatArray     & radiusArray        = pItem_->getRadiusArray();
		
	}
	void depopulate()
	{
		DECL_LOGGER(base::InstancingTask);
		
		if(!isPopulated_)
			return;
		
		GlIntArrayBuffer      & rbo  = instancingBuffers_.renderableBuffer;
		GlMat4x4fArrayBuffer  & xbo  = instancingBuffers_.xformBuffer;
		GlRGBAColorArrayBuffer& cbo  = instancingBuffers_.colorBuffer;
		GlRGBAColorArrayBuffer& wcbo = instancingBuffers_.wireColorBuffer;
		
		rbo.clear();
		xbo.clear();
		cbo.clear();
		wcbo.clear();
		
		isPopulated_ = false;
	}
public:
	bool isPopulated() const
	{
		return isPopulated_;
	}
};

NAMESPACE_BASE_END