/*
 * postnews [-h] [-s subject] [-n newsgroups] [-e expiredate]
 *		 [-r references] [-i interpfile] [-d distribution]
 * postnews -c control_command [-n newsgroups] [-d distribution]
 * postnews -p
 *
 *	SETUID to NEWSROOT "news"
 *
 *	Michael Rourke (UNSW) April 1984
 */

#include "defs.h"

char sys[]		 = SYS;
extern char mydomain[];
extern char newsversion[];
#ifndef NETID
char systemid[DIRSIZ];
#else
char systemid[] = NETID;
#endif
#ifdef UUNAME
char uuname[] = UUNAME;
#endif

char *sflag;		/* subject */
char *nflag;		/* newsgroups */
char *dflag;		/* distribution */
long eflag;		/* expire date */
char *cflag;		/* control function */
char *rflag;		/* references */
char *iflag;		/* interp file */
bool		hflag;		/* headers in input */
bool		pflag;		/* from other host (su only) */
bool		tty;		/* isatty */

#if AUSAM
struct pwent pe;		/* current user passwd struct */
char sbuf[SSIZ];	/* strings thereof */
#else
struct passwd *pp;		/* current user passwd struct */
#endif
long now;		/* current time */
int oumask;		/* old umask */
header		h;		/* current articles header */
bool		su;		/* if news super-user */
char *tname;		/* name of temp file */
FILE		*tmp;		/* file ptr */
char *newsdir;	/* %news */
uid_t		newsuid;	/* news uid */

int checkng(), linkng();
char *dfltgrp();
extern FILE	*mailreply(), *mailnewsroot();
extern bool	chkhist(), cancel();

main(argc, argv)
int argc;
char *argv[];
{
	register int i;
	extern long maketime();

	time(&now);
	for (argv++, argc--; argc > 0; argc--, argv++) {
		if (argv[0][0] != '-' && argv[0][2] != '\0')
			break;
		switch (argv[0][1]) {
		case 'h':
			hflag = true;
			continue;
		case 'p':
			pflag = true;
			continue;
		case 's':
			sflag = argv[1];
			break;
		case 'n':
			nflag = argv[1];
			break;
		case 'r':
			rflag = argv[1];
			break;
		case 'i':
			iflag = argv[1];
			break;
		case 'd':
			dflag = argv[1];
			break;
		case 'e':
			for (i = 1; i < argc; i++)
				if (argv[i][0] == '-' && argv[i][2] == '\0')
					break;
			if (i == 1) {
				argc = -1;
				break;
			}
			i--;
			if ((eflag = maketime(i, &argv[1], TIMES)) == 0L)
				exit(1);
			argc -= i, argv += i;
			continue;
		case 'c':
			cflag = argv[1];
			break;
		default:
			argc = -1;
			break;
		}
		argv++, argc--;
	}
	if (argc != 0 || (cflag && pflag) || (pflag && (nflag || dflag)) ||
	    ((cflag || pflag) && (hflag || sflag || eflag || rflag || iflag))) {
		fprintf(stderr, "Usage: postnews [-h] [-s subject] [-n newsgroups] [-e expiredate ..]\n");
		fprintf(stderr, "                [-r references] [-i interpfile] [-d distribution]\n");
		fprintf(stderr, "       postnews -c control_command [-n newsgroups] [-d distribution]\n");
		fprintf(stderr, "       postnews -p\n");
		exit(1);
	}
	oumask = umask(022);

#if AUSAM
	pe.pw_strings[LNAME] = NEWSROOT;
	if (getpwuid(&pe, sbuf, sizeof(sbuf)) == PWERROR)
		error("Password file error.");
	newsdir = newstr(pe.pw_strings[DIRPATH]);
	newsuid = pe.pw_limits.l_uid;
#else
	if ((pp = getpwnam(NEWSROOT)) == (struct passwd *) NULL)
		error("Password file error.");
	newsdir = newstr(pp->pw_dir);
	newsuid = pp->pw_uid;
#endif

#if AUSAM
	pe.pw_limits.l_uid = getuid();
	if (getpwlog(&pe, sbuf, sizeof(sbuf)) == PWERROR)
		error("Password file error.");
	pwclose();
	su = (bool) (pe.pw_limits.l_uid == 0 || pe.pw_limits.l_uid == newsuid);
#else
	if ((pp = getpwuid(getuid())) == (struct passwd *) NULL)
		error("Password file error.");
	endpwent();
	su = (bool) ((pp->pw_uid == 0) || (pp->pw_uid = newsuid));
#endif

#ifndef NETID
	getaddr(G_SYSNAME, systemid);
#endif
	tty = (bool) isatty(fileno(stdin));

#if AUSAM
	if (!su && (pe.pw_limits.l_flags & USENET) == 0)
		error("Net permission is required to post news.");
#endif

	if (!su && pflag)
		error("Permission denied.");

	if (pflag || hflag)
		gethead(stdin, &h);
	if (sflag)
		h.h_subject = sflag;
	if (cflag) {
		h.h_control = cflag;
		h.h_subject = "Control";
		if (!h.h_replyto)
			h.h_replyto = newstr5(
#if AUSAM
					pe.pw_strings[LNAME],
#else
					pp->pw_name,
#endif
				"@", systemid, ".", mydomain);
	}
	if (nflag)
		h.h_newsgroups = nflag;
	if (rflag)
		h.h_references = rflag;
	if (eflag)
		if (eflag < now)
			error("Time specified has passed.");
		else
			h.h_expires = newstr(ttoa(eflag));
	if (dflag)
		h.h_distribution = dflag;
	if (tty)
		askheads();
	if (h.h_newsgroups)
		convgrps(h.h_newsgroups);
	else
		h.h_newsgroups = DFLTGRP;
	if (h.h_distribution) {
		convgrps(h.h_distribution);
		if (CMP(h.h_newsgroups, h.h_distribution) == 0)
			h.h_distribution = NIL(char);
	}
	if (!h.h_subject)
		error("No subject specified.");
	if (pflag) {
		if (chkhist(h.h_messageid))
			error("Duplicate article %s rejected.", h.h_messageid);
	} else
	 {
		if (h.h_relayversion || h.h_postversion || h.h_from || h.h_date ||
		    h.h_messageid || h.h_path || h.h_sender || h.h_datereceived ||
		    h.h_lines)
			error("Illegally specified field(s).");

		if (!su && ngmatch(h.h_newsgroups, MODGROUPS))
			error("Moderated newsgroup:\n\tArticle must be mailed to the newsgroup moderator.");

		if (!cflag && applyng(h.h_newsgroups, checkng))
			exit(1);
		if (!cflag && h.h_distribution && applyng(h.h_distribution,
		    checkng))
			exit(1);
	}
	install(&h, stdin);
	exit(0);
}


