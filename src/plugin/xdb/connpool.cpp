
#include "connpool.h"
#include "connmysqlppadapter.h"

namespace x {
ConnPool::ConnPool(void):ConnectionPool()
{
}


ConnPool::~ConnPool(void)
{
}
//void ConnPool::InitConnection(int iInitialSize){
//}
void ConnPool::InitConnection(DSArgs* args){
	//
	clear(true);
	//连接池信息
	 init_conn_count = args->init_conn_count;
	 cur_conn_count = 0;
	 max_conn_count = args->max_conn_count;
	 wait_conn_time = args->wait_conn_time;

	//连接数据库信息
	server_url = args->server;
    port = args->port;
	user_name = args->user_name;
	password = args->password;
	database_context = args->database_context;

	//
	
	ConnMysqlppAdapter *conn;
	
	for (int i = 0; i < args->init_conn_count; i++) {
		conn = this->my_grab();
			++(this->cur_conn_count);
		}  
	
}
IDBConnection* ConnPool::getConn(){
	//pthread_mutex_lock(&lock);
	ConnMysqlppAdapter* conn = my_grab();
	conn->SetStatus(XDB_CONNECTION_STATUS_BUSY);
    return conn;
	
	// //存在连接
	//if (list.size() > 0) {   //连接池容器中还有连接
	//	conn = list.front(); //得到第一个连接
	//	list.pop_front();   //移除第一个连接

	//	if (conn->getConnStatus() == XDB_CONNECTION_STATUS_FREE ) {   //如果连接已经被关闭，删除后重新建立一个
	//		return conn;
	//	}

	//	delete conn;
	//	conn = NULL;
	//	--cur_conn_count;
	//}
	//
	////超过最大连接数
	//if (cur_conn_count > max_conn_count)
	//{
	//	//pthread_mutex_unlock(&lock);
	//	return NULL;
	//}
	//
	//conn = this->CreateConnection();
	//if (conn != NULL) {
	//	++cur_conn_count;		
	//} 
	////pthread_mutex_unlock(&lock);
	//return NULL;
}


ConnMysqlppAdapter* ConnPool::my_grab(){

	//更改
	mysqlpp::Connection* c = NULL;
	try
	{
		 grab();
	}
	catch (Exception* e)
	{
		perror("grab");
	}
	
	

	if (!c)
	{
		return NULL;
	}
	if (list_.find(c)!= list_.end())
	{
		ConnMysqlppAdapter* conn = new ConnMysqlppAdapter(c);
		list_[c] = conn;
	}
	 
	return list_[c];
	
     
	// conn->SetStatus(XDB_CONNECTION_STATUS_FREE) ;
	// return conn;
}

//////////////////////////////////////////////////////////////////////////
 Connection* ConnPool::create(){
	
	 return new mysqlpp::Connection();
	 mysqlpp::Connection* conn = NULL;
	try
	{
		//mysqlpp::Connection* conn =  new mysqlpp::Connection();
		 conn =  new mysqlpp::Connection(database_context.c_str(), server_url.c_str(), user_name.c_str(),
			password.c_str(), port);
		
	}
	catch (OptionalExceptions* e)
	{
		if (conn)
		{
			delete conn;
			conn = NULL;
		}
        return conn;
	}

	return conn;
 }
 void ConnPool::destroy(Connection*){

 }
 unsigned int ConnPool::max_idle_time(){
	 return 1000;
 }

}
