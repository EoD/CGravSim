#include "gGravStep.h"
#include "gDebugFunc.h"
#include <iostream>
#include <fstream>

/*
#ifdef DEBUG
 #ifndef DEBUG_INC_GRAVITY_GRAVSTEP
  #define DEBUG_INC_GRAVITY_GRAVSTEP DEBUG
  #undef DEBUG
 #endif
#endif
*/

GravStep::GravStep() {
	numObjects = 0;
}

GravStep::~GravStep() {
	empty();
}

void GravStep::empty() {
	numObjects = 0;
	std::vector<GravObject*>::iterator i;
	for (i = objects.begin(); i != objects.end(); ++i) {
		if (*i) {
			delete *i;
		}
	}
	objects.clear();
}

void GravStep::add(GravObject* pgo) {
	if(pgo != NULL) {
		numObjects++;
		objects.push_back(pgo);
		debugout("GravStep::add - Object added", 5);
	}
	else
		debugout("GravStep::add - Tried to add NULL", 90);
}

GravObject* GravStep::addnew(GravObject* pgo) {
	GravObject* newone = new GravObject(pgo->id, pgo->mass, pgo->radius);
	newone->setSpeed(pgo->vel);
	newone->setCoord(pgo->pos);
	objects.push_back(newone);
	debugout("GravStep::add - new Object added", 5);
	numObjects++;
	return newone;
}

bool GravStep::savetofile(std::ofstream& ofs, const unsigned int stepid) {
#ifdef DEBUG
	std::vector<GravObject*>::iterator j;
	std::cout << "  savetofile Objectlist:" << std::endl;
	(std::cout).precision(DATAPRECISION);
	for (j = objects.begin(); j != objects.end(); ++j) {
			std::cout << "  ID: " << (*j)->id << ", ";
			std::cout << "  Mass: " << (*j)->mass << ", ";
			std::cout << "  Radius: " << (*j)->radius << ", ";
			std::cout << "  vel: " << (*j)->vel << ", ";
			std::cout << "  pos: " << (*j)->pos << std::endl;
	}
#endif
	
	if (!ofs) {
		debugout("savetofile() - No File found!", 99);
		return false;
	}
	
	ofs << "#" << stepid << ";" << numObjects << DELIMLINE;
	ofs.precision(DATAPRECISION);
	
	//Add Data of each object to datafile
	std::vector<GravObject*>::iterator i;
	for (i = objects.begin(); i != objects.end(); ++i) {
		if (*i) {
	    	//[Objekt ID];[Masse];[Radius];[Geschw.Vektor x];[Geschw.Vektor y];[Geschw.Vektor z];
	    	//[Beschl.Vektor x];[Beschl.Vektor y];[Beschl.Vektor z];[Position x];[Position y];[Position z]
			//debugout("Model - AddStep() - Adding data to file No"+i);
			ofs << (*i)->id << DELIMDATA;
			ofs << (*i)->mass << DELIMDATA;
			ofs << (*i)->radius << DELIMDATA;
			ofs << (*i)->vel.x << DELIMDATA;
			ofs << (*i)->vel.y << DELIMDATA;
			ofs << (*i)->vel.z << DELIMDATA;
			ofs << (*i)->pos.x << DELIMDATA;
			ofs << (*i)->pos.y << DELIMDATA;
			ofs << (*i)->pos.z << DELIMLINE;
		}
	}
	debugout("savetofile() - Step successfully saved!", 40);
	return true;
}


/*
  	public int correctHeader(File infile, int newSteps) {
		try {
			FileReader fr;
			RandomAccessFile rafile = NULL;
			BufferedReader br;
			String sCurLine;
			String[] saCurLine;
			fr = new FileReader(infile);
			rafile = new RandomAccessFile(infile, "rw");
			br = new BufferedReader(fr);
			
			sCurLine = br.readLine(); // read the first line //
			//new FileWriter(infile);
			
			if(sCurLine == NULL) {
				br.close();
				return INFILE_EOFSTARTERROR;
			}
			
			saCurLine = sCurLine.split(";");
			if(saCurLine.length != 3) {
				br.close();
				return INFILE_EOFSTARTERROR;
			}

			//String snewCurLine = sCurLine.replaceFirst(saCurLine[1], String.valueOf(newSteps));
			String snewSteps = String.valueOf(newSteps);
			debugout("correcttempHeader() - newSteps("+snewSteps.length()+")="+snewSteps+", saCurLine[1]("+saCurLine[1].length()+")="+saCurLine[1]);
			while(snewSteps.length() <= saCurLine[1].length()) {
				debugout("correcttempHeader()-loop-newSteps ="+snewSteps+", saCurLine[1]="+saCurLine[1]);
				snewSteps = "0"+snewSteps;
			}
			
			String snewCurLine = saCurLine[0]+";"+snewSteps+";"+saCurLine[2]+DELIM;//sCurLine.substring(sCurLine.indexOf(';')+1,sCurLine.lastIndexOf(';'));
			debugout("correcttempHeader() - folder="+snewCurLine);
			
				try {
					rafile.writeBytes(snewCurLine);
					///if(snewCurLine.length() < sCurLine.length())
						//rafile.write("",snewCurLine.length(),sCurLine.length());
				}		
				catch ( IOException e ) {
					e.printStackTrace();
				}
				finally {
					if ( rafile != NULL ) {
						try { rafile.close(); } 
						catch ( IOException e ) { e.printStackTrace(); }
					}
				}		
			fr.close();
			br.close();
		} catch(FileNotFoundException e) {
			return INFILE_FILENOTFOUND;
		} catch(IOException e) {
			return INFILE_READERROR;
		}
		return INFILE_NOERROR; // no error detected //	
	}*/

/*
#ifdef DEBUG_INC_GRAVITY_GRAVSTEP
#define DEBUG DEBUG_INC_GRAVITY_GRAVSTEP
#undef DEBUG_INC_GRAVITY_GRAVSTEP
#endif
*/
