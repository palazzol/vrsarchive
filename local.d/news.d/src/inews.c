/*
 * inews - insert, receive, and transmit news articles.
 */

#ifndef lint
static char	*SccsId = "@(#)inews.c	2.44	9/3/84";
#endif !lint

#include "iparams.h"

/* local defines for inews */

#define OPTION	0	/* pick up an option string */
#define STRING	1	/* pick up a string of arguments */

#define UNKNOWN 0001	/* possible modes for news program */
#define UNPROC	0002	/* Unprocessed input */
#define PROC	0004	/* Processed input */
#define	CANCEL	0010	/* Cancel an article */
#define	CREATENG 0020	/* Create a new newsgroup */

char	forgedname[NAMELEN];	/* A user specified -f option. */
extern char histline[];

char *Progname = "inews";	/* used by xerror to identify failing program */

struct {			/* options table. */
	char	optlet;		/* option character. */
	char	filchar;	/* if to pickup string, fill character. */
	int	flag;		/* TRUE if have seen this opt. */
	int	oldmode;	/* OR of legal input modes. */
	int	newmode;	/* output mode. */
	char	*buf;		/* string buffer */
} *optpt, options[] = { /*
optlet	filchar		flag	oldmode	newmode		buf	*/
't',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.title,
'n',	NGDELIM,	FALSE,	UNPROC,	UNKNOWN,	header.nbuf,
'd',	'\0',		FALSE,	UNPROC,	UNKNOWN,	header.distribution,
'e',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.expdate,
'p',	'\0',		FALSE,	UNKNOWN,PROC,		filename,
'f',	'\0',		FALSE,	UNPROC,	UNKNOWN,	forgedname,
'F',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.followid,
'c',	'\0',		FALSE,	UNKNOWN,CANCEL,		filename,
'C',	'\0',		FALSE,	UNKNOWN,CREATENG,	header.nbuf,
#define hflag	options[9].flag
'h',	'\0',		FALSE,	UNPROC,	UNKNOWN,	filename,
'\0',	'\0',		0,	0,	0,		(char *)NULL
};

FILE *mailhdr();
char *any();
char *genversion();

extern log(char *fmt, ...);

/*
 *	Authors:
 *		Matt Glickman	glickman@ucbarpa.Berkeley.ARPA
 *		Mark Horton	mark@cbosgd.UUCP
 *		Stephen Daniels	swd@mcnc.UUCP
 *		Tom Truscott	trt@duke.UUCP
 *	IHCC version adapted by:
 *		Larry Marek	larry@ihuxf.UUCP
 */
