#include "linkfactory.h"
#include "linkclient.h"
#include "message.h"
#include "pack.h"


namespace x{
LinkFactory::LinkFactory(void)
{
}


LinkFactory::~LinkFactory(void)
{
}



IMessage* LinkFactory::NewMessage(){
	return new Message();
	//return NULL;
}

IPack* LinkFactory::NewPack(){
	return new Pack();
	//return NULL;
}

int LinkFactory::Init(LinkClientArgs& args){


	threadpool_ = new EventThreadPool(NULL);
	threadpool_->setThreadNum(args.threadNum);
	
	threadpool_->start();
	//Á¬½Ó³Ø£¬

	return 0;
};

int LinkFactory::UnInit(){


	return 0;
}

ILinkClient* LinkFactory::NewLinkClient(LinkClientCallback* callback){
	
	Linker* link = new Linker(threadpool_->GetNextDispatch());
	link->SetCallBack(callback);

	return link;
}
XLINK_EXPORT ILinkClientApi* NewLinkClientAPI(){
	return new LinkFactory();
}
	
}