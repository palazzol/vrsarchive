/*
 * readnews
 *
 *	Michael Rourke (UNSW) April 1984
 */

#include "defs.h"

#define ARTSEP "/"

char postnews[]	 = POSTNEWS;
char admsub[]	 = ADMSUB;
char dfltsub[]	 = DFLTSUB;
char *mailpath	 = MAIL;

#define	MAXARGV	10		/* used in building argv[]s below */

#ifndef NETID
char systemid[DIRSIZ];
#else
char systemid[] = NETID;
#endif

bool iflag;		/* -i ignore .newsrc */
bool lflag;		/* -l print headers only */
bool cflag;		/* -c check for news only */
bool pflag;		/* -p print everything selected */
char **uflag;		/* -u messagid (unsubscribe from followups) */
int usize;		/* number of uflag entries */
bool Cflag;		/* -C verbose -c */
bool sflag;		/* -s print newsgroup subscription list */
bool splus;		/* -s+ */
bool slistall;		/* -s? */
bool sminus;		/* -s- */
char *sarg;		/* arg to -s[+-] */
char *nflag;		/* -n newsgroups */
extern char *rcgrps;	/* -n newsgroups from newsrc file */
bool n_on_cline;	/* nflag set on command line */

extern newsrc	*rc;		/* internal .newsrc */

active *alist;		/* internal active list */

#if MANGRPS
char *mangrps;	/* mandatory subsciption list */
#endif

#if AUSAM
struct pwent pe;		/* current user passwd struct */
char sbuf[SSIZ];	/* passwd strings */
#else
struct passwd *pp;		/* current user passwd struct */
#endif
long now;		/* current time */
bool interrupt;		/* if interrupt hit */
char *newsdir;		/* %news */
uid_t newsuid;		/* %news uid (not used) */
bool su;		/* if super user (not used) */

applycom list(), check(), commands();
void *onintr();
bool ureject(), seen(), subs(), subsub();

#if MANGRPS
char *getmangrps();
#endif

main(argc, argv)
int argc;
char *argv[];
{
	char buf[BUFSIZ], *p;

	setbuf(stdout, buf);			/* TODO: remove this? */
	if (options(--argc, ++argv, true)) {
		(void) fprintf(stderr, "Usage: readnews [-n newsgroups] [-i] [-clpC] [-s[-+? [group]]] [-u messageid]\n");
		exit(1);
	}
	now = time(&now);

#if AUSAM
	pe.pw_strings[LNAME] = NEWSROOT;
	if (getpwuid(&pe, sbuf, sizeof(sbuf)) == PWERROR)
		error("Password file error.");
	newsdir = newstr(pe.pw_strings[DIRPATH]);
	newsuid = pe.pw_limits.l_uid;
#else
	if ((pp = getpwnam(NEWSROOT)) == NULL)
		error("Password file error.");
	newsdir = newstr(pp->pw_dir);
	newsuid = pp->pw_uid;
#endif

#if AUSAM
#if MANGRPS
	pe.pw_limits.l_uid = getuid();
	if (getpwlog(&pe, NIL(char), 0) == PWERROR)	/* want pw_cmask */
		error("Password file error.");
#endif
	pwclose();
#else
	pp = NIL(struct passwd );
	endpwent();
#endif

#ifndef NETID
	getaddr(G_SYSNAME, systemid);
#endif

	if (!iflag)
		readnewsrc();

	if (nflag)
		convgrps(nflag);
	else
		nflag = dfltsub;
	if (rcgrps)
		convgrps(rcgrps);
	if (!n_on_cline) {
#if MANGRPS
		int addsub();

		if (mangrps = getmangrps(pe.pw_cmask))
			applyng(mangrps, addsub, &nflag);
#endif
		if (!ngmatch(admsub, nflag))
			nflag = newstr3(admsub, NGSEPS, nflag);
	}
	if ((int) sflag + (int) lflag + (int) cflag + (int) pflag > 1)
		error("-clpsC flags are mutually exclusive.");
	if (uflag)
		qsort((char *) uflag, (unsigned) usize, sizeof(char *), strpcmp);

	/* user has private mailer? */
	if ((p = getenv("MAILER")) != NULL)
		mailpath = newstr(p);

	alist = readactive();

	if (sflag) {
		if (subs() && !iflag)
			writenewsrc(alist);
	} else if (lflag)
		apply(alist, nflag, list, false);
	else if (cflag)
		apply(alist, nflag, check, false);
	else {
		if (!pflag) {
			if (signal(SIGINT, SIG_IGN) != SIG_IGN)
				(void) signal(SIGINT, onintr);
			if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
				(void) signal(SIGQUIT, onintr);
		}
		apply(alist, nflag, commands, true);
		if (!iflag)
			writenewsrc(alist);
	}
	exit(0);
}

