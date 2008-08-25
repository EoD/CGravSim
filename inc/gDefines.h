#ifndef _INC_GRAVITY_DEFINES
#define _INC_GRAVITY_DEFINES


#ifdef _DEBUG32
 #define DEBUG 101
#endif


#define BVERSION 0.9
#define FVERSION 1.8
#define null NULL

#define FLAG_DEBUG1 "--version"
#define FLAG_DEBUG2 "-v"

#define DATAPRECISION 20
#define DELIMDATA ";"

#ifdef WIN32
 #define DELIMLINE "\n"
 #define LINEDELIMNUM	0	//this comes from the fact, that windows recognizes "CR LN" as _one_ linebreak
#else
 //#define DELIMLINE "\x0A\x0D"	//hexcode for "\r\n"
 #define DELIMLINE "\r\n"
 #define LINEDELIMNUM	1	//this comes from the fact, that unix recognizes "CR LN" as one linebreak + one character
#endif

#ifndef isnan
#define isnan(x) ((x) != (x))
#endif

//natural constants
#define LIGHTSPEED 299792458.0 	//Lichtgeschwindigkeit in m/s
#define GRAVCONST 6.673E-11		//Gravitationskonstante
#define AE 1.495979E11 	//Astronomische Einheit (Längeneinheit) in m
#define LY 9.461E15		//Lichtjahr in m
#define PI 3.141592654; //TODO new PI requ
#define DOUBLE_MIN DBL_MIN 
#define LONGLONG_MAX LONG_MAX*LONG_MAX	//TODO FIX!
#define LONGLONG_MIN -LONGLONG_MAX		//TODO FIX!

#define LACCURACY 1.0E2
/* #define TIMESTEPX 60.0*60.0			//(in s)
 #define TIMECOUNTX TIMESTEPX*10.0 //2months
 // TODO manuelle Eingabe!
 #define DATACOUNTX 365.0*24.0*60.0*60.0	//(int)(1.0*60.0/TIMESTEP);	//F�r wie lange er berechnen soll =10s
 //static final double EXACTSTEP = TIMESTEP/Math.powx(10.0, 3.0); //default exactstep 10ms
 #define LACCURACY 1.0E2 //Math.powx(10.0,3.0);	//Genauigkeit der longs: 0=m;2=cm;3=mm;6=nm;9=pm (yes it is 10^+3)
 #define SMASSCONST 21			//constant for mass-slider (nach belieben �nderbar)
 #define SRADIUSCONST 8.4			//constant for radius-slider (nach belieben �nderbar)
 #define SSPEEDCONST 8.0		//constant for speed-slider
 //static final double ZOOM = AE / (1.0E6) //the zoom of the main window
 #define RACCURACY Math.powx(10.0, 3.0) //Genauigkeit des radius-feldes (std: km=1000)
 */
#define UNKNOWNERROR -1
#define NOERROR 0
#define LIGHTSPEEDERROR 1
#define LONGLIMIT 2
#define DOUBLELIMIT 3
#define NANERROR 4

//Nur um sicher zu gehen das alle anderen sourcedateien die globalen variablen der main ebenfalls haben
/*#ifndef MAIN
	extern int error;
	extern bool flagcalc;
#endif*/

#endif
