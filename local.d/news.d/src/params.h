/*
 * params.h - parameters for everyone.
 */

/*	@(#)params.h	2.12	9/3/84	*/

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>

#include "defs.h"

#ifndef UNAME
/*
 * 9 bytes is for compatibility with USG, in case you forget to define UNAME.
 * 33 bytes in nodename because many sites have names longer than 8 chars.
 */
struct utsname {
	char	sysname[9];
	char	nodename[33];
	char	release[9];
	char	version[9];
};
#else
#include <sys/utsname.h>
#endif

#ifndef USG
#include <sys/timeb.h>
#else
struct timeb
{
	time_t	time;
	unsigned short millitm;
	short	timezone;
	short	dstflag;
};
#endif

#include "header.h"

/* line from SUBFILE */
struct	srec {
	char	s_name[BUFLEN];		/* system name		*/
	char	s_nbuf[LBUFLEN];	/* system subscriptions */
	char	s_flags[BUFLEN];	/* system flags		*/
	char	s_xmit[LBUFLEN];	/* system xmit routine	*/
};

extern	int	uid, gid, duid, dgid;
extern	int	savmask, sigtrap, mode, lockcount, defexp;
extern	struct	hbuf header;
extern	char	bfr[LBUFLEN], *username, *userhome;

extern	char	*SPOOL, *LIB, *BIN, *SUBFILE, *ACTIVE;
extern	char	*LOCKFILE, *SEQFILE, *ARTFILE;
extern	char	*news_version;

#ifdef NOTIFY
extern	char	*TELLME;
#endif

extern	char	*FULLSYSNAME;
#ifndef SHELL
extern char	*SHELL;
#endif

/* external function declarations */
extern	FILE	*xfopen(), *hread();
extern	char	*strcpy(), *strncpy(), *strcat(), *index(), *rindex();
extern	char	*ctime(), *mktemp(), *malloc(), *realloc(), *getenv();
extern	char	*arpadate(), *dirname(), *dotname(), *AllocCpy();
extern	struct	passwd *getpwnam(), *getpwuid(), *getpwent();
extern	struct	group *getgrnam();
extern	time_t	time(), getdate(), cgtdate();
extern	int	broadcast(), save(), newssave(), ushell(), pshell(), onsig();
extern	long	atol();
