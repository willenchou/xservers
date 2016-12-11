#pragma once
#include "include/xlink_intf.h"
#include <vector>
using namespace std;
using namespace x;


namespace xlinkconn_test {

	
class TestClientCallback:public LinkClientCallback
{
public:
	TestClientCallback(void);
	~TestClientCallback(void);
protected:
	virtual void OnConnect(ILinkClient* api) ;
	virtual void OnRegister(ILinkClient* api);
	virtual void OnClose(ILinkClient* api) ;
	virtual void OnRecv(ILinkClient* api, IMessage* msg) ;
public:
	int g_connecting_num;
	long g_connected_num ;
	long g_register_num;
	uint32_t ubegin_time_;
	uint32_t ubegin_register_time_;
	uint32_t uconnected_time_;
	uint32_t uregister_time_;

	vector<ILinkClient*> connclientlist;
	ILinkClientApi* linkapi_;
};
}

