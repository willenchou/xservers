
#include "linkserver.h"

namespace x{
LinkServer::LinkServer(void){
	acceptThread_ = NULL;
	 dispatch_ = NULL;
	 threadpool_ = NULL;
	 acceptConn_ = NULL;
	 callback_ = NULL;
	 memset(&args_,0,sizeof(LinkServerArgs));
}


LinkServer::~LinkServer(void){
	if (acceptThread_){
		 delete acceptThread_;
	}

	if (!args_.enable_accept_thread && dispatch_){
		delete dispatch_;

	}

	if (threadpool_){
		delete threadpool_;
	}

	if (acceptConn_){
		delete acceptConn_;
	}
}

 int LinkServer::Init(LinkServerCallback* callback,LinkServerArgs& args){

	 callback_ = callback;
	 args_ = args;
	 if (args_.enable_accept_thread){
		 acceptThread_ =new EventThread();
		 dispatch_ = acceptThread_->GetDispatch();
	 }
	 else{
		dispatch_ =  new EventDispatch();
	 }
	 

	 threadpool_ = new EventThreadPool(dispatch_);
	 
	 threadpool_->setThreadNum(args_.threadNum);

	 acceptConn_ =new Acceptor(dispatch_);
	 acceptConn_->SetCallBack(this);

	
	 return 0;
 }
 int LinkServer::UnInit(){
	 return 0;
 }
 int LinkServer::Start(){
	 threadpool_->start();
	 if (acceptConn_->Listen(args_.ip,args_.port))
	 {
		 //
		 if (args_.enable_accept_thread)
		 {
			 acceptThread_->Start();
		 }
		 else
		 {
		  dispatch_->StartDispatch();
		 }
	 }	 

	 return 0;

 }
 int LinkServer::Send(IMessage* msg){

	 //根据路由信息找到Connector
	 Connector* conn = clientconns_.front();
	 if (conn)
	 {
		return conn->Send(msg);
	 }

	 return -1;
 }
//Iconnectorcallback
 void LinkServer::OnRecv(Connector* conn, IMessage* msg){

	 if (callback_)callback_->OnRecv(this,conn,msg);
	 //void* data;
	 //uint32_t dataLen;
	 //if(!msg->GetData(MSG_TAG_FUNC_INFO, &data, &dataLen)) return;

	 //MsgFuncInfo* funcInfo = (MsgFuncInfo*)data;
	 //switch(funcInfo->funcID){
	 //case FUNC_REGISTER_CONNECTION:
		// if(funcInfo->type == MSG_FUNC_TYPE_REQ){
		//	FuncReqRegisterConnection(conn, msg);
		// }else{
		//	// FuncAnsRegisterConnection(conn, msg);
		// }
		// break;
	 //default:
		// //给其他业务线程处理
		// if (callback_)callback_->OnRecv(this,msg);
		// break;
	 //}
 }
  void LinkServer::OnConnected(Connector* conn){
	  return;//NULL
 }
 void LinkServer::OnClose(Connector* conn){
	 return;//NULL
 }

 void LinkServer::OnAccept(Connection* conn){
	 
	 newConnector(conn);
	 EventDispatch* ioLoop = threadpool_->GetNextDispatch();

	  //看看把这一层如何封到里面去
	 Channel* channel =new Channel(conn->GetSocket()->GetSocket(),ioLoop);
	 channel->SetState(Channel_STATE_CONNECTED);
	 conn->SetChannel(channel);
	 conn->Init(ioLoop);

	 if (callback_){
		 callback_->OnAccept(this);
	 }
		 
 }
 Connector* LinkServer::NewConnector(){
	 return new Connector();
 }
 void LinkServer:: newConnector(Connection* conn){

	 
	 Connector* client = NewConnector();
	 client->direction = 1;
	 
	 client->name[0] = '\0';
	 client->connectionID = clientIndex_++; //
	 Util::StrCopy(client->IP, conn->GetSocket()->GetRemoteIP());
	 client->port = conn->GetSocket()->GetRemotePort();
	 client->status = CONN_STATUS_CONNECTED;
	 client->connectTime = Util::TimeTickCount();

	 client->connChannel = conn;
	 client->callback_ = this;

	 conn->SetCallback( (Connection::Callback*)client);
	 clientconns_.push_back(client);
	 //printf("--",);
 }
 void LinkServer::FuncReqRegisterConnection(Connector* conn, IMessage* msg){
	 void* data;
	 uint32_t dataLen;
	 if ( !msg->GetData(MSG_TAG_FUNC_BODY, &data, &dataLen )) return;

	 FuncReqRegisterBody* req = (FuncReqRegisterBody*)data;

	 ScopedLock lock(clientMutex_);//是否需要移到外面去

	 Connector* client = conn;//FindClient(conn);
	 if (!client) return;

	 client->status = CONN_STATUS_REGISTED;
	 
 }
 
  Connector* LinkServer::FindClient(Connector* conn){
	 for(size_t i = 0; i < clientconns_.size(); i++){
		 Connector* client = clientconns_.at(i);
		 if(client == conn)
			 return client;
	 }
	 return NULL;
 }
 XLINK_EXPORT LinkServerApi*  NewLinkServerAPI(){
	 return new LinkServer();
 }
}
