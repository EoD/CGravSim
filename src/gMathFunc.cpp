#include <cmath>

#include "gDefines.h"
#include "gDebugFunc.h"
#include "gMathFunc.h"

//this function is, to prevent from converting x to x/LACCURACY
long double toDouble(long double x) {
	return x;
}

long double toDouble(long long x) {
	long double dtemp = (long double)x / (long double)LACCURACY;
	return dtemp;
}

long long toLong(long long x) {
	return x;
}
long long toLong(long double x) {
	long long dtemp = (long long)floor((x * (long double)LACCURACY)+0.5);
	return dtemp;
}

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

long double powx(long double x, long double y) {
#ifdef WIN32
	return powl(x,y);
#else
	return pow((long double)x,(long double) y);
#endif
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

long double abs(long double x[], int size) {
	long double y=0;
	for (int i=0; i < size; i++) {
		y += powx(x[i], 2.0);
	}
	return sqrtx(y);
}
long double abs(long double x[]) {
	return abs(x, 3);
}

long double abs(long long x[]) {
	//removed the old pure ll-code
	//long long ltemp = (x[0] * x[0]);
	//ltemp += (x[1] * x[1]);
	//ltemp += (x[2] * x[2]);
	//long double dltemp = sqrtx(ltemp);
	
	//i have no idea what i'm doing here ;)
	//if(isnan(ltemp) || isnan(dltemp) || ltemp < 0 || dltemp < 0) {
		//debugout("abs(ll) - d/ltemp is NaN"/*,ltemp*/,90);
		long double dtemp = powx(toDouble(x[0]), 2.0);
		dtemp += powx(toDouble(x[1]), 2.0);
		dtemp += powx(toDouble(x[2]), 2.0);
		return sqrtx(dtemp);
	//}
	//debugout("abs(ll) - ltemp is ok. dltemp=",dltemp,90);
	//return dltemp;
}
