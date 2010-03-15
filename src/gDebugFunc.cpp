#include <iostream>
//#include <sstream>
//#include <fstream>

#include "gDefines.h"
#include "gDebugFunc.h"

void debugout(std::string strdbg, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		std::cout << output << std::endl;
	}
#endif
}

void debugout(std::string strdbg, int var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		(std::cout).precision( DATAPRECISION );
		std::cout << output << var << std::endl;
	}
#endif
}
void debugout(std::string strdbg, long double var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		(std::cout).precision( DATAPRECISION );
		std::cout << output << var << std::endl;
	}
#endif
}
void debugout(std::string strdbg, mdv& var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		(std::cout).precision( DATAPRECISION );
		std::cout << output << var << std::endl;
	}
#endif
}
void debugout(std::string strdbg, long long var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		(std::cout).precision( DATAPRECISION );
		std::cout << output << var << std::endl;
	}
#endif
}
void debugout(std::string strdbg, mlv& var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		(std::cout).precision( DATAPRECISION );
		std::cout << output << var << std::endl;
	}
#endif
}

void debugout(std::string strdbg, std::string var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		const char* output2 = var.c_str();
		std::cout << output << output2 << std::endl;
	}
#endif
}

/*void debugout(std::string strdbg, std::stringstream var, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if ( 101 - dbgprio <= DEBUG) {
		const char* output = strdbg.c_str();
		std::string output2;
		var >> output2;
		const char* output3 = output2.c_str();
		std::cout << output << output3 << std::endl;
	}
#endif
}*/

//_itoa( myInt, myString, 10);

void debugout(std::string strdbg) {
#ifdef DEBUG
	if ( 91 <= DEBUG) {
		const char* output = strdbg.c_str();
		std::cout << output << " - DBG outdated!" << std::endl;
	}
#endif
}

