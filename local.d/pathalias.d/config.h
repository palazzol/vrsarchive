/* pathalias -- by steve bellovin, as told to peter honeyman */

/* use strchr (strrchr), not index (rindex) -- probably system v */
#define STRCHR /* */

/* uname() -- probably system v or 8th ed. */
/* #define UNAME /* */

/* memset() -- probably system v or 8th ed. */
/* #define MEMSET /* */

/* gethostname() -- probably 4.2bsd */
/* #define GETHOSTNAME	/* */

/* bzero() -- probably 4.2bsd */
/* #define BZERO /* */

/* default place for dbm output of makedb; can use -o file at run-time  */
#ifndef ALIASDB
#define	ALIASDB	"/usr/local/lib/palias"
#endif

/*
 * after much profiling, i finally found a decent malloc/free
 * remove the next line if you disagree
 */
/* #define MYMALLOC	/**/

/*
 * how many trailing 0's needed in a pointer?
 *
 * vax doesn't care, but setting ALIGN to 2 saves about 5% in time, at
 * the expense of about 2% in space.  why bother?
 *
 * i am told that the 68000 and 3b20 want ALIGN to be 1.
 *
 * perkin-elmer 3220 wants ALIGN to be 2. 
 */
#define ALIGN 0

/****************************************/
/*	END OF CONFIGURATION SECTION	*/
/*		EDIT NO MORE		*/
/****************************************/
#ifdef MAIN
#ifndef lint
static char	*c_sccsid = "@(#)config.h	7.1 (down!honey) 85/08/06";
#endif /*lint*/
#endif /*MAIN*/

#ifdef MYMALLOC

#define malloc mymalloc
#define free myfree
char	*sbrk(), *mymalloc();

#ifdef ALIGN

#if ALIGN == 0
#undef ALIGN
#endif /*ALIGN == 0*/

#endif /*ALIGN*/

#ifndef ALIGN
#define memget sbrk
#else /*ALIGN*/
char	*memget();
#endif /*ALIGN*/

#endif /*MYMALLOC*/

#ifdef STRCHR
#define index strchr
#define rindex strrchr
#endif /*STRCHR*/

#ifdef BZERO
#define strclear(s, n)	((void) bzero((s), (n)))
#else /*!BZERO*/
#	ifdef MEMSET
char	*memset();
#	define strclear(s, n)	((void) memset((s), 0, (n)))
#	else /*!MEMSET*/
	void strclear();
#	endif /*MEMSET*/
#endif /*BZERO*/

long	atol();
char	*malloc();
char	*index();
char	*rindex();
FILE	*popen();
char	*strcpy();