#if MANGRPS
/*
 * make a subscription list of all class groups the user belongs too
 * (mandatory subscription)
 */
char *
getmangrps(cmask)
char *cmask;
{
	static char *weekday[] = { 
		"mon", "tue", "wed", "thu", "fri" 	};
	register char **classes;
	register char *s, *end;
	register char *grp;
	register int i, size;
	extern char **getclasses();

	grp = NIL(char);
	if ((classes = getclasses(cmask)) == NIL(char *))
		error("Can't get classes.");
	while (*classes) {
		if (isdigit(**classes)) {
			/*
			 * trim string after numeric class
			 * if it is a day of the week
			 */
			s = *classes;
			while (isdigit(*s) || *s == '.')
				s++;
			if (*s) {
				end = s;
				while (isalpha(*end))
					end++;
				if (*end && end != s && end - s <= 3) {
					size = end - s;
					for (i = 0; i < 5; i++)
						if (CMPN(s, weekday[i], size) == 0)
							break;
					if (i != 5)
						*s = '\0';
				}
			}
		}
		grp = (grp? catstr2(grp, ",class.", *classes):
			newstr2("class.", *classes));
		classes++;
	}
	return grp;
}

/*
 * if newsgroup "ng" isn't subscribed to, add it to subscription list
 */
addsub(ng, slist)
char *ng;
char **slist;
{
	if (!ngmatch(ng, *slist))
		*slist = newstr3(ng, NGSEPS, *slist);
}

#endif

void *
onintr()
{
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGINT, onintr);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGQUIT, onintr);
	interrupt = true;
}

/*
 * process options
 * can be called from readnewsrc()
 */
options(argc, argv, cline)
int argc;
char *argv[];
bool cline;
{
	register char c;

	/* TODO: use getopt(3) */
	while (argc > 0) {
		if (argv[0][0] != '-')
			break;
		while (c = *(++(argv[0]))) {
			switch (c) {
			case 'n':
				if (cline)
					nflag = argv[1], n_on_cline = true;
				else {
					if (!n_on_cline)
						nflag = (nflag?
							catstr2(nflag, NGSEPS, argv[1]):
							newstr(argv[1]));
					rcgrps = (rcgrps?
						catstr2(rcgrps, NGSEPS, argv[1]):
						newstr(argv[1]));
				}
				argc--, argv++; 
				break;
			case 'u':
				usize++;
				uflag = (uflag? (char **)myrealloc((char *)uflag,
						(int)sizeof(char *) * usize):
					(char **)myalloc((int)sizeof(char *)));
				uflag[usize - 1] = newstr(argv[1]);
				argc--, argv++; 
				break;
			case 'i':
				iflag = true; 
				continue;
			case 's':
				sflag = true;
				switch (argv[0][1]) {
				case '\0':
					continue;
				case '+':
					splus = true; 
					break;
				case '?':
					slistall = true, ++(argv[0]); 
					continue;
				case '-':
					sminus = true; 
					break;
				default:
					argc = -1; 
					break;
				}
				if (argc > 0) {
					sarg = newstr(argv[1]);
					argc--, argv++;
				}
				break;
			case 'p':
				pflag = true; 
				continue;
			case 'l':
				lflag = true; 
				continue;
			case 'c':
				cflag = true; 
				continue;
			case 'C':
				cflag = Cflag = true; 
				continue;
			default:
				argc = -1; 
				break;
			}
			break;
		}
		argc--, argv++;
	}
	return argc != 0;
}

/*
 * subscription list handling
 * return true if newsrc is to be re-written
 */
