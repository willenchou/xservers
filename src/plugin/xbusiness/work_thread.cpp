#include "work_thread.h"
#include "xbusiness.h"

namespace x {

WorkThread::WorkThread(BusinessService* busiService, IMessageService* msgService,IDBService* dbService)
:busiService_(busiService)
,msgService_(msgService)
,dbService_(dbService)
,context_(NULL){
	context_ = new BusinessContext(msgService_,dbService_);
}

WorkThread::~WorkThread(){

}

void WorkThread::PushMsg(IMessage* msg){
	void* data = NULL;
	uint32_t dataLen = 0;
	if(msg->GetBuff(&data, &dataLen)){	
		IMessage* copyMsg = busiService_->GetMessageService()->NewMessage();
		copyMsg->SetBuff(data, dataLen);

		ScopedLock lock(mutex_);
		IMessage** item = queue_.Bottom();
		*item = copyMsg;
		queue_.Push();

		event_.Set();
	}
}

long WorkThread::Run(){
	while(!IsStoping()){
		{
			ScopedLock lock(mutex_);
			IMessage** item = queue_.Top();
			if(item){
				IMessage* msg = *item;
				if(msg){
					busiService_->DealMessage(context_, msg);
					msg->Release();
				}
				
				queue_.Pop();

				continue;
			}
		}
			
		event_.Wait(10);
	}

	return 0L;
}


}//namespace x
