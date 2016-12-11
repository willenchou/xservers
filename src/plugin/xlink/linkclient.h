#ifndef XLINK_CLIENT_H_
#define XLINK_CLIENT_H_
#include "event_dispatch.h"
#include "eventthreadpool.h"
#include "include/xlink_intf.h"
#include "connector.h"
namespace x{

	class Linker:public ILinkClient,Connector::ConnectorCallback{
	public:
		Linker(EventDispatch* dispatch);
		~Linker(void);
	public:
		bool Connect(const char* ip, uint16_t port);
		int Send(IMessage* msg);
	public:
		void SetCallBack(LinkClientCallback* callback);
	protected:
		virtual void OnRecv(Connector* conn, IMessage* msg);
		virtual void OnClose(Connector* conn);
		virtual void OnConnected(Connector* conn);
	private:
		EventDispatch* dispatch_;
		Connector* connector;

		LinkClientCallback* callback_;

		int status_;
	};
}
#endif//XLINK_CLIENT_H_


