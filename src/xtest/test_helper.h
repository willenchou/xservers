#ifndef TEST_TEST_HELPER_H_
#define TEST_TEST_HELPER_H_
#ifndef FD_CONNSIZE
#	define FD_CONNSIZE 1024000
#endif // !FD_CONNSIZE
#include <stdlib.h>
#include <stdio.h>
#include <xbase/os_common.h>

#ifdef OS_WIN
# include <windows.h>
# define DLLHANDL HMODULE
# define DLLOPEN(dll) ::LoadLibrary(dll)
# define DLLFUNC(handle, name) GetProcAddress(handle, name)
# define DLLCLOSE(handle) ::FreeLibrary(handle)
#else
#include <dlfcn.h>
# define DLLHANDL void*
# define DLLOPEN(dll) dlopen(dll, RTLD_LAZY);
# define DLLFUNC(handle, name) dlsym(handle, name)
# define DLLCLOSE(handle) dlclose(handle)
#endif

typedef void* (FUNC_CALL * FUNCGetService)(const char* id);

extern int __failTest;
extern int __testNum;
extern int _port;
extern char* _ip;
extern int _testconnNum;

#define TEST(descr, c) do{		\
	__testNum++;printf("\n     -%d - %-30s: \n", __testNum, descr);		\
	if(c##_test::Test()) printf("PASSED\n"); else {printf("FAILED\n"); __failTest++;}		\
}while(0);

#define TEST_REPORT() do {	\
	printf("--------------------------------------------\n");	\
	printf("%d tests, %d passed, %d failed\n", __testNum, __testNum-__failTest,__failTest);	\
	if(__failTest){	printf("===WARNING=== we have failed test here...\n");}	\
}while(0)

static void* GetService(const char* dll, const char* service){
	char dllName[1024];
#ifdef OS_WIN
	sprintf(dllName, "%s.dll", dll);
#else
	sprintf(dllName, "lib%s.so", dll);
#endif

	DLLHANDL h = DLLOPEN(dllName);
	if(h){
		FUNCGetService func = (FUNCGetService)DLLFUNC(h, "GetService");
		if(func){
			return func(service);
		}
	}

	return NULL;
}

#endif //TEST_TEST_HELPER_H_
