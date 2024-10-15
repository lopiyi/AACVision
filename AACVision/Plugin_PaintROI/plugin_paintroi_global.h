#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PLUGIN_PAINTROI_LIB)
#  define PLUGIN_PAINTROI_EXPORT Q_DECL_EXPORT
# else
#  define PLUGIN_PAINTROI_EXPORT Q_DECL_IMPORT
# endif
#else
# define PLUGIN_PAINTROI_EXPORT
#endif
