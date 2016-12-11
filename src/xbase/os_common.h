#ifndef XBASE_OS_COMMON_H_
#define XBASE_OS_COMMON_H_

//-----------------------------------------------
//操作系统定义
//------------------------------------------------
#if defined(_AIX) && !defined(OS_AIX)
	#define OS_AIX
#endif

#if defined(__hpux) && !defined(OS_HPUX)
	#define OS_HPUX
#endif

#if defined(__SVR4) && !defined(OS_SOLARIS)
	#define OS_SOLARIS
#endif

#if defined(__FreeBSD__) && !defined(OS_FREEBSD)
	#define OS_FREEBSD
#endif

#if defined(__linux) && !defined(OS_LINUX)
	#define OS_LINUX
#endif

#if defined(__APPLE__) && !defined(OS_APPLE)
	#define OS_APPLE
#endif

#if (defined(_WIN32) || defined(_WIN64)) && !defined(OS_WIN)
	#define OS_WIN
#endif

#if !defined(OS_WIN) && !defined(OS_LINUX) && !defined(OS_UNIX)
	#define OS_UNIX
#endif

//--------------------------------------------
//整形数据定义
//-----------------------------------------------------
#if defined(OS_WIN)
	#if defined(_MSC_VER) && (_MSC_VER < 1800)
		#if _MSC_VER < 1300
			typedef signed char			int8_t;
			typedef signed short		int16_t;
			typedef signed int			int32_t;
			typedef unsigned char		uint8_t;
			typedef unsigned short		uint16_t;
			typedef unsigned int		uint32_t;
		#else
			typedef signed __int8		int8_t;
			typedef signed __int16		int16_t;
			typedef signed __int32		int32_t;
			typedef unsigned __int8		uint8_t;
			typedef unsigned __int16	uint16_t;
			typedef unsigned __int32	uint32_t;
		#endif
		
		typedef signed __int64			int64_t;
		typedef unsigned __int64		uint64_t;
		
		#if defined(_WIN64)
			#define __PRI64_PREFIX		"l"
		#else
			#if _MSC_VER < 1300
				#define __PRI64_PREFIX		"I64"
			#else
				#define __PRI64_PREFIX		"ll"
			#endif
		#endif

		#define PRId8          "d"
		#define PRId16         "d"
		#define PRId32         "d"
		#define PRId64         __PRI64_PREFIX "d"
				
		#define PRIu8          "u"
		#define PRIu16         "u"
		#define PRIu32         "u"
		#define PRIu64         __PRI64_PREFIX "u"

		#define PRIx8           "x"
		#define PRIx16         "x"
		#define PRIx32         "x"
		#define PRIx64         __PRI64_PREFIX "x"

		#define PRIX8          "X"
		#define PRIX16         "X"
		#define PRIX32         "X"
		#define PRIX64         __PRI64_PREFIX "X"
	#else
		#include <stdint.h>
		#include <inttypes.h>
	#endif
#else
	#include <inttypes.h>
#endif

#include <string.h>
#include <stdio.h>
#ifdef OS_WIN
#	ifndef FD_CONNSIZE
#	define FD_CONNSIZE FD_SETSIZE
#	endif
#	ifndef  FD_SETSIZE
#		define  FD_SETSIZE  10240
#	endif
#else
#	ifndef FD_CONNSIZE
#	 define FD_CONNSIZE 10240
#   endif
#endif

#ifdef OS_WIN
	#include <WinSock2.h>
	#include <Windows.h>
	#include <direct.h>  
	#include <io.h>
#else
	#ifdef OS_APPLE
		#include <sys/event.h>
		#include <sys/syscall.h>	// syscall(SYS_gettid)
	#else
		#include <sys/epoll.h>
	#endif
	#include <pthread.h>
	#include <fcntl.h>
	#include <strings.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <errno.h>
	#include <unistd.h>
	
	#include <signal.h>
#endif

#if defined(__HP_aCC)
///对于gcc 编译器
#elif defined(__GNUC__)
#include <ctype.h>
#endif

#ifdef OS_WIN
	#if !defined( FUNC_CALL )
	#define FUNC_CALL __stdcall
	#endif
#else
	#define FUNC_CALL
#endif

#ifndef OS_WIN
#define ioctlsocket ioctl
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

#ifdef OS_WIN
	#define	 snprintf sprintf_s
#endif

#ifdef OS_WIN
	#define ACCESS _access  
	#define MKDIR(a) _mkdir((a)) 
#else
	#define ACCESS access  
	#define MKDIR(a) mkdir((a),0755)  
#endif
#ifndef OS_WIN
#define InterlockedIncrement(x)  __sync_add_and_fetch((x),1) //atomic_inc(x)  
#define InterlockedDecrement(x)  __sync_sub_and_fetch((x),1) //atomic_dec(x)  
#endif

#ifndef OS_WIN
inline uint32_t GetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec*1000+ts.tv_nsec/1000000;
}
#endif // _WIN32
#endif //XBASE_OS_COMMON_H_
