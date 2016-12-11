#ifndef XMESSAGE_PLUGIN_H_
#define XMESSAGE_PLUGIN_H_

#include <xserver/service_intf.h>

#ifdef __cplusplus
extern "C" 
{
#endif

void FUNC_CALL GetPluginInfo(x::PluginInfo& info);
bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info);
x::IService* FUNC_CALL GetService(const char* id);

#ifdef __cplusplus
}
#endif

extern x::IContainer* _containner;

#endif //XMESSAGE_PLUGIN_H_
