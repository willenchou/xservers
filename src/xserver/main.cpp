#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <xbase/os_common.h>
#include "version.h"
#include "args.h"
#include "server.h"

//全局变量定义
x::Server* _app = NULL;

void version(){
	std::cout << VERSION << std::endl;
}

void usage(){
	std::cout << "Usage:" << std::endl;
	std::cout << "./x [/path/to/xxx.xml] [options]" << std::endl;
	std::cout << "./x -v or --version" << std::endl;
	std::cout << "./x -h or --help" << std::endl;
}

void daemonize() {
#ifdef OS_LINUX
	if (fork() != 0) exit(0);
	setsid();

	int fd = open("/dev/null", O_RDWR, 0);
	if (fd != -1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO) 
			close(fd);
     }
#endif
}

int main(int argc, char** argv){
	if (argc >= 2) {
		if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
			version();
			return 0;
		}
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			usage();
			return 0;
		}
	}else{
		usage();
	}

	x::Args::Instance()->ParseArgs(argc, argv);

	_app = new x::Server();
	_app->Run();

	return 0;
}
