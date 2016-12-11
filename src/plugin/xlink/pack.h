#ifndef XMESSAGE_PACK_H_
#define XMESSAGE_PACK_H_

#include <xbase/os_common.h>
#include <map>
using namespace std;

#ifdef OS_WIN
	#include <hash_map>
	using namespace stdext;
#else
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif
#include <string>
#include <vector>
#include <include/xmessage_intf.h>
#include <xbase/ref_counted.h>
#include <xbase/binary_buff.h>

namespace x{

class PackItem;
class Pack : public RefCountedThreadSafe<Pack, IPack> {
public:
	Pack();
	virtual ~Pack();

	virtual bool SetBuff(void* buff, uint32_t len);
	virtual bool GetBuff(void** buff, uint32_t* len);
	virtual void Clear();
	virtual void BeginPack();
	virtual void EndPack();
	virtual void AddCol(const char* name, int8_t type = PACK_COL_TYPE_STR);
	virtual uint32_t GetColCount();
	virtual const char* GetColName(uint32_t col);
	virtual int8_t GetColType(uint32_t col);
	virtual void AddRow();
	virtual uint32_t GetRowCount();
	virtual void AddValue(const char* name, uint8_t value);
	virtual void AddValue(const char* name, uint16_t value);
	virtual void AddValue(const char* name, uint32_t value);
	virtual void AddValue(const char* name, int8_t value);
	virtual void AddValue(const char* name, int16_t value);
	virtual void AddValue(const char* name, int32_t value);
	virtual void AddValue(const char* name, double value);
	virtual void AddValue(const char* name, char* value);
	virtual void AddValue(const char* name, void* value, uint32_t len);
	virtual uint8_t GetValueUint8(const char* name, uint32_t row = 0);
	virtual uint16_t GetValueUint16(const char* name, uint32_t row = 0);
	virtual uint32_t GetValueUint32(const char* name, uint32_t row = 0);
	virtual int8_t GetValueInt8(const char* name, uint32_t row = 0);
	virtual int16_t GetValueInt16(const char* name, uint32_t row = 0);
	virtual int32_t GetValueInt32(const char* name, uint32_t row = 0);
	virtual double GetValueDouble(const char* name, uint32_t row = 0);
	virtual char* GetValueStr(const char* name, uint32_t row = 0);
	virtual void* GetValueRaw(uint32_t* len, const char* name, uint32_t row = 0);
	virtual uint8_t GetValueUint8(uint32_t col, uint32_t row);
	virtual uint16_t GetValueUint16(uint32_t col, uint32_t row);
	virtual uint32_t GetValueUint32(uint32_t col, uint32_t row);
	virtual int8_t GetValueInt8(uint32_t col, uint32_t row);
	virtual int16_t GetValueInt16(uint32_t col, uint32_t row);
	virtual int32_t GetValueInt32(uint32_t col, uint32_t row);
	virtual double GetValueDouble(uint32_t col, uint32_t row);
	virtual char* GetValueStr(uint32_t col, uint32_t row);
	virtual void* GetValueRaw(uint32_t* len, uint32_t col, uint32_t row);

protected:
	PackItem* GetItem(const char* name);
	PackItem* GetItem(uint32_t col);
	
	bool BuildBuff();
	bool BuildItem();

private:
	typedef map<std::string, PackItem*> PackItemMap;
	typedef std::vector<PackItem*> PackItemVector;
	
	PackItemVector items_;
	PackItemMap itemIndex_;
	
	BinaryBuff* buff_;
};


class PackItem {
public:
	inline PackItem(const char* name, uint8_t type)
	:name_(NULL)
	,type_(type)
	,items_(NULL)
	,itemsCap_(0)
	,itemsLen_(0){
		if(name)
			name_ = strdup(name);
	}

	inline ~PackItem(){
		if(name_) 
			free(name_);

		for (uint32_t i = 0; i < itemsCap_; i++) {
			delete items_[i];	
		}
		delete items_;
	}
	
	inline const char* Name() { return name_; }
	inline uint8_t const Type() { return type_;}
	inline uint32_t const Count() { return itemsLen_;}
	inline bool GetData(uint32_t index, void** data, uint32_t* len){
		if(index < itemsLen_){
			if(data) *data = items_[index]->buff;
			if(len) *len = items_[index]->length;
			return true;
		}
		return false;
	}
	inline bool AddData(void* data, uint32_t len){
		if(itemsLen_ >= itemsCap_){
			uint32_t newCapacity = itemsCap_ > 0 ? itemsCap_ * 2 : 1;
			Item** newItems = new Item*[newCapacity];
			for (uint32_t i = 0; i < newCapacity; i++) {
				if ( i < itemsCap_ ) {
					newItems[i] = items_[i];
				} else {
					newItems[i] = new Item();
				}
			}
			
			delete items_;
			items_ = newItems;
			itemsCap_ = newCapacity;
		}

		items_[itemsLen_]->capacity = len;
		items_[itemsLen_]->length = len;
		items_[itemsLen_]->buff = new char[len];
		memcpy(items_[itemsLen_]->buff, data, len);
		itemsLen_++;

		return true;
	}

private:
	struct Item{
		char*		buff;
		uint32_t	capacity;
		uint32_t	length;
	};

	uint8_t type_;
	char* name_;
	Item** items_;
	uint32_t itemsCap_;
	uint32_t itemsLen_;
};

}//namespace x

#endif //XMESSAGE_PACK_H_
