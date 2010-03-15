
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "gDebugFunc.h"
#include "gGravDataSet.h"

GravDataSet::GravDataSet() {
	strVersion = "";
	llnumSteps=0;
	drTime=0.0;
}
GravDataSet::GravDataSet(std::string file) {
	loadFile(file);
}
GravDataSet::~GravDataSet() {
	empty();
}

void GravDataSet::empty() {
	strVersion = "";
	llnumSteps = 0;
	drTime = 0.0;
	std::vector<GravStep*>::iterator i;
	std::vector<GravObject*>::iterator j;
	for (i = steps.begin(); i != steps.end(); ++i) {
		if (*i) {
			for (j = (*i)->objects.begin(); j != (*i)->objects.end(); ++j) {
				if (*j)
					delete *j;
			}
			(*i)->objects.clear();
			delete *i;
		}
	}
	steps.clear();
}

bool GravDataSet::loadFile(std::string file) {
	empty();
	std::string::size_type fp;
	std::string::size_type sp;
	std::string line;
	std::stringstream sstr;
	std::ifstream ifs;
	ifs.open(file.c_str(), std::ios::in); // | std::ios::binary);
	if (!ifs) {
		debugout("loadFile() - No File found!", 99);
		return false;
	}

	if (std::getline(ifs, line)) {
		fp = line.find_first_not_of(";"); // find letter after first seperator
		if (fp != std::string::npos && line.substr(fp, line.length()-1-fp)!= "") // to avoid failures with lines only containing " "
		{
			sp = line.find_first_of(";", fp); // find next seperator
			if (sp != std::string::npos) {
				strVersion = line.substr(fp, sp-fp); //extract strVersion
				line.erase(0, sp); //cut line
			} else {
				debugout("loadFile() - Failure 1", 99);
				return false;
			}
			fp = line.find_first_not_of(";"); // find letter after first seperator
			sp = line.find_first_of(";", fp); // find next seperator
			if (sp != std::string::npos) {
				sstr.str(""); // empty stream
				sstr.clear(); // clear Flags
				sstr << line.substr(fp, sp-fp); //extract numSteps
				sstr >> llnumSteps;
				line.erase(0, sp); //cut line
			} else {
				debugout("loadFile() - Failure 2", 99);
				return false;
			}
			fp = line.find_first_not_of(";"); // find letter after first seperator
			sp = line.find_first_of(";", fp); // find next seperator
			if (sp == std::string::npos) {
				sstr.str(""); // empty stream
				sstr.clear(); // clear Flags
				sstr << line.substr(fp, line.length()-LINEDELIMNUM-fp); //extract rTime and cut off "\n" at the end
				sstr >> drTime;
				line.clear(); //clear line
			} else {
				debugout("loadFile() - Failure 3", 99);
				return false;
			}
		} else {
			debugout("loadFile() - Failure 4", 99);
			return false;
		}
	} else {
		debugout("loadFile() - Failure 5", 99);
		return false;
	}

	long long stepNum = -1;
	long long numObjects = -1;
	GravObject* obj = NULL;
	GravStep* step = NULL;
	while (std::getline(ifs, line)) {
		if (line.length()> 1) {
			if (line.at(0)== '#') {
				line.erase(0, 1); //delete first letter
				fp = line.find_first_not_of(";"); // find letter after first seperator
				sp = line.find_first_of(";", fp); // find next seperator
				if (sp != std::string::npos) {
					sstr.str(""); // empty stream
					sstr.clear(); // clear Flags	
					debugout("loadFile() - sstr0=",line, 5);
					sstr << line.substr(fp, sp-fp); //extract stepNumber
					debugout("loadFile() - stepNum=",line.substr(fp, sp-fp), 5);
					sstr >> stepNum;
					line.erase(fp, sp-fp); //cut line
					debugout("loadFile() - sstr2=",line, 5);
				} else {
					debugout("loadFile() - Failure 6", 99);
					return false;
				}
				fp = line.find_first_not_of(";"); // find letter after first seperator
				sp = line.find_first_of(";", fp); // find next seperator
				if (sp == std::string::npos) {
					sstr.str(""); // empty stream
					sstr.clear(); // clear Flags					
					debugout("loadFile() - sstr3=",line, 5);
					sstr << line.substr(fp, line.length()-LINEDELIMNUM-fp); //extract numObjects and cut of (\r)"\n" at the end
					debugout("loadFile() - numObjects=",line.substr(fp, line.length()-LINEDELIMNUM-fp), 5);
					sstr >> numObjects;
					line.clear(); //cut line
				} else {
					debugout("loadFile() - Failure 7", 99);
					return false;
				}
				step = new GravStep();
				//step->numObjects = numObjects;
				steps.push_back(step);
			} else if (stepNum != -1 && numObjects > 0) {
				fp = line.find_first_not_of(";"); // find letter after first seperator
				if (fp != std::string::npos && line.substr(fp,
						line.length()-1-fp)!= "") // to avoid failures with lines only containing " "
				{
					int objectID;
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract objectID
						sstr >> objectID;
						obj = new GravObject();
						obj->id = objectID;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 8", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract mass
						debugout("loadFile() - mass0=",line.substr(fp, sp-fp), 5);
						sstr >> obj->mass;
						debugout("loadFile() - mass1=",obj->mass, 5);
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 9", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract radius
						sstr >> obj->radius;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 10", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract velocity x
						sstr >> obj->vel.x;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 11", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract velocity y
						sstr >> obj->vel.y;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 12", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract velocity z
						sstr >> obj->vel.z;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 13", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract acceleration x
						sstr >> obj->accx;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 14", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract acceleration y
						sstr >> obj->accy;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 15", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract acceleration z
						sstr >> obj->accz;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 16", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract position x
						sstr >> obj->pos.x;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 17", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp != std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, sp-fp); //extract position y
						sstr >> obj->pos.y;
						line.erase(fp, sp-fp); //cut line
					} else {
						debugout("loadFile() - Failure 18", 99);
						return false;
					}
					fp = line.find_first_not_of(";"); // find letter after first seperator
					sp = line.find_first_of(";", fp); // find next seperator
					if (sp == std::string::npos) {
						sstr.str(""); // empty stream
						sstr.clear(); // clear Flags
						sstr << line.substr(fp, line.length()-LINEDELIMNUM-fp); //extract position z and cut of "\n" at the end
						sstr >> obj->pos.z;
						line.clear(); //clear line
					} else {
						debugout("loadFile() - Failure 19", 99);
						return false;
					}
					//obj->type = 0; //deprecated
					debugout("GravDataSet::loadFile - adding object", 10);
					step->add(obj);
				}
			} else {
				debugout("loadFile() - Failure 20, numObjects ==",numObjects, 99);
				return false;
			}
		}
	}
	if(step->numObjects != numObjects) {
		debugout("loadFile() - Failure 21 - numObjects not identical", 99);
		return false;			
	}
	return true;
}
