#ifndef XCONNECTION_XMESSAGE_IMPL_H_
#define XCONNECTION_XMESSAGE_IMPL_H_

#include <include/xmessage_intf.h>

namespace x {

class MessageService : public RefCountedThreadSafe<MessageService, IMessageService> {
public:
	MessageService();
	virtual ~MessageService();

	void Init();
	void UnInit();
	void Start();
	void Stop();

	virtual IMessage* NewMessage();
	virtual IPack* NewPack();
};

extern MessageService* _messageService;

}//namespac x


#endif //XCONNECTION_XMESSAGE_IMPL_H_
