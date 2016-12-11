#include "xconnection.h"
#include "xmessage_impl.h"
#include "xconnection_impl.h"

namespace x {

ConnectionIoc* _connIoc = new ConnectionIoc();
ConnectionMng* _connMng = new ConnectionMng();
ILog* _log = NULL;

ConnectionIoc::ConnectionIoc(){
	
}

ConnectionIoc::~ConnectionIoc(){
	
}
	
IContainer* ConnectionIoc::GetContainer() { 
	return container_;
}
	
XMLElement* ConnectionIoc::GetConfig() { 
	return config_;
}
	
const char* ConnectionIoc::GetArg(const char* name) { 
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

void ConnectionIoc::SetContainer(IContainer* container) { 
	container_ = container; 
}

void ConnectionIoc::SetConfig(XMLElement* config) { 
	config_ = config; 
}

void ConnectionIoc::SetArg(const char* name, const char* value) { 
	args_[std::string(name)] = std::string(value);
}

ConnectionMng::ConnectionMng(){
	
}

ConnectionMng::~ConnectionMng(){
	
}

void ConnectionMng::OnEvent(int eventType){
	switch(eventType){
		case MANAGER_EVENT_INIT : 
			_log = (ILog*)_connIoc->GetContainer()->GetService(SID_LOG);
			if(_connService) _connService->Init(); 
			if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_START : 
			if(_connService) _connService->Start(); 
			if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_STOP : 
			if(_connService) _connService->Stop(); 
			if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_connService) _connService->UnInit(); 
			if(_messageService) _messageService->Init();
			break;
	}
}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_CONNECTION;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_MESSAGE;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		case 1:
			info.id = SID_CONNECTION;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_MESSAGE) == 0){
		return x::_messageService;
	}else if(strcmp(id, SID_CONNECTION) == 0 || strcmp(id, SID_MSG) == 0){
		return x::_connService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_connIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_connMng;
	}

	return NULL;
}

