/*
 * funcs2 - functions used by both inews and readnews.
 */

#ifndef lint
static char	*SccsId = "@(#)funcs2.c	1.5	8/28/84";
#endif !lint

#include "params.h"

/*
 * Get user name and home directory.
 */
getuser()
{
	static int flag = TRUE;
	register struct passwd *p;

	if (flag) {
		if ((p = getpwuid(uid)) == NULL)
			xerror("Cannot get user's name");
		if ( username == NULL || username[0] == 0)
			username = AllocCpy(p->pw_name);
		userhome = AllocCpy(p->pw_dir);
		flag = FALSE;
	}
	strcpy(header.path, username);
}

static	FILE	*sysfile;

char *fldget();

/*
 * Open SUBFILE.
 */
s_openr()
{
	sysfile = xfopen(SUBFILE, "r");
}

/*
 * Read SUBFILE.
 */
s_read(sp)
register struct srec *sp;
{
	register char *p;
again:
	p = bfr;
	if (fgets(p, LBUFLEN, sysfile) == NULL)
		return(FALSE);
	if (!nstrip(p))
		xerror("SUBFILE line too long.");
	if (*p == '#')
		goto again;
	sp->s_xmit[0] = '\0';
	sp->s_flags[0] = '\0';

	p = fldget(sp->s_name, p);
	if (*p++ == '\0')
		xerror("Bad SUBFILE line.");
/*
 * A sys file line reading "ME" means the name of the local system.
 */
	if (strcmp(sp->s_name, "ME") == 0)
		strcpy(sp->s_name, FULLSYSNAME);
	p = fldget(sp->s_nbuf, p);
	lcase(sp->s_nbuf);
	ngcat(sp->s_nbuf);
	if (*p++ == '\0')
		return(TRUE);

	p = fldget(sp->s_flags, p);
	if (*p++ == '\0')
		return(TRUE);

	(void) fldget(sp->s_xmit, p);
	return(TRUE);
}

char *
fldget(q, p)
register char *q, *p;
{
	while (*p && *p != ':') {
		if (*p == '\\' && p[1]==':')
			p++;
		*q++ = *p++;
	}
	*q = '\0';
	return(p);
}

/*
 * Find the SUBFILE record for a system.
 */
s_find(sp, system)
register struct srec *sp;
char *system;
{
	s_openr();
	while (s_read(sp))
		if (strncmp(system, sp->s_name, SNLN) == 0) {
			s_close();
			return(TRUE);
		}
	s_close();
	return(FALSE);
}

/*
 * Close sysfile.
 */
s_close()
{
	fclose(sysfile);
}

time_t
cgtdate(datestr)
char *datestr;
{
	time_t	i;
	char	junk[40],month[40],day[30],tod[60],year[50];

	if ((i = getdate(datestr, (struct timeb *) NULL)) >= 0)
		return i;
	sscanf(datestr, "%s %s %s %s %s", junk, month, day, tod, year);
	sprintf(bfr, "%s %s, %s %s", month, day, year, tod);
	return getdate(bfr, (struct timeb *) NULL);
}

lcase(s)
register char *s;
{
	register char *ptr;

	for (ptr = s; *ptr; ptr++)
		if (isupper(*ptr))
			*ptr = tolower(*ptr);
}

ohwrite(hp, fp)
register struct hbuf *hp;
register FILE *fp;
{
	ngdel(strcpy(bfr, hp->nbuf));
	fprintf(fp, "A%s\n%s\n%s!%s\n%s\n%s\n", hp->oident, bfr, FULLSYSNAME, hp->path, hp->subdate, hp->title);
}

/*
 * Return a compact representation of the person who posted the given
 * message.  A sender or internet name will be used, otherwise
 * the last part of the path is used preceeded by an optional ".."
 */
char *
tailpath(hp)
struct hbuf *hp;
{
	char *p, *r;
	static char resultbuf[BUFLEN];
	char pathbuf[PATHLEN];
	char *malloc();

	/*
	 * This only happens for articles posted by old news software
	 * in non-internet format.
	 */
	resultbuf[0] = '\0';
	strcpy(pathbuf, hp->path);
	p = index(pathbuf, ' ');
	if (p)
		*p = '\0';	/* Chop off trailing " (name)" */
	r = rindex(pathbuf, '!');
	if (r == 0) {
		r = pathbuf;
	}
	else {
		while (r > pathbuf && *--r != '!')
			;
		if (r > pathbuf) {
			r++;
			strcpy(resultbuf, "..!");
		}
	}
	strcat(resultbuf, r);
	return resultbuf;
}

/*
 * arpadate is like ctime(3) except that the time is returned in
 * an acceptable ARPANET time format instead of ctime format.
 */
