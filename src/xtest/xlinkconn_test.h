#ifndef XTEST_XLINKCONN_TEST_H_
#define XTEST_XLINKCONN_TEST_H_
#pragma   once  
#include "test_helper.h"
#include <include/xlink_intf.h>
#include "testclientcallback.h"
#include <vector>
#include "xbase/ring_queue.h"
#include "xbase/os_common.h"
#include "test_def.h"
#include "xbase/thread.h"
#include <string>
using namespace x;
using namespace std;


namespace xlinkconn_test {
	TestClientCallback callback;
	
	bool Test(){
		string remotingip = TEST_IP;

		int  remotingport = TEST_PORT;
		if (_ip)
		{
			 remotingip =_ip;
		}

		remotingport = _port;
		int connednum = 0;
		ILinkClientApi* linkapi = NewLinkClientAPI();
		callback.linkapi_ = linkapi;
		LinkClientArgs arg;
		arg.init_connNum = _testconnNum;
		arg.init_RecvBuffSize = 1024;
		arg.init_SendBuffSize = 1024;
		arg.init_SendQueueSize = 100;
		arg.max_connNum = FD_CONNSIZE-1;
		arg.threadNum = 1;
		linkapi->Init(arg);
		callback.g_connecting_num = arg.init_connNum;
		ILinkClient* client;

		callback.ubegin_time_ = GetTickCount();
		callback.ubegin_register_time_ = callback.ubegin_time_;
		printf("\n--Client--1----connecting num:[%d]----connect_begin_time:[%d]----\n",callback.g_connecting_num,callback.ubegin_time_);
		int i = 0;
		for ( i= 0;i< arg.init_connNum ; i++){
			client = linkapi->NewLinkClient(&callback); 
			callback.connclientlist.push_back(client);
			if (client->Connect(remotingip.c_str(),remotingport)){
				//Sleep(100);
			}
			//printf("\n--Client--1.1----connecting 1.1. num:[%d]--------\n",i);
		
		}
        printf("\n--Client--1.1----connecting 1.1. num:[%d]--------\n",i);
		while(1){
			x::SleepX(10);
		}

		//
		
		return true;

	}

};
#endif