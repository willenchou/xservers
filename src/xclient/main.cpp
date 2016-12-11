#include <stdlib.h>
#include <stdio.h>
#include "client.h"

int main(int argc, char** argv){
	if (argc == 1){
		printf("请指定测试文件\n");
		return 0;
	}
	
	Client* client = new Client();
	
	if (client->Connect()){
		client->Test(argv[1]);
	}

	fflush(stdin);
	getchar();

	return 0;
}