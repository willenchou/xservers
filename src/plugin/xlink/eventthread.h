#pragma once
#include <xbase/thread.h>
#include <xbase/mutex.h>
#include <xbase/event.h>

#include "event_dispatch.h"
namespace x {
class EventThread:public Thread
{
public:
	EventThread();
	virtual ~EventThread();
public:
	EventDispatch* GetDispatch();
protected:
	virtual long Run() ;

private:
	EventDispatch* eventDispatch;
	int maxconnNum_;
};
}