/*
 * create all the directories required for a given group
 */
creatgroup(grp)
char *grp;
{
	register char *dname;
	register char *s, *slash;

	if (strpbrk(grp, BADGRPCHARS))
		error("%s: Illegal char in newsgroup.", grp);
	initgrp(grp);		/* make entry in active file */
	dname = convg(newstr3(newsdir, "/", grp));
	s = dname + strlen(newsdir);
	while (*s) {
		slash = strchr(s, '/');
		if (slash)
			*slash = '\0';
		if (access(dname, 0) != 0)
			mkdir(dname);
		if (slash)
			*slash = '/', s = slash + 1;
		else
			break;
	}
	free(dname);
}


/*
 * create dir
 */
mkdir(s)
char *s;
{
	int pid, status, r;
	int (*istat)(), (*qstat)();

	switch (pid = fork()) {
	default:
		/* parent */
		break;
	case 0:
		/* child */
		execl(MKDIR, "mkdir", s, 0);
		error("Can't exec %s", MKDIR);
		exit(1);
	case -1:
		error("Can't fork.");
	}

	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);

	while ((r = wait(&status)) != pid && r != -1)
		;
	if (r == -1 || status)
		error("Couldn't mkdir %s", s);

	if (getuid() == 0) {
		switch (pid = fork()) {
		default:
			/* parent */
			break;
		case 0:
			/* child */
			setgid(0);
			setuid(0);
			execl(CHOWN, "chown", NEWSROOT, s, 0);
			error("Can't exec %s", CHOWN);
			exit(1);
		case -1:
			error("Can't fork.");
		}
		while ((r = wait(&status)) != pid && r != -1)
			;
		if (r == -1 || status)
			error("Couldn't chown %s", s);
	}

	chmod(s, 0755);

	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
}


/*
 * get unset headers from stdin
 */
