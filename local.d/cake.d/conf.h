/*
**	Cake configuration parameters.
**
**	$Header: /home/Vince/cvs/local.d/cake.d/conf.h,v 1.1 1987-11-16 17:19:31 vrs Exp $
*/

/* type used when type is not known */
typedef	int	Cast;

/* location of the systemwide cake library */
#define	SLIB		"/u/pgrad/zs/lib/cake"

/* suffix (after $HOME) of personal libraries */
#define	ULIB		"/lib/cake"

/* command to invoke the C preprocessor */
#define	CPP		"/lib/cpp"

/* location of the statistics file - if not defined, no stats kept */
#define	STATS_FILE	"/u/pgrad/zs/lib/cake_stats"

/* characters always requiring shell attention */
#define	METACHARS	"*?!&|;<>()[]{}'`\"%$~#"

/* default command to execute non-script shell actions */
#define	SYSTEM_CMD	"/bin/csh -cf"

/* default command to execute shell scripts */
#define	SCRIPT_CMD	"/bin/csh -f"

/*	System V compatibility			*/
#ifdef	ATT
/* the resolution of the times(2) system call */
#ifdef	exlsi
#define	TIMERES		100
#else
#define	TIMERES		60
#endif

#include		"port.h"

#define	vfork()		fork()
#define	index(s, c)	strchr(s, c)
#define	rindex(s, c)	strrchr(s, c)
#endif

#define	MAXARGS		64
#define	MAXGSTACK	128
#define	MAXARGSIZE	128
#define	MAXPATSIZE	512
#define	MAXLEXBUF	2048
#define	MAXSIZE		2048
#define	MAXSCRIPT	16384
#define	MAXNEST		8
#define	SIZE		97		/* size of tables */
#define	CHARSETSIZE	256
#define	GENESIS		(time_t) 42	/* something distinctive */
#define	CHASEROOT	"!MAINCAKE!"
