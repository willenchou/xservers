#pragma once
//xlink makefile gxx
//g++ -shared -o ../../bin/libxlink.so -fPIC acceptor.cpp channel.cpp connection.cpp linkserver.cpp  connector.cpp linkclient.cpp linkfactory.cpp socket.cpp event_dispatch.cpp eventthread.cpp eventthreadpool.cpp message.cpp pack.cpp epoll.cpp kqueue.cpp select.cpp multiplexingfactory.cpp -I../../ -lxbase -L../../bin
#include "base/base.h"
//glink makefile gxx
//1.
//g++ -c -fPIC -O2 link.cpp -I../../
//g++ -shared -o ../../bin/libglink.so link.o -lgbase -L../../bin
//2.
//g++ -shared -o ../../bin/libglink.so -fPIC link.cpp -I../../ -lgbase -L../../bin 
namespace gxx{
#	if defined(OS_WIN)
#		if defined(XLINK_IMPLEMENTATION)
#			define XLINK_EXPORT __declspec(dllexport)
#		else
#			define XLINK_EXPORT __declspec(dllimport)
#		endif
#	else
#		define XLINK_EXPORT
#	endif
XLINK_EXPORT int SleepL();
class link
{
public:
	link(void);
	~link(void);
};
}

