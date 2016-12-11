#ifndef XMESSAGE_XMESSAGE_SERVICE_H_
#define XMESSAGE_XMESSAGE_SERVICE_H_

#include "xmessage_intf.h"

namespace x{

class MessageService : public RefCountedThreadSafe<MessageService, IService> {
public:
	MessageService();
	virtual ~MessageService();

	virtual IMessage* NewMessage();

};

extern MessageService* _messageService;

}//namespace x


#endif //XMESSAGE_XMESSAGE_SERVICE_H_
