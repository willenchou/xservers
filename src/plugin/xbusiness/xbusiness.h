#ifndef X_PLUGIN_XBUSINESS_XBUSINESS_H_
#define X_PLUGIN_XBUSINESS_XBUSINESS_H_

#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <string>
#include <vector>
#include <include/xbusiness_intf.h>
#include <include/xlog_intf.h>
#include <include/xmessage_intf.h>
#include "business_component.h"
#include "work_thread.h"

namespace x{

#define DEFAULT_THREAD_NUM		5
#define MAX_THREAD_NUM			50

class BusinessService : public RefCountedThreadSafe<BusinessService, IBusinessService>{
public:
	BusinessService();
	~BusinessService();

	void Init();
	void UnInit();
	void Start();
	void Stop();
	
	virtual void RegisterPre(IMsgService* service);
	virtual void RegisterNext(IMsgService* service);
	virtual void PostMsg(IMessage* msg, IMsgService* from);

	void DealMessage(BusinessContext* context, IMessage* msg);

	inline IMessageService* GetMessageService() { return msgService_; }
    
    inline IDBService* GetDBService() { return dbService_; }
private:
	int cfgThreadNum_;
	
	IDBService* dbService_;
	IMessageService* msgService_;
	IMsgService* servicePre_;
	IMsgService* serviceNext_;

	BusinessComponent* components_;
	WorkThread** threads_;
	uint32_t threadCount_;
	uint32_t currThreadIndex_;
	BusinessContext* context_;
};

class BusinessIoc : public RefCountedThreadSafe<BusinessIoc, IIocService> {
public:
	BusinessIoc();
	~BusinessIoc();
	
	IContainer* GetContainer();
	XMLElement* GetConfig();
	const char* GetArg(const char* name);

protected:
	virtual void SetContainer(IContainer* container);
	virtual void SetConfig(XMLElement* config);
	virtual void SetArg(const char* name, const char* value);

private:
	typedef hash_map<std::string, std::string> ArgMap;
	IContainer* container_;
	XMLElement* config_;
	ArgMap args_;
};

class BusinessMng : public RefCountedThreadSafe<BusinessMng, IMngService> {
public:
	BusinessMng();
	~BusinessMng();

protected:
	virtual void OnEvent(int eventType);
};

extern BusinessService* _businessService;
extern BusinessIoc* _businessIoc;
extern BusinessMng* _businessMng;
extern ILog* _log;

}//namespace x

#ifdef __cplusplus
extern "C" 
{
#endif

void FUNC_CALL GetPluginInfo(x::PluginInfo& info);
bool FUNC_CALL GetServiceInfo(int index, x::ServiceInfo& info);
x::IService* FUNC_CALL GetService(const char* id);

#ifdef __cplusplus
}
#endif



#endif //X_PLUGIN_XBUSINESS_XBUSINESS_H_
