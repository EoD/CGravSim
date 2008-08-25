#ifndef _INC_GRAVITY_MATHFUNC
#define _INC_GRAVITY_MATHFUNC

long double toDouble(long double x);
long double toDouble(long long x);
long long toLong(long long x);
long long toLong(long double x);
long long sqrtx(long long x);
long double sqrtx(long double x);
long double powx(long double x, long double y);
long double gamma(long double v);
long double ProScaMV(long double mv1x,long double mv1y,long double mv1z,long double mv2x,long double mv2y,long double mv2z);
long double ProScaMV(long double mv1x,long double mv1y,long double mv1z);
//long double abs(long double x);
long double abs(long double x1, long double x2, long double x3);
long double abs(long double *x, int size);
long double abs(long double *x);
long double abs(long long *x);

#endif
