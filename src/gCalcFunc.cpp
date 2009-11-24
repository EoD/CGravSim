#include <iostream>
#include <fstream>
#include <cmath>
#include <cfloat>
#include "gDefines.h"
//#include "gGravObject.h"
//#include "gGravStep.h"

/*
#ifdef DEBUG
#ifndef DEBUG_INC_GRAVITY_CALCFUNC
#define DEBUG_INC_GRAVITY_CALCFUNC DEBUG
#undef DEBUG
#endif
#endif
*/

#include "gCalcFunc.h"
#include "gMathFunc.h"
#include "gDebugFunc.h"

int error = NOERROR;
bool flagcalc = true;
long double dtime_step = 0;

//Berechnet die Kraft auf ein referenz objekt (mpmain)
//Kraft entsteht durch grav-wirkung aller anderen
long double* calcForce(GravObject* mpmain, GravStep* vmpsinsert, long double mdvforcetotal[3]) {
	//MDVector mdvforcetotal = new MDVector(0,0,0);
	mdvforcetotal[0] = 0.0;
	mdvforcetotal[1] = 0.0;
	mdvforcetotal[2] = 0.0;

	debugout("calcForce() - START ID: ",mpmain->id,5);
	std::vector<GravObject*>::reverse_iterator i;
	for (i= vmpsinsert->objects.rbegin(); i != vmpsinsert->objects.rend(); ++i) {
		GravObject* mpsec = (*i);
		debugout("calcForce() -  START secID: ",mpsec->id,5);

		//Objekt wechselwirkt nicht mit sich selbst (nicht hier!)
		if (mpmain->id == mpsec->id) {
			continue;
		}

		//distance between objects
		long long mlvdist[] = { 0, 0, 0 };
		mlvdist[0] = mpsec->posx - mpmain->posx;
		debugout("calcForce() - mlvdist[0]=", mlvdist[0], 5);
		mlvdist[1] = mpsec->posy - mpmain->posy;
		debugout("calcForce() - mlvdist[1]=", mlvdist[1], 5);
		mlvdist[2] = mpsec->posz - mpmain->posz;
		debugout("calcForce() - mlvdist[2]=", mlvdist[2], 5);

		//halb-relativistische Gravitations-Kraftberechnung
		//(in mehrere Einzelschritt zerlegt)

		//absolute newtonian force
		long double dforce = 0.0; // |Fg| = m1*m2*G
		dforce = mpmain->getSRTMass();
		debugout("calcForce() - dforce1=", dforce, 5);
		dforce *= mpsec->getSRTMass();
		debugout("calcForce() - dforce2=", dforce, 5);
		dforce *= GRAVCONST;
		debugout("calcForce() - dforce3=", dforce, 5);
		dforce /= (powx(toDouble(mlvdist[0]), 2.0)
			+ powx(toDouble(mlvdist[1]), 2.0)
			+ powx(toDouble(mlvdist[2]), 2.0));
		debugout("calcForce() - dforce4=", dforce, 5);

		if (isnan(dforce)) {
			debugout("calcForce() - dforce is NaN - ERROR", 99);
			flagcalc = false;
			error = NANERROR;
			return NULL;
		}
		//debugout("calcForce() - relm1,relm2 = dforce: "+mpmain.getSRTMass()+","+mpsec.getSRTMass()+" = "+dforce);
		//debugout("calcForce() - mpsec: ID="+mpsec.id+", absmass="+mpsec.getAbsMass()+", absspeed="+mpsec.getSpeed());
		//debugout("calcForce() - mpsec: vx="+mpsec.getMDVSpeed().x1+", absmass="+mpsec.getAbsMass()+", absspeed="+mpsec.getSpeed());

		//debugout("calcForce() - Gravconst = "+GRAVCONST+","+(double)GRAVCONST+","+(float)GRAVCONST+","+(int)GRAVCONST+","+(long)GRAVCONST);

		//mdvrquot calculated with MDVector because 'radius' is r / r^3
		long double mdvrquot[] = { 0.0, 0.0, 0.0 };
		//Converted from long (mm) to double (meter)
		debugout("calcForce() - abs(mlvdist)=", abs(mlvdist), 5);
		mdvrquot[0] = toDouble(mlvdist[0]) / abs(mlvdist);
		mdvrquot[1] = toDouble(mlvdist[1]) / abs(mlvdist);
		mdvrquot[2] = toDouble(mlvdist[2]) / abs(mlvdist);
		debugout(" calcForce() - mdvrquot x:", mdvrquot[0], 5);
		debugout(" calcForce() - mdvrquot y:", mdvrquot[1], 5);
		debugout(" calcForce() - mdvrquot z:", mdvrquot[2], 5);
		//mdvrquot = MVMath.DivMVNum(MVMath.ConvertToD(mlvdist), MVMath.ConvertToD(mlvdist).abs()); 	// vec r / |r|
		//debugout(" Mdvrquot.abs,r1,r2,r3: "+mdvrquot.abs()+","+mdvrquot.x1+","+mdvrquot.x2+","+mdvrquot.x3);

		//Force-Vector: combination of mdvrquot and dforce
		long double mdvforce[] = { 0.0, 0.0, 0.0 };
		mdvforce[0] = mdvrquot[0] * dforce;
		mdvforce[1] = mdvrquot[1] * dforce;
		mdvforce[2] = mdvrquot[2] * dforce; // % * |Fg|
		debugout(" calcForce() - mdvforce x:", mdvforce[0], 5);
		debugout(" calcForce() - mdvforce y:", mdvforce[1], 5);
		debugout(" calcForce() - mdvforce z:", mdvforce[2], 5);
		//debugout(" |mdvforce|,x1,x2,x3: "+mdvforce.abs()+","+mdvforce.x1+","+mdvforce.x2+","+mdvforce.x3);

		//kraft von objekt i auf ref-objekt wird zu einer gesamt kraft addiert
		mdvforcetotal[0] += mdvforce[0];
		mdvforcetotal[1] += mdvforce[1];
		mdvforcetotal[2] += mdvforce[2];
		debugout(" calcForce() - totalmvforce x:", mdvforcetotal[0], 5);
		debugout(" calcForce() - totalmvforce y:", mdvforcetotal[1], 5);
		debugout(" calcForce() - totalmvforce z:", mdvforcetotal[2], 5);
		//debugout(" |mvforcetotal|,x1,x2,x3: "+mdvforcetotal.abs()+","+mdvforcetotal.x1+","+mdvforcetotal.x2+","+mdvforcetotal.x3);
	}
	//debugout("calcForce() - |mvforcesum|,x1,x2,x3: "+mdvforcetotal.abs()+","+mdvforcetotal.x1+","+mdvforcetotal.x2+","+mdvforcetotal.x3);

	//return gesamtkraft
	return mdvforcetotal;
}

