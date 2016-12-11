#ifndef XCONNECTION_XCONNECTION_IMPL_H_
#define XCONNECTION_XCONNECTION_IMPL_H_

#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <string>
#include <vector>
#include <include/xconnection_intf.h>
#include <xbase/ref_counted.h>
#include <xbase/thread.h>
#include <xbase/mutex.h>
#include <include/xlog_intf.h>
#include "xconnection_common.h"
#include "network_config.h"
#include "connection.h"

namespace x {

#define DEFAULT_NETOWRK_FILE "network.xml"
#define TIMER_INTERVAL 10

class ConnectionService : public RefCountedThreadSafe<ConnectionService, IConnectionService> 
						, public Connection::Callback
						, public EventDispatch::Callback
						, public Thread
{
public:
	ConnectionService();
	virtual ~ConnectionService();

	void Init();
	void UnInit();
	void Start();
	void Stop();

	bool Send(IMessage* msg);

protected:
	virtual long Run();
	
	virtual IMessage* NewMessage();
	virtual IPack* NewPack();
	virtual bool GetRoute(const char* nodeName, MsgRoute* route);

	virtual void RegisterPre(IMsgService* service);
	virtual void RegisterNext(IMsgService* service);
	virtual void PostMsg(IMessage* msg, IMsgService* from);
	
	virtual void OnConnect(Connection* conn, Socket* socket);
	virtual void OnConfirm(Connection* conn);
	virtual void OnRecv(Connection* conn, IMessage* msg);
	virtual void OnClose(Connection* conn);

	virtual void OnTimer(int id);

private:
	struct ClientConn{
		char			name[128];				//[name]#[id]@[sysNo]#[subSysNo]
		int8_t			direction;				//0:发起端 1:接收端				
		uint32_t		sysNo;
		uint32_t		subSysNo;
		char			appName[32];
		uint32_t		appNo;	
		int32_t			connectionID;
		char			IP[32];		
		uint16_t		port;
		int8_t			status;					//0:连接未建立 1:连接建立中 2:连接已建立 3:连接已注册
		Connection* 	conn;
		uint64_t		connectTime;
	};
	typedef hash_map<std::string, ClientConn*> ClientConnMap;
	typedef std::vector<ClientConn*> ClientConnVector;

	ClientConn* FindClient(Connection* conn);
	void FuncReqRegisterConnection(Connection* conn, IMessage* msg);
	void FuncAnsRegisterConnection(Connection* conn, IMessage* msg);

	uint32_t	sysNo_;				//系统号
	uint32_t	subSysNo_;			//子系统号
	char		appName_[32];		//应用名
	uint32_t    appNo_;				//应用编号

	IMsgService* preMsgService_;
	IMsgService* nextMsgService_;

	IMessageService* msgService_;

	bool initResult_;

	int timerID_;
	NetworkConfig* networkCfg_;
	Connection* server_;
	ClientConnMap clientMap_;
	ClientConnVector clients_;
	Mutex clientMutex_;
	int32_t clientIndex_;
	//MsgRoute route_;
};

extern ConnectionService* _connService;

}//namespace x


#endif //XCONNECTION_XCONNECTION_IMPL_H_
