#ifndef XDB_XDATASOURCE_H_
#define XDB_XDATASOURCE_H_
#include <include/xdb_intf.h>
#include "dsargs.h"
namespace x {
//IDataSourceAdapter
	struct IDataSource
	{
		virtual void Init(DSArgs* args)=0;
		virtual IDBConnection* GetConn()=0;
	};
}
#endif//XDB_XDATASOURCE_H_