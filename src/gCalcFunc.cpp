#ifdef _OPENMP
 #include <omp.h>
#endif
#include <iostream>
#include <fstream>

#include "gDefines.h"
#include "gCalcFunc.h"
#include "gMathFunc.h"
#include "gDebugFunc.h"
#include "gError.h"

std::bitset<ERROR_CALC_MAX> calc::cerrors;
bool calc::flagcalc = true;
long double calc::dtime_step = 0;
bool calc::flag_collision = true;

/** 
 * Calculcate the force between reference object and all the other 
 * objects in the GravStep
 *
 * @param mpmain	pointer to the reference object
 * @param vmpsinsert	GravStep* which we calculate against
 * @param mdvforcetotal	mdv& the resulting total force for that object
 * @return		the error value (ERROR_NONE or ERROR_CALC_NAN)
 */
int calc::calcForce(GravObject* mpmain, GravStep* vmpsinsert, mdv& mdvforcetotal) {
	mdvforcetotal = mdv(0);

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
		const mlv mlvdist = mpsec->pos - mpmain->pos;
		//Converted from long (mm) to double (meter)
		const mdv mdvdist = mlvdist;
		debugout("calcForce() - mlvdist=", mlvdist, 5);

		//halb-relativistische Gravitations-Kraftberechnung
		//(in mehrere Einzelschritt zerlegt)

		//absolute newtonian force
		long double dforce = 0.0; // |Fg| = m1*m2*G/(pos2-pos1)^2
		dforce = mpmain->getSRTMass();
		debugout("calcForce() - dforce1=", dforce, 5);
		dforce *= mpsec->getSRTMass();
		debugout("calcForce() - dforce2=", dforce, 5);
		dforce *= GRAVCONST;
		debugout("calcForce() - dforce3=", dforce, 5);
		dforce /= mdvdist * mdvdist;
		debugout("calcForce() - dforce4=", dforce, 5);

		if (isnan(dforce)) {
			debugout("calcForce() - dforce is NaN - ERROR", 99);
			return ERROR_CALC_NAN;
		}

		//mdvrquot calculated with MDVector because 'radius' is r / r^3
		mdv mdvrquot(0);
		debugout("calcForce() - abs(mdvdist)=", abs(mdvdist), 5);
		mdvrquot = mdvdist / abs(mdvdist);
		debugout(" calcForce() - mdvrquot: ", mdvrquot, 5);

		//Force-Vector: combination of mdvrquot and dforce
		mdv mdvforce(0);
		mdvforce = mdvrquot * dforce;
		debugout(" calcForce() - mdvforce: ", mdvforce, 5);

		//kraft von objekt i auf ref-objekt wird zu einer gesamt kraft addiert
		mdvforcetotal += mdvforce;
		debugout(" calcForce() - totalmvforce: ", mdvforcetotal, 5);
	}
	return ERROR_NONE;
}

/** 
 * Calculcate the new position of the whole GravStep
 *
 * It loops through vmpinsert, uses calcForce() to calculate the force which acts
 * on that object, calculates the (relat. corrected) acceleration and uses that value 
 * to get its speed and its new position.<br>
 * In the case that calcForce() returns ERROR_CALC_NAN, calcAcc cancels the calculation
 * and returns NULL.
 *
 * @param vmpsinsert	The starting GravStep pointer for the calculation
 * @param vmpsout	A GravStep pointer which will the result will be added to
 * @return		GravStep *vmpsout if calculation successfully completed,
 * 			NULL if the calculation failed.
 */
