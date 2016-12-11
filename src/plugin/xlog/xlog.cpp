#include "xlog.h"
#ifdef OS_WIN
#include <windows.h>
#endif
#include <include/xservice_intf.h>

namespace x {

LogService* _logService = new LogService();
LogIocService* _logIocService = new LogIocService();
LogMngService* _logMngService = new LogMngService();

LogService::LogService(){
	isSync_ = true;
	level_ = LOG_LEVEL_DEBUG;
}

LogService::~LogService(){

}

void LogService::Init(){
	CreateLogDir();
	
	XMLElement* config = _logIocService->GetConfig();
	if(config) {
		XMLElement* argsNode = config->FirstChildElement("args");
		if(argsNode) {
			isSync_ = argsNode->BoolAttribute("sync");
			level_ = argsNode->IntAttribute("level");
		}
	}
}

void LogService::UnInit(){

}

void LogService::Start(){
	if(!isSync_){
		Thread::Start();
	}
}

void LogService::Stop(){
	if(!isSync_){
		Thread::Stop();
	}
}

long LogService::Run(){
	while(1){
		{
			ScopedLock lock(mutex_);
			LogItem* item = queue_.Top(); 
			while(item){
				
				WriteLog(item->time, item->level, item->content);

				queue_.Pop();
				item = queue_.Top();
			}
		}
		event_.Wait(100);
	}
	return 0L;
}

void LogService::AddLog(char* owner, int level, const char* location, char* content){
	if(level < level_) return;
	
	time_t currTime;
	time(&currTime);
	struct tm * timeinfo = localtime(&currTime);

	char logStr[1024];
	snprintf(logStr, sizeof(logStr)-1, "[%02d:%02d:%02d][%d][%s][%s][%s]", 
		timeinfo->tm_hour, 
		timeinfo->tm_min, 
		timeinfo->tm_sec,
		level, 
		owner, 
		content,
		location);

	if(isSync_){
		WriteLog(currTime, level, logStr);
	}else{
		ScopedLock lock(mutex_);
		
		LogItem* item = queue_.Bottom();
		if(item){
			item->time = currTime;
			item->level = level;
			strncpy(item->content, logStr, sizeof(item->content));
		}

		queue_.Push();
		event_.Reset();
	}
}

void LogService::CreateLogDir(){
	if(ACCESS(LOG_DIR, 0) != 0){
		MKDIR(LOG_DIR);
	}
}

void LogService::WriteLog(time_t time, int level, char* str){
	struct tm * timeinfo = localtime(&time);

	char filePath[1024];
	snprintf(filePath, sizeof(filePath), "%s/%04d-%02d-%02d-%s.log", 
		LOG_DIR, 
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		_logIocService->GetArg(ARGS_APP_NAME));

	FILE* f = fopen(filePath, "a+");
	if(f){
		fputs(str, f);
		fputs("\n", f);
		fclose(f);
	}

	if(level == LOG_LEVEL_DEBUG)
		printf("%s\n", str);
}

LogIocService::LogIocService(){
	
}

LogIocService::~LogIocService(){
	
}
	
IContainer* LogIocService::GetContainer() { 
	return container_;
}
	
XMLElement* LogIocService::GetConfig() { 
	return config_;
}
	
const char* LogIocService::GetArg(const char* name) { 
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

void LogIocService::SetContainer(IContainer* container) { 
	container_ = container; 
}

void LogIocService::SetConfig(XMLElement* config) { 
	config_ = config; 
}

void LogIocService::SetArg(const char* name, const char* value) { 
	args_[std::string(name)] = std::string(value);
}

LogMngService::LogMngService(){
	
}

LogMngService::~LogMngService(){
	
}

void LogMngService::OnEvent(int eventType){
	switch(eventType){
		case MANAGER_EVENT_INIT : 
			if(_logService) _logService->Init(); 
			break;
		case MANAGER_EVENT_START : 
			if(_logService) _logService->Start(); 
			break;
		case MANAGER_EVENT_STOP : 
			if(_logService) _logService->Stop(); 
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_logService) _logService->UnInit(); 
			break;
	}
}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_LOG;
	info.version = __DATE__""__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_LOG;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_LOG) == 0){
		return x::_logService;	
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_logIocService;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_logMngService;
	}
	return NULL;
}