main(argc, argv)
int	argc;
register char **argv;
{
	int	state;		/* which type of argument to pick up	*/
	int	tlen, len;	/* temps for string processing routine	*/
	register char *ptr;	/* pointer to rest of buffer		*/
	int	filchar;	/* fill character (state = STRING)	*/
	char	*user, *home;	/* environment temps			*/
	struct passwd	*pw;	/* struct for pw lookup			*/
	struct group	*gp;	/* struct for group lookup		*/
	register int	i;
	FILE	*mfd;		/* mail file file-descriptor		*/
	char	cbuf[BUFLEN];	/* command buffer			*/

	/* uuxqt doesn't close all it's files */
	for (i = 3; !close(i); i++)
		;
	/* set up defaults and initialize. */
	mode = UNKNOWN;
	pathinit();
	ptr = rindex(*argv, '/');
	if (!ptr)
		ptr = *argv - 1;
	if (!strncmp(ptr+1, "rnews", 5))
		mode = PROC;
	else if (argc < 2)
		goto usage;

	state = OPTION;
	header.title[0] = header.nbuf[0] = filename[0] = '\0';

	/* check for existence of special files */
	if (!rwaccess(ARTFILE)) {
		mfd = mailhdr((struct hbuf *)NULL, exists(ARTFILE) ? "Unwritable files!" : "Missing files!");
		if (mfd != NULL) {
			fprintf(mfd,"System: %s\n\nThere was a problem with %s!!\n", FULLSYSNAME, ARTFILE);
			sprintf(cbuf, "touch %s;chmod 666 %s", ARTFILE, ARTFILE);
			system(cbuf);
			if (rwaccess(ARTFILE))
				fprintf(mfd, "The problem has been taken care of.\n");
			else
				fprintf(mfd, "Corrective action failed - check suid bits.\n");
			mclose(mfd);
		}
	}
	if (!rwaccess(ACTIVE)) {
		mfd = mailhdr((struct hbuf *)NULL, exists(ACTIVE) ? "Unwritable files!" : "Missing files!");
		if (mfd != NULL) {
			fprintf(mfd, "System: %s\n\nThere was a problem with %s!!\n", FULLSYSNAME, ACTIVE);
			sprintf(cbuf, "touch %s;chmod 666 %s", ACTIVE, ACTIVE);
			system(cbuf);
			if (rwaccess(ACTIVE))
				fprintf(mfd, "The problem has been taken care of.\n");
			else
				fprintf(mfd, "Corrective action failed - check suid bits.\n");
			mclose(mfd);
		}
	}
	sigtrap = FALSE;	/* true if a signal has been caught */
	if (mode != PROC) {
		signal(SIGHUP, onsig);
		signal(SIGINT, onsig);
	}
	savmask = umask(N_UMASK);	/* set up mask */
	uid = getuid();
	gid = getgid();
	duid = geteuid();
	dgid = getegid();
	if (uid == 0 && geteuid() == 0) {
		/*
		 * Must go through with this kludge since
		 * some systems do not honor the setuid bit
		 * when root invokes a setuid program.
		 */
		if ((pw = getpwnam(NEWSUSR)) == NULL)
			xerror("Cannot get NEWSU pw entry");

		duid = pw->pw_uid;
		if ((gp = getgrnam(NEWSGRP)) == NULL)
			xerror("Cannot get NEWSG gr entry");
		dgid = gp->gr_gid;
		setgid(dgid);
		setuid(duid);
	}

#ifndef IHCC
	/*
	 * We force the use of 'getuser()' to prevent forgery of articles
	 * by just changing $LOGNAME
	 */
	if (isatty(fileno(stderr))) {
		if ((user = getenv("USER")) == NULL)
			user = getenv("LOGNAME");
		if ((home = getenv("HOME")) == NULL)
			home = getenv("LOGDIR");
	}
#endif
	if (user == NULL || home == NULL)
		getuser();
	else {
		if (username == NULL || username[0] == 0) {
			username = AllocCpy(user);
		}
		userhome = AllocCpy(home);
	}
	getuser();

	/* loop once per arg. */

	++argv;		/* skip first arg, which is prog name. */

	while (--argc) {
	    if (state == OPTION) {
		if (**argv != '-') {
			xerror("Bad option string \"%s\"", *argv);
		}
		while (*++*argv != '\0') {
			for (optpt = options; optpt->optlet != '\0'; ++optpt) {
				if (optpt->optlet == **argv)
					goto found;
			}
			/* unknown option letter */
usage:
			fprintf(stderr, "usage: inews -t title");
			fprintf(stderr, " [ -n newsgroups ]");
			fprintf(stderr, " [ -e expiration date ]\n");
			fprintf(stderr, "\t[ -f sender]\n\n");
			xxit(1);

		    found:;
			if (optpt->flag == TRUE || (mode != UNKNOWN &&
			    (mode&optpt->oldmode) == 0)) {
				xerror("Bad %c option", **argv);
			}
			if (mode == UNKNOWN)
				mode = optpt->newmode;
			filchar = optpt->filchar;
			optpt->flag = TRUE;
			state = STRING;
			ptr = optpt->buf;
			len = BUFLEN;
		}

		argv++;		/* done with this option arg. */

	    } else {

		/*
		 * Pick up a piece of a string and put it into
		 * the appropriate buffer.
		 */
		if (**argv == '-') {
			state = OPTION;
			argc++;	/* uncount this arg. */
			continue;
		}

		if ((tlen = strlen(*argv)) >= len)
			xerror("Argument string too long");
		strcpy(ptr, *argv++);
		ptr += tlen;
		if (*(ptr-1) != filchar)
			*ptr++ = filchar;
		len -= tlen + 1;
		*ptr = '\0';
	    }
	}

	/*
	 * ALL of the command line has now been processed. (!)
	 */

	tty = isatty(fileno(stdin));

	/* This code is really intended to be replaced by the control message. */
	if (mode == CANCEL) {
		char *p; FILE *f;
		f = xfopen(filename, "r");
		hread(&header, f, TRUE);
		p = index(header.path, ' ');
		if (p != NULL)
			*p = 0;
		p = header.path;
		if (strncmp(username, p, strlen(username))
			&& uid != ROOTID && uid != geteuid() && uid)
			xerror("Not contributor");
		cancel();
		xxit(0);
	}

	if (*header.nbuf)
		lcase(header.nbuf);
	nstrip(header.title);
	nstrip(header.expdate);
	nstrip(header.followid);
	if (mode != PROC) {
		getident(&header);
#ifdef MYORG
		strncpy(header.organization, MYORG, BUFLEN);
		if (strncmp(header.organization, "Frobozz", 7) == 0)
			header.organization[0] = '\0';
		if (ptr = getenv("ORGANIZATION"))
			strncpy(header.organization, ptr, BUFLEN);
		/*
		 * Note that the organization can also be turned off by
		 * setting it to the null string, either in MYORG or
		 * $ORGANIZATION in the environment.
		 */
		if (header.organization[0] == '/') {
			mfd = fopen(header.organization, "r");
			if (mfd) {
				fgets(header.organization, sizeof header.organization, mfd);
				fclose(mfd);
			}
			ptr = index(header.organization, '\n');
			if (ptr)
				*ptr = 0;
		}
#endif
		if (hflag) {
			/* Fill in a few to make frmread return TRUE */
			strcpy(header.subdate, "today");
			strcpy(header.path, "me");
			strcpy(header.oident, "id");
			/* Allow the user to supply some headers. */
			hread(&header, stdin, FALSE);
			/* But there are certain fields we won't let him specify. */
			if (header.from[0])
				strcpy(forgedname, header.from);
			header.from[0] = '\0';
			header.path[0] = '\0';
			header.subdate[0] = '\0';
			header.sender[0] = '\0';
			if (strcmp(header.oident, "id") == 0)
				header.oident[0] = '\0';
		}
		if (forgedname[0]) {
			strcpy(header.path, forgedname);
			if (any(forgedname, "@ (<") == NULL)
				sprintf(header.from,"%s@%s%s",
					forgedname, FULLSYSNAME, MYDOMAIN);
			else
				strncpy(header.from, forgedname, BUFLEN);
			sprintf(header.sender, "%s@%s%s",
				username, FULLSYSNAME, MYDOMAIN);
		} else {
			gensender(&header, username);
		}
		strncpy(header.postversion, genversion(), BUFLEN);
	}

	/* Authorize newsgroups. */
	if (mode == PROC) {
#ifdef BATCH
		checkbatch();
#endif BATCH
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		if (hread(&header, stdin, TRUE) == NULL)
			xerror("Inbound news is garbled");
		input();
		if (history(&header)) {
			log("Duplicate article %s rejected", header.ident);
			xxit(0);
		}
	}

	/* Easy way to make control messages, since all.all.ctl is unblessed */
	if (mode != PROC && prefix(header.title, "cmsg ") && header.ctlmsg[0] == 0)
		strcpy(header.ctlmsg, &header.title[5]);
	is_ctl = mode != CREATENG &&
		(ngmatch(header.nbuf, "all.all.ctl,") || header.ctlmsg[0]);
#ifdef DEBUG
	fprintf(stderr,"is_ctl set to %d\n", is_ctl);
#endif

			/* Must end in comma (NGDELIM) */
#define MODGROUPS	"mod.all,all.mod,all.announce,"
	if (ngmatch(header.nbuf, MODGROUPS) && !header.approved[0]) {
		mfd = mailhdr(&header, "Moderated newsgroup");
		if (mfd) {
			fprintf(mfd, "This newsgroup is moderated, and cannot be posted to directly.\n");
			fprintf(mfd, "Please mail your article to the moderator for posting.\n");
			hwrite(&header, mfd);
			if (infp)
				while ((i = getc(infp)) != EOF)
					putc(i, mfd);
			mclose(mfd);
		}
		xerror("Unapproved moderated newsgroup.");
	}

	if (mode != CREATENG) {
		if (!*header.title)
			xerror("No title");
		if (!*header.nbuf)
			strcpy(header.nbuf, DFLTNG);
	}

	if (mode <= UNPROC)
		ctlcheck();

	if (mode == CREATENG)
		createng();
	/* Determine input. */
	if (mode != PROC)
		input();

#ifndef VMS
	/* Go into the background so the user can get on with his business. */
	if (mode != PROC) {
		i = fork();
		if (i != 0)
			exit(0);
	}
#endif VMS

	/* Do the actual insertion. */
	insert();
}