GravStep* calc::calcAcc(GravStep* vmpsinsert, GravStep* vmpsout) {

	debugout("calcAcc() - vmpsinsert Objectlist", vmpsinsert->objects, 5);

	//Berechnung der neuen Position f�r alle Objekte von ID 0 bis ID i
	debugout("calcAcc() - Start() - dtime_step = ",dtime_step, 10);
	
#ifdef _OPENMP
	int numProcs = omp_get_num_procs();
	omp_set_num_threads( numProcs * numProcs / 2 + 1);
#endif
	bool bkillloop = false;

	#pragma omp parallel for                                                                                  
	for (std::vector<GravObject*>::size_type i = 0; i < vmpsinsert->objects.size() ; i++) {

		//skip the for-loop if error has been reached	
		#pragma omp flush (bkillloop)
		if(bkillloop)
			FORKILLER;

		GravObject* mpold = (vmpsinsert->objects)[i];
		GravObject* mpnew;
		
		#pragma omp critical
		mpnew = vmpsout->addnew(mpold);

		debugout("calcAcc() - old (x,y,z)=",mpold->pos,5);
		debugout("calcAcc() - ID: ", mpold->id, 5);

		//Calculation of the whole force on object i
		mdv mvforce(0);
		int error = calcForce(mpold, vmpsinsert, mvforce);
		if(error != ERROR_NONE) {
			flagcalc = false;
			bkillloop = true;
			cerrors |= ERROR_TO_BITSET(error);
			FORKILLER;
		}

		debugout(" calcAcc() - totalmvforce: ", mvforce, 5);

		//relativistic acceleration formula
		const long double dLightspeed2 = powx(LIGHTSPEED, 2);
		const long double dSpeedFactor1 = gamma(mpold->getAbsSpeed());
		const long double dSpeedFactor2 = mvforce * mpold->vel;
		const long double dMassFactor = dLightspeed2 * mpold->getAbsMass();
	
		long double da1 = dSpeedFactor1;
		da1 *= dLightspeed2 * mvforce.x - mpold->vel.x * dSpeedFactor2;
		da1 /= dMassFactor;

		long double da2 = dSpeedFactor1;
		da2 *= dLightspeed2 * mvforce.y - mpold->vel.y * dSpeedFactor2;
		da2 /= dMassFactor;

		long double da3 = dSpeedFactor1;
		da3 *= dLightspeed2 * mvforce.z - mpold->vel.z * dSpeedFactor2;
		da3 /= dMassFactor;

		const mdv mva(da1, da2, da3);
		debugout(" calcAcc() - da = ", mva, 5);

		const mdv deltav = mva * dtime_step;
		debugout(" calcAcc() - deltav: ", deltav, 5);

		if (abs(deltav)+mpold->getAbsSpeed() > LIGHTSPEED) {
#ifdef DEBUG
			debugout("calcAcc() - Changing dtime_step", 15);
			debugout("calcAcc() - abs(deltav)+mpold->getAbsSpeed()=", abs(deltav)+mpold->getAbsSpeed(), 5);
			int count = 0;
#endif
			while ((LIGHTSPEED - mpold->getAbsSpeed()) / abs(mva) < dtime_step) {
#ifdef DEBUG
				count++;
#endif
				if (dtime_step <= 10.0*LDBL_MIN) {
					debugout("calcAcc - ERROR double-limit reached!");
					flagcalc = false;
					cerrors |= cerror::limit_dbl;
					break;
				}
				dtime_step /= 10.0;
#ifdef DEBUG
				debugout("calcAcc() -  (c-v0)/a < dtime_step. Count=",count,5);
#endif
			}
			debugout("calcAcc() - dv+v0 > c => break. New dtime_step=", dtime_step, 10); //="+dtime_step);
			std::cerr << "WARNING - reduced timestep to " << dtime_step << "!" << std::endl;

			bkillloop = true;
			#pragma omp flush (bkillloop)
			FORKILLER;
		}

		//add dv to complete v
		if (!mpnew->addSpeed(deltav)) {
			flagcalc = false;
			cerrors |= cerror::lightspeed;;
		}

		/*
		 * new function which produces the new coordinates
		 * calculates the delta-s (ds = v * dtime_step)
		 * and converts mvspeed mdvector to mlvector
		 */
		const mlv mlvds = mpnew->vel * dtime_step;

		//TODO FIX really requ
		/*		if(mpold->posx < 0)
		 limit = LLONG_MIN - mpold->posx;
		 else
		 limit = LLONG_MAX - mpold->posx;
		 if(abs(limit) - abs(mlvds[0]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit1 reached, break. Object ID: ",mpold->id,99);
		 error = ERROR_CALC_LIMIT_LONG;
		 return vmpsout;
		 }

		 if(mpold->posy < 0)
		 limit = LLONG_MIN - mpold->posy;
		 else
		 limit = LLONG_MAX - mpold->posy;
		 if(abs(limit) - abs(mlvds[1]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit2 reached, break. Object ID: ",mpold->id,99);
		 error = ERROR_CALC_LIMIT_LONG;
		 return vmpsout;
		 }

		 if(mpold->posz < 0)
		 limit = LLONG_MIN - mpold->posz;
		 else
		 limit = LLONG_MAX - mpold->posz;
		 if(abs(limit) - abs(mlvds[2]) < 0) {
		 flagcalc = false;
		 debugout("calcAcc - Long Limit3 reached, break. Object ID: ",mpold->id,99);
		 error = ERROR_CALC_LIMIT_LONG;
		 return vmpsout;
		 }
		 */
		
		/* New position = ds + old position */
		mpnew->addCoords(mlvds);
		debugout("calcAcc() - mlvds=",mlvds,5);
		debugout("calcAcc() - new (x,y,z)=",mpnew->pos,5);
	}
	if(bkillloop)
		return NULL;

	debugout("calcAcc() - vmpsout Objectlist", vmpsout->objects, 5);
	debugout("calcAcc() - Finish", 5);	
	return vmpsout;
}

