/*
 * expire [ -n newsgroups ] [ -d days ] [ -w weeks ]
 *
 *	delete articles that arrived before the given date (now - days|weeks)
 *	or that have expiry dates in the past
 *
 *	Michael Rourke, UNSW, April 1984.
 */

#include "defs.h"
#include "at.h"

char histname[]	 = HISTORY;
char all[]		 = "all";

char *nflag		 = all;
long etime		 = SECINWEEK *2L;
char *newsdir;
uid_t	newsuid;
long now;

main(argc, argv)
int argc;
char *argv[];
{
#if AUSAM
	struct pwent pe;
	char sbuf[SSIZ];
#else
	struct passwd *pp;
	struct passwd *getpwnam();
#endif

	for (argv++, argc--; argc > 0; argc--, argv++) {
		if (argv[0][0] != '-' && argv[0][2] != '\0')
			break;
		switch (argv[0][1]) {
		case 'n':
			nflag = argv[1]; 
			break;
		case 'd':
			etime = atoi(argv[1]) * SECINDAY; 
			break;
		case 'w':
			etime = atoi(argv[1]) * SECINWEEK; 
			break;
		default:
			argc = -1; 
			break;
		}
		argv++, argc--;
	}
	if (argc != 0 || etime < 0) {
		fprintf(stderr, "Usage: expire [-n newsgroups] [-d days] [-w weeks]\n");
		exit(1);
	}
	time(&now);
#if AUSAM
	pe.pw_strings[LNAME] = NEWSROOT;
	if (getpwuid(&pe, sbuf, sizeof(sbuf)) == PWERROR)
		error("Password file error.");
	newsdir = pe.pw_strings[DIRPATH];
	newsuid = pe.pw_limits.l_uid;
#else
	if ((pp = getpwnam(NEWSROOT)) == NULL)
		error("Password file error.");
	newsdir = pp->pw_dir;
	newsuid = pp->pw_uid;
#endif
	umask(022);
	setgid((int) newsuid);
	setuid((int) newsuid);

	expire(nflag, now - etime);
	chklow(readactive());
	exit(0);
}


/*
 * expire articles in the given groups that have arrived before stime
 */
expire(grps, stime)
char *grps;
long stime;
{
	register FILE	*f, *tf, *nf;
	register int i;
	register char *s, *name;
	char buf[BUFSIZ];
	bool		eflag;
	long pos, tpos, tim;
	FILE * tmpfile();

	bool		okgrp();

	f = fopenl(histname);
	tf = NIL(FILE);
	while (1) {
		pos = ftell(f);
		if (fgets(buf, sizeof(buf), f) == NIL(char))
			break;
		if ((s = strchr(buf, '>')) == NIL(char))
			error("Bad format: %s", histname);
		s += 2;
		if (*s == 'E')
			eflag = true, s++;
		else
			eflag = false;
		tim = atol(s);
		if ((name = strchr(s, ' ')) == NIL(char))
			error("Bad format: %s", histname);
		name++;
		if (!okgrp(name, grps) || (!eflag && tim > stime || eflag &&
		    tim > now)) {
			/* don't expire now */
			if (tf)
				fputs(buf, tf);
			continue;
		}
		/*
		 * have something to expire
		 */
		if (!tf) {
			/*
 			 * start saving unexpired history
			 */
			if ((tf = tmpfile()) == NIL(FILE))
				error("Can't open tmp file.");
			tpos = ftell(f);
			rewind(f);
			for (i = 0; i < pos; i++)
				putc(getc(f), tf);
			fseek(f, tpos, 0);
		}
		while (*name && (s = strpbrk(name, " \n"))) {
			*s = '\0';
			name = newstr3(newsdir, "/", name);
			unlink(name);
			free(name);
			name = s + 1;
		}
	}
	if (tf) {
		rewind(tf);
		nf = fopenf(histname, "w");
		while ((i = getc(tf)) != EOF)
			putc(i, nf);
		fclose(nf);
		fclose(tf);
	}
#if !AUSAM
	unlock(histname);
#endif
	fclose(f);
}


/*
 * check that these groups are ok to expire
 */
bool
okgrp(names, grp)
char *names, *grp;
{
	register char *s, *hash, c;
	register bool	matched;

	if (grp == all)
		return true;
	matched = true;
	while (matched && *names && (s = strpbrk(names, " \n"))) {
		c = *s;
		*s = '\0';
		if ((hash = strchr(names, '#')) == NIL(char))
			error("Bad format: %s", histname);
		*--hash = '\0';		/* delete last '/' */

		rconvg(names);
		matched = (bool) ngmatch(names, grp);
		convg(names);

		*hash = '/';
		*s = c;
		names = s + 1;
	}
	return matched;
}


/*
 * set the "low" values in active file
 */
chklow(ap)
active *ap;
{
	register char *fname;
	register int low, i;
	register FILE	*f;
	struct direct dbuf;

	for ( ; ap; ap = ap->a_next) {
		low = ap->a_seq + 1;
		fname = convg(newstr3(newsdir, "/", ap->a_name));
		if ((f = fopen(fname, "r")) == NIL(FILE)) {
			warn("Can't open %s", fname);
			free(fname);
			continue;
		}
		fseek(f, (long) (sizeof(dbuf) * 2), 0);
		while (fread((char *) & dbuf, sizeof(dbuf), 1, f) == 1) {
			if (dbuf.d_ino == 0)
				continue;
			if (dbuf.d_name[0] != '#')
				continue;
			i = atoi(&dbuf.d_name[1]);
			if (i > 0 && i < low)
				low = i;
		}
		fclose(f);
		if (low > ap->a_low)
			setlow(ap->a_name, low);
		free(fname);
	}
}


/* VARARGS1 */
error(s, a0, a1, a2, a3)
char *s;
{
	fprintf(stderr, "expire: ");
	fprintf(stderr, s, a0, a1, a2, a3);
	fprintf(stderr, "\n");
	exit(1);
}


/* VARARGS1 */
warn(s, a0, a1, a2, a3)
char *s;
{
	fprintf(stderr, "expire: Warning: ");
	fprintf(stderr, s, a0, a1, a2, a3);
	fprintf(stderr, "\n");
}


