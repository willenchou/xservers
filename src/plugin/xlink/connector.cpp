#include "connector.h"
namespace x {
	Connector::Connector():callback_(NULL)
	{
		status = 0;
	}


	Connector::~Connector(void)
	{
	}
	bool Connector::Connect(const char* ip, uint16_t port){
	    return	connChannel->Connect(ip,port);
	}
	int Connector::Send(IMessage* msg){
		return connChannel->Send(msg);
	}
	void Connector::OnAccept(Connection* conn,Socket* socket) {
		return;//NULL
	}

	void Connector::OnConfirm(Connection* conn){
		if (conn != connChannel){
			return;
		}
		if (callback_)callback_->OnConnected(this);
	}
	void Connector::OnRecv(Connection* conn, IMessage* msg){
		if (callback_)callback_->OnRecv(this,msg);
		
	}
	void Connector::OnClose(Connection* conn){
		if (callback_)
		{
			callback_->OnClose(this);
		}
	}
}