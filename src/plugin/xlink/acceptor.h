#ifndef XLINK_ACCEPTOR_H_
#define XLINK_ACCEPTOR_H_
#include "event_dispatch.h"
#include "channel.h"
#include "connection.h"
namespace x{


class Acceptor:public Connection::Callback{
public:
	class AcceptorCallback {
	public:
		virtual void OnAccept(Connection* conn){}
		//virtual void OnAcceptorClose(Connection* conn){}
	};

public:
	Acceptor(EventDispatch* eventdispatch);
	~Acceptor(void);
public:
	void SetCallBack(AcceptorCallback*  callback);
	bool Listen(const char* ip, uint16_t port);
protected:
	virtual void OnAccept(Connection* conn,Socket* socket);
	virtual void OnConfirm(Connection* conn) ;
	virtual void OnRecv(Connection* conn, IMessage* msg);
	virtual void OnClose(Connection* conn);
private:
	Connection* newConnection(Socket* socket);
private:
	EventDispatch* disPatch_;
	//Socket acceptSocket_;
	Connection* acceptChannel_;
	AcceptorCallback* newConnectionCallback_;
	bool listenning_;
	int idleFd_;

};

}
#endif//XLINK_ACCEPTOR_H_