/**
 * Lets two object collide (fully inelastic) into a bigger one
 * 
 * The code preserve both volume and impulse.
 * It saves the resulting GravObject into mpsurvive
 *
 * @param mpsurvive	First GravObject *, will be used as result
 * @param mpkill	Second GravObject *
 * @return		GravObject * to mpsurvive
 */
GravObject* calc::collision(GravObject* mpsurvive, GravObject* mpkill) {
	debugout("Collision() - starting", 10);

	long double dvolumesurvive = mpsurvive->getVolume();
	long double dvolumekill = mpkill->getVolume();

	long double dvolume = dvolumesurvive + dvolumekill; //die volumina (nicht die Radien!) werden addiert

	//Berechnung des neuen Radius' aus dem Volumen
	long double temp = 3.0 * dvolume;
	temp /= 4.0 * PI;
	long double dradius = pow(temp , (long double)(1.0/3.0));

	//new momentum (=impuls)
	mdv dmom_all = mpkill->getImpulse() + mpsurvive->getImpulse();

	long double dfactora = dmom_all * dmom_all; //(momentum1+momentum2)^2
	/*
	 * The resulting mass of a total inelastic collision between relavistic
	 * objects is m = sqrt( ( E1 + E2 )^2 / c^2 - (p1 + p2)^2 ) / c
	 */
	long double E1 = mpsurvive->getEnergy(), E2 = mpkill->getEnergy();
	long double dmass = sqrt( (E1+E2)*(E1+E2) / (LIGHTSPEED*LIGHTSPEED) - dfactora) / LIGHTSPEED;
	
	/*
	 * It's pretty easy to calculate the speed of the center of mass
	 * of those two objects with E=mc^2 and p=mv =>  v = E/c^2
	 * m is in both cases m_0*gamma.
	 */
	const mdv dspeed_all = dmom_all * (LIGHTSPEED * LIGHTSPEED / (E1+E2));

	/*
	 * To calculate the new positions, we weight them by their "relativistic" 
	 * mass (not the invariant mass!). See collision.pdf for details.
	 */
	const long double mass_surv = mpsurvive->getSRTMass();
	const long double mass_kill = mpkill->getSRTMass();

	const mlv llpos_surv = mpsurvive->pos * ( mass_surv / (mass_surv + mass_kill) );
	const mlv llpos_kill = mpkill->pos * ( mass_kill / (mass_surv + mass_kill) );

	mpsurvive->setCoord(llpos_surv+llpos_kill);
	mpsurvive->setMass(dmass);
	if (!mpsurvive->setSpeed(dspeed_all)) {
		//TODO FIX myController.flagcalc = false;
		cerrors |= cerror::lightspeed;
	}
	mpsurvive->setRadius(dradius);
	return mpsurvive;
}
/**
 * Checks all objects of a GravStep for collision
 *
 * @param pgs_insert	GravStep * which should be checked against
 * @return		pointer to a new GravStep pgs_output
 */
