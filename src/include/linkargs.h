#ifndef LINKSERVERARG_H_
#define LINKSERVERARG_H_


namespace x{
	struct LinkServerArgs{

		char appName[32];
		uint32_t appNo;

		//net
		char ip[32];
		uint16_t port;

		//other
		uint32_t threadNum; //接收线程数
		uint32_t init_connNum;//连接池
		uint32_t max_connNum;//最大连接池

		uint32_t init_RecvBuffSize;//接收缓存大小
		uint32_t init_SendBuffSize;//发送缓存大小
		uint32_t init_SendQueueSize;//发送池大小

		bool enable_accept_thread; //接收连接的线程独立
		bool enable_quick_ack; //TCP快速确认
	};

	struct LinkClientArgs{

		char appName[32];
		uint32_t appNo;

		//net
		//char ip[32];
		//uint16_t port;

		//other
		uint32_t threadNum; //发送线程数
		uint32_t init_connNum;//连接池
		uint32_t max_connNum;//最大连接池

		uint32_t init_RecvBuffSize;//接收缓存大小
		uint32_t init_SendBuffSize;//发送缓存大小
		uint32_t init_SendQueueSize;//发送池大小

	};
}

#endif//LINKSERVERARG_H_