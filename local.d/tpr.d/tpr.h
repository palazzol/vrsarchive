#include	<stdio.h>
#include	<ctype.h>

#define	min(a,b)	(a<b?a:b)
#define	max(a,b)	(a>b?a:b)
#define	skipbl(p)	{while(*p == ' ' || *p == '\t')p++;}
#define	skipnbl(p)	{while(*p != ' ' && *p != '\t' && *p != '\n')p++;}
#define	CHARNULL	((char *)NULL)
#define	NO		0
#define	YES		1
#define	COMMAND		'.'
#define	PAGENUM		'%'
#define	HUGE		10000
#define	MAXIN		256
#define	MAXOUT		256
#define	MAXFN		64	/* length of included filename */
#define	MAXCHARS	14
#define	MAXMAC		50
#define	MAXPB		50
#define	MAXTABS		20
#define	PAGLEN		66
#define	PAPERSIZE	65
#define	M1DEF		3
#define	M2DEF		1
#define	M3DEF		1
#define	M4DEF		3
#define	PAGEWIDTH	60
#define	ARABIC		0
#define	ROMAN		1
#define	ENGLISH		2

struct linelink{
	char *lineptr;
	struct linelink *lastline;
	};
struct	macro{
	char macnam[3];
	struct linelink *macend;
	} macros[MAXMAC];
struct	envir{
	short plval;
	short m1val;
	short m2val;
	short m3val;
	short m4val;
	short bottom;
	char *evenhdr,*oddhdr;
	char *evenftr,*oddftr;
	char comchr;
	char tabchr;
	char ubchr;
	short fill;
	short adjust;
	short numtyp;
	short lsval;
	short llval;
	short inval;
	short tival;
	short poval;
	short ceval;
	short ulval;
	short bdval;
	short litval;
	short blval;
	short skpval;
	short tabpos[MAXTABS];
	struct envir *lastenv;
	};
struct cmdents{
	char cmdname[3];
	short notredefd;
	};
enum cmdnum	{ADJ, ARA, BLD, BLN, BPG, BRE, CMC, CEN, DFN, EFO, ENG, EHD,
		FIL, FOT, HED, IND, INX, LIT, LNL, LNS, M1, M2, M3, M4, NAD,
		NED, NFL, OFO, OHD, PGL, POF, PGT, RNV, REF, ROM, RPG, SNV, SKP,
		SOU, SPA, TCL, TCH, TMI, UBC, UDL, MAC, UNKNOWN};
extern	struct macro macros[MAXMAC];
extern	short maccnt;
extern	char *pbptr[MAXMAC];
extern	short pblev;
extern	char outbuf[MAXOUT];
extern	char *outp;
extern	short outw;
extern	short outwds;
extern	short pages;
extern	short pausecount;
extern	short curpag;
extern	short newpag;
extern	short lineno;
extern	short peekno;
extern	short indline;
extern	short respage;
extern	char trapmac[];
extern	char blnkhdr[];
extern	struct envir env;
extern	struct envir *curenv;
extern	struct cmdents builtins[];
extern	short echodir;
extern	char *progname;
extern	char *filename;
extern	short fileline;
extern	short ttyfd;
extern	FILE *indfp;
extern	char *nomem;