char *
arpadate(longtime)
time_t *longtime;
{
	register char *p, *q, *ud;
	register int i;
	static char b[40];
	extern struct tm *gmtime();
	extern char *asctime();

	/*  Get current time. This will be used resolve the timezone. */
	ud = asctime(gmtime(longtime));

	/*  Crack the UNIX date line in a singularly unoriginal way. */
	q = b;

#ifdef notdef
/* until every site installs the fix to getdate.y, the day
   of the week can cause time warps */
	p = &ud[0];		/* Mon */
	*q++ = *p++;
	*q++ = *p++;
	*q++ = *p++;
	*q++ = ','; *q++ = ' ';
#endif

	p = &ud[8];		/* 16 */
	if (*p == ' ')
		p++;
	else
		*q++ = *p++;
	*q++ = *p++; *q++ = ' ';

	p = &ud[4];		/* Sep */
	*q++ = *p++; *q++ = *p++; *q++ = *p++; *q++ = ' ';

	p = &ud[22];		/* 1979 */
	*q++ = *p++; *q++ = *p++; *q++ = ' ';

	p = &ud[11];		/* 01:03:52 */
	for (i = 8; i > 0; i--)
		*q++ = *p++;

	*q++ = ' ';
	*q++ = 'G';		/* GMT */
	*q++ = 'M';
	*q++ = 'T';
	*q = '\0';

	return (b);
}

char *
replyname(hptr)
struct hbuf *hptr;
{
	register char *ptr;
	register char *ptr2;
	static char tbuf[PATHLEN];

	ptr = hptr->path;
	if (prefix(ptr, FULLSYSNAME))
		ptr = index(ptr, '!') + 1;
#ifdef INTERNET
	if (hptr->from[0])
		ptr = hptr->from;
	if (hptr->replyto[0])
		ptr = hptr->replyto;
#endif
	strcpy(tbuf, ptr);
	ptr = index(tbuf, '(');
	if (ptr) {
		while (ptr[-1] == ' ')
			ptr--;
		*ptr = 0;
	}
#ifndef INTERNET
	/*
	 * Play games stripping off multiple berknet
	 * addresses (a!b!c:d:e => a!b!d:e) here.
	 */
	for (ptr=tbuf; *ptr; ptr++)
		if (index(NETCHRS, *ptr) && *ptr == ':' && (ptr2=index(ptr+1, ':')))
				strcpy(ptr, ptr2);
#endif !INTERNET
	return tbuf;
}

#ifdef DBM
typedef struct {
	char *dptr;
	int dsize;
} datum;
#endif DBM

/*
 * Given an article ID, find the line in the history file that mentions it.
 * Return the text of the line, or NULL if not found.  A pointer to a
 * static area is returned.
 */
char *
findhist(artid)
char *artid;
{
	static char lbuf[256];
	char oidbuf[BUFSIZ];
	FILE *hfp;
	register char *p;
#ifdef DBM
	datum lhs, rhs;
	datum fetch();
#endif DBM

	/* Try to understand old artid's as well.  Assume .UUCP domain. */
	if (artid[0] != '<') {
		p = index(artid, '.');
		if (p)
			*p++ = '\0';
		sprintf(oidbuf, "<%s@%s.UUCP>", p, artid);
		if (p)
			*--p = '.';
	} else
		strcpy(oidbuf, artid);
	p = oidbuf;
	while (*++p)
		if (isupper(*p))
			*p = tolower(*p);

	hfp = xfopen(ARTFILE, "r");
#ifdef DBM
	dbminit(ARTFILE);
	lhs.dptr = oidbuf;
	lhs.dsize = strlen(lhs.dptr) + 1;
	rhs = fetch(lhs);
	if (rhs.dptr) {
		fseek(hfp, (long)rhs.dptr, 0);
#endif DBM
	while (fgets(lbuf, BUFLEN, hfp) != NULL) {
		p = index(lbuf, '\t');
		if (p == NULL)
			p = index(lbuf, '\n');
		*p = 0;
		if (strcmp(lbuf, artid) == 0 || strcmp(lbuf, oidbuf) == 0) {
			fclose(hfp);
			*p = '\t';
			*(lbuf + strlen(lbuf) - 1) = 0;	/* zap the \n */
			return(lbuf);
		}
	}
#ifdef DBM
	}
#endif DBM
	fclose(hfp);
	return(NULL);
}

/*
 * Hunt up the article "artid", and return the newsgroup/artnum
 * where it can be found.
 */
char *
findfname(artid)
char *artid;
{
	char *line, *p, *q;
	char *findhist();
	static char fname[256];

	line = findhist(artid);
	if (line) {
		/* Look for it stored as an article, where it should be */
		p = index(line, '\t');
		p = index(p+1, '\t');
		p++;
		if (*p) {
			q = index(p, ' ');
			if (q)
				*q = 0;
			strcpy(fname, p);
			return fname;
		}
	}

	return NULL;
}

/*
 * Hunt up the article "artid", fopen it for read, and return a
 * file descriptor to it.  We look everywhere we can think of.
 */
FILE *
hfopen(artid)
char *artid;
{
	char *p;
	char *findhist();
	FILE *rv = NULL;
	char fname[256];

	p = findfname(artid);
	if (p) {
		strcpy(fname, dirname(p));
		rv = fopen(fname, "r");	/* NOT xfopen! */
		if (rv != NULL)
			return rv;
	}

	xerror("Cannot hfopen article %s", artid);
#ifdef lint
	return NULL;
#endif lint
}
