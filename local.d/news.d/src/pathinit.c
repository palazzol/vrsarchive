/*
 * This function initializes all the strings used for the various
 * filenames.  They cannot be compiled into the program, since that
 * would be non-portable.  With this convention, the netnews sub-system
 * can be owned by any non-priviledged user.  It is also possible
 * to work when the administration randomly moves users from one
 * filesystem to another.  The convention is that a particular user
 * (HOME, see Makefile) is searched for in /etc/passwd and all files
 * are presumed relative to there.  This method also allows one copy
 * of the object code to be used on ANY machine.  (this code runs
 * un-modified on 50+ machines at IH!!)
 *
 * The disadvantage to using this method is that all netnews programs
 * (inews, readnews, rnews, checknews) must first search /etc/passwd
 * before they can start up.  This can cause significant overhead if
 * you have a big password file.
 *
 * Some games are played with ifdefs to get three .o files out of this
 * one source file.  INEW is defined for inews, READ for readnews,
 * and CHKN for checknews.
 */

#ifndef lint
static char	*SccsId = "@(#)pathinit.c	1.13	9/3/84";
#endif !lint

#ifdef INEW
#include	"iparams.h"
#endif INEW

#ifdef READ
#include	"rparams.h"
#endif READ

#if CHKN || EXP
#include <stdio.h>
#endif CHKN

char *FULLSYSNAME, *SPOOL, *LIB, *BIN, *ACTIVE, *OLDNEWS, *SUBFILE, *ARTFILE,
	*MAILPARSER, *LOCKFILE, *SEQFILE, *ARTICLE, *INFILE, *ALIASES,
	*TELLME, *username, *userhome;

extern char bfr[];

struct passwd *getpwnam();
char *rindex();

#define Sprintf(where,fmt,arg)	sprintf(bfr,fmt,arg); where = AllocCpy(bfr)

char *
AllocCpy(cp)
register char *cp;
{
	register char *mp;
	char *malloc();

	mp = malloc(strlen(cp)+1);

	if (mp == NULL)
		xerror("malloc failed on %s",cp);

	strcpy(mp, cp);
	return mp;
}

pathinit()
{
	FILE	*nfd;		/* notify file descriptor		*/
#ifndef ROOTID
	struct passwd	*pw;	/* struct for pw lookup			*/
#endif !ROOTID
#ifdef EXP
	char *p;
#endif EXP
#if INEW || READ
	struct utsname ubuf;

	uname(&ubuf);
	FULLSYSNAME = AllocCpy(ubuf.nodename);
#endif INEW || READ

#ifdef HOME
	/* Relative to the home directory of user HOME */
	sprintf(bfr, "%s/%s", logdir(HOME), SPOOLDIR);
	SPOOL = AllocCpy(bfr);
	sprintf(bfr, "%s/%s", logdir(HOME), LIBDIR);
	LIB = AllocCpy(bfr);
#else !HOME
	/* Fixed paths defined in Makefile */
	SPOOL = AllocCpy(SPOOLDIR);
	LIB = AllocCpy(LIBDIR);
#endif !HOME

#ifdef IHCC
	sprintf(bfr, "%s/%s", logdir(HOME), BINDIR);
	BIN = AllocCpy(bfr);
#else !IHCC
	Sprintf(BIN, "%s", BINDIR);
#endif !IHCC

	Sprintf(ACTIVE, "%s/active", LIB);

#ifdef EXP
	strcpy(bfr, SPOOL);
	p = rindex(bfr, '/');
	if (p) {
		strcpy(++p, "oldnews");
		OLDNEWS = AllocCpy(bfr);
	} else
		OLDNEWS = AllocCpy("oldnews");
#endif EXP

#if INEW || READ || EXP
	Sprintf(SUBFILE, "%s/sys", LIB);
	Sprintf(ARTFILE, "%s/history", LIB);
# endif INEW || READ

# ifdef READ
#ifdef SENDMAIL
	MAILPARSER = AllocCpy(SENDMAIL);
#else !SENDMAIL
	Sprintf(MAILPARSER, "%s/recmail", LIB);
#endif !SENDMAIL
# endif READ

# ifdef INEW
	Sprintf(LOCKFILE, "%s/LOCK", LIB);
	Sprintf(SEQFILE, "%s/seq", LIB);
	Sprintf(ARTICLE, "%s/.arXXXXXX", SPOOL);
	Sprintf(INFILE, "%s/.inXXXXXX", SPOOL);
	Sprintf(ALIASES, "%s/aliases", LIB);
/*
 * The person notified by the netnews sub-system.  Again, no name is
 * compiled in, but instead the information is taken from a file.
 * If the file does not exist, a "default" person will get the mail.
 * If the file exists, but is empty, nobody will get the mail.  This
 * may seem backwards, but is a better fail-safe.
 */
# ifdef NOTIFY
	sprintf(bfr, "%s/notify", LIB);
	nfd = fopen(bfr, "r");
	if (nfd != NULL) {
		bfr[0] = '\0';
		fscanf(nfd, "%s", bfr);
		TELLME = AllocCpy(bfr);
		fclose(nfd);
	} else
		TELLME = AllocCpy(NOTIFY);
# endif NOTIFY

/*
 * Since the netnews owner's id number is different on different
 * systems, we'll extract it from the /etc/passwd file.  If no entry,
 * default to root.  This id number seems to only be used to control who
 * can input certain control messages or cancel any message.  Note that
 * entry is the name from the "notify" file as found above if possible.
 * Defining ROOTID in defs.h hardwires in a number and avoids
 * another search of /etc/passwd.
 */
# ifndef ROOTID
	if ((pw = getpwnam(TELLME)) != NULL)
		ROOTID =  pw->pw_uid;
	else if ((pw = getpwnam(HOME)) != NULL)
		ROOTID =  pw->pw_uid;
	else
		ROOTID = 0;		/* nobody left, let only root */
# endif ROOTID
#endif INEW
}
