#include "message.h"

namespace x {

Message::Message(){

}

Message::~Message(){

}


bool Message::SetBuff(void* data, uint32_t len){
	if(!data || len == 0) return false;
	
	buff_.Set(data, len);
	return DecodeBuff();
}

bool Message::GetBuff(void** buff, uint32_t& len){
	if(!EncodeBuff()) return false;
	
	*buff = (void*)buff_.Data();
	len = buff_.Len();
	return true;
}

bool Message::SetData(void* data, uint32_t len){
	return dataBuff_.Set(data, len);
}

bool Message::GetData(void** data, uint32_t& len){
	len = dataBuff_.Len();
	if(len > 0){
		*data = (void*)dataBuff_.Data();	
	}
	return true;
}

void Message::SetSrcRouteInfo(RouteInfo* route){
	if(!route) return;
	memcpy(&routeSrc_, route, sizeof(routeSrc_));
}

void Message::GetSrcRouteInfo(RouteInfo& route){
	memcpy(&route, &routeSrc_, sizeof(route));
}

void Message::SetDestRouteInfo(RouteInfo* route){
	if(!route) return;
	memcpy(&routeDest_, route, sizeof(routeDest_));
}

void Message::GetDestRouteInfo(RouteInfo& route){
	memcpy(&route, &routeSrc_, sizeof(route));
}

void Message::SetMsgInfo(MsgInfo* msgInfo){
	if(!msgInfo) return;
	memcpy(&msgInfo_, msgInfo, sizeof(msgInfo_));
}

void Message::GetMsgInfo(MsgInfo& msgInfo){
	memcpy(&msgInfo, &msgInfo_, sizeof(msgInfo));
}

bool Message::EncodeBuff(){
	buff_.Clear();
	
	uint32_t dataLen = dataBuff_.Len();
	const void* data = dataBuff_.Data();

	buff_.Append(&msgInfo_, sizeof(msgInfo_));
	buff_.Append(&routeDest_, sizeof(routeDest_));
	buff_.Append(&routeSrc_, sizeof(routeSrc_));
	buff_.Append(dataLen);
	buff_.Append(data, dataLen);

	return true;
}

bool Message::DecodeBuff(){
	uint32_t buffLen = buff_.Len();
	char* buffData = (char*)buff_.Data();

	uint32_t pos = 0;
	if(pos +  sizeof(msgInfo_) > buffLen) return false;
	memcpy(&msgInfo_, buffData + pos, sizeof(msgInfo_)); pos += sizeof(msgInfo_);
	if(pos + sizeof(routeDest_) > buffLen) return false;
	memcpy(&routeDest_, buffData + pos, sizeof(routeDest_)); pos += sizeof(routeDest_);
	if(pos + sizeof(routeSrc_) > buffLen) return false;
	memcpy(&routeSrc_, buffData + pos, sizeof(routeSrc_)); pos += sizeof(routeSrc_);
	if(pos + sizeof(uint32_t) > buffLen) return false;
	uint32_t dataLen = *(uint32_t*)(buffData+pos); pos += sizeof(uint32_t);
	if(pos + dataLen > buffLen) return false;
	void* dataBuff = buffData+pos;

	dataBuff_.Set(dataBuff, dataLen);

	return true;
}

}//namespace x