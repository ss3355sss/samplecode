#pragma once
#include "maya/pch.h"

#include "maya/sourceMPxData.h"

NAMESPACE_BASE_BEGIN
class SourceManager final
{
public:
	typedef std::map<int, std::shared_ptr<Source>, std::less<int>> SourceMapT;

private:
	SourceMapT map_;
public:
	IMPL_MAP(SourceManager, SourceMapT, map_);

public:
	const Array1<std::shared_ptr<Source>> sources() const
	{
		Array1<std::shared_ptr<Source>> array;
		array.reserve(size());
		
		for(const_iterator cIter = cbegin(); cIter != cend(); ++cIter)
		{
			const int&                      id  = cIter->first;
			const std::shared_ptr<Source>&      src = cIter->second;
			array.push_back(src);
		}
		return array;
	}
	const Int32Array sourceIDs() const
	{
		Array1<int> array;
		array.reserve(size());
		for(const_iterator cIter = cbegin(); cIter != cend(); ++cIter)
		{
			const int&  id  = cIter->first;
			array.push_back(id);
		}
		return array;
	}
	
	
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
class SourcesMPxData :public MPxData
{
public:
	static MTypeId id;
	static MString typeName;

private:
	std::shared_ptr<SourceManager> data_;

public:
	static void *creator();

public:
	SourcesMPxData();
	virtual ~SourcesMPxData();

public:
	virtual void 	copy(const MPxData &other);
	virtual MTypeId typeId() const;
	virtual MString name() const;

public:
	const std::shared_ptr<SourceManager>& data() const;
	std::shared_ptr<SourceManager>&       data();
	
};

NAMESPACE_BASE_END