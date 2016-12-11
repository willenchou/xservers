#include "xmessage_service.h"
#include "message.h"

namespace x {

MessageService* _messageService = new MessageService();

MessageService::MessageService(){

}

MessageService::~MessageService(){

}

IMessage* MessageService::NewMessage(){
	return new (std::nothrow) Message();
}


}//namespace x