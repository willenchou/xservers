#include "network_config.h"

namespace x{

NetworkConfig::NetworkConfig()
:nodeCount_(0)
,configNodes_(NULL){
	memset(nodeRelation_, 0, sizeof(uint8_t) * MAX_NET_NODE_COUNT * MAX_NET_NODE_COUNT);
}

NetworkConfig::~NetworkConfig(){
	for(uint32_t i = 0; i < nodeCount_; i++){
		delete nodes_[i];
	}
}

bool NetworkConfig::Init(const char* configFile){
	if(!LoadConfig(configFile))
		return false;

	BuildRelation();

	return true;
}

NetworkConfig::Node* NetworkConfig::GetNode(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo){
	for(uint32_t i = 0; i < nodeCount_; i++){
		Node* node = nodes_[i];
		if(node->sysNo == sysNo &&
			node->subSysNo == subSysNo &&
			strcmp(node->appName, appName) == 0 &&
			node->appNo == appNo)
		{
			return node;
		}
	}
	return NULL;
}

uint32_t NetworkConfig::GetParentCount(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo){
	uint32_t count = 0;
	for(uint32_t i = 0; i < nodeCount_; i++){
		Node* node = nodes_[i];
		if(node->sysNo == sysNo &&
			node->subSysNo == subSysNo &&
			strcmp(node->appName, appName) == 0 &&
			node->appNo == appNo)
		{
			for(uint32_t j = 0; j < MAX_NET_NODE_COUNT; j++){
				if(nodeRelation_[i][j] == 1)
					count++;
			}
		}
	}
	return count;
}

NetworkConfig::Node* NetworkConfig::GetParent(uint32_t sysNo, uint32_t subSysNo, const char* appName, uint32_t appNo, uint32_t index){
	uint32_t count = 0;
	for(uint32_t i = 0; i < nodeCount_; i++){
		Node* node = nodes_[i];
		if(node->sysNo == sysNo &&
			node->subSysNo == subSysNo &&
			strcmp(node->appName, appName) == 0 &&
			node->appNo == appNo)
		{
			for(uint32_t j = 0; j < MAX_NET_NODE_COUNT; j++){
				if(nodeRelation_[i][j] == 1){
					if(count == index)
						return nodes_[j];
					count++;
				}					
			}
		}
	}

	return NULL;
}

NetworkConfig::Node* NetworkConfig::FindRouteNode(MsgRoute* src, MsgRoute* dest){
	int srcIndex = -1, destIndex = -1, resultIndex = -1;
	for(uint32_t i = 0; i < nodeCount_; i++){
		Node* node = nodes_[i];
		
		if(srcIndex == -1 &&
			src->sysNo == node->sysNo &&
			src->subSysNo == node->subSysNo &&
			strcmp(src->appName, node->appName) == 0 &&
			src->appNo == node->appNo)
		{
			srcIndex = i;
		}
		
		if(destIndex == -1 &&
			dest->sysNo == node->sysNo &&
			dest->subSysNo == node->subSysNo &&
			strcmp(dest->appName, node->appName) == 0 &&
			dest->appNo == node->appNo)
		{
			destIndex = i;
		}
	
		if(srcIndex != -1 && destIndex != -1)
			break;
	}
	
	if(srcIndex == -1 || destIndex == -1){
		return NULL;
	}

	bool* visitFlags = new bool[nodeCount_];
	memset(visitFlags, 0, sizeof(bool) * nodeCount_);

	uint32_t* nodeQueue = new uint32_t[nodeCount_];
	uint32_t nodeQueueCount = 0;

	nodeQueue[nodeQueueCount++] = destIndex;
	visitFlags[destIndex] = true;
	while(nodeQueueCount != 0){
		uint32_t curr = nodeQueue[nodeQueueCount - 1];
		
		bool hasNext = false;
		for(uint32_t i = 0; i < nodeCount_; i++){
			if(nodeRelation_[curr][i] != 0){
				if(i == srcIndex){
					resultIndex = curr;
					break;
				}else{
					if(visitFlags[i] == false){
						nodeQueue[nodeQueueCount++] = i;
						visitFlags[i] = true;
						hasNext = true;
					}
				}
			}
		}

		if(!hasNext)
			nodeQueueCount--;
	
		if(resultIndex != -1)
			break;
	}

	delete [] visitFlags;
	delete [] nodeQueue;

	if(resultIndex != -1){
		return nodes_[resultIndex];
	}

	return NULL;

/*
	//假设已经寻到源节点和目标节点的位置
	uint32_t nSrc,nDest;
	int32_t nNextIndex = -1;
	bool bVisit[nodeCount_];
	memset(bVisit,0,sizeof(bVisit));
	std::queue<uint32_t> que;
	que.push(nDest);
	bVisit[nDest] = true;
	while(!que.empty()){
		uint32_t cur = que.front();
		que.pop();
		for(uint32_t i = 0; i < nodeCount_; i++){
			if(nodeRelation_[cur][i] != 0)
			{
				if(i == nSrc){
					nNextIndex = i;
					break;
				}
				if(bVisit[i] == false){
					bVisit[i] = true;
					que.push(i);
				}
			}
		}
	}
	//找到为nNextIndex的节点
	if(nNextIndex != -1)
		return;
*/
}

bool NetworkConfig::LoadConfig(const char* configFile){
	XMLDocument doc;
	if(doc.LoadFile(configFile) != XML_SUCCESS){
		return false;
	}

	XMLElement* nodeNetwork = doc.FirstChildElement("network");
	if(!nodeNetwork){
		return false;
	}

	XMLElement* nodeGroup = nodeNetwork->FirstChildElement("group");
	while(nodeGroup){
		uint32_t sysNo = nodeGroup->IntAttribute("system_no");
		uint32_t subSysNo = nodeGroup->IntAttribute("sub_system_no");
		
		ConfigNode* lastConfigNode = NULL;
		XMLElement* nodeApp = nodeGroup->FirstChildElement("app");
		while(nodeApp){
			const char* appName = nodeApp->Attribute("name");
			uint32_t appNo = nodeApp->IntAttribute("no");
			const char* appIP = nodeApp->Attribute("ip");
			const char* appPort = nodeApp->Attribute("port");

			if(!appName || !appIP || !appPort)
				continue;
		
			Node* node = new Node();
			node->sysNo = sysNo;
			node->subSysNo = subSysNo;
			Util::StrCopy(node->appName, appName);
			node->appNo = appNo;
			Util::StrCopy(node->ip, appIP);
			node->port = atoi(appPort);	
			
			ConfigNode* configNode = new ConfigNode();
			configNode->node = node;
			configNode->next = NULL;
			configNode->parent = NULL;

			if(lastConfigNode) 
				lastConfigNode->next = configNode;
			lastConfigNode = configNode;
			
			if(!configNodes_)
				configNodes_ = configNode;

			XMLElement* nodeParents = nodeApp->FirstChildElement("parents");
			if(nodeParents){
				ConfigNode* lastParentConfigNode = NULL;
				XMLElement* nodeParent = nodeParents->FirstChildElement("parent");
				while(nodeParent){
					uint32_t parentSysNo = nodeParent->IntAttribute("system_no");
					uint32_t parentSubSysNo = nodeParent->IntAttribute("sub_system_no");
					const char* parentAppName = nodeParent->Attribute("app_name");
					const char* parentAppNo = nodeParent->Attribute("app_no");
					
					if(!parentAppName)
						continue;
					
					Node* parentNode = new Node();
					parentNode->sysNo = parentSysNo;
					parentNode->subSysNo = parentSubSysNo;
					Util::StrCopy(parentNode->appName, parentAppName);
					parentNode->appNo = parentAppNo ? atoi(parentAppNo) : (uint32_t)-1;
					
					ConfigNode* parentConfigNode = new ConfigNode();
					parentConfigNode->node = parentNode;
					parentConfigNode->next = NULL;
					parentConfigNode->parent = NULL;

					if(lastParentConfigNode)
						lastParentConfigNode->next = parentConfigNode;
					lastParentConfigNode = parentConfigNode;

					if(configNode->parent == NULL)
						configNode->parent = parentConfigNode;

					nodeParent = nodeParent->NextSiblingElement("parent");
				}
			}			

			nodeApp = nodeApp->NextSiblingElement("app");			
		}

		nodeGroup = nodeGroup->NextSiblingElement("group");
	}

	return true;
}

void NetworkConfig::BuildRelation(){
	ConfigNode* currConfigNode = configNodes_;
	while(currConfigNode){
		nodes_[nodeCount_++] = currConfigNode->node;
		if(nodeCount_ >= MAX_NET_NODE_COUNT)
			break;

		currConfigNode = currConfigNode->next;
	}
	
	currConfigNode = configNodes_;
	for(uint32_t i = 0; i < nodeCount_; i++){
		ConfigNode* parentConfigNode = currConfigNode->parent;
		while(parentConfigNode){
			Node* parentNode = parentConfigNode->node;
			for(uint32_t j = 0; j < nodeCount_; j++){
				if( j != i &&
					parentNode->sysNo == nodes_[j]->sysNo &&
					parentNode->subSysNo == nodes_[j]->subSysNo &&
					strcmp(parentNode->appName, nodes_[j]->appName) == 0 &&
					(parentNode->appNo == (uint32_t)-1 || parentNode->appNo == nodes_[j]->appNo))
				{
					nodeRelation_[i][j] = 1;
					nodeRelation_[j][i] = 2;
				}
			}

			parentConfigNode = parentConfigNode->next;
		}
		currConfigNode = currConfigNode->next;
	}
}

}//namespace x

