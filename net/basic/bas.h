/*
 * BASIC by Phil Cockcroft
 */
/*
 *      This file contains all the variables and definitions needed by
 *    all the C parts of the interpreter.
 */

/*
 * include the correct include file for the current machine
 */

#ifdef  vax
#include "vax/conf.h"
#endif
#ifdef  pdp11
#include "pdp11/conf.h"
#endif
#ifdef  m68000
#include "m68000/conf.h"
#endif
#ifdef  pyramid
#include "pyramid/conf.h"
#endif

#define MASK            0377
#define SPECIAL         0200            /* top bit set */
#define SYNTAX          1               /* error code */
#define MAXLIN          255             /* maximum length of input line */
#define BUSERR          10              /* bus error */
#define SEGERR          11              /* segmentation violation */
#define DEFAULTSTRING   512             /* default size of string space */
#define VARREQD         2               /* error code */
#define OUTOFSTRINGSPACE 3              /* ditto */
#define NORMAL          0               /* normal return from a command */
#define GTO             1               /* ignore rest of line return */
#define normret return(NORMAL)
#define MAXERR          51              /* maximum value of error code */
#define BADDATA         26              /* error message values */
#define OUTOFDATA       27
#define FUNCT           33
#define FLOATOVER       34
#define INTOVER         35
#define REDEFFN         45
#define UNDEFFN         46
#define CANTCONT        47

#ifdef  LNAMES                          /* if you want long names... */

#define MAXNAME         16              /* maximum size of a name -1 */
#define HSHTABSIZ       37              /* size of initial hash table */
					/* very rule of thumb. */
#endif

/*
 *      values of constants from the symbol table
 */

#define MAXFUNC         0350            /* maximum allowed function code */
#define RND             0343            /* rnd function code */
#define FN              0344
#define MINFUNC         0311
#define MAXSTRING       0307
#define DATE            0310
#define MAXCOMMAND      0272            /* maximum allowed command code */
#define MINSTRING       0271            /* the rest are pretty obvious */
#define DATA            0236
#define QUOTE           0233
#define ERROR           0231
#define GOSUB           0226
#define FOR             0224
#define IF              0221
#define INPUT           0212
#define RUNN            0201
#define REM             0203
#define GOTO            0202
#define WHILE           0257
#define WEND            0260
#define REPEAT          0255
#define UNTIL           0256
#define ELSE            0351
#define THEN            0352
#define ON              0230
#define RESUME          0220
#define RESTORE         0240
#define TABB            0353            /* tab command */
#define STEP            0354
#define TO              0355
#define AS              0365
#define OUTPUT          0366
#define APPEND          0367
#define TERMINAL        0371

/*      logical operators */

#define MODD            0361
#define ANDD            0356
#define ORR             0357
#define XORR            0360
#define NOTT            0370

/*      comparison operators */

#define EQL             '='
#define LTEQ            0362
#define NEQE            0363
#define LTTH            '<'
#define GTEQ            0364
#define GRTH            '>'

/*      values used for file maintainance */

#define _READ           01
#define _WRITE          02
#define _EOF            04
#define _TERMINAL       010

/*
   N.B. The value of this (_BLOCKED) controls wether the blockmode file stuff
	is included. ( comment this constant out if don't want it).
*/
#define _BLOCKED        020

#define MAXFILES        9

#define ESCAPE		'\033'

/*      definitions of some simple functions */
/*      isletter()      - true if character is a letter */
/*      isnumber()      - true if character is a number */
/*      istermin()      - true if character is a terminator */

#define isletter(c)  ((c)>='a' && (c)<='z')
#define isnumber(c)  ((c)>='0' && (c)<='9')
#define istermin(c)  (!(c)|| (c)==':' ||((char)(c)==(char)ELSE && elsecount))

/*      define the offset to the next line */

#define lenv(p)      ((p)->llen)

typedef struct  olin    *lpoint;        /* typedef for pointer to a line */
typedef struct  deffn   *deffnp;        /* pointer to a function definition */
typedef struct  filebuf *filebufp;      /* pointer to a filebuffer */
typedef struct  forst   *forstp;        /* pointer to a for block */
typedef struct  strarr  *strarrp;       /* pointer to an array header */
typedef struct  vardata *vardp;         /* pointer to a variable */
typedef struct  stdata  *stdatap;       /* pointer to a string header */
typedef char    *memp;                  /* a memory pointer */

