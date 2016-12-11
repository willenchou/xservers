#include "xmessage_intf.h"
#include <xbase/ref_counted.h>
#include <xbase/binary_buff.h>

namespace x{

class Message : public RefCountedThreadSafe<Message, IMessage> {
public:
	Message();
	virtual ~Message();

	virtual bool SetBuff(void* data, uint32_t len);
	virtual bool GetBuff(void** buff, uint32_t& len);
	
	virtual bool SetData(void* data, uint32_t len);	
	virtual bool GetData(void** data, uint32_t& len);

	virtual void SetSrcRouteInfo(RouteInfo* route);
	virtual void GetSrcRouteInfo(RouteInfo& route);
	virtual void SetDestRouteInfo(RouteInfo* route);
	virtual void GetDestRouteInfo(RouteInfo& route);
	virtual void SetMsgInfo(MsgInfo* msgInfo);
	virtual void GetMsgInfo(MsgInfo& msgInfo);

private:
	bool EncodeBuff();
	bool DecodeBuff();

	RouteInfo routeDest_;
	RouteInfo routeSrc_;
	MsgInfo msgInfo_;
	BinaryBuff buff_;
	BinaryBuff dataBuff_;
};

}//namesapce x
