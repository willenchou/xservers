##########################################################################
##  makefile.gcc 通用包含文件
##  -------------------------------------------------
##  
##概    要: 
##  Makefile for g++ 3.4.6~
##  
##  文件名: xbase.inc
##  模块名: xbase.so
##  模块说明: 
##   
##作    者: willen
##创建时间: 2016-04-09
##备    注: 
##
##修改记录: 
##########################################################################

#(1)目标文件

OBJS=thread.$(TO) xml.$(TO)\


#(2)编译依赖.cpp.o:,
#先清除目标文件
all : cleanobj  $(EXES)
#
thread.$(TO): thread.cpp
	$(CC) $(CFLAGS) thread.cpp $(INCLUDE_DIR)
xml.$(TO): xml.cpp
	$(CC) $(CFLAGS) xml.cpp $(INCLUDE_DIR)
#
	
$(EXES): $(OBJS)
	$(LD) $(LFLAGS) -o$(OUTPUT_DIR)/$(LIB_PRE)$(EXES)$(LIB_EXT) $(OBJS) $(STDLIBS) $(DEFLIBS) $(EXPS)

#(3)清理命令
cleanobj:
	$(RM) *.$(TO)
clean:cleanobj
	@echo clean all
