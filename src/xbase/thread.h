#ifndef XBASE_THREAD_H_
#define XBASE_THREAD_H_

#include "os_common.h"
#include "xbase_export.h"

#ifdef OS_WIN
	#include <windows.h>
	#define THREAD HANDLE
#elif defined(OS_LINUX)
	#include <pthread.h>
	#define THREAD pthread_t
#else
	#error Thread not implement on this platform
#endif

namespace x{

 XBASE_EXPORT void SleepX(uint32_t millsec);
class XBASE_EXPORT Thread {
public:
	Thread();
	virtual ~Thread();

	static void SleepM(uint32_t millsec);

	int Start();
	void Stop();
	void StopSoon();
	
	void Join();

	inline THREAD Handle() const { return thread_;}
	inline bool IsRuning() const { return isStarted_; }
	inline bool IsStoping() const { return isStoping_; }

	virtual long Run() = 0;
	
private:
	THREAD thread_;
	bool isStarted_;
	bool isStoping_;
};

}//namespace x

#endif //XBASE_THREAD_H_