/*
 *	Create a newsgroup
 */
createng()
{

	/*
	 * Only certain users are allowed to create newsgroups
	 */
	if (uid != ROOTID && uid != geteuid() && uid) {
		fprintf(stderr, "Please contact one of the local netnews people\n\tto create this group for you");
		xxit(1);
	}

	ngdel(header.nbuf);
	sprintf(bfr, "%s/inews -n %s.ctl -t cmsg newgroup %s", LIB,
		header.nbuf, header.nbuf);
	printf("Please type in a paragraph describing the new newsgroup.\n");
	printf("End with control D as usual.\n");
	printf("%s\n", bfr);
	fflush(stdout);
	system(bfr);
	exit(0);
#ifdef lint
	return 0;
#endif lint
}

#include <errno.h>
char firstbufname[BUFLEN];
/*
 *	Link ARTICLE into dir for ngname and update active file.
 */
localize(ngname)
char	*ngname;
{
	char afline[BUFLEN];
	long ngsize;
	long fpos;
	int e;
	char *cp;
	extern int errno;

	lock();
	actfp = xfopen(ACTIVE, "r+");

	for(;;) {
		fpos = ftell(actfp);
		if (fgets(afline, sizeof afline, actfp) == NULL) {
			unlock();
			fclose(actfp);
			return FALSE;		/* No such newsgroup locally */
		}
		if (prefix(afline, ngname)) {
			sscanf(afline, "%s %ld", bfr, &ngsize);
			if (strcmp(bfr, ngname) == 0) {
				break;
			}
			if (ngsize < 0 || ngsize > 99998) {
				logerr("found bad ngsize %d ng %s, setting to 1", ngsize, bfr);
				ngsize = 1;
			}
		}
	}
	for (;;) {
		cp = dirname(ngname);
		if (!exists(cp))
			mknewsg(cp, ngname);

		sprintf(bfr, "%s/%ld", cp, ngsize+1);
#ifdef VMS
		if ((f2 = creat(bfr, 0666)) >=0 ) {
			f1 = open(article, 0);
			if (f1 > 0) {
				while((e=read(f1, afline, BUFLEN)) > 0)
					write(f2, afline, r);
				close(f1);
				close(f2);
				break;
			}
		}
#else !VMS
		if (link(ARTICLE, bfr) == 0)
			break;
#endif !VMS
		e = errno;	/* keep log from clobbering it */
		logerr("Cannot install article as %s", bfr);
		if (e != EEXIST) {
			logerr("Link into %s failed, errno %d, check dir permissions.", bfr, e);
			unlock();
			fclose(actfp);
			return FALSE;
		}
		ngsize++;
	}

	/* Next two lines program around a bug in 4.1BSD stdio. */
	fclose(actfp);
	actfp = fopen(ACTIVE, "r+");

	fseek(actfp, fpos, 0);
	/* Has to be same size as old because of %05d.
	 * This will overflow with 99999 articles.
	 */
	fprintf(actfp, "%s %05ld", ngname, ngsize+1);
	fclose(actfp);
	unlock();
	if (firstbufname[0] == '\0')
		strcpy(firstbufname, bfr);
	sprintf(bfr, "%s/%ld ", ngname, ngsize+1);
	addhist(bfr);
	return TRUE;
}

