#include "connmysqlppadapter.h"
#include "statementmysqladapter.h"
namespace x {

ConnMysqlppAdapter::ConnMysqlppAdapter(mysqlpp::Connection* conn)
	:conn_(conn)
{
	//conn_ = conn;
	conn_status = XDB_CONNECTION_STATUS_FREE;
}


ConnMysqlppAdapter::~ConnMysqlppAdapter(void)
{
}


 int ConnMysqlppAdapter:: getConnStatus(void) {
	 return conn_status;

 }
 IDBStatement* ConnMysqlppAdapter:: createStatement(void){
	 IDBStatement* statement= new StatementMysqlAdapter(conn_);
	 return statement;
 }
 void* ConnMysqlppAdapter::toFree(){
	 if (conn_)
	 {
		// delete conn_;
	 }
	 return NULL;
 }
 bool ConnMysqlppAdapter:: connect(const char* db , const char* server,
	 const char* user , const char* password,unsigned int port){
	/*if (conn_ == NULL)
	{
	conn_ = new Connection();
	}*/
	if (conn_ == NULL)
	{
		return false;
	}
	return conn_->connect(db,server,user,password,port);
 }
}
