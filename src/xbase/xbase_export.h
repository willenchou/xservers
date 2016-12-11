#ifndef XBASE_XBASE_EXPORT_H_
#define XBASE_XBASE_EXPORT_H_

#include "os_common.h"
#define  BASE_EXPORT XBASE_EXPORT

#if defined(XBASE_COMPONENT_BUILD)//__attribute__((visibility("default")))
#	if defined(OS_WIN)
#		if defined(XBASE_IMPLEMENTATION)
#			define XBASE_EXPORT __declspec(dllexport)
#		else
#			define XBASE_EXPORT __declspec(dllimport)
#		endif
#	else
#		define XBASE_EXPORT
#	endif
#else
#	define XBASE_EXPORT
#endif


#endif //XBASE_XBASE_EXPORT_H_
