#include "baseUsd/pch.h"

/* ----------------------------------------------- log config */
NAMESPACE_BASE_BEGIN
namespace logging
{
	const bool _baseUsd::UsdImporter    = (ENABLE_LOGGER ? false : false);
	const bool _baseUsd::UsdExporter    = (ENABLE_LOGGER ? false : false);

	StaticData<_baseUsd> baseUsd;
}

NAMESPACE_BASE_END
/* ----------------------------------------------- sementic */
NAMESPACE_BASE_BEGIN
namespace sementic
{
}
NAMESPACE_BASE_END
