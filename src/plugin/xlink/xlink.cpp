#include "xlink.h"
#include "xlink_impl.h"
#include "xlink_intf.h"

namespace x {

	LinkIoc* _connIoc = new LinkIoc();
	LinkMng* _connMng = new LinkMng();
	ILog* _log = NULL;

	LinkIoc::LinkIoc(){

	}

	LinkIoc::~LinkIoc(){

	}

	IContainer* LinkIoc::GetContainer() { 
		return container_;
	}

	XMLElement* LinkIoc::GetConfig() { 
		return config_;
	}

	const char* LinkIoc::GetArg(const char* name) { 
		ArgMap::iterator it = args_.find(std::string(name));
		if(it != args_.end()){
			return it->second.c_str();
		}
		return NULL;
	}

	void LinkIoc::SetContainer(IContainer* container) { 
		container_ = container; 
	}

	void LinkIoc::SetConfig(XMLElement* config) { 
		config_ = config; 
	}

	void LinkIoc::SetArg(const char* name, const char* value) { 
		args_[std::string(name)] = std::string(value);
	}

	LinkMng::LinkMng(){

	}

	LinkMng::~LinkMng(){

	}

	void LinkMng::OnEvent(int eventType){
		switch(eventType){
		case MANAGER_EVENT_INIT : 
			_log = (ILog*)_connIoc->GetContainer()->GetService(SID_LOG);
			//if(_connService) _connService->Init(); 
			//if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_START : 
			//if(_connService) _connService->Start(); 
			//if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_STOP : 
			//if(_connService) _connService->Stop(); 
			//if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_UNINIT : 
			//if(_connService) _connService->UnInit(); 
			//if(_messageService) _messageService->Init();
			break;
		}
	}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_LINK;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
	case 0:
		info.id = SID_LINK;
		info.caption = "";
		info.version = __DATE__":"__TIME__;
		return true;
	default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_LINK) == 0){
		return x::_messageService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_connIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_connMng;
	}

	return NULL;
}


