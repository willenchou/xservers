
#include "statementmysqladapter.h"

namespace x {

StatementMysqlAdapter::StatementMysqlAdapter(Connection* adptee)
:adaptee_(adptee){
	statementadptee = adptee;
	//adaptee_ = Query(adptee);
}


StatementMysqlAdapter::~StatementMysqlAdapter()
{
	 statementadptee = NULL;
	 
}

 int   StatementMysqlAdapter::prepare(char * sql){
	adaptee_<<sql; 
	return 0;
 }
 int  StatementMysqlAdapter::exec(void){
	adaptee_.exec();

	return 0;
}

IPack * StatementMysqlAdapter::open(void){
	StoreQueryResult res = adaptee_.store(); //Result 对象
	Row row;
	//StoreQueryResult::iterator i;

	//for(i = res.begin(); i != res.end(); i++)
	//{
	//	row = *i;
		//cout << row[0].c_str() << row[1].c_str << endl; //显示内容0、1
	//}

	return NULL;

}
}