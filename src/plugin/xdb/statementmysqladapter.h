#pragma once
#include "connection.h"
#include <include/xdb_intf.h>
#include "connmysqlppadapter.h"
#include "mysql++.h"
using namespace mysqlpp;
namespace x {
class StatementMysqlAdapter:public IDBStatement
{
public:
	StatementMysqlAdapter(Connection* adptee);
	~StatementMysqlAdapter();

	//IDBStatement
	virtual int   prepare(char * sql);

	//执行增，删，改等操作。
	virtual int  exec(void);
	//执行查询返回结果集等操作
	virtual IPack * open(void);

	//adaptee

	mysqlpp::Connection* statementadptee;
	mysqlpp::Query adaptee_;
	
};
}

