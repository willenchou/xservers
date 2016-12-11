#include "xbusiness.h"

namespace x{

BusinessService* _businessService = new BusinessService();
BusinessIoc* _businessIoc = new BusinessIoc();
BusinessMng* _businessMng = new BusinessMng();
ILog* _log = NULL;

BusinessService::BusinessService()
:msgService_(NULL)
,dbService_(NULL)
,cfgThreadNum_(DEFAULT_THREAD_NUM)
,servicePre_(NULL)
,serviceNext_(NULL)
,components_(NULL)
,threadCount_(0)
,currThreadIndex_(0){
	components_ = new BusinessComponent();
}

BusinessService::~BusinessService(){
	delete components_;
}

void BusinessService::Init(){
	msgService_ = (IMessageService*)_businessIoc->GetContainer()->GetService(SID_MESSAGE);
	dbService_ = (IDBService* )_businessIoc->GetContainer()->GetService(SID_DB);

	XMLElement* config = _businessIoc->GetConfig();
	if(!config) {
		return;
	}

	XMLElement* nodeArgs = config->FirstChildElement("args");
	if(nodeArgs){
		cfgThreadNum_ = nodeArgs->IntAttribute("thread_num");
	}

	XMLElement* nodeComps = config->FirstChildElement("components");
	if(nodeComps){
		XMLElement* nodeComp = nodeComps->FirstChildElement("component");
		while(nodeComp){
			const char* compName = nodeComp->Attribute("name");
			if(compName){
				components_->Load(compName);	
			}

			nodeComp = nodeComp->NextSiblingElement("component");
		}
	}

	threadCount_ = cfgThreadNum_;
	if(threadCount_ <= 0) threadCount_ = 0;
	if(threadCount_ > MAX_THREAD_NUM) threadCount_ = MAX_THREAD_NUM;
	if(threadCount_ > 0){
		threads_ = new WorkThread*[threadCount_];
		for(uint32_t i = 0; i < threadCount_; i++){
			threads_[i] = new WorkThread(this, msgService_,dbService_);
		}
	}
}

void BusinessService::UnInit(){

}

void BusinessService::Start(){
	for(uint32_t i = 0; i < threadCount_; i++){
		threads_[i]->Start();
	}
}

void BusinessService::Stop(){
	for(uint32_t i = 0; i < threadCount_; i++){
		threads_[i]->Stop();
	}
}

void BusinessService::RegisterPre(IMsgService* service){
	servicePre_ = service;
}

void BusinessService::RegisterNext(IMsgService* service){
	serviceNext_ = service;
}

void BusinessService::PostMsg(IMessage* msg, IMsgService* from){
	if(threadCount_ > 0){
		uint32_t index = currThreadIndex_++;
		if(index >= threadCount_){
			currThreadIndex_ = 0;
			index = 0;
		}
		threads_[index]->PushMsg(msg);
	}else{
		DealMessage(context_, msg);
	}
}

void BusinessService::DealMessage(BusinessContext* context, IMessage* msg){
	uint32_t funcID = 0;
	uint32_t funcType = MSG_FUNC_TYPE_REQ;

	msg->GetFuncID(&funcID);
	msg->GetFuncType(&funcType);

	if(funcType == MSG_FUNC_TYPE_REQ){
		IPack* inPack = context->GetInPack(); inPack->Clear();
		IPack* outPack = context->GetOutPack(); outPack->Clear();
		
		void* inPackData = NULL;
		uint32_t inPackDataLen = 0u;
		if(msg->GetData(MSG_TAG_FUNC_BODY, &inPackData, &inPackDataLen)){
			inPack->SetBuff(inPackData, inPackDataLen);
		}
		
		int nRet = components_->CallFunc(funcID, context, inPack, outPack);
		
		void* outPackData = NULL;
		uint32_t outPackDataLen = 0u;
		outPack->GetBuff(&outPackData, &outPackDataLen);

		msg->ChangeReq2Ans();
		msg->SetReturnID(nRet);
		msg->SetData(MSG_TAG_FUNC_BODY, outPackData, outPackDataLen);
		
		if(serviceNext_)
			serviceNext_->PostMsg(msg, this);
	}else{
		//TODO
	}	
}

BusinessIoc::BusinessIoc(){
	
}

BusinessIoc::~BusinessIoc(){
	
}
	
IContainer* BusinessIoc::GetContainer() { 
	return container_;
}
	
XMLElement* BusinessIoc::GetConfig() { 
	return config_;
}
	
const char* BusinessIoc::GetArg(const char* name) { 
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

void BusinessIoc::SetContainer(IContainer* container) { 
	container_ = container; 
}

void BusinessIoc::SetConfig(XMLElement* config) { 
	config_ = config; 
}

void BusinessIoc::SetArg(const char* name, const char* value) { 
	args_[std::string(name)] = std::string(value);
}

BusinessMng::BusinessMng(){
	
}

BusinessMng::~BusinessMng(){
	
}

void BusinessMng::OnEvent(int eventType){
	switch(eventType){
		case MANAGER_EVENT_INIT : 
			if(_businessService) _businessService->Init(); 
			break;
		case MANAGER_EVENT_START : 
			if(_businessService) _businessService->Start(); 
			break;
		case MANAGER_EVENT_STOP : 
			if(_businessService) _businessService->Stop(); 
			break;
		case MANAGER_EVENT_UNINIT : 
			if(_businessService) _businessService->UnInit(); 
			break;
	}
}

}//namespace x

void FUNC_CALL GetPluginInfo(x::PluginInfo& info){
	info.caption = PLUGIN_BUSINESS;
	info.version = __DATE__":"__TIME__;
}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_BUSINESS;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default:return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_BUSINESS) == 0 || strcmp(id, SID_MSG) == 0){
		return x::_businessService;
	}else if(strcmp(id, SID_IOC) == 0){
		return x::_businessIoc;
	}else if(strcmp(id, SID_MANAGER) == 0){
		return x::_businessMng;
	}

	return NULL;
}
