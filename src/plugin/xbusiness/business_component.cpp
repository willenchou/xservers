#include "business_component.h"

namespace x {

BusinessComponent::BusinessComponent()
:compnents_(NULL)
,compnentCount_(0)
,funcs_(NULL){
	compnents_ = new CompItem[MAX_COMPONENT_COUNT];
	funcs_ = new FuncItem*[MAX_FUNC_COUNT];
	memset(funcs_, 0, sizeof(FuncItem*) * MAX_FUNC_COUNT);
}

BusinessComponent::~BusinessComponent(){
	UnLoadAll();
	delete [] compnents_;
	for(int i = 0; i < MAX_FUNC_COUNT; i++){
		if(funcs_[i] != NULL){
			delete funcs_[i];
			funcs_[i] = NULL;
		}
	}
	delete [] funcs_;
}

bool BusinessComponent::Load(const char* name){
	char file[1024];
#ifdef OS_WIN
	sprintf(file, "%s.dll", name);
#else
	sprintf(file, "lib%s.so", name);
#endif
	
	DLLHANDL handle = DLLOPEN(file);
	if(handle){
	
		PFBusiCompInit funcInit = (PFBusiCompInit)DLLFUNC(handle, "Init");
		PFBusiCompUnInit funcUnInit = (PFBusiCompUnInit)DLLFUNC(handle, "UnInit");
		PFBusiGetFuncEntry funcGetFuncEntry = (PFBusiGetFuncEntry)DLLFUNC(handle, "GetFuncEntry");
		
		if(funcInit && funcUnInit && funcGetFuncEntry){
			if(compnentCount_ < MAX_COMPONENT_COUNT){
				CompItem& item = compnents_[compnentCount_++];
				item.handle = handle;
				item.init = funcInit;
				item.unInit = funcUnInit;
				item.getEntry = funcGetFuncEntry;
				
				item.init();

				const BusinessFuncEntry* entrys = item.getEntry();
				for(int i = 0; ; i++){
					if(!entrys[i].funcEntry)
						break;
					AddFuncEntry(entrys[i].funcID, entrys[i].funcEntry);
				}

				return true;
			}
		}
	}

	return false;
}

void BusinessComponent::UnLoadAll(){
	for(uint32_t i = 0; i < compnentCount_; i++){
		CompItem& item = compnents_[i];
		item.unInit();
		DLLCLOSE(item.handle);
	}
	compnentCount_ = 0;
}

int BusinessComponent::CallFunc(uint32_t funcID, IBusinessContext* context, IPack* in, IPack* out){
	int ret = MSG_RETURN_CODE_ERR_SYSTEM;
	PFBusinessFunc funcEntry = GetFuncEntry(funcID);
	if(funcEntry){
		ret = funcEntry(context, in, out);
	}else{
		out->BeginPack();
		out->AddCol("error_no", PACK_COL_TYPE_INT32);
		out->AddCol("error_info");
		out->AddCol("error_detail");
		out->AddValue("error_no", MSG_RETURN_CODE_ERR_SYSTEM);
		out->AddValue("error_info", "暂不支持该功能号！");
		out->AddValue("error_detail", "找不到处理该功能号的模块!");
		out->EndPack();

		ret = MSG_RETURN_CODE_ERR_SYSTEM;
	}

	return ret;
}

void BusinessComponent::AddFuncEntry(uint32_t funcID, PFBusinessFunc func){
	int index = funcID % MAX_FUNC_COUNT;
	int realIndex = -1;
	for(int i = 0; i < MAX_FUNC_COUNT; i++){
		int currIndex = (i + index) % MAX_FUNC_COUNT;
		if(funcs_[currIndex] == NULL){
			realIndex = currIndex;
			break;
		}
	}

	if(realIndex > -1){	
		FuncItem* item = new FuncItem();	
		item->funcID = funcID;
		item->funcEntry = func;
		funcs_[realIndex] = item;
	}
}

PFBusinessFunc BusinessComponent::GetFuncEntry(uint32_t funcID){
	int index = funcID % MAX_FUNC_COUNT;
	int realIndex = -1;
	for(int i = 0; i < MAX_FUNC_COUNT; i++){
		int currIndex = (i + index) % MAX_FUNC_COUNT;
		if(funcs_[currIndex] != NULL){
			if(funcs_[currIndex]->funcID == funcID)
				return funcs_[currIndex]->funcEntry;
		}
	}

	return NULL;
}

}//namespace x