bool
subs()
{
	register newsrc	*np;
	register active	*ap;
	register char *tmp, *com;
	register FILE *f;

	if (slistall) {
		(void) printf("Active newsgroups:\n");
		(void) fflush(stdout);
#ifdef	MC			/* gok. probably a local paginator */
		com = newstr2("exec ", MC);
		if ((f = popen(com, "w")) == NULL)
			f = stdout;
		free(com);
#else
		f = stdout;
		com = 0;		/* for lint */
		com = com;		/* for lint */
#endif
		for (ap = alist; ap; ap = ap->a_next)
			(void) fprintf(f, "%s\n", ap->a_name);
#ifdef	MC
		if (f != stdout)
			pclose(f);
#endif
		return false;
	} else if (splus || sminus) {
		if (strpbrk(sarg, BADGRPCHARS)) {
			(void) printf("%s: Illegal char in newsgroup.\n", sarg);
			return false;
		}
		if (ngmatch(sarg, nflag)) {
			/*
			 * normally we subscribe, check for an exclusion
			 */
			for (np = rc; np; np = np->n_next)
				if (CMP(sarg, np->n_name) == 0)
					break;
			if (np) {
				/*
				 * altering subscribe flag is all
				 * we need to change
				 */
				np->n_subscribe = splus;
				return true;
			}
			if (sminus) {
				/*
				 * try deleting from sub list
				 */
				if (subsub(sarg, rcgrps))
					return true;
				/*
				 * add specific exclusion
				 */
				rcgrps = newstr4(rcgrps, NGSEPS, NEGS, sarg);
				return true;
			}
		} else if (splus) {
			/*
			 * we don't subscribe,
			 * try deleting !sarg first
			 */
			tmp = newstr2(NEGS, sarg);
			subsub(tmp, rcgrps);
			if (!ngmatch(sarg, rcgrps))
				/*
				 * didn't work, so add explicit subscription
				 */
				rcgrps = rcgrps? newstr3(rcgrps, NGSEPS, sarg):
					newstr(sarg);
			return true;
		}
	} else {
		(void) printf("Subscription list: %s", nflag);
		for (np = rc; np; np = np->n_next)
			if (!np->n_subscribe && ngmatch(np->n_name, nflag))
				(void) printf(",!%s", np->n_name);
		(void) printf("\n");
	}
	return false;
}


/*
 * try and delete group from subscription list
 * return true if successful
 */
bool
subsub(grp, slist)
char *grp;
char *slist;
{
	register char *delim;

	while (*slist) {
		if (delim = strchr(slist, NGSEPCHAR))
			*delim = '\0';
		if (CMP(grp, slist) == 0) {
			if (delim)
				(void) strcpy(slist, delim + 1);
			else if (slist[-1] = ',')
				slist[-1] = '\0';
			else
				slist[0] = '\0';
			return true;
		}
		if (delim)
			*delim = NGSEPCHAR, slist = delim + 1;
		else
			break;
	}
	return false;
}

/*
 * list titles command (-l)
 */
applycom
list(ap, np)
active *ap;
newsrc *np;
{
	static active *lastap;
	static bool first = true;
	register char *fname;
	register FILE *f;
	header h;
	ino_t ino;

	np->n_last++;
	fname = convg(newstr5(newsdir, "/", ap->a_name, ARTSEP,
		itoa(np->n_last)));
	ino = 0;
	f = fopen(fname, "r");
	free(fname);
	if (!f || seen(f, &ino))
		return next;
	gethead(f, &h);
	if (uflag && h.h_references && ureject(&h)) {
		freehead(&h);
		return next;
	}
	if (first) {
		(void) printf("News articles:\n");
		first = false;
	}
	if (lastap != ap)
		(void) printf("  %s:\n", ap->a_name);
	lastap = ap;
	(void) printf("    %-4d %s\n", np->n_last, h.h_subject);
	(void) fclose(f);
	freehead(&h);
	if (ino)
		seen(NIL(FILE), &ino);
	return next;
}

/*
 * check command (-c or -C)
 */
