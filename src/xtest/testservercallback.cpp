#include "testservercallback.h"
#include "../plugin/xlink/link_common.h"

namespace xlink_test {
TestServerCallback::TestServerCallback(void)
{
	 connNum_ = 0;
	 registerNum_ = 0;

	 ubegin_time_ = 0;
	 ubegin_register_time_ = 0;

	 uconnected_time = 0;

	 uregister_time = 0;
}


TestServerCallback::~TestServerCallback(void)
{
}

void TestServerCallback::OnRecv(LinkServerApi* api, ILinkClient* curConn, IMessage* msg){
	void* data;
	uint32_t dataLen;
	if(!msg->GetData(MSG_TAG_FUNC_INFO, &data, &dataLen)) return;

	MsgFuncInfo* funcInfo = (MsgFuncInfo*)data;
	switch(funcInfo->funcID){
	case FUNC_REGISTER_CONNECTION:
	 if(funcInfo->type == MSG_FUNC_TYPE_REQ){
		FuncReqRegisterConnection(curConn, msg);
	 }else{
		// FuncAnsRegisterConnection(conn, msg);
	 }
	 break;
	default:
		msg->ChangeReq2Ans();
		msg->SetReturnID(MSG_RETURN_CODE_ERR_TRANSFORM);
		api->Send(msg);
	 //给其他业务线程处理
	// if (callback_)callback_->OnRecv(this,msg);
	 break;
	}
	
}
void TestServerCallback::OnAccept(LinkServerApi* api){
	InterlockedIncrement(&connNum_);
	
	if (connNum_ == 1)
	{
		ubegin_time_ = GetTickCount();
		ubegin_register_time_ = ubegin_time_;
		 printf("\n--server--1----connectedNum:[%d]----conn_begin_time:[%d]----\n",connNum_,ubegin_time_);
	}
	if (connNum_ == _testconnNum)
	{
		uconnected_time = GetTickCount();
		uint32_t utime = uconnected_time - ubegin_time_;

		printf("\n--server--2----connectedNum:[%d]----conntime:[%d]----conn_splantime:[%d]----\n",_testconnNum,uconnected_time,utime);
	}
   
  // printf("-----------connNum:[%d]--------------------\n",connNum_);
}

void TestServerCallback::FuncReqRegisterConnection(ILinkClient* api, IMessage* msg){
	InterlockedIncrement(&registerNum_);
	if (registerNum_ == 1)
	{
		ubegin_register_time_ = GetTickCount();
		printf("\n--server--3----connectedNum:[%d]----registerNum:[%d]----register_begin_time:[%d]----\n",connNum_,registerNum_,ubegin_register_time_);
	}
	if (registerNum_ == _testconnNum)
	{
		uregister_time = GetTickCount();
		uint32_t utime = uregister_time - ubegin_register_time_;
		
		printf("\n--server--4----connectedNum:[%d]----registerNum:[%d]---registertime:[%d]-----register_splantime:[%d]--------\n",connNum_,registerNum_,uregister_time,utime);
		
	}
	//printf("-----------connNum:[%d]-----registerNum:[%d]---------------\n",connNum_,registerNum_);
	void* data;
	uint32_t dataLen;
	if ( !msg->GetData(MSG_TAG_FUNC_BODY, &data, &dataLen )) return;

	FuncReqRegisterBody* req = (FuncReqRegisterBody*)data;

	ScopedLock lock(clientMutex_);//是否需要移到外面去

	FuncAnsRegisterBody* ans = new FuncAnsRegisterBody();
	ans->sysNo = 2;
	ans->subSysNo = 1;
	//Util::StrCopy(ans->appName, appName_);
	ans->appNo = 1;
	ans->connectionID =2;

	msg->ChangeReq2Ans();
	msg->SetData(MSG_TAG_FUNC_BODY, ans, sizeof(FuncAnsRegisterBody));
	api->Send(msg);
	
}
}
