#include "event_dispatch.h"
#include <xbase/util.h>
#include <xbase/thread.h>

namespace x {

#define MIN_TIMER_DURATION 100

EventDispatch* _eventDispatch = new EventDispatch();

EventDispatch::EventDispatch()
:currTimerId_(0){
#ifdef OS_WIN
	FD_ZERO(&readSet_);
	FD_ZERO(&writeSet_);
	FD_ZERO(&excepSet_);
#elif OS_APPLE
	kqfd_ = kqueue();
	if (kqfd_ == -1) {
		//
	}
#else
	epfd_ = epoll_create(1024);
	if (epfd_ == -1) {
		//
	}
#endif
}

EventDispatch::~EventDispatch() {
#ifdef _WIN32

#elif __APPLE__
	close(kqfd_);
#else
	close(epfd_);
#endif
}

EventDispatch* EventDispatch::Instance() {
	return _eventDispatch;
}

int EventDispatch::AddTimer(uint64_t interval, Callback* callback){
	ScopedLock lock(mutex_);

	TimerItem* item = new TimerItem();
	item->id = currTimerId_++;
	item->callback = callback;
	item->interval = interval;
	item->nextTick = Util::TimeTickCount() + interval;
	
	timers_[item->id] = item;

	return item->id;

}

void EventDispatch::RemoveTimer(int id){
	ScopedLock lock(mutex_);

	for (TimerMap::iterator it = timers_.begin(); it != timers_.end(); it++) {
		TimerItem* item = it->second;
		if (item->id == id) {
			timers_.erase(it);
			delete item;
			return;
		}
	}
}

EventDispatch::EventItem* EventDispatch::FindEvent(SOCKET fd) {
	EventMap::iterator it = events_.find(fd);
	if (it != events_.end()) {
		return it->second;
	}
	return NULL;
}

EventDispatch::TimerItem* EventDispatch::FindTimer(int id){
	TimerMap::iterator it = timers_.find(id);
	if (it != timers_.end()) {
		return it->second;
	}
	return NULL;
}

void EventDispatch::CheckTimer(){
	uint64_t currTick = Util::TimeTickCount();

	for (TimerMap::iterator it = timers_.begin(); it != timers_.end(); it++) {
		TimerItem* item = it->second;
		if (currTick >= item->nextTick) {
			item->nextTick += item->interval;
			item->callback->OnTimer(item->id);
		}
	}
}

#ifdef OS_WIN

void EventDispatch::AddEvent(SOCKET fd, Callback* callback) {
	ScopedLock lock(mutex_);

	EventItem* item = FindEvent(fd);
	if (item == NULL) {
		item = new EventItem();
		item->fd = fd;
		events_[fd] = item;
	}
	item->callback = callback;

	
	FD_SET(fd, &readSet_);
	FD_SET(fd, &writeSet_);
	FD_SET(fd, &excepSet_);
}

void EventDispatch::RemoveEvent(SOCKET fd) {
	ScopedLock lock(mutex_);

	EventMap::iterator it = events_.find(fd);
	if (it != events_.end()) {
		delete it->second;
		events_.erase(it);

		FD_CLR(fd, &readSet_);
		FD_CLR(fd, &writeSet_);
		FD_CLR(fd, &excepSet_);
	}
}

void EventDispatch::StartDispatch() {
	fd_set readSet, writeSet, excepSet;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = MIN_TIMER_DURATION;	// 10 millisecond

	while (true) {
		
		mutex_.Lock();
		CheckTimer();
		mutex_.UnLock();

		if (!readSet_.fd_count && !writeSet_.fd_count && !excepSet_.fd_count) {
			x::SleepX(MIN_TIMER_DURATION);
		}

		mutex_.Lock();
		memcpy(&readSet, &readSet_, sizeof(fd_set));
		memcpy(&writeSet, &writeSet_, sizeof(fd_set));
		memcpy(&excepSet, &excepSet_, sizeof(fd_set));
		mutex_.UnLock();

		int nfds = select(0, &readSet, &writeSet, &excepSet, &timeout);
		if (nfds == SOCKET_ERROR) {
			x::SleepX(MIN_TIMER_DURATION);
			continue;			// select again
		}

		if (nfds == 0) {
			continue;
		}
	
		for (uint32_t i = 0; i < readSet.fd_count; i++) {
			EventItem* item = FindEvent(readSet.fd_array[i]);
			if (item && item->callback)
				item->callback->OnRead();
		}

		for (uint32_t i = 0; i < writeSet.fd_count; i++) {
			EventItem* item = FindEvent(writeSet.fd_array[i]);
			if (item && item->callback)
				item->callback->OnWrite();
		}

		for (uint32_t i = 0; i < excepSet.fd_count; i++) {
			EventItem* item = FindEvent(excepSet.fd_array[i]);
			if (item && item->callback)
				item->callback->OnClose();
		}
	}
}

#elif OS_APPLE

void EventDispatch::AddEvent(SOCKET fd, int socketEvent) {
	struct kevent ke;

	if ((socketEvent & SOCKET_EVENT_READ) != 0) {
		EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
	}

	if ((socketEvent & SOCKET_EVENT_WRITE) != 0) {
		EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
	}
}

void EventDispatch::RemoveEvent(SOCKET fd, int socketEvent) {
	struct kevent ke;

	if ((socketEvent & SOCKET_EVENT_READ) != 0) {
		EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
	}

	if ((socketEvent & SOCKET_EVENT_WRITE) != 0) {
		EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
	}
}

void EventDispatch::StartDispatch() {
	struct kevent events[1024];
	int nfds = 0;
	struct timespec timeout;
	timeout.tvSec = 0;
	timeout.tvNsec = MIN_TIMER_DURATION * 1000000;

	SocketFactory* socketFactory = SocketFactory::GetInstance();

	while (true) {	

		CheckTimer();

		nfds = kevent(kqfd_, NULL, 0, events, 1024, &timeout);
		for (int i = 0; i < nfds; i++) {
			int fd = events[i].ident;
			Socket* socket = socketFactory->FindSocket((SocketHandle)fd);
			if (!socket)
				continue;
			
			socket->AddRef();
			if (events[i].filter == EVFILT_READ) {
				socket->OnRead();
			}
			if (events[i].filter == EVFILT_WRITE) {
				socket->OnWrite();
			}
			socket->Release();
		}
	}
}

#else

void EventDispatch::AddEvent(SOCKET fd, int socketEvent) {
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP;
	ev.data.fd = fd;
	if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) != 0) {
	}
}

void EventDispatch::RemoveEvent(SOCKET fd, SOCKET_EVENT socketEvent) {
	if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) != 0) {
	}
}

void EventDispatch::StartDispatch() {
	struct epoll_event events[1024];
	int nfds = 0;
	
	SocketFactory* socketFactory = SocketFactory::GetInstance();

	while (true) {

		CheckTimer();

		nfds = epoll_wait(epfd_, events, 1024, MIN_TIMER_DURATION);
		for (int i = 0; i < nfds; i++) {
			int fd = events[i].data.fd;
			Socket* socket = socketFactory->FindSocket((SocketHandle)fd);
			if (!socket)
				continue;

			socket->AddRef();
			if (events[i].events & EPOLLIN) {
				socket->OnRead();
			}
			if (events[i].events & EPOLLOUT) {
				socket->OnWrite();
			}
			if (events[i].events & (EPOLLPRI | EPOLLERR | EPOLLHUP)) {
				socket->OnClose();
			}
			socket->Release();
		}
	}
}

#endif

}//namespace x