GravStep* calcAcc(GravStep* vmpsinsert, GravStep* vmpsout) {
	//Vector<Masspoint> vmpsout = new Vector<Masspoint>();
#ifdef DEBUG
	std::vector<GravObject*>::reverse_iterator j;
	std::cout << "  vmpsinsert Objectlist ("<< vmpsinsert->numObjects <<" elements) :" << std::endl;
	(std::cout).precision(DATAPRECISION);
	int count = 0;
	for (j = (vmpsinsert)->objects.rbegin(); j != (vmpsinsert)->objects.rend(); ++j) {
		std::cout << "  ID: " << (*j)->id << ", ";
		std::cout << "  Mass: " << (*j)->mass << ", ";
		std::cout << "  Radius: " << (*j)->radius << ", ";
		std::cout << "  velx: " << (*j)->velx << ", ";
		std::cout << "  vely: " << (*j)->vely << ", ";
		std::cout << "  velz: " << (*j)->velz << ", ";
		std::cout << "  posx: " << (*j)->posx << ", ";
		std::cout << "  posy: " << (*j)->posy << ", ";
		std::cout << "  posz: " << (*j)->posz << std::endl;
		std::cout << " count: " << count++ << std::endl;

		if((*j)->mass <= 0 || (*j)->radius <= 0) {
			debugout("calcAcc() - Var ERROR0",99);
			return 0;
		}
	}
	if(count < 1) {
		debugout("calcAcc() - ERROR0, help",99);
		flagcalc = false;
		return 0;
	}
#endif
	//TODO Fix? vmpsout.addAll(vmpsinsert);
	//debugout("ID 0: Old Coords(x1,x2,x3): "+((Masspoint)masspoints.get(0)).mlvpos.x1+","+((Masspoint)masspoints.get(0)).mlvpos.x2+","+((Masspoint)masspoints.get(0)).mlvpos.x3);
	//debugout("ID 1: Old Coords(x1,x2,x3): "+((Masspoint)masspoints.get(1)).mlvpos.x1+","+((Masspoint)masspoints.get(1)).mlvpos.x2+","+((Masspoint)masspoints.get(1)).mlvpos.x3);
	//Berechnung der neuen Position f�r alle Objekte von ID 0 bis ID i
	debugout("calcAcc() - Start() - dtime_step = ",dtime_step, 10);

	std::vector<GravObject*>::reverse_iterator i;
	for (i = vmpsinsert->objects.rbegin(); i != vmpsinsert->objects.rend() ; ++i) {
		//debugout("calcAcc() - for Schleife Start");
		GravObject* mpold = (*i);
		GravObject* mpnew = vmpsout->addnew(mpold);
		//debugout("calcAcc() - Object added",15);

		debugout("calcAcc() - old x=",mpold->posx,5);
		debugout("calcAcc() - old y=",mpold->posy,5);
		debugout("calcAcc() - old z=",mpold->posz,5);
		debugout("calcAcc() - ID: ", mpold->id, 5); //+": Old Coords(x1,x2,x3): "+mpold.mlvpos.x1+","+mpold.mlvpos.x2+","+mpold.mlvpos.x3);		


		//Calculation of the whole force on object i

		//JAVA
		//MDVector mvforce = new MDVector(0,0,0);		
		long double mvforce[] = { 0.0, 0.0, 0.0 };
		calcForce(mpold, vmpsinsert, mvforce);

		debugout(" calcAcc() - totalmvforce x:", mvforce[0], 5);
		debugout(" calcAcc() - totalmvforce y:", mvforce[1], 5);
		debugout(" calcAcc() - totalmvforce z:", mvforce[2], 5);

		//debugout(" |MVforcetotal|,x1,x2,x3: "+mvforce.abs()+","+mvforce.x1+","+mvforce.x2+","+mvforce.x3);

		//relativistic acceleration formula
		//Math.sqrtx( Math.powx(LIGHTSPEED,2.0) - Math.powx(mpmain.getSpeed(),2.0) / Math.powx(LIGHTSPEED,2.0));
		//TODO reform!
		long double da1 = gamma(mpold->getAbsSpeed());
		da1 *= powx(LIGHTSPEED, 2.0) * mvforce[0]- mpold->velx * ProScaMV(mvforce[0], mvforce[1], mvforce[2], mpold->velx,
				mpold->vely, mpold->velz);
		da1 /= powx(LIGHTSPEED, 2.0) * mpold->getAbsMass();

		long double da2 = gamma(mpold->getAbsSpeed());
		da2 *= powx(LIGHTSPEED, 2.0) * mvforce[1]- mpold->vely * ProScaMV(mvforce[0], mvforce[1], mvforce[2], mpold->velx,
				mpold->vely, mpold->velz);
		da2 /= powx(LIGHTSPEED, 2.0) * mpold->getAbsMass();

		long double da3 = gamma(mpold->getAbsSpeed());
		da3 *= powx(LIGHTSPEED, 2.0) * mvforce[2]- mpold->velz * ProScaMV(mvforce[0], mvforce[1], mvforce[2], mpold->velx,
				mpold->vely, mpold->velz);
		da3 /= powx(LIGHTSPEED, 2.0) * mpold->getAbsMass();
		//debugout("calcAcc() -  Acceleration (a1,a2,a3): "+da1+","+da2+","+da3);

		debugout(" calcAcc() - da1:", da1, 5);
		debugout(" calcAcc() - da2:", da2, 5);
		debugout(" calcAcc() - da3:", da3, 5);

		long double mva[] = { da1, da2, da3 };
		long double deltav[] = { 0.0, 0.0, 0.0 };

		deltav[0] = mva[0] * dtime_step;
		deltav[1] = mva[1] * dtime_step;
		deltav[2] = mva[2] * dtime_step;

		debugout(" calcAcc() - deltav x:", deltav[0], 5);
		debugout(" calcAcc() - deltav y:", deltav[1], 5);
		debugout(" calcAcc() - deltav z:", deltav[2], 5);
		debugout(" calcAcc() - abs(deltav)+mpold->getAbsSpeed()=", abs(deltav)+mpold->getAbsSpeed(), 5);
		//debugout("calcAcc() -  Delta-v (dv1,dv2,dv3): "+deltav.x1+","+deltav.x2+","+deltav.x3);

		if (abs(deltav)+mpold->getAbsSpeed() > LIGHTSPEED) {
			debugout("calcAcc() - Changing dtime_step", 15);
#ifdef DEBUG
			int count = 0;
#endif
			while ((LIGHTSPEED - mpold->getAbsSpeed()) / abs(mva) < dtime_step) {
#ifdef DEBUG
				count++;
#endif
				if (dtime_step <= 10.0*DOUBLE_MIN) { //TODO FIX REQUIRED java.lang.Double.MIN_VALUE) {
					debugout("calcAcc - ERROR double-limit reached!");
					flagcalc = false;
					error = DOUBLELIMIT;
					break;
				}
				dtime_step /= 10.0;
#ifdef DEBUG
				debugout("calcAcc() -  (c-v0)/a < dtime_step. Count=",count,5); // = ("+LIGHTSPEED+"-"+mpold.getSpeed()+")/"+mva.abs()+"="+(LIGHTSPEED - mpold.getSpeed()) / mva.abs()+"<"+dtime_step);			
#endif
			}
			debugout("calcAcc() - dv+v0 > c => break. New dtime_step=", dtime_step, 10); //="+dtime_step);
			return null;
		}

		//add dv to complete v
		if (!mpnew->addSpeed(deltav)) {
			flagcalc = false;
			error = LIGHTSPEEDERROR;
		}

		//new function which produces new coords
		long long mlvds[] = { 0, 0, 0 };

		//calculates the delta-s (ds = v * dtime_step)
		//converts mvspeed mdvector to mlvector
		//FIX requ?
		mlvds[0] = toLong(mpnew->velx * dtime_step);
		mlvds[1] = toLong(mpnew->vely * dtime_step);
		mlvds[2] = toLong(mpnew->velz * dtime_step);
		//mlvds = MVMath.ProMVNum(MVMath.ConvertToL(mpold.getMDVSpeed()),dtime_step);
		//debugout("calcAcc() -  |mdv-v|,v1,v2,v3: "+mpold.getMDVSpeed().abs()+","+mpold.getMDVSpeed().x1+","+mpold.getMDVSpeed().x2+","+mpold.getMDVSpeed().x3);
		//debugout("calcAcc() -  |mlv-v|,v1,v2,v3: "+MVMath.ConvertToL(mpold.getMDVSpeed()).abs()+","+MVMath.ConvertToL(mpold.getMDVSpeed()).x1+","+MVMath.ConvertToL(mpold.getMDVSpeed()).x2+","+MVMath.ConvertToL(mpold.getMDVSpeed()).x3);
		//debugout("calcAcc() -  |delta-s|,ds1,ds2,ds3: "+mlvds.abs()+","+mlvds.x1+","+mlvds.x2+","+mlvds.x3);

		//new position = ds + old position
		long long limit = 0;
		++limit;
		//TODO FIX really requ
		/*		if(mpold->posx < 0)
		 limit = LONGLONG_MIN - mpold->posx;
		 //limit = Long.MIN_VALUE - mpold.getCoordMLV().x1;
		 else
		 limit = LONGLONG_MAX - mpold->posx;
		 //limit = Long.MAX_VALUE - mpold.getCoordMLV().x1;
		 if(abs(limit) - abs(mlvds[0]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit1 reached, break. Object ID: ",mpold->id,99);
		 error = LONGLIMIT;
		 return vmpsout;
		 }

		 if(mpold->posy < 0)
		 limit = LONGLONG_MIN - mpold->posy;
		 //limit = Long.MIN_VALUE - mpold.getCoordMLV().x2;
		 else
		 limit = LONGLONG_MAX - mpold->posy;
		 //limit = Long.MAX_VALUE - mpold.getCoordMLV().x2;
		 if(abs(limit) - abs(mlvds[1]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit2 reached, break. Object ID: ",mpold->id,99);
		 error = LONGLIMIT;
		 return vmpsout;
		 }

		 if(mpold->posz < 0)
		 limit = LONGLONG_MIN - mpold->posz;
		 //limit = Long.MIN_VALUE - mpold.getCoordMLV().x3;
		 else
		 limit = LONGLONG_MAX - mpold->posz;
		 //limit = Long.MAX_VALUE - mpold.getCoordMLV().x3;
		 if(abs(limit) - abs(mlvds[2]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit3 reached, break. Object ID: ",mpold->id,99);
		 error = LONGLIMIT;
		 return vmpsout;
		 }
		 */
		mpnew->addCoords(mlvds);
		debugout("calcAcc() - mlvds[0]=",mlvds[0],5);
		debugout("calcAcc() - mlvds[1]=",mlvds[1],5);
		debugout("calcAcc() - mlvds[2]=",mlvds[2],5);
		//mp.mlvpos = MVMath.AddMV(mlvds, mp.mlvpos);
		//debugout("calcAcc() - ID "+mpnew.id+": New Coords(x1,x2,x3): "+mpnew.mlvpos.x1+" , "+mpnew.mlvpos.x2+" , "+mpnew.mlvpos.x3);	
		debugout("calcAcc() - new x=",mpnew->posx,5);
		debugout("calcAcc() - new y=",mpnew->posy,5);
		debugout("calcAcc() - new z=",mpnew->posz,5);
	}

#ifdef DEBUG
	//std::vector<GravObject*>::reverse_iterator j;
	std::cout << "  vmpsout Objectlist ("<< vmpsout->numObjects <<" elements) :" << std::endl;
	(std::cout).precision(DATAPRECISION);
	count = 0;
	for (j = (vmpsout)->objects.rbegin(); j != (vmpsout)->objects.rend(); ++j) {
		std::cout << "  ID: " << (*j)->id << ", ";
		std::cout << "  Mass: " << (*j)->mass << ", ";
		std::cout << "  Radius: " << (*j)->radius << ", ";
		std::cout << "  velx: " << (*j)->velx << ", ";
		std::cout << "  vely: " << (*j)->vely << ", ";
		std::cout << "  velz: " << (*j)->velz << ", ";
		std::cout << "  posx: " << (*j)->posx << ", ";
		std::cout << "  posy: " << (*j)->posy << ", ";
		std::cout << "  posz: " << (*j)->posz << std::endl;
		count ++;

		if((*j)->mass <= 0 || (*j)->radius <= 0) {
			debugout("calcAcc() - Var ERROR1",99);
			return 0;
		}
	}
	if(count < 1) {
		debugout("calcAcc() - ERROR1, help",99);
		flagcalc = false;
		return 0;
	}
#endif
	debugout("calcAcc() - Finish", 5); //, size="+vmpsout.size());	
	return vmpsout;
}