/*
 *	Localize for each newsgroup and broadcast.
 */
insert()
{
	register char *ptr;
	register FILE *tfp;
	char c;
	struct srec srec;	/* struct for sys file lookup	*/
	int is_invalid = FALSE;

	/* Fill up the rest of header. */
	if (mode != PROC) {
		history(&header);
	}
	dates(&header);
	ptr = ctime(&header.rectime);
	ptr[16] = 0;
	ptr += 4;

	addhist(ptr);
	addhist("\t");
	log("%s %s ng %s subj '%s'", mode==PROC ? "received" : "posted", header.ident, header.nbuf, header.title);
	if (mode==PROC)
		log("from %s relay %s", header.from, header.relayversion);

	/* Clean up Newsgroups: line */
	if (!is_ctl && mode != CREATENG)
		is_invalid = ngfcheck(mode == PROC);

	/* Write article to temp file. */
	tfp = xfopen(mktemp(ARTICLE), "w");
	if ( (c=getc(infp)) == ' ' || c == '\t' ) {
		header.intnumlines++;
		sprintf(header.numlines,"%d",header.intnumlines);
	}
	lhwrite(&header, tfp);
	/* Kludge to get around article truncation problem */
	if (c == ' ' || c == '\t' )
		putc('\n', tfp);
	putc(c,tfp);
	while (fgets(bfr, BUFLEN, infp) != NULL)
		fputs(bfr, tfp);

	if (bfr[strlen(bfr)-1] != '\n')
		putc('\n',tfp);
	fclose(tfp);
	fclose(infp);

	if (is_invalid) {
		logerr("No valid newsgroups found, moved to junk");
		if (localize("junk"))
			savehist(histline);
		xxit(1);
	}

	if (time((time_t)0) > (cgtdate(header.subdate) + DFLTEXP) ){
		logerr("Article too old, moved to junk");
		if (localize("junk"))
			savehist(histline);
		xxit(1);
	}

	if (is_ctl) {
		control(&header);
		localize("control");
	} else {
		if (s_find(&srec, FULLSYSNAME) == FALSE)
			xerror("Cannot find my name '%s' in %s", FULLSYSNAME, SUBFILE);
		for (ptr = nbuf; *ptr;) {
			if (ngmatch(ptr, srec.s_nbuf) || index(ptr,'.') == NULL)
				localize(ptr);
			while (*ptr++)
				;
		}
		if (firstbufname[0] == '\0') {
			logerr("Newsgroups in active, but not sys");
			localize("junk");
		}
	}

	broadcast();
	savehist(histline);
	xxit(0);
}

