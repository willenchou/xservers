#include "xconnection_sdk.h"
#include "pack.h"
#include "message.h"
#include "event_dispatch.h"

namespace x {

ConnectionSDK::ConnectionSDK()
:callback_(NULL)
,connection_(NULL)
,status_(CONN_STATUS_NOT_CONNECT){
	
}

ConnectionSDK::~ConnectionSDK(){

}

long ConnectionSDK::Run(){
	EventDispatch::Instance()->StartDispatch();
	return 0L;
}

IMessage* ConnectionSDK::NewMessage(){
	return new Message();
}

IPack* ConnectionSDK::NewPack(){
	return new Pack();
}

int ConnectionSDK::Init(ConnectionCallback* callback){
	callback_ = callback;

	dispatch_ = new EventDispatch();
	connection_ = new Connection();
	connection_->SetCallback(this);

	Thread::Start();

	return 0;
}

int ConnectionSDK::UnInit(){
	if(connection_) 
		delete connection_;
	if(dispatch_)
		delete dispatch_;

	return 0;
}

int ConnectionSDK::Connect(const char* ip, uint16_t port){
	status_ = CONN_STATUS_CONNECTING;
	return connection_->Connect(ip, port) ? 0 : -1;
}

int ConnectionSDK::Send(IMessage* msg){
	return connection_->Send(msg);
}

int ConnectionSDK::SendSync(IMessage* reqMsg, IMessage* ansMsg, uint32_t timeout){
	return -1;
}

void ConnectionSDK::OnConnect(Connection* conn, Socket* socket){

}

void ConnectionSDK::OnConfirm(Connection* conn){
	status_ = CONN_STATUS_CONNECTED;
	
	FuncReqRegisterBody* req = new FuncReqRegisterBody();
	memset(req, 0, sizeof(FuncReqRegisterBody));
	req->type = 1;

	IMessage* msg = new Message();
	msg->SetFuncType(MSG_FUNC_TYPE_REQ);
	msg->SetFuncID(FUNC_REGISTER_CONNECTION);
	msg->AddData(MSG_TAG_FUNC_BODY, req, sizeof(FuncReqRegisterBody));

	conn->Send(msg);

	delete req;
	msg->Release();
}

void ConnectionSDK::OnRecv(Connection* conn, IMessage* msg){
	

	uint32_t funcID = 0;
	uint32_t funcType = MSG_FUNC_TYPE_REQ;
	
	

	msg->GetFuncID(&funcID);
	msg->GetFuncType(&funcType);

	if(funcID == FUNC_REGISTER_CONNECTION){
		if(funcType == MSG_FUNC_TYPE_ANS){
			status_ = CONN_STATUS_CONNECTED;
			if(callback_)
				callback_->OnConnect(this);
		}
	}else{
		if(callback_)
			callback_->OnRecv(this, msg);
	}
}

void ConnectionSDK::OnClose(Connection* conn){
	if(callback_)
		callback_->OnClose(this);
}

XCONNECTION_EXPORT ConnectionApi* NewConnectionAPI(){
	return new ConnectionSDK();
}

}//namespace x


