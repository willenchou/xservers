#include "testclientcallback.h"
//#include "windows.h"
#include "../plugin/xlink/link_common.h"
namespace xlinkconn_test {

TestClientCallback::TestClientCallback(void)
{
	 g_connecting_num = 0;
	 g_connected_num = 0;
	 g_register_num = 0;

	 ubegin_time_ = 0;
	 ubegin_register_time_ = 0;
	 uconnected_time_ = 0;
	 uregister_time_ = 0;
}


TestClientCallback::~TestClientCallback(void)
{
	for(size_t i = 0; i < connclientlist.size(); i++){
		delete connclientlist.at(i);
	}
	connclientlist.clear();
}

 void TestClientCallback::OnConnect(ILinkClient* api) {
	InterlockedIncrement(&g_connected_num);
	//if (g_connected_num> g_connecting_num-10)
	{
		// printf("\n----connecting:[%d]----connected:[%d]----[%p]------\n",g_connecting_num,g_connected_num,api);
	}
   
   if (g_connected_num == g_connecting_num)
   {
	    uconnected_time_ = GetTickCount();
		ubegin_register_time_ = uconnected_time_;
	   uint32_t utime = uconnected_time_- ubegin_time_;
	   printf("\n--Client--2----connected:[%d]----connected_time_:[%d]----connected_splantime:[%d]----\n",g_connected_num,uconnected_time_,utime);

	   printf("\n--Client--3----RegisterNum:[%d]----register_time_:[%d]----\n",g_register_num,ubegin_register_time_);
	   FuncReqRegisterBody* req = new FuncReqRegisterBody();
		memset(req, 0, sizeof(FuncReqRegisterBody));
		req->type = 1;

		IMessage* msg = linkapi_->NewMessage();
		msg->SetFuncType(MSG_FUNC_TYPE_REQ);
		msg->SetFuncID(FUNC_REGISTER_CONNECTION);
		msg->AddData(MSG_TAG_FUNC_BODY, req, sizeof(FuncReqRegisterBody));

		

		
	   for (int i = 0;i<connclientlist.size();i++)
	   {
		   //printf("-------Send----index:[%d]---------\n",i);
		   connclientlist.at(i)->Send(msg);
	   }
	   delete req;
	   msg->Release();
   }
 }

 void TestClientCallback::OnRegister(ILinkClient* api) {
	 InterlockedIncrement(&g_register_num);

	// printf("\n---register:[%d]--[%p]------\n",g_register_num,api);
	 if (g_register_num == g_connecting_num)
	 {
		 uregister_time_ = GetTickCount();
		 uint32_t utime = uregister_time_- ubegin_register_time_;
		
		 printf("\n--Client--4----RegisterNum:[%d]---register_time_:[%d]----register_splantime:[%d]----\n",g_register_num,uregister_time_,utime);	 

	 }
 }
 void TestClientCallback::OnClose(ILinkClient* api) {

 }
 void TestClientCallback::OnRecv(ILinkClient* api, IMessage* msg) {
	 
	 return ;
 }
}
