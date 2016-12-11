#ifndef X_PLUGIN_XPIPE_XPIPE_INTF_H_
#define X_PLUGIN_XPIPE_XPIPE_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>

namespace x {
	
#define PLUGIN_PIPE		"xpipe"
#define SID_PIPE		"x.pipe"

struct PipeItem {
	char plugin[32];
	IMsgService* servcie;
	PipeItem* next;
};

struct IPipeService : public IService{
	virtual PipeItem* GetPipeline(const char* firstPlugin) = 0;
};

}//namespace x

#endif//X_PLUGIN_XPIPE_XPIPE_INTF_H_
