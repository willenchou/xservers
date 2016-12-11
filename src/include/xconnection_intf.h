#ifndef XCONNECTION_XCONNECTION_SERVICE_INTF_H_
#define XCONNECTION_XCONNECTION_SERVICE_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>
#include <include/xmessage_intf.h>

namespace x {

#define PLUGIN_CONNECTION		"xconnection"
#define SID_CONNECTION			"x.connection"

struct IConnectionService : public IMsgService {
	virtual bool GetRoute(const char* nodeName, MsgRoute* route) = 0;
};


//----------------------------------------------------------
//xconnection作为独立dll/so使用
//------------------------------------------------------------

struct ConnectionApi;
struct ConnectionCallback{
	virtual void OnConnect(ConnectionApi* api) = 0;
	virtual void OnClose(ConnectionApi* api) = 0;
	virtual void OnRecv(ConnectionApi* api, IMessage* msg) = 0;
};

struct ConnectionApi {
	virtual IMessage* NewMessage() = 0;
	virtual IPack* NewPack() = 0;

	virtual int Init(ConnectionCallback* callback) = 0;
	virtual int UnInit() = 0;

	virtual int Connect(const char* ip, uint16_t port) = 0;
	virtual int Send(IMessage* msg) = 0;
	virtual int SendSync(IMessage* reqMsg, IMessage* ansMsg, uint32_t timeout = 5000) = 0;
};

struct ConnectionServerApi;
struct ConnectionServerCallback{
	virtual void OnConnect(ConnectionServerApi* api) = 0;
	virtual void OnClose(ConnectionServerApi* api) = 0;
	virtual void OnRecv(ConnectionServerApi* api, IMessage* msg) = 0;
};

struct ConnectionServerApi {
	virtual IMessage* NewMessage() = 0;
	virtual IPack* NewPack() = 0;

	virtual int Init(ConnectionServerCallback* callback) = 0;
	virtual int UnInit() = 0;

	virtual int Listen(const char* ip, uint16_t port) = 0; //监听

	virtual int Send(IMessage* msg) = 0;//将数据发回去
};

struct TcpServerApi;
struct TcpServerCallback{

	virtual void OnRecv(TcpServerApi* api, IMessage* msg) = 0;
};

struct  TcpServerApi{
	virtual int Init(TcpServerCallback* callback) = 0;
	virtual int UnInit() = 0;

	virtual int Listen(const char* ip, uint16_t port) = 0; //监听
	virtual int EventLoop() = 0;
};


#ifdef OS_WIN
	#if defined(XCONNECTION_IMPL)
		#define	XCONNECTION_EXPORT __declspec(dllexport)
	#else
		#define XCONNECTION_EXPORT __declspec(dllimport)
	#endif
#else
	#define	XCONNECTION_EXPORT __attribute__((visibility("default")))
#endif

XCONNECTION_EXPORT ConnectionApi*  NewConnectionAPI();
XCONNECTION_EXPORT ConnectionServerApi*  NewConnectionServerAPI();

}//namespace

#endif //XCONNECTION_XCONNECTION_SERVICE_INTF_H_
