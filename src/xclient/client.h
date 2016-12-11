#ifndef XCLIENT_CLIENT_H_
#define XCLIENT_CLIENT_H_

#include <include\xmessage_intf.h>
#include <include\xconnection_intf.h>
#include <xbase/xml.h>
#include <xbase/thread.h>

#define CONNECTION_CFG_FILE "xclient.xml"

struct ConnCfg {
	int				id;
	const char*		name;
	const char*		ip;
	const char*		port;
};

struct ColItem {
	const char*		name;
	const char*		type;
	const char*		value;
	ColItem*		next;
};

struct FuncTest{
	int				funcID;
	ColItem*		inputs;
};

class Client : public x::ConnectionCallback {
public:
	Client();
	~Client();

	virtual void OnConnect(x::ConnectionApi* api);
	virtual void OnClose(x::ConnectionApi* api);
	virtual void OnRecv(x::ConnectionApi* api, x::IMessage* msg);

	bool Connect();
	void Test(char* file);

private:
	void LoadConnCfg();
	void LoadTestCfg(char* file);

	x::XMLDocument connDoc_;
	ConnCfg** connCfgs_;
	int connCfgCount_;

	x::XMLDocument testDoc_;
	FuncTest** testCfgs_;
	int testCfgCount_;

	int connectStatus_;
	x::ConnectionApi* api_;
};

#endif //XCLIENT_CLIENT_H_