applycom
check(ap, np)
active *ap;
newsrc *np;
{
	static bool done;
	register int num;

	np->n_last++;
	if (Cflag) {
		if (!done)
			(void) printf("You have news:\n");
		done = true;
		num = ap->a_seq - np->n_last + 1;
		(void) printf("\t%s %d article%s\n", ap->a_name, num, num > 1 ? "s" :
		    "");
		return nextgroup;
	} else
	 {
		(void) printf("You have news.\n");
		exit(0);
		/* NOTREACHED */
	}
}


/*
 * normal command handler (or pflag)
 * commands:
 *
 * \n 		print current article
 * + 		go to next article
 * q		quit
 * c		cancel
 * r		reply
 * m [person]	mail
 * f 		followup
 * p 		postnews
 * n [newsgrp]	next newsgroup
 * s [file]	save
 * u		unsubscribe from followup
 * U		unsubscribe from group
 * !stuff	shell escape
 * number or .	go to number
 * - 		back to previous article (toggle)
 * x		quick exit
 * h		long header info
 * H		full header
 *
 * inside r, f or p:
 *	.e	edit
 *	.i	interpolate
 *	. or EOT terminate message
 *	.!comd	shell escape
 */
applycom
commands(ap, np, last, pushed)
active *ap;
newsrc *np;
bool last;
bool pushed;
{
	static char errmess[] = "Incorrect command; Type `?' for help.\n";
	static char form[]    = "%s: %s\n";

	static char savedsys[BUFSIZ / 2];
	static active	*lastap, *rlastap;
	static newsrc	lastn;
	static char number[20];
	static active	*wantap;

	register char *com, *arg;
	register int c, i, size;
	register FILE 	*f;
	char *fname;
	header		h;
	newsrc		ntmp;
	ino_t		ino;
	bool printed, pheader, verbose, hadinterrupt;
	applycom	nextact;

	extern char t_from[], t_subject[], t_date[];
	extern char t_newsgroups[], t_path[], t_sender[];
	extern char t_replyto[], t_organization[];
	extern char *strncpy();
	extern active	*activep();


	if (last) {
		/*
		 * give user one last chance to
		 * see this article
		 */
		ap = rlastap;
		np = &lastn;
		wantap = NIL(active);
		if (!ap || pflag)
			return stop;
	} else if (wantap)
		/*
		 * doing an "n newsgroup" command
		 */
		if (wantap != ap)
			return nextgroup;
		else
			wantap = NIL(active);

	fname = convg(newstr5(newsdir, "/", ap->a_name, ARTSEP,
		itoa(np->n_last + 1)));
	f = fopen(fname, "r");
	ino = 0;
	if (!f || !last && !pushed && seen(f, &ino)) {
		if (pushed)
			(void) printf("Article %d (%s) no longer exists.\n",
				np->n_last + 1, ap->a_name);
		else
			np->n_last++;
		if (f)
			(void) fclose(f);
		free(fname);
		return next;
	}

	gethead(f, &h);
	if (!pushed && uflag && h.h_references && ureject(&h)) {
		/* unsubscribed followup */
		freehead(&h);
		np->n_last++;
		(void) fclose(f);
		free(fname);
		return next;
	}

	(void) printf("\n");
	interrupt = hadinterrupt = verbose = false;
	if (last) {
		(void) printf("No more articles.\n");
		printed = pheader = true;
	} else
		printed = pheader = false;

	while (1) {
		if (lastap != ap) {
			size = strlen(ap->a_name) + sizeof("Newsgroup");
			for (i = 0; i < size; i++)
				(void) putc('-', stdout);
			(void) printf("\nNewsgroup %s\n", ap->a_name);
			for (i = 0; i < size; i++)
				(void) putc('-', stdout);
			(void) printf("\n\n");
		}
		lastap = ap;
		if (!pheader) {
			(void) printf("Article %d of %d (%s)",
				np->n_last + 1, ap->a_seq, ap->a_name);
			if (h.h_lines != 0)
				(void) printf(" (%s lines)", h.h_lines);
			(void) printf("\n");
			(void) printf(form, t_subject, h.h_subject);
			(void) printf(form, t_from, h.h_from);
			if (verbose || pflag) {
				(void) printf(form, t_date, h.h_date);
				(void) printf(form, t_newsgroups, h.h_newsgroups);
				(void) printf(form, t_path, h.h_path);
				if (h.h_sender)
					(void) printf(form, t_sender, h.h_sender);
				if (h.h_replyto)
					(void) printf(form, t_replyto, h.h_replyto);
				if (h.h_organisation)
					(void) printf(form, t_organization, h.h_organisation);
				verbose = false;
			}
			pheader = true;
		}
		if (!pushed && number[0])
			/*
			 * just returned from a number command
			 * and have another to do
			 */
			com = number;
		else if (pflag)
			/*
			 * just print it
			 */
			com = "";
		else
		 {
			(void) printf("? ");
			if (fflush(stdout) == EOF) {
				(void) printf("\n? ");
				(void) fflush(stdout);
			}
			interrupt = false;
			if ((com = mgets()) == NIL(char)) {
				if (interrupt)
					if (!hadinterrupt) {
						clearerr(stdin);
						(void) printf("Interrupt\n");
						hadinterrupt = true;
						interrupt = false;
						continue;
					}
					else
						exit(1);
				nextact = stop;
				break;
			}
			hadinterrupt = false;
		}
		if (*com == '!') {
			if (com[1] == '!') {
				(void) printf("!%s\n", savedsys);
				com = savedsys;
			} else
				com++;
			(void) fflush(stdout);
#ifdef F_SETFD
			fcntl(fileno(f), F_SETFD, 1);	/* close on exec */
#endif
			system(com);
			if (com != savedsys)
				strncpy(savedsys, com, sizeof(savedsys) - 1);
			(void) printf("!\n");
			if (!printed)
				pheader = false;
			continue;
		}
		/*
		 * check command syntax
		 */
		if (*com && !isdigit(*com) && com[1] && (!isspace(com[1]) ||
		    strchr("nsm", *com) == NIL(char))) {
			(void) printf(errmess);
			continue;
		}
		if (c = *com) {
			arg = com;
			while (isspace(*++arg))
				;
		} else
			arg = NIL(char);
		switch (c) {
		case 0:
		case '.':
			if (!printed || c == '.') {
				if (pflag)
					(void) printf("\n");
				print(&h, f);
				if (pflag) {
					nextact = next;
					break;
				}
				printed = true;
				continue;
			}
		case 'n':			/* B compatible */
		case '+':
		case ';':
			nextact = next;
			break;
		case '?':
			help();
			continue;
		case 'c':
			cancelarticle(&h);
			continue;
		case 'r':
			reply(&h, fname);
			continue;
		case 'm':
			if (!arg || !*arg)
				(void) printf("Person argument missing.\n");
			else
				mail(&h, fname, arg);
			continue;
		case 'f':
			followup(&h, fname);
			continue;
		case 'p':
			pnews(fname);
			continue;
		case 'U':
			if (ngmatch(np->n_name, ADMSUB)
#if MANGRPS
			 || ngmatch(np->n_name, mangrps))
#else
				)
#endif
				 {
					(void) printf("Group \"%s\" can't be unsubscribed.\n",
					     					 np->n_name);
					continue;
				}
			np->n_subscribe = false;
			nextact = nextgroup;
			break;
		case 'u':
			unsubscribe(h.h_references);
			nextact = next;
			break;
		case 'N':			/* B compatible */
			if (!*arg) {
				nextact = nextgroup;
				break;
			}
			if ((wantap = activep(arg)) == NIL(active)) {
				(void) printf("%s: non-existent newsgroup.\n", arg);
				continue;
			}
			if (!ngmatch(arg, nflag)) {
				(void) printf("%s: is not subscribed to!\n", arg);
				wantap = NIL(active);
				continue;
			}
			nextact = searchgroup;
			break;
		case 's':
			save(&h, f, arg);
			continue;
		case 'q':
			nextact = stop;
			break;
		case 'x':
			exit(0);
		case 'h':
			verbose = true;
			pheader = false;
			continue;
		case 'H':
			puthead(&h, stdout, printing);
			continue;
		case '-':
			if (pushed) {
				nextact = next;
				break;
			}
			if (!rlastap || !lastn.n_name) {
				(void) printf("Can't go back!\n");
				continue;
			}
			nextact = commands(rlastap, &lastn, false, true);
			/*
			 * number commands, after a "-" act on the
			 * group of the "-" command
			 */
			while (number[0]) {
				ntmp = lastn;
				ntmp.n_last = atoi(number) - 1;
				number[0] = '\0';
				nextact = commands(rlastap, &ntmp, false, true);
			}
			if (nextact != next)
				break;
			(void) printf("\n");
			pheader = false;
			continue;
		default:
			if (isdigit(c)) {
				i = c - '0';
				while (isdigit(*arg))
					i = i * 10 + *arg++ - '0';
			}
			if (!isdigit(c) || *arg != '\0') {
				(void) printf(errmess);
				continue;
			}
			number[0] = '\0';
			if (i < ap->a_low || i > ap->a_seq) {
				(void) printf("Articles in \"%s\" group range %d to %d.\n",
				     					np->n_name, ap->a_low, ap->a_seq);
				continue;
			}
			if (pushed) {
				sprintf(number, "%d", i);
				nextact = next;
				break;
			}
			ntmp = *np;
			ntmp.n_last = i - 1;
			if ((nextact = commands(ap, &ntmp, false, true)) !=
			    next)
				break;
			if (!number[0]) {
				(void) printf("\n");
				pheader = false;
			}
			continue;
		}
		break;
	}
	rlastap = ap;
	lastn = *np;
	if (!pushed && (nextact == next || printed)) {
		np->n_last++;
		if (ino)
			seen(NIL(FILE), &ino);
	}
	freehead(&h);
	(void) fclose(f);
	free(fname);
	return nextact;
}


