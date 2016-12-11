#include "eventthreadpool.h"
namespace x {

EventThreadPool::EventThreadPool(EventDispatch* acceptdispatch)
{
	 numThreads_ = 0;
	 curIndex_ = 0;
	 //acceptdispatch_ = acceptdispatch;
}


EventThreadPool::~EventThreadPool(void)
{
}

EventDispatch* EventThreadPool::GetNextDispatch(){
	++curIndex_;
	if (curIndex_ ==  numThreads_)
	{
		curIndex_ = 0;
	}
	
	return threads_[curIndex_]->GetDispatch();
	
}
void EventThreadPool::setThreadNum(int numThreads)
{
	numThreads_ = numThreads;
	maxconnNum_ = numThreads_ *(FD_CONNSIZE-1) ;
}

int EventThreadPool::start()
{
	EventThread* et = NULL;
	for (int i = 0; i < numThreads_; i++)
	{
		et = new EventThread();
		threads_.push_back(et);
		et->Start();
	}

	return 0;
}
}
