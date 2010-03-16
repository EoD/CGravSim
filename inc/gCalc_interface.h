#ifndef _INC_GRAVITY_CALCFUNC_IF
#define _INC_GRAVITY_CALCFUNC_IF

#include "gGravStep.h"
namespace calc {
	int master(std::string filename, GravStep* pgs_start, long double dtime_max, long double dtime_save, long double dtime_step_default);
};

#endif