/*
 * see if this is a followup we are ignoring
 */
bool
ureject(hp)
register header *hp;
{
	register bool found;
	register char *rp, *ids, c;
	char *key[1];

	found = false;
	rp = hp->h_references;
	while (*rp && !found) {
		if (ids = strpbrk(rp, " ,")) {
			c = *ids;
			*ids = '\0';
		}
		key[0] = rp;
		found = (bool) (bsearch((char *) key, (char *) uflag, (unsigned) usize,
		     sizeof(char *), strpcmp) != NIL(char));
		if (ids)
			*ids = c, rp = ids + 1;
		else
			break;
		while (isspace(*rp) || *rp == ',')
			rp++;
	}
	return found;
}


/*
 * see if the article has links, if so have we seen it?
 * close file if we return true
 *
 * called twice,
 *	first (with f set) to test (and possibly set *ino)
 *	again to put *ino in memory
 */
bool
seen(f, ino)
FILE *f;
ino_t *ino;
{
	static int num;
	static ino_t	*ilist;
	struct stat statb;
	register int i;

	if (f) {
		if (fstat(fileno(f), &statb) != 0 || statb.st_nlink <= 1)
			return false;
		for (i = 0; i < num; i++)
			if (ilist[i] == statb.st_ino) {
				(void) fclose(f);
				return true;
			}
		*ino = statb.st_ino;
		return false;
	} else if (*ino) {
		num++;
		ilist = (ino_t * ) (ilist ? myrealloc((char *) ilist, (int) sizeof(ino_t) *
		    num) : myalloc((int) sizeof(ino_t)));
		ilist[num - 1] = *ino;
	}
	return true;
}


