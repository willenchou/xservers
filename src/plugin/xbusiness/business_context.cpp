#include "business_context.h"

namespace x {

BusinessContext::BusinessContext(IMessageService* msgService,IDBService* dbservice)
:msgService_(msgService)
,dbService_(dbservice)
,inPack_(NULL)
,outPack_(NULL){
	inPack_ = msgService_->NewPack();
	outPack_ = msgService_->NewPack();

}

BusinessContext::~BusinessContext(){
	if(inPack_) inPack_->Release();
	if(outPack_) outPack_->Release();
}

}//namespace x
