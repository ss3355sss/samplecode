#pragma once

/* ----------------------------------------------- include */
/* ---------------------------- base */
#include "base/pch.h"
#include "base/logger.h"
#include "base/staticData.h"

/* ---------------------------- usd api */
//TODO::include usd sdk

/* ----------------------------------------------- macro */

/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
namespace logging
{
	struct _baseUsd
	{
		static const bool UsdImporter;
		static const bool UsdExporter;
	};
	extern StaticData<_baseUsd> baseUsd;
}

NAMESPACE_BASE_END
/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
namespace sementic
{

}
NAMESPACE_BASE_END
