#include "multiplexing_impl.h"

//
namespace x{

#ifdef OS_APPLE
MultiPlexing::MultiPlexing(void)
{
	kqfd_ = kqueue();
	if (kqfd_ == -1) {
		//
	}
}

MultiPlexing::~MultiPlexing(void)
{
	close(kqfd_);
}

 int MultiPlexing:: poll(int timeoutMs){
	 int nfds = 0;
	 nfds = kevent(kqfd_, NULL, 0, events, 1024, &timeout);
	 if (nfds == 0) {
		 return nfds;
	 }
	 for (int i = 0; i < nfds; i++) {
		 
		 int fd = eventsSet[i].ident;
		 EventItem* item = FindEvent(fd);

		 if (eventsSet[i].filter == EVFILT_READ) {
			 item->callback->OnRead();
		 }
		 if (eventsSet[i].filter == EVFILT_WRITE) {
			 item->callback->OnWrite();
		 }

	 }
	 return nfds;
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

		 struct kevent ke;
		
		EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
		 

		 
		EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		kevent(kqfd_, &ke, 1, NULL, 0, NULL);
		 
	 }
	
 }


 void MultiPlexing::removeChannel(SOCKET fd){

	 ScopedLock lock(mutex_);
	 {
		 EventMap::iterator it = events_.find(fd);
		 if (it != events_.end()) {
			 delete it->second;
			 events_.erase(it);

			 struct kevent ke;

			 
			 EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			 kevent(kqfd_, &ke, 1, NULL, 0, NULL);
			 
			 EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			 kevent(kqfd_, &ke, 1, NULL, 0, NULL);
		 }
	 }	  
 }

 EventItem* MultiPlexing:: FindEvent(SOCKET fd){

	 EventMap::iterator it = events_.find(fd);
	 if (it != events_.end()) {
		 return it->second;
	 }
	 return NULL;
 }
 
#endif
}