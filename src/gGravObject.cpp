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
	accx = accy = accz = 0;
	posx = posy = posz = 0;
}

void GravObject::setCoord(long long x1, long long x2, long long x3) {
	posx = x1;
	posy = x2;
	posz = x3;
}	
void GravObject::addCoords(long long x[3]) {
	posx += x[0];
	posy += x[1];
	posz += x[2];
}

long double GravObject::getAbsMass() {
	return mass;
}
long double GravObject::getSRTMass() {
	//falls der v-vektor (0|0|0) ist, ist auch v=0
	if(vel == 0) 
		return mass;
		
	return (mass * gamma(dabsspeed()) );
}
void GravObject::setMass(long double a) {
	mass = a;
}
void GravObject::setMass(double a) {
	mass = (long double)a;
}

long double GravObject::getRadius() {
	//debugout("getRadius() - returning", 5);
	return radius;
}	
void GravObject::setRadius(long double a) {
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
long double GravObject::getSchwarzschildRadius() {
	//r = 2Gm / c^2
	//debugout("getSchwarzschildRadius() - returning", 5);
	return (2.0*GRAVCONST*getSRTMass())/powx(LIGHTSPEED, 2.0);
}

long double GravObject::drange(GravObject* mp2) {
	//long double drange = sqrtx(dx^2 + dy^2 + dz^2)
	long double drange = powx(toDouble(mp2->posx - posx), 2.0) 
		+ powx(toDouble(mp2->posy - posy), 2.0) 
		+ powx(toDouble(mp2->posz - posz), 2.0);
	//MLVector mlvrange = MVMath.SubMV(this.getCoordMLV(), mp2.getCoordMLV());	
	return sqrtx(drange);
}

long double GravObject::dabsspeed() {
	return abs(vel);
}	
long double GravObject::getAbsSpeed() {
	return dabsspeed();
}
bool GravObject::setSpeed(mdv& v) {
	if(abs(v) > LIGHTSPEED)
		return false;
	else {
		vel = v;
		return true;
	}
}
bool GravObject::addSpeed(mdv& v) {
	if(abs(vel + v) > LIGHTSPEED)
		return false;
	else {
		vel += v;
		return true;
	}
}

long double GravObject::getVolume() {
	return (4.0/3.0*powx(radius, 3.0))*PI;
}

long double GravObject::getSchwarzschildVolume() {
	return (4.0/3.0*powx(getSchwarzschildRadius(), 3.0))*PI;
}

/*
#ifdef DEBUG_INC_GRAVITY_GRAVOBJECT
#define DEBUG DEBUG_INC_GRAVITY_GRAVOBJECT
#undef DEBUG_INC_GRAVITY_GRAVOBJECT
#endif
*/
