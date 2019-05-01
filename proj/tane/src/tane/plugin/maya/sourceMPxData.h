#pragma once
#include "maya/pch.h"

#include "base/bbox.h"
#include "base/renderItem.h"

NAMESPACE_BASE_BEGIN
class Source
{
protected:
	const int                         type_;
	const MDagPath                    objPath_;
	const std::shared_ptr<RenderItem> pRenderItem_;

public:
	///------------------------------------------------------------ constructor / destructor
	Source(const MDagPath& objPath, const std::shared_ptr<RenderItem> pRenderItem, const int type)
		: objPath_(objPath),
		  pRenderItem_(pRenderItem),
		  type_(type)
	{
	}
	virtual ~Source()
	{
	}
public:
	///------------------------------------------------------------ accessor function
	const int type() const
	{
		return type_;
	}
public:
	const MDagPath objPath() const
	{
		return objPath_;
	}
	const BBox3f bound() const
	{
		return pRenderItem_->bbox();
	}
	const std::shared_ptr<RenderItem>& renderItem() const
	{
		return pRenderItem_;
	}
private:
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
class SourceAbc final : public Source
{
public:
	typedef Source Base;
private:
	const std::string 	filePath_;
	const bool 	        drawAsBBox_;
	const std::string 	objectPath_;
	
	const std::string 	proxyPath_;
	const bool 	        useDifferentObjPath_;
	const std::string 	objectPathForProxy_;

public:
	SourceAbc(const MDagPath&                   objPath,
	          const std::shared_ptr<RenderItem> pRenderItem,
	          const std::string&                filePath,
	          const bool&                       drawAsBBox,
	          const std::string&                objectPath,
	          const std::string&                proxyPath              = "",
	          const bool&                       useDifferentObjPath    = false,
	          const std::string&                objectPathForProxy     = ""
	)
		: Base(objPath, pRenderItem, sourceType::abc),
		  filePath_(filePath),
		  drawAsBBox_(drawAsBBox),
		  objectPath_(objectPath),
		  proxyPath_(proxyPath),
		  useDifferentObjPath_(useDifferentObjPath),
		  objectPathForProxy_(objectPathForProxy)
	{
	}
	virtual ~SourceAbc()
	{
	}

public:
	const std::string&  filePath() const             { return filePath_;     };
	const bool&         drawAsBBox() const           { return drawAsBBox_;   }
	const std::string&  objectPath() const           { return objectPath_;   };
	
	const std::string&  proxyPath() const            { return proxyPath_;    };
	const bool&         useDifferentObjPath() const  { return useDifferentObjPath_;  }
	const std::string&  objectPathForProxy() const   { return objectPathForProxy_;   };
};

NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
class SourceAlembic final : public Source
{
public:
	typedef Source Base;
public:
	const MString filePath0;
	const MString objectPath0;
	const MString proxyPath0;
	const MString proxyObjectPath0;
	const bool    useLOD1;
	const MString filePath1;
	const MString objectPath1;
	const MString proxyPath1;
	const MString proxyObjectPath1;
	const bool    useLOD2;
	const MString filePath2;
	const MString objectPath2;
	const MString proxyPath2;
	const MString proxyObjectPath2;
public:
	SourceAlembic(const MDagPath& objPath,
	              const std::shared_ptr<RenderItem> pRenderItem,
	              const MString& filePath0,
	              const MString& objectPath0,
	              const MString& proxyPath0,
	              const MString& proxyObjectPath0,
	              const bool& useLOD1,
	              const MString& filePath1,
	              const MString& objectPath1,
	              const MString& proxyPath1,
	              const MString& proxyObjectPath1,
	              const bool& useLOD2,
	              const MString& filePath2,
	              const MString& objectPath2,
	              const MString& proxyPath2,
	              const MString& proxyObjectPath2
	)
		: Base(objPath, pRenderItem, sourceType::abc2),
		  filePath0(filePath0),
		  objectPath0(objectPath0),
		  proxyPath0(proxyPath0),
		  proxyObjectPath0(proxyObjectPath0),
		  useLOD1(useLOD1),
		  filePath1(filePath1),
		  objectPath1(objectPath1),
		  proxyPath1(proxyPath1),
		  proxyObjectPath1(proxyObjectPath1),
		  useLOD2(useLOD2),
		  filePath2(filePath2),
		  objectPath2(objectPath2),
		  proxyPath2(proxyPath2),
		  proxyObjectPath2(proxyObjectPath2)
	{
	}
	virtual ~SourceAlembic()
	{
	}

public:
};
NAMESPACE_BASE_END


NAMESPACE_BASE_BEGIN
///------------------------------------------------- SourceMpxData
class SourceMPxData : public MPxData
{
public:
	static MTypeId id;
	static MString typeName;

private:
	std::shared_ptr<Source> data_;
public:
	static void* creator();

public:
	SourceMPxData();
	virtual ~SourceMPxData();

public:
	virtual void copy(const MPxData& other);
	virtual MTypeId typeId() const;
	virtual MString name() const;

public:
	const std::shared_ptr<Source>& data() const;
	std::shared_ptr<Source>& data();
};
NAMESPACE_BASE_END

