#ifndef X_PLUGIN_XROUTE_XROUTE_H_
#define X_PLUGIN_XROUTE_XROUTE_H_

#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <string>
#include <vector>
#include <include/xlog_intf.h>
#include <include/xroute_intf.h>
#include <include/xpipe_intf.h>
#include <include/xconnection_intf.h>

namespace x{

#define ROUTE_ITEM_MAX_COUNT 1024

class RouteService : public RefCountedThreadSafe<RouteService, IRouteService>{
public:
	RouteService();
	~RouteService();

	void Init();
	void UnInit();
	void Start();
	void Stop();

protected:
	virtual void RegisterPre(IMsgService* service);
	virtual void RegisterNext(IMsgService* service);
	virtual void PostMsg(IMessage* msg, IMsgService* from);

private:
	struct RouteItem {
		bool hasSysNo;
		uint32_t sysNo;
		bool hasSubSysNo;
		uint32_t subSysNo;
		bool hasFunctions;
		char functions[128];
		bool hasPlugin;
		char plugin[32];
		bool hasNode;
		char node[128];
	};

	bool SendMsgToPlugin(IMessage* msg, const char* pluginName);
	bool RouteToPlugin(IMessage* msg, const char* pluginName);
	bool RouteToNode(IMessage* msg, const char* nodeName);
	int MatchFunctions(uint32_t function, const char* functions);

	IMsgService* preMsgService_;
	IMsgService* nextMsgService_;
	IPipeService* pipeService_;
	IConnectionService* connectionService_;
	
	uint32_t	sysNo_;				//系统号
	uint32_t	subSysNo_;			//子系统号
	char		appName_[32];		//应用名
	uint32_t    appNo_;				//应用编号

	RouteItem* routes_[ROUTE_ITEM_MAX_COUNT];
	uint32_t routeCount_;
};

class RouteIoc : public RefCountedThreadSafe<RouteIoc, IIocService> {
public:
	RouteIoc();
	~RouteIoc();
	
	IContainer* GetContainer();
	XMLElement* GetConfig();
	const char* GetArg(const char* name);

protected:
	virtual void SetContainer(IContainer* container);
	virtual void SetConfig(XMLElement* config);
	virtual void SetArg(const char* name, const char* value);

private:
	typedef hash_map<std::string, std::string> ArgMap;
	IContainer* container_;
	XMLElement* config_;
	ArgMap args_;
};

class RouteMng : public RefCountedThreadSafe<RouteMng, IMngService> {
public:
	RouteMng();
	~RouteMng();

protected:
	virtual void OnEvent(int eventType);
};

extern RouteService* _routeService;
extern RouteIoc* _routeIoc;
extern RouteMng* _routeMng;
extern ILog* _log;

}//namespace x

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



#endif //X_PLUGIN_XPIPE_XPIPE_H_
