#pragma once
#include "baseGl/pch.h"

#include "base/bbox.h"

#include "base/renderItem.h"
#include "base/instancingItem.h"

#include "baseGl/instancingTask.h"
#include "baseGl/renderTask.h"

NAMESPACE_BASE_BEGIN
class TaskManager final
{
public:
	typedef std::map<int, std::shared_ptr<RenderTask>, std::less<int>>       TaskMap;
	typedef Array1 <std::pair<const int, const std::shared_ptr<RenderTask>>> TaskArray;
	
private:
	std::shared_ptr<InstancingTask> pInstancingTask_;
	TaskMap                         taskMap_;
	
public:
	TaskManager() : pInstancingTask_(nullptr)
	{
	}
	~TaskManager()
	{
	}

public:
	const std::shared_ptr<InstancingTask> getInstancingTask()
	{
		return pInstancingTask_;
	}
public:
	void setInstancingTask(const std::shared_ptr<InstancingTask> pInputTask)
	{
		if(!pInputTask->isPopulated())
		{
			STDOUT_ERROR("inputTask is not populated");
			return;
		}
		if(pInstancingTask_ && pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("current instancingTask is populated");
			return;
		}
		pInstancingTask_ = pInputTask;
	}
	void removeInstancingTask()
	{
		if(pInstancingTask_ && pInstancingTask_->isPopulated())
		{
			STDOUT_ERROR("Failed to remove instancingTask (populated task),")
			return;
		}
		pInstancingTask_ = nullptr;
	}
	void clearInstancingTask()
	{
		std::shared_ptr<InstancingTask> pInstancingTask = nullptr;
		/* depopulate previous instancingTask if exists */
		pInstancingTask = getInstancingTask();
		if(pInstancingTask && pInstancingTask->isPopulated())
		{
			pInstancingTask->depopulate();
		}
	}

public:
	const std::shared_ptr<RenderTask> getRenderTask(const int32_t& instanceId)
	{
		//if(taskMap_.find(taskKey) == taskMap_.end())
		//{
		//	STDOUT_ERROR("Failed to get renderTask[%d], (not exists in queue)", taskKey);
		//	return nullptr;
		//}
		return taskMap_[instanceId];
	}
public:
	void setRenderTask(const int32_t& instanceId, const std::shared_ptr<RenderTask> pInputTask)
	{
		if(!pInputTask->isPopulated())
		{
			STDOUT_ERROR("inputTask is not populated");
			return;
		}
		const std::shared_ptr<RenderTask> pRenderTask = taskMap_[instanceId];
		if(pRenderTask && pRenderTask->isPopulated())
		{
			STDOUT_ERROR("Failed to update renderTask[%d], (populated task)", instanceId);
			return;
		}
		
		taskMap_[instanceId] = pInputTask;
	}
	void removeRenderTask(const int32_t& instanceId)
	{
		if(taskMap_.find(instanceId) == taskMap_.end())
		{
			STDOUT_ERROR("Failed to remove renderTask[%d], (not exists in queue)", instanceId);
			return;
		}
		if(taskMap_[instanceId] && taskMap_[instanceId]->isPopulated())
		{
			STDOUT_ERROR("Failed to remove renderTask[%d], (populated task)", instanceId);
			return;
		}
		taskMap_.erase(instanceId);
	}
	void clearRenderTasks()
	{
		for(TaskMap::iterator iter = taskMap_.begin(); iter != taskMap_.cend(); ++iter)
		{
			const int                         instanceID  = iter->first;
			const std::shared_ptr<RenderTask> pRenderTask = iter->second;
			if(pRenderTask && pRenderTask->isPopulated())
			{
				pRenderTask->depopulate();
			}
		}
		taskMap_.clear();
	}

public:
	const TaskArray taskArray() const
	{
		TaskArray taskArray;
		taskArray.reserve(taskMap_.size());

		for(TaskMap::const_iterator cIter = taskMap_.cbegin(); cIter != taskMap_.cend(); ++cIter)
		{
			taskArray.push_back(*cIter);
		}
		
		return taskArray;
	}
	
	

};
NAMESPACE_BASE_END
