#ifndef XTEST_XMESSAGE_TEST_H_
#define XTEST_XMESSAGE_TEST_H_

#include "test_helper.h"
#include <include/xmessage_intf.h>

namespace xmessage_test {
	bool Test(){
		x::IMessageService* service = (x::IMessageService*)GetService(PLUGIN_MESSAGE, SID_MESSAGE);
		if(!service){
			return false;
		}

		x::IPack* pack = service->NewPack();
		if(!pack)
			return false;

		x::IPack* pack1 = service->NewPack();
		if(!pack1)
			return false;

		int rowCol = 8;
		for(int i = 0; i < rowCol; i++){
			char colName[32];
			sprintf(colName, "col%d", i);
			pack->AddCol(colName, i);
		}
		
		int rowCount = 10;
		for(int i = 0; i < rowCount; i++){
			char str[32];
			sprintf(str, "%d", i);

			pack->AddValue("col0", (uint8_t)i);
			pack->AddValue("col1", (uint16_t)i);
			pack->AddValue("col2", (uint32_t)i);
			pack->AddValue("col3", (int8_t)i);
			pack->AddValue("col4", (int16_t)i);
			pack->AddValue("col5", (int32_t)i);
			pack->AddValue("col6", (double)i);
			pack->AddValue("col7", str);
		}

		if(rowCol != pack->GetColCount())
			return false;
		for(int i = 0; i < rowCol; i++){
			char expectColName[32];
			sprintf(expectColName, "col%d", i);

			const char* colName = pack->GetColName(i);
			if(strcmp(colName, expectColName) != 0)
				return false;

			if(pack->GetColType(i) != i)
				return false;
		}

		if(rowCount != pack->GetRowCount())
			return false;
		for(int i = 0; i < rowCount; i++){
			uint8_t v_uint8 = pack->GetValueUint8("col0", i);
			if(v_uint8 != (uint8_t)i) return false;
			uint16_t v_uint16 = pack->GetValueUint16("col1", i);
			if(v_uint16 != (uint16_t)i) return false;
			uint32_t v_uint32 = pack->GetValueUint32("col2", i);
			if(v_uint32 != (uint32_t)i) return false;
			int8_t v_int8 = pack->GetValueInt8("col3", i);
			if(v_int8 != (int8_t)i) return false;
			int16_t v_int16 = pack->GetValueInt16("col4", i);
			if(v_int16 != (int16_t)i) return false;
			int32_t v_int32 = pack->GetValueInt32("col5", i);
			if(v_int32 != (int32_t)i) return false;
			double v_double = pack->GetValueDouble("col6", i);
			if(v_double - (double)i > 0.0000001) return false;
			char* v_str = pack->GetValueStr("col7", i);
			char v_strExpect[32];
			sprintf(v_strExpect, "%d", i);
			if(strcmp(v_str, v_strExpect) != 0) return false;
		}


		uint32_t packDataLen = 0;
		void* packData = NULL;
		if(!pack->GetBuff(&packData, &packDataLen))
			return false;

		if(!pack1->SetBuff(packData, packDataLen))
			return false;

		if(rowCol != pack1->GetColCount())
			return false;
		for(int i = 0; i < rowCol; i++){
			char expectColName[32];
			sprintf(expectColName, "col%d", i);

			const char* colName = pack1->GetColName(i);
			if(strcmp(colName, expectColName) != 0)
				return false;

			if(pack->GetColType(i) != i)
				return false;
		}

		if(rowCount != pack1->GetRowCount())
			return false;
		for(int i = 0; i < rowCount; i++){
			uint8_t v_uint8 = pack1->GetValueUint8("col0", i);
			if(v_uint8 != (uint8_t)i) return false;
			uint16_t v_uint16 = pack1->GetValueUint16("col1", i);
			if(v_uint16 != (uint16_t)i) return false;
			uint32_t v_uint32 = pack1->GetValueUint32("col2", i);
			if(v_uint32 != (uint32_t)i) return false;
			int8_t v_int8 = pack1->GetValueInt8("col3", i);
			if(v_int8 != (int8_t)i) return false;
			int16_t v_int16 = pack1->GetValueInt16("col4", i);
			if(v_int16 != (int16_t)i) return false;
			int32_t v_int32 = pack1->GetValueInt32("col5", i);
			if(v_int32 != (int32_t)i) return false;
			double v_double = pack1->GetValueDouble("col6", i);
			if(v_double - (double)i > 0.0000001) return false;
			char* v_str = pack1->GetValueStr("col7", i);
			char v_strExpect[32];
			sprintf(v_strExpect, "%d", i);
			if(strcmp(v_str, v_strExpect) != 0) return false;
		}


		x::IMessage* msg = service->NewMessage();
		if(!msg)
			return false;
		x::IMessage* msg1 = service->NewMessage();
		if(!msg1)
			return false;

		int tagDataCount = 2;
		for(int i = 0; i < 1024; i++){
			for(int j = 0; j < tagDataCount; j++){
				msg->AddData(i, packData, packDataLen);
			}
		}

		for(int i = 0; i < 1024; i++){
			if(msg->GetDataCount(i) != tagDataCount)
				return false;
			
			void* tagData = NULL;
			uint32_t tagpackDataLen = 0;
			for(int j = 0; j < tagDataCount; j++){
				msg->GetData(i, &tagData, &tagpackDataLen, NULL, j);
				if(!tagData || tagpackDataLen == 0)
					return false;
				if(memcmp(packData, tagData, packDataLen) != 0)
					return false;
			}
		}


		uint32_t msgDataLen = 0;
		void* msgData = NULL;
		if(!msg->GetBuff(&msgData, &msgDataLen))
			return false;

		if(!msg1->SetBuff(msgData, msgDataLen))
			return false;

		
		for(int i = 0; i < 1024; i++){
			if(msg1->GetDataCount(i) != tagDataCount)
				return false;
			
			void* tagData = NULL;
			uint32_t tagpackDataLen = 0;
			for(int j = 0; j < tagDataCount; j++){
				msg1->GetData(i, &tagData, &tagpackDataLen, NULL, j);
				if(!tagData || tagpackDataLen == 0)
					return false;
				if(memcmp(packData, tagData, packDataLen) != 0)
					return false;
			}
		}

		return true;
	}
};

#endif //XTEST_XMESSAGE_TEST_H_