/*
 * print out help file
 */
help()
{
	register FILE	*f;
	register int c;
	static char helppath[]	 = HELP;

	if ((f = fopen(helppath, "r")) == NIL(FILE)) {
		(void) printf("Can't open %s\n", helppath);
		return;
	}
	while ((c = getc(f)) != EOF)
		(void) putc(c, stdout);
	(void) fclose(f);
}

/*
 * cancel an article.
 * inews does permission checking.
 */
cancelarticle(hp)
header *hp;
{
	char *argv[MAXARGV];

	argv[0] = strrchr(postnews, '/') + 1;
	argv[1] = "-c";		/* TODO: no such option in C news */
	argv[2] = newstr2("cancel ", hp->h_messageid);
	argv[3] = "-n";
	argv[4] = hp->h_newsgroups;
	if (hp->h_distribution) {
		argv[5] = "-d";
		argv[6] = hp->h_distribution;
		argv[7] = NIL(char);
	} else
		argv[5] = NIL(char);
	run(postnews, argv, true);
	free(argv[2]);
}

/*
 * reply to sender by mail
 */
/* ARGSUSED fname */
reply(hp, fname)
header *hp;
char *fname;
{
	char *argv[MAXARGV];
	register int argc;

	argc = 0;
	argv[argc++] = "mail";
#ifdef UNSWMAIL
	argv[argc++] = "-s";
	if ((argv[argc++] = getsubject(hp)) == NIL(char))
		return;
	argv[argc++] = "-i";
	argv[argc++] = fname;
#endif

	if ((argv[argc++] = getretaddr(hp)) == NIL(char)) {
		(void) printf("Can't work out an address!\n");
		return;
	}

	argv[argc++] = NIL(char);

	run(mailpath, argv, false);

	free(argv[argc - 2]);
}


