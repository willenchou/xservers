#ifndef X_PLUGIN_XPIPE_XPIPE_H_
#define X_PLUGIN_XPIPE_XPIPE_H_

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
#include <include/xpipe_intf.h>
#include <include/xlog_intf.h>

namespace x{

#define PIPE_LINE_MAX_COUNT 1024

class PipeService : public RefCountedThreadSafe<PipeService, IPipeService>{
public:
	PipeService();
	~PipeService();

	void Init();
	void UnInit();
	void Start();
	void Stop();

protected:
	virtual PipeItem* GetPipeline(const char* firstPlugin);

private:
	void AddPipe(const char* strPipe);

	PipeItem* pipes_[PIPE_LINE_MAX_COUNT];
	uint32_t pipeCount_;
};

class PipeIoc : public RefCountedThreadSafe<PipeIoc, IIocService> {
public:
	PipeIoc();
	~PipeIoc();
	
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

class PipeMng : public RefCountedThreadSafe<PipeMng, IMngService> {
public:
	PipeMng();
	~PipeMng();

protected:
	virtual void OnEvent(int eventType);
};

extern PipeService* _pipeService;
extern PipeIoc* _pipeIoc;
extern PipeMng* _pipeMng;
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



#endif //X_PLUGIN_XPIPE_XPIPE_H_
