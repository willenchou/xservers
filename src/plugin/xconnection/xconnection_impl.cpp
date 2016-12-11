#include "xconnection_impl.h"
#include "xconnection.h"
#include <xbase/util.h>
#include "event_dispatch.h"
#include "pack.h"
#include "message.h"

namespace x {

ConnectionService* _connService = new ConnectionService();

ConnectionService::ConnectionService(){
	preMsgService_ = NULL;
	nextMsgService_ = NULL;
	
	msgService_ = NULL;
	
	timerID_ = -1;
	networkCfg_ = new NetworkConfig();
	initResult_ = false;
	server_ = NULL;
	clientIndex_ = 0;
}

ConnectionService::~ConnectionService(){

}

void ConnectionService::Init(){
	const char* strSysNo = _connIoc->GetArg(ARGS_SYS_NO);
	sysNo_ = strSysNo ? atoi(strSysNo) : 0;
	const char* strSubSysNo = _connIoc->GetArg(ARGS_SUB_SYS_NO);
	subSysNo_ = strSubSysNo ? atoi(strSubSysNo) : 0;
	const char* strAppName = _connIoc->GetArg(ARGS_APP_NAME);
	Util::StrCopy(appName_, strAppName ? strAppName : "");
	const char* strAppNo = _connIoc->GetArg(ARGS_APP_NO);
	appNo_ = strAppNo ? atoi(strAppNo) : 0; 

	msgService_ = (IMessageService*)_connIoc->GetContainer()->GetService(SID_MESSAGE);

	XMLElement* config = _connIoc->GetConfig();
	if(!config) {
		return;
	}
	
	const char* networkCfgFile = DEFAULT_NETOWRK_FILE;

	XMLElement* argsNode = config->FirstChildElement("args");
	if(argsNode){
		networkCfgFile = argsNode->Attribute("network_config");
		if(!networkCfgFile)
			networkCfgFile = DEFAULT_NETOWRK_FILE;
	}
	
	networkCfg_->Init(networkCfgFile);
	
	initResult_ = true;
}

void ConnectionService::UnInit(){

}

void ConnectionService::Start(){
	if(!initResult_) return;

	Thread::Start();
	
	timerID_ = EventDispatch::Instance()->AddTimer(TIMER_INTERVAL, this);

	NetworkConfig::Node* node = NULL;
	node = networkCfg_->GetNode(sysNo_, subSysNo_, appName_, appNo_);
	if(node){
		server_ = new Connection();
		server_->SetCallback(this);
		if(!server_->Listen("0.0.0.0", node->port)){
			delete server_;
			server_ = NULL;
		}
	}

	ScopedLock lock(clientMutex_);

	uint32_t parentCount = networkCfg_->GetParentCount(sysNo_, subSysNo_, appName_, appNo_);
	for(uint32_t i = 0; i < parentCount; i++){
		node = networkCfg_->GetParent(sysNo_, subSysNo_, appName_, appNo_, i);
		
		Connection* conn = new Connection();
		conn->SetCallback(this);
		if(conn->Connect(node->ip, node->port)){
			ClientConn* client = new ClientConn();
			client->direction = 0;
			client->conn = conn;
			client->name[0] = '\0';
			Util::StrCopy(client->IP, node->ip);
			client->port = node->port;
			client->status = CONN_STATUS_CONNECTING;
			client->connectTime = Util::TimeTickCount();

			clients_.push_back(client);
		}else{
			delete conn;
			conn = NULL;
		}
	}
}

void ConnectionService::Stop(){

}

bool ConnectionService::Send(IMessage* msg){
	/*
	void* routeSrcdata = NULL;
	uint32_t routeSrcdataLen = 0;
	MsgRoute* routeSrc = NULL;
	if(msg->GetData(MSG_TAG_ROUTE_SRC, &routeSrcdata, &routeSrcdataLen)){
		routeSrc = (MsgRoute*)routeSrcdata;
	}
	*/
	
	void* routeDestdata = NULL;
	uint32_t routeDestdataLen = 0;
	MsgRoute* routeDest = NULL;
	if(msg->GetData(MSG_TAG_ROUTE_DEST, &routeDestdata, &routeDestdataLen)){
		routeDest = (MsgRoute*)routeDestdata;
	}else if(msg->GetData(MSG_TAG_ROUTE_INTERNAL, &routeDestdata, &routeDestdataLen)){
		routeDest = (MsgRoute*)routeDestdata;
	}

	if(routeDest){
		if(routeDest->sysNo == sysNo_ &&
			routeDest->subSysNo == subSysNo_ &&
			strcmp(routeDest->appName, appName_) == 0 &&
			routeDest->appNo == appNo_)
		{
			for(size_t i = 0; i < clients_.size(); i++){
				ClientConn* client = clients_.at(i);
				if(client->status != CONN_STATUS_REGISTED) continue;
				if(client->direction == 1 && client->connectionID == routeDest->connectionID){
					client->conn->Send(msg);
					return true;
				}
			}
		}else{
			MsgRoute currRoute;
			currRoute.sysNo = sysNo_;
			currRoute.subSysNo = subSysNo_;
			Util::StrCopy(currRoute.appName, appName_);
			currRoute.appNo = appNo_;

			NetworkConfig::Node* node = networkCfg_->FindRouteNode(&currRoute, routeDest);
			if(node){
				for(size_t i = 0; i < clients_.size(); i++){
					ClientConn* client = clients_.at(i);
					if(client->status != CONN_STATUS_REGISTED) continue;
					if(node->sysNo == client->sysNo &&
						node->subSysNo == client->subSysNo &&
						strcmp(node->appName, client->appName) == 0 &&
						node->appNo == client->appNo)
					{
						client->conn->Send(msg);
						return true;
					}
				}
			}
		}
	}

	return false;
}

long ConnectionService::Run(){
	EventDispatch::Instance()->StartDispatch();
	return 0L;
}

IMessage* ConnectionService::NewMessage(){
	return new Message();
}

IPack* ConnectionService::NewPack(){
	return new Pack();
}

bool ConnectionService::GetRoute(const char* nodeName, MsgRoute* route){
	char strNodeName[128];
	Util::StrCopy(strNodeName, nodeName);

	char* appName = strNodeName;
	char* appNo = NULL;
	char* sysNo = NULL;
	char* subSysNo = NULL;

	char* pos = strNodeName;
	while(*pos != '\0'){
		if(*pos == '#' && *(pos+1) != '\0'){
			*pos = '\0';
			if(!appNo){
				appNo = pos+1;
			}else{
				subSysNo = pos+1;
			}
		}

		if(*pos == '@' && *(pos+1) != '\0'){
			if(!sysNo)
				sysNo = pos+1;
		}
		pos++;
	}
	
	char clientName[128];
	sprintf(clientName, "%s#%s@%s#%s", appName, appNo?appNo:"0", sysNo?sysNo:"0", subSysNo?subSysNo:"0");
	for(size_t i = 0; i < clients_.size(); i++){
		ClientConn* client = clients_.at(i);
		if(strcmp(client->name, clientName) == 0){
			if(route){
				route->sysNo = client->sysNo;
				route->subSysNo = client->subSysNo;
				Util::StrCopy(route->appName, client->appName);
				route->appNo = client->appNo;
			}
			return true;
		}
	}

	return false;
}

void ConnectionService::RegisterPre(IMsgService* service){
	preMsgService_ = service;
}

void ConnectionService::RegisterNext(IMsgService* service){
	nextMsgService_ = service;
}

void ConnectionService::PostMsg(IMessage* msg, IMsgService* from){
	if(from == preMsgService_){
		//nextMsgService_->PostMsg(msg, this);
	}else if(from == nextMsgService_) {
		if(!Send(msg)){
			msg->ChangeReq2Ans();
			msg->SetReturnID(MSG_RETURN_CODE_ERR_TRANSFORM);
			if(!Send(msg)){
				//TODO
			}
		}
	}
}

void ConnectionService::OnConnect(Connection* conn, Socket* socket){
	LOG_DEBUG_F(PLUGIN_CONNECTION, "%s:%d connected:totalcount=[%d]", socket->GetRemoteIP(), socket->GetRemotePort(),clients_.size());
	
	ScopedLock lock(clientMutex_);

	Connection* c = new Connection();
	c->SetCallback(this);
	c->SetSocket(socket);

	ClientConn* client = new ClientConn();
	client->direction = 1;
	client->conn = c;
	client->name[0] = '\0';
	client->connectionID = clientIndex_++;
	Util::StrCopy(client->IP, socket->GetRemoteIP());
	client->port = socket->GetRemotePort();
	client->status = CONN_STATUS_CONNECTED;
	client->connectTime = Util::TimeTickCount();

	clients_.push_back(client);
}

void ConnectionService::OnConfirm(Connection* conn){
	
	ScopedLock lock(clientMutex_);
	LOG_DEBUG_F(PLUGIN_CONNECTION, "connected to %s:%d", conn->GetSocket()->GetRemoteIP(), conn->GetSocket()->GetRemotePort());

	FuncReqRegisterBody* req = new FuncReqRegisterBody();
	req->type = 0;
	req->sysNo = sysNo_;
	req->subSysNo = subSysNo_;
	Util::StrCopy(req->appName, appName_);
	req->appNo = appNo_;

	IMessage* msg = msgService_->NewMessage();
	msg->SetFuncType(MSG_FUNC_TYPE_REQ);
	msg->SetFuncID(FUNC_REGISTER_CONNECTION);
	msg->AddData(MSG_TAG_FUNC_BODY, req, sizeof(FuncReqRegisterBody));

	conn->Send(msg);

	delete req;
	msg->Release();
}

void ConnectionService::OnRecv(Connection* conn, IMessage* msg){
	void* data;
	uint32_t dataLen;
	if(msg->GetData(MSG_TAG_FUNC_INFO, &data, &dataLen)){
		MsgFuncInfo* funcInfo = (MsgFuncInfo*)data;
		switch(funcInfo->funcID){
			case FUNC_REGISTER_CONNECTION:
				if(funcInfo->type == MSG_FUNC_TYPE_REQ){
					FuncReqRegisterConnection(conn, msg);
				}else{
					FuncAnsRegisterConnection(conn, msg);
				}
				break;
			default:
				ScopedLock lock(clientMutex_);
				ClientConn* client = FindClient(conn);
				if(client){
					//如果发现没有路由来源，这里打上路由来源
					if(!msg->DataExist(MSG_TAG_ROUTE_SRC)){
						MsgRoute route;
						route.sysNo = client->sysNo;
						route.subSysNo = client->subSysNo;
						Util::StrCopy(route.appName, client->appName);
						route.appNo = client->appNo;
						route.connectionID = client->connectionID;
						route.plugin[0] = '\0';
						msg->SetData(MSG_TAG_ROUTE_SRC, &route, sizeof(route));
					}

					if(nextMsgService_){
						nextMsgService_->PostMsg(msg, this);
					}else{
						msg->ChangeReq2Ans();
						msg->SetReturnID(MSG_RETURN_CODE_ERR_TRANSFORM);
						client->conn->Send(msg);
					}
				}
				break;
		}
	}
}

void ConnectionService::OnClose(Connection* conn){
	
	ScopedLock lock(clientMutex_);
	LOG_DEBUG_F(PLUGIN_CONNECTION, "connect %s:%d closed leftcount[%d]", conn->GetSocket()->GetRemoteIP(), conn->GetSocket()->GetRemotePort(),clients_.size());

	ClientConnVector::iterator it;
	for(it = clients_.begin(); it != clients_.end(); it++){
		ClientConn* client = *it;
		if(client->conn == conn){
			if(client->direction == 0){
				client->status = CONN_STATUS_NOT_CONNECT;
			}else if(client->direction == 1){
				ClientConnMap::iterator mapIt = clientMap_.find(std::string(client->name));
				if(mapIt != clientMap_.end())
					clientMap_.erase(mapIt);
				clients_.erase(it);
			}
			break;
		}
	}
}

void ConnectionService::OnTimer(int id){
	if(id == timerID_){
		ScopedLock lock(clientMutex_);
		for(size_t i = 0; i < clients_.size(); i++){
			ClientConn* client = clients_.at(i);
			if(client->direction == 0 && client->status == CONN_STATUS_NOT_CONNECT){
				if(client->conn->Connect(client->IP, client->port)){
					client->status = CONN_STATUS_CONNECTING;
					client->connectTime = Util::TimeTickCount();
				}
			}
		}
	}
}

ConnectionService::ClientConn* ConnectionService::FindClient(Connection* conn){
	for(size_t i = 0; i < clients_.size(); i++){
		ClientConn* client = clients_.at(i);
		if(client->conn == conn)
			return client;
	}
	return NULL;
}

void ConnectionService::FuncReqRegisterConnection(Connection* conn, IMessage* msg){
	void* data;
	uint32_t dataLen;
	if(msg->GetData(MSG_TAG_FUNC_BODY, &data, &dataLen)){
		FuncReqRegisterBody* req = (FuncReqRegisterBody*)data;

		ScopedLock lock(clientMutex_);

		ClientConn* client = FindClient(conn);
		if(client){
			if(req->type == 0){
				sprintf(client->name, "%s#%d@%d#%d", req->appName, req->appNo, req->sysNo, req->subSysNo);
				client->sysNo = req->sysNo;
				client->subSysNo = req->subSysNo;
				Util::StrCopy(client->appName, req->appName);
				client->appNo = req->appNo;
			}else{
				sprintf(client->name, "%s#%d@%d#%d_%d", appName_, appNo_, sysNo_, subSysNo_, client->connectionID);
				client->sysNo = sysNo_;
				client->subSysNo = subSysNo_;
				Util::StrCopy(client->appName, appName_);
				client->appNo = appNo_;
			}
			client->status = CONN_STATUS_REGISTED;
			clientMap_[std::string(client->name)] = client;
			
			FuncAnsRegisterBody* ans = new FuncAnsRegisterBody();
			ans->sysNo = sysNo_;
			ans->subSysNo = subSysNo_;
			Util::StrCopy(ans->appName, appName_);
			ans->appNo = appNo_;
			ans->connectionID = client->connectionID;

			msg->ChangeReq2Ans();
			msg->SetData(MSG_TAG_FUNC_BODY, ans, sizeof(FuncAnsRegisterBody));
			conn->Send(msg);

			LOG_DEBUG_F(PLUGIN_CONNECTION, "connect %s:%d registed at %s", 
				conn->GetSocket()->GetRemoteIP(), 
				conn->GetSocket()->GetRemotePort(),
				client->name);

			delete ans;
		}
	}
}

void ConnectionService::FuncAnsRegisterConnection(Connection* conn, IMessage* msg){
	void* data;
	uint32_t dataLen;
	if(msg->GetData(MSG_TAG_FUNC_BODY, &data, &dataLen)){
		FuncAnsRegisterBody* ans = (FuncAnsRegisterBody*)data;
		
		ScopedLock lock(clientMutex_);
		
		ClientConn* client = FindClient(conn);
		if(client){
			sprintf(client->name, "%s#%d@%d#%d", ans->appName, ans->appNo, ans->sysNo, ans->subSysNo);
			client->sysNo = ans->sysNo;
			client->subSysNo = ans->subSysNo;
			Util::StrCopy(client->appName, ans->appName);
			client->appNo = ans->appNo;
			client->connectionID = ans->connectionID;
			client->status = CONN_STATUS_REGISTED;
			clientMap_[std::string(client->name)] = client;
			
			LOG_DEBUG_F(PLUGIN_CONNECTION, "connect %s:%d registed at %s", 
				conn->GetSocket()->GetRemoteIP(), 
				conn->GetSocket()->GetRemotePort(),
				client->name);
		}
	}
}

}//namespace x