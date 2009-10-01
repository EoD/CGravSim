#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <vector>
#include <cfloat>
//using namespace std; 
//einlesen, berechnen, auslesen

#define MAIN

#include "gDefines.h"
#include "gGravDataSet.h"
#include "gDebugFunc.h"
#include "gCalcFunc.h"

/*
 long double dtsmallsum;
 long double dtsum;
 long double dtime_step;
 */

//int error = NOERROR;
//bool flagcalc = true;

int main(int argc, char* pArgs[]) {
	//	long double datacount;
	//	long double timecount;
	//	long double timestep;
	//	long double exactstep;
	std::string filename;

	GravDataSet* pgdsStart = new GravDataSet();

	if (argc > 1) {
		if( (std::string)pArgs[1] == FLAG_DEBUG1 || (std::string)pArgs[1] == FLAG_DEBUG2) {
			std::cout << "Frontend: " << FVERSION << std::endl;
			std::cout << "Backend : " << BVERSION << std::endl;
#ifdef DEBUG
			std::cout << "Debug   : " << DEBUG << std::endl;
#endif
			return 0;
		} 
		else if( (std::string)pArgs[1] == FLAG_HELP1 || (std::string)pArgs[1] == FLAG_HELP2) {
			std::cout << "JGravSim Backend - a program to calculate gravitational effects with relativistic corrections" << std::endl;
			std::cout << "USAGE: jgravsim [FILENAME]" << std::endl;
			std::cout << "\t FILENAME has to be in the current WPT (v" << FVERSION << ") format" << std::endl;
			std::cout << std::endl << "Version : " << BVERSION << std::endl;
#ifdef DEBUG
			std::cout <<   "Debug   : " << DEBUG << std::endl;
#endif
			return 0;
		}
		else {
			debugout("Filename has been specified", 10);
			filename = pArgs[1];
		}
	} else {
		debugout("No filename has been specified", 10);
		std::cout << "Hello. Tell me the filename: ";
		std::cin >> filename;
	}
	std::cout << "Thanks. One moment please..." << std::endl;
	if (pgdsStart->loadFile(filename)) {
		std::cout << "Loading Finished!" << std::endl;
		std::cout << "Version: " << pgdsStart->strVersion << " (expected Version: ";
		std::cout << FVERSION;
		std::cout << ")" << std::endl;
		std::cout << "Number of Steps: " << pgdsStart->llnumSteps << std::endl;
		std::cout << "rTime: " << pgdsStart->drTime << std::endl;
		//GravStep* pgstest = (GravStep*)(*pgdsStart).steps;
		std::vector<GravStep*>::iterator i = pgdsStart->steps.begin();
#ifdef DEBUG
		int count = 0;
		std::vector<GravObject*>::iterator j;
//		std::stringstream sstr;
		debugout("Main() - For loop starts. No of elements: ",(*i)->numObjects, 15);
		for (j = (*i)->objects.begin(); j != (*i)->objects.end(); ++j) {
			std::cout << "Main() - Loop No: " << count << std::endl;
//			if ((*j)->type == 0) {	//deprecated
				// add a Planet

				std::cout << "  ID: " << (*j)->id << std::endl;
				std::cout << "  Mass: " << (*j)->mass << std::endl;
				std::cout << "  Radius: " << (*j)->radius << std::endl;
				std::cout << "  velx: " << (*j)->velx << std::endl;
				std::cout << "  vely: " << (*j)->vely << std::endl;
				std::cout << "  velz: " << (*j)->velz << std::endl;
				std::cout << "  posx: " << (*j)->posx << std::endl;
				std::cout << "  posy: " << (*j)->posy << std::endl;
				std::cout << "  posz: " << (*j)->posz << std::endl;
//			}
			count++;
		}

		std::cout << "All Data loaded!" << std::endl;
		std::cout << "Calculation executing!" << std::endl;
#ifdef DEBUG
		std::cout << " Debuglevel=" << DEBUG << std::endl;
#endif
#endif
//		std::vector<GravObject*>::iterator j = (*i)->objects.begin();
	//	std::cout << "id: " << (*j)->id << ;
		
		CalcCode(filename, *i, (long double)(pgdsStart->llnumSteps*pgdsStart->drTime), (long double)(pgdsStart->drTime), (long double)(pgdsStart->drTime));
		std::cout << "Caclulation finished!" << std::endl;
	}
	else {
		std::cout << "Loading failed!" << std::endl;
	}
	return 0;
}
