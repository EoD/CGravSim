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

void debugout(std::string strdbg, const int var, int dbgprio) {
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
void debugout(std::string strdbg, const long double var, int dbgprio) {
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
void debugout(std::string strdbg, const mdv& var, int dbgprio) {
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
void debugout(std::string strdbg, const long long var, int dbgprio) {
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
void debugout(std::string strdbg, const mlv& var, int dbgprio) {
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

void debugout(std::string strdbg, const std::string var, int dbgprio) {
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

#ifdef DEBUG
void debugout(std::string strdbg) {
	#if DEBUG >= 91
		const char* output = strdbg.c_str();
		std::cout << output << " - DBG outdated!" << std::endl;
	#endif
}
#else
void debugout(std::string /* strdbg */) { }
#endif

#ifdef __INTEL_COMPILER
 #pragma warning (disable:981) /* remark #981: operands are evaluated in unspecified order */
#endif
void debugout(std::string name, const std::vector<GravObject*> & objects, int dbgprio) {
#ifdef DEBUG
	if (dbgprio > 100 || dbgprio < 0) {
		dbgprio = 101;
	}
	if (101 - dbgprio <= DEBUG) {
		const size_t size = objects.size();
		std::cout << " " << name << " (size = "<< size << ")" << std::endl;
		(std::cout).precision(DATAPRECISION);

		int id = -1;
		for (std::vector<GravObject*>::size_type i = 0; i < size; ++i) {
			id = objects[i]->id;
			std::cout << "  ID:";
			if (id < 10)
				std::cout << " ";
			if (id < 100)
				std::cout << " ";
			if (id < 1000)
				std::cout << " ";
			std::cout << id << ", ";
			std::cout << "  Mass: " << objects[i]->mass << ", ";
			std::cout << "  Radius: " << objects[i]->radius << ", ";
			std::cout << "  vel: " << objects[i]->vel << ", ";
			std::cout << "  pos: " << objects[i]->pos << std::endl;
		}
	}
#endif
}
