/*
 * funcs - functions used by many programs
 */

#ifndef lint
static char	*SccsId = "@(#)funcs.c	2.19	8/28/84";
#endif !lint

#include "params.h"
#include <errno.h>
#if defined(USG) || defined(BSD4_2) || defined(BSD4_1C)
#include <fcntl.h>
#endif !v7
#include <stdarg.h>
extern char *Progname;

/*
 * Append NGDELIM to string.
 */
ngcat(s)
register char *s;
{
	if (*s) {
		while (*s++)
			;
		s -= 2;
		if (*s++ == NGDELIM)
			return;
	}
	*s++ = NGDELIM;
	*s = '\0';
}

/*
 * News group matching.
 *
 * nglist is a list of newsgroups.
 * sublist is a list of subscriptions.
 * sublist may have "meta newsgroups" in it.
 * All fields are NGDELIM separated,
 * and there is an NGDELIM at the end of each argument.
 *
 * Currently implemented glitches:
 * sublist uses 'all' like shell uses '*', and '.' like shell '/'.
 * If subscription X matches Y, it also matches Y.anything.
 */
ngmatch(nglist, sublist)
register char *nglist, *sublist;
{
	register char *n, *s;
	register int rc;

	rc = FALSE;
	for (n = nglist; *n != '\0' && rc == FALSE;) {
		for (s = sublist; *s != '\0';) {
			if (*s != NEGCHAR)
				rc |= ptrncmp(s, n);
			else
				rc &= ~ptrncmp(s+1, n);
			while (*s++ != NGDELIM && *s != '\0')
				;
		}
		while (*n++ != NGDELIM && *n != '\0')
			;
	}
	return(rc);
}

/*
 * Compare two newsgroups for equality.
 * The first one may be a "meta" newsgroup.
 */
ptrncmp(ng1, ng2)
register char *ng1, *ng2;
{
	while (*ng1 != NGDELIM && *ng1 != '\0') {
		if (ng1[0]=='a' && ng1[1]=='l' && ng1[2]=='l') {
			ng1 += 3;
			while (*ng2 != NGDELIM && *ng2 != '.' && *ng2 != '\0')
				if (ptrncmp(ng1, ng2++))
					return(TRUE);
			return (ptrncmp(ng1, ng2));
		} else if (*ng1++ != *ng2++)
			return(FALSE);
	}
	return (*ng2 == '.' || *ng2 == NGDELIM || *ng2 == '\0');
}

/*
 * Exec the shell.
 * This version resets uid, gid, and umask.
 * Called with fsubr(ushell, s, NULL)
 */
/* ARGSUSED */
ushell(s, dummy)
char *s, *dummy;
{
	umask(savmask);
	setgid(gid);
	setuid(uid);
	xshell(s);
}

/*
 * Exec the shell.
 * This version restricts PATH to bin and /usr/bin.
 * Called with fsubr(pshell, s, NULL)
 */
extern char	**environ;

/* ARGSUSED */
pshell(s, dummy)
char *s, *dummy;
{
	static char *penv[] = { SYSPATH, NULL };
	register char **ep, *p;
	register int found;

	found = FALSE;
	for (ep = environ; p = *ep; ep++) {
		if (strncmp(p, "PATH=", 5) == 0) {
			*ep = penv[0];
			found = TRUE;
		}
	}
	if (!found)
		environ = &penv[0];
	xshell(s);
}

/*
 * Exec the shell.
 */
xshell(s)
char *s;
{
	execl(SHELL, SHELL, "-c", s, (char *)0);
	xerror("No shell!");
}

/*
 * Fork and call a subroutine with two args.
 * Return pid without waiting.
 */
fsubr(f, s1, s2)
int (*f)();
char *s1, *s2;
{
	register int pid;

	while ((pid = fork()) == -1)
		sleep((unsigned)1);
	if (pid == 0) {
		(*f)(s1, s2);
		exit(0);
	}
	return(pid);
}

/*
 * Wait on a child process.
 */
fwait(pid)
register int pid;
{
	register int w;
	int status;
	int (*onhup)(), (*onint)();

	onint = signal(SIGINT, SIG_IGN);
	onhup = signal(SIGHUP, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, onint);
	signal(SIGHUP, onhup);
	return(status);
}

/*
 * Strip trailing newlines, blanks, and tabs from 's'.
 * Return TRUE if newline was found, else FALSE.
 */
nstrip(s)
register char *s;
{
	register char *p;
	register int rc;

	rc = FALSE;
	p = s;
	while (*p)
		if (*p++ == '\n')
			rc = TRUE;
	while (--p >= s && (*p == '\n' || *p == ' ' || *p == '\t'));
	*++p = '\0';
	return(rc);
}

/*
 * Delete trailing NGDELIM.
 */
ngdel(s)
register char *s;
{
	if (*s++) {
		while (*s++);
		s -= 2;
		if (*s == NGDELIM)
			*s = '\0';
	}
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 *
 * These are the v7 index and rindex routines, stolen for portability.
 * (Some Unix systems call them strchr and strrchr, notably PWB 2.0
 * and its derivitives such as Unix/TS 2.0, Unix 3.0, etc.)  Others,
 * like v6, don't have them at all.
 */

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}

