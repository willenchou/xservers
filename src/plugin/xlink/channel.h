#ifndef XLINK_CHANNEL_H_
#define XLINK_CHANNEL_H_
#include "event_dispatch.h"
#include "xbase/os_common.h"

namespace x {

	//ICHANNELCALLBACK
class ChannelCallback {
public:
		virtual void OnAccept(){}
		//virtual void OnConnect(SOCKET* socket) {}
		virtual void OnConfirm() {}
		virtual void OnRead() {}
		virtual void OnWrite() {}
		virtual void OnClose() {}
};

enum Channel_STATE {
	Channel_STATE_IDLE,
	Channel_STATE_LISTENING,
	Channel_STATE_CONNECTING,
	Channel_STATE_CONNECTED,
	Channel_STATE_CLOSING
};
class Channel:public IOEventCallback
{

public:
	Channel(SOCKET fd,EventDispatch* eventdispatch);
	~Channel(void);

public:
	void SetCallBack(ChannelCallback* callback);
	void SetState(Channel_STATE state);
public:
	static Channel* NewChannel();
protected:
    
	//EventDispatch::Callback
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose();
	virtual void OnTimer(int id);
private:
	ChannelCallback* callback_;
	//Socket* socket_;
	SOCKET socket_;
	Channel_STATE state_;
	EventDispatch* ownerEventDispatch_;

};
}
#endif//XLINK_CHANNEL_H_
