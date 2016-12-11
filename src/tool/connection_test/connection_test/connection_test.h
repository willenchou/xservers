#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <include/xconnection_intf.h>

class Callback : public x::ConnectionCallback{
public:
	Callback(x::ConnectionApi* api);
	~Callback();

	virtual void OnConnect(x::ConnectionApi* api);
	virtual void OnClose(x::ConnectionApi* api);
	virtual void OnRecv(x::ConnectionApi* api, x::IMessage* msg);

private:
	void ShowRoute(x::IMessage* msg, const char* caption, uint32_t tagID);

	x::ConnectionApi* api_;
};


#endif //CONNECTION_H_