askheads()
{
	static char form[]	 = "%s: %s\n";

	char *geth();

	extern char t_subject[], t_newsgroups[], t_distribution[];
	extern char t_references[], t_expires[];

	if (h.h_subject)
		printf(form, t_subject, h.h_subject);
	else
		h.h_subject = geth(t_subject, NIL(char));
	if (h.h_newsgroups)
		printf(form, t_newsgroups, h.h_newsgroups);
	else
		h.h_newsgroups = geth(t_newsgroups, dfltgrp());
	if (h.h_distribution)
		printf(form, t_distribution, h.h_distribution);
	else
		h.h_distribution = geth(t_distribution, h.h_newsgroups);
	if (h.h_expires)
		printf(form, t_expires, h.h_expires);
	if (h.h_references)
		printf(form, t_references, h.h_references);
}


/*
 * get a header from stdin
 */
char *
geth(fname, def)
char *fname, *def;
{
	register char *s;

	while (1) {
		if (def)
			printf("%s (%s): ", fname, def);
		else
			printf("%s: ", fname);
		if ((s = mgets()) && *s)
			return newstr(s);
		if (def)
			return newstr(def);
		printf("%s field is mandatory.\n", fname);
	}
}


/*
 * install the news item
 */
install(hp, f)
register header *hp;
FILE *f;
{
	register FILE	*sf;
	register int c;
	register char *mach, *subs, *type, *com, *end;
	char buf[BUFLEN];

	int cleanup();
	FILE		 * getbody(), *trimit();

	signal(SIGINT, cleanup);
	signal(SIGQUIT, cleanup);

	if (tty && !cflag)
		f = getbody(f);

	if (!hp->h_lines && !cflag)
		f = trimit(hp, f);

	if ((tname = mtempnam(newsdir, "itmp")) == NIL(char) || (tmp = fopen(tname,
	     "w+")) == NIL(FILE))
		error("Can't create itmp file.");

	chown(tname, (int) newsuid, (int) newsuid);	/* in case we are currently root */

	if (pflag)
		puthead(hp, tmp, passing);
	else
	 {
		h.h_messageid = newstr5("<", getunique(), "@", systemid, ".");
		h.h_messageid = catstr2(h.h_messageid, mydomain, ">");
		puthead(hp, tmp, making);
	}

	putc('\n', tmp);
	if (!cflag)
		while ((c = getc(f)) != EOF)
			putc(c, tmp);

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	sf = fopenf(sys, "r");
	while (fgets(buf, sizeof(buf), sf)) {
		mach = buf;
		if ((subs = strchr(buf, ':')) == NIL(char) || (type = strchr(subs +
		    1, ':')) == NIL(char) || (com = strchr(type + 1, ':')) == NIL(char)
		    || (end = strchr(com + 1, '\n')) == NIL(char))
			error("%s: Bad format.", sys);
		*subs++ = *type++ = *com++ = *end = '\0';
		if ((hp->h_distribution && ngmatch(hp->h_distribution, subs)
		    || !hp->h_distribution) && ngmatch(hp->h_newsgroups, subs))
			if (CMP(mach, systemid) == 0)
				local(hp, tname, subs);
			else if ((pflag && checkpath(hp->h_path, mach) || !pflag)
			    && remote(tmp, com))
				printf("Couldn't transmit to %s\n", mach);
	}
	fclose(sf);
	fclose(tmp);
	unlink(tname);
}


/*
 * have to count the number of lines
 * and trim leading and trailing empty lines
 */
FILE *
trimit(hp, f)
header *hp;
FILE *f;
{
	register int ccount, lcount, llcount;
	register int c, lastc;

	ccount = lcount = llcount = 0;
	if ((tmp = tmpfile()) == NIL(FILE))
		error("Can't create tempfile.");
	while ((c = getc(f)) != EOF && c == '\n')
		;
	lastc = c;
	if (c != EOF)
		do
		 {
			if (c == '\n' && lastc == '\n') {
				llcount++;
				continue;
			}
			while (llcount > 0) {
				putc('\n', tmp);
				lcount++;
				llcount--;
			}
			if (isprint(c))
				putc(c, tmp), ccount++;
			else if (isspace(c) || c == '\b') {
				if (c == '\n')
					lcount++;
				putc(c, tmp);
			} else
				putc('?', tmp);
			lastc = c;
		} while ((c = getc(f)) != EOF);
	if (ccount < 5 && !pflag)
		error("Article too short.");
	hp->h_lines = newstr(itoa(lcount));
	rewind(tmp);
	fclose(f);
	return tmp;
}


