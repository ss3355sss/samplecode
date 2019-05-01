#pragma once

#include "base/pch.h"

#include "base/frame.h"
#include "base/bbox.h"


NAMESPACE_BASE_BEGIN
struct _DrawElementCmd
{
	int     indexOffset;
	size_t  indexCount;
};
typedef Array1 <_DrawElementCmd> DrawElementCmd;

struct _DrawArrayCmd
{
	int     first;
	size_t  count;
};
typedef Array1 <_DrawArrayCmd> DrawArrayCmd;


NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
/* ----------------------------------------------------- RenderItem Base */
class RenderItem //: boost::noncopyable
{
public:
	enum Type
	{
		kInvalid,
		kBBox,
		kIndexedGeom,
		kExpandedGeom,
	};
	static const std::string GetTypeStr(const Type& type)
	{
		std::string msg;
		switch (type)
		{
			case RenderItem::kInvalid:
				msg = "kInvalid";
				break;
			case RenderItem::kBBox:
				msg = "kBBox";
				break;
			case RenderItem::kIndexedGeom:
				msg = "kIndexedGeom";
				break;
			case RenderItem::kExpandedGeom:
				msg = "kExpandedGeom";
				break;
		}
		return msg;
	}


private:
	DrawElementCmd drawElementCmd_ = DrawElementCmd(LOD::maxLevel);
	BBox3f         bbox_           = BBox3f();
	Type           type_           = Type::kInvalid;

protected:
	///------------------------------------------------------------ constructor / destructor
	RenderItem(const BBox3f& bbox, const Type type) : bbox_(bbox), type_(type)
	{
	}
	virtual ~RenderItem()
	{
	}
	

public:
	///------------------------------------------------------------ accessor
	const Type& type() const                        { return type_;             }
public:
	const BBox3f& bbox() const                      { return bbox_;             }
	BBox3f& bbox()                                  { return bbox_;             }
public:
};

/* ----------------------------------------------------- RenderItem Proxy */
template<typename T>
class RenderItemProxy : public RenderItem
{
public:
	typedef RenderItem Base;
	typedef T          ItemT;

private:
	const std::shared_ptr<ItemT> pItem_;

protected:
	RenderItemProxy(const BBox3f& bbox, const Type type, const std::shared_ptr<ItemT> pItem)
		: Base(bbox, type),
		  pItem_(pItem)
	{
	}
	virtual ~RenderItemProxy()
	{
	}

public:
	const std::shared_ptr<ItemT>& item() const
	{
		return pItem_;
	}
private:

};

NAMESPACE_BASE_END

/* ----------------------------------------------------- BBoxItem */
NAMESPACE_BASE_BEGIN
class BBoxItem final : public RenderItemProxy<BBox3f>
{
public:
	typedef RenderItemProxy<BBox3f> Base;
	typedef typename Base::ItemT    ItemT;

public:
	/* ------------------------------------------------------------ constructor / destructor */
	BBoxItem(const BBox3f& bbox)
		: Base(bbox, Base::kBBox, std::make_shared<BBox3f>(bbox))
	{
	}
	virtual ~BBoxItem()
	{
	}

private:

};
NAMESPACE_BASE_END

/* ----------------------------------------------------- IndexedGeomItem */
NAMESPACE_BASE_BEGIN
struct IndexedGeom
{
	Point3fArray    pointArray;
	Int32Array      pointIndices;
	Normal3fArray   normalArray;
	Int32Array      normalIndices;
	TexCoord2fArray uvArray;
	Int32Array      uvIndices;
};
class IndexedGeomItem final : public RenderItemProxy<IndexedGeom>
{
public:
	typedef RenderItemProxy<IndexedGeom> Base;
	typedef typename Base::ItemT         ItemT;

public:
	/* ------------------------------------------------------------ constructor / destructor */
	IndexedGeomItem (const BBox3f& bbox, const std::shared_ptr<ItemT> pItem)
		: Base(bbox, Base::kIndexedGeom, pItem)
	{
	
	}
	virtual ~IndexedGeomItem()
	{
	}

private:
};
NAMESPACE_BASE_END

/* ----------------------------------------------------- ExpandedGeomItem */
NAMESPACE_BASE_BEGIN
struct ExpandedGeom
{
	DrawElementCmd  drawCmd         = DrawElementCmd(LOD::maxLevel);
	Point3fArray    pointArray      = Point3fArray(0);
	Normal3fArray   normalArray     = Normal3fArray(0);
	TexCoord3fArray uvArray         = TexCoord3fArray(0);
	Int32Array      indexArray      = Int32Array(0);
};
class ExpandedGeomItem final : public RenderItemProxy<ExpandedGeom>
{
public:
	typedef RenderItemProxy<ExpandedGeom> Base;
	typedef typename Base::ItemT          ItemT;

public:
	/* ------------------------------------------------------------ constructor / destructor */
	ExpandedGeomItem(const BBox3f& bbox, const std::shared_ptr<ItemT> pItem)
		: Base(bbox, Base::kExpandedGeom, pItem)
	{
	}
	virtual ~ExpandedGeomItem()
	{
	}

private:
};
NAMESPACE_BASE_END

