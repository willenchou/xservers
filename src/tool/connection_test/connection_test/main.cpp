#include <stdlib.h>
#include <stdio.h>
#include "connection_test.h"

int main(int argc, char** argv){
	x::ConnectionApi* api = x::NewConnectionAPI();
	Callback* callback = new Callback(api);
	api->Init(callback);
	if(api->Connect("127.0.0.1", 18001) != 0){
		printf("Á¬½ÓÊ§°Ü\n");
	}

	getchar();

	x::IMessage * msg = api->NewMessage();
	msg->SetFuncID(1000000);
	api->Send(msg);


	getchar();

	return 0;
};