GravObject* collision(GravObject* mpsurvive, GravObject* mpkill) {
	debugout("Collision() - starting", 10);

	long double dvolumesurvive = mpsurvive->getVolume();
	long double dvolumekill = mpkill->getVolume();

	long double dmass = mpsurvive->getAbsMass() + mpkill->getAbsMass(); //die massen werden addiert
	long double dvolume = dvolumesurvive + dvolumekill; //die volumina (nicht die Radien!) werden addiert
	//debugout("Collision! Object "); //+mpsurvive.id+" ("+mpsurvive.getVolume()+") and Object "+mpkill.id+"/kill ("+mpkill.getVolume()+") collided. New volume: "+dvolume);

	//Berechnung des neuen Radius' aus dem Volumen
	long double temp = 3.0 * dvolume;
	temp /= 4.0 * PI;
	long double dradius = powx(temp , (1.0 / 3.0));

	temp = 0;
	//double dradius = powx((3.0*dvolume)/(4.0*PI), 1.0/3.0);	
	//V=4/3*r^3*PI --> r = 3.sqrtx(3*V/PI/4)
	//debugout("Collision! Object "+mpsurvive.id+" ("+mpsurvive.getRadius()+") and Object "+mpkill.id+"/kill ("+mpkill.getRadius()+") collided. New radius: "+dradius);

	//new momentum (=impuls)
	long double dmomx_surv = mpsurvive->velx * mpsurvive->getSRTMass();
	long double dmomy_surv = mpsurvive->vely * mpsurvive->getSRTMass();
	long double dmomz_surv = mpsurvive->velz * mpsurvive->getSRTMass();

	long double dmomx_kill = mpkill->velx * mpkill->getSRTMass();
	long double dmomy_kill = mpkill->vely * mpkill->getSRTMass();
	long double dmomz_kill = mpkill->velz * mpkill->getSRTMass();

	long double dmomx_all = dmomx_surv + dmomx_kill;
	long double dmomy_all = dmomy_surv + dmomy_kill;
	long double dmomz_all = dmomz_surv + dmomz_kill;

	//JAVA
	//MDVector mdvmoment1 = MVMath.ProMVNum(mpsurvive.getMDVSpeed(), mpsurvive.getSRTMass());	//momentum = gamma*absmass*speed
	//MDVector mdvmoment2 = MVMath.ProMVNum(mpkill.getMDVSpeed(), mpkill.getSRTMass());	//momentum = gamma*absmass*speed
	//MDVector mdvmoment = MVMath.AddMV(mdvmoment1, mdvmoment2);

	long double dfactora = ProScaMV(dmomx_all, dmomy_all, dmomz_all); //(momentum1+momentum2)^2
	long double dgamma3;
	dgamma3 = LIGHTSPEED*LIGHTSPEED*powx(dmass, 2.0);
	dgamma3 += dfactora;
	dgamma3 = dfactora / dgamma3;
	dgamma3 = sqrtx((long double)1.0 - dgamma3);

	long double dspeedx_all = dmomx_all / dmass;
	long double dspeedy_all = dmomy_all / dmass;
	long double dspeedz_all = dmomz_all / dmass;
	dspeedx_all *= dgamma3;
	dspeedy_all *= dgamma3;
	dspeedz_all *= dgamma3;

	//JAVA
	//mpsecspeed = MVMath.DivMVNum(mdvmoment, dmass); 	//gesamtv = gesamtimpuls / gesamtmasse
	//mpsecspeed = MVMath.ProMVNum(mpsecspeed, dgamma3);

	//Object has position between the old centers
	//(arithmetisches mittel der positionen)

	if (mpsurvive->getSchwarzschildRadius() > mpsurvive->getRadius())
		dvolumesurvive = mpsurvive->getSchwarzschildVolume();

	if (mpkill->getSchwarzschildRadius() > mpkill->getRadius())
		dvolumekill = mpkill->getSchwarzschildVolume();

	long double dconst = (dvolumesurvive+dvolumekill) / 2.0;

	long long llposx_surv = (long long)(mpsurvive->posx * (dvolumesurvive/ dconst));
	long long llposy_surv = (long long)(mpsurvive->posy * (dvolumesurvive/ dconst));
	long long llposz_surv = (long long)(mpsurvive->posz * (dvolumesurvive/ dconst));
	long long llposx_kill = (long long)(mpkill->posx * (dvolumekill / dconst));
	long long llposy_kill = (long long)(mpkill->posy * (dvolumekill / dconst));
	long long llposz_kill = (long long)(mpkill->posz * (dvolumekill / dconst));

	llposx_surv += llposx_kill;
	llposy_surv += llposy_kill;
	llposz_surv += llposz_kill;

	llposx_surv /= (long long)((dvolumesurvive+dvolumekill)/dconst);
	llposy_surv /= (long long)((dvolumesurvive+dvolumekill)/dconst);
	llposz_surv /= (long long)((dvolumesurvive+dvolumekill)/dconst);

	//JAVA
	//MLVector mlvcoordsur = MVMath.ProMVNum(mpsurvive.getCoordMLV(), dvolumesurvive/dconst);
	//MLVector mlvcoordkil = MVMath.ProMVNum(mpkill.getCoordMLV(), dvolumekill/dconst);
	//MLVector mlvnewcoord = MVMath.AddMV(mlvcoordsur, mlvcoordkil);
	//mlvnewcoord = MVMath.DivMVNum(mlvnewcoord, (dvolumesurvive+dvolumekill)/dconst);

	mpsurvive->setCoord(llposx_surv, llposy_surv, llposz_surv);
	mpsurvive->setMass(dmass);
	if (!mpsurvive->setSpeed(dspeedx_all, dspeedy_all, dspeedz_all)) {
		//TODO FIX myController.flagcalc = false;
		error = LIGHTSPEEDERROR;
	}
	mpsurvive->setRadius(dradius);
	return mpsurvive;
}
GravStep* collisionCheck(GravStep* pgs_insert) {
	GravStep* pgs_output = new GravStep();
	debugout("collisionCheck() - Starting. number of objects to check: ", pgs_insert->numObjects, 7);

	//if there are no other objects to collide with
	if (pgs_insert->numObjects <= 1) {
		debugout("collisionCheck() - Cancel - Only one object survived", 10);
		std::vector<GravObject*>::reverse_iterator i;
		for (i = pgs_insert->objects.rbegin(); i != pgs_insert->objects.rend(); ++i) {
			pgs_output->addnew(*i);
		}
		return pgs_output;
	}

	std::vector<GravObject*>::reverse_iterator i;
	//Check collision, starting with last object and don't check "first" object
	for (i = pgs_insert->objects.rbegin(); i != pgs_insert->objects.rend(); ++i) {
		debugout("collisionCheck() - Checking collision, i: ", (*i)->id, 4); // between No"+i+"and No"+j);

		GravObject* mpi = *i;
		bool breakflag = false;

		//check collision only for (i-1) objects
		std::vector<GravObject*>::reverse_iterator j = pgs_insert->objects.rbegin();

		//to fix various problems
		//Use the element which follows i and stop before it reeches 0
		std::vector<GravObject*>::reverse_iterator k;
		for (k = pgs_insert->objects.rbegin(); k != i; ++k) {
			++j;
		}

		//Check all objects, beginning with number 2 (not 1) and ending with number n-1 (not n)
		for (++j ; j != pgs_insert->objects.rend() && i != --(pgs_insert->objects.rend()); ++j) {
			debugout("collisionCheck() - Checking collision, j: ", (*j)->id, 4); // between No"+i+"and No"+j);
			GravObject* mpj= *j;
			long double iradius = mpi->getRadius();
			//debugout("collisionCheck() - iraduis: ", iradius, 10); //- Object "+mpi->id+" collided with Object"+mpj->id);
			long double jradius = mpj->getRadius();
			//debugout("collisionCheck() - jradius: ", jradius, 10); //- Object "+mpi->id+" collided with Object"+mpj->id);

			if ((*j)->id == (*i)->id) {
				debugout("collisionCheck() - hmmmm. ID: ", mpi->id, 10);
				flagcalc = false;
				error = UNKNOWNERROR;
				return null;
			}

			//check if the two objects are colliding 
			//(they are also colliding if object goes into schwarschild radius)
			if (mpi->getRadius() < mpi->getSchwarzschildRadius())
				iradius = mpi->getSchwarzschildRadius();
			if (mpj->getRadius() < mpj->getSchwarzschildRadius())
				jradius = mpj->getSchwarzschildRadius();

			//if objects have a distance less than 2*Schwarzschild Radius the calculation gets very imprecisely
			//TODO FIX required!
			//if(myController.flagschwarzschild == false && mpi.drange(mpj) < 2*(mpj.getSchwarzschildRadius()+mpi.getSchwarzschildRadius())) {
			//	myController.flagschwarzschild = true;
			//}

			//debugout("collisionCheck() - radius collected",4);

			//WARNING HACK TODO remove
			if (mpi->drange(mpj) < (iradius+jradius)) {
				debugout("collisionCheck() - range: ", mpi->drange(mpj), 10); //- Object "+mpi->id+" collided with Object"+mpj->id);
				collision(mpj, mpi);
				breakflag = true;
				break;
			}
		}
		//if object didn't collide add it to the first entry of the list of objects
		//(the for starts with the last object so, the first object to add, was
		// before the last object)
		if (breakflag == false) {
			//debugout("collisionCheck() - Adding Object "+i);
			//TODO fix requ!
			pgs_output->addnew(mpi);
		}
	}
	//debugout("collisionCheck() - Ending, Size:"+vmpout.size());
	return pgs_output;
}

