#include "thread.h"

namespace x{
 void SleepX(uint32_t millsec){
#ifdef OS_WIN
	 Sleep(millsec);
#else
	 usleep(millsec * 1000);
#endif
 }
#ifdef OS_WIN
	#include <process.h>
	static unsigned WINAPI ThreadFunc(void* pArg){
		return ((Thread*)pArg)->Run();
	}
#else
	void* ThreadFunc(void *pArg){
		return (void *)((Thread *)pArg)->Run();
	}
#endif

Thread::Thread():
	thread_(NULL),
	isStarted_(false),
	isStoping_(false){
}

Thread::~Thread(){
#ifdef OS_WIN
	if(thread_ != 0){
		CloseHandle(thread_);
	}
#elif defined(OS_LINUX)
#else
	#error Thread not implement on this platform
#endif
}

void Thread::SleepM(uint32_t millsec) {
	SleepX(millsec);
}

int Thread::Start(){
#ifdef OS_WIN
	unsigned nThreadID;
	thread_ = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0 , &nThreadID );
	if(thread_){
		isStarted_ = true;
		return 0;
	}
	return ::GetLastError();
#elif defined(OS_LINUX)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 1024 * 10);
	int ret = pthread_create(&thread_, &attr, ThreadFunc, (void *)this);
	pthread_attr_destroy(&attr);
	if(ret){
		return ret;
	}
	isStarted_ = true;
	return ret;
#else
	#error Thread not implement on this platform
#endif
}

void Thread::Stop(){
	StopSoon();	
	Join();
}

void Thread::StopSoon(){
	if(!isStarted_)
		return;

	isStoping_ = true;
}

void Thread::Join(){
	if(thread_){
#ifdef OS_WIN
		WaitForSingleObject(thread_, INFINITE);
		CloseHandle(thread_);
		thread_ = NULL;
#elif defined(OS_LINUX)
		pthread_join(thread_, NULL);
		thread_ = NULL;
#else
	#error Thread not implement on this platform
#endif
	}
}

}//namespace x