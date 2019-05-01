#include "baseGl/renderTask.h"

/* ----------------------------------------------------- RenderTask Creator */
NAMESPACE_BASE_BEGIN
const std::shared_ptr<RenderTask> RenderTask::Create(const std::shared_ptr<RenderItem> pRenderItem,
                                                     const size_t instanceId,
                                                     const std::shared_ptr<InstancingTask> pInstancingTask)
{
	DECL_LOGGER(base::RenderTask);
	if(!pRenderItem)
	{
		STDOUT_ERROR("Failed to get pRenderItem or pInstancingTask");
		return nullptr;
	}
	
	std::shared_ptr<RenderTask> pRenderTask = nullptr;
	const RenderItem::Type& type = pRenderItem->type();
	LOGGER_DEBUG("RenderItem.type() = %s", RenderItem::GetTypeStr(type).c_str());
	switch (type)
	{
		case RenderItem::kInvalid:
			pRenderTask = nullptr;
			break;
		case RenderItem::kBBox:
			pRenderTask = std::make_shared<BBoxTask>(pRenderItem, instanceId, pInstancingTask);
			break;
		case RenderItem::kIndexedGeom:
			//pRenderTask = std::make_shared<IndexedGeomTask>(pRenderItem, instanceId, pInstancingTask);
			break;
		case RenderItem::kExpandedGeom:
			pRenderTask = std::make_shared<ExpandedGeomTask>(pRenderItem, instanceId, pInstancingTask);
			break;
	}
	
	
	return pRenderTask;
	
}
NAMESPACE_BASE_END