bool checkSpeedBorder(GravStep* pgs_test, long double dpercentage) {
	//diese for-schleife dient nur daf�r, dass die berechnung exakt wird, falls ein objekt �ber dpercentage% c kommt
	std::vector<GravObject*>::iterator i;
	for (i = pgs_test->objects.begin(); i != pgs_test->objects.end(); ++i) {
		if ((*i)->dabsspeed() >= (dpercentage*LIGHTSPEED)) {
			//debugout("checkSpeedBorder() - Object Nr."+i+"/"+mp.id+" has a speed larger "+dpercentage*100.0+"% lightspeed",10);
			return true;
		}
	}
	debugout("checkSpeedBorder() - No Object has a speed larger 75% lightspeed", 5);
	return false;
}

int CalcCode(std::string filename, GravStep* pgs_start, long double dtime_max, long double dtime_save, long double dtime_step_default) {

	//TODO myCalculationView = new CalcView((int)(datacount/timecount), myController);
	GravStep* pgs_current = NULL;
	GravStep* pgs_temp = NULL;
	long double dtime_sum = 0;
	long double dtime_smallsum = 0;
	/*long double*/dtime_step = dtime_step_default;
	long double dtime_exactstep = dtime_step/powx(10.0, 3.0);
	error = NOERROR;
	int percent = 0;
	debugout("CalcCode() - Vars initialized, starting", 15);
	savepercentage(FILE_PERCENT,percent);

	while (dtime_sum < dtime_max && flagcalc == true) {

		//Collision-Check
		if (pgs_temp == NULL && pgs_start != NULL) {
#ifdef DEBUG
			std::vector<GravObject*>::reverse_iterator j;
			std::cout << "  pgs_start Objectlist ("<< pgs_start->numObjects <<" elements) :" << std::endl;
			(std::cout).precision(DATAPRECISION);
			int count = 0;
			for (j = (pgs_start)->objects.rbegin(); j != (pgs_start)->objects.rend(); ++j) {
				std::cout << "  ID: " << (*j)->id << ", ";
				std::cout << "  Mass: " << (*j)->mass << ", ";
				std::cout << "  Radius: " << (*j)->radius << ", ";
				std::cout << "  velx: " << (*j)->velx << ", ";
				std::cout << "  vely: " << (*j)->vely << ", ";
				std::cout << "  velz: " << (*j)->velz << ", ";
				std::cout << "  posx: " << (*j)->posx << ", ";
				std::cout << "  posy: " << (*j)->posy << ", ";
				std::cout << "  posz: " << (*j)->posz << std::endl;
				count ++;

				if((*j)->mass <= 0 || (*j)->radius <= 0) {
					debugout("CalcCode() - Var ERROR1",99);
					return 0;
				}
			}
			if(count < 1) {
				debugout("CalcCode() - ERROR1, help",99);
				flagcalc = false;
				return 0;
			}
#endif
			//debugout("CalcCode - First Collision Check starting",15);
			pgs_current = collisionCheck(pgs_start);
			//empty start GravStep object and use it for temporary calcs/vars
			//pgs_temp = pgs_start;
#ifdef DEBUG
			std::vector<GravObject*>::reverse_iterator k;
			std::cout << "  pgs_current Objectlist ("<< pgs_current->numObjects <<" elements) :" << std::endl;
			(std::cout).precision(DATAPRECISION);
			count = 0;
			for (k = (pgs_current)->objects.rbegin(); k != (pgs_current)->objects.rend(); ++k) {
				std::cout << "  ID: " << (*k)->id << ", ";
				std::cout << "  Mass: " << (*k)->mass << ", ";
				std::cout << "  Radius: " << (*k)->radius << ", ";
				std::cout << "  velx: " << (*k)->velx << ", ";
				std::cout << "  vely: " << (*k)->vely << ", ";
				std::cout << "  velz: " << (*k)->velz << ", ";
				std::cout << "  posx: " << (*k)->posx << ", ";
				std::cout << "  posy: " << (*k)->posy << ", ";
				std::cout << "  posz: " << (*k)->posz << std::endl;
				count ++;

				if((*k)->mass <= 0 || (*k)->radius <= 0) {
					debugout("CalcCode() - Var ERROR2",99);
					return 0;
				}
			}
			if(count < 1) {
				debugout("CalcCode() - ERROR2, help",99);
				flagcalc = false;
				return 0;
			}
#endif
			//pgs_start->empty();
			debugout("CalcCode - First Collision Check survived", 4);
			if (pgs_current) {
				debugout("CalcCode - if(pgs_current) survived", 4);
				pgs_start->empty();
				debugout("CalcCode - if(pgs_current) survived2", 4);
				delete pgs_start;
				pgs_start = NULL;
			} else {
				debugout("CalcCode - ERROR, undefined. Line555", 99);
				return 0;
			}
			debugout("CalcCode - First Collision Check ending", 5);
			pgs_temp = new GravStep();
		}
		//the case that a new dt was found there would be problems (temp==0 && start==0)
		else if (pgs_temp != NULL && pgs_start == NULL) {
#ifdef DEBUG
			std::vector<GravObject*>::reverse_iterator j;
			std::cout << "  pgs_temp Objectlist:" << std::endl;
			(std::cout).precision(DATAPRECISION);
			int count = 0;
			for (j = (pgs_temp)->objects.rbegin(); j != (pgs_temp)->objects.rend(); ++j) {
				std::cout << "  ID: " << (*j)->id << ", ";
				std::cout << "  Mass: " << (*j)->mass << ", ";
				std::cout << "  Radius: " << (*j)->radius << ", ";
				std::cout << "  velx: " << (*j)->velx << ", ";
				std::cout << "  vely: " << (*j)->vely << ", ";
				std::cout << "  velz: " << (*j)->velz << ", ";
				std::cout << "  posx: " << (*j)->posx << ", ";
				std::cout << "  posy: " << (*j)->posy << ", ";
				std::cout << "  posz: " << (*j)->posz << std::endl;
				count++;

				if((*j)->mass <= 0 || (*j)->radius <= 0) {
					debugout("CalcCode() - Var ERROR3",99);
					return 0;
				}
			}
			if(count < 1) {
				debugout("CalcCode() - ERROR3, help",99);
				flagcalc = false;
				return 0;
			}
#endif
			debugout("CalcCode - Collision Check starting", 7);
			if (pgs_current) {
				debugout("CalcCode - Collision Check survived", 7);
				//delete pgs_current;
				pgs_current = NULL;
			}
			pgs_current = collisionCheck(pgs_temp);
			pgs_temp->empty();
			debugout("CalcCode - Collision Check ending", 5);
		} else {
			pgs_temp = new GravStep();
			debugout("CalcCode() - temp==null, start==null - seems that new dt was found", 20);
			//TODO Remove for final
			if(pgs_current->numObjects <= 1) {
				return 0;
			}
		}

		//TODO remove!
		//if((pgs_current->objects.front())->id > pgs_current->objects.back())->id) {
		//	pgs_current->objects.reverseswap();
		//}
		
		//�berpr�fe ob eines der MPs schneller als 60%c fliegt
		if (checkSpeedBorder(pgs_current, 0.8)) {
			debugout("CalcCode - Speed has been higher than 80% c", 10);
			if (dtime_step > dtime_exactstep) {
				dtime_step = dtime_exactstep;
				//TODO debugout("calcMain() - dtime_step = exactstep ="+dtime_step+"="+exactstep);
			}
			//else
			//TODO debugout("calcMain() - dtime_step <= exactstep: "+dtime_step+"<"+exactstep);
		}

		//Avoid major problems
		/*		if(pgs_temp == NULL || pgs_temp->objects.size() <= 0) {
		 debugout("calcMain() - vmps_current.size <= 0!");
		 flagcalc = false;
		 error = UNKNOWN;
		 }
		 */
		debugout("CalcCode - Starting calcAcc", 7);
		pgs_temp = calcAcc(pgs_current, pgs_temp);

		if (pgs_temp == NULL) {
			debugout("calcMain() - calcAcc hat null zurückgegeben.", 10); // Neues dtime_step="+dtime_step+", dtsum="+dtsum);
			continue;
		} else {
			dtime_smallsum += dtime_step;
			debugout("calcMain() - calcAcc hat nicht null zurückgegeben.", 10); //(size="+vmps_temp.size()+", dtime_step="+dtime_step+"). Neues dtsum="+dtsum);
		}

		if (dtime_smallsum >= dtime_save) {
			debugout("calcMain() - dtsmallsum >= timestep", 10); //: "+dtsmallsum+">="+timestep);
			//TODO FIX myModel.AddStep(vmps_temp);	//save data to file
			pgs_temp->savetofile(filename, (int)(dtime_sum/dtime_save));
			//to be able to communicate with the frontend
			//std::cout << "Step#"<< (int)(dtime_sum/dtime_save) << std::endl;
			if((dtime_sum*100.0)/dtime_max > percent+1) {
				if(!savepercentage(FILE_PERCENT,++percent)) {
					error = UNKNOWNERROR;
					break;
				}
			}


			dtime_sum += dtime_smallsum;
			dtime_smallsum = 0;
			//debugout("calcMain() - dtsum ="+dtsum+", Datacount="+datacount);
			dtime_step = dtime_step_default; //reset to old step size
			debugout("calcMain() - end, dtime_step = dtime_step_default = ", dtime_step, 10); // ="+dtime_step+"="+dtime_step_default);
			//myCalculationView.step();
		} else
			debugout("dtsmallsum < timestep", 7); // "+dtsmallsum+" < "+timestep+".dtsum="+dtsum);

		//debugout("calcMain() -d currentsize="+vmps_current.size()+", tempsize="+vmps_temp.size());			
		//vmps_current.removeAllElements();
		//debugout("calcMain() -e currentsize="+vmps_current.size()+", tempsize="+vmps_temp.size());
		//vmps_current.addAll(vmps_temp);	   
	}

	if (flagcalc == false) {
		debugout("calcMain - flagcalc==false, error=",error, 15);
	} //TODO FIX myModel.correctHeader(new File(Model.Defaultname), (int)(dtsum/timecount));

	debugout("calcMain - Quit - Roger and out", 15);
	if(!savepercentage(FILE_PERCENT, 100))
		error = UNKNOWNERROR;

	delete pgs_temp;
	return error;
}

bool savepercentage(std::string file, int percent) {
#ifdef DEBUG
	std::cout << "Write Percent#"<< percent << " to " << (std::string)file << std::endl;
#else
	if(percent < 10) 
		std::cout << " ";

	std::cout << percent << "%" << std::flush << "\b\b\b";
#endif 

	std::ofstream ofs;
	ofs.open(file.c_str(), std::ios::out | std::ios::app);
	if(!ofs.is_open()) {
	        std::cout << "savepercentage() - No File found!" << std::endl;
	        debugout("savepercentage() - No File found!", 99);
	        return false;
	}
	ofs << percent << DELIMLINE;
	ofs.close();
	debugout("savepercentage() - Percentage successfully saved!", 41);
	return true;
}

/*#ifdef DEBUG_INC_GRAVITY_CALCFUNC
#efine DEBUG DEBUG_INC_GRAVITY_CALCFUNC
#undef DEBUG_INC_GRAVITY_CALCFUNC
#endif
*/
