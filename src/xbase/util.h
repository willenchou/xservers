#ifndef X_XBASE_HELPER_H_
#define X_XBASE_HELPER_H_
#include "os_common.h"
#include <string.h>

namespace x {

class Util {
public:
	static void StrCopy(char* dest, const char* src){
		if(!dest) 
			return;
		
		if(!src){
			*dest = '\0';
			return;
		}

		for(; *dest++ = *src++; ){
		}
		
		return;
	}

	static void StrNCopy(char* dest, const char* src, size_t size){
		if(!dest) 
			return;
		
		if(!src){
			*dest = '\0';
			return;
		}

		for(size_t i = 0; i < size - 1; i++){
			*(dest + i) = *(src+i);	
			if(*(src+i) == '\0') break;
		}
		*(dest + size -1) = '\0';
	}

	static char* StrTrim(char* str) {
		char* startPos = str;
		while ((*startPos == ' ') || (*startPos == '\t')) {
			startPos++;
		}

		if (strlen(startPos) == 0)
			return "";

		char* endPos = str + strlen(str) - 1;
		while ((*endPos == ' ') || (*endPos == '\t')) {
			*endPos = 0;
			endPos--;
		}

		return startPos;
	}

	static uint64_t TimeTickCount() {
#ifdef OS_WIN
		LARGE_INTEGER liCounter; 
		LARGE_INTEGER liCurrent;
		if (!QueryPerformanceFrequency(&liCounter))
			return GetTickCount();
		QueryPerformanceCounter(&liCurrent);
			return (uint64_t)(liCurrent.QuadPart * 1000 / liCounter.QuadPart);
#else
		struct timeval tval;
		gettimeofday(&tval, NULL);
		return tval.tv_sec * 1000L + tval.tv_usec / 1000L;
#endif
	}
};



}//namespace x

#endif //X_XBASE_HELPER_H_
