#pragma once
#include "socket.h"
#include "eventthread.h"
#include "event_dispatch.h"
namespace x {
//连接事件线程池
class EventThreadPool
{
public:
	EventThreadPool(EventDispatch* acceptdispatch);
	~EventThreadPool(void);

public:
	EventDispatch* GetNextDispatch();
	void setThreadNum(int numThreads_);
	int start();

	//
private:
	//EventDispatch* acceptdispatch_;
	typedef hash_map<SOCKET, EventThread*> EventThreadMap;
	EventThreadMap threadsMaps;

    std::vector<EventThread*> threads_;	
	int numThreads_;
	int maxconnNum_;
	int curIndex_;
};
}

