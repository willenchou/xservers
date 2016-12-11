#include "xdb.h"

#include "xdb_impl.h"

namespace x {

	DBIoc* _dbIoc = new DBIoc();
	DBMng* _dbMng = new DBMng();
	ILog* _log = NULL;
	DBIoc::DBIoc(){

	}

	DBIoc::~DBIoc(){

	}

	IContainer* DBIoc::GetContainer() { 
		return container_;
	}

	XMLElement* DBIoc::GetConfig() { 
		return config_;
	}

	const char* DBIoc::GetArg(const char* name) { 
		ArgMap::iterator it = args_.find(std::string(name));
		if(it != args_.end()){
			return it->second.c_str();
		}
		return NULL;
	}

	void DBIoc::SetContainer(IContainer* container) { 
		container_ = container; 
	}

	void DBIoc::SetConfig(XMLElement* config) { 
		config_ = config; 
	}

	void DBIoc::SetArg(const char* name, const char* value) { 
		args_[std::string(name)] = std::string(value);
	}

	DBMng::DBMng(){

	}

	DBMng::~DBMng(){

	}

	void DBMng::OnEvent(int eventType){
		switch(eventType){
		case MANAGER_EVENT_INIT : 
			_log = (ILog*)_dbIoc->GetContainer()->GetService(SID_LOG);
			if(_dbService) _dbService->Init(); 
			
			break;
		case MANAGER_EVENT_START : 
			if(_dbService) _dbService->Start(); 
			break;
		case MANAGER_EVENT_STOP : 
			if(_dbService) _dbService->Stop(); 
			//if(_messageService) _messageService->Init();
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_dbService) _dbService->UnInit(); 
			//if(_messageService) _messageService->Init();
			break;
		}
	}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_DB;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
	case 0:
		info.id = SID_DB;
		info.caption = "";
		info.version = __DATE__":"__TIME__;
		return true;
	default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_DB) == 0 ){
		return x::_dbService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_dbIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_dbMng;
	}

	return NULL;
}
