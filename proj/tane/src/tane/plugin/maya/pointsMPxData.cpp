#include "maya/pointsMPxData.h"
NAMESPACE_BASE_BEGIN
///----------------------------------------------------------------------------- PointsIterator
///------------------------------------------------------------ static functions
///------------------------------------------------------------ constructor / destructor
PointsIterator::PointsIterator(void* userGeometry, MObjectArray &components)
	: MPxGeometryIterator(userGeometry, components),
	  data_((Points*) userGeometry)
{
	DECL_LOGGER(logging::maya->PointsIterator);
	reset();
}
PointsIterator::PointsIterator(void* userGeometry, MObject &components)
	: MPxGeometryIterator(userGeometry, components),
	  data_((Points*) userGeometry)
{
	DECL_LOGGER(logging::maya->PointsIterator);
	reset();
}
PointsIterator::~PointsIterator()
{
}
///------------------------------------------------------------ virtual functions
void PointsIterator::reset()
{
	DECL_LOGGER(logging::maya->PointsIterator);
	MPxGeometryIterator::reset();
	setCurrentPoint(0);
	if(data_ != nullptr)
	{
		setMaxPoints(data_->size());
	}
}
MPoint PointsIterator::point() const
{
	DECL_LOGGER(logging::maya->PointsIterator);
	MPoint ret;
	if(data_ != nullptr)
	{
		unsigned int idx = index();
		if(idx < data_->size())
		{
			//ret = (*data_)[index()];
			ret.x = (*data_)[idx][0];
			ret.y = (*data_)[idx][1];
			ret.z = (*data_)[idx][2];
		}
	}
	return ret;
}
void PointsIterator::setPoint(const MPoint &point) const
{
	DECL_LOGGER(logging::maya->PointsIterator);
	if(data_ != nullptr)
	{
		unsigned int idx = this->index();
		if(idx < data_->size())
		{
			//(*data_)[index()] = point;
			(*data_)[idx][0] = point.x;
			(*data_)[idx][1] = point.y;
			(*data_)[idx][2] = point.z;

		}
	}
}
int PointsIterator::iteratorCount() const
{
	DECL_LOGGER(logging::maya->PointsIterator);
	return data_->size();
}
bool PointsIterator::hasPoints() const
{
	DECL_LOGGER(logging::maya->PointsIterator);
	return true;
}

///------------------------------------------------------------ public helper functions
///------------------------------------------------------------ private helper functions
NAMESPACE_BASE_END
NAMESPACE_BASE_BEGIN
///----------------------------------------------------------------------------- PointsMPxData
MTypeId PointsMPxData::id(RegisteredNodes->PointsMPxData.id);
MString PointsMPxData::typeName(RegisteredNodes->PointsMPxData.name);

///------------------------------------------------------------ static functions
void* PointsMPxData::creator()
{
	return new PointsMPxData;
}
///------------------------------------------------------------ constructor / destructor
PointsMPxData::PointsMPxData() : data_(Points::Create(0))
{
}
PointsMPxData::~PointsMPxData()
{
}

///------------------------------------------------------------ virtual functions
//MStatus PointsMPxData::readASCII(const MArgList &argList, unsigned &index)
//{
//	DECL_LOGGER(log::PointsMPxData);
//	//TODO::read points as ascii
//
//	return MS::kFailure;
//}
//MStatus PointsMPxData::readBinary(istream &in, unsigned length)
//{
//	DECL_LOGGER(log::PointsMPxData);
//
//	//if(length <= 0)
//	//{
//	//	LOGGER_WARNING("Failed to get valid length");
//	//	return MS::kFailure;
//	//}
//	//boost::shared_ptr<Points> pBoost;
//	//boost::archive::binary_iarchive ia(in);
//	//ia >> pBoost;
//	//data(ToStdPtr(pBoost));
//	return MS::kFailure;
//
//	return MS::kSuccess;
//}
//MStatus PointsMPxData::writeASCII(ostream &out)
//{
//	DECL_LOGGER(log::PointsMPxData);
//	//TODO::write points as ascii
//	return MS::kFailure;
//}
//MStatus PointsMPxData::writeBinary(ostream &out)
//{
//	DECL_LOGGER(log::PointsMPxData);
//
//	//boost::shared_ptr<Points> pBoost = ToBoostPtr(data());
//	//boost::archive::binary_oarchive oa(out);
//	//oa << pBoost;
//	return MS::kSuccess;
//}

void PointsMPxData::copy(const MPxData &other)
{
	if(other.typeId() != PointsMPxData::id)
	{
		STDOUT_ERROR("Failed to copy PointsMPxData");
		return;
	}
	const PointsMPxData* otherData = dynamic_cast<const PointsMPxData*>(&other);
	
	/*copy*/
	//*data_ = *otherData->data_;

	/*reference*/
	data_ = otherData->data_;
}

MTypeId PointsMPxData::typeId() const
{
	return PointsMPxData::id;
}
MString PointsMPxData::name() const
{
	return PointsMPxData::typeName;
}

MPxGeometryIterator* PointsMPxData::iterator(MObjectArray &componentList, MObject &component, bool useComponents)
{
	DECL_LOGGER(logging::maya->PointsMPxData);
	PointsIterator* result = nullptr;
	if(useComponents)
		result = new PointsIterator(data_.get(), componentList);
	else
		result = new PointsIterator(data_.get(), component);
	return result;

}
MPxGeometryIterator* PointsMPxData::iterator(MObjectArray &componentList, MObject &component, bool useComponents, bool world) const
{
	DECL_LOGGER(logging::maya->PointsMPxData);
	PointsIterator* result = NULL;
	if(useComponents)
		result = new PointsIterator(data_.get(), componentList);
	else
		result = new PointsIterator(data_.get(), component);
	return result;
}

bool PointsMPxData::updateCompleteVertexGroup(MObject &component) const
{
	DECL_LOGGER(logging::maya->PointsMPxData);
	MStatus                   stat;
	MFnSingleIndexedComponent fnComponent(component, &stat);

	if(stat && (nullptr != data_) && (fnComponent.isComplete()))
	{
		int maxVerts;
		fnComponent.getCompleteData(maxVerts);

		int numVertices = data_->size();

		if((numVertices > 0) && (maxVerts != numVertices))
		{
			fnComponent.setCompleteData(numVertices);
			return true;
		}
	}
	return false;

}

///------------------------------------------------------------ public helper functions
const std::shared_ptr<Points>& PointsMPxData::data() const
{
	return data_;
}
std::shared_ptr<Points>& PointsMPxData::data()
{
	return data_;
}
///------------------------------------------------------------ private helper functions
NAMESPACE_BASE_END
