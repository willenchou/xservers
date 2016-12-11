#include "message.h"

namespace x {

Message::Message()
:buff_(NULL)
,buffCap_(0)
,buffLen_(0){
	memset(tags_, 0, sizeof(tags_));
	
	buff_ = new char[INIT_MSG_BUFF_LEN];
	buffCap_ = INIT_MSG_BUFF_LEN;
	buffLen_ = 0;
}

Message::~Message(){
	if(buff_) 
		delete buff_;
}

bool Message::SetBuff(void* data, uint32_t len){
	if(len < sizeof(Head)) 
		return false;
	
	if(buffCap_ < len){
		Resize(len);
	}

	memcpy(buff_, data, len);

	BuildTags();

	return true;
}

bool Message::GetBuff(void** data, uint32_t* len){
	
	BuildBuff();

	Head* head = (Head*)buff_;
	if(head->len > 0){
		if(data) *data = buff_;
		if(len) *len = sizeof(Head) + head->len;
		return true;
	}
	return false;
}

bool Message::SetData(uint32_t tagID, void* data, uint32_t len, int32_t type){
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] == NULL){
			tags_[tagID] = new MessageTag(tagID);	
		}else{
			tags_[tagID]->Clear();
		}
		tags_[tagID]->AddData(data, len ,type);
		return true;
	}
	return false;
}

bool Message::AddData(uint32_t tagID, void* data, uint32_t len, int32_t type){
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] == NULL){
			tags_[tagID] = new MessageTag(tagID);	
		}
		tags_[tagID]->AddData(data, len ,type);
		return true;
	}
	return false;
}

uint32_t Message::GetDataCount(uint32_t tagID){
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] != NULL){
			return tags_[tagID]->GetDataCount();
		}
	}
	return 0u;
}

bool Message::GetData(uint32_t tagID, void** data, uint32_t* len, int32_t* type, uint32_t index){
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] != NULL){
			return tags_[tagID]->GetData(data, len, type, index);
		}
	}
	return false;
}

bool Message::DataExist(uint32_t tagID){	
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] != NULL){
			return true;
		}
	}
	return false;
}

void Message::RemoveData(uint32_t tagID){
	if(tagID < MSG_TAG_MAX_COUNT){
		if(tags_[tagID] != NULL){
			delete tags_[tagID];
			tags_[tagID] = NULL;
		}
	}
}

void Message::Clear(){
	for(int i = 0; i < MSG_TAG_MAX_COUNT; i++){
		if(tags_[i] != NULL){
			delete tags_[i];
			tags_[i] = NULL;
		}
	}
	
	buffLen_ = 0;
}

void Message::ChangeReq2Ans(){
	MessageTag* tagTemp = tags_[MSG_TAG_ROUTE_SRC];
	tags_[MSG_TAG_ROUTE_SRC] = tags_[MSG_TAG_ROUTE_DEST];
	tags_[MSG_TAG_ROUTE_DEST] = tagTemp;
	
	if(tags_[MSG_TAG_ROUTE_SRC] != NULL)
		tags_[MSG_TAG_ROUTE_SRC]->SetTagID(MSG_TAG_ROUTE_SRC);
	if(tags_[MSG_TAG_ROUTE_DEST] != NULL)
		tags_[MSG_TAG_ROUTE_DEST]->SetTagID(MSG_TAG_ROUTE_DEST);

	SetFuncType(MSG_FUNC_TYPE_ANS);
}

void Message::SetSysNo(uint32_t sysNo){
	GetTagFuncInfo()->sysNo = sysNo;
}

bool Message::GetSysNo(uint32_t* sysNo){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(sysNo) *sysNo = info->sysNo;
	return true;
}

void Message::SetSubSysNo(uint32_t subSysNo){
	GetTagFuncInfo()->subSysNo = subSysNo;
}

bool Message::GetSubSysNo(uint32_t* subSysNo){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(subSysNo) *subSysNo = info->subSysNo;
	return true;
}

void Message::SetFuncID(uint32_t funcID){
	GetTagFuncInfo()->funcID = funcID;
}

bool Message::GetFuncID(uint32_t* funcID){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(funcID) *funcID = info->funcID;
	return true;
}

void Message::SetFuncType(uint32_t funcType){
	GetTagFuncInfo()->type = funcType;
}

bool Message::GetFuncType(uint32_t* funcType){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(funcType) *funcType = info->type;
	return true;
}

void Message::SetSendID(uint32_t sendID){
	GetTagFuncInfo()->sendID = sendID;
}

bool Message::GetSendID(uint32_t* sendID){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(sendID) *sendID = info->sendID;
	return true;
}

void Message::SetReturnID(int32_t returnID){
	GetTagFuncInfo()->returnNo = returnID;
}

bool Message::GetReturnID(int32_t* returnID){
	MsgFuncInfo * info = GetTagFuncInfo(false);
	if(!info)
		return false;
	if(returnID) *returnID = info->returnNo;
	return true;	
}

void Message::Resize(uint32_t newSize){
	if(newSize <= buffCap_) return;

	char* buff = new char[newSize];
	memcpy(buff, buff_, buffLen_);
	
	delete buff_;

	buff_ = buff;
	buffCap_ = newSize;
}

