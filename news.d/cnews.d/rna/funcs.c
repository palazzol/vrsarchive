#include "defs.h"

/*
 * string handling functions
 */
char *
myalloc(size)
int size;
{
	register char *cp;

	extern char *malloc();

	if ((cp = malloc((unsigned) size)) == NIL(char))
		error("No more memory.");
	return cp;
}


char *
myrealloc(ptr, size)
char *ptr;
int size;
{
	register char *cp;

	extern char *realloc();

	if ((cp = realloc(ptr, (unsigned) size)) == NIL(char))
		error("No more memory.");
	return cp;
}


char *
newstr(s)
char *s;
{
	return strcpy(myalloc(strlen(s) + 1), s);
}


char *
newstr2(s1, s2)
char *s1, *s2;
{
	return strcat(strcpy(myalloc(strlen(s1) + strlen(s2) + 1), s1), s2);
}


char *
newstr3(s1, s2, s3)
char *s1, *s2, *s3;
{
	return strcat(strcat(strcpy(myalloc(strlen(s1) + strlen(s2) + strlen(s3) +
	    1), s1), s2), s3);
}


char *
newstr4(s1, s2, s3, s4)
char *s1, *s2, *s3, *s4;
{
	return strcat(strcat(strcat(strcpy(myalloc(strlen(s1) + strlen(s2) +
	    strlen(s3) + strlen(s4) + 1), s1), s2), s3), s4);
}


char *
newstr5(s1, s2, s3, s4, s5)
char *s1, *s2, *s3, *s4, *s5;
{
	return strcat(strcat(strcat(strcat(strcpy(myalloc(strlen(s1) + strlen(s2) +
	    strlen(s3) + strlen(s4) + strlen(s5) + 1), s1), s2), s3), s4), s5);
}


char *
newstr6(s1, s2, s3, s4, s5, s6)
char *s1, *s2, *s3, *s4, *s5, *s6;
{
	return strcat(strcat(strcat(strcat(strcat(strcpy(myalloc(strlen(s1) +
	    strlen(s2) + strlen(s3) + strlen(s4) + strlen(s5) + strlen(s6) + 1),
	     s1), s2), s3), s4), s5), s6);
}


char *
catstr(old, s)
char *old, *s;
{
	return strcat(myrealloc(old, strlen(old) + strlen(s) + 1), s);
}


char *
catstr2(old, s1, s2)
char *old, *s1, *s2;
{
	return strcat(strcat(myrealloc(old, strlen(old) + strlen(s1) + strlen(s2) +
	    1), s1), s2);
}


/*
 * News group matching.
 *
 * nglist is a list of newsgroups.
 * sublist is a list of subscriptions.
 * sublist may have "meta newsgroups" in it.
 * All fields are NGSEPCHAR separated.
 *
 * sublist uses "all" like shell uses "*", and "." like shell uses "/"
 * if subscription X matches Y, it also matches Y.anything
 */
ngmatch(nglist, sublist)
char *nglist, *sublist;
{
	register char *n, *s, *nd, *sd;
	register int rc;

	rc = 0;
	n = nglist;
	while (*n && rc == 0) {
		if (nd = strchr(n, NGSEPCHAR))
			*nd = '\0';
		s = sublist;
		while (*s) {
			if (sd = strchr(s, NGSEPCHAR))
				*sd = '\0';
			if (*s != NEGCHAR)
				rc |= ptrncmp(s, n);
			else
				rc &= ~ptrncmp(s + 1, n);
			if (sd)
				*sd = NGSEPCHAR, s = sd + 1;
			else
				break;
		}
		if (nd)
			*nd = NGSEPCHAR, n = nd + 1;
		else
			break;
	}
	return rc;
}


/*
 * Compare two newsgroups for equality.
 * The first one may be a "meta" newsgroup.
 */
static
ptrncmp(ng1, ng2)
register char *ng1, *ng2;
{

	while (1) {
		if (ng1[0] == 'a' && ng1[1] == 'l' && ng1[2] == 'l' && (ng1[3] ==
		    '\0' || ng1[3] == '.')) {
			if (ng1[3] == '\0')	/* "all" matches anything */
				return 1;
			while (*ng2 && *ng2 != '.')
				ng2++;
			if (*ng2 != '.')		/* "all." doesn't match "xx" */
				return 0;
			ng1 += 4, ng2++;
			continue;
		}
		while (*ng1 && *ng1 != '.' && *ng1 == *ng2)
			ng1++, ng2++;
		if (*ng1 == '.') {
			if (*ng2 != '.' && *ng2 != '\0')
				return 0;	/* "."'s don't line up */
			if (*ng2)
				ng2++;
			ng1++;			/* "."'s line up - keep going */
		} else if (*ng1 == '\0')
			return (*ng2 == '\0' || *ng2 == '.');
			/* full match or X matching X.thing */
		else
			return 0;
	}
	/* NOTREACHED */
}


