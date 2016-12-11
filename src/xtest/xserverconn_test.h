#ifndef XTEST_XSERVERCONN_TEST_H_
#define XTEST_XSERVERCONN_TEST_H_
#pragma   once  
#include "test_helper.h"
#include <include/xlink_intf.h>
#include "testservercallback.h"
#include "test_def.h"
#include "xbase/thread.h"
using namespace x;

namespace xlink_test {
	bool Test(){
		//
		TestServerCallback callback;
		//LinkServerCallback* callback = new TestServerCallback();
		LinkServerArgs args ;
		args.enable_quick_ack = true;

		args.enable_accept_thread = true;//接收连接使用独立线程，不占用当前线程
		args.init_connNum = 1000;
		args.init_RecvBuffSize = 10240;
		args.init_SendBuffSize = 10240;
		args.init_SendQueueSize = 5;
		args.max_connNum = 50000;
        args.threadNum = 2;
		args.port = _port;
		if (_ip)
		{
			strncpy(args.ip, _ip, sizeof(args.ip)-1);
			args.ip[sizeof(args.ip)-1] = '\0';
		}
		else
		{
			strncpy(args.ip, TEST_IP, sizeof(args.ip)-1);
			args.ip[sizeof(args.ip)-1] = '\0';
		}
		
		
		
		//GetService(PLUGIN_LINK, SID_LINK);

	
		x::LinkServerApi* server = x::NewLinkServerAPI();

		server->Init(&callback,args);
		server->Start();

		//mainloop
		while(1){
			x::SleepX(10);
		}
		return true;
		//server->Init()
		
	}
	
};
#endif//XTEST_XSERVERCONN_TEST_H_