#include "xroute.h"
#include <xbase/util.h>
#include <include/xmessage_intf.h>
#include <include/xconnection_intf.h>

namespace x{

RouteService* _routeService = new RouteService();
RouteIoc* _routeIoc = new RouteIoc();
RouteMng* _routeMng = new RouteMng();
ILog* _log = NULL;

RouteService::RouteService()
:preMsgService_(NULL)
,nextMsgService_(NULL)
,pipeService_(NULL)
,connectionService_(NULL)
,routeCount_(0){

}

RouteService::~RouteService(){

}

void RouteService::Init(){
	pipeService_ = (IPipeService*)_routeIoc->GetContainer()->GetService(SID_PIPE);
	connectionService_ = (IConnectionService*)_routeIoc->GetContainer()->GetService(SID_CONNECTION);
	
	const char* strSysNo = _routeIoc->GetArg(ARGS_SYS_NO);
	sysNo_ = strSysNo ? atoi(strSysNo) : 0;
	const char* strSubSysNo = _routeIoc->GetArg(ARGS_SUB_SYS_NO);
	subSysNo_ = strSubSysNo ? atoi(strSubSysNo) : 0;
	const char* strAppName = _routeIoc->GetArg(ARGS_APP_NAME);
	Util::StrCopy(appName_, strAppName ? strAppName : "");
	const char* strAppNo = _routeIoc->GetArg(ARGS_APP_NO);
	appNo_ = strAppNo ? atoi(strAppNo) : 0; 

	XMLElement* config = _routeIoc->GetConfig();
	if(!config) {
		return;
	}

	XMLElement* nodeRoutes = config->FirstChildElement("routes");
	if(nodeRoutes){
		XMLElement* nodeRoute = nodeRoutes->FirstChildElement("route");
		while(nodeRoute){
			RouteItem* item = new RouteItem();
			
			const char* str = NULL;
			str = nodeRoute->Attribute("system_no");
			if(str){
				item->hasSysNo = true;
				item->sysNo = atoi(str);
			}else{
				item->hasSysNo = false;
			}
			str = nodeRoute->Attribute("sub_system_no");
			if(str){
				item->hasSubSysNo = true;
				item->subSysNo = atoi(str);
			}else{
				item->hasSubSysNo = false;
			}
			str = nodeRoute->Attribute("function_id");
			if(str){
				item->hasFunctions = true;
				Util::StrCopy(item->functions, str);
			}else{
				item->hasFunctions = false;
			}
			str = nodeRoute->Attribute("plugin");
			if(str){
				item->hasPlugin = true;
				Util::StrCopy(item->plugin, str);
			}else{
				item->hasPlugin = false;
			}
			str = nodeRoute->Attribute("node");
			if(str){
				item->hasNode = true;
				Util::StrCopy(item->node, str);
			}else{
				item->hasNode = false;
			}
			
			routes_[routeCount_++] = item;

			nodeRoute = nodeRoute->NextSiblingElement("route");
		}
	}
}

void RouteService::UnInit(){

}

void RouteService::Start(){

}

void RouteService::Stop(){

}

void RouteService::RegisterPre(IMsgService* service){
	//preMsgService_ = service;
}

void RouteService::RegisterNext(IMsgService* service){
	//nextMsgService_ = service;
}

void RouteService::PostMsg(IMessage* msg, IMsgService* from){
	bool routeRet = false;
	
	void* data = NULL;
	uint32_t dataLen = 0u;
	if(msg->GetData(MSG_TAG_ROUTE_DEST, &data, &dataLen)){
		MsgRoute* route = (MsgRoute*)data;
		if(route->sysNo == sysNo_ &&
			route->subSysNo == subSysNo_ &&
			strcmp(route->appName, appName_) == 0 &&
			route->appNo == appNo_ &&
			route->plugin[0] != '\0')
		{
			routeRet = SendMsgToPlugin(msg, route->plugin);
		}else{
			routeRet = SendMsgToPlugin(msg, PLUGIN_CONNECTION);
		}
	}else{
		void* data = NULL;
		uint32_t dataLen = 0u;
		if(msg->GetData(MSG_TAG_FUNC_INFO, &data, &dataLen)){
			MsgFuncInfo* funcInfo = (MsgFuncInfo*)data;
		
			for(uint32_t i = 0; i < routeCount_; i++){
				RouteItem* item = routes_[i];
				
				if(item->hasSysNo && (item->sysNo != funcInfo->sysNo))
					continue;
				if(item->hasSubSysNo && (item->subSysNo != funcInfo->subSysNo))
					continue;
				if(item->hasFunctions && !MatchFunctions(funcInfo->funcID, item->functions))
					continue;

				if(item->hasPlugin){
					if(RouteToPlugin(msg, item->plugin)){
						routeRet = true;
						break;
					}
				}else if(item->hasNode){
					if(RouteToNode(msg, item->node)){
						routeRet = true;
						break;
					}
				}
			}
		}
	}

	if(!routeRet){
		msg->ChangeReq2Ans();
		msg->SetReturnID(MSG_RETURN_CODE_ERR_TRANSFORM);
		SendMsgToPlugin(msg, PLUGIN_CONNECTION);
	}
}

bool RouteService::SendMsgToPlugin(IMessage* msg, const char* pluginName){
	PipeItem* pipe = pipeService_->GetPipeline(pluginName);
	while(pipe){
		if(pipe->next->next == NULL){
			pipe->servcie->PostMsg(msg, this);
			return true;
		}
	}
	return false;
}

bool RouteService::RouteToPlugin(IMessage* msg, const char* pluginName){
	/*
	MsgRoute route;
	route.sysNo = sysNo_;
	route.subSysNo = subSysNo_;
	Util::StrCopy(route.appName, appName_);
	route.appNo = appNo_;
	route.connectionID = -1;
	Util::StrCopy(route.plugin, pluginName);
	msg->SetData(MSG_TAG_ROUTE_DEST, &route, sizeof(route));
	*/

	return SendMsgToPlugin(msg, pluginName);
}

bool RouteService::RouteToNode(IMessage* msg, const char* nodeName){
	char strNodeName[128];
	Util::StrCopy(strNodeName, nodeName);

	char* appName = strNodeName;
	char* appNo = NULL;
	char* sysNo = NULL;
	char* subSysNo = NULL;

	char* pos = strNodeName;
	while(*pos != '\0'){
		if(*pos == '#' && *(pos+1) != '\0'){
			*pos = '\0';
			if(!appNo){
				appNo = pos+1;
			}else{
				subSysNo = pos+1;
			}
		}

		if(*pos == '@' && *(pos+1) != '\0'){
			if(!sysNo)
				sysNo = pos+1;
		}
		pos++;
	}
	
	MsgRoute route;
	route.sysNo = sysNo ? atoi(sysNo) : 0;
	route.subSysNo = subSysNo ? atoi(subSysNo) : 0;
	Util::StrCopy(route.appName, appName);
	route.appNo = appNo ? atoi(appNo) : 0;
	route.connectionID = -1;
	route.plugin[0] = '\0';
	
	msg->SetData(MSG_TAG_ROUTE_INTERNAL, &route, sizeof(route));
	
	return SendMsgToPlugin(msg, PLUGIN_CONNECTION);
}

int RouteService::MatchFunctions(uint32_t function, const char* functions){
	char szSub[32];
	sprintf(szSub, "%d", function);

	int inset;
	const char *pSub, *pWild, *pSubTail, *pWildTail, *pSavedSubTail = NULL, *pSavedWildTail;
	for (pSub = szSub, pWild = functions; *pWild != '\0'; pWild ++)
	{
		if ((*pWild == *pSub) || ((*pWild == '?') && (*pSub != '\0')) ) // 当前字符匹配
		{
			pSub ++;
		}
		else if (*pWild == ';') // 本节结束
		{
			if (*pSub == '\0')
			{
				return 1;
			}
			pSub = szSub;
		} // ';'
		else if (*pWild == '*')
		{
			// 找尾
			pWildTail = pWild + 1;
			if (*pWildTail == ';')
			{
				return 1;
			}
			while (*(++pWildTail) != ';');
			pSavedWildTail = pWildTail;
			pWildTail --;
			if (pSavedSubTail == NULL)
			{
				pSavedSubTail = pSub;
				while (*(++pSavedSubTail) != '\0');
				pSavedSubTail --;
			}
			pSubTail = pSavedSubTail;
			// 逆向匹配
			//for (pSubTail = pSavedSubTail; pSubTail > pSub; pWildTail --)
			while (pSubTail > pSub)
			{
				if ((*pWildTail == *pSubTail) || (*pWildTail == '?')) // 当前字符匹配
				{
					pSubTail --;
				}
				else if (pWildTail == pWild) // 结束
				{
					return 1;
				}
				else if (*pWildTail == ']')
				{
					inset = 0;
					do
					{
						pWildTail --;
						if (*pWildTail == *pSubTail) // 在集合中
						{                                                             inset = 1;
							pSubTail --;
							while (*(--pWildTail) != '['); // 跳过集合剩余部分
							break;
						}
					} while (*pWildTail != '[');
					if (inset == 0) // 不在集合中, 准备匹配下一节
					{
						pSub = szSub;
						pWild = pSavedWildTail;
						break; // for
					}
				}
				else
				{
					pSub = szSub;
					pWild = pSavedWildTail;
					break; // for
				}
				pWildTail --;
			}
		} // '*'
		else if (*pWild == '[')
		{
			inset = 0;
			do
			{
				pWild ++;
				if (*pWild == *pSub) // 在集合中
				{
					inset = 1;
					pSub ++;
					while (*(++pWild) != ']'); // 跳过集合剩余部分
					break;
				}
			} while (*pWild != ']');
			if (inset == 0) // 不在集合中, 准备匹配下一节
			{
				pSub = szSub;
				while (*(++pWild) != ';');
			}
		} // '['
		else // 不匹配, 准备匹配下一节
		{
			pSub = szSub;
			while (*(++pWild) != ';');
		}
	}
	 return 0;
}

RouteIoc::RouteIoc(){
	
}

RouteIoc::~RouteIoc(){
	
}
	
IContainer* RouteIoc::GetContainer() { 
	return container_;
}
	
XMLElement* RouteIoc::GetConfig() { 
	return config_;
}
	
const char* RouteIoc::GetArg(const char* name) { 
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

void RouteIoc::SetContainer(IContainer* container) { 
	container_ = container; 
}

void RouteIoc::SetConfig(XMLElement* config) { 
	config_ = config; 
}

void RouteIoc::SetArg(const char* name, const char* value) { 
	args_[std::string(name)] = std::string(value);
}

RouteMng::RouteMng(){
	
}

RouteMng::~RouteMng(){
	
}

void RouteMng::OnEvent(int eventType){
	switch(eventType){
		case MANAGER_EVENT_INIT : 
			if(_routeService) _routeService->Init(); 
			break;
		case MANAGER_EVENT_START : 
			if(_routeService) _routeService->Start(); 
			break;
		case MANAGER_EVENT_STOP : 
			if(_routeService) _routeService->Stop(); 
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_routeService) _routeService->UnInit(); 
			break;
	}
}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_ROUTE;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_ROUTE;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_ROUTE) == 0 || strcmp(id, SID_MSG) == 0){
		return x::_routeService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_routeIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_routeMng;
	}

	return NULL;
}