/*      typedef fo the standard dual type of variable */

typedef union {
		short   i;
		double  f;
	   } value;

/*      declarations to stop the C compiler complaining */

filebufp getf();
lpoint  getline();
memp    xpand(),getname();
char    *printlin(),*strcpy(),*grow(),*getenv();

int     rnd(),ffn(),pii(),erlin(),erval(),tim();
int     sgn(),len(),abs(),val(),ascval(),instr(),eofl(),fposn(),sqrtf(),
	logf(),expf(),evalu(),intf(),peekf(),sinf(),cosf(),atanf(),
	mkint(),mkdouble(), ssystem();
int     midst(),rightst(),leftst(),strng(),estrng(),chrstr(),nstrng(),
	space(),getstf(),mkistr(),mkdstr();
int     endd(),runn(),gotos(),rem(),lets(),list(),
	print(),stop(),delete(),editl(),input(),clearl(),
	save(),old(),neww(),shell(),resume(),iff(),
	random(),dimensio(),forr(),next(),gosub(),retn(),
	onn(),doerror(),print(),rem(),dauto(),
	readd(),dodata(),cls(),restore(),base(),fopen(),
	fclosef(),merge(),quit(),chain(),deffunc(),cont(),lhmidst(),
	linput(),poke(),rept(),untilf(),whilef(),wendf(),fseek(),renumb(),
	dump(),loadd();

/*      all structures must have an exact multiple of the size of an int
 *    to the start of the next structure
 */

struct  stdata  {               /* data for the string pointer */
	unsigned snam;          /* getname() will return the address */
	char    *stpt;          /* of this structure for a string access */
	};

struct  vardata {               /* storage of a standard non-indexed */
	unsigned nam;           /* variable */
	value   dt;
	};

typedef unsigned xlinnumb;      /* the type of linnumbers */

struct olin{                    /* structure for a line */
	unsigned linnumb;
	unsigned llen;
	char     lin[1];
	};

struct  strarr {                /* structure for an array */
	unsigned snm;           /* name */
	int     hash;           /* index to the next array or the start */
	short   dimens;         /* of the special numbers */
	short   dim[3];         /* the dimensions */
	};


struct  forst {                 /* for / gosub stack */
	char    *fnnm;          /* pointer to variable - relative to earray */
	char    fr,elses;       /* type of structure , elsecount on return */
	value   final;          /* the start and end values */
	value   step;
	lpoint  stolin;         /* pointer to return start of line */
	char    *pt;            /* return value for point */
	};

#ifdef  LNAMES

struct  entry   {               /* the structure for a long name storage */
	struct  entry   *link;
	int     ln_hash;        /* hash value of entry */
	char    _name[MAXNAME];
	};

#endif

#ifdef  V7

#include        <setjmp.h>
#include        <signal.h>
#include        <sys/types.h>
#include        <sys/stat.h>

#define setexit()       setjmp(rcall)
#define reset()         longjmp(rcall,0)

#else

struct  stat    {
	short   st_dev;
	short   st_ino;
	short   st_mode;
	int     _stat[15];
	};

#define _exit(x)        exit(x)

int     (*signal())();
#define SIGINT  2
#define SIGQUIT 3
#define SIGFPE  8
#define SIG_IGN ((int(*)())1)
#define SIG_DFL ((int(*)())0)
#define NSIG    16

#endif

#ifndef pdp11           /* don't need it on a VAX system */
#define checksp()       /* nothing */
#endif

struct  filebuf {               /* the file buffer structure */
	short   filedes;        /* system file descriptor */
	short   userfiledes;    /* user name */
	int     posn;           /* cursor / read positon */
#ifdef  _BLOCKED
	short   blocksiz;       /* if want block mode files */
#endif
	short   inodnumber;     /* to stop people reading and writing */
	short   device;         /* to the same file at the same time */
	short   use;            /* flags */
	short   nleft;          /* number of characters in buffer */
	char    buf[BLOCKSIZ];  /* the buffer itself */
	};

struct tabl {                   /* structure for symbol table */
	char    *string;
	int     chval;
	};

struct  deffn  {                /* structure for a user definable function */
	int     dnm;
	int     offs;
	char    narg;
	char    vtys;
	short   vargs[3];
	char    exp[1];
	};

