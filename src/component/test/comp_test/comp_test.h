#ifndef COMP_TEST_H_
#define COMP_TEST_H_

#include <include/xbusiness_intf.h>


int F1000000(x::IBusinessContext* context, x::IPack* in, x::IPack* out);


#ifdef __cplusplus
extern "C" 
{
#endif

int FUNC_CALL Init();
int FUNC_CALL UnInit();
const x::BusinessFuncEntry* FUNC_CALL GetFuncEntry();

#ifdef __cplusplus
}
#endif

#endif //COMP_TEST_H_