GravStep* calc::collisionCheck(GravStep* pgs_insert) {
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
				cerrors |= cerror::unknown;
				return NULL;
			}

			//check if the two objects are colliding 
			//(they are also colliding if object goes into schwarschild radius)
			if (mpi->getRadius() < mpi->getSchwarzschildRadius())
				iradius = mpi->getSchwarzschildRadius();
			if (mpj->getRadius() < mpj->getSchwarzschildRadius())
				jradius = mpj->getSchwarzschildRadius();

			/* if objects have a distance less than 2*Schwarzschild radius the calculation gets very unprecise */
			if( !cerrors.test(ERROR_CALC_GRT) && mpi->drange(mpj) < 2.0*(mpi->getSchwarzschildRadius() + mpj->getSchwarzschildRadius()) )
				cerrors |= cerror::grt;
		
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

/**
 * Check if any object of a GravStep is faster than dpercenatage * c
 *
 * @param pgs_test	GravStep * which should be tested
 * @param dpercentage	percentage of speed of light
 * @return		true if there is an object, false if there is none
 */
bool calc::checkSpeedBorder(GravStep* pgs_test, long double dpercentage) {
	//diese for-schleife dient nur daf�r, dass die berechnung exakt wird, falls ein objekt �ber dpercentage% c kommt
	std::vector<GravObject*>::iterator i;
	for (i = pgs_test->objects.begin(); i != pgs_test->objects.end(); ++i) {
		if ((*i)->getAbsSpeed() >= (dpercentage*LIGHTSPEED)) {
			//debugout("checkSpeedBorder() - Object Nr."+i+"/"+mp.id+" has a speed larger "+dpercentage*100.0+"% lightspeed",10);
			return true;
		}
	}
	debugout("checkSpeedBorder() - No Object has a speed larger 75% lightspeed", 5);
	return false;
}

/**
 * Handles the whole calculation
 *
 * Additionally the master checks for collision, finds new timesteps 
 * if forces get too strong, saves percentage of calculation to file
 * and saves GravStep to file.
 *
 * @param filename		file in which each GravStep will be saved
 * @param pgs_start		starting GravStep
 * @param dtime_max		total amount of time for calculation
 * @param dtime_save		amount of time after a GravStep will be saved
 * @param dtime_step_default	default time for each step
 * @return			the bitset cerrors which contains information about all errors
 * 
 */
