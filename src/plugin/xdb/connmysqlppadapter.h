#pragma once

#include <include/xdb_intf.h>
#include "mysql++.h"

namespace x {
class ConnMysqlppAdapter:public IDBConnection
{
public:
	ConnMysqlppAdapter(mysqlpp::Connection* conn);
	~ConnMysqlppAdapter(void);

	virtual int  getConnStatus(void); 
	virtual IDBStatement* createStatement(void);
	virtual void*  toFree();
    
	bool connect(const char* db = 0, const char* server = 0,
		const char* user = 0, const char* password = 0,
		unsigned int port = 0);
	void SetStatus(int status){ conn_status = status; };
private:
	int conn_status;
	//adaptee
	mysqlpp::Connection* conn_;

};
}