#ifndef SOFTFP

#define fadd(p,q)       ((q)->f += (p)->f)
#define fsub(p,q)       ((q)->f = (p)->f - (q)->f)
#define fmul(p,q)       ((q)->f *= (p)->f)
#define fdiv(p,q)       ((q)->f = (p)->f / (q)->f)

#define conv(p) \
	( ((p)->f > MAXint || (p)->f < MINint) ? 1 : ( ((p)->i = (p)->f), 0) )

#define cvt(p)  (p)->f = (p)->i

#endif

/*
 * On pdp11's and VAXen the loader is clever about global bss symbols
 * On 68000's this is not true so we have to define the memory pointers
 * to be members of an array.
 */
#ifdef  MPORTABLE
#define estring _space_[0]
#ifdef  LNAMES
#define enames  _space_[1]
#define edefns  _space_[2]
#define estarr  _space_[3]
#define earray  _space_[4]
#define vend    _space_[5]
#define bstk    _space_[6]
#define vvend   _space_[7]
#else
#define edefns  _space_[1]
#define estarr  _space_[2]
#define earray  _space_[3]
#define vend    _space_[4]
#define bstk    _space_[5]
#define vvend   _space_[6]
#endif

#endif


/*
 *      PART1 is declared only once and so allocates storage for the
 *    variables only once , otherwise the definiton for the variables
 *    ( in all source files except bas1.c ). is declared as external.
 */

#ifdef  PART1

int     baseval=1;              /* value of the initial base for arrays */
char    nl[]="\n";              /* a new_line character */
char    line[MAXLIN+2];         /* the input line */
char    nline[MAXLIN];         /* the array used to store the compiled line */
unsigned linenumber;            /* linenumber form compile */

/*  pointers to the various sections of the memory map */

memp    filestart;      /* end of bss , start of file buffers */
memp    fendcore;       /* end of buffers , start of text */
memp    ecore;          /* end of text , start of string space */
memp    eostring;       /* end of full strings */
memp    estdt;          /* start of string header blocks */

/* all these pointers below must be defined in this order so that xpand
 * will be able to increment them all */

#ifndef MPORTABLE
memp    estring;        /* end of strings , start of func defs */
#ifdef  LNAMES
memp    enames;         /* end of symbol table. start of def fncs */
#endif
memp    edefns;         /* end of def fncs , start of arrays */
memp    estarr;         /* end of string array structures */
memp    earray;         /* end of arrays , start of simple variables */
memp    vend;           /* end of simple variables , start of gosub stack */
memp    bstk;
memp    vvend;          /* end of stack , top of memory */
#else
memp    _space_[8];     /* for use in portable systems */
#endif

/* up to this point */

int     cursor;         /* position of cursor on line */
unsigned shash;         /* starting value for string arrays */
int     mcore();        /* trap functions- keep compiler happy */
int     seger();
int     trap();
lpoint  stocurlin;      /* start of current line */
unsigned curline;       /* current line number */
int     readfile;       /* input file , file descriptor */
char    *point;         /* pointer to current location */
char    *savepoint;     /* value of point at start of current command */
char    elsecount;      /* flag for enabling ELSEs as terminators */
char    vartype;        /* current type of variable */
char    runmode;        /* run or immeadiate mode */
char    ertrap;         /* are about to call the error trapping routine */
char    intrap;         /* we are in the error trapping routine */
char    trapped;        /* cntrl-c trap has occured */
char    inserted;       /* the line table has been changed, clear variables */
char    eelsecount;     /* variables to save the current state after an */
lpoint  estocurlin;     /* error */
unsigned elinnumb;      /* ditto */
char    *epoint;        /* ditto */
int     ecode;          /* error code */
lpoint  errortrap;      /* error trap pointer */
lpoint  saveertrap;     /* error trap save location - during trap  */
lpoint  datastolin;     /* pointer to start of current data line */
char    *datapoint;     /* pointer into current data line */
int     evallock;       /* lock to stop recursive eval function */
unsigned autostart=10;  /* values for auto command */
unsigned autoincr=10;
int     ter_width;      /* set from the terms system call */

lpoint  constolin;      /* values for 'cont' */
unsigned concurlin;
lpoint  conerp;
char    *conpoint;
char    contelse;
char    contpos;
char    cancont;
char    noedit;         /* set if noediting is to be done */

