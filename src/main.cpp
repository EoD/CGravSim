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
			std::cout << "Frontend:" << FVERSION << "\n";
			std::cout << "Backend :" << BVERSION << "\n";
#ifdef DEBUG
			std::cout << "Debug   :" << DEBUG << "\n";
#endif
			return 0;
		} 
		else if( (std::string)pArgs[1] == FLAG_HELP1 || (std::string)pArgs[1] == FLAG_HELP2) {
			std::cout << "JGravSim Backend - a program to calculate gravitational effects with relativistic corrections" << "\n";
			std::cout << "USAGE: jgravsim [FILENAME]" << "\n";
			std::cout << "\t FILENAME has to be in the current WPT (v" << FVERSION << ") format" << "\n";
			std::cout << "\nVersion :" << BVERSION << "\n";
#ifdef DEBUG
			std::cout <<   "Debug   :" << DEBUG << "\n";
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
	std::cout << "Thanks. One moment please...\n";
	if (pgdsStart->loadFile(filename)) {
		std::cout << "Loading Finished!\n";
		std::cout << "Version: " << pgdsStart->strVersion << " (expected Version: ";
		std::cout << FVERSION;
		std::cout << ")\n";
		std::cout << "Number of Steps: " << pgdsStart->llnumSteps << "\n";
		std::cout << "rTime: " << pgdsStart->drTime << "\n";
		//GravStep* pgstest = (GravStep*)(*pgdsStart).steps;
		std::vector<GravStep*>::iterator i = pgdsStart->steps.begin();
#ifdef DEBUG
		int count = 0;
		std::vector<GravObject*>::iterator j;
//		std::stringstream sstr;
		debugout("Main() - For loop starts. No of elements: ",(*i)->numObjects, 15);
		for (j = (*i)->objects.begin(); j != (*i)->objects.end(); ++j) {
			std::cout << "Main() - Loop No: " << count << "\n";
//			if ((*j)->type == 0) {	//deprecated
				// add a Planet

				std::cout << "  ID: " << (*j)->id << "\n";
				//std::cout << "  type: " << (*j)->type << "\n";	//deprecated
				std::cout << "  Mass: " << (*j)->mass << "\n";
				std::cout << "  Radius: " << (*j)->radius << "\n";
				std::cout << "  velx: " << (*j)->velx << "\n";
				std::cout << "  vely: " << (*j)->vely << "\n";
				std::cout << "  velz: " << (*j)->velz << "\n";
				//std::cout << "  accx: " << (*j)->accx << "\n";	//deprecated
				//std::cout << "  accy: " << (*j)->accy << "\n";	//deprecated
				//std::cout << "  accz: " << (*j)->accz << "\n";	//deprecated
				std::cout << "  posx: " << (*j)->posx << "\n";
				std::cout << "  posy: " << (*j)->posy << "\n";
				std::cout << "  posz: " << (*j)->posz << "\n";
//			}
			count++;
		}

		std::cout << "All Data loaded!\n";
		std::cout << "Calculation executing!\n";
#ifdef DEBUG
		std::cout << " Debuglevel=" << DEBUG << "\n";
#endif
#endif
//		std::vector<GravObject*>::iterator j = (*i)->objects.begin();
	//	std::cout << "id: " << (*j)->id << ;
		
		CalcCode(filename, *i, (long double)(pgdsStart->llnumSteps*pgdsStart->drTime), (long double)(pgdsStart->drTime), (long double)(pgdsStart->drTime));
		std::cout << "Caclulation finished!\n";
	}
	else {
		std::cout << "Loading failed!\n";
	}
	return 0;
}
