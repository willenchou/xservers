#ifndef XBASE_BINARY_BUFF_H_
#define XBASE_BINARY_BUFF_H_

#include <new>
#include "os_common.h"
#include "xbase_export.h"

namespace x {

class XBASE_EXPORT BinaryBuff {
public:
	inline BinaryBuff(uint32_t capacity = 1024) 
		:buff_(NULL)
		,capacity_(0)
		,len_(0)
	{
		if (capacity > 0) {
			buff_ = new (std::nothrow) char[capacity];
			if (buff_) capacity_ = capacity;
		}
	}
	
	inline ~BinaryBuff() {
		if (buff_) delete [] buff_;
	}

	inline const void* Data() {
		return buff_;
	}

	inline uint32_t Len() {
		return len_;
	}

	inline void Clear() { 
		len_ = 0; 
	}

	inline bool Set(const void* data, uint32_t len) {
		Clear();

		if (data && len > 0) {
			if (!CheckRoom(len)) return false;
			
			memcpy(buff_, data, len);
			len_ = len;
		}

		return true;
	}

	inline bool Append(const void* value, uint32_t len) {
		if (!value || len < 1) return false;
		if (!CheckRoom(len)) return false;
		memcpy(buff_ + len_, value, len);
		len_ += len;
		return true;
	}

	inline bool Append(uint8_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(uint16_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(uint32_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(int8_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(int16_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(int32_t value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(double value) {
		return Append((void*)&value, sizeof(value));
	}

	inline bool Append(const char* value, int len = -1) {
		return Append((void*)value, len <= 0 ? strlen(value) + 1 : len);
	}

private:
	bool CheckRoom(uint32_t len) {
		if (len_ + len > capacity_) {
			uint32_t newCapacity = capacity_ * 2;
			if (newCapacity < len_ + len) 
				newCapacity = (len_ + len) * 2;

			char* newBuff = new (std::nothrow) char[newCapacity];
			if (!newBuff) return false;
			
			if (buff_) {
				if (len_ > 0)
					memcpy(newBuff, buff_, len_);
				delete[] buff_;
			}
			
			buff_ = newBuff;
			capacity_ = newCapacity;
		}

		return true;
	}

	char* buff_;
	uint32_t capacity_;
	uint32_t len_;
};

}//namespace x

#endif //XBASE_BINARY_BUFF_H_
