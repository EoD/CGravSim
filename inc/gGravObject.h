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
	mlv pos;

	GravObject(int id=-1, long double mass=0., long double radius=0.);
	
	void setCoord(const mlv& x);
	void addCoords(const mlv& x);
	long double getAbsMass() const;
	long double getSRTMass() const;
	void setMass(const long double a);
	void setMass(const double a);
	long double getRadius() const;
	void setRadius(const long double a);
	long double getSchwarzschildRadius() const;
	long double drange(const GravObject* mp2);
	long double getAbsSpeed() const;
	bool setSpeed(const mdv& v);
	bool addSpeed(const mdv& v);
	long double getVolume() const;
	long double getSchwarzschildVolume() const;
	mdv getImpulse() const;
	long double getEnergy() const;
};

#endif
