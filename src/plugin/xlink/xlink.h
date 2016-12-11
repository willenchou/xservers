#ifndef XLINK_XLINK_H_
#define XLINK_XLINK_H_

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

	class LinkIoc : public RefCountedThreadSafe<LinkIoc, IIocService> {
	public:
		LinkIoc();
		~LinkIoc();

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

	class LinkMng : public RefCountedThreadSafe<LinkMng, IMngService> {
	public:
		LinkMng();
		~LinkMng();

	protected:
		virtual void OnEvent(int eventType);
	};

	extern LinkIoc* _connIoc;
	extern LinkMng* _connMng;
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

#endif //XLINK_XLINK_H_

