#ifndef XSERVER_PLUGIN_H_
#define XSERVER_PLUGIN_H_

#include <string>
#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include "xbase/os_common.h"
#include <include/xservice_intf.h>

#ifdef _WIN32
# include <windows.h>
# define DLLHANDL HMODULE
# define DLLOPEN(dll) ::LoadLibrary(dll)
# define DLLFUNC(handle, name) GetProcAddress(handle, name)
# define DLLCLOSE(handle) ::FreeLibrary(handle)
#else
# define DLLHANDL void*
# define DLLOPEN(dll) dlopen(dll, RTLD_LAZY);
# define DLLFUNC(handle, name) dlsym(handle, name)
# define DLLCLOSE(handle) dlclose(handle)
#endif

namespace x {

class Plugin {
public:
	inline Plugin(PFGetPluginInfo getPluginInfo, PFGetServiceInfo getServiceInfo, PFGetService getService)
	:getPluginInfo_(getPluginInfo)
	,getServiceInfo_(getServiceInfo)
	,getService_(getService)
	,msgService_(NULL)
	,iocService_(NULL)
	,mngService_(NULL){
		msgService_ = (IMsgService*)getService_(SID_MSG);
		iocService_ = (IIocService*)getService_(SID_IOC);
		mngService_ = (IMngService*)getService_(SID_MANAGER);
	}

	inline ~Plugin(){
	
	}

	inline void GetPluginInfo(PluginInfo& info){ return getPluginInfo_(info); }
	inline bool GetServiceInfo(int index, ServiceInfo& info){ return getServiceInfo_(index, info); }
	inline IService* FUNC_CALL GetService(const char* id){ return getService_(id); }
	
	inline IMsgService* GetMsgService(){ return msgService_;}
	inline IIocService* GetIocService(){ return iocService_;}
	inline IMngService* GetMngService(){ return mngService_;}

private:
	PFGetPluginInfo getPluginInfo_;
	PFGetServiceInfo getServiceInfo_;
	PFGetService getService_;

	IMsgService* msgService_;
	IIocService* iocService_;
	IMngService* mngService_;

};

class PluginMng {
private:
	PluginMng();
	~PluginMng();
public:
	static PluginMng* Instance();

	Plugin* Load(const char* name);
	bool UnLoad(const char* name);

	size_t GetPluginCount();
	Plugin* GetPlugin(size_t index);
	Plugin* GetPlugin(const char* name);


private:
	struct PluginItem {
		DLLHANDL handle;
		Plugin* plugin;
	};

	typedef hash_map<std::string, PluginItem*> PluginMap;

	static PluginMng* instance_;
	PluginMap plugins_;
};

} //namespace x

#endif //XSERVER_PLUGIN_H_
