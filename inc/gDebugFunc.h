#ifndef _INC_GRAVITY_DEBUGFUNC
#define _INC_GRAVITY_DEBUGFUNC

#include <iostream>
#include <vector>
#include "gGravStep.h"
#include "gMathVec.h"

void debugout(std::string strdbg, int dbgprio);
void debugout(std::string strdbg, const int var, int dbgprio);
void debugout(std::string strdbg, const long double var, int dbgprio);
void debugout(std::string strdbg, const mdv& var, int dbgprio);
void debugout(std::string strdbg, const long long var, int dbgprio);
void debugout(std::string strdbg, const mlv& var, int dbgprio);
void debugout(std::string strdbg, const std::string var, int dbgprio);
//void debugout(std::string strdbg, std::stringstream var, int dbgprio);
void debugout(std::string strdbg, const std::vector<GravObject*> & objects, int dbgprio);
void debugout(std::string a);

#endif