/*
 * return new newsgroup composed of only those from 'nglist'
 * subscribed to by 'sublist'
 * return NULL for empty list
 */
char *
ngsquash(nglist, sublist)
register char *nglist, *sublist;
{
	register char *delim;
	register char *newg;

	newg = NIL(char);
	while (*nglist) {
		if (delim = strchr(nglist, NGSEPCHAR))
			*delim = '\0';
		if (ngmatch(nglist, sublist))
			newg = (newg ? catstr2(newg, NGSEPS, nglist) : newstr(nglist));
		if (delim)
			*delim = NGSEPCHAR, nglist = delim + 1;
		else
			break;
	}
	return newg;
}


/*
 * get unique sequence number from SEQ
 */
char *
getunique()
{
	register long number;
	register FILE	*f;
	static char buf[12];

	f = fopenl(SEQ);
	if (fread(buf, 1, sizeof(buf), f) > 0)
		number = atol(buf);
	else
		number = 1;

	rewind(f);
	(void) fprintf(f, "%ld\n", number + 1);
	fclose(f);
#if !AUSAM
	unlock(SEQ);
#endif

	sprintf(buf, "%ld", number);
	return buf;
}


/*
 * open a locked file (or create) for reading and writing
 */
FILE *
fopenl(fname)
char *fname;
{
	register FILE	*f;
#ifdef AUSAM
	struct stat sbuf;
#endif

	extern uid_t	newsuid;

	if ((f = fopen(fname, "r+")) == NIL(FILE) && (f = fopen(fname, "w+")) ==
	    NIL(FILE))
		error("Can't open %s", fname);

#if AUSAM
	if (fstat(fileno(f), &sbuf) != 0)
		error("Can't stat %s", fname);
	if ((sbuf.st_mode & S_IFMT) != S_IFALK && (chmod(fname, (int) (sbuf.st_mode
	    &~S_IFMT) | S_IFALK) != 0 || chown(fname, (int) newsuid, (int) newsuid) !=
	    0 || fclose(f) == EOF || (f = fopen(fname, "r+")) == NIL(FILE)))
		error("Can't create %s", fname);
#else
	chown(fname, (int) newsuid, (int) newsuid);
	lock(fname);
#endif

	return f;
}


#if !AUSAM

#define LSUFFIX	".lock"		/* suffix for lock files */

lock(fname)
char *fname;
{
	register char *lname;
	register int i, f;

	lname = newstr2(fname, LSUFFIX);
	for (i = 0; i < 10; i++) {
		if ((f = creat(lname, 0)) != -1) {
			close(f);
			free(lname);
			return;
		}
		sleep(2);
	}
	error("Can't creat %s after %d tries", lname, i);
}


unlock(fname)
char *fname;
{
	register char *lname;

	lname = newstr2(fname, LSUFFIX);
	unlink(lname);
	free(lname);
}


#endif

/*
 * open a file
 */
FILE *
fopenf(name, mode)
char *name, *mode;
{
	register FILE	*f;

	if ((f = fopen(name, mode)) == NIL(FILE))
		error("Can't %s %s", *mode == 'r' ? "open" : "create", name);
	return f;
}


/*
 * replace all '.''s with '/'
 */
char *
convg(s)
register char *s;
{
	register char *sav;

	sav = s;
	while (s = strchr(s, '.'))
		*s = '/';
	return sav;
}


/*
 * replace all '/''s with '.'
 */
char *
rconvg(s)
register char *s;
{
	register char *sav;

	sav = s;
	while (s = strchr(s, '/'))
		*s = '.';
	return sav;
}


/*
 * get a line from stdin
 * trim leading and trailing blanks
 */
char *
mgets()
{
	register char *s;
	static char buf[BUFSIZ];

	fflush(stdout);
	if (fgets(buf, sizeof(buf), stdin) == NIL(char)) {
		(void) printf("\n");
		return NIL(char);
	}
	if (s = strchr(buf, '\n'))
		while (isspace(*s) && s > buf)
			*s-- = '\0';
	else
	 {
		(void) printf("Input line too long.\n");
		return NIL(char);
	}
	s = buf;
	while (isspace(*s))
		s++;
	return s;
}


