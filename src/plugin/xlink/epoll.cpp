#include "multiplexing_impl.h"

namespace x{
#ifdef OS_LINUX
MultiPlexing::MultiPlexing(void)
{
	epfd_ = epoll_create(1024);
}


MultiPlexing::~MultiPlexing(void)
{

	close(epfd_);

}

int MultiPlexing:: poll(int timeoutMs){
	int nfds = 0;
	nfds = epoll_wait(epfd_, eventsSet, 1024, timeoutMs);
	if (nfds == 0) {
		return nfds;
	}
	for (int i = 0; i < nfds; i++) {
		int fd = eventsSet[i].data.fd;
		
		 EventItem* item = FindEvent(fd);
		 //if (((eventsSet[i].events & EPOLLHUP) && !(eventsSet[i].events & EPOLLIN))
		//	 ||(eventsSet[i].events &  EPOLLERR ))
		 if (eventsSet[i].events &  EPOLLERR)
		 {	
			// printf("poll::OnClose--state_:[%d]",eventsSet[i].events);
			  item->callback->OnClose();
		 }

		if (eventsSet[i].events & (EPOLLIN| EPOLLPRI | EPOLLRDHUP)) {
			//printf("poll::OnRead--state_:[%d]",eventsSet[i].events);
			item->callback->OnRead();
		}
		if (eventsSet[i].events & EPOLLOUT) {
			item->callback->OnWrite();
		}
		

	}
	return nfds;
}

EventItem* MultiPlexing:: FindEvent(SOCKET fd){
	EventMap::iterator it = events_.find(fd);
	if (it != events_.end()) {
		return it->second;
	}
	return NULL;
 }
void MultiPlexing::addChannel(SOCKET fd,IOEventCallback* channel){
	 ScopedLock lock(mutex_);
	 {
		EventItem* item = FindEvent(fd);
		if (item == NULL) {
			item = new EventItem();

		}
		item->fd = fd;
		item->io_event = io_event_none;
		events_[fd] = item;
		item->callback = channel;

	}
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP;//| EPOLLET
	ev.data.fd = fd;
	if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) != 0) {
	}
	
}


void MultiPlexing::removeChannel(SOCKET fd){
	 ScopedLock lock(mutex_);
	 {
		 EventMap::iterator it = events_.find(fd);
		 if (it != events_.end()) {
			 delete it->second;
			 events_.erase(it);
			 if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) != 0) {
			 }
			
		 }
	 }
}
#endif
}
