
#include "multiplexing_impl.h"

//
namespace x{

#ifdef OS_WIN
MultiPlexing::MultiPlexing(void)
{
	FD_ZERO(&readSet_);
	FD_ZERO(&writeSet_);
	FD_ZERO(&excepSet_);
}

MultiPlexing::~MultiPlexing(void)
{
}

 int MultiPlexing:: poll(int timeoutMs){

	 fd_set readSet, writeSet, excepSet;
	 timeval timeout;
	 timeout.tv_sec = 0;
	 timeout.tv_usec = timeoutMs;

	 if (!readSet_.fd_count && !writeSet_.fd_count && !excepSet_.fd_count) {
		return SOCKET_ERROR;
	 }

	 memcpy(&readSet, &readSet_, sizeof(fd_set));
	 memcpy(&writeSet, &writeSet_, sizeof(fd_set));
	 memcpy(&excepSet, &excepSet_, sizeof(fd_set));

	 int nfds = select(0, &readSet, &writeSet, &excepSet, &timeout);
	 if (nfds == SOCKET_ERROR) {
		 return nfds;
	 }


	 if (nfds == 0) {
		return nfds;
	 }

	 for (uint32_t i = 0; i < readSet.fd_count; i++) {
		 EventItem* item = FindEvent(readSet.fd_array[i]);
		 if (item && item->callback){
			// activeChannels->push_back(item);
			// item->io_event = io_event_read;
			 item->callback->OnRead();
		 }
	 }

	 for (uint32_t i = 0; i < writeSet.fd_count; i++) {
		 EventItem* item = FindEvent(writeSet.fd_array[i]);
		 if (item && item->callback){
			// activeChannels->push_back(item);
			// item->io_event = io_event_write;
			 item->callback->OnWrite();
		 }
	 }

	 for (uint32_t i = 0; i < excepSet.fd_count; i++) {
		 EventItem* item = FindEvent(excepSet.fd_array[i]);
		 if (item && item->callback){
			 //activeChannels->push_back(item);
			 //item->io_event = io_event_close;
			 item->callback->OnClose();
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

		 FD_SET(fd, &readSet_);
		 FD_SET(fd, &writeSet_);
		 FD_SET(fd, &excepSet_);
	 }
	 //read
	 //{
		// EventItem* item = FindReadEvent(fd);
		// if (item == NULL) {
		//	 item = new EventItem();
		// }
		// item->fd = fd;
		// item->io_event = io_event_none;
		// readevents_[fd] = item;
		// item->callback = channel;
		// 
	 //}
	 ////write
	 //{
		// EventItem* item = FindWriteEvent(fd);
		// if (item == NULL) {
		//	 item = new EventItem();

		// }
		// item->fd = fd;
		// item->io_event = io_event_none;
		// writeevents_[fd] = item;
		// item->callback = channel;
	 //}
	 ////erro
	 //{
		// EventItem* item = FindErrorEvent(fd);
		// if (item == NULL) {
		//	 item = new EventItem();

		// }
		// item->fd = fd;
		// item->io_event = io_event_none;
		// erroevents_[fd] = item;
		// item->callback = channel;
	 //}
	

	///* FD_SET(fd, &readSet_);
	// FD_SET(fd, &writeSet_);
	// FD_SET(fd, &excepSet_);*/
 }


 void MultiPlexing::removeChannel(SOCKET fd){

	 ScopedLock lock(mutex_);
	 {
		 EventMap::iterator it = events_.find(fd);
		 if (it != events_.end()) {
			 delete it->second;
			 events_.erase(it);

			 FD_CLR(fd, &readSet_);
			 FD_CLR(fd, &writeSet_);
			 FD_CLR(fd, &excepSet_);
		 }
	 }
	 /*{
	 EventMap::iterator it = readevents_.find(fd);
	 if (it != readevents_.end()) {
	 delete it->second;
	 readevents_.erase(it);

	 FD_CLR(fd, &readSet_);

	 }
	 }
	 {
	 EventMap::iterator it = writeevents_.find(fd);
	 if (it != writeevents_.end()) {
	 delete it->second;
	 writeevents_.erase(it);

	 FD_CLR(fd, &writeSet_);

	 }
	 }
	 {
	 EventMap::iterator it = erroevents_.find(fd);
	 if (it != erroevents_.end()) {
	 delete it->second;
	 erroevents_.erase(it);
	 FD_CLR(fd, &excepSet_);
	 }
	 }*/
		  
 }

 EventItem* MultiPlexing:: FindEvent(SOCKET fd){

	 EventMap::iterator it = events_.find(fd);
	 if (it != events_.end()) {
		 return it->second;
	 }
	 return NULL;
 }
 /*EventItem* Select:: FindReadEvent(SOCKET fd){

	 EventMap::iterator it = readevents_.find(fd);
	 if (it != readevents_.end()) {
		 return it->second;
	 }
	 return NULL;

 }
 EventItem* Select:: FindWriteEvent(SOCKET fd){

	 EventMap::iterator it = writeevents_.find(fd);
	 if (it != writeevents_.end()) {
		 return it->second;
	 }
	 return NULL;

 }
 EventItem* Select:: FindErrorEvent(SOCKET fd){

	 EventMap::iterator it = erroevents_.find(fd);
	 if (it != erroevents_.end()) {
		 return it->second;
	 }
	 return NULL;

 }*/
#endif
}
