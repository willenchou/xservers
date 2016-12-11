///数据库驱动组件接口定义(数据库驱动接口规范)
/*******************************************************
  源程序名称:DefDriver.h
  软件著作权:杭州恒生电子股份有限公司
  系统名称:  数据库统一访问-数据库驱动 
  模块名称:  数据库驱动组件接口定义
  功能说明:  开发各类数据库的驱动，必须实现的接口函数 
  相关文档:  概要设计接口部分
  作    者:  章乐焱
  开发日期:  20040509
  备    注:  
    修改人员：dongpf
	修改时间：2009-09-21
	修改说明：增加Oracle rac切换模式定义
    20140915 huxb 支持事务
    20141215 huxb 增加将结果集缓存到本地相关接口
	20150203 huxb 增加发送心跳和获取最后执行的sql的接口
*********************************************************/     
#ifndef DefDriver_H
#define DefDriver_H


#ifdef PLATFORM_UNIX
	#define DLL_EXPORT
#else
	#define DLL_EXPORT  __declspec(dllexport)
#endif

///驱动返回错误函数
typedef void (*fpHasError)(void * errOwner,int HSErrint,int err ,const char * errinfo);
///驱动返回信息函数
typedef void (*fpHasMessage)(void * msgOwner,int HSMsgno,int msgno,const char * msgtext);

//--------常量定义-------------
#define  HSDB_NO                 1         ///否定,没有
#define  HSDB_YES                0         ///肯定,有,存在 
#define  HSDB_SUCESS             0         //处理成功
#define  HSDB_FAILURE            1         //处理失败
#define  HSDB_TRUE               1         //true
#define  HSDB_FALSE              0         //false
#define  HSDB_WASNULL            1000      ///记录中有NULL数据
#define  HSDB_SELECT             1001      ///表示是执行select
#define  HSDB_PROCESS            1002      ///表示执行的是存储过程
#define  HSDB_PARAM_INPUT         1003      /// 参数为输入参数
#define  HSDB_PARAM_OUTPUT       1004      ///参数为输出参数   
#define  HSDB_TEMPDATASOURCE     1005      ///临时数据源
#define  HSDB_NOTTEMPDATASOURCE  1006      ///非临时数据源
#define  HSDB_CONNECT_DISCONN    1007      //驱动返回连接断开
#define  HSDB_CONNECT_OK         1009      //驱动返回连接正常
#define  HSDB_NO_MORE_ROWS       1008      //已经没有记录了
#define  HSDB_END_PARAM          -1        //过程参数结束标志
#define  HSDB_DRIVER_DATATYPE_STRING    'S'         //参数类型为string C标准的字符串 
#define  HSDB_DRIVER_DATATYPE_CHAR      'C'         //参数类型为char
#define  HSDB_DRIVER_DATATYPE_LONG      'I'        //参数类型为long
#define  HSDB_DRIVER_DATATYPE_DOUBLE    'D'         //参数类型为double
#define  HSQL_DRIVER_DATATYPE_BINARY    'R'         //任意二进制数据
#define  HSDB_APPLICATION_NAME   "FINBASE_AS"//应用名
#define  HSDB_ERRINFO_LENGTH     200         //错误信息长度

//2009-09-21 dongpf 增加Oracle rac切换模式定义
#define  HSDB_SERVERTYPE_NONE     0         //无rac切换模式
#define  HSDB_SERVERTYPE_SELECT   1         //select模式
#define  HSDB_SERVERTYPE_SESSION  2         //session模式
//-------------------------------

///数据库连接句柄定义(驱动开发者应先行定义HSQLHANDLE句柄)
#ifndef HSQLHANDLE
	#define HSQLHANDLE void *
#endif

///数据库语句句柄定义(驱动开发者应先行定义HSQLHANDLE句柄)
#ifndef STMT_HANDLE
	#define STMT_HANDLE void *
#endif

