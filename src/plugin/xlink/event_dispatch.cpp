#include "event_dispatch.h"
#include <xbase/util.h>
#include <xbase/thread.h>

namespace x {

#define MIN_TIMER_DURATION 10

EventDispatch* _eventDispatch = new EventDispatch();

EventDispatch::EventDispatch()
:currTimerId_(0){

  multiplexing_ = IMultiPlexing::newMultiPlexing();
}

EventDispatch::~EventDispatch() {

}

EventDispatch* EventDispatch::Instance() {
	return _eventDispatch;
}

int EventDispatch::AddTimer(uint64_t interval, IOEventCallback* callback){
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


void EventDispatch::AddEvent(SOCKET fd, IOEventCallback* callback) {
	multiplexing_->addChannel(fd,callback);
}

void EventDispatch::RemoveEvent(SOCKET fd) {
	
	multiplexing_->removeChannel(fd);
}

void EventDispatch::StartDispatch() {
	
	while (true) {
		
		CheckTimer(); //not to do long loop  task,TODO

		int nEvents =multiplexing_->poll(MIN_TIMER_DURATION);
		
		if (nEvents == SOCKET_ERROR) {
			x::SleepX(MIN_TIMER_DURATION);
			continue;			
		}	
		if (nEvents == 0) {
			continue;
		}
	
	}
}

}//namespace x
