#ifndef XDB_XDSARGS_H_
#define XDB_XDSARGS_H_
//#include <string>
////////////////////
namespace x {

	struct DSArgs{
		//数据源 逻辑数据库名称
		char db_logicname[64+1];//std::string db_logicname;

		//int db_type;
		char ds_type[64+1];
		//连接池信息
		int init_conn_count;
		int max_conn_count;
		int wait_conn_time;
    
		//连接数据库信息
		char server[64+1];
		int port;
		char user_name[64+1];
		char password[64+1];
		char database_context[128+1];
	};
}
#endif //XDB_XDSARGS_H_