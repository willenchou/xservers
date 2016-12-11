#include "comp_test.h"

int F1000000(x::IBusinessContext* context, x::IPack* in, x::IPack* out){
	out->Clear();
	
	out->AddCol("col1");
	out->AddCol("col2");
	out->AddValue("col1", "col1_value");
	out->AddValue("col2", "col2_value");

	return 0;
}

int FUNC_CALL Init(){
	return 0;
}

int FUNC_CALL UnInit(){
	return 0;
}

const x::BusinessFuncEntry* FUNC_CALL GetFuncEntry(){
	static x::BusinessFuncEntry entry[] = {
		{1000000, F1000000},
		{0, NULL}
	};
	return entry;
}