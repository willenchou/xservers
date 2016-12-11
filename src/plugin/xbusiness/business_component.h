#ifndef XBUSINESS_BUSINESS_COMPONENT_H_
#define XBUSINESS_BUSINESS_COMPONENT_H_

#include <xbase/os_common.h>
#include <include/xbusiness_intf.h>

namespace x {

#ifdef _WIN32
# include <windows.h>
# define DLLHANDL HMODULE
# define DLLOPEN(dll) ::LoadLibrary(dll)
# define DLLFUNC(handle, name) GetProcAddress(handle, name)
# define DLLCLOSE(handle) ::FreeLibrary(handle)
#else
# define DLLHANDL void*
# define DLLOPEN(dll) dlopen(dll, RTLD_LAZY);
# define DLLFUNC(handle, name) dlsym(handle, name)
# define DLLCLOSE(handle) dlclose(handle)
#endif

static const uint32_t MAX_COMPONENT_COUNT = 1024;
static const uint32_t MAX_FUNC_COUNT = 10000000;

typedef int (FUNC_CALL * PFBusiCompInit)();
typedef int (FUNC_CALL * PFBusiCompUnInit)();
typedef const BusinessFuncEntry* (FUNC_CALL * PFBusiGetFuncEntry)();

class BusinessComponent {
public:
	BusinessComponent();
	~BusinessComponent();

	bool Load(const char* name);
	void UnLoadAll();
	int CallFunc(uint32_t funcID, IBusinessContext* context, IPack* in, IPack* out);

private:
	struct CompItem{
		DLLHANDL handle;
		PFBusiCompInit init;
		PFBusiCompUnInit unInit;
		PFBusiGetFuncEntry getEntry;
	};

	struct FuncItem {
		uint32_t funcID;
		PFBusinessFunc funcEntry;
	};

	void AddFuncEntry(uint32_t funcID, PFBusinessFunc func);
	PFBusinessFunc GetFuncEntry(uint32_t funcID);

	CompItem* compnents_;
	uint32_t compnentCount_;
	FuncItem** funcs_;
};

}//namespace x

#endif //XBUSINESS_BUSINESS_COMPONENT_H_
