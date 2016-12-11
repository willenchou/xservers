#ifndef XDB_SERVICE_INTF_H_
#define XDB_SERVICE_INTF_H_

#include <xbase/os_common.h>
#include <include/xservice_intf.h>
#include <include/xmessage_intf.h>

namespace x {

#define PLUGIN_DB		"xdb"
#define SID_DB			"x.db"

	

	const int XDB_CONNECTION_STATUS_DISCONN                    = 0;//连接为断开状态
	const int XDB_CONNECTION_STATUS_FREE                       = 1;//连接为空闲状态
	const int XDB_CONNECTION_STATUS_BUSY                       = 2;//连接为忙状态
	const int XDB_CONNECTION_STATUS_BUSY_AND_DISCONN           = 3;//连接为忙并且断开状态。

	

	

	struct IDBStatement{
	    ///准备要调用的sql语句或存储过程语句
		/**@param  char *sql  标准SQL语句或存储过程语句(参见扩展的ICallableStatement接口)
		*@return 0成功，1失败
		*/
		virtual int   prepare(char * sql) = 0;
	
		///执行一条update,insert,delete,truncate的sql语句
		/**@param  sql: 一条update,insert,delete,truncate的sql语句
		*@return 0成功，1失败
		*/
		virtual int  exec(void) = 0;

		///执行一条select语句
		/**@param   sql: 一个结果集返回的sql语句
		*@return  :结果集指针
		*/
		virtual IPack * open(void) = 0;
	};
	//Target
	struct IDBConnection{
		virtual int  getConnStatus(void) = 0; 
		virtual IDBStatement * createStatement(void)=0;
		virtual void *  toFree() = 0;
	};
	//数据库统一访问服务。
	struct IDBService : public IService{
		//virtual IDBConnection* getConnection(int func_id)=0;
		virtual IDBConnection*  getConnection(const char* datasource_name) = 0;
	};
}//namespace

#endif //XDB_SERVICE_INTF_H_
