#ifndef XBASE_MUTEX_H_
#define XBASE_MUTEX_H_

#include "os_common.h"
#include "xbase_export.h"

namespace x {

#ifdef OS_WIN

#include <windows.h>

class XBASE_EXPORT Mutex {
public:
	inline Mutex(){
		InitializeCriticalSection(&cs_);
	}

	inline ~Mutex(){
		DeleteCriticalSection(&cs_);
	}

	inline void Lock(){
		EnterCriticalSection(&cs_);
	}

	inline bool TryLock(){
		return (TryEnterCriticalSection(&cs_)) ? true : false;
	}

	inline void UnLock(){
		LeaveCriticalSection(&cs_);
	}

private:
	CRITICAL_SECTION cs_;
};

#elif defined(OS_LINUX)
	
#include <pthread.h>

class XBASE_EXPORT Mutex {
public:
	inline Mutex(){
		pthread_mutex_init(&mt_, NULL);
	}

	inline ~Mutex(){
		pthread_mutex_destroy(&mt_);
	}

	inline void Lock(){
		pthread_mutex_lock (&mt_);
	}

	inline bool TryLock(){
		return pthread_mutex_trylock(&mt_) == 0;
	}

	inline void UnLock(){
		pthread_mutex_unlock(&mt_);
	}

private:
	pthread_mutex_t mt_;
};

#else
	#error Mutex not implement on this platform
#endif

struct ScopedLock{
	ScopedLock(Mutex& mutex)
	:mutex_(mutex){
		mutex.Lock();
	}

	~ScopedLock(){
		mutex_.UnLock();
	}

	Mutex& mutex_;
};

}//namespace x

#endif //XBASE_MUTEX_H_