/*
 * mail to person
 */
/* ARGSUSED hp fname */
mail(hp, fname, person)
header *hp;
char *fname, *person;
{
	char *argv[MAXARGV];
	register int argc;

	argc = 0;
	argv[argc++] = "mail";
#ifdef UNSWMAIL
	argv[argc++] = "-i";
	argv[argc++] = fname;
	argv[argc++] = "-s";
	argv[argc++] = hp->h_subject;
#endif
	argv[argc++] = person;
	argv[argc++] = NIL(char);

	run(mailpath, argv, false);
}


/*
 * generate correct headers for a followup article
 * then call inews.
 */
followup(hp, fname)
header *hp;
char *fname;
{
	register int argc;
	char *argv[10];

	argc = 0;
	argv[argc++] = strrchr(postnews, '/') + 1;
	argv[argc++] = "-i";		/* TODO: what's this in B news? */
	argv[argc++] = fname;
	argv[argc++] = "-r";
	if (hp->h_references && hp->h_messageid)
		argv[argc++] = newstr3(hp->h_references, " ", hp->h_messageid);
	else if (hp->h_messageid)
		argv[argc++] = newstr(hp->h_messageid);
	else
		argc--;

	argv[argc++] = "-s";
	if ((argv[argc++] = getsubject(hp)) == NIL(char))
		return;

	argv[argc++] = "-n";
	if (hp->h_followupto)
		argv[argc++] = hp->h_followupto;
	else
		argv[argc++] = hp->h_newsgroups;
	argv[argc++] = NIL(char);

	run(postnews, argv, false);

	if (argc == 10)
		free(argv[4]);
}

/*
 * get correct "Subject: Re: .." line
 */
char *
getsubject(hp)
register header *hp;
{
	register char *s;

	if (!hp->h_subject) {
		(void) printf("Subject: Re: ");
		(void) fflush(stdout);
		if ((s = mgets()) == NIL(char) || !*s) {
			(void) printf("The Subject field is mandatory.\n");
			return NIL(char);
		}
		return newstr2("Re: ", s);
	} else if (CMPN(hp->h_subject, "Re: ", 4) != 0 && CMPN(hp->h_subject,
	     "re: ", 4) != 0)
		return newstr2("Re: ", hp->h_subject);
	else
		return hp->h_subject;
}


/*
 * run a command, optionally closing stdin
 */
run(com, argv, closein)
char *com;
char *argv[];
bool closein;
{
	int pid, status, r;

	switch (pid = fork()) {
	default:
		/* parent */
		break;
	case 0:
		/* child */
		if (closein)
			close(fileno(stdin));
		execvp(com, argv);
		error("can't exec %s", com);
		exit(1);

	case -1:
		error("can't fork");
	}

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGINT, SIG_IGN);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGQUIT, SIG_IGN);

	while ((r = wait(&status)) != pid && r != -1)
		;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGINT, onintr);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		(void) signal(SIGQUIT, onintr);
}

/*
 * call postnews
 */
pnews(fname)
char *fname;
{
	char *argv[MAXARGV];

	argv[0] = strrchr(postnews, '/') + 1;
	argv[1] = "-i";		/* TODO: what's this inews option? */
	argv[2] = fname;
	argv[3] = NIL(char);
	run(postnews, argv, false);
}