input()
{
	register int empty = TRUE;
	register char *cp;
	int c;
	long chcount = 0;
	FILE *tmpfp;
	int consec_newlines = 0;
	int linecount = 0;

	tmpfp = xfopen(mktemp(INFILE), "w");
	if (*filename) {
		tty = FALSE;
		infp = xfopen(filename, "r");
	} else {
		infp = stdin;
	}
	while (!sigtrap && fgets(bfr, BUFLEN, stdin) != NULL) {
 		if (mode == PROC) {	/* zap trailing empty lines */
 			if (bfr[0] == '\n') {	/* 1 empty line, to go */
 				consec_newlines++;	/* count it, in case */
 				continue;		/* but don't write it*/
 			}
 			/* foo! a non-empty line. write out all saved lines. */
 			while (consec_newlines > 0) {
 				putc('\n', tmpfp);
 				consec_newlines--;
				linecount++;
 			}
#ifdef ZAPNOTES
			if (empty && bfr[0] == '#' && bfr[2] == ':') {
				strcpy(header.nf_id, bfr);
				nstrip(header.nf_id);
				fgets(bfr, BUFLEN, stdin);
				strcpy(header.nf_from, bfr);
				nstrip(header.nf_from);
				fgets(bfr, BUFLEN, stdin);

				header.intnumlines -= 2;
				sprintf(header.numlines, "%d", header.intnumlines);

				/* Strip trailing " - (nf)" */
				if ((cp = rindex(header.title, '-')) != NULL
				    && !strcmp(--cp, " - (nf)"))
					*cp = '\0';
				log("Stripped notes header on %s", header.ident);
				continue;
			}
#endif ZAPNOTES
 		}
		if (mode != PROC && tty && strcmp(bfr, ".\n") == 0)
			break;
		for (cp = bfr; c = *cp; cp++) {
			if (isprint(c) || isspace(c) || c=='\b')
				putc(c, tmpfp);
			if (isprint(c))
				chcount++;
			if (c == '\n')
				linecount++;
		}
		empty = FALSE;
	}
	if (*filename)
		fclose(infp);

	if (mode != PROC && !is_ctl) {
		sprintf(bfr, "%s/%s", userhome, ".signature");
		if (infp = fopen(bfr, "r")) {
			fprintf(tmpfp, "-- \n");	/* To separate */
			linecount++;
			while ((c = getc(infp)) != EOF) {
				putc(c, tmpfp);
				if (c == '\n')
					linecount++;
			}
			fclose(infp);
		}
	}

	fclose(tmpfp);
	if (sigtrap) {
		if (tty)
			fprintf(stderr, "Interrupt\n");
		if (tty && !empty)
			fwait(fsubr(newssave, (char *) NULL, (char *) NULL));
		if (!tty)
			log("Blown away by an interrupt %d", sigtrap);
		xxit(1);
	}
	if (tty)
		printf("EOT\n");
	fflush(stdout);
	infp = fopen(INFILE, "r");
	if (chcount < 5 && mode <= UNPROC && !is_ctl) {
		fprintf(stderr,"Body of article too short, not posted\n");
		xxit(1);
	}
	if (header.numlines[0]) {
		/* adjust count for blank lines we stripped off */
		if (consec_newlines) {
			header.intnumlines -= consec_newlines;
			if (header.intnumlines < 0 )
				header.intnumlines = 0; /* paranoia */
			sprintf(header.numlines, "%d", header.intnumlines);
		}

		/*
		 * Check line count if there's already one attached to
		 * the article.  Could make this a fatal error -
		 * throwing it away if it got chopped, in hopes that
		 * another copy will come in later with a correct
		 * line count.  But that seems a bit much for now.
		 */
		if (linecount != header.intnumlines) {
			if (linecount)
				log("linecount expected %d, got %d", header.intnumlines, linecount);
			else
				xerror("%s rejected. linecount expected %d, got 0", header.ident, header.intnumlines);
		}
	} else {
		/* Attach a line count to the article. */
		header.intnumlines = linecount;
		sprintf(header.numlines, "%d", linecount);
	}
}

