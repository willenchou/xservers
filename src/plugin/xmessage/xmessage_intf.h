#ifndef X_PLUGIN_XMESSAGE_XMESSAGE_INTF_H_
#define X_PLUGIN_XMESSAGE_XMESSAGE_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>

namespace x {

#define SID_MESSAGE "xmessage"

#define MSG_TYPE_REQ 0
#define MSG_TYPE_ANS 1

struct RouteInfo {
	char sysName[32+1];
	int32_t sysNo;
	char nodeName[32+1];
	int32_t nodeNo;	
};

struct MsgInfo {
	int32_t type;
	int32_t id;
	int32_t sendFlag;
	int32_t returnCode;
};

struct IMessage : public IRefCounted{
	virtual bool SetBuff(void* data, uint32_t len) = 0;
	virtual bool GetBuff(void** buff, uint32_t& len) = 0;
	
	virtual bool SetData(void* data, uint32_t len) = 0;	
	virtual bool GetData(void** data, uint32_t& len) = 0;	

	virtual void SetSrcRouteInfo(RouteInfo* route) = 0;
	virtual void GetSrcRouteInfo(RouteInfo& route) = 0;
	virtual void SetDestRouteInfo(RouteInfo* route) = 0;
	virtual void GetDestRouteInfo(RouteInfo& route) = 0;
	virtual void SetMsgInfo(MsgInfo* msgInfo) = 0;
	virtual void GetMsgInfo(MsgInfo& msgInfo) = 0;
};


struct IMessageService : public IService {
	virtual IMessage* NewMessage() = 0;
};


}//namespace x


#endif //X_PLUGIN_XMESSAGE_XMESSAGE_INTF_H_