/*
 * save an article
 */
save(hp, f, s)
header *hp;
FILE *f;
char *s;
{
	register long pos;
	register int c;
	register char *cp;
	register FILE	*sf;
	register char *aname;
	long then;
	extern char *getenv();

	if (!*s) {
		if ((aname = getenv("HOME")) == NIL(char)) {
			(void) printf("No $HOME in environment.\n");
			return;
		}
		s = aname = newstr3(aname, "/", ARTICLES);
	} else
		aname = NIL(char);
	if ((sf = fopen(s, "a")) == NIL(FILE)) {
		(void) fprintf(stderr, "readnews: can't open %s\n", s);
		return;
	}
	if (aname)
		free(aname);
	pos = ftell(f);
	rewind(f);
	if (cp = strchr(hp->h_from, ' '))
		*cp = '\0';
	if (hp->h_date)
		then = atot(hp->h_date);
	else
		then = 0L;
	(void) fprintf(sf, "From %s %s", hp->h_from, ctime(then ? &then : &now));
	if (cp)
		*cp = ' ';
	while ((c = getc(f)) != EOF)
		(void) putc(c, sf);
	(void) fclose(sf);
	fseek(f, pos, 0);
}


/*
 * unsubscribe from all followup articles
 * on this topic
 */
unsubscribe(id)
char *id;
{
	register char *s, c;

	if (!id || !*id) {
		(void) printf("no references! (warning)\n");
		return;
	}
	while (*id) {
		if (s = strpbrk(id, " ,")) {
			c = *s;
			*s = '\0';
		}
		usize++;
		uflag = (uflag ? (char **) myrealloc((char *) uflag, (int) sizeof(char
		    *) * usize) : (char **) myalloc((int) sizeof(char *)));
		uflag[usize - 1] = newstr(id);
		if (s)
			*s = c, id = s + 1;
		else
			break;
		while (isspace(*id) || *id == ',')
			id++;
	}
	qsort((char *) uflag, (unsigned) usize, sizeof(char *), strpcmp);
}


/*
 * print an article, if it's long enough call page()
 */
print(hp, f)
header *hp;
FILE *f;
{
	register int c;
	register long pos;

	pos = ftell(f);
	if (!pflag && hp->h_lines && atoi(hp->h_lines) >= PAGESIZE - 4)
		page(f);
	else
		while ((c = getc(f)) != EOF)
			(void) putc(c, stdout);
	fseek(f, pos, 0);
}


/*
 * copy article text to stdout, and break into pages
 */
page(f)
FILE *f;
{
	static char moremess[]	 = "<CR for more>";

	register int c;
	register unsigned lineno;
	char lbuf[80];
	struct sgttyb ttybuf;

	gtty(fileno(stdin), &ttybuf);
	if (ttybuf.sg_flags & ECHO) {
		ttybuf.sg_flags &= ~ECHO;
		stty(fileno(stdin), &ttybuf);
		ttybuf.sg_flags |= ECHO;
	}
	lineno = 1;
	while (!interrupt) {
		while (lineno < PAGESIZE - 4 && !interrupt) {
			while ((c = getc(f)) != EOF && c != '\n')
				(void) putc(c, stdout);
			if (c == EOF)
				goto fastexit;
			(void) putc('\n', stdout);
			lineno++;
		}
		if (interrupt)
			break;
		(void) printf(moremess);
		if (fflush(stdout) == EOF)
			break;
		(void) read(fileno(stdin), lbuf, sizeof(lbuf));	/* TODO: fix */
		(void) printf("\r%*s\r", sizeof(moremess), " ");
		lineno = 0;
	}
	if (lineno)
		(void) putc('\n', stdout);
	interrupt = false;
fastexit:
	if (ttybuf.sg_flags & ECHO)
		stty(fileno(stdin), &ttybuf);
}

/* VARARGS1 */
error(s, a0, a1, a2, a3)
char *s;
{
	(void) fprintf(stderr, "readnews: ");
	(void) fprintf(stderr, s, a0, a1, a2, a3);
	(void) fprintf(stderr, "\n");
	exit(1);
}
