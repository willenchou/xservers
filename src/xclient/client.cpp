#include "client.h"

Client::Client()
:connCfgs_(NULL)
, connCfgCount_(0)
, testCfgCount_(0)
, connectStatus_(-1){
	connCfgs_ = new ConnCfg*[1024];
	testCfgs_ = new FuncTest*[1024];
	api_ = x::NewConnectionAPI();
	api_->Init(this);
}

Client::~Client(){
}

void Client::OnConnect(x::ConnectionApi* api) {
	connectStatus_ = 1;
}

void Client::OnClose(x::ConnectionApi* api) {
	connectStatus_ = 0;
}

void Client::OnRecv(x::ConnectionApi* api, x::IMessage* msg){
	int32_t returnCode = 0;
	uint32_t funcID = 0u;
	msg->GetReturnID(&returnCode);
	msg->GetFuncID(&funcID);

	printf("收到应答数据\n");
	printf("功能号:%u\n", funcID);
	printf("返回码:%d\n", returnCode);

	void* packData = NULL;
	uint32_t packDataLen = 0u;
	if (msg->GetData(x::MSG_TAG_FUNC_BODY, &packData, &packDataLen)){
		x::IPack* pack = api_->NewPack();
		if (pack->SetBuff(packData, packDataLen)){
			for (uint32_t row = 0; row < pack->GetRowCount(); row++) {
				printf("第%u行:\n", row);
				for (uint32_t col = 0; col < pack->GetColCount(); col++) {
					const char* colName = pack->GetColName(col);
					int8_t colType = pack->GetColType(col);
					switch (colType){
					case x::PACK_COL_TYPE_UINT8:
						printf("%-20s%u\n", colName, pack->GetValueUint8(col, row));
						break;
					case x::PACK_COL_TYPE_UINT16:
						printf("%-20s%u\n", colName, pack->GetValueUint16(col, row));
						break;
					case x::PACK_COL_TYPE_UINT32:
						printf("%-20s%u\n", colName, pack->GetValueUint32(col, row));
						break;
					case x::PACK_COL_TYPE_INT8:
						printf("%-20s%d\n", colName, pack->GetValueInt8(col, row));
						break;
					case x::PACK_COL_TYPE_INT16:
						printf("%-20s%d\n", colName, pack->GetValueInt16(col, row));
						break;
					case x::PACK_COL_TYPE_INT32:
						printf("%-20s%d\n", colName, pack->GetValueInt32(col, row));
						break;
					case x::PACK_COL_TYPE_DOUBLE:
						printf("%-20s%f\n", colName, pack->GetValueDouble(col, row));
						break;
					case x::PACK_COL_TYPE_STR:
						printf("%-20s%s\n", colName, pack->GetValueStr(col, row));
						break;
					case x::PACK_COL_TYPE_RAW:
						printf("%-20s[rawData]\n", colName);
						break;
					default:
						printf("%-20s[未知类型]\n");
						break;
					}
				}
			}
		}
		pack->Release();
	} else {
		printf("解PACK包失败！\n");
	}
}

bool Client::Connect(){
	LoadConnCfg();

	if (connCfgCount_ < 1)
		return false;

	int choose = -1;
	while (choose < 1 || choose > connCfgCount_) {
		printf("------------------------------------\n");
		printf("选择连接地址\n");
		printf("------------------------------------\n");
		for (int i = 0; i < connCfgCount_; i++) {
			ConnCfg* cfg = connCfgs_[i];
			printf("%-5d%s:%s\n", i + 1, cfg->ip, cfg->port);
		}
		printf("------------------------------------\n");
		printf("请选择:");
		scanf_s("%d", &choose);
	}

	ConnCfg* conn = connCfgs_[choose - 1];
	if (api_->Connect(conn->ip, atoi(conn->port)) == 0) {
		for (int i = 0; i < 100; i++){
			if (connectStatus_ == 1) {
				return true;
			} else if (connectStatus_ == 0) {
				break;
			}

			x::SleepX(100);
		}
	}

	return false;
}

