#ifndef _INC_GRAVITY_CALCFUNC
#define _INC_GRAVITY_CALCFUNC

#include "gGravObject.h"
#include "gGravStep.h"

long double* calcForce(GravObject* mpmain, GravStep* vmpsinsert, long double mdvforcetotal[3]);
GravStep* calcAcc(GravStep* vmpsinsert, GravStep* vmpsout);
GravObject* collision(GravObject* mpsurvive, GravObject* mpkill);
GravStep* collisionCheck(GravStep* pgs_insert);
bool checkSpeedBorder(GravStep* pgs_test, long double dpercentage);
int CalcCode(std::string filename, GravStep* pgs_start, long double dtime_max, long double dtime_save, long double dtime_step_default);
bool savepercentage(std::string file, int percent);

#endif
