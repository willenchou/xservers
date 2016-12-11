#ifndef XBUSINESS_BUSINESS_CONTEXT_H_
#define XBUSINESS_BUSINESS_CONTEXT_H_

#include <include/xbusiness_intf.h>
#include <include/xmessage_intf.h>
#include <include/xdb_intf.h>

namespace x {

class BusinessContext : public IBusinessContext {
public:
	BusinessContext(IMessageService* msgService,IDBService* dbservice);
	~BusinessContext();
	
	inline IPack* GetInPack(){ return inPack_; }
	inline IPack* GetOutPack(){ return outPack_; }
	inline IDBConnection* GetDBConnection(const char * szDataSourceName = NULL){ return dbService_->getConnection(szDataSourceName); }

private:
	IDBService* dbService_;
	IMessageService* msgService_;
	IPack* inPack_;
	IPack* outPack_;

};

}//namespace x



#endif //XBUSINESS_BUSINESS_CONTEXT_H_