/*
 * input new article interactivly
 * and place on standard input
 */
FILE *
getbody(f)
FILE *f;
{
	register int c;
	char buf[BUFSIZ];
	bool		quit;

	printf("\n");
	quit = false;
	if ((tname = mtempnam(newsdir, "itmp")) == NIL(char) || (tmp = fopen(tname,
	     "w+")) == NIL(FILE))
		error("Can't create itmp file.");
	/*
	 * read article text, interpreting escape commands
	 */
	while (!quit && (c = getc(f)) != EOF)
		if (c == '.' || c == '!') {
			switch (c = (c == '!' ? c : getc(f))) {
			case 'e':
				fclose(tmp);
				readln(f);
				doed(tname);
				if ((tmp = fopen(tname, "a+")) == NIL(FILE))
					error("Can't re-open %s", tname);
				break;
			case 'i':
				if (!iflag) {
					readln(f);
					printf("Can't interpolate.\n");
					break;
				}
				icopy(iflag, tmp);
				break;
			case '!':
				msystem(mgets());
				break;
			case '\n':
				quit = true;
				continue;
			default:
				printf("Unknown escape command: \"%c\"\n", c);
				printf("Commands are:\n");
				printf("\t.e          - edit article so far\n");
				printf("\t.i          - interpolate article\n");
				printf("\t.!command   - shell escape\n");
				readln(f);
			}
			printf("(continue)\n");
			fflush(stdout);
		}
		else
		 {
			if (c == '\\' && (c = getc(f)) != '.')
				fprintf(tmp, "\\");
			fprintf(tmp, "%c", c);
			if (c != '\n' && fgets(buf, sizeof buf, f) != NIL(char))
				fputs(buf, tmp);
		}
	rewind(tmp);
	fclose(f);
	unlink(tname);
	free(tname);
	tname = NIL(char);
	return tmp;
}


/*
 * interpolate fname
 */
icopy(fname, to)
char *fname;
FILE *to;
{
	register FILE	*f;
	register int c, lastc;

	if ((f = fopen(fname, "r")) == NIL(FILE)) {
		perror(fname);
		return;
	}
	lastc = '\n';
	while ((c = getc(f)) != EOF) {
		if (lastc == '\n')
			fprintf(to, "    ");
		putc(lastc = c, to);
	}
	fclose(f);
}


/*
 * invoke an editor on fname
 */
