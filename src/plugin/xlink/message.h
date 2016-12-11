#ifndef XMESSAGE_MESSAGE_H_
#define XMESSAGE_MESSAGE_H_

#include <include/xmessage_intf.h>
#include <xbase/ref_counted.h>
#include <xbase/binary_buff.h>

namespace x{

#define INIT_TAG_BUFF_LEN 1024
#define INIT_MSG_BUFF_LEN 1024
#define MSG_TAG_MAX_COUNT 1024
#define MSG_FLAG 1024

class MessageTag;
class Message : public RefCountedThreadSafe<Message, IMessage> {
public:
	struct Head {
		int32_t		flag;
		uint32_t	tagCount;
		uint32_t	len;
	};

	Message();
	virtual ~Message();

	virtual bool SetBuff(void* data, uint32_t len);
	virtual bool GetBuff(void** data, uint32_t* len);
	
	virtual bool SetData(uint32_t tagID, void* data, uint32_t len, int32_t type=-1);
	virtual bool AddData(uint32_t tagID, void* data, uint32_t len, int32_t type=-1);
	virtual uint32_t GetDataCount(uint32_t tagID);
	virtual bool GetData(uint32_t tagID, void** data, uint32_t* len, int32_t* type = NULL, uint32_t index = 0);
	virtual bool DataExist(uint32_t tagID);
	virtual void RemoveData(uint32_t tagID);
	virtual void Clear();
	virtual void ChangeReq2Ans();
	virtual void SetSysNo(uint32_t sysNo);
	virtual bool GetSysNo(uint32_t* sysNo);
	virtual void SetSubSysNo(uint32_t subSysNo);
	virtual bool GetSubSysNo(uint32_t* subSysNo);
	virtual void SetFuncID(uint32_t funcID);
	virtual bool GetFuncID(uint32_t* funcID);
	virtual void SetFuncType(uint32_t funcType);
	virtual bool GetFuncType(uint32_t* funcType);
	virtual void SetSendID(uint32_t sendID);
	virtual bool GetSendID(uint32_t* sendID);
	virtual void SetReturnID(int32_t returnID);
	virtual bool GetReturnID(int32_t* returnID);	

private:
	void Resize(uint32_t newSize);
	void BuildTags();
	void BuildBuff();
	
	MsgFuncInfo* GetTagFuncInfo(bool insert = true);

	MessageTag* tags_[MSG_TAG_MAX_COUNT];
	
	char* buff_;
	uint32_t buffCap_;
	uint32_t buffLen_;
};

class MessageTag {
public:
	struct Head {
		int32_t		ID;
		uint32_t	count;
		uint32_t	len;
	};

	struct ItemHead {
		int32_t		type;
		uint32_t	len;
	};

	MessageTag(int32_t ID);
	~MessageTag();

	bool SetBuff(void* data, uint32_t len);
	void* GetBuff(uint32_t& len);

	void AddData(void* data, uint32_t dataLen, int32_t type = -1);
	uint32_t GetDataCount();
	bool GetData(void** data, uint32_t* len, int32_t* type = NULL, uint32_t index = 0);
	void Clear();

	inline void SetTagID(int32_t ID) { GetHead()->ID = ID;}
	inline Head* GetHead(){ return (Head*)buff_; }

private:
	void Resize(uint32_t newSize);
	uint32_t GetItemPos(uint32_t index);

	char* buff_;
	uint32_t buffLen_;
};

}//namesapce x

#endif //XMESSAGE_MESSAGE_H_
