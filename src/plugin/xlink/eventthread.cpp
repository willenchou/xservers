#include "eventthread.h"

namespace x {
EventThread::EventThread()
{
	eventDispatch= new EventDispatch();
	maxconnNum_ = FD_CONNSIZE -1;

}


EventThread::~EventThread()
{
	if(eventDispatch) delete eventDispatch;
}
EventDispatch* EventThread::GetDispatch(){
	return eventDispatch;
}

 long EventThread::Run() 
 {
	 eventDispatch->StartDispatch();
	 return 0;
 }
}
