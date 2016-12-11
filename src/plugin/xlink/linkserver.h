
#pragma once
#include "event_dispatch.h"
#include "eventthreadpool.h"
#include "include/xlink_intf.h"
#include "include/linkargs.h"
#include "acceptor.h"

#include <map>
#include "connector.h"

namespace x {
	class LinkServer:public LinkServerApi,Acceptor::AcceptorCallback,Connector::ConnectorCallback
		//Iconnectorcallback
		//Iacceptorcallback
	{
	public:
		LinkServer(void);
		~LinkServer(void);
	public:
		//ITcpServer,LinkServerApi
		virtual int Init(LinkServerCallback* callback,LinkServerArgs& args);
		virtual int UnInit();
		virtual int Start();
		virtual int Send(IMessage* msg);
		

	protected:
		//Iacceptorcallback
		virtual void OnAccept(Connection* conn);
	protected:
		//IConnectorCallback
		virtual void OnRecv(Connector* conn, IMessage* msg);
		virtual void OnConnected(Connector* conn);
		virtual void OnClose(Connector* conn);
		
		virtual Connector* NewConnector();
	private:
		void newConnector(Connection* conn);

		void FuncReqRegisterConnection(Connector* conn, IMessage* msg);
		//查找校验该链接是否还有效存在，可优化
		Connector* FindClient(Connector* conn);
		//void FuncAnsRegisterConnection(Connection* conn, IMessage* msg);

	private:
		LinkServerCallback* callback_;
		LinkServerArgs args_;

		typedef hash_map<std::string, Connector*> ConnectorMap;
		typedef std::vector<Connector*> ConnectorVector;

		

		//accept
		Acceptor* acceptConn_;
		EventDispatch* dispatch_;
		EventThread* acceptThread_;
		bool userAcceptThread_;

		//connect client //PS:看看是否直接复用 linkfactory
		EventThreadPool* threadpool_;
		ConnectorVector clientconns_;
		int32_t clientIndex_;
		//Linkserverargs

		Mutex clientMutex_;
	};
}


