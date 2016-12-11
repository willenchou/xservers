
#include "test_helper.h"
#include "xmessage_test.h"
#include "xserverconn_test.h"
#include "xlinkconn_test.h"

int __failTest=0;
int __testNum=0;
int _testconnNum =0;

char* _ip = NULL;
int _port = TEST_PORT;

const int TEST_XLINK_SERVER = 0;
const int TEST_XLINK_CLIENT = 1;
const int TEST_XMESSAGE = 3;
int main(int argc, char** argv){

	_testconnNum =FD_CONNSIZE -1;
	//TEST("test xmessage", xmessage);
	if (argc < 2) {

		TEST_REPORT();
		getchar();
		return 0;
	}

	const char* key = NULL;
	const char* value = NULL;
	int dll = 0;
	key = argv[1];	

	if(strcmp(key, "-d") != 0){
		return 0;
	}

	value = argv[2];
	
	dll = atoi(value);

	switch ( dll)
	{
	case TEST_XLINK_SERVER:
		if (argc >= 6)
		{
			_ip = argv[4];

			_port = atoi(argv[6]);

		}
		if (argc >= 8)
		{
			
			_testconnNum = atoi(argv[8]);
			if (_testconnNum > (FD_CONNSIZE -1))
			{
				_testconnNum = FD_CONNSIZE -1;
			}
		}
		TEST("test xlink server",xlink);

		TEST_REPORT();

		getchar();

		return 0;
		break;
	case  TEST_XLINK_CLIENT:
		if (argc >= 6)
		{
			_ip = argv[4];

			_port = atoi(argv[6]);

		}
		if (argc >= 8)
		{
			_testconnNum = atoi(argv[8]);
			if (_testconnNum > FD_CONNSIZE)
			{
				_testconnNum = FD_CONNSIZE;
			}
		}
		TEST("test xlink client",xlinkconn);

		TEST_REPORT();

		getchar();

		return 0;
		break;
	case  TEST_XMESSAGE:
		break;
	default:
		break;
	}
	
	return 0;
}

