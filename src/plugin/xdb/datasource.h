#pragma once
//#define  MYSQLPP_MYSQL_HEADERS_BURIED
#include "connpool.h"

#include "Idatasource.h"

namespace x {
//adapter
class DataSource:public IDataSource
{
public:
	DataSource(void);
	~DataSource(void);

public:
	virtual void Init(DSArgs* args);
	virtual IDBConnection* GetConn();

private:
	DSArgs* args_;
	//adaptee
	ConnPool* conns_;
};
}

