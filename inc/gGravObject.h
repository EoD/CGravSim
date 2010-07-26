#ifndef _INC_GRAVITY_GRAVOBJECT
#define _INC_GRAVITY_GRAVOBJECT

#include "gMathVec.h"

class GravObject {
public:
	int id;
	//int type;	//deprecated
	long double mass;
	long double radius;
	mdv vel;
	long long accx;
	long long accy;
	long long accz;
	mlv pos;

	GravObject(int id=-1, long double mass=0., long double radius=0.);
	
	void setCoord(mlv& x);
	void addCoords(mlv& x);
	long double getAbsMass();
	long double getSRTMass();
	void setMass(long double a);
	void setMass(double a);
	long double getRadius();
	void setRadius(long double a);
	long double getSchwarzschildRadius();
	long double drange(GravObject* mp2);
	long double dabsspeed();
	long double getAbsSpeed();
	bool setSpeed(mdv& v);
	bool addSpeed(mdv& v);
	long double getVolume();
	long double getSchwarzschildVolume();
	mdv getImpulse();
	long double getEnergy();
};

#endif
