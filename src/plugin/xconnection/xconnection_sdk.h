#ifndef XCONNECTION_XCONNECTION_SDK_H_
#define XCONNECTION_XCONNECTION_SDK_H_

#include <xbase/os_common.h>
#include <xbase/mutex.h>
#include <xbase/event.h>
#include <xbase/thread.h>
#include "connection.h"
#include "event_dispatch.h"
#include "xconnection_common.h"

namespace x {

class ConnectionSDK : public Thread
					, public ConnectionApi
					, public Connection::Callback
{
public:
	ConnectionSDK();
	~ConnectionSDK();

	virtual long Run();

	virtual IMessage* NewMessage();
	virtual IPack* NewPack();
	virtual int Init(ConnectionCallback* callback);
	virtual int UnInit();
	virtual int Connect(const char* ip, uint16_t port);
	virtual int Send(IMessage* msg);
	virtual int SendSync(IMessage* reqMsg, IMessage* ansMsg, uint32_t timeout = 5000);
	
	virtual void OnConnect(Connection* conn, Socket* socket);
	virtual void OnConfirm(Connection* conn);
	virtual void OnRecv(Connection* conn, IMessage* msg);
	virtual void OnClose(Connection* conn);

private:
	ConnectionCallback* callback_;
	Connection* connection_;
	EventDispatch* dispatch_;

	int status_;
};

}//namespace x



#endif //XCONNECTION_XCONNECTION_SDK_H_