doed(fname)
char *fname;
{
	register int i;
	register char *editor;
	register char *edname;
	int pid, stat;
	extern char *getenv();
	static char ed[] 		 = ED;

	chmod(fname, 0660);
	if ((pid = fork()) == 0) {
		setuid(getuid());
		if ((editor = getenv("EDITOR")) == NIL(char))
			editor = ed;
		edname = strrchr(editor, '/');
		edname = (edname ? edname + 1 : editor);
		execl(editor, edname, fname, 0);
		perror(editor);
		exit(1);
	}
	if (pid == -1) {
		error("Can't fork ed.");
		return;
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	while ((i = wait(&stat)) != pid && i != -1)
		;

	chmod(fname, 0644);

	signal(SIGINT, cleanup);
	signal(SIGQUIT, cleanup);
}


/*
 * remove temp file, and perhaps save in $HOME/dead.article
 */
cleanup(sig)
int sig;
{
	if (sig)
		printf("\nInterrupt\n");
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (tname && *tname) {
		unlink(tname);
		tname = NIL(char);
		if (tty && tmp)
			saveit();
	}
	exit(1);
}


/*
 * save the current temporary file
 * in $HOME/dead.article
 */
saveit()
{
	register FILE	*f;
	register char *nname;
	register int c;

	extern char *getenv();

	rewind(tmp);
	if ((nname = getenv("HOME")) == NIL(char))
		return;
	nname = newstr2(nname, "/dead.article");

	umask(oumask);
	setgid(getgid());
	setuid(getuid());

	f = fopenf(nname, "w");
	h.h_messageid = NIL(char);
	puthead(&h, f, printing);
	putc('\n', f);
	while ((c = getc(tmp)) != EOF)
		putc(c, f);
	fclose(f);
	printf("Article saved in %s\n", nname);
	free(nname);
}


/*
 * check that mach is not in path
 * so we won't send it back again
 */
checkpath(path, mach)
char *path;
char *mach;
{
	register char *ex;
	register int r;

	while (*path && (ex = strchr(path, PSEPCHAR))) {
		*ex = '\0';
		r = (CMP(path, mach) == 0);
		*ex = PSEPCHAR;
		if (r)
			return 0;
		path = ex + 1;
	}
	return 1;
}


/* VARARGS1 */
error(s, a0, a1, a2, a3)
char *s;
{
	fprintf(stderr, "postnews: ");
	fprintf(stderr, s, a0, a1, a2, a3);
	fprintf(stderr, "\n");
	cleanup(0);
}


/*
 * check that a newsgroup exists
 */
checkng(g)
char *g;
{
	register char *dname;
	register char *s, *com;

	dname = convg(newstr3(newsdir, "/", g));
	free(dname);
	if (access(dname, 0) != 0) {
		if (su && tty) {
			printf("%s: Nonexistent newgroup. Create? ", g);
			if ((s = mgets()) == NIL(char) || !*s || (CMP(s, "y") !=
			    0 && CMP(s, "yes") != 0))
				return 1;
			com = newstr6("exec ", POSTNEWS, " -c 'newgroup ", g,
			     				 "' -n ", g);
			system(com);
			free(com);
			return 0;
		}
		printf("%s: Nonexistent newsgroup.\n", g);
		return 1;
	}
	return 0;
}


/*
 * place news locally
 * by linking tmp file into each newsgroup directory
 */
local(hp, tmpname, subs)
header *hp;
char *tmpname;
char *subs;
{
	static char junk[] = "junk";

	register char *newg;
	register FILE	*pp;

	if (hp->h_control)
		control(hp->h_control);
	else if (newg = ngsquash(hp->h_newsgroups, subs)) {
		openhist(hp);
		if (!applyng(newg, linkng, tmpname)) {
			while (!linkng(junk, tmpname))
				creatgroup(junk);
			pp = mailnewsroot("Postnews: Article placed in \"junk\"");
			fprintf(pp, "Article placed in \"junk\" because no groups active from:\n\t%s\n",
			     				newg);
			mailclose(pp);
		}
		closehist();
		free(newg);
	} else
		error("Snark: in local");
}


/*
 * link news items into local dirs
 */
linkng(g, t)
char *g, *t;
{
	register char *lname, *dname;
	extern char *getseq();

	dname = convg(newstr3(newsdir, "/", g));
	free(dname);
	if (access(dname, 0) != 0)
		return 0;		/* don't save - not an active group */
	dname = convg(newstr3(g, "/#", getseq(g)));
	writehist(dname);
	lname = newstr3(newsdir, "/", dname);
	if (link(t, lname) != 0)
		error("Link from %s to %s failed.", t, lname);
	free(lname);
	free(dname);
	return 1;
}


/*
 * execute control messages
 */
control(com)
char *com;
{
	register char *s;
	register FILE	*pp;

	if (s = strchr(com, ' ')) {
		*s++ = '\0';
		while (*s && isspace(*s))
			s++;
	}
	if (CMP(com, "cancel") == 0) {
		if (!s || !*s)
			error("cancel: message-id not specified.");
		cancel(s);
	} else if (CMP(com, "newgroup") == 0 || CMP(com, "rmgroup") == 0) {
		if (!s || !*s)
			error("%s: group name not specified.", com);
		if (!su)
			error("%s: permission denied.", com);
		else if (CMP(com, "newgroup") == 0)
			creatgroup(s);
		else
		 {
			pp = mailnewsroot("Postnews: rmgroup request");
			fprintf(pp, "rmgroup %s, sent from %s\n", s, (h.h_replyto ?
			    h.h_replyto : (h.h_from ? h.h_from  : "Unknown?")));
			fprintf(pp, "run '/usr/lib/news/rmgroup %s' if sender is authorised.\n",
			     s);
			mailclose(pp);
		}
	} else if (CMP(com, "sendsys") == 0)
		sendsys();
	else if (CMP(com, "version") == 0)
		version();
	else if (CMP(com, "senduuname") == 0)
		senduuname();
	else
	 {
		sorry(com);
		error("Unknown control command: %s %s\n(Valid: cancel, newgroup, sendsys, senduuname, version)\nSent from: %s",
		     			com, s ? s : "", (h.h_replyto ? h.h_replyto : (h.h_from ?
		    h.h_from : "Unknown?")));
	}
}


/*
 * send sys file to originator
 */
sendsys()
{
	register FILE	*pp, *fp;
	register int c;

	fp = fopenf(sys, "r");
	pp = mailreply("News sendsys request");
	while ((c = getc(fp)) != EOF)
		putc(c, pp);
	mailclose(pp);
	fclose(fp);
}


/*
 * send version name and number to originator
 */
version()
{
	register FILE	*pp;

	pp = mailreply("News version request");
	fprintf(pp, "Current version: %s\n", newsversion);
	mailclose(pp);
	fclose(pp);
}


/*
 * send uuname data to originator
 */
senduuname()
{
	register FILE	*pp, *f;
	register int c;

	extern FILE	*tmpfile();

#ifndef UUNAME
	sorry("uuname");
#else
	if ((pp = popen(uuname, "r")) == NIL(FILE))
		error("Couldn't run \"%s\"", uuname);
	if ((f = tmpfile()) == NIL(FILE))
		error("Can't open tmp file.");

	while ((c = getc(pp)) != EOF)
		putc(c, f);

	if (pclose(pp) != 0)
		error("\"%s\" had bad exit status.", uuname);
	rewind(f);

	pp = mailreply("News senduuname request");
	while ((c = getc(f)) != EOF)
		putc(c, pp);

	fclose(f);
	mailclose(pp);
#endif
}


/*
 * send message about unimplemented command
 */
sorry(com)
char *com;
{
	register FILE	*pp;

	pp = mailreply("Unimplemented news control message");
	fprintf(pp, "The control message \"%s\" is not implemented at this site.\n",
	     com);
	fprintf(pp, "Our current version of news is: %s\n", newsversion);
	mailclose(pp);
}


/*
 * set up a pipe to a mail program to reply to control requests
 */
FILE *
mailreply(s)
char *s;
{
	register FILE	*pp;
	register char *com, *ra;

	if ((ra = getretaddr(&h)) == NIL(char))
		error("Can't form return address for control message");
	com = newstr4("exec ", MAIL, " ", ra);
	if ((pp = popen(com, "w")) == NIL(FILE))
		error("Couldn't run \"%s\"", com);
	fprintf(pp, "Subject: %s\n", s);
	fprintf(pp, "Responding-system: %s.%s\n\n", systemid, mydomain);
	free(com);
	free(ra);
	return pp;
}


/*
 * set up a pipe to mail to NEWSROOT
 */
FILE *
mailnewsroot(s)
char *s;
{
	register FILE	*pp;
	register char *com;

	com = newstr4("exec ", MAIL, " ", NEWSROOT);
	if ((pp = popen(com, "w")) == NIL(FILE))
		error("Couldn't run \"%s\"", com);
	fprintf(pp, "Subject: %s\n", s);
	free(com);
	return pp;
}


/*
 * close the mail pipe
 */
mailclose(pp)
FILE *pp;
{
	if (pclose(pp) != 0)
		error("Mail failed");
}


/*
 * send item to remote hosts
 */
remote(f, com)
FILE *f;
char *com;
{
	register int c;
	FILE		 * out;

	if ((out = popen(com, "w")) == NIL(FILE))
		return 1;
	rewind(f);
	while ((c = getc(f)) != EOF)
		putc(c, out);
	return pclose(out);
}


/*
 * shell escape
 */
msystem(s)
char *s;
{
	int status, pid, w;

	if ((pid = fork()) == 0) {
		close(fileno(tmp));
		/*
 		 * remember we are setuid to NEWS so...
		 */
		umask(oumask);
		setgid(getgid());
		setuid(getuid());
		execl(SHELL, "sh", "-c", s, 0);
		_exit(127);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;

	signal(SIGINT, cleanup);
	signal(SIGQUIT, cleanup);
	return(status);
}


/*
 * work out the default group for this user
 */
char *
dfltgrp()
{
#if MANGRPS
	register char **sp;
	extern char **getclasses();

	for (sp = getclasses(pe.pw_cmask); *sp; sp++) {
		if (CMP(*sp, "C-Staff") == 0 || CMP(*sp, "System") == 0)
			return "system";
		if (CMP(*sp, "CLUBS") == 0)
			return newstr2("general.club.", pe.pw_strings[LNAME]);
		if (CMP(*sp, "Languages") == 0)
			return newstr2("general.lang.", pe.pw_strings[LNAME]);
		if (CMP(*sp, "Classaccount") == 0)
			return newstr2("class.", pe.pw_strings[LNAME]);
	}
#endif
	return DFLTGRP;
}
