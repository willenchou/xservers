#ifndef X_PLUGIN_XLOG_LOG_INTF_H_
#define X_PLUGIN_XLOG_LOG_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>

namespace x {

#define PLUGIN_LOG  "xlog"
#define SID_LOG "x.log"

#define LOG_LEVEL_DEBUG		0
#define LOG_LEVEL_WARN		1
#define LOG_LEVEL_ERROR		2
#define LOG_LEVEL_SYS		3

struct ILog : public IService{
	virtual void FUNC_CALL AddLog(char* owner, int level, const char* location, char* content) = 0;
};

#define LOG(owner, level, content)		\
do{	\
	char location[129];		\
	snprintf(location, 128, "%s:%d", __FILE__, __LINE__);		\
	_log->AddLog(owner, level, location, content);		\
}while(0)

#define LOG_F(owner, level, format, ...)		\
do{	\
	char location[129]={0};		\
	snprintf(location, 128, "%s:%d", __FILE__, __LINE__);		\
	char content[1025]={0};	\
	snprintf(content, 1024, format, __VA_ARGS__);	\
	_log->AddLog(owner, level, location, content);	\
}while(0)

#define LOG_DEBUG(owner, content)	\
	LOG(owner, LOG_LEVEL_DEBUG, content)
#define LOG_DEBUG_F(owner, format, ...)	\
	LOG_F(owner, LOG_LEVEL_DEBUG, format, __VA_ARGS__)

#define LOG_WARN(owner, content)	\
	LOG(owner, LOG_LEVEL_WARN, content)
#define LOG_WARN_F(owner, format, ...)	\
	LOG_F(owner, LOG_LEVEL_WARN, format, __VA_ARGS__)

#define LOG_ERROR(owner, content)	\
	LOG(owner, LOG_LEVEL_ERROR, content)
#define LOG_ERROR_F(owner, format, ...)	\
	LOG_F(owner, LOG_LEVEL_ERROR, format, __VA_ARGS__)

#define LOG_SYS(owner, content)	\
	LOG(owner, LOG_LEVEL_SYS, content)
#define LOG_SYS_F(owner, format, ...)	\
	LOG_F(owner, LOG_LEVEL_SYS, format, __VA_ARGS__)


}//namespace x

#endif //X_PLUGIN_XLOG_LOG_INTF_H_
