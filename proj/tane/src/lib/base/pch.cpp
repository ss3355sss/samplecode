#include "base/pch.h"

NAMESPACE_BASE_BEGIN
/* ----------------------------------------------- log config */
bool base::gl               = (ENABLE_LOGGER ? false : false);
bool base::abc              = (ENABLE_LOGGER ? false : false);
bool base::usd              = (ENABLE_LOGGER ? false  : false);
bool base::Texture          = (ENABLE_LOGGER ? false : false);
bool base::RenderItem       = (ENABLE_LOGGER ? false : false);
bool base::BBoxItem         = (ENABLE_LOGGER ? false : false);
bool base::GeomItem         = (ENABLE_LOGGER ? false : false);
bool base::InstanceItem     = (ENABLE_LOGGER ? false  : false);
bool base::RenderTask       = (ENABLE_LOGGER ? false : false);
bool base::BBoxTask         = (ENABLE_LOGGER ? false : false);
bool base::IndexedGeomTask  = (ENABLE_LOGGER ? false : false);
bool base::ExpandedGeomTask = (ENABLE_LOGGER ? false : false);
bool base::InstancingTask   = (ENABLE_LOGGER ? false : false);
bool base::TaskMananger     = (ENABLE_LOGGER ? false : false);
NAMESPACE_BASE_END
