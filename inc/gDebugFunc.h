#ifndef _INC_GRAVITY_DEBUGFUNC
#define _INC_GRAVITY_DEBUGFUNC

#include <iostream>
//#include <sstream>
//#include <fstream>

void debugout(std::string strdbg, int dbgprio);
void debugout(std::string strdbg, int var, int dbgprio);
void debugout(std::string strdbg, long double var, int dbgprio);
void debugout(std::string strdbg, long long var, int dbgprio);
void debugout(std::string strdbg, std::string var, int dbgprio);
//void debugout(std::string strdbg, std::stringstream var, int dbgprio);
void debugout(std::string a);

#endif
