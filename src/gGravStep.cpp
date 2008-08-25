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
	if(pgo != null) {
		numObjects++;
		objects.push_back(pgo);
		debugout("GravStep::add - Object added", 5);
	}
	else
		debugout("GravStep::add - Tried to add null", 90);
}

GravObject* GravStep::addnew(GravObject* pgo) {
	GravObject* newone = new GravObject();
	newone->id = pgo->id;
	newone->velx = pgo->velx;
	newone->vely = pgo->vely;
	newone->velz = pgo->velz;
	newone->setCoord(pgo->posx, pgo->posy,pgo->posz);
	newone->mass = pgo->mass;
	newone->radius = pgo->radius;
	objects.push_back(newone);
	debugout("GravStep::add - new Object added", 5);
	numObjects++;
	return newone;
}

bool GravStep::savetofile(std::string file, int stepid) {
#ifdef DEBUG
	std::vector<GravObject*>::iterator j;
	std::cout << "  savetofile Objectlist:\n";
	(std::cout).precision(DATAPRECISION);
	for (j = objects.begin(); j != objects.end(); ++j) {
			std::cout << "  ID: " << (*j)->id << ", ";
			//std::cout << "  type: " << (*j)->type << "\n";	//deprecated
			std::cout << "  Mass: " << (*j)->mass << ", ";
			std::cout << "  Radius: " << (*j)->radius << ", ";
			std::cout << "  velx: " << (*j)->velx << ", ";
			std::cout << "  vely: " << (*j)->vely << ", ";
			std::cout << "  velz: " << (*j)->velz << ", ";
			//std::cout << "  accx: " << (*j)->accx << "\n";	//deprecated
			//std::cout << "  accy: " << (*j)->accy << "\n";	//deprecated
			//std::cout << "  accz: " << (*j)->accz << "\n";	//deprecated
			std::cout << "  posx: " << (*j)->posx << ", ";
			std::cout << "  posy: " << (*j)->posy << ", ";
			std::cout << "  posz: " << (*j)->posz << "\n";
	}
#endif
	
	std::ofstream ofs;
	ofs.open(file.c_str(), std::ios::out | std::ios::app);
	if (!ofs) {
		debugout("savetofile() - No File found!", 99);
		return false;
	}
	
	ofs << "#" << stepid+1 << ";" << numObjects << DELIMLINE;
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
			ofs << (*i)->velx << DELIMDATA;
			ofs << (*i)->vely << DELIMDATA;
			ofs << (*i)->velz << DELIMDATA;
			ofs << (*i)->accx << DELIMDATA;
			ofs << (*i)->accy << DELIMDATA;
			ofs << (*i)->accz << DELIMDATA;
			ofs << (*i)->posx << DELIMDATA;
			ofs << (*i)->posy << DELIMDATA;
			ofs << (*i)->posz << DELIMLINE;
		}
	}
	ofs.close();
	debugout("savetofile() - Step successfully saved!", 40);
	return true;
}


/*
  	public int correctHeader(File infile, int newSteps) {
		try {
			FileReader fr;
			RandomAccessFile rafile = null;
			BufferedReader br;
			String sCurLine;
			String[] saCurLine;
			fr = new FileReader(infile);
			rafile = new RandomAccessFile(infile, "rw");
			br = new BufferedReader(fr);
			
			sCurLine = br.readLine(); // read the first line //
			//new FileWriter(infile);
			
			if(sCurLine == null) {
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
					if ( rafile != null ) {
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
