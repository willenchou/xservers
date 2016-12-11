#include "xdb_impl.h"
#include "xdb.h"
#include <xbase/util.h>

namespace x {
DBService* _dbService = new DBService();
DBService::DBService(void)
	:initResult_(false)
{
}


DBService::~DBService(void)
{
}
void DBService::Init(){
	
	//msgService_ = (IMessageService*)_connIoc->GetContainer()->GetService(SID_MESSAGE);

	XMLElement* config = _dbIoc->GetConfig();
	if(!config) {
		return;
	}

	XMLElement* argsNode = config->FirstChildElement("args");
	if(argsNode){

	}

	XMLElement* dssNode = config->FirstChildElement("dss");

	bool loadresult;
	if(dssNode){
		XMLElement* dsNode = dssNode->FirstChildElement("ds");
		while(dsNode){
			loadresult = true;
			DSArgs* item = new DSArgs();

			const char* str = NULL;
			str = dsNode->Attribute("logic_name");
			if(str){
				//item->db_logicname = str;
				Util::StrCopy(item->db_logicname, str);
		
			}else{
			
				loadresult = false;
				_log->AddLog("",LOG_LEVEL_ERROR,"","");
			}
			str = dsNode->Attribute("ds_type");
			if(str){
				Util::StrCopy(item->ds_type, str);
				//item->db_type = atoi(str);
			}else{
				loadresult = false;
				_log->AddLog("",LOG_LEVEL_ERROR,"","");
			}
			str = dsNode->Attribute("init_conn_count");
			if(str){
	
				item->init_conn_count = atoi(str);
			}else{
				item->init_conn_count = 1;
			}
			str = dsNode->Attribute("max_conn_count");
			if(str){
				item->max_conn_count = atoi(str);
			}else{
				item->max_conn_count = item->init_conn_count+1;
			}
			str = dsNode->Attribute("wait_conn_time");
			if(str){
				item->wait_conn_time = atoi(str);
				
			}else{
				item->wait_conn_time = 2000;
			}

			str = dsNode->Attribute("server");
			if(str){
				Util::StrCopy(item->server, str);
			}else{
				Util::StrCopy(item->server, "localhost");
			}

			str = dsNode->Attribute("port");
			if(str){

				item->port = atoi(str);
			}else{
				item->port = 3306;
			}

			str = dsNode->Attribute("user_name");
			if(str){
				Util::StrCopy(item->user_name, str);

			}else{
				_log->AddLog("",LOG_LEVEL_ERROR,"","");
			}
			str = dsNode->Attribute("password");
			if(str){
				Util::StrCopy(item->password, str);
			}else{
				_log->AddLog("",LOG_LEVEL_ERROR,"","");
			}

			str = dsNode->Attribute("database_context");
			if(str){
				Util::StrCopy(item->database_context, str);
			}else{
				_log->AddLog("",LOG_LEVEL_ERROR,"","");
			}

			
			IDataSource* ds = NULL;
			if (loadresult )
			{
				ds = new DataSource();
				ds->Init(item);
				datasources_[item->ds_type] = ds;
			}
			else
			{
				delete item;
			}
			
			dsNode = dssNode->NextSiblingElement("ds");
		}
	}

	if(datasources_.size() >0)
	{
		initResult_ = true;
	}
}

void DBService::UnInit(){



}

void DBService::Start(){
	if (!initResult_)
	{
		return;
	}
	for(DataSourceMap::iterator it = datasources_.begin(); it != datasources_.end(); it++){
		IDataSource* ds = it->second;
		
		//ds->GetConn()->
	}
	

}

void DBService::Stop(){

}


 IDBConnection * DBService::getConnection(const char * datasource_name){

	IDataSource* ds = NULL;
	DataSourceMap::iterator it =  datasources_.find(datasource_name);

	if(it != datasources_.end()){
		ds = it->second;	
	}

	if (ds == NULL)
	{
		return NULL;
	}

	 return ds->GetConn();
 }
}
