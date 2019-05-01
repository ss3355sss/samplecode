#include "maya/messageMPxData.h"
NAMESPACE_BASE_BEGIN

MTypeId MessageMPxData::id(RegisteredNodes->MessageMPxData.id);
MString MessageMPxData::typeName(RegisteredNodes->MessageMPxData.name);

///------------------------------------------------------------ static functions
void* MessageMPxData::creator()
{
	return new MessageMPxData;
}
///------------------------------------------------------------ constructor / destructor
MessageMPxData::MessageMPxData() : data_(std::make_shared<Message>())
{
}
MessageMPxData::~MessageMPxData()
{
}
///------------------------------------------------------------ virtual functions
void MessageMPxData::copy(const MPxData &other)
{
	if(other.typeId() != MessageMPxData::id)
	{
		STDOUT_ERROR("Failed to copy MessageMPxData");
		return;
	}
	const MessageMPxData* otherData = dynamic_cast<const MessageMPxData*>(&other);
	/*copy*/
	//*data_ = *otherData->data();
	
	/*reference*/
	data_ = otherData->data();
}
MTypeId MessageMPxData::typeId() const
{
	return MessageMPxData::id;
}
MString MessageMPxData::name() const
{
	return MessageMPxData::typeName;
}

///------------------------------------------------------------ public helper functions
const std::shared_ptr<Message>& MessageMPxData::data() const
{
	return data_;
}
std::shared_ptr<Message>& MessageMPxData::data()
{
	return data_;
}
///------------------------------------------------------------ private helper functions



NAMESPACE_BASE_END
