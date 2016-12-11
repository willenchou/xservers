#ifndef XBUSINESS_WORK_HTREAD_H_
#define XBUSINESS_WORK_HTREAD_H_

#include <xbase/ring_queue.h>
#include <xbase/thread.h>
#include <xbase/mutex.h>
#include <xbase/event.h>
#include <include/xmessage_intf.h>
#include "business_context.h"

namespace x{

class BusinessService;

class WorkThread : public Thread {
public:
	WorkThread(BusinessService* owner, IMessageService* msgService,IDBService* dbService_);
	~WorkThread();

	void PushMsg(IMessage* msg);
	virtual long Run();

private:
	IDBService* dbService_;
	BusinessService* busiService_;
	IMessageService* msgService_;
	BusinessContext* context_;
	RingQueue<IMessage*> queue_;
	Mutex mutex_;
	Event event_;
};


}//namespace x

#endif //XBUSINESS_WORK_HTREAD_H_