/**@name 驱动库接口函数指针定义*/
//@{
    typedef void (* fpHSQLSetUpErrorHandle)(fpHasError  errhandle);
    typedef  void (* fpHSQLSetUpMsgHandle)(fpHasMessage  msghandle);
	typedef void (*fpHSQLFreeMemory)(void * param);
	typedef int (*fpHSQLGetInfo)(char ** dbType);
	typedef int (*fpHSQLInit)(void ** context);

	typedef HSQLHANDLE (*fpHSQLInitHandle)(void * context,void * owner);
	typedef void (*fpHSQLFreeHandle)(HSQLHANDLE hp);
	typedef int (*fpHSQLConnect)( HSQLHANDLE hp,char * username,char *password,char * server,char * database);
	typedef int (*fpHSQLDisconnect)( HSQLHANDLE hp);
    typedef STMT_HANDLE (*fpHSQLAllocStmt)(HSQLHANDLE hp);
    typedef void (*fpHSQLFreeStmt)(HSQLHANDLE hp,STMT_HANDLE pStmt);
    typedef int (* fpHSQLPrepare)(STMT_HANDLE pStmt,const char * StatementText, int TextLen);
    typedef int (* fpHSQLExecute)(STMT_HANDLE pStmt);
    typedef int (* fpHSQLOpen)(STMT_HANDLE pStmt);
    typedef int (* fpHSQLResult)(STMT_HANDLE pStmt,int SelecOrProce);

    typedef int (* fpHSQLNumResultCols)(STMT_HANDLE pStmt,int SelecOrProce,int * ColumnCountPtr);
    typedef int (* fpHSQLColName)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol,char  namebuffer[]);
    typedef char (* fpHSQLColType)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
    typedef int (* fpHSQLColLen)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
    typedef int ( * fpHSQLColPrecision)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
    typedef int ( * fpHSQLColScale)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
    typedef int (* fpHSQLColIsNull)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
    typedef int (* fpHSQLDefineCol)(STMT_HANDLE pStmt,int SelecOrProce,int indexCol,void * buff,int len);
    typedef int (* fpHSQLNext)(STMT_HANDLE pStmt,int SelecOrProce);
	typedef int (*fpHSQLRpcPrepare)(STMT_HANDLE pStmt ,int YorNForResult, char * sql);
	typedef int (*fpHSQLExecuteStoreProcess)(STMT_HANDLE hp ,int YorNForResult,int iRetExits, char * p_storeName, int *type,int * inOroutType,void  ** p_ProParamUnion);
	typedef int (*fpHSQLDbretdata)(STMT_HANDLE pStmt,int iRetExist, int *type,int * inOroutType,void  ** p_ProParamUnion);

    typedef int ( * fpHSQLCancels)(STMT_HANDLE hp);
    typedef int (* fpHSQLCount)(STMT_HANDLE pStmt);

	typedef int (* fpHSQLGetServerType)(const char * ServerName);

	typedef void * (* fpHSQLGetSQLContext)(HSQLHANDLE hp);

	typedef int (* fpHSQLBindByPos)(STMT_HANDLE pStmt,int iIndex,void * pData,int iDataLen,char cDataType);

    //huxb 20140915 事务支持
    typedef int (* fpHSQLSetAutoCommit)(HSQLHANDLE hp, bool bAutoCommit);
    typedef int (* fpHSQLCommit)(HSQLHANDLE hp);
    typedef int (* fpHSQLRollback)(HSQLHANDLE hp);
    //huxb 20141215 缓存结果集到本地
    typedef int (* fpHSQLOpenAll)(STMT_HANDLE pStmt, int * lpCount);
    typedef int (* fpHSQLExecuteStoreProcess2)(STMT_HANDLE hp ,int YorNForResult,int iRetExits, char * p_storeName, int *type,int * inOroutType,void  ** p_ProParamUnion, int * lpCount);
	//huxb 20140203 发送心跳
	typedef int (* fpHSQLHeartbeat)(STMT_HANDLE hp);
	//获取最后执行的SQL
	typedef const char * (* fpHSQLGetLastSQL)(STMT_HANDLE hp);
//@}


