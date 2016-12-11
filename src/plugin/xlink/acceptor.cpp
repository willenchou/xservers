#include "acceptor.h"
namespace x{
	Acceptor::Acceptor(EventDispatch* eventdispatch):newConnectionCallback_(NULL)
	{
		disPatch_ = eventdispatch;
		acceptChannel_ = NULL;
	}


	Acceptor::~Acceptor(void)
	{
		 if (acceptChannel_)
		 {
			  delete acceptChannel_;
		 }
	}


	bool Acceptor::Listen(const char* ip, uint16_t port){

		if (!acceptChannel_)
		{
			acceptChannel_ = new Connection();
			acceptChannel_->Init(disPatch_);
			acceptChannel_->SetCallback(this);
		}
		

		return acceptChannel_->Listen(ip,port);
		
		
	}

	void Acceptor::SetCallBack(AcceptorCallback*  callback){
		newConnectionCallback_ = callback;
	}
	 void Acceptor::OnAccept(Connection* conn,Socket* socket){
		 if (acceptChannel_ != conn)
		 {
			 return;
		 }
		 if (newConnectionCallback_)
		 {
			 newConnectionCallback_->OnAccept(newConnection(socket));
		 }
		
	 }
	 void Acceptor::OnConfirm(Connection* conn){

	 }
	 void Acceptor::OnRecv(Connection* conn, IMessage* msg){

	 }
	 void Acceptor::OnClose(Connection* conn){
		 if (newConnectionCallback_)
		 {
			 //newConnectionCallback_->OnClose(conn);
		 }
		
	 }
	 Connection* Acceptor::newConnection(Socket* socket){
		 Connection* newConn =new Connection();
		 newConn->SetSocket(socket);
		 
		 return newConn;
	 }
}