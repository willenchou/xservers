#pragma once

#include <string>
#include <map>
#include "Idatasource.h"
#include "datasource.h"
#include "datasourceloader.h"

#include <include/xdb_intf.h>

namespace x {

class DBService: public RefCountedThreadSafe<DBService, IDBService> 
{
public:
	DBService(void);
	~DBService(void);
public:
	void Init();
	void UnInit();
	void Start();
	void Stop();

	/////////////////////////////IDBService//////////////////////////////
	virtual IDBConnection * getConnection(const char * datasource_name);
protected:
	IDataSource* getDataSource(const char * datasource_name);
private:
	void LoadDataSource();
private:
	typedef std::map<std::string, IDataSource*> DataSourceMap;
	bool initResult_; 

	DataSourceMap  datasources_;//IDataSource
	DataSourceLoader dsLoader; //不同类型数据库连接的DLL加载。

};

extern DBService* _dbService;
}//namespac x

