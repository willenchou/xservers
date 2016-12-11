#ifndef XCONNECTION_XCONNECTION_H_
#define XCONNECTION_XCONNECTION_H_

#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <include/xservice_intf.h>
#include <include/xlog_intf.h>

namespace x {

class ConnectionIoc : public RefCountedThreadSafe<ConnectionIoc, IIocService> {
public:
	ConnectionIoc();
	~ConnectionIoc();
	
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

class ConnectionMng : public RefCountedThreadSafe<ConnectionMng, IMngService> {
public:
	ConnectionMng();
	~ConnectionMng();

protected:
	virtual void OnEvent(int eventType);
};

extern ConnectionIoc* _connIoc;
extern ConnectionMng* _connMng;
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

#endif //XCONNECTION_XCONNECTION_H_
