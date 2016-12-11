##########################################################################
##  makefile.gcc ͨ�ð����ļ�
##  -------------------------------------------------
##  
##��    Ҫ: 
##  Makefile for g++ 3.4.6~
##  
##  �ļ���: xbase.inc
##  ģ����: xbase.so
##  ģ��˵��: 
##   
##��    ��: willen
##����ʱ��: 2016-04-09
##��    ע: 
##
##�޸ļ�¼: 
##########################################################################

#(1)Ŀ���ļ�

OBJS=thread.$(TO) xml.$(TO)\


#(2)��������.cpp.o:,
#�����Ŀ���ļ�
all : cleanobj  $(EXES)
#
thread.$(TO): thread.cpp
	$(CC) $(CFLAGS) thread.cpp $(INCLUDE_DIR)
xml.$(TO): xml.cpp
	$(CC) $(CFLAGS) xml.cpp $(INCLUDE_DIR)
#
	
$(EXES): $(OBJS)
	$(LD) $(LFLAGS) -o$(OUTPUT_DIR)/$(LIB_PRE)$(EXES)$(LIB_EXT) $(OBJS) $(STDLIBS) $(DEFLIBS) $(EXPS)

#(3)��������
cleanobj:
	$(RM) *.$(TO)
clean:cleanobj
	@echo clean all