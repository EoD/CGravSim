#include <cmath>

#include "gGravObject.h"
#include "gMathFunc.h"
#include "gDebugFunc.h"
#include "gDefines.h"

/*
#ifdef DEBUG
 #ifndef DEBUG_INC_GRAVITY_GRAVOBJECT
  #define DEBUG_INC_GRAVITY_GRAVOBJECT DEBUG
  #undef DEBUG
 #endif
#endif
*/

GravObject::GravObject(int i, long double m, long double r): id(i), mass(m), radius(r)  {
	vel = mdv();
	pos = mlv();
}

void GravObject::setCoord(const mlv& x) {
	pos = x;
}	
void GravObject::addCoords(const mlv& x) {
	pos += x;
}

long double GravObject::getAbsMass() const {
	return mass;
}
long double GravObject::getSRTMass() const {
	//falls der v-vektor (0|0|0) ist, ist auch v=0
	if(vel == 0) 
		return mass;
		
	return (mass * gamma(getAbsSpeed()) );
}
void GravObject::setMass(const long double a) {
	mass = a;
}
void GravObject::setMass(const double a) {
	mass = (long double)a;
}

long double GravObject::getRadius() const {
	//debugout("getRadius() - returning", 5);
	return radius;
}	
void GravObject::setRadius(const long double a) {
	debugout("Masspoint - new Radius",11);
	radius = a;
}
//Zu beachten ist hierbei, dass der Schwarzschildradius in der Allgemeinen 
//Relativitätstheorie nicht den Abstand vom Mittelpunkt angibt, sondern über 
//die Oberfläche von Kugeln definiert ist. Ein kugelförmiger Ereignishorizont 
//mit Radius rS hat dieselbe Fläche wie eine Sphäre gleichen Radius im euklidischen 
//Raum, nämlich A=4\,\pi\,r^2. Aufgrund der Raumzeitkrümmung sind die radialen 
//Abstände im Gravitationsfeld vergrößert (sprich: der Abstand zweier Kugelschalen 
//mit � über die Kugelfläche definierten � Radialkoordinaten r1 und r2 ist gr��er 
//als die Differenz dieser Radien).
//de.wikipedia.org/wiki/Ereignishorizont#Schwarzschild-Radius_und_Gravitationsradius
long double GravObject::getSchwarzschildRadius() const {
	//r = 2Gm / c^2
	//debugout("getSchwarzschildRadius() - returning", 5);
	return (2.0*GRAVCONST*getSRTMass())/powx(LIGHTSPEED, 2);
}

long double GravObject::drange(const GravObject* mp2) {
	//long double drange = sqrtx(dx^2 + dy^2 + dz^2)
	//MLVector mlvrange = MVMath.SubMV(this.getCoordMLV(), mp2.getCoordMLV());	
	return abs((mdv)(mp2->pos - pos));
}

long double GravObject::getAbsSpeed() const {
	return abs(vel);
}	
bool GravObject::setSpeed(const mdv& v) {
	if(abs(v) > LIGHTSPEED)
		return false;
	else {
		vel = v;
		return true;
	}
}
bool GravObject::addSpeed(const mdv& v) {
	if(abs(vel + v) > LIGHTSPEED)
		return false;
	else {
		vel += v;
		return true;
	}
}

long double GravObject::getVolume() const {
	return (4.0/3.0*powx(radius, 3))*PI;
}

long double GravObject::getSchwarzschildVolume() const {
	return (4.0/3.0*powx(getSchwarzschildRadius(), 3))*PI;
}

/** Returns the relativistic impulse for the masspoint */
mdv GravObject::getImpulse() const {
	return vel * getSRTMass();	//momentum = gamma*absmass*speed
}

/** Returns the relativistic energy for the masspoint */
long double GravObject::getEnergy() const {
	long double Energy = mass * LIGHTSPEED * LIGHTSPEED;
	Energy *= Energy;
	Energy += LIGHTSPEED * LIGHTSPEED + (getImpulse() * getImpulse());
	return sqrt(Energy);
}

/*
#ifdef DEBUG_INC_GRAVITY_GRAVOBJECT
#define DEBUG DEBUG_INC_GRAVITY_GRAVOBJECT
#undef DEBUG_INC_GRAVITY_GRAVOBJECT
#endif
*/
