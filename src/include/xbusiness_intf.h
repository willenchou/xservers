#ifndef X_PLUGIN_XBUSINESS_XBUSINESS_INTF_H_
#define X_PLUGIN_XBUSINESS_XBUSINESS_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>
#include <include/xmessage_intf.h>

namespace x {
	
#define PLUGIN_BUSINESS		"xbusiness"
#define SID_BUSINESS		"x.business"

struct IBusinessContext;

typedef int (* PFBusinessFunc)(IBusinessContext* context, IPack* in, IPack* out);

struct BusinessFuncEntry{
	uint32_t			funcID;
	PFBusinessFunc		funcEntry;
};

struct IBusinessContext {
	//IMessage* NewMessage() = 0;
	//IPack* NewPack() = 0;
};

struct IBusinessService : public IMsgService{

};

}//namespace x

#endif//X_PLUGIN_XBUSINESS_XBUSINESS_INTF_H_
