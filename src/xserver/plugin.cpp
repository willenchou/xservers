#include "plugin.h"

namespace x {

PluginMng* PluginMng::instance_ = NULL;

PluginMng::PluginMng(){

}

PluginMng::~PluginMng(){
	PluginMap::iterator it = plugins_.begin();
	for(; it != plugins_.end(); it++){
		UnLoad(it->first.c_str());
	}
}

PluginMng* PluginMng::Instance(){
	if(!instance_){
		instance_ = new PluginMng();
	}
	return instance_;
}

Plugin* PluginMng::Load(const char* name){
	char file[1024];
#ifdef OS_WIN
	sprintf(file, "%s.dll", name);
#else
	sprintf(file, "lib%s.so", name);
#endif
	
	DLLHANDL handle = DLLOPEN(file);
	if(!handle){
		return NULL;
	}

	PFGetPluginInfo getPluginInfo = (PFGetPluginInfo)DLLFUNC(handle, "GetPluginInfo");
	PFGetServiceInfo getServiceInfo = (PFGetServiceInfo)DLLFUNC(handle, "GetServiceInfo");
	PFGetService getService = (PFGetService)DLLFUNC(handle, "GetService");
	
	if(!(getPluginInfo && getServiceInfo && getService)){
		DLLCLOSE(handle);
		return NULL;
	}

	PluginItem* item = new PluginItem();
	item->handle = handle;
	item->plugin = new Plugin(getPluginInfo, getServiceInfo, getService);

	plugins_[std::string(name)] = item;

	return item->plugin;
}

bool PluginMng::UnLoad(const char* name){
	PluginMap::iterator it = plugins_.find(std::string(name));
	if(it != plugins_.end()){
		DLLCLOSE(it->second->handle);
		delete it->second->plugin;
		plugins_.erase(it);
		return true;
	}

	return false;
}

size_t PluginMng::GetPluginCount(){
	return plugins_.size();
}

Plugin* PluginMng::GetPlugin(size_t index){
	size_t count = GetPluginCount();

	if(index >= count) return NULL;
	
	size_t i = 0;
	PluginMap::iterator it = plugins_.begin();
	for(it; it != plugins_.end(); it++){
		if(i == index){
			return it->second->plugin;
		}
		i++;
	}

	return NULL;
}

Plugin* PluginMng::GetPlugin(const char* name){
	PluginMap::iterator it = plugins_.find(std::string(name));
	if(it != plugins_.end()){
		return it->second->plugin;	
	}
	return NULL;
}

} //namespace x