int     pipes[2];       /* pipe structure for chain */

long    overfl;         /* value of overflowed integers, converting to real */

value   res;            /* global variable for maths function */

double  pivalue= 3.14159265358979323846;        /* value of pi */
#ifndef SOFTFP
double  MAXint= 32767;                          /* for cvt */
double  MINint= -32768;
#endif

#ifdef  V7
jmp_buf rcall;
#endif
#ifdef  BSD42
jmp_buf ecall;                  /* for use of cntrl-c in edit */
char    ecalling;
#endif
				/* one edit mode , one for normal mode */
int     nm;                     /* name of variable being accessed */

#ifdef  LNAMES
char    nam[MAXNAME];                   /* local array for long names */
struct  entry   *hshtab[HSHTABSIZ];     /* hash table pointers */
int     varshash[HSHTABSIZ];            /* hashing for variables */
int     chained;                /* force full search only after a chain() */
#endif

char    gblock[256];            /* global place for string functions */
int     gcursiz;                /* size of string in gblock[] */

/*
 *      definition of the command , function and string function 'jump'
 *    tables.
 */

/*      maths functions that do not want an argument */

int     (*functs[])()= {
	rnd,ffn, pii, erlin, erval, tim,
	};

/*      other maths functions */

int     (*functb[])()={
	sgn, len, abs, val, ascval, instr, eofl, fposn, sqrtf, logf, expf,
	evalu,intf,peekf,sinf,cosf,atanf,mkint,mkdouble, ssystem,
	};

/*      string function , N.B. date$ is not here. */

int     (*strngcommand[])()= {
	midst, rightst, leftst, strng, estrng, chrstr, nstrng, space, getstf,
	mkistr,mkdstr,
	};

/*      commands */

int     (*commandf[])()= {
	endd,runn,gotos,rem,list,lets,print,stop,delete,editl,input,clearl,
	save,old,neww,shell,resume,iff,random,dimensio,forr,next,gosub,retn,
	onn,doerror,print,rem,dauto,readd,dodata,cls,restore,base,fopen,
	fclosef,merge,quit,quit,quit,chain,deffunc,cont,poke,linput,rept,
	untilf,whilef,wendf,fseek,renumb,loadd,dump,0,0,0,0,lhmidst,
	};

/*      table of error messages */

char    *ermesg[]= {
	"syntax error",
	"variable required",
	"out of string space",
	"assignment '=' required",
	"line number required",
	"undefined line number",
	"line number overflow",
	"illegal command",
	"string overflow",
	"illegal string size",
	"illegal function",
	"illegal core size",
	"illegal edit",
	"cannot creat file",
	"cannot open file",
	"dimension error",
	"subscript error",
	"next without for",
	"undefined array",
	"redimension error",
	"gosub / return error",
	"illegal error code",
	"bad load",
	"out of core",
	"zero divisor error",
	"bad data",
	"out of data",
	"bad base",
	"bad file descriptor",
	"unexpected eof",
	"out of files",
	"line length overflow",
	"argument error",
	"floating point overflow",
	"integer overflow",
	"bad number",
	"negative square root",
	"negative or zero log",
	"overflow in exp",
	"overflow in power",
	"negative power",
	"no space for chaining",
	"mutually recursive eval",
	"expression too complex",
	"illegal redefinition",
	"undefined user function",
	"can't continue",
	"until without repeat",
	"wend without while",
	"no wend statement found",
	"illegal loop nesting",
	};

/*      tokenising table */

