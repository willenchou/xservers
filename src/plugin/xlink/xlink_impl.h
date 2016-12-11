#ifndef XLINK_XLINK_IMPL_H_
#define XLINK_XLINK_IMPL_H_

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
#include <include/xlink_intf.h>
#include <xbase/ref_counted.h>
#include <xbase/thread.h>
#include <xbase/mutex.h>
#include <include/xlog_intf.h>

namespace x {

#define DEFAULT_NETOWRK_FILE "network.xml"
#define TIMER_INTERVAL 10

	class LinkService : public RefCountedThreadSafe<LinkService, ILINKService> 
	{
	public:
		LinkService();
		virtual ~LinkService();

		void Init();
		void UnInit();
		void Start();
		void Stop();

		bool Send(IMessage* msg);

	protected:
		
		virtual bool GetRoute(const char* nodeName, MsgRoute* route);

		virtual void RegisterPre(IMsgService* service);
		virtual void RegisterNext(IMsgService* service);
		virtual void PostMsg(IMessage* msg, IMsgService* from);

	private:
		uint32_t	sysNo_;				//系统号
		uint32_t	subSysNo_;			//子系统号
		char		appName_[32];		//应用名
		uint32_t    appNo_;				//应用编号

		IMsgService* preMsgService_;
		IMsgService* nextMsgService_;

		IMessageService* msgService_;

		bool initResult_;
	};

	extern LinkService* _connService;

}//namespace x


#endif //XLINK_XLINK_IMPL_H_


