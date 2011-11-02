#ifndef _INC_GRAVITY_CALCFUNC
#define _INC_GRAVITY_CALCFUNC

#include "gGravObject.h"
#include "gGravStep.h"
#include "gMathVec.h"
#include <bitset>

namespace calc {
	namespace cerror {
		using namespace std;
		const bitset<ERROR_CALC_MAX> unknown	( ERROR_TO_BITSET(ERROR_CALC_UNKNOWN) );
		const bitset<ERROR_CALC_MAX> data	( ERROR_TO_BITSET(ERROR_CALC_DATA) );
		const bitset<ERROR_CALC_MAX> lightspeed	( ERROR_TO_BITSET(ERROR_CALC_LIGHTSPEED) );
		const bitset<ERROR_CALC_MAX> limit_long	( ERROR_TO_BITSET(ERROR_CALC_LIMIT_LONG) );
		const bitset<ERROR_CALC_MAX> limit_dbl	( ERROR_TO_BITSET(ERROR_CALC_LIMIT_DOUBLE) );
		const bitset<ERROR_CALC_MAX> nan	( ERROR_TO_BITSET(ERROR_CALC_NAN) );
		const bitset<ERROR_CALC_MAX> grt	( ERROR_TO_BITSET(ERROR_CALC_GRT) );
	}

	extern std::bitset<ERROR_CALC_MAX> cerrors;
	extern bool flagcalc;
	extern long double dtime_step;
	extern bool flag_collision;
	
	int calcForce(GravStep* vmpsinsert, std::vector<mdv>& mdv_forces);
	GravStep* calcAcc(GravStep* vmpsinsert, GravStep* vmpsout);
	GravObject* collision(GravObject* mpsurvive, GravObject* mpkill);
	GravStep* collisionCheck(GravStep* pgs_insert);
	bool checkSpeedBorder(GravStep* pgs_test, long double dpercentage);
	std::bitset<ERROR_CALC_MAX> master(std::string filename, GravStep* pgs_start, long double dtime_max, long double dtime_save, long double dtime_step_default);
	bool savepercentage(std::string file, int percent);
}

#endif
