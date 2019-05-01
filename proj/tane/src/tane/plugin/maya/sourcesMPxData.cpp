#include "maya/sourcesMPxData.h"

NAMESPACE_BASE_BEGIN
MTypeId SourcesMPxData::id(RegisteredNodes->SourcesMPxData.id);
MString SourcesMPxData::typeName(RegisteredNodes->SourcesMPxData.name);

void* SourcesMPxData::creator()
{
	return new SourcesMPxData;
}
SourcesMPxData::SourcesMPxData() : data_(std::make_shared<SourceManager>())
{
}
SourcesMPxData::~SourcesMPxData()
{
}

void SourcesMPxData::copy(const MPxData &other)
{
	if(other.typeId() != SourcesMPxData::id)
	{
		STDOUT_ERROR("Failed to copy SourcesMPxData");
		return;
	}
	
	const SourcesMPxData* otherData = dynamic_cast<const SourcesMPxData*>(&other);
	
	/*copy*/
	//*data_ = *otherData->data();
	
	/*reference*/
	data_ = otherData->data();
	
}
MTypeId SourcesMPxData::typeId() const
{
	return SourcesMPxData::id;
}
MString SourcesMPxData::name() const
{
	return SourcesMPxData::typeName;
}
const std::shared_ptr<SourceManager>& SourcesMPxData::data() const
{
	return data_;
}
std::shared_ptr<SourceManager>&       SourcesMPxData::data()
{
	return data_;
}


NAMESPACE_BASE_END
