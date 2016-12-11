#include "container.h"
#include "plugin.h"
#include "args.h"

namespace x {

Container::Container(){
}

Container::~Container(){
}

bool Container::Init(XMLDocument* doc){
	xmlDoc_ = doc;
	if(!LoadPlugin()){
		return false;
	}

	if(!LoadServiceInfo()){
		return false;
	}

	if(!SetServiceIoc()){
		return false;
	}
	
	if(!SetServiceMng(MANAGER_EVENT_INIT)){
		return false;
	}

	return true;
}

bool Container::Start(){
	if(!SetServiceMng(MANAGER_EVENT_START)){
		return false;
	}

	return true;
}

IService* Container::GetService(const char* plugin, const char* id){
	PluginMap::iterator it = plugins_.find(std::string(plugin));
	if(it != plugins_.end()){
		return it->second->plugin->GetService(id);
	}

	return NULL;
}

IService* Container::GetService(const char* id){
	ServiceInfoMap::iterator it = serviceInfos_.find(std::string(id));
	if(it == serviceInfos_.end()){
		return NULL;
	}

	IService* service = it->second->plugin->plugin->GetService(id);

	return service;
}

bool Container::LoadPlugin(){
	XMLElement* rootNode = xmlDoc_->RootElement();
	if(!rootNode){
		return false;
	}

	XMLElement* pluginsNode = rootNode->FirstChildElement("plugins");
	if(!pluginsNode){
		return false;
	}
	
	PluginMng* pluginMng = PluginMng::Instance();

	XMLElement* pluginNode = pluginsNode->FirstChildElement("plugin");
	for(;pluginNode; pluginNode = pluginNode->NextSiblingElement("plugin")){
		const char* name = pluginNode->Attribute("name");
		if(name){
			Plugin* plugin = pluginMng->Load(name);
			if(plugin){
				PluginItem* item = new PluginItem();
				strcpy(item->name, name);
				item->config = pluginNode;
				item->plugin = plugin;
				item->inited = false;
				plugins_[std::string(name)] = item;
			}
		}
	}

	return true;
}

bool Container::LoadServiceInfo() {
	for(PluginMap::iterator it = plugins_.begin(); it != plugins_.end(); it++){
		Plugin* plugin = it->second->plugin;

		int index = 0;
		ServiceInfo serviceInfo;
		while(plugin->GetServiceInfo(index++, serviceInfo)){
			ServiceInfoItem* item = new ServiceInfoItem();
			memcpy(&(item->info), &serviceInfo, sizeof(ServiceInfo));
			item->plugin = it->second;
			serviceInfos_[std::string(serviceInfo.id)] = item;
		}
	}

	return true;
}

bool Container::SetServiceIoc(){
	for(PluginMap::iterator it = plugins_.begin(); it != plugins_.end(); it++){
		Plugin* plugin = it->second->plugin;
		IIocService* ioc = plugin->GetIocService();
		if(!ioc) continue;

		ioc->SetContainer(this);
		ioc->SetConfig(it->second->config);
		
		Args::ArgMap args = Args::Instance()->GetArgs();
		for(Args::ArgMap::iterator it = args.begin(); it != args.end(); it++){
			ioc->SetArg(it->first.c_str(), it->second.c_str());	
		}
	}
	return true;
}

bool Container::SetServiceMng(int eventType){
	for(PluginMap::iterator it = plugins_.begin(); it != plugins_.end(); it++){
		Plugin* plugin = it->second->plugin;
		IMngService* mng = plugin->GetMngService();
		if(!mng) continue;

		mng->OnEvent(eventType);
	}
	return true;
}

Container::PluginItem* Container::FindPlugin(const char* name){
	for(PluginMap::iterator it = plugins_.begin(); it != plugins_.end(); it++){
		if(strcmp(name, it->second->name) == 0){
			return it->second;
		}
	}
	return NULL;
}

Container::PluginItem* Container::FindPluginByService(const char* serviceID){
	for(ServiceInfoMap::iterator it = serviceInfos_.begin(); it != serviceInfos_.end(); it++){
		if(strcmp(serviceID, it->second->plugin->name) == 0){
			return it->second->plugin;
		}
	}
	return NULL;
}

}//namespace x
