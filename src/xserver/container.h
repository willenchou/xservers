#ifndef XSERVER_CONTAINER_H_
#define XSERVER_CONTAINER_H_

#include <string>
#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <xbase/xml.h>
#include <include/xservice_intf.h>
#include "plugin.h"

namespace x {

class Container : public IContainer{
public:
	Container();
	~Container();

	bool Init(XMLDocument* doc);
	bool Start();

protected:
	virtual IService* GetService(const char* plugin, const char* id);
	virtual IService* GetService(const char* id);

private:
	struct PluginItem {
		char name[128];
		XMLElement* config;
		Plugin* plugin;
		bool inited;
	};

	struct ServiceInfoItem {
		ServiceInfo info;
		PluginItem* plugin;
	};
	
	bool LoadPlugin();
	bool LoadServiceInfo();
	bool SetServiceIoc();
	bool SetServiceMng(int eventType);

	PluginItem* FindPlugin(const char* name);
	PluginItem* FindPluginByService(const char* serviceID);

	typedef hash_map<std::string, PluginItem*> PluginMap;
	typedef hash_map<std::string, ServiceInfoItem*> ServiceInfoMap; 

	XMLDocument* xmlDoc_;
	PluginMap plugins_;
	ServiceInfoMap serviceInfos_;
};

}//namespace x

#endif //XSERVER_CONTAINER_H_
