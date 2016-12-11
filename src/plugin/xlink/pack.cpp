#include "pack.h"

namespace x {
	
Pack::Pack()
:buff_(NULL){
	buff_ = new BinaryBuff();
}

Pack::~Pack(){
	Clear();
	if(buff_) 
		delete buff_;
}

bool Pack::SetBuff(void* buff, uint32_t len){
	Clear();
	
	if (!buff_->Set(buff, len)) {
		return false;
	}

	return BuildItem();
}

bool Pack::GetBuff(void** buff, uint32_t* len){
	BuildBuff();
	
	if(buff_->Len() > 0){
		if(buff) *buff = (void*)buff_->Data();
		if (len) *len = buff_->Len();
		return true;
	}

	return false;
}

void Pack::Clear(){
	for(size_t i = 0; i < items_.size(); i++){
		delete items_.at(i);
	}
	items_.clear();
	itemIndex_.clear();

	buff_->Clear();
}

void Pack::BeginPack(){
	Clear();
}

void Pack::EndPack(){

}

void Pack::AddCol(const char* name, int8_t type){
	PackItemMap::iterator it = itemIndex_.find(name);
	if (it != itemIndex_.end()) {
		return;
	}

	PackItem* item = new PackItem(name, type);
	items_.push_back(item);
	itemIndex_[name] = item;
}

uint32_t Pack::GetColCount(){
	return items_.size();
}

const char* Pack::GetColName(uint32_t col){
	PackItem* item = GetItem(col);
	if(item){
		return item->Name();
	}
	return NULL;
}

int8_t Pack::GetColType(uint32_t col){
	PackItem* item = GetItem(col);
	if(item){
		return item->Type();
	}
	return NULL;
}

void Pack::AddRow(){
	
}

uint32_t Pack::GetRowCount(){
	uint32_t rowCount = 0;
	if(items_.size() > 0){
		rowCount = items_.at(0)->Count();
		for(size_t i = 0; i < items_.size(); i++){
			if(items_.at(i)->Count() < rowCount){
				rowCount = items_.at(i)->Count();
			}
		}
	}
	return rowCount;
}

void Pack::AddValue(const char* name, uint8_t value){
	AddValue(name, &value, sizeof(value));	
}

void Pack::AddValue(const char* name, uint16_t value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, uint32_t value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, int8_t value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, int16_t value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, int32_t value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, double value){
	AddValue(name, &value, sizeof(value));
}

void Pack::AddValue(const char* name, char* value){
	AddValue(name, value, (strlen(value) + 1) * sizeof(char));
}

void Pack::AddValue(const char* name, void* value, uint32_t len){
	PackItem* item = GetItem(name);
	if(!item) return;
	item->AddData(value, len);
}

uint8_t Pack::GetValueUint8(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(uint8_t*)data;
	return 0;
}

uint16_t Pack::GetValueUint16(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(uint16_t*)data;
	return 0;
}

uint32_t Pack::GetValueUint32(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(uint32_t*)data;
	return 0;
}

int8_t Pack::GetValueInt8(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(int8_t*)data;
	return 0;
}

int16_t Pack::GetValueInt16(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(int16_t*)data;
	return 0;
}

int32_t Pack::GetValueInt32(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(int32_t*)data;
	return 0;
}

double Pack::GetValueDouble(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return *(double*)data;
	return 0;
}

char* Pack::GetValueStr(const char* name, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, name, row); 
	if(data)
		return (char*)data;
	return 0;
}

void* Pack::GetValueRaw(uint32_t* len, const char* name, uint32_t row){
	void* data = NULL;
	PackItem* item = GetItem(name);
	if(item)
		item->GetData(row, &data, len);
	return data;
}

uint8_t Pack::GetValueUint8(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(uint8_t*)data;
	return 0;
}

uint16_t Pack::GetValueUint16(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(uint16_t*)data;
	return 0;
}

uint32_t Pack::GetValueUint32(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(uint32_t*)data;
	return 0;
}

int8_t Pack::GetValueInt8(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(int8_t*)data;
	return 0;
}

int16_t Pack::GetValueInt16(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(int16_t*)data;
	return 0;
}

int32_t Pack::GetValueInt32(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(int32_t*)data;
	return 0;
}

double Pack::GetValueDouble(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return *(double*)data;
	return 0;
}

char* Pack::GetValueStr(uint32_t col, uint32_t row){
	uint32_t len;
	void* data = GetValueRaw(&len, col, row); 
	if(data)
		return (char*)data;
	return 0;
}

void* Pack::GetValueRaw(uint32_t* len, uint32_t col, uint32_t row){
	void* data = NULL;
	PackItem* item = GetItem(col);
	if(item)
		item->GetData(row, &data, len);
	return data;
}

PackItem* Pack::GetItem(const char* name){
	PackItemMap::iterator it = itemIndex_.find(name);
	if (it != itemIndex_.end()) {
		return it->second;
	}
	return NULL;
}

PackItem* Pack::GetItem(uint32_t col){
	if(col < items_.size()){
		return items_.at(col);
	}
	return NULL;
}

bool Pack::BuildBuff(){
	buff_->Clear();
	
	for(size_t i = 0; i < items_.size(); i++){
		PackItem* item = items_.at(i);

		buff_->Append(item->Name());
		buff_->Append(item->Type());
		buff_->Append(item->Count());

		for(uint32_t j = 0; j < item->Count(); j++){
			void* data = NULL; uint32_t len = 0;
			item->GetData(j, &data, &len);			
			if(item->Type() == PACK_COL_TYPE_RAW){
				buff_->Append(len);
			}
			buff_->Append(data, len);
		}
	
	}
	
	return true;
}

bool Pack::BuildItem(){
	
	for(size_t i = 0; i < items_.size(); i++){
		delete items_.at(i);
	}
	items_.clear();
	itemIndex_.clear();

	char* data = (char*)buff_->Data();
	uint32_t len = buff_->Len();
	
	for (int pos = 0; pos < len;) {
		char* PACK_COLName = (char*)(data + pos); pos += strlen(PACK_COLName) + 1;
		uint8_t type = *(uint8_t*)(data + pos); pos += sizeof(type);
		uint32_t count = *(uint32_t*)(data + pos); pos += sizeof(count);
		AddCol(PACK_COLName, type);
		
		for (uint32_t i = 0; i < count; i++) {
			switch(type) {
				case PACK_COL_TYPE_UINT8: {
					uint8_t itemValue = *(uint8_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_UINT16: {
					uint16_t itemValue = *(uint16_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_UINT32: {
					uint32_t itemValue = *(uint32_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_INT8: {
					int8_t itemValue = *(int8_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_INT16: {
					int16_t itemValue = *(int16_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_INT32: {
					int32_t itemValue = *(int32_t*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;	
				}
				case PACK_COL_TYPE_DOUBLE: {
					double itemValue = *(double*)(data + pos); pos += sizeof(itemValue);
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_STR: {
					char* itemValue = data + pos; pos += strlen(itemValue) + 1;
					AddValue(PACK_COLName, itemValue);
					break;
				}
				case PACK_COL_TYPE_RAW: {
					uint32_t itemLen = *(uint32_t*)(data + pos); pos += sizeof(itemLen);
					void* itemValue = (void*)(data + pos); pos += itemLen;
					AddValue(PACK_COLName, itemValue, itemLen);
					break;
				}
				default: break;
			}
		}
	}
	return true;
}

}//namespace x
