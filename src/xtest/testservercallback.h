#pragma once
#include "test_helper.h"
#include <include/xlink_intf.h>
using namespace x;
namespace xlink_test {
class TestServerCallback:public LinkServerCallback
{
public:
	TestServerCallback(void);
	~TestServerCallback(void);

public:
	virtual void OnRecv(LinkServerApi* api,ILinkClient* curConn,  IMessage* msg);
	virtual void OnAccept(LinkServerApi* api);
private:
	void FuncReqRegisterConnection(ILinkClient* api, IMessage* msg);
private:
	long connNum_;
	long registerNum_;
	Mutex clientMutex_;

	//test
	uint32_t ubegin_time_;
	uint32_t ubegin_register_time_;
	uint32_t uconnected_time;
	uint32_t uregister_time;
};
}

