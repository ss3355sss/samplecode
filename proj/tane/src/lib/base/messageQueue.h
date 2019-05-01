#pragma once
#include "base/pch.h"
#include "base/hackedQueue.h"
NAMESPACE_BASE_BEGIN
template <typename MessageT>
class MessageQueueProxy final //: boost::noncopyable
{
public:
	typedef std::priority_queue<MessageT,
	                            std::vector<MessageT>,
	                            std::greater<MessageT>  /* ordered */
	> ContainerT;
private:
	ContainerT msgs_;

public:
	MessageQueueProxy()
	{
	}
	MessageQueueProxy(const std::initializer_list<MessageT>& lst)
	{
		typename std::initializer_list<MessageT>::iterator iter;
		for(iter = lst.begin(); iter != lst.end(); ++iter)
		{
			push((*iter));
		}
	}
	~MessageQueueProxy()
	{
	}

public:
	void push(MessageT msg)
	{
		std::vector<MessageT>& container = HackedQueue(msgs_);
		
		if(std::find(container.begin(), container.end(), msg) == container.end())
		{
			msgs_.push(msg);
		}
	}
	void pop()
	{
		msgs_.pop();
	}
	MessageT top()
	{
		return msgs_.top();
	}
	bool empty()
	{
		return msgs_.empty();
	}
	size_t size()
	{
		return msgs_.size();
	}
	MessageQueueProxy<MessageT> clone()
	{
		return *this;
	}
};


NAMESPACE_BASE_END



