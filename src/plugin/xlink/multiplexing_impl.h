#pragma once
#include "multiplexing.h"
#include "xbase/mutex.h"
#include "xbase/os_common.h"

#ifdef OS_WIN
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

namespace x{
	class MultiPlexing :public IMultiPlexing
	{
	public:
		MultiPlexing(void);
		~MultiPlexing(void);

		virtual int poll(int timeoutMs);


		virtual void addChannel(SOCKET fd,IOEventCallback* channel);


		virtual void removeChannel(SOCKET fd);
	private:
		EventItem* FindEvent(SOCKET fd);
		//EventItem* FindReadEvent(SOCKET fd);
		//EventItem* FindWriteEvent(SOCKET fd);
		//EventItem* FindErrorEvent(SOCKET fd);

	private:
		typedef std::map<SOCKET, EventItem*> EventMap;

		EventMap events_;
		Mutex mutex_;

#ifdef OS_WIN
		//select
		fd_set readSet_;
		fd_set writeSet_;
		fd_set excepSet_;
#elif OS_APPLE
		int kqfd_;
		struct kevent eventsSet[1024];
#else
		int epfd_;
		struct epoll_event eventsSet[1024];
#endif
		
	};
}


