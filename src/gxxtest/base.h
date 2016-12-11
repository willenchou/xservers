#pragma once
//xbase makefile gxx
//g++ -shared -o ../bin/libxbase.so -fPIC thread.cpp xml.cpp
//
//
//

#include "os_common.h"
//gbase makefile gxx
//1.
//g++ -c -fPIC -O2 base.cpp
//g++ -shared -o ../bin/libgbase.so base.o
//2.
//g++ -shared -o ../bin/libgbase.so -fPIC base.cpp
namespace gxx{
#	if defined(OS_WIN)
#		if defined(XBASE_IMPLEMENTATION)
#			define XBASE_EXPORT __declspec(dllexport)
#		else
#			define XBASE_EXPORT __declspec(dllimport)
#		endif
#	else
#		define XBASE_EXPORT
#	endif
XBASE_EXPORT int SleepG();
class XBASE_EXPORT base
{
public:
	base(void);
	~base(void);
};
}

