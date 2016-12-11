#include "connection.h"
#include "message.h"
#include "xbase/thread.h"

namespace x {

Connection::Connection()
:socket_(NULL)
,callback_(NULL)
,sendBuff_(NULL)
,recvBuff_(NULL)
,callbackMsg_(NULL)
,eventDispatch_(NULL)
,channel_(NULL){
	sendBuff_ = new ConnectionBuff();
	recvBuff_ = new ConnectionBuff();
	callbackMsg_ = new Message();
}

Connection::~Connection() {
	delete socket_;
	socket_ = NULL;
}
void Connection::Init(EventDispatch* eventdispatch){
	if (!socket_)
	{
		socket_ = new Socket();
		socket_->SetSocket(Socket::NewSOCKET());
	}
	
	if (!eventDispatch_)
	{
		eventDispatch_ = eventdispatch;
	}
	
	if (!channel_)
	{
		channel_ = new Channel(socket_->GetSocket(),eventDispatch_);
		channel_->SetCallBack(this);
	}

	if(eventDispatch_){
		eventDispatch_->AddEvent(socket_->GetSocket(),channel_);
	}
}
bool Connection::Listen(const char* ip, uint16_t port) {	

	if (!socket_|| !channel_){
		return false;
	}

	channel_->SetState(Channel_STATE_LISTENING);

	if (!socket_->Listen(ip, port)) {
		return false;
	}
    
	

	return true;
}

bool Connection::Connect(const char* ip, uint16_t port) {
	
	if (!socket_|| !channel_){
		return false;
	}
	channel_->SetState(Channel_STATE_CONNECTING);

	if (!socket_->Connect(ip, port)) {
		//return false;
	}

	
	return true;
}

int Connection::Send(IMessage* msg) {
	if (!socket_) return -1;
	
	uint32_t len = 0;
	void* data = NULL;
	if(msg->GetBuff(&data, &len)){
		sendBuff_->Push(data, len);
		return len;
	}

	uint32_t offset = 0;
	while (offset <  len) {
		int sendSize = len - offset;
		if (sendSize > MAX_SOCKET_BUF_SIZE) 
			sendSize = MAX_SOCKET_BUF_SIZE;

		int ret = socket_->Send((char*)data + offset, sendSize);
		if (ret <= 0) 
			break;

		offset += ret;
	}
	
	if (offset < len) {
		sendBuff_->Push((char*)data + offset, len - offset);
	}

	return 0;
}

void Connection::Close() {
	if (!socket_) return;
	socket_->Close();
}
void Connection::OnAccept(){
	acceptSocket(socket_->GetSocket());

}

void Connection::OnConfirm() {
	if (callback_)
		callback_->OnConfirm(this);
}

void Connection::OnRead() {
	if (!socket_) return;

	char buff[1024];
	for (;;) {
		int ret = socket_->Recv(buff, 1024);
		if (ret <= 0)
			break;
		recvBuff_->Push(buff, ret);
	}

	void* data = recvBuff_->GetData();
	uint32_t dataLen = recvBuff_->GetLen();
	
	uint32_t offset = 0;
	while (offset < dataLen) {
		if(dataLen - offset < sizeof(MsgBuffHead))
			break;
		
		MsgBuffHead* head = (MsgBuffHead*)data;
		uint32_t exceptLen = head->len + sizeof(MsgBuffHead);
		if (exceptLen < dataLen - offset) 
			break;
		
		callbackMsg_->SetBuff((char*)data + offset, exceptLen);
		if (callback_)
			callback_->OnRecv(this, callbackMsg_);

		offset += exceptLen;
	}

	recvBuff_->Pop(offset);
}

void Connection::OnWrite() {
	if (sendBuff_->GetLen() == 0){
		x::SleepX(10);//add by zwr 20160613
		return;
	}
	void* data = sendBuff_->GetData();
	uint32_t len = sendBuff_->GetLen();

	uint32_t offset = 0;
	while (offset < len) {
		int sendSize = len - offset;
		if (sendSize > MAX_SOCKET_BUF_SIZE) 
			sendSize = MAX_SOCKET_BUF_SIZE;

		int ret = socket_->Send((char*)data + offset, sendSize);
		if (ret <= 0) 
			break;
		
		offset += ret;
	}

	sendBuff_->Pop(offset);
}

void Connection::OnClose() {
	if (callback_)
		callback_->OnClose(this);
}
void Connection::acceptSocket(SOCKET fd){
	SOCKET newfd = 0;
	sockaddr_in peerAddr;
	socklen_t addrLen = sizeof(sockaddr_in);
	char ipStr[64];

	while ((newfd = accept(fd, (sockaddr*)&peerAddr, &addrLen)) != INVALID_SOCKET ) {
		Socket* socket = new Socket();

		uint32_t ip = ntohl(peerAddr.sin_addr.s_addr);
		uint16_t port = ntohs(peerAddr.sin_port);
		
		snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

		socket->SetSocket(newfd);
		
		socket->SetRemoteIP(ipStr);
		socket->SetRemotePort(port);

	    Socket::SetNoDelay(newfd);
		Socket::SetNonblock(newfd);

		

		if (callback_) callback_->OnAccept(this,socket);
	}
}
}//namespace x