void Client::Test(char* file){
	LoadTestCfg(file);

	if (testCfgCount_ == 0)
		return;

	x::IMessage* msg = api_->NewMessage();
	x::IPack* pack = api_->NewPack();

	while (1){

		int choose = -1;
		while (choose < 1 || choose > testCfgCount_) {
			printf("------------------------------------\n");
			printf("选择测试功能号\n");
			printf("------------------------------------\n");
			for (int i = 0; i < testCfgCount_; i++) {
				FuncTest* cfg = testCfgs_[i];
				printf("%-5d%d\n", i+1, cfg->funcID);
			}
			printf("------------------------------------\n");
			printf("请选择:");
			scanf_s("%d", &choose);
		}

		FuncTest* test = testCfgs_[choose - 1];

		pack->BeginPack();
		
		ColItem* item = test->inputs;
		while (item){
			if (strcmp(item->type, "uint8") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_UINT8);
			} else if (strcmp(item->type, "uint16") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_UINT16);
			} else if (strcmp(item->type, "uint32") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_UINT32);
			} else if (strcmp(item->type, "int8") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_INT8);
			} else if (strcmp(item->type, "int16") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_INT16);
			} else if (strcmp(item->type, "int32") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_INT32);
			} else if (strcmp(item->type, "double") == 0){
				pack->AddCol(item->name, x::PACK_COL_TYPE_DOUBLE);
			} else {
				pack->AddCol(item->name, x::PACK_COL_TYPE_STR);
			}

			item = item->next;
		}

		item = test->inputs;
		while (item){
			if (strcmp(item->type, "uint8") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "uint16") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "uint32") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "int8") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "int16") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "int32") == 0){
				pack->AddValue(item->name, atoi(item->value));
			} else if (strcmp(item->type, "double") == 0){
				pack->AddValue(item->name, atof(item->value));
			} else {
				pack->AddValue(item->name, (char*)item->value);
			}

			item = item->next;
		}

		pack->EndPack();

		void* packData = NULL;
		uint32_t packDataLen = 0u;
		pack->GetBuff(&packData, &packDataLen);

		msg->SetFuncID(test->funcID);
		msg->SetData(x::MSG_TAG_FUNC_BODY, packData, packDataLen);

		api_->Send(msg);

		fflush(stdin);
		getchar();
	}
}

void Client::LoadConnCfg() {
	if (connDoc_.LoadFile(CONNECTION_CFG_FILE) == x::XML_SUCCESS) {
		x::XMLElement* nodeRoot = connDoc_.RootElement();
		x::XMLElement* nodeConn = nodeRoot->FirstChildElement("conn");
		while (nodeConn) {
			const char* name = nodeConn->Attribute("name");
			const char* ip = nodeConn->Attribute("ip");
			const char* port = nodeConn->Attribute("port");

			if (ip && port){
				ConnCfg* cfg = new ConnCfg();
				cfg->name = name;
				cfg->ip = ip;
				cfg->port = port;
				
				connCfgs_[connCfgCount_++] = cfg;
			}
			nodeConn = nodeConn->NextSiblingElement();
		}
	}
}

void Client::LoadTestCfg(char* file){
	if (testDoc_.LoadFile(file) == x::XML_SUCCESS) {
		x::XMLElement* nodeRoot = testDoc_.RootElement();
		if (!nodeRoot) return;

		for (x::XMLElement* nodeTest = nodeRoot->FirstChildElement("test"); 
			nodeTest;
			nodeTest = nodeTest->NextSiblingElement())
		{
			const char* funcID = nodeTest->Attribute("function_id");
			if (!funcID) continue;

			x::XMLElement* nodeInput = nodeTest->FirstChildElement("input");
			if (!nodeInput) continue;
			
			ColItem* colHead = NULL;
			ColItem* colTail = NULL;
			for (x::XMLElement* nodeCol = nodeInput->FirstChildElement("col");
				nodeCol;
				nodeCol = nodeCol->NextSiblingElement("col"))
			{
				const char* name = nodeCol->Attribute("name");
				const char* type = nodeCol->Attribute("type");
				const char* value = nodeCol->Attribute("ip");

				if (name && value) {
					ColItem* item = new ColItem();
					item->name = name;
					item->type = type ? type : "str";
					item->value = value;
					item->next = NULL;

					if (!colHead) colHead = item;
					if (colTail) colTail->next = item;
					colTail = item;
				}
			}
			
			FuncTest* test = new FuncTest();
			test->funcID = atoi(funcID);
			test->inputs = colHead;

			testCfgs_[testCfgCount_++] = test;
		}
	}
}