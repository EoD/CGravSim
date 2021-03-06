#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <bitset>

#define MAIN

#include "gDefines.h"
#include "gGravDataSet.h"
#include "gDebugFunc.h"
#include "gCalcFunc.h"
#include "gError.h"

int main(int argc, char* pArgs[]) {
	std::string filename;
	GravDataSet* pgdsStart = new GravDataSet();
	long double dtime_step_default = -1;
	std::ofstream ofs_silent;

	for(int i=1; i < argc ; i++) {
		if( (std::string)pArgs[i] == FLAG_DEBUG1 || (std::string)pArgs[i] == FLAG_DEBUG2) {
			std::cout << "Frontend: " << FVERSION << std::endl;
			std::cout << "Backend : " << BVERSION << std::endl;
			std::cout << "WPT     : " << WPTVERSION << std::endl;
#ifdef DEBUG
			std::cout << "Debug   : " << DEBUG << std::endl;
#endif
			return 0;
		} 
		else if( (std::string)pArgs[i] == FLAG_HELP1 || (std::string)pArgs[i] == FLAG_HELP2) {
			std::cout << "JGravSim Backend - a program to calculate gravitational effects with relativistic corrections" << std::endl;
			std::cout << "USAGE: cgravsim [" << FLAG_HELP2 << " " << FLAG_DEBUG2 << " " << FLAG_QUIET2 <<"] ["<< FLAG_TIME2 <<" TIMESTEP] [FILENAME]" << std::endl;
			std::cout << "\t FILENAME has to be in the current WPT (v" << WPTVERSION << ") format" << std::endl;
			std::cout << "Arguments:" << std::endl;
			std::cout << "  " << FLAG_HELP2  << ", " << FLAG_HELP1  << "\t\tdisplay this help" << std::endl;
			std::cout << "  " << FLAG_DEBUG2 << ", " << FLAG_DEBUG1 << "\t\tprint version of backend and expected frontend" << std::endl;
			std::cout << "  " << FLAG_TIME2  << ", " << FLAG_TIME1  << "  \toverrides the timestep which is used for calculation" << std::endl;
			std::cout << "  " << FLAG_NOCOLL2  << ", " << FLAG_NOCOLL1  << "\n\t\t\tDisables collision detection.\n\t\t\tBe aware that this may result in extremly small time steps during calculation!" << std::endl;
			std::cout << "  " << FLAG_QUIET2  << ", " << FLAG_QUIET1  << "\t\tmutes stdout (shows preceding parameters)" << std::endl;
#ifdef DEBUG
			std::cout <<   "Debug   : " << DEBUG << std::endl;
#endif
			return 0;
		}
		else if((std::string)pArgs[i] == FLAG_TIME1 || (std::string)pArgs[i] == FLAG_TIME2) {
			if( ++i < argc ) {
				std::istringstream(pArgs[i]) >> dtime_step_default;
#ifdef DEBUG
				std::cout << "Timestep has been specified: " << dtime_step_default << std::endl;
#endif
			} else { 
				break; 
			}
		}
		else if((std::string)pArgs[i] == FLAG_NOCOLL1 || (std::string)pArgs[i] == FLAG_NOCOLL2) {
			calc::flag_collision = false;
#ifdef DEBUG
			std::cout << "Collision detection has been disabled" << std::endl;
#endif
		}
		else if((std::string)pArgs[i] == FLAG_QUIET1 || (std::string)pArgs[i] == FLAG_QUIET2) {
			/* 
			 * We are overwriting the streambuf of cout to go to a ofstream of /dev/null or NUL
			 * depending on the OS. We never close the ofstream stream, as it will be closed on
			 * exit().
			 */
			std::streambuf *psbuf;
			ofs_silent.open("/dev/null");
			bool silenced = false;
			
			if(!ofs_silent.is_open()) {
#ifdef DEBUG
				std::cout << "Could not open /dev/null" << std::endl;
#endif
				ofs_silent.open("NUL");
				if(ofs_silent.is_open())
					silenced = true;
#ifdef DEBUG
				else 
					std::cerr << "Could not open /dev/null or NUL" << std::endl;
#endif
			} else
				silenced = true;

			if(silenced) {
				psbuf = ofs_silent.rdbuf();
				std::cout.rdbuf(psbuf);
#ifdef DEBUG
				std::cout << "stdout will be quiet." << std::endl;
#endif
			}
		}
		else {
			filename = pArgs[i];
#ifdef DEBUG
			std::cout << "Filename has been specified: " << filename << std::endl;
#endif
		}
	}

#ifdef DEBUG
	int del = remove(FILE_PERCENT);
	if (del != 0)
		std::cout << "Remove operation 1 failed" << std::endl;
	else
		std::cout << FILE_PERCENT << " has been deleted #1" << std::endl;
#else
	remove(FILE_PERCENT);
#endif

	if( filename.size() <= 0 ) {
		debugout("No filename has been specified", 10);
		std::cout << "Hello. Tell me the filename: ";
		std::cin >> filename;
	}
	std::cout << "Thanks. One moment please, loading " << filename << "..." << std::endl;
	if (pgdsStart->loadFile(filename)) {
		if(dtime_step_default < 0 || dtime_step_default > (long double)(pgdsStart->drTime)) {
			debugout("Setting dtime_step_default to default value!", 90);
			dtime_step_default = (long double)(pgdsStart->drTime);
		}

		int version;
		std::istringstream ssversion(pgdsStart->strVersion);
		if(ssversion >> version) {
			if(version != WPTVERSION)
				std::cerr << "WARNING! wpt version doesn't match!" << std::endl;
		} else
			std::cerr << "WARNING! Can't read wpt version!" << std::endl;

		std::cout << "Loading Finished!" << std::endl;
		std::cout << "Version: " << pgdsStart->strVersion << " (expected Version: ";
		std::cout << WPTVERSION;
		std::cout << ")" << std::endl;
		std::cout << "Number of Steps: " << pgdsStart->llnumSteps << std::endl;
		std::cout << "Save dTime: " << pgdsStart->drTime << std::endl;
		std::cout << "Calc dTime: " << dtime_step_default << std::endl;
		std::cout << "Collision: " << (calc::flag_collision?"en":"dis") << "abled" << std::endl;

		std::vector<GravStep*>::iterator i = pgdsStart->steps.begin();

		debugout("main() - objects to start with", (*i)->objects, 15);

		std::cout << "All Data loaded!" << std::endl;

		std::cout << "Calculation executing!" << std::endl;
#ifdef DEBUG
		std::cout << " Debuglevel=" << DEBUG << std::endl;
#endif
		std::bitset<ERROR_CALC_MAX> cerrors = calc::master(filename, *i, pgdsStart->llnumSteps*pgdsStart->drTime, pgdsStart->drTime, dtime_step_default);

		std::cout << std::endl << "Calculation finished";
		if(cerrors.any()) {
			error::errors |= error::calc;
			std::cout << " with errors " << cerrors;
		}

		std::cout << "!" << std::endl;

		//del = remove(FILE_PERCENT);
		return error::errors.any();
	}
	else {
		std::cout << "Loading failed!" << std::endl;
		return ERROR_FILE_IN;
	}
}
