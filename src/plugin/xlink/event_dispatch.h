#ifndef XCONNECTION_EVENT_DISPATHCH_H_
#define XCONNECTION_EVENT_DISPATHCH_H_

#include <list>
#include "xbase/os_common.h"
#include "xbase/mutex.h"
#include "multiplexing.h"

#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif

namespace x {

class EventDispatch {
public:
	EventDispatch();
	~EventDispatch();

	void AddEvent(SOCKET fd,IOEventCallback* callback);
	void RemoveEvent(SOCKET fd);
	
	int AddTimer(uint64_t interval, IOEventCallback* callback);
	void RemoveTimer(int id);

	void StartDispatch();
public:
	static EventDispatch* Instance();

private:
	

	struct TimerItem {
		int id;
		IOEventCallback* callback;
		uint64_t interval;
		uint64_t nextTick;
	};

	//typedef hash_map<SOCKET, EventItem*> EventMap;
	typedef hash_map<int, TimerItem*> TimerMap;

	EventItem* FindEvent(SOCKET fd);
	TimerItem* FindTimer(int id);

	void CheckTimer();


//#elif OS_APPLE
//	int kqfd_;
//#else
//	int epfd_;
//#endif
	Mutex mutex_;
	
	EventItem* curActiveEvent_;
	IMultiPlexing* multiplexing_;
	bool isdispatch_;
	TimerMap timers_;
	int currTimerId_;
};

extern EventDispatch* _eventDispatch;

}//namespace x

#endif //XCONNECTION_EVENT_DISPATHCH_H_
