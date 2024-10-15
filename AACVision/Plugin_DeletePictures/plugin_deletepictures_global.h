#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PLUGIN_DELETEPICTURES_LIB)
#  define PLUGIN_DELETEPICTURES_EXPORT Q_DECL_EXPORT
# else
#  define PLUGIN_DELETEPICTURES_EXPORT Q_DECL_IMPORT
# endif
#else
# define PLUGIN_DELETEPICTURES_EXPORT
#endif
