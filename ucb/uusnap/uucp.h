/*
 *	%Z% %M% %I% %D% %Q%
 *
 *	Copyright (C) Microsoft Corporation, 1983
 *
 *	This Module contains Proprietary Information of Microsoft
 *	Corporation and AT&T, and should be treated as Confidential.
 */
/*
 * HCR: RRG-1, 17 June 1981
 *	- Changes for reading system name from /etc/systemid.
 *	M002	06 Nov 83	andyp
 *	- Changed default for MYNAME to something generic; it is initialized
 *	  at runtime, so this should not cause problems.
 */

#include <stdio.h>
	/*  some system names  */
#define MYNAME          "unknown"

/* define UNAME if uname() should be used to get uucpname */
#define UNAME

/* define UUSTAT if you need "uustat" command */
#define UUSTAT
 
/*	define UUSUB if you need "uusub" command */
#define UUSUB

#define THISDIR		"/usr/lib/uucp"
#define SQFILE		"/usr/lib/uucp/SQFILE"
#define SQTMP		"/usr/lib/uucp/SQTMP"
#define SYSFILE		"/usr/lib/uucp/L.sys"
#define SYSFILECR	"/usr/lib/uucp/L.sys.cr"
#define DEVFILE		"/usr/lib/uucp/L-devices"
#define DIALFILE	"/usr/lib/uucp/L-dialcodes"
#define USERFILE	"/usr/lib/uucp/USERFILE"
#define CMDFILE		"/usr/lib/uucp/L.cmds"

#define SPOOL		"/usr/spool/uucp"
#define LOGDIR		"/usr/spool/uucp"
#define SEQFILE		"/usr/spool/uucp/SEQF"
#define SQLOCK		"/usr/spool/uucp/LCK.SQ"
#define SYSLOG		"/usr/spool/uucp/SYSLOG"
#define PUBDIR		"/usr/spool/uucppublic"
#define XQTDIR		"/usr/spool/uucp/.XQTDIR"
#define XFDDIR          "/usr/spool/uucp/.XFDDIR"

#define SEQLOCK		"LCK.SEQL"
#define CMDPRE		'C'
#define DATAPRE		'D'
#define XQTPRE		'X'

#define LOGPREFIX	"LOG."
#define LOGLOCK	"/usr/spool/uucp/LCK.LOG"
#define LOGFILE	"/usr/spool/uucp/LOGFILE"

#define RMTDEBUG	"AUDIT"
#define SQTIME		60L

/* do not allow this in production: only in testing. It prints sensitive stuff*/
#define DEBUG(l, f, s) if (Debug >= l) fprintf(stderr, f, s)

#define ASSERT(e, f, v) if (!(e)) {\
fprintf(stderr, "AERROR - (%s) ", "e");\
fprintf(stderr, f, v);\
fprintf(stderr,"errno=%d\n",errno);\
cleanup(FAIL);};

#define FMV(p, n) close(n); dup(p[n]); close(p[n]);

#define SAME 0
#define FAIL -1
#define SUCCESS 0
#define MASTER 1
#define SLAVE 0
#define MAXFULLNAME 100
#define MAXMSGTIME 45
#define MAXCHARTIME 15
#define NAMESIZE 15
#define ACULAST "-<"
#define EOTMSG "\004\n\004\n"
#define CALLBACK 1
#define LINKLEVEL 100

	/*  commands  */
#define SHELL		"/bin/sh"
#define MAIL		"mail"
#define UUCICO		"/usr/lib/uucp/uucico"
#define UUXQT		"/usr/lib/uucp/uuxqt"
#define UUCP		"usr/lib/uucp/uucp"


	/*  call connect fail stuff  */
#define CF_SYSTEM	-1
#define CF_TIME		-2
#define CF_LOCK		-3
#define CF_DIAL		-5
#define CF_LOGIN	-6

	/*  system status stuff  */
#define SS_OK		0
#define SS_FAIL		4
#define SS_NODEVICE	1
#define SS_CALLBACK	2
#define SS_INPROGRESS	3
#define SS_BADSEQ	5

	/*  fail/retry parameters  */
#define RETRYTIME 600L
#define INPROGTIME 600L
#define MAXRECALLS 30

	/*  stuff for command execution  */
#define X_RQDFILE	'F'
#define X_STDIN		'I'
#define X_STDOUT	'O'
#define X_CMD		'C'
#define X_USER		'U'
#define X_SENDFILE	'S'
#define X_LOCK		"LCK.XQT"
#define X_LOCKTIME	3600L

extern int errno;
int Ifn, Ofn;
char Rmtname[10];
char User[10];
char Loginuser[10];
char *Thisdir;
char *Spool;
extern char Myname[];
extern char *Sysfiles[];
extern char Progname[];
char *Devfile;
char *Dialfile;
int Debug;
int Pkdebug;
int Pkdrvon;
int Bspeed;
extern short Usrf;

	/* the define geteuid should be deleted if the */
	/* routine is available in the libc.a library. */
/*#define geteuid getuid*/

#define WKDSIZE	100	/*  size of work dir name  */
char Wrkdir[WKDSIZE];

	/* This structure tells about a device */
struct Devices {
	char D_type[20];
	char D_line[20];
	char D_calldev[20];
	char D_ascspeed[20];	/* speed field in ascii */
	int D_speed;
	};
struct Devices dev;
