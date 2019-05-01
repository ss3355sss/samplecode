#pragma once
#include "maya/pch.h"

#include "base/points.h"
///------------------------------------------------- PointsMpxData
NAMESPACE_BASE_BEGIN
///----------------------------------------------------------------------------- PointsIterator
class PointsIterator : public MPxGeometryIterator
{
private:
	Points* data_;

public:
	///------------------------------------------------------------ constructor / destructor
	PointsIterator(void* userGeometry, MObjectArray &components);
	PointsIterator(void* userGeometry, MObject &components);
	virtual ~PointsIterator();

public:
	///------------------------------------------------------------ virtual functions
	virtual void reset();
	virtual MPoint point() const;
	virtual void setPoint(const MPoint &point) const;
	virtual int iteratorCount() const;
	virtual bool hasPoints() const;

public:
	///------------------------------------------------------------ public helper functions
private:
	///------------------------------------------------------------ private helper functions
};
NAMESPACE_BASE_END
NAMESPACE_BASE_BEGIN
///----------------------------------------------------------------------------- PointsMPxData
class PointsMPxData : public MPxGeometryData
{
public:
	static MTypeId id;
	static MString typeName;

public:
	std::shared_ptr<Points> data_;

public:
	///------------------------------------------------------------ static functions
	static void* creator();

public:
	///------------------------------------------------------------ constructor / destructor
	PointsMPxData();
	virtual ~PointsMPxData();

public:
	///------------------------------------------------------------ virtual functions
	//virtual MStatus readASCII(const MArgList &argList, unsigned &index);
	//virtual MStatus readBinary(istream &in, unsigned length);
	//virtual MStatus writeASCII(ostream &out);
	//virtual MStatus writeBinary(ostream &out);
	virtual void copy(const MPxData &other);
	virtual MTypeId typeId() const;
	virtual MString name() const;

	virtual MPxGeometryIterator* iterator(MObjectArray &componentList, MObject &component, bool useComponents);
	virtual MPxGeometryIterator* iterator(MObjectArray &componentList, MObject &component, bool useComponents, bool world) const;
	virtual bool updateCompleteVertexGroup(MObject &component) const;
public:
	///------------------------------------------------------------ public helper functions
	const std::shared_ptr<Points>& data() const;
	std::shared_ptr<Points>& data();
	//void data(const std::shared_ptr<Points> data);

private:
	///------------------------------------------------------------ private helper functions
};
NAMESPACE_BASE_END