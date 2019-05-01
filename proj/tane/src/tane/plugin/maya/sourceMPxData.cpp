#include "maya/sourceMPxData.h"

NAMESPACE_BASE_BEGIN

MTypeId SourceMPxData::id(RegisteredNodes->SourceMPxData.id);
MString SourceMPxData::typeName(RegisteredNodes->SourceMPxData.name);

///------------------------------------------------------------ static functions
void* SourceMPxData::creator()
{
	return new SourceMPxData;
}
///------------------------------------------------------------ constructor / destructor
SourceMPxData::SourceMPxData() : data_(std::make_shared<Source>(MDagPath(), nullptr, sourceType::invalid))
{
}
SourceMPxData::~SourceMPxData()
{
}
///------------------------------------------------------------ virtual functions
void SourceMPxData::copy(const MPxData &other)
{
	if(other.typeId() != SourceMPxData::id)
	{
		STDOUT_ERROR("Failed to copy SourceMPxData");
		return;
	}
	const SourceMPxData* otherData = dynamic_cast<const SourceMPxData*>(&other);
	
	data_       = otherData->data_;     ///reference
}
MTypeId SourceMPxData::typeId() const
{
	return SourceMPxData::id;
}
MString SourceMPxData::name() const
{
	return SourceMPxData::typeName;
}
///------------------------------------------------------------ public helper functions
const std::shared_ptr<Source>& SourceMPxData::data() const
{
	return data_;
}
std::shared_ptr<Source>& SourceMPxData::data()
{
	return data_;
}



///------------------------------------------------------------ private helper functions



NAMESPACE_BASE_END
