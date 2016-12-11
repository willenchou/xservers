#include "connection.h"
#include "message.h"

namespace x {

Connection::Connection()
:socket_(NULL)
,callback_(NULL)
,sendBuff_(NULL)
,recvBuff_(NULL)
,callbackMsg_(NULL){
	sendBuff_ = new ConnectionBuff();
	recvBuff_ = new ConnectionBuff();
	callbackMsg_ = new Message();
}

Connection::~Connection() {

}

bool Connection::Listen(const char* ip, uint16_t port) {
	socket_ = new Socket();
	socket_->SetCallback(this);
	if (!socket_->Listen(ip, port)) {
		delete socket_;
		socket_ = NULL;
	}

	return true;
}

bool Connection::Connect(const char* ip, uint16_t port) {
	socket_ = new Socket();
	socket_->SetCallback(this);
	if (!socket_->Connect(ip, port)) {
	//	delete socket_;
	//	socket_ = NULL;
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

void Connection::OnConnect(Socket* socket) {
	if (callback_) 
		callback_->OnConnect(this, socket);
}

void Connection::OnConfirm(){
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
	if (sendBuff_->GetLen() == 0) {
		Sleep(10);//add 20160613
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

}//namespace x