/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
 */

char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}

/*
 * Local open routine.
 */
FILE *
xfopen(name, fmode)
register char *name, *fmode;
{
	register FILE *fp;
	char	*fname;
	extern int errno, sys_nerr;

	if ((fp = fopen(name, fmode)) == NULL) {
#ifdef IHCC
		/*
		 * IHCC users only see the "filename" that was in trouble,
		 * not the whole path.  (for security!)
		 */
		fname = rindex(name, '/') + 1;
#else
		fname = name;
#endif
		if (errno > sys_nerr)
			sprintf(bfr, "Cannot open %s (%s): Error %d",
			    fname, fmode, errno);
		else
			sprintf(bfr, "Cannot open %s (%s): %s",
			    fname, fmode, sys_errlist[errno]);
		xerror(bfr);
	}
	/* kludge for setuid not being honored for root */
	if ((uid == 0) && (duid != 0) && ((fmode == "a") || (fmode == "w")))
		chown(name, duid, dgid);
	return(fp);
}

prefix(full, pref)
register char *full, *pref;
{
	register char fc, pc;

	do {
		fc = *full++;
		pc = *pref++;
		if (isupper(fc))
			fc = tolower(fc);
		if (isupper(pc))
			pc = tolower(pc);
	} while (fc == pc);
	if (*--pref == 0)
		return 1;
	else
		return 0;
}

char *
dirname(ngname)
char *ngname;
{
	static char rbuf[BUFLEN];
	register char *p;

	sprintf(rbuf, "%s/%s", SPOOL, ngname);

	/* Use the new style name for all new stuff. */
	for (p=rbuf+strlen(SPOOL); *p; p++)
		if (*p == '.')
			*p = '/';
	return rbuf;
}

/*
 * Return TRUE iff ngname is a valid newsgroup name
 */
validng(ngname)
char *ngname;
{
	register FILE *fp;
	register char *p, *q;
	char abuf[BUFLEN];

	fp = xfopen(ACTIVE, "r");
	while(fgets(abuf, BUFLEN, fp) != NULL) {
		p = abuf;
		q = ngname;
		while (*p++ == *q++)
			;
		if (*--q == '\0' && *--p == ' ') {
			fclose(fp);
			return TRUE;
		}
	}
	fclose(fp);
	return FALSE;
}

/* VARARGS1 */
log(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_dolog(0, fmt, ap);
}

/* VARARGS1 */
logerr(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_dolog(1, fmt, ap);
}

/* VARARGS1 */
xerror(message, arg1, arg2, arg3)
char *message;
int arg1, arg2, arg3;
{
	char buffer[128];

	fflush(stdout);
	sprintf(buffer, message, arg1, arg2, arg3);
	logerr(buffer);
	xxit(1);
}

char *lfsuffix[] = {
	"log",
	"errlog",
	0
};


/*
 * Log the given message, with printf strings and parameters allowed,
 * on the log file, if it can be written.  The date and an attempt at
 * figuring out the remote system name are also logged.
 */
/* VARARGS1 */
_dolog(which, fmt, ap)
char *fmt;
{
	FILE *logfile;
	register char *p, *q, *logtime;
	int i;
	char logfname[BUFLEN];		/* the log file */
	char rmtsys[BUFLEN];
	char msg[BUFLEN];
	char c;
	time_t t;

	if (header.relayversion[0]) {
		for (p=header.relayversion; p; p=index(p+1, 's'))
			if (strncmp(p, "site ", 5) == 0)
				break;
		if (p == NULL)
			goto crackpath;
		p += 4;
		while (*p == ' ' || *p == '\t')
			p++;
		for (q=p; *q && *q!=' ' && *q != '\t'; q++)
			;
		c = *q;
		strcpy(rmtsys, p);
		*q = c;
	} else {
crackpath:
		strcpy(rmtsys, header.path);
		p = index(rmtsys, '!');
		if (p == NULL)
			p = index(rmtsys, ':');
		if (p)
			*p = 0;
		else {
			p = rindex(rmtsys, '@');
			if (p)
				strcpy(rmtsys, p+1);
			else
				strcpy(rmtsys, "local");
		}
	}

	(void) time(&t);
	logtime = ctime(&t);
	logtime[16] = 0;
	logtime += 4;


	vsprintf(msg, fmt, ap);

	if (which)
		fprintf(stderr,"%s: %s\n", Progname, msg);

	for (i=0; i<=which;i++) {
		sprintf(logfname, "%s/%s", LIB, lfsuffix[i]);

		if (access(logfname, 0) == 0 && (logfile = fopen(logfname, "a")) != NULL) {
#if defined(USG) || defined(BSD4_2) || defined(BSD4_1C)
			int flags;
			flags = fcntl(fileno(logfile), F_GETFL, 0);
			fcntl(fileno(logfile), F_SETFL, flags|O_APPEND);
#else v7
			lseek(fileno(logfile), 0L, 2);
#endif v7
			if (i)
				fprintf(logfile, "%s\t%s\t%s: %s\n", logtime,
					header.ident, Progname, msg);
			else
				fprintf(logfile, "%s\t%s\t%s\n", logtime,
					rmtsys, msg);
			fclose(logfile);
		}
	}
}
