//xtest makefile gxx
//g++ -o ../bin/xtest testclientcallback.cpp testservercallback.cpp main.cpp -I../ -lrt -ldl -L../bin -lxlink -lxbase
#include "link.h"
//gtest makefile gxx
//1/
//g++ -c -fPIC -O2 gxxtest.cpp -I../
//g++ -o ../bin/gtest gxxtest.o -L../bin -lglink -Wl,-rpath-link=../bin
//2.
//g++ -o ../bin/gtest gxxtest.cpp -I../ -L../bin -lglink -lgbase
//3.
//g++ -o ../bin/gtest gxxtest.cpp -I../ -L../bin -lglink -Wl,-rpath-link=../bin

int main(int argc, char** argv){
	gxx::SleepL();
	return 0;
}