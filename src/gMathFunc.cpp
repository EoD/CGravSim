#include <cmath>

#include "gDefines.h"
#include "gDebugFunc.h"
#include "gMathFunc.h"

long long sqrtx(long long x) {
	int curLog = 32;
	long long res = 0;
	while (curLog > 0) {
		long long nextNum = x - (res << curLog);
		curLog--;
		if (nextNum > 0) {
			long long resAdd = (1L << curLog);
			nextNum -= (resAdd << curLog);
			if (nextNum > 0) {
				x = nextNum;
				res |= resAdd;
			} else if (nextNum == 0)
				return res | resAdd;
		}
	}
	return res;
}

long double sqrtx(long double x) {
	//long double temp = x / 2;
	//const long double Tolerance = 1.0e-7;

	//do temp = (temp + x / temp) / 2;
	//while( abs(temp * temp - x) > Tolerance);

	//return temp;
#ifdef WIN32
	return sqrtl(x);
#else
	return (long double)sqrt((long double)x);
#endif
}

long double powx(long double x, int y) {
	long double dtemp = 1.;
	for(int i=0; i<y; i++) {
		dtemp *= x;
	}
	return dtemp;
}

long double gamma(long double v) {
	if (v > LIGHTSPEED)
		return 0.0;

	debugout("Gamma() - v=",v,5);

	long double dsqrt = v/(long double)LIGHTSPEED;
	dsqrt = dsqrt * dsqrt;
	dsqrt = 1.0 - dsqrt;
	dsqrt = sqrtx(dsqrt);
	dsqrt = 1.0 / dsqrt;

	debugout("Gamma() - gamma=",(long double)dsqrt,5);
	return (long double)dsqrt;
}
