#include "connection_test.h"

Callback::Callback(x::ConnectionApi* api)
:api_(api){
}

Callback::~Callback(){
}

void Callback::OnConnect(x::ConnectionApi* api){
	printf("connected\n");
}

void Callback::OnClose(x::ConnectionApi* api){
	printf("closed\n");
}

void Callback::OnRecv(x::ConnectionApi* api, x::IMessage* msg){
	printf("onrecv\n");

	uint32_t funcID = 0;
	int32_t returnID = 0;
	msg->GetFuncID(&funcID);
	msg->GetReturnID(&returnID);
	printf("功能号:%u,返回码:%d\n", funcID, returnID);
	
	ShowRoute(msg, "发送路由", x::MSG_TAG_ROUTE_SRC);
	ShowRoute(msg, "目标路由", x::MSG_TAG_ROUTE_DEST);
	ShowRoute(msg, "内部路由", x::MSG_TAG_ROUTE_INTERNAL);

	void* packData = NULL;
	uint32_t packDataLen = 0;
	x::IPack* pack = api_->NewPack();
	if(msg->GetData(x::MSG_TAG_FUNC_BODY, &packData, &packDataLen)){
		pack->SetBuff(packData, packDataLen);
		for(uint32_t row = 0; row < pack->GetRowCount(); row++){
			printf("第%u行:\n", row + 1u);
			for(uint32_t col = 0; col < pack->GetColCount(); col++){
				const char* colName = pack->GetColName(col);
				int8_t colType = pack->GetColType(col);
				switch(colType){
					case x::PACK_COL_TYPE_UINT8:
						printf("%s:%u\n", colName, pack->GetValueUint8(colName, row));
						break;
					case x::PACK_COL_TYPE_UINT16:
						printf("%s:%u\n", colName, pack->GetValueUint16(colName, row));
						break;
					case x::PACK_COL_TYPE_UINT32:
						printf("%s:%u\n", colName, pack->GetValueUint32(colName, row));
						break;
					case x::PACK_COL_TYPE_INT8:
						printf("%s:%d\n", colName, pack->GetValueInt8(colName, row));
						break;
					case x::PACK_COL_TYPE_INT16:
						printf("%s:%d\n", colName, pack->GetValueInt16(colName, row));
						break;
					case x::PACK_COL_TYPE_INT32:
						printf("%s:%d\n", colName, pack->GetValueInt32(colName, row));
						break;
					case x::PACK_COL_TYPE_DOUBLE:
						printf("%s:%f\n", colName, pack->GetValueDouble(colName, row));
						break;
					case x::PACK_COL_TYPE_STR:
						printf("%s:%s\n", colName, pack->GetValueStr(colName, row));
						break;
					case x::PACK_COL_TYPE_RAW:
						printf("%s:[rowdata]\n", colName);
						break;
					default:
						break;
				}
			}
		}
	}else{
		printf("获取Pack包失败!\n");
	}
}

void Callback::ShowRoute(x::IMessage* msg, const char* caption, uint32_t tagID){
	void* routeData = NULL;
	uint32_t roureDataLen = 0u;
	if(msg->GetData(tagID, &routeData, &roureDataLen)){
		x::MsgRoute* route = (x::MsgRoute*)routeData;
		printf("%s:%u,%u,%s,%u,%d,%s\n",
			caption,
			route->sysNo,
			route->subSysNo,
			route->appName,
			route->appNo,
			route->connectionID,
			route->plugin);
	}
}