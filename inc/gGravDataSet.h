#ifndef _INC_GRAVITY_GRAVDATASET
#define _INC_GRAVITY_GRAVDATASET

#include <string>
#include <vector>
#include "gGravStep.h"

class GravDataSet {
public:
	GravDataSet();
	GravDataSet(std::string file);
	~GravDataSet();

	std::string strVersion;
	long long llnumSteps;
	long double drTime;
	std::vector<GravStep*> steps;

	void empty();
	bool loadFile(std::string file);
};

#endif
