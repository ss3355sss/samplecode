#pragma once
/* ----------------------------------------------- include */
#include "base/pch.h"
#include "base/staticData.h"

/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
struct tane
{

};
NAMESPACE_BASE_END
/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
/* ---------------------------- templateType */
namespace templetType
{
	enum : short
	{
		invalid,
		environment,
		bind,
		destruction,
		footprint,
	};
	inline const std::string ToString(const short e)
	{
		switch (e)
		{
			case invalid:
				return ENUM_TO_STRING(invalid);
			case environment:
				return ENUM_TO_STRING(environment);
			case destruction:
				return ENUM_TO_STRING(destruction);
			case footprint:
				return ENUM_TO_STRING(footprint);
		}
	}
}
/* ---------------------------- sourceType */
namespace sourceType
{
	enum : int
	{
		invalid,
		abc,
		abc2,
		usd,
		rib,
		vdb,
		mesh,
	};
}
/* ---------------------------- node attribute */
namespace nodeAttr
{
	typedef const char* value_type;
	typedef std::vector<value_type> container;
	
	struct _base
	{
	public:
		///----------------------------------- display attributes
		value_type displayMode  = "displayMode";
		value_type color        = "color";
		value_type transparency = "transparency";
		///----------------------------------- time attributes
		value_type forceStatic  = "forceStatic";
		value_type frameOffset  = "frameOffset";
		value_type animCycle    = "animCycle";
		
		///----------------------------------- internal attributes
		value_type drawBound    = "drawBound";
		value_type enableIcr    = "enableIcr";
		value_type enableLOD    = "enableLOD";
		value_type minDistance  = "minDistance";
		value_type maxDistance  = "maxDistance";
		value_type noLighting   = "noLighting";
		value_type noWire       = "noWire";
		value_type overrideMask = "overrideMask";

	protected:
		container data_;
	public:
		_base() : data_({displayMode,
		                 color,
		                 transparency,
		                 forceStatic,
		                 frameOffset,
		                 animCycle,
		                 drawBound,
		                 enableIcr,
		                 enableLOD,
		                 minDistance,
		                 maxDistance,
		                 noLighting,
		                 noWire,
		                 overrideMask})
		{
		}
		virtual ~_base()
		{
		}
	public:
		const container& data() const { return data_; }
	};
	struct _abc  : public _base
	{
	public:
		typedef _base Base;
	
	public:
		value_type renderFile         = "renderFile";
		value_type renderObjectPath   = "renderObjectPath";
		value_type useProxyFile       = "useProxyFile";
		value_type proxyFile          = "proxyFile";
		value_type useProxyObjectPath = "useProxyObjectPath";
		value_type proxyObjectPath    = "proxyObjectPath";
		
	public:
		_abc() : Base()
		{
			data_.push_back(renderFile         );
			data_.push_back(renderObjectPath   );
			data_.push_back(useProxyFile       );
			data_.push_back(proxyFile          );
			data_.push_back(useProxyObjectPath );
			data_.push_back(proxyObjectPath    );
		}
		virtual ~_abc()
		{
		}
		
	};
	struct _abc2 : public _base
	{
	public:
		typedef _base Base;
	public:
		value_type filePath0        = "filePath0";
		value_type objectPath0      = "objectPath0";
		value_type proxyPath0       = "proxyPath0";
		value_type proxyObjectPath0 = "proxyObjectPath0";

		value_type useLOD1          = "useLOD1";
		value_type filePath1        = "filePath1";
		value_type objectPath1      = "objectPath1";
		value_type proxyPath1       = "proxyPath1";
		value_type proxyObjectPath1 = "proxyObjectPath1";

		value_type useLOD2          = "useLOD2";
		value_type filePath2        = "filePath2";
		value_type objectPath2      = "objectPath2";
		value_type proxyPath2       = "proxyPath2";
		value_type proxyObjectPath2 = "proxyObjectPath2";
	public:
		_abc2() : Base()
		{
			data_.push_back(filePath0        );
			data_.push_back(objectPath0      );
			data_.push_back(proxyPath0       );
			data_.push_back(proxyObjectPath0 );
			data_.push_back(useLOD1          );
			data_.push_back(filePath1        );
			data_.push_back(objectPath1      );
			data_.push_back(proxyPath1       );
			data_.push_back(proxyObjectPath1 );
			data_.push_back(useLOD2          );
			data_.push_back(filePath2        );
			data_.push_back(objectPath2      );
			data_.push_back(proxyPath2       );
			data_.push_back(proxyObjectPath2 );
		}
		virtual ~_abc2()
		{
		}
	};
	struct _usd  : public _base
	{
	public:
		typedef _base Base;
	public:
		value_type renderFile  = "renderFile";
		value_type primPath    = "primPath";
		value_type purpose     = "purpose";
		value_type variantKey  = "variantKey";
	public:
		_usd() : Base()
		{
			data_.push_back(renderFile);
			data_.push_back(primPath);
			data_.push_back(purpose);
			data_.push_back(variantKey);
		}
		virtual ~_usd()
		{
		}

	};
	struct _vdb  : public _base
	{
	public:
		typedef _base Base;
	public:
		value_type renderFile         = "renderFile";
		value_type proxyFile          = "proxyFile";
	public:
		_vdb() : Base()
		{
			data_.push_back(renderFile);
			data_.push_back(proxyFile);
		}
		virtual ~_vdb()
		{
		}
		
	};
	extern StaticData<_base>  base;
	extern StaticData<_abc>   abc;
	extern StaticData<_abc2>  abc2;
	extern StaticData<_usd>   usd;
	extern StaticData<_vdb>   vdb;
}

namespace AttrNames
{
	struct _UsdProxyShape
	{
		const char* filePath            = "filePath";
		const char* primPath            = "primPath";
		const char* excludePrimPaths    = "excludePrimPaths";
		const char* inTime              = "inTime";
		const char* variantKey          = "variantKey";
		const char* complexity          = "complexity";
		const char* inStageData         = "inStageData";
		const char* inStageDataCached   = "inStageDataCached";
		const char* fastPlayback        = "fastPlayback";
		const char* outStageData        = "outStageData";
		const char* purpose             = "purpose";
		const char* displayGuides       = "displayGuides";
		const char* displayRenderGuides = "displayRenderGuides";
		const char* softSelectable      = "softSelectable";
	};
	extern StaticData<_UsdProxyShape> UsdProxyShape;
	
}






NAMESPACE_BASE_END
