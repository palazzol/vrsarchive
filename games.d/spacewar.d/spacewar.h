/*
 * Spacewar - include file for general stuff
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */
#ifdef __STDC__
#define STR(x)		#x
#define STRING(x)	STR(X)
#define FILENM(x)	STRING(GAMEDIR) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMEDIR)/x"
#endif

#ifdef VMS
#define VOID
#define delete dbmdelete
#endif /* VMS */

#ifdef BSD
#	define SWPIDFILE	FILENM(swpid)
#	define SWLGNFILE	FILENM(swlgn)
#else /* VMS SYSIII SYSV */
#ifdef VMS
#	define SWCOMFILE	"swmlbx"
#else /* SYSIII SYSV */
#	define SWCOMFILE	FILENM(swcomm)
#endif /* VMS SYSIII SYSV */
#endif /* BSD VMS SYSIII SYSV */

#ifdef VMS
#	define SWDATABASE	"$DISK2:[TSDIR.SW]swdb."
#	define SWGAME		"$DISK2:[TSDIR.SW]sw"
#	define SWREAD		"$DISK2:[TSDIR.SW]vmsrsw"
#	define SWNEWS		"$DISK2:[TSDIR.SW]swnews"
#	define SWERR		"$DISK2:[TSDIR.SW]swerr"
#	define SWOBJ		"$DISK2:[TSDIR.SW]swobj"
#else /* BSD SYSIII SYSV */
#	define SWDATABASE	FILENM(swdb)
#	define SWGAME		FILENM(sw)
#	define SWREAD		FILENM(rsw)
#	define SWNEWS		FILENM(swnews)
#	define SWERR		FILENM(swerr)
#	define SWOBJ		FILENM(swobj) /* see objupdate.c */
#endif /* VMS BSD SYSIII SYSV */

#define SWMASTER	"root"

#define PI		(3.1415926536)
#define TWOPI		(6.2831853072)
#define DEGTORAD	DIV(TWOPI,360.)
#define PROMPT		"\nPlay/See/Info/Complain/Mail/reBuild/Usercmd/Logoff/Who>"
#ifdef NULL
#undef	NULL
#endif
#define	NULL		(0)

/* uninteresting fudge factors */
#define VANGVDST	(3084.2514)		/* makes 45deg match 5000 */

/*
 * all floating-point operations through these macros
 * allowing co-processor routines to be substituted for in-line code
 */
#define ADD(x,y)	((x)+(y))
#define SUB(x,y)	((x)-(y))
#define	MUL(x,y)	((x)*(y))
#define DIV(x,y)	((x)/(y))
#define SQUARE(x)	((x)*(x))
#define CUBE(x)		((x)*(x)*(x))

extern double sqrt();
extern int rand();
#define SQRT(x)		(sqrt(x))
#define RANDOM(x)	(rand()%(x))

extern double atan2(),sin(),cos();
#define ATAN2(x,y)	(atan2((x),(y)))
#define SIN(x)		(sin(x))
#define COS(x)		(cos(x))

extern double fmod();
#define FMOD(x,y)	(fmod((x),(y)))
#define NEG(x)		(-(x))
#define INT(x)		((long)(x))
#define FLOAT(i)	((double)(i))
#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))

#ifdef DEBUG
extern VOID DBG(),VDBG();
#endif

typedef struct {
	double vec[3];
	double dst;
	char valid;
} dsplcmnt;
#define VECVALID 1
#define DSTVALID 2

dsplcmnt vdisp();
