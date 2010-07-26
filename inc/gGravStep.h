#ifndef _INC_GRAVITY_GRAVSTEP
#define _INC_GRAVITY_GRAVSTEP

#include <vector>
//#include <iostream>
#include <string>
//#include <sstream>
//#include <fstream>
#include "gGravObject.h"
#include "gDefines.h"

class GravStep {
public:
	long long numObjects;
	std::vector<GravObject*> objects;

	GravStep();
	~GravStep();
	void empty();
	void add(GravObject* pgo);
	GravObject* addnew(GravObject* pgo);
	bool savetofile(std::ofstream& ofs, const unsigned int stepid);
};
#endif
