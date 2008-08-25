#ifndef _INC_GRAVITY_GRAVOBJECT
#define _INC_GRAVITY_GRAVOBJECT

class GravObject {
public:
	int id;
	//int type;	//deprecated
	long double mass;
	long double radius;
	long double velx;
	long double vely;
	long double velz;
	long long accx;
	long long accy;
	long long accz;
	long long posx;
	long long posy;
	long long posz;
	
	void setCoord(long long x1, long long x2, long long x3);
	void addCoords(long long x[3]);
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
	bool setSpeed(long double v1, long double v2, long double v3);
	bool addSpeed(long double v[3]);
	long double getVolume();
	long double getSchwarzschildVolume();
};

#endif
