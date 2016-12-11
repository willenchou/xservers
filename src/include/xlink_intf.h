#ifndef XLINK_SERVICE_INTF_H_
#define XLINK_SERVICE_INTF_H_

#include <xbase/os_common.h>
#include <xbase/util.h>
#include <include/xservice_intf.h>
#include <include/xmessage_intf.h>
#include "linkargs.h"


namespace x {

#define PLUGIN_LINK		"xlink"
#define SID_LINK			"x.link"



	//数据库统一访问服务。
	struct ILINKService : public IService{
		
	};
	struct ILinkClient:public IRefCounted{
		virtual bool Connect(const char* ip, uint16_t port) = 0;
		virtual int Send(IMessage* msg) = 0;
		//virtual int SendSync(IMessage* reqMsg, IMessage* ansMsg, uint32_t timeout = 5000) = 0;
	};

	struct LinkClientCallback{
		    virtual void OnRegister(ILinkClient* api) = 0;
			virtual void OnConnect(ILinkClient* api) = 0;
			virtual void OnClose(ILinkClient* api) = 0;
			virtual void OnRecv(ILinkClient* api, IMessage* msg) = 0;
		};
	struct ILinkClientApi{
			virtual IMessage* NewMessage() = 0;
			virtual IPack* NewPack() = 0;
			virtual ILinkClient*  NewLinkClient(LinkClientCallback* callback) = 0;

			virtual int Init(LinkClientArgs& args) = 0;
			virtual int UnInit() = 0;
	};


	struct LinkServerApi;
	struct LinkServerCallback{

		virtual void OnRecv(LinkServerApi* api,ILinkClient* curConn, IMessage* msg) = 0;
		virtual void OnAccept(LinkServerApi* api) = 0;
	};

	struct  LinkServerApi{
		virtual int Init(LinkServerCallback* callback,LinkServerArgs& args) = 0;
		virtual int UnInit() = 0;
		virtual int Start() = 0; //listen
		virtual int Send(IMessage* msg) = 0;
	};

#ifdef OS_WIN
#	if defined(XLINK_IMPL)
#		define	XLINK_EXPORT __declspec(dllexport)
#	else
#		define XLINK_EXPORT __declspec(dllimport)
#	endif
#else
#	define	XLINK_EXPORT __attribute__((visibility("default")))
#endif
	 //extern "C"
	 XLINK_EXPORT LinkServerApi*  NewLinkServerAPI();
	 XLINK_EXPORT ILinkClientApi* NewLinkClientAPI();
	

}//namespace

#endif //XLINK_SERVICE_INTF_H_