#ifdef __cplusplus
extern "C" { 
#endif

///20150203 huxb
///获取最后执行的SQL
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@return int 0成功，非0失败
*/ 
DLL_EXPORT const char * HSQLGetLastSQL(HSQLHANDLE hp); 

///发送心跳
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@return int 0成功，非0失败
*/ 
DLL_EXPORT int HSQLHeartbeat(HSQLHANDLE hp); 

///20140915 huxb
///设置自动递交（默认自动递交）
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@param bool bAutoCommit  是否自动递交
*@return int 成功与否，0成功，非0失败
*/ 
DLL_EXPORT int HSQLSetAutoCommit( HSQLHANDLE hp, bool bAutoCommit = true); 

///提交事务
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@return int 成功与否，0成功，非0失败
*/ 
DLL_EXPORT int HSQLCommit( HSQLHANDLE hp);

///事务回滚
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@return int 成功与否，0成功，非0失败
*/ 
DLL_EXPORT int HSQLRollback( HSQLHANDLE hp);

///20141215 huxb
///执行select语句，并返回结果集行数
/**
*@param HSQLHANDLE hp  数据库连接句柄
*@param int * lpCount 返回结果集行数
*@return int 成功与否，0成功，非0失败
*/ 
DLL_EXPORT int HSQLOpenAll( STMT_HANDLE pStmt, int * lpCount);

/**执行存储过程(如果有结果集，则会全部缓存到本地，并返回结果集行数)
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int YorNForResult  表示是有无结果集:HSQL_NO,HSQL_YES
 *@param char * p_storeName 存储过程名 
 *@param int * type     参数类型 整数数组地址
 *@param int * inOroutType 参数的输入输出标识
 *@param void ** p_ProParamUnion 参数联合体数组地址
 *@param int * lpCount 返回结果集行数
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLExecuteStoreProcess2(STMT_HANDLE pStmt ,int YorNForResult,int iRetExist, char * p_storeName,int *type,int * inOroutType,void ** p_ProParamUnion, int * lpCount);

///设置错误处理回调函数
DLL_EXPORT void HSQLSetUpErrorHandle(fpHasError errhandle);
///设置消息处理回调函数
DLL_EXPORT void HSQLSetUpMsgHandle(fpHasMessage  msghandle);

///释放由驱动库申请的内存空间
/**当输出参数为字符串型时，字符串的空间是由驱动库申请的，由驱动库负责释放
 *@param void * p 由驱动库中申请的内存指针
 */
DLL_EXPORT void  HSQLFreeMemory(void * p);

/**取数据库驱动接口库信息
 * @param  char ** dbType  当前接口库对应的数据库类型串，调用者声明一个指针，此函数将修改指针指向
 * @return int             返回接口库版本号
 */
DLL_EXPORT int  HSQLGetInfo(char ** dbType);

///数据库接口库初始化,接口库加载后应首先调用此函数.
/**@param void ** context:驱动库上下文指针地址(sybase用到)
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLInit(void ** context);

///创建数据库连接句柄，为句柄分配对应的存储空间
/**
 *@param context 驱动库上下文指针
 *@param void * owner  驱动使用者指针，错语处理回调函数需要该指针作为回调参数(驱动库中应保存)
 *@return  HSQLHANDLE  数据库连接句柄,返回NULL表示创建失败
 */
DLL_EXPORT HSQLHANDLE HSQLInitHandle(void * lpContext,void * owner);

///释放数据库连接句柄
/**
 *@param HSQLHANDLE p_db_handle  连接句柄
 */
DLL_EXPORT void HSQLFreeHandle(HSQLHANDLE p_db_handle);

///连接数据库	
/**使用给定参数，连接数据库服务器
 *@param HSQLHANDLE hp  数据库连接句柄
 *@param  char * username  数据库用户名
 *@param  char * password  口令
 *@param  char * server    数据库客户端配置的数据库别名
 *@param  char * database  当前连接缺省数据库名(在sql server,sybase时起作用)
 *@return int   HSQLSUCCEED 连接  HSQLFAIL失败
 */
DLL_EXPORT int HSQLConnect( HSQLHANDLE hp,char * username,char *password,char * server,char * database); 

///断开数据库连接
/**
 *@param HSQLHANDLE hp  数据库连接句柄
 *@return int   HSQLSUCCEED 连接  HSQLFAIL失败
 */ 
DLL_EXPORT int HSQLDisconnect( HSQLHANDLE hp); 

/**创建语句句柄 ，在oracle与sql server中语句句柄指向的数据结构里包含连接句柄。
 *@param    HSQLHANDLE hp  数据库连接句柄
 *@return   STMT_HANDLE 语句句柄
 */
DLL_EXPORT STMT_HANDLE HSQLAllocStmt (HSQLHANDLE hp);

/**释放语句句柄
 *@param   HSQLHANDLE hp  数据库连接句柄
 *@param   STMT_HANDLE 被释放语句句柄
 */
DLL_EXPORT void HSQLFreeStmt(HSQLHANDLE hp,STMT_HANDLE pStmt);

/**准备SQL语句
 *@param   STMT_HANDLE 语句句柄
 *@param   char * StatementText 语句字符串地址
 *@param   int TextLen  语句长度
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLPrepare(STMT_HANDLE pStmt,char * StatementText, int TextLen);

/**执行准备好的sql语句(无结果集返回的sql语句,即非select 语句)
 *@param   STMT_HANDLE 语句句柄
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLExecute (STMT_HANDLE pStmt);

/**执行准备好的select 语句
 *@param   STMT_HANDLE 语句句柄
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLOpen (STMT_HANDLE pStmt);

/** @name 结果集操作函数 */
//@{

/**打开结果集，取出第一条记录
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLResult(STMT_HANDLE pStmt,int SelecOrProce);

/**取结果集字段数
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int * ColumnCountPtr 返回字段数
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLNumResultCols(STMT_HANDLE pStmt,int SelecOrProce,int * ColumnCountPtr);

/**取结果集字段名
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
  *@param   char * namebuffer 字段名buffer  空间有外面创建
 *@return  char * 字段名
 */
DLL_EXPORT int HSQLColName(STMT_HANDLE pStmt,int SelecOrProce,int indexCol,char  namebuffer[]);

/**取结果集字段数据类型
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@return  char  数据类型 
 */
DLL_EXPORT char HSQLColType(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);

/**取结果集字段长度
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@return  int  返回字段最大长度,出错返回-1(下标越界);
 */
DLL_EXPORT int HSQLColLen(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);
	
/**取字段精度
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@return  int  下标越界或者失败返回 -1;
 */
DLL_EXPORT  int HSQLColPrecision(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);

/**取字段小数位数
 *@param   STMT_HANDLE pStmt 语句句柄
  *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@return  int  下标越界或者失败返回 -1;
 */
DLL_EXPORT int HSQLColScale(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);

/**取字段取值是否可以为NULL
 *@param   STMT_HANDLE pStmt 语句句柄
  *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@return  int  0 不可以 ,1 可以,下标越界或者失败返回 -1;
 */
DLL_EXPORT int HSQLColIsNull(STMT_HANDLE pStmt,int SelecOrProce,int indexCol);


/**绑定字段，为方便操作，都以字符串方式绑定
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@param   int  indexCol    字段序号(以1为基数)
 *@param   void * buff      输出字符串缓存区
 *@param   int len          输出字符串最大长度
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLDefineCol(STMT_HANDLE pStmt,int SelecOrProce,int indexCol,void * buff,int len);

/**取一条记录，字段值被输出到绑定的缓存区
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int SelecOrProce  表示是取select的结果集还是存储过程返回的结果集:HSDB_SELECT,HSDB_PROCESS
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLNext(STMT_HANDLE pStmt,int SelecOrProce);

//@}

/**调用存储过程的准备
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int YorNForResult  表示是有无结果集: :HSQL_NO,HSQL_YES
 *@param   char * sql   调用存储过程语句:对格式要求如下:
 *
 *有return的
 *
 *"? = call StoreProName(?,?,?,?)"  
 *
 *
 *说明: 第一个问号代表return值的占位符
 *
 *						      在小括号中的问号代表输入或输出参数占位符.数量要和
 *
 *							  过程真实参数个数相等.
 *
 *如果没有返回值或不关心返回值 ,这里的sql语句应为:
 *
 *						 " call StoreProName(?,?,?,?)"
 *
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLRpcPrepare(STMT_HANDLE pStmt,int YorNForResult,char * sql);

/**执行存储过程
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param   int YorNForResult  表示是有无结果集:HSQL_NO,HSQL_YES
 *@param char * p_storeName 存储过程名 
 *@param int * type     参数类型 整数数组地址
 *@param int * inOroutType 参数的输入输出标识
 *@param void ** p_ProParamUnion 参数联合体数组地址
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLExecuteStoreProcess(STMT_HANDLE pStmt ,int YorNForResult,int iRetExist, char * p_storeName,int *type,int * inOroutType,void ** p_ProParamUnion);

/**在有结果集的情况下获得存储过程return 值 和输出参数
 *@param   STMT_HANDLE pStmt 语句句柄
 *@param  int iRetExist 指明是否存在return值
 *@param int * type     参数类型 整数数组地址
 *@param int * inOroutType 参数的输入输出标识
 *@param void ** p_ProParamUnion 参数联合体数组地址
 *@return int   HSQLSUCCEED 成功  HSQLFAIL失败无数据
 */
DLL_EXPORT int HSQLDbretdata(STMT_HANDLE  s_handle,int iRetExist,int *type,int * inputtype,void ** p_ProParamUnion);

/**Cancels the current command batch and flushes any pending results.
 *@param   STMT_HANDLE pStmt 语句句柄
 *@return  int   HSQLSUCCEED 成功  HSQLFAIL失败
 */
DLL_EXPORT int HSQLCancels(STMT_HANDLE pStmt);
/**取当前语句影响的记录条数
 *@param   STMT_HANDLE pStmt 语句句柄
 *@return  当前语句影响的记录条数
 */
DLL_EXPORT int HSQLCount(STMT_HANDLE pStmt);

/**根据服务名取服务类型
 *@param   const char * ServerName 服务名
 *@return  服务类型，找不到返回-1
 */
DLL_EXPORT int HSQLGetServerType(const char * ServerName);

/**@name 以下为zly测试用接口 */
//@{
	///按位置绑定，由调用者准备空间
	DLL_EXPORT int HSQLBindByPos(STMT_HANDLE pStmt,int iIndex,void * pData,int iDataLen,char cDataType);	
	///执行存储过程
	DLL_EXPORT int HSQLExecRPC(STMT_HANDLE pStmt);

	///获取嵌入SQL所需连接上下文
	/**
	 *@param HSQLHANDLE p_db_handle  连接句柄
	 */
	DLL_EXPORT void *  HSQLGetSQLContext(HSQLHANDLE p_db_handle);
	
//@}
	
 

#ifdef __cplusplus
  }
#endif

#endif
