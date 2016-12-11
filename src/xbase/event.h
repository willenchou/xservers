#ifndef XBASE_EVENT_H_
#define XBASE_EVENT_H_

#include "os_common.h"
#include "xbase_export.h"

namespace x {

//等待时间返回值定义
enum EVENTS{
	EVENT_FAILED = -1,
	EVENT_OK = 0,
	EVENT_TIMEOUT,
	EVENT_ABANDONED
};

#ifdef OS_WIN

#include <windows.h>
#include <errno.h>

class XBASE_EXPORT Event {
public:
	Event(bool status = false){
		event_ = CreateEvent(NULL, false, status, NULL); 
	}
	~Event(){
		if(event_){
			CloseHandle(event_);
		}
	}

	EVENTS Wait(long millSec){
		if(event_){
			switch(::WaitForSingleObject(event_, millSec)){
			case WAIT_ABANDONED:
				return EVENT_ABANDONED;
			case WAIT_OBJECT_0:
				return EVENT_OK;
			case WAIT_TIMEOUT:
				return EVENT_TIMEOUT;
			default:
				return EVENT_FAILED;
			}
		}
		return EVENT_FAILED;        //CEvent is invalid
	}

	bool Set(){
		if(event_){
			return ::SetEvent(event_) == 0;
		}else{
			return false;
		}
	}

	bool Reset(){
		if (event_){
			return ::ResetEvent(event_) == 0;
		}else{
			return false;
		}
	}

private:
	HANDLE event_;
};

#elif defined(OS_LINUX)

#include <pthread.h>
#include <errno.h>
class BASE_EXPORT Event {
	Event(bool status):notify_(status){
		pthread_cond_init(&cond_, NULL);
		pthread_mutex_init(&mutex_, NULL);
	}

	~Event(){
		pthread_cond_destroy(&cond_);
		pthread_mutex_destroy(&mutex_);
	}


	EVENTS Wait(long millSecond){
		int ret = EVENT_OK;
		if(millSecond == INFINITE){
			pthread_mutex_lock(&mutex_);
			while(!notify_){
				ret = pthread_cond_wait(&cond_, &mutex_);
				if(ret != 0) {
					pthread_mutex_unlock(&mutex_);
					return EVENT_FAILED;
				}
			}
			notify_ = false; //重置事件
			pthread_mutex_unlock(&mutex_);
			return EVENT_OK;
		}else{
			timespec tm;
			struct timeval now;
			gettimeofday(&now, NULL);
			tm.tv_sec = now.tv_sec + millSecond / 1000;
			tm.tv_nsec = (now.tv_usec + (millSecond % 1000) * 1000);
			if( tm.tv_nsec/(1000*1000) ) {
				tm.tv_sec++;
				tm.tv_nsec=tm.tv_nsec%(1000*1000);
			}
			tm.tv_nsec=tm.tv_nsec*1000;
		
			pthread_mutex_lock(&mutex_);
			while (notify_) {
				ret = pthread_cond_timedwait(&cond_, &mutex_, &tm);
				if(ret == ETIMEDOUT){
					pthread_mutex_unlock(&mutex_); 
					return EVENT_TIMEOUT;
				}else if( ret == 0 ) {
					continue;
				}else{
					pthread_mutex_unlock(&mutex_);
					return EVENT_FAILED;
				}
			}
			
			notify_ = false;
			pthread_mutex_unlock(&mutex_);
			return EVENT_OK;
		}
	}

	bool Set(){
		pthread_mutex_lock(&mutex_);
		notify_ = true;
		pthread_cond_signal(&cond_);
		pthread_mutex_unlock(&mutex_);
		return true;
	}

	bool Reset(){
		pthread_mutex_lock(&mutex_);
		notify_ = false;
		pthread_mutex_unlock(&mutex_);
		return true;
	}

	private:
		pthread_mutex_t		mutex_;
		pthread_cond_t		cond_;
		bool				notify_;
	};

#else
	#error Event not implement on this platform
#endif

}//namespace x

#endif //XBASE_EVENT_H_
