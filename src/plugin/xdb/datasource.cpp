
#include "datasource.h"

namespace x {
DataSource::DataSource(void)
	:conns_(NULL)
{
}


DataSource::~DataSource(void)
{
	if (conns_)
	{
		delete conns_;
	}
}

 void DataSource ::Init(DSArgs* args){
	 conns_ =new x::ConnPool();
	 conns_->InitConnection(args);
     //心跳线程检测连接
 }
 IDBConnection* DataSource ::GetConn(){

	 if (conns_ == NULL){
		 return NULL;
	 }
	 //做个循环，直到取到为止
	return conns_->getConn();
 }
}
