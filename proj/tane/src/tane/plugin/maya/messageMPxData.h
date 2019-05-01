#pragma once
#include "maya/pch.h"
#include "base/messageQueue.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------- Message
struct Message
{
public:
	enum Type : int{
		kBODY_DIRTY,
		kSOURCE_DIRTY,
		kPOINT_DIRTY,
	};
	typedef MessageQueueProxy<Type> MessageQueue;

public:
	short        templet;
	MessageQueue msgQueue;
};

///------------------------------------------------- MessageMPxData
class MessageMPxData : public MPxData
{
public:
	static MTypeId id;
	static MString typeName;

private:
	std::shared_ptr<Message> data_;

public:
	static void* creator();

public:
	MessageMPxData();
	virtual ~MessageMPxData();
public:
	virtual void copy(const MPxData& other);
	virtual MTypeId typeId() const;
	virtual MString name() const;
public:
	const std::shared_ptr<Message>& data() const;
	std::shared_ptr<Message>& data();
};
NAMESPACE_BASE_END
