#include "xpipe.h"
#include <xbase/util.h>

namespace x{

PipeService* _pipeService = new PipeService();
PipeIoc* _pipeIoc = new PipeIoc();
PipeMng* _pipeMng = new PipeMng();
ILog* _log = NULL;

PipeService::PipeService()
:pipeCount_(0){
	
}

PipeService::~PipeService(){

}

void PipeService::Init(){
	int i = 0;
}

void PipeService::UnInit(){

}

void PipeService::Start(){
	XMLElement* config = _pipeIoc->GetConfig();
	if(!config) {
		return;
	}

	XMLElement* nodePipes = config->FirstChildElement("pipes");
	if(nodePipes){
		XMLElement* nodePipe = nodePipes->FirstChildElement("pipe");
		while(nodePipe){
			const char* strPipe = nodePipe->GetText();
			if(strPipe){
				AddPipe(strPipe);
			}
			nodePipe = nodePipe->NextSiblingElement("pipe");
		}
	}

	for(uint32_t i = 0; i < pipeCount_; i++){
		PipeItem* preItem = NULL;
		PipeItem* currItem = pipes_[i];
		while(currItem){
			if(preItem){
				currItem->servcie->RegisterPre(preItem->servcie);
			}
			if(currItem->next){
				currItem->servcie->RegisterNext(currItem->next->servcie);
			}
			
			preItem = currItem;
			currItem = currItem->next;
		}
	}
}

void PipeService::Stop(){

}

PipeItem* PipeService::GetPipeline(const char* firstPlugin){
	for(uint32_t i = 0; i < pipeCount_; i++){
		PipeItem* item = pipes_[i];
		if(strcmp(item->plugin, firstPlugin) == 0)
			return item;
	}
	return NULL;
}

void PipeService::AddPipe(const char* strPipe){
	
	char strPipeCopy[1024];
	Util::StrCopy(strPipeCopy, strPipe);

	PipeItem* itemHead = NULL;
	PipeItem* itemTail = NULL;
	char* lastPos = strPipeCopy;
	for(char* strPos = strPipeCopy;;strPos++){
		bool isEnd = false;
		if(*strPos == '\0'){
			isEnd = true;
		}else if(*strPos == ';'){
			*strPos = '\0';
		}

		if(*strPos == '\0'){
			IMsgService* service = (IMsgService*)_pipeIoc->GetContainer()->GetService(lastPos, SID_MSG);
			if(service){
				PipeItem* item = new PipeItem();
				Util::StrCopy(item->plugin, lastPos);
				item->servcie = service;
				item->next = NULL;
				
				if(!itemHead)
					itemHead = item;

				if(itemTail)
					itemTail->next = item;
				itemTail = item;
			}

			lastPos = strPos+1;
		}
		
		if(isEnd) 
			break;
	}

	if(itemHead){
		pipes_[pipeCount_++] = itemHead;
	}
}

PipeIoc::PipeIoc(){
	
}

PipeIoc::~PipeIoc(){
	
}
	
IContainer* PipeIoc::GetContainer() { 
	return container_;
}
	
XMLElement* PipeIoc::GetConfig() { 
	return config_;
}
	
const char* PipeIoc::GetArg(const char* name) { 
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

void PipeIoc::SetContainer(IContainer* container) { 
	container_ = container; 
}

void PipeIoc::SetConfig(XMLElement* config) { 
	config_ = config; 
}

void PipeIoc::SetArg(const char* name, const char* value) { 
	args_[std::string(name)] = std::string(value);
}

PipeMng::PipeMng(){
	
}

PipeMng::~PipeMng(){
	
}

void PipeMng::OnEvent(int eventType){
	switch(eventType){
		case MANAGER_EVENT_INIT : 
			if(_pipeService) _pipeService->Init(); 
			break;
		case MANAGER_EVENT_START : 
			if(_pipeService) _pipeService->Start(); 
			break;
		case MANAGER_EVENT_STOP : 
			if(_pipeService) _pipeService->Stop(); 
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_pipeService) _pipeService->UnInit(); 
			break;
	}
}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_PIPE;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_PIPE;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_PIPE) == 0){
		return x::_pipeService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_pipeIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_pipeMng;
	}

	return NULL;
}
