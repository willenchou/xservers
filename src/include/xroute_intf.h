#ifndef X_PLUGIN_XROUTE_XROUTE_INTF_H_
#define X_PLUGIN_XROUTE_XROUTE_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>

namespace x {
	
#define PLUGIN_ROUTE	"xroute"
#define SID_ROUTE		"x.route"

struct IRouteService : public IMsgService{

};

}//namespace x

#endif//X_PLUGIN_XPIPE_XPIPE_INTF_H_
