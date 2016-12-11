#include "xmessage_service.h"

x::IContainer* _containner = NULL;

void FUNC_CALL Init(x::IContainer* container){
	_containner = container;
}

void FUNC_CALL UnInit(){

}

void FUNC_CALL Start(){

}

void FUNC_CALL Stop(){

}

bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info){
	switch(index){
		case 0:
			info.id = SID_MESSAGE;
			info.caption = "";
			info.version = __DATE__":"__TIME__;
			return true;
		default: return false;
	}
}

x::IService* FUNC_CALL GetService(const char* id){
	if(strcmp(id, SID_MESSAGE) == 0){
		return new x::MessageService();
	}
	return NULL;
}