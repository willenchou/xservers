#pragma once
#include <string>
#include <list>
#include "include/xdb_intf.h"
#include "dsargs.h"
#include "connmysqlppadapter.h"
#include "mysql++.h"
using namespace mysqlpp;
namespace x {


class ConnPool:public mysqlpp::ConnectionPool  
{
public:
	ConnPool(void);
	~ConnPool(void);
public:
	 //void InitConnection(int iInitialSize); //初始化数据库连接池

	 void InitConnection(DSArgs* args); //初始化数据库连接池
	 IDBConnection* getConn();
protected:
	virtual Connection* create();
	virtual void destroy(Connection*);
	virtual unsigned int max_idle_time();
private:
	//IDBConnection* CreateConnection();
	ConnMysqlppAdapter* my_grab();
private:
	//连接池信息
	int init_conn_count;
	int cur_conn_count;
	int max_conn_count;
	int wait_conn_time;

	//连接数据库信息
	std::string server_url;
	int port;
	std::string user_name;
	std::string password;
	std::string database_context;

	typedef std::map<Connection*,ConnMysqlppAdapter*> ConnMap;
	ConnMap list_; //连接池的容器队列

};
}

