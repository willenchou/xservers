#ifndef XCONNECTION_EVENT_DISPATHCH_H_
#define XCONNECTION_EVENT_DISPATHCH_H_

#include <list>
#include <xbase/os_common.h>
#include <xbase/mutex.h>

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

	struct Callback {
		virtual void OnRead() {}
		virtual void OnWrite() {}
		virtual void OnClose() {}
		virtual void OnTimer(int id) {}
	};

	static EventDispatch* Instance();

	void AddEvent(SOCKET fd, Callback* callback);
	void RemoveEvent(SOCKET fd);
	
	int AddTimer(uint64_t interval, Callback* callback);
	void RemoveTimer(int id);

	void StartDispatch();

private:
	struct EventItem {
		SOCKET fd;
		Callback* callback;
	};

	struct TimerItem {
		int id;
		Callback* callback;
		uint64_t interval;
		uint64_t nextTick;
	};

	typedef hash_map<SOCKET, EventItem*> EventMap;
	typedef hash_map<int, TimerItem*> TimerMap;

	EventItem* FindEvent(SOCKET fd);
	TimerItem* FindTimer(int id);

	void CheckTimer();

#ifdef OS_WIN
	fd_set readSet_;
	fd_set writeSet_;
	fd_set excepSet_;
#elif OS_APPLE
	int kqfd_;
#else
	int epfd_;
#endif
	Mutex mutex_;
	EventMap events_;
	TimerMap timers_;
	int currTimerId_;
};

extern EventDispatch* _eventDispatch;

}//namespace x

#endif //XCONNECTION_EVENT_DISPATHCH_H_
