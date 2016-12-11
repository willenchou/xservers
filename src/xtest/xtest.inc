##########################################################################
##  makefile.gcc ͨ�ð����ļ�
##  -------------------------------------------------
##  
##��    Ҫ: 
##  Makefile for g++ 3.4.6~
##  
##  �ļ���: xtest.inc
##  ģ����: xtest.so
##  ģ��˵��: 
##   
##��    ��: willen
##����ʱ��: 2016-04-09
##��    ע: 
##
##�޸ļ�¼: 
##########################################################################

#(1)Ŀ���ļ�

OBJS= main.$(TO) testclientcallback.$(TO) testservercallback.$(TO)\

#(2)��������.cpp.o:,
#�����Ŀ���ļ�
all : cleanobj  $(EXES)
#
main.$(TO): main.cpp
	$(CC) $(CFLAGS) main.cpp $(INCLUDE_DIR)
testclientcallback.$(TO): testclientcallback.cpp
	$(CC) $(CFLAGS) testclientcallback.cpp $(INCLUDE_DIR)
testservercallback.$(TO): testservercallback.cpp
	$(CC) $(CFLAGS) testservercallback.cpp $(INCLUDE_DIR)

$(EXES): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(OUTPUT_DIR)/$(LIB_PRE)$(EXES)$(LIB_EXT) $(DEFLIBS)$(STDLIBS)$(EXPS) 

#(3)��������
cleanobj:
	$(RM) *.$(TO)
clean:cleanobj
	@echo clean all