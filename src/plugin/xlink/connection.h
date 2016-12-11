#ifndef XCONNECTION_CONNECTION_H_
#define XCONNECTION_CONNECTION_H_

//#include <include/xmessage_intf.h>
//#include <include/xconnection_intf.h>
#include <include/xlink_intf.h>
#include "socket.h"
#include "channel.h"

namespace x {

class ConnectionBuff;
class Connection : public ChannelCallback{
public:
	class Callback {
	public:
		virtual void OnAccept(Connection* conn,Socket* socket) {}
		virtual void OnConfirm(Connection* conn) {}
		virtual void OnRecv(Connection* conn, IMessage* msg) {}
		virtual void OnClose(Connection* conn) {}
	};
	
	Connection();
	~Connection();
	
	void Init(EventDispatch* eventdispatch);

	void SetCallback(Callback* callback) { callback_ = callback; }
	Socket* GetSocket() { return socket_; }
	void SetSocket(Socket* socket) { socket_ = socket; /*if(socket_) socket_->SetCallback(this);*/}

	Channel* GetChannel() { return channel_; }
	void SetChannel(Channel* channel) { channel_ = channel; if(channel_) channel_->SetCallBack(this);}
	
	bool Listen(const char* ip, uint16_t port);
	bool Connect(const char* ip, uint16_t port);
	int Send(IMessage* msg);
	void Close();

protected:
	virtual void OnAccept();
	virtual void OnConfirm();
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose();
private:
	void acceptSocket(SOCKET fd);
private:
	struct MsgBuffHead {
		int32_t		flag;
		uint32_t	tagCount;
		uint32_t	len;
	};

	Socket* socket_;
	Callback* callback_;
	Channel* channel_;
	ConnectionBuff* sendBuff_;
	ConnectionBuff* recvBuff_;

	IMessage* callbackMsg_;
	EventDispatch* eventDispatch_;
};

class ConnectionBuff {
public:
	ConnectionBuff(uint32_t capacity = 1024)
	:buff_(NULL)
	,capacity_(0)
	,len_(0) {
		Resize(capacity > 0 ? capacity : 1024);
	}
	~ConnectionBuff() {
	}

	inline void* GetData() const {return buff_; }
	inline uint32_t GetLen() const { return len_;}
	inline void Push(void* data, uint32_t len) {
		if (len_ + len > capacity_) {
			Resize(capacity_ * 2> len_ + len ? capacity_ * 2 : len_ + len);
		}
		memcpy(buff_+len_, data, len);
		len_ += len;
	}
	inline void Pop(uint32_t len) {
		if (len > len_) len = len_;
		if (len_ - len > 0) {
			for (uint32_t i = 0; i < len_ - len; i++) {
				buff_[i] = buff_[len+i];
			}
		}
		len_ = len_ - len;
	}

private:
	inline void Resize(uint32_t capacity) {
		if (capacity <= capacity_) return;
		char* newBuff = new char[capacity];
		if (buff_){
			if (len_ > 0) {
				memcpy(newBuff, buff_, len_);
			}
			delete [] buff_;
		}
		buff_ = newBuff;
		capacity_ = capacity;
	}

	char* buff_;
	uint32_t capacity_;
	uint32_t len_;
};

}//namespace x

#endif //XCONNECTION_CONNECTION_H_