struct  tabl    table[]={
	"end",0200,             /* commands 0200 - 0300 */
	"run",0201,
	"goto",0202,
	"rem",0203,
	"list",0204,
	"let",0205,
	"print",0206,
	"stop",0207,
	"delete",0210,
	"edit",0211,
	"input",0212,
	"clear",0213,
	"save",0214,
	"old",0215,
	"new",0216,
	"shell",0217,
	"resume",0220,
	"if",0221,
	"random",0222,
	"dim",0223,
	"for",0224,
	"next",0225,
	"gosub",0226,
	"return",0227,
	"on",0230,
	"error",0231,
	"?",0232,
	"'",0233,
	"auto",0234,
	"read",0235,
	"data",0236,
	"cls",0237,
	"restore",0240,
	"base",0241,
	"open",0242,
	"close",0243,
	"merge",0244,
	"quit",0245,
	"bye",0246,
	"exit",0247,
	"chain",0250,
	"def",0251,
	"cont",0252,
	"poke",0253,
	"linput",0254,
	"repeat",0255,
	"until",0256,
	"while",0257,
	"wend",0260,
	"seek",0261,
#ifdef  RENUMB
	"renumber",0262,
#endif
	"load",0263,
	"dump",0264,
	"mid$",0271,            /* string functions 0271 - 0310 */
	"right$",0272,
	"left$",0273,
	"string$",0274,
	"ermsg$",0275,
	"chr$",0276,
	"str$",0277,
	"space$",0300,
	"get$",0301,
#ifdef  _BLOCKED
	"mkis$",0302,
	"mkds$",0303,
#endif
	"date$",0310,           /* date must be last string funct */
	"sgn",0311,             /* maths functions 0311 - 0350 */
	"len",0312,
	"abs",0313,
	"val",0314,
	"asc",0315,
	"instr",0316,
	"eof",0317,
	"posn",0320,
	"sqrt",0321,
	"log",0322,
	"exp",0323,
	"eval",0324,
	"int",0325,
	"peek",0326,
	"sin",0327,
	"cos",0330,
	"atan",0331,
#ifdef  _BLOCKED
	"mksi",0332,
	"mksd",0333,
#endif
	"system", 0334,
	"rnd",0343,
	"fn",0344,
	"pi",0345,
	"erl",0346,
	"err",0347,
	"tim",0350,
	"else",0351,            /* seperators and others 0351 - 0377 */
	"then",0352,
	"tab",0353,
	"step",0354,
	"to",0355,
	"and",0356,
	"or",0357,
	"xor",0360,
	"mod",0361,
	"<=",0362,
	"<>",0363,
	">=",0364,
	"as",0365,
	"output",0366,
	"append",0367,
	"not",0370,
	"terminal",0371,
	0,0
	};

#else

/*   definition of variables for other source files */

extern  int     baseval;
extern  char    nl[];
extern  char    line[];
extern  char    nline[];
extern  unsigned linenumber;
extern  memp    fendcore;
#ifndef MPORTABLE
extern  memp    estring,edefns,estarr,earray,vend,bstk,vvend;
#else
extern  memp    _space_[];
#endif
extern  memp    filestart;
extern  memp    ecore,eostring,estdt;
extern  int     cursor;
extern  unsigned shash;
extern  int     mcore(),seger(),trap();
extern  lpoint  stocurlin;
extern  unsigned curline;
extern  int     readfile;
extern  char    *point;
extern  char    *savepoint;
extern  char    elsecount;
extern  char    vartype;
extern  char    runmode;
extern  char    ertrap;
extern  char    intrap;
extern  char    trapped;
extern  char    inserted;
extern  char    eelsecount;
extern  lpoint  estocurlin;
extern  unsigned elinnumb;
extern  char    *epoint;
extern  int     ecode;
extern  lpoint  errortrap;
extern  lpoint  saveertrap;
extern  lpoint  datastolin;
extern  char    *datapoint;
extern  int     evallock;
extern  unsigned autostart;
extern  unsigned autoincr;
extern  int     ter_width;
extern  lpoint  constolin;
extern  unsigned concurlin;
extern  lpoint  conerp;
extern  char    *conpoint;
extern  char    contelse;
extern  char    contpos;
extern  char    cancont;
extern  char    noedit;

extern  int     pipes[];

extern  long    overfl;
extern  value   res;

extern  double  pivalue;
extern  double  MAXint,MINint;
#ifdef  V7
extern  jmp_buf rcall;
#endif

#ifdef  BSD42
extern  jmp_buf ecall;
extern  char    ecalling;
#endif

extern  int     nm;

#ifdef  LNAMES
extern  struct  entry   *hshtab[];
extern  char    nam[];
extern  int     varshash[];
extern  int     chained;
#ifndef MPORTABLE
extern  memp    enames;
#endif
#endif

extern  char    gblock[];
extern  int     gcursiz;

extern  (*functs[])();
extern  (*functb[])();
extern  (*strngcommand[])();
extern  (*commandf[])();
extern  char    *ermesg[];
extern  struct  tabl    table[];

#endif
