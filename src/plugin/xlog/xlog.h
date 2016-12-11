#ifndef X_PLUGIN_XLOG_H_
#define X_PLUGIN_XLOG_H_

#include <xbase/os_common.h>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <string>
#include <time.h>
#include <include/xlog_intf.h>
#include <xbase/ring_queue.h>
#include <xbase/thread.h>
#include <xbase/mutex.h>
#include <xbase/event.h>

namespace x {

#define LOG_DIR "./log/"

class LogService : public RefCountedThreadSafe<LogService, ILog> 
		  , public Thread{
public:
	LogService();
	virtual ~LogService();

	void Init();
	void UnInit();
	void Start();
	void Stop();

protected:
	virtual long Run();
	virtual void FUNC_CALL AddLog(char* owner, int level, const char* location, char* content);
	
private:
	struct LogItem{
		time_t time;
		int level;
		char content[2048];
	};

	void CreateLogDir();
	void WriteLog(time_t time, int level, char* str);

	bool isSync_;
	int level_;
	RingQueue<LogItem> queue_;
	Mutex mutex_;
	Event event_;
};


class LogIocService : public RefCountedThreadSafe<LogIocService, IIocService> {
public:
	LogIocService();
	~LogIocService();
	
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

class LogMngService : public RefCountedThreadSafe<LogMngService, IMngService> {
public:
	LogMngService();
	~LogMngService();

protected:
	virtual void OnEvent(int eventType);
};

extern LogService* _logService;
extern LogIocService* _logIocService;
extern LogMngService* _logMngService;

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

#endif //X_PLUGIN_XLOG_H_