readln(f)
FILE *f;
{
	register int c;

	if (feof(f) || ferror(f))
		return;
	while ((c = getc(f)) != '\n' && c != EOF)
		;
}


/*
 * compare string pointers
 */
strpcmp(a, b)
char **a, **b;
{
	return CMP(*a, *b);
}


/*
 * apply the given function to each member in the newsgroup
 */
/* VARARGS2 */
applyng(ng, func, arg1)
register char *ng;
register int (*func)();
char *arg1;
{
	register char *delim;
	register int err;

	err = 0;
	while (*ng) {
		if (delim = strchr(ng, NGSEPCHAR))
			*delim = '\0';
		err += (*func)(ng, arg1);
		if (delim)
			*delim = NGSEPCHAR, ng = delim + 1;
		else
			break;
	}
	return err;
}


/*
 * generate a return address
 */
char *
getretaddr(hp)
header *hp;
{
	register char *ra;

	extern char *getpath(), *exaddress();
#ifdef NETPATH
	extern char *getnetpath();
#endif

	if (hp->h_replyto)
		ra = exaddress(hp->h_replyto);
	else if (hp->h_from)
		ra = exaddress(hp->h_from);
	else
		ra = NIL(char);
	if (hp->h_path && !ra)
		ra = getpath(hp->h_path);
#ifdef NETPATH
	if (CMPN(ra, PATHPREF, sizeof(PATHPREF) - 1) == 0)
		ra = getnetpath(ra);
#endif
	return ra;
}


/*
 * try and make a proper address
 */
char *
exaddress(addr)
char *addr;
{
	register char *space, *dot, *at;
	register char *raddr;

	raddr = NIL(char);
	if (space = strchr(addr, ' '))
		*space = '\0';
	if (at = strchr(addr, '@')) {
		*at = '\0';
		if (dot = strchr(at + 1, '.')) {
			*dot = '\0';
#if OZ
			if (CMP(dot + 1, MYDOMAIN) == 0)
				raddr = newstr3(addr, ":", at + 1);
			else
#endif
				raddr = newstr4(PATHPREF, at + 1, PSEPS, addr);
			*dot = '.';
		}
		*at = '@';
	}
	if (space)
		*space = ' ';
	return raddr;

}


/*
 * return the last two components of the path
 */
char *
getpath(path)
char *path;
{
	register char *exlast, *ex;
	register char *raddr;

	if (exlast = strrchr(path, PSEPCHAR)) {
		*exlast = '\0';
		if (ex = strrchr(path, PSEPCHAR))
			raddr = newstr4(PATHPREF, ex + 1, PSEPS, exlast + 1);
		else
			raddr = newstr3(path, PSEPS, exlast + 1);
		*exlast = PSEPCHAR;
	} else
		raddr = NIL(char);
	return raddr;
}


#ifdef NETPATH
/*
 * try and work out a path from our "netpath" database
 */
char *
getnetpath(path)
char *path;
{
	FILE		 * f;
	register char *ex1, *ex2, *com, *new;
	char buf[BUFSIZ];

	if ((ex1 = strchr(path, PSEPCHAR)) && (ex2 = strchr(ex1 + 1, PSEPCHAR))) {
		*ex2 = '\0';
		com = newstr4("exec ", NETPATH, " mulga ", ex1 + 1);
		if ((f = popen(com, "r")) == NIL(FILE))
			(void) printf("Couldn't run \"%s\"\n", com);
		else
		 {
			fread(buf, sizeof(buf), 1, f);
			if (pclose(f) != 0) {
				(void) printf("Error in running \"%s\"\n", com);
				fflush(stdout);
			} else if (CMPN(buf, "mulga!", 6) == 0) {
				if (ex1 = strchr(buf, '\n'))
					*ex1 = '\0';
				new = newstr4(buf + 6, PSEPS, ex2 + 1, ":mulga");
				free(path);
				path = new;
			}
		}
		free(com);
		*ex2 = PSEPCHAR;
	}
	return path;

}


#endif

/*
 * remove extra spaces, and insert separators if necessary in
 * newsgroups specification
 */
convgrps(sp)
register char *sp;
{
	register char *sep;

	sep = NIL(char);
	while (*sp) {
		if (sep)
			sp++;
		while (*sp && (isspace(*sp) || *sp == NGSEPCHAR))
			strcpy(sp, sp + 1);
		if (sep)
			*sep = (*sp ? NGSEPCHAR : '\0');
		while (*sp && !isspace(*sp) && *sp != NGSEPCHAR)
			sp++;
		sep = sp;
	}
}