std::bitset<ERROR_CALC_MAX> calc::master(std::string filename, GravStep* pgs_start, long double dtime_max, long double dtime_save, long double dtime_step_default) {

	GravStep* pgs_current = NULL;
	GravStep* pgs_temp = NULL;
	long double dtime_sum = 0;
	unsigned int lstep = 0;
	/*long double*/dtime_step = dtime_step_default;
	long double dtime_exactstep = dtime_step/powx(10.0, 3);
	cerrors.reset();
	int percent = 0;
	debugout("CalcCode() - Vars initialized, starting", 15);
	savepercentage(FILE_PERCENT,percent);
	std::ofstream ofs_temp;
	ofs_temp.open(filename.c_str(), std::ios::out | std::ios::app);

	while (dtime_sum < dtime_max && flagcalc == true) {

		//Collision-Check
		if (pgs_temp == NULL && pgs_start != NULL) {
			debugout("master() - pgs_start Objectlist", pgs_start->objects, 65);
			debugout("CalcCode - First Collision Check starting", 5);

			if(flag_collision) {
				pgs_current = collisionCheck(pgs_start);
			} else {
				pgs_current = pgs_start;
				pgs_start = new GravStep();
			}
			//empty start GravStep object and use it for temporary calcs/vars
			//pgs_temp = pgs_start;
			//pgs_start->empty();
			debugout("CalcCode - First Collision Check survived", 4);
			debugout("master() - pgs_current Objectlist", pgs_current->objects, 15);

			if (pgs_current) {
				debugout("CalcCode - if(pgs_current) survived", 4);
				pgs_start->empty();
				debugout("CalcCode - if(pgs_current) survived2", 4);
				delete pgs_start;
				pgs_start = NULL;
			} else {
				debugout("CalcCode - ERROR, undefined. Line555", 99);
				return cerrors |= cerror::unknown;
			}
			debugout("CalcCode - First Collision Check ending", 5);
			pgs_temp = new GravStep();
		}
		//the case that a new dt was found there would be problems (temp==0 && start==0)
		else if (pgs_temp != NULL && pgs_start == NULL) {
			debugout("master() - pgs_temp Objectlist", pgs_temp->objects, 20);
			debugout("CalcCode - Collision Check starting", 7);
			if (pgs_current) {
				debugout("CalcCode - Collision Check survived", 7);
				//delete pgs_current;
				pgs_current = NULL;
			}
			if(flag_collision) {
				pgs_current = collisionCheck(pgs_temp);
				pgs_temp->empty();
			} else {
				pgs_current = pgs_temp;
				pgs_temp = new GravStep();
			}

			debugout("CalcCode - Collision Check ending", 5);
		} else {
			pgs_temp = new GravStep();
			debugout("CalcCode() - temp==NULL, start==NULL - seems that new dt was found", 20);
		}
		
		/* Check if one of the mps is faster then 80% of c.
		 * Only required when checking for collsion! 
		 */		
		if (flag_collision && checkSpeedBorder(pgs_current, 0.8)) {
			debugout("CalcCode - Speed has been higher than 80% c", 10);
			if (dtime_step > dtime_exactstep) {
				dtime_step = dtime_exactstep;
				//TODO debugout("calcMain() - dtime_step = exactstep ="+dtime_step+"="+exactstep);
			}
			//else
			//TODO debugout("calcMain() - dtime_step <= exactstep: "+dtime_step+"<"+exactstep);
		}

		debugout("CalcCode - Starting calcAcc", 7);
		pgs_temp = calcAcc(pgs_current, pgs_temp);

		if (pgs_temp == NULL) {
			debugout("calcMain() - calcAcc hat NULL zurückgegeben.", 10); // Neues dtime_step="+dtime_step+", dtsum="+dtsum);
			continue;
		} else {
			dtime_sum += dtime_step;
			debugout("calcMain() - calcAcc hat nicht NULL zurückgegeben.", 10); //(size="+vmps_temp.size()+", dtime_step="+dtime_step+"). Neues dtsum="+dtsum);
		}

		if (dtime_sum >= lstep*dtime_save) {
			debugout("calcMain() - dtime_sum >= lstep*dtime_save", 10);
			pgs_temp->savetofile(ofs_temp, ++lstep);
			//to be able to communicate with the frontend
			if((dtime_sum*100.0)/dtime_max > percent+1) {
				if(!savepercentage(FILE_PERCENT,++percent)) {
					error::errors |= error::file_out;
					break;
				}
			}

			dtime_step = dtime_step_default; //reset to old step size
			debugout("calcMain() - end, dtime_step = dtime_step_default = ", dtime_step, 10);
		} else
			debugout("dtime_sum < lstep*dtime_save", 7);
	}

	if (flagcalc == false) {
		debugout("calcMain - flagcalc==false, cerrors=", cerrors.to_string(), 15);
	} //TODO FIX myModel.correctHeader(new File(Model.Defaultname), (int)(dtsum/timecount));

	debugout("calcMain - Quit - Roger and out", 15);

	/* in case calculation stopped before 100% were reached */
	if(percent < 100 && !savepercentage(FILE_PERCENT, 100))
		error::errors |= error::file_out;

	ofs_temp.close();
	delete pgs_temp;
	return cerrors;
}

/**
 * Saves the percentage of the calculation to file 
 *
 * @param file		The file the percentage will be saved to
 * @param percent	the percentage as int
 * @return		true if percentage could be saved, false otherwise
 */
bool calc::savepercentage(std::string file, int percent) {
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