/*
 * Make the directory for a new newsgroup.  ngname should be the
 * full pathname of the directory.  Do the other stuff too.
 * The various games with setuid and chown are to try to make sure
 * the directory is owned by NEWSUSR and NEWSGRP, which is tough to
 * do if you aren't root.  This will work on a UCB system (which allows
 * setuid(geteuid()) or a USG system (which allows you to give away files
 * you own with chown), otherwise you have to change your kernel to allow
 * one of these things or run with your dirs 777 so that it doesn't matter
 * who owns them.
 */
mknewsg(fulldir, ngname)
char	*fulldir;
char	*ngname;
{
	int	pid;
	register char *p;
	char sysbuf[200];
	char parent[200];
	struct stat sbuf;

	if (ngname == NULL || !isalpha(ngname[0]))
		xerror("Tried to make illegal newsgroup %s", ngname);

	/*
	 * If the parent is 755 and we're on a USG system, the setuid(getuid)
	 * will fail, and since mkdir is suid, and our real uid is random,
	 * the mkdir will fail.  So we have to temporarily chmod it to 777.
	 */
	strcpy(parent, fulldir);
	while (p = rindex(parent, '/')) {
		*p = '\0';
		if (stat(parent, &sbuf) == 0) {
			chmod(parent, 0777);
			break;
		}
	}

	if ((pid = fork()) <= 0) {
		int status;

		setgid(getegid());
		if (setuid(geteuid()))	/* This fails on some systems, but
					 * works on 4BSD, and 2BSD. */
#ifndef USG
			umask(0)
#endif
				;
		/* Create the directory */
		mkparents(fulldir);
		sprintf(sysbuf, "mkdir %s", fulldir);
		setgid(getgid());	/* Consistent story to mkdir & chown */
		setuid(getuid());
		status = system(sysbuf);
		/* Give to news if USG or XENIX, no-op if BSD */
		chown(fulldir, duid, dgid);
		exit(status);
	} else if (fwait(pid)) {
		xerror("Cannot mkdir %s", fulldir);
	}

	chmod(parent, (int)sbuf.st_mode);	/* put it back */

#ifdef USG
# ifndef CHEAP
	/*
	 * We now have the directory created and owned by our EFFECTIVE id
	 * on all known permutations of semantics for chown and set-uid.
	 * We are still set-uid.
	 */
	strcpy(sysbuf, fulldir);
	while (p = rindex(sysbuf, '/')) {
		*p = '\0';
		/* stop when we get to last known "good" parent */
		if (strcmp(sysbuf, parent) == 0)
			break;
		chown(sysbuf, duid, dgid);
	}
# endif
#endif

	/* Update the "active newsgroup" file, unless it's already there */
	if (validng(ngname) == 0) {
		actfp = xfopen(ACTIVE, "a");
		fprintf(actfp, "%s 00000 00001 %c\n", ngname,
			prefix(ngname, "fa.") ? 'n' : 'y');
		fclose(actfp);
	}

	log("make newsgroup %s in dir %s", ngname, fulldir);
}

/*
 * If any parent directories of this dir don't exist, create them.
 */
mkparents(dname)
char *dname;
{
	char buf[200], sysbuf[200];
	register char *p;

	strcpy(buf, dname);
	p = rindex(buf, '/');
	if (p)
		*p = '\0';
	if (exists(buf))
		return;
	mkparents(buf);
	sprintf(sysbuf, "mkdir %s", buf);
	system(sysbuf);
}

cancel()
{
	register FILE *fp;

	log("cancel article %s", filename);
	fp = xfopen(filename, "r");
	if (hread(&header, fp, TRUE) == NULL)
		xerror("Article is garbled.");
	fclose(fp);
	unlink(filename);
}


/*
 * Return the ptr in sp at which a character in sq appears;
 * NULL if not found
 *
 */

char *
any(sp, sq)
char *sp, *sq;
{
	register c1, c2;
	register char *q;

	while (c1 = *sp++) {
		q = sq;
		while (c2 = *q++)
			if (c1 == c2)
				return(--sp);
	}
	return(NULL);
}
