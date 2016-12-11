#ifndef XSERVER_ARGS_H_
#define XSERVER_ARGS_H_

#include <xbase/os_common.h>
#include <string>
#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif

namespace x {

class Args {
private:
	Args();
	~Args();

public:
	typedef hash_map<std::string, std::string> ArgMap;

	static Args* Instance();
	
	void ParseArgs(int argc, char** argv);
	void SetArg(const char* key, const char* value);
	const char* GetArg(const char* name);
	ArgMap& GetArgs();

private:
	static Args* instance_;
	ArgMap args_;
};

}//namespace x

#endif //XSERVER_ARGS_H_