void Message::BuildTags(){
	for(int i = 0; i < MSG_TAG_MAX_COUNT; i++){
		if(tags_[i] != NULL){
			delete tags_[i];
			tags_[i] = NULL;
		}
	}
	
	Head* head = (Head*)buff_;
	
	uint32_t pos = sizeof(Head);
	while(pos < head->len){
		MessageTag::Head* tagHead = (MessageTag::Head*)(buff_ + pos);
		if(tagHead->ID < MSG_TAG_MAX_COUNT){
			MessageTag* tag = new MessageTag(tagHead->ID);
			tag->SetBuff(buff_ + pos, sizeof(MessageTag::Head) + tagHead->len);
			tags_[tagHead->ID] = tag;
		}
		pos += (sizeof(MessageTag::Head) + tagHead->len);
	}
}

void Message::BuildBuff(){
	Head* head = (Head*)buff_;
	head->flag = MSG_FLAG;
	head->tagCount = 0;
	head->len = 0;

	buffLen_ = sizeof(Head);
	for(int i = 0; i < MSG_TAG_MAX_COUNT; i++){
		MessageTag* tag = tags_[i];
		if(tag != NULL){
			uint32_t tagBuffLen = 0;
			void* tagBuffData = tag->GetBuff(tagBuffLen);
			if(tagBuffData){
				if(buffLen_ + tagBuffLen > buffCap_){
					Resize((buffCap_ + tagBuffLen) * 2);
					head = (Head*)buff_;
				}
				memcpy(buff_ + buffLen_, tagBuffData, tagBuffLen);
				head->tagCount++;
				head->len += tagBuffLen;

				buffLen_ += tagBuffLen;
			}
		}
	}
}

MsgFuncInfo* Message::GetTagFuncInfo(bool insert){
	if(tags_[MSG_TAG_FUNC_INFO] == NULL){
		if(!insert)
			return NULL;

		MsgFuncInfo info;
		memset(&info, 0, sizeof(info));
		tags_[MSG_TAG_FUNC_INFO] = new MessageTag(MSG_TAG_FUNC_INFO);
		tags_[MSG_TAG_FUNC_INFO]->AddData(&info, sizeof(info));
	}
	
	void* data = NULL;
	uint32_t dataLen = 0;
	tags_[MSG_TAG_FUNC_INFO]->GetData(&data,&dataLen);
	
	return (MsgFuncInfo*)data;
}

MessageTag::MessageTag(int32_t ID){
	buff_ = new char[INIT_TAG_BUFF_LEN];
	buffLen_ = INIT_TAG_BUFF_LEN;
	
	Head* head = GetHead();
	head->ID = ID;
	head->count = 0;
	head->len = 0;
}

MessageTag::~MessageTag(){
	delete [] buff_;
	buff_ = NULL;
	buffLen_ = 0;
}

bool MessageTag::SetBuff(void* data, uint32_t len){
	if(len < sizeof(Head)) return false;

	if(buffLen_ < len){
		Resize(len);
	}
	memcpy(buff_, data, len);

	return true;
}

void* MessageTag::GetBuff(uint32_t& len){
	Head* head = GetHead();
	if(head->len > 0){
		len = sizeof(Head) + head->len;
		return buff_;
	}
	return NULL;
}

void MessageTag::AddData(void* data, uint32_t dataLen, int32_t type){
	if(GetHead()->len + dataLen > buffLen_){
		Resize((GetHead()->len + dataLen) * 2);
	}
	
	Head* head = GetHead();
	ItemHead itemHead = {type, dataLen};
	memcpy(buff_ + sizeof(Head) + head->len, &itemHead, sizeof(itemHead)); head->len += sizeof(itemHead);
	memcpy(buff_ + sizeof(Head) + head->len, data, dataLen); head->len += dataLen;
	head->count++;
}

uint32_t MessageTag::GetDataCount(){
	return GetHead()->count;
}

bool MessageTag::GetData(void** data, uint32_t* len, int32_t* type, uint32_t index){
	Head* head = GetHead();
	if(index >= head->count) 
		return false;
	
	uint32_t pos = GetItemPos(index);

	if(data) *data = buff_ + pos + sizeof(ItemHead);
	if(len) *len = ((ItemHead*)(buff_ + pos))->len;
	if(type) *type = ((ItemHead*)(buff_ + pos))->type;

	return true;
}

void MessageTag::Clear(){
	Head* head = GetHead();
	head->len = 0;
	head->count = 0;
}

void MessageTag::Resize(uint32_t newSize){
	if(newSize <= buffLen_) return;

	char* buff = new char[newSize];
	memcpy(buff, buff_, buffLen_);
	
	delete buff_;

	buff_ = buff;
	buffLen_ = newSize;
}

uint32_t MessageTag::GetItemPos(uint32_t index){
	Head* head = GetHead();

	uint32_t pos = sizeof(Head);
	for(uint32_t i = 0; i < head->count; i++){
		if(i == index){
			return pos;
		}
		ItemHead* head = (ItemHead*)(buff_ + pos);
		pos += sizeof(ItemHead) + head->len;
	}

	return pos;
}

}//namespace x