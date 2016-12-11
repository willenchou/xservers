#ifndef XCONNECTION_XCONNECTION_COMMON_H_
#define XCONNECTION_XCONNECTION_COMMON_H_

namespace x {

enum{
	CONN_STATUS_NOT_CONNECT = 0,
	CONN_STATUS_CONNECTING,
	CONN_STATUS_CONNECTED,
	CONN_STATUS_REGISTED
};

struct FuncReqRegisterBody{
	uint32_t type;			//0:中间件 1:客户端
	uint32_t sysNo;
	uint32_t subSysNo;
	char appName[32];
	uint32_t appNo;
};

struct FuncAnsRegisterBody{
	uint32_t sysNo;
	uint32_t subSysNo;
	char appName[32];
	uint32_t appNo;
	uint32_t connectionID;
};

}//namespace x

#endif //XCONNECTION_XCONNECTION_COMMON_H_
