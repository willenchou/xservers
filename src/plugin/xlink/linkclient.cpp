
#include "linkclient.h"
#include "connector.h"
#include "message.h"


namespace x{

	Linker::Linker(EventDispatch* dispatch)
	{
		status_ = CONN_STATUS_NOT_CONNECT;
		dispatch_ = dispatch;


		Connection* conn = new Connection();
		conn->Init(dispatch);
		
		connector = new Connector();
		conn->SetCallback((Connection::Callback*)connector);
		connector->connChannel =conn;

		connector->callback_ = this;
	}


	Linker::~Linker(void)
	{
	}

	bool Linker::Connect(const char* ip, uint16_t port){
		status_ =CONN_STATUS_CONNECTING;
		return connector->Connect(ip,port);
	}

	int Linker::Send(IMessage* msg){
		return connector->Send(msg);
	}

	void Linker::SetCallBack(LinkClientCallback* callback){
		callback_ =callback;
	}

	//Iconnectorcallback
	void Linker::OnRecv(Connector* conn, IMessage* msg){

		uint32_t funcID = 0;
		uint32_t funcType = MSG_FUNC_TYPE_REQ;

		msg->GetFuncID(&funcID);
		msg->GetFuncType(&funcType);

		if(funcID == FUNC_REGISTER_CONNECTION){
			if(funcType == MSG_FUNC_TYPE_ANS){
				status_ = CONN_STATUS_REGISTED;
				if(callback_)
					callback_->OnRegister(this);
			}
		}else{
			if(callback_)
				callback_->OnRecv(this, msg);
		}


	}
	void Linker::OnConnected(Connector* conn){

		status_ = CONN_STATUS_CONNECTED;
		/*FuncReqRegisterBody* req = new FuncReqRegisterBody();
		memset(req, 0, sizeof(FuncReqRegisterBody));
		req->type = 1;

		IMessage* msg = new Message();
		msg->SetFuncType(MSG_FUNC_TYPE_REQ);
		msg->SetFuncID(FUNC_REGISTER_CONNECTION);
		msg->AddData(MSG_TAG_FUNC_BODY, req, sizeof(FuncReqRegisterBody));

		conn->Send(msg);

		delete req;
		msg->Release();*/
		if (callback_)callback_->OnConnect(this);
	}
	void Linker::OnClose(Connector* conn){
		status_ = CONN_STATUS_NOT_CONNECT;
		if (callback_)callback_->OnClose(this);
	}

}

