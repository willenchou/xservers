#ifndef XCONNECTION_NETWORK_CONFIG_H_
#define XCONNECTION_NETWORK_CONFIG_H_

#include <include/xconnection_intf.h>
#include <xbase/util.h>
#include <xbase/xml.h>

namespace x{

#define MAX_NET_NODE_COUNT 1024

class NetworkConfig {
public:
	struct Node{
		uint32_t sysNo;
		uint32_t subSysNo;
		char appName[32];
		uint32_t appNo;
		char ip[32];
		uint16_t port;
	};

	NetworkConfig();
	~NetworkConfig();

	bool Init(const char* configFile);
	Node* GetNode(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo);
	uint32_t GetParentCount(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo);
	Node* GetParent(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo, uint32_t index);
	Node* FindRouteNode(MsgRoute* src, MsgRoute* dest);

private:
	struct ConfigNode{
		Node* node;
		ConfigNode* next;
		ConfigNode* parent;
	};

	bool LoadConfig(const char* configFile);
	void BuildRelation();
	
	ConfigNode* configNodes_;
	Node* nodes_[MAX_NET_NODE_COUNT];
	uint32_t nodeCount_;
	uint8_t nodeRelation_[MAX_NET_NODE_COUNT][MAX_NET_NODE_COUNT];
};

}//namespace x

#endif //XCONNECTION_NETWORK_CONFIG_H_
