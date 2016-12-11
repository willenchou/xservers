#include "args.h"
#include <include/xservice_intf.h>

namespace x {

Args* Args::instance_ = NULL;

Args::Args(){
}

Args::~Args(){
}

Args* Args::Instance(){
	if(!instance_){
		instance_ = new Args();
	}
	return instance_;
}

void Args::ParseArgs(int argc, char** argv){
	
	args_[std::string(ARGS_WORKSPACE)] = std::string("./");
	args_[std::string(ARGS_SYS_NO)] = std::string("0");
	args_[std::string(ARGS_SUB_SYS_NO)] = std::string("0");
	args_[std::string(ARGS_APP_NO)] = std::string("0");

	const char* key = NULL;
	const char* value = NULL;
	int index = 1;

	while(index < argc){
		if(!key){
			key = argv[index];	
		}else{
			if(strcmp(key, "-f") == 0){
				args_[std::string(ARGS_APP_CFG)] = std::string(argv[index]);
			}else if(strcmp(key, "-s") == 0){
				args_[std::string(ARGS_SYS_NO)] = std::string(argv[index]);
			}else if(strcmp(key, "-b") == 0){
				args_[std::string(ARGS_SUB_SYS_NO)] = std::string(argv[index]);
			}else if(strcmp(key, "-n") == 0){
				args_[std::string(ARGS_APP_NO)] = std::string(argv[index]);
			}else{
				if(*key == '-'){
					key = key++;
				}
				args_[std::string(key)] = std::string(argv[index]);
			}

			key = NULL;
		}
		index++;
	}
}

void Args::SetArg(const char* key, const char* value){
	args_[std::string(key)] = std::string(value);
}

const char* Args::GetArg(const char* name){
	ArgMap::iterator it = args_.find(std::string(name));
	if(it != args_.end()){
		return it->second.c_str();
	}
	return NULL;
}

Args::ArgMap& Args::GetArgs(){
	return args_;
}

}//namespace x
