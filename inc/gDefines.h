#ifndef _INC_GRAVITY_DEFINES
#define _INC_GRAVITY_DEFINES

/* Debug */
#ifdef _DEBUG
 #define DEBUG 40
#endif

/* Versions */
#define BVERSION 1.0
#define FVERSION 1.9
#define WPTVERSION 2

/* Command line arguments */
#define FLAG_DEBUG1 "--version"
#define FLAG_DEBUG2 "-v"

#define FLAG_HELP1 "--help"
#define FLAG_HELP2 "-h"

#define FLAG_TIME1 "--timestep"
#define FLAG_TIME2 "-t"

#define FLAG_NOCOLL1	"--disable-collision"
#define	FLAG_NOCOLL2	"-C"

#define	FLAG_QUIET1	"--quiet"
#define	FLAG_QUIET2	"-q"

/* General definitions */
#define DATAPRECISION 20
#define DELIMDATA ";"

#define FILE_PERCENT "percent.tmp"

#if defined _WIN32 || defined _WIN64
 #define DELIMLINE "\n"
 #define LINEDELIMNUM	0	//this comes from the fact, that windows recognizes "CR LN" as _one_ linebreak
#else
 //#define DELIMLINE "\x0A\x0D"	//hexcode for "\r\n"
 #define DELIMLINE "\r\n"
 #define LINEDELIMNUM	1	//this comes from the fact, that unix recognizes "CR LN" as one linebreak + one character
#endif

/* isnan - in case cmath has none */
#ifndef isnan
 #include <cmath>
 #ifndef isnan
  #define isnan(x) ((x) != (x))
 #endif
#endif

/* Natural constants */
#define LIGHTSPEED 299792458.0 	//Lichtgeschwindigkeit in m/s
#define GRAVCONST 6.673E-11		//Gravitationskonstante
#define AE 1.495979E11 	//Astronomische Einheit (Längeneinheit) in m
#define LY 9.461E15		//Lichtjahr in m
#define PI 3.141592654; //TODO new PI requ

/* Limits */
#if !defined LDBL_MAX || !defined LDBL_MIN
 #include <float.h>
 #ifndef LDBL_MAX
  #define LDBL_MAX DBL_MAX	//TODO: check if there is something nicer out there
 #endif
 #ifndef LDBL_MIN
  #define LDBL_MIN DBL_MIN
 #endif
#endif

#if !defined LLONG_MAX || !defined LLONG_MIN
 #include <limits.h>
 #ifndef LLONG_MAX
  #define LLONG_MAX LONG_MAX*LONG_MAX
 #endif
 #ifndef LLONG_MIN
  #define LLONG_MIN -LLONG_MAX-1
 #endif
#endif

/* Accuracy of calculation */
#define LACCURACY 1.0E2

/* Error handling */
#define ERROR_NONE 0

#define ERROR_MAX	4
#define ERROR_UNKNOWN	0
#define ERROR_FILE_IN	1
#define ERROR_FILE_OUT	2
#define ERROR_CALC	3

#define ERROR_CALC_MAX		7
#define ERROR_CALC_UNKNOWN	0	/* an unknown/not specified error occured */
#define ERROR_CALC_DATA		1	/* only one object left or bad radius/mass values */
#define ERROR_CALC_LIGHTSPEED	2	/* object got a speed > c during calculations */
#define ERROR_CALC_LIMIT_LONG	3	/* the coordination of an object reached the LLONG limit */
#define ERROR_CALC_LIMIT_DOUBLE	4	/* reached LDBL_MIN limit during auto timestep correction */
#define ERROR_CALC_NAN		5	/* calculation of force returned nan */
#define ERROR_CALC_GRT		6	/* two objects got a distance less than 2*Schwarzschild radius */

#define ERROR_TO_BITSET(x) (unsigned long)(ldexp(1.0f, x))

/* OpenMP support */
#ifdef _OPENMP
 #define FORKILLER continue
#else
 #define FORKILLER break
#endif

#endif
