#include "xmessage_impl.h"
#include "pack.h"
#include "message.h"

namespace x {

MessageService* _messageService = new MessageService();

MessageService::MessageService(){

}

MessageService::~MessageService(){

}

void MessageService::Init(){

}

void MessageService::UnInit(){

}

void MessageService::Start(){

}

void MessageService::Stop(){

}

IMessage* MessageService::NewMessage(){
	return new Message();
}

IPack* MessageService::NewPack(){
	return new Pack();
}

}//namespace x