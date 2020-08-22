/*
 * readr - /bin/mail and msgs interface and associated functions.
 */

#ifndef lint
static char	*SccsId = "@(#)readr.c	2.46	9/5/84";
#endif !lint

#include "rparams.h"
#if defined(BSD4_2) || defined(BSD4_1C)
#include <sys/dir.h>
#else
#include "ndir.h"
#endif !BSD4_2 && !BSD4_1C
#include <setjmp.h>
#include <errno.h>

extern int errno;

char *Progname = "readnews";	/* used by xerror to identify failing program */

static char	lbuf[BUFLEN*2];
long atol();

#define	saveart	oobit = bit;strcpy(ofilename1, filename);strcpy(ogroupdir, groupdir);hptr = h;h = hold;hold = hptr;ongsize = pngsize
#define NLINES(h, fp) (h->numlines[0] ? h->intnumlines : (h->intnumlines=linecnt(fp),sprintf(h->numlines, "%d", h->intnumlines), h->intnumlines))

char *tft = "/tmp/folXXXXXX";

/*
 * These were made static for u370 with its buggy cc.
 * I judged it better to have one copy with no ifdefs than
 * to conditionally compile them as automatic variables
 * in readr (which they originally were).  Performance
 * considerations might warrent moving some of the simple
 * things into register variables, but I don't know what
 * breaks the u370 cc.
 */
static char goodone[BUFLEN];		/* last decent article		*/
static char ogroupdir[BUFLEN];		/* last groupdir		*/
static char address[PATHLEN];		/* for reply copy		*/
static char edcmdbuf[128];
static int rfq = 0;			/* for last article		*/
static long ongsize;			/* Previous ngsize		*/
static long pngsize;			/* Printing ngsize		*/
static char *bptr;			/* temp pointer.		*/
static struct srec srec;		/* srec for sys file entries	*/
static char *tfilename;			/* temporary file name 		*/
static char ofilename1[BUFLEN];		/* previous file name		*/
static struct hbuf hbuf1, hbuf2,	/* for minusing			*/
		*h = &hbuf1,		/* current header		*/
		*hold = &hbuf2,		/* previous header		*/
		*hptr;			/* temporary 			*/
static char *ptr1, *ptr2, *ptr3;	/* for reply manipulation	*/
static int  news = 0;
static int  abs = FALSE;		/* TRUE if we asked absolutely	*/
static char tf[100];
static long oobit;			/* last bit, really		*/
static int dgest = 0;
static FILE *ofp;			/* Current output file to terminal*/
static FILE *fp;			/* current article to be printed*/
static int holdup;			/* 1 iff should stop before hdr */
static int ignorenews;			/* 1 iff readnews -p > /dev/null*/
static time_t timelastsaved;		/* time newsrc last written out */
static jmp_buf sigjmpbuf;		/* for signal processing */
static int canlongjmp;			/* TRUE if setjmp on sigjmp valid */

int catchcont();

readr()
{

#ifdef DEBUG
	fprintf(stderr, "readr()\n");
#endif
	if (aflag) {
		if (*datebuf) {
			if ((atime = cgtdate(datebuf)) == -1)
				xerror("Cannot parse date string");
		} else
			atime = 0;
	}

	if (pflag && ignoring())
		ignorenews = TRUE;

	if (xflag)
		uflag = 0;
	if (uflag)
		(void) time(&timelastsaved);

	ofp = stdout;
	if (cflag && coptbuf[0] != '\0') {
		umask(022);
		mktemp(outfile);	/* get "unique" file name */
		close(creat(outfile,0666));
		ofp = xfopen(outfile, "w");
		umask(N_UMASK);
		cflag = FALSE;
		pflag = TRUE;
	}

	/* loop reading articles. */
	fp = NULL;
	obit = -1;
	nextng();
	for ( ;; ) {
		if (getnextart(FALSE))
			break;
#ifdef DEBUG
		fprintf(stderr,"after getnextart, fp %x, pos %ld, bit %ld, group '%s', filename '%s'\n",
			fp, ftell(fp), bit, groupdir, filename);
#endif
		strcpy(goodone, filename);
		if (pflag || lflag || eflag) {
			/* This code should be gotten rid of */
			if (sigtrap) {
				qfflush(ofp);
				fprintf(ofp, "\n");
				cdump(ofp);
				xxit(0); /* kludge! drop when qfflush works */
				return;
			}
			clear(bit);
			nextbit();
			FCLOSE(fp);
			continue;
		}
		for ( ;; ) {
			char *pp;
			int nlines;
			int (*ointr)();
#ifdef	SIGCONT
			int (*ocont)();
#endif
			setjmp(sigjmpbuf);
			canlongjmp = TRUE;

			sigtrap = FALSE;
			if (!cflag) {
				if (rfq)
					sprintf(bfr, "Last article.  [qfr] ");
				else {
					nlines = NLINES(h, fp);
					if (nlines <= 0) {
						sprintf(bfr, "(0 lines) Next? [nqfr] ");
						FCLOSE(fp);
					} else {
						sprintf(bfr, "(%d lines) More? [ynq] ", nlines);
					}
				}
			} else
				sprintf(bfr, "? ");
			fprintf(ofp, "%s", bfr);
			fflush(ofp);
			bptr = lbuf;
			ointr = signal(SIGINT, catchcont);
#ifdef SIGCONT
			ocont = signal(SIGCONT, catchcont);
#endif
			pp = fgets(bptr, BUFLEN, stdin);
			canlongjmp = FALSE;
			signal(SIGINT, ointr);
#ifdef SIGCONT
			signal(SIGCONT, ocont);
#endif
			if (pp != NULL)
				break;
			if (!sigtrap)
				return;
#ifdef SIGCONT
			if (sigtrap != SIGCONT)
#endif
				fprintf(ofp, "\n");
		}
		nstrip(bptr);
		while (*bptr == ' ' || *bptr == '\t')
			bptr++;
		if (command())
			break;
	}

	if (!news)
		fprintf(stderr, "No news.\n");
	cout(ofp);
}


#define EOL() if (*bptr != '\0') { fprintf(ofp, "? for commands.\n"); return FALSE; }
/*
 * Process one command, which has already been typed in.
 */
command()
{
	char *findhist();
	long i;

	switch (*bptr++) {

	/* No.  Go on to next article. */
	case 'n':
		EOL();
		readmode = NEXT;
		if (!cflag)
			FCLOSE(fp);
		fprintf(ofp, "\n");
		clear(bit);
		saveart;
		nextbit();
		break;

	/* Undigestify the article. */
	case 'd':
		dgest = 1;
		/* fall through */

	/* yes: print this article, go on. */
	case 'y':
		EOL();
		/* fall through. */

	/* The user hit return.  Default is 'y' unless rfq, then it's 'q'. */
	case '\0':
		if (!bptr[-1] && rfq)
			return TRUE;
		readmode = NEXT;
		showtail(fp);
		clear(bit);
		saveart;
		nextbit();
		break;

	/*
	 * Unsubscribe to the newsgroup and go on to next group
	 */
	case 'u':
		fprintf(ofp, "To unsubscribe, use 'U'\n");
		break;

	case 'U':
		fprintf(ofp, "Unsubscribing to newsgroup: %s\n", groupdir);
		obit = -1;
		FCLOSE(fp);
		if (cflag)
			clear(bit);
		else
			putc('\n', ofp);
		rfq = 0;
		zapng = TRUE;
		saveart;
		if (nextng()) {
			if (actdirect == BACKWARD)
				fprintf(ofp, "Can't back up.\n");
			else
				return TRUE;
		}
		break;

		/* Print the current version of news */
	case 'v':
		fprintf(ofp, "News version: %s\n", news_version);
		break;

		/* reprint the article */
	case 'p':
		EOL();
		if (!cflag)
			goto minus;
		readmode = NEXT;
		if (!cflag) {
			FCLOSE(fp);
			bit = last;
			putc('\n', ofp);
		}
		obit = -1;
		break;

		/* decrypt joke */
	case 'D':
		caesar_command();
		readmode = NEXT;
		clear(bit);
		saveart;
		nextbit();
		break;

		/* write out the article someplace */
	case 's':
	case 'w':
		{
		char *grn = groupdir;
		tfilename = filename;
		if (*bptr == '-') {
			bptr++;
			grn = ogroupdir;
			if (*ofilename1)
				tfilename = ofilename1;
		}
		if (*bptr != '\0' && *bptr != ' ') {
			fprintf(ofp, "Bad file name.\n");
			break;
		}
		while (*bptr == ' ')
			bptr++;
		if (*bptr != '|' && *bptr != '/') {
			char	hetyped[BUFLEN];
			char	*boxptr;
			strcpy(hetyped, bptr);
			if (boxptr = getenv("NEWSBOX"))
				if (index(boxptr, '%'))
					sprintf(bptr, boxptr, grn);
				else
					strcpy(bptr, boxptr);
			else if (hetyped[0] == '~' && hetyped[1] == '/') {
				strcpy(hetyped, bptr+2);
				strcpy(bptr, userhome);
			} else
				strcpy(bptr, ".");
			strcat(bptr, "/");
			if (hetyped[0] != '\0')
				strcat(bptr, hetyped);
			else
				strcat(bptr, "Articles");
		}
		fwait(fsubr(save, tfilename, bptr));
		}
		break;

		/* back up  */
	case '-':
minus:
		rfq = 0;
		abs = TRUE;
		if (!*ofilename1) {
			fprintf(ofp, "Can't back up.\n");
			break;
		}
		if (cflag)
			clear(bit);
		else {
			FCLOSE(fp);
			putc('\n', ofp);
		}
		hptr = h;
		h = hold;
		hold = hptr;
		strcpy(bfr, filename);
		strcpy(filename, ofilename1);
		strcpy(ofilename1, bfr);
		obit = bit;
		if (strcmp(groupdir, ogroupdir)) {
			strcpy(bfr, groupdir);
			selectng(ogroupdir, TRUE);
			strcpy(groupdir, ogroupdir);
			strcpy(ogroupdir, bfr);
			ngrp = 1;
			back();
		}
		bit = oobit;
		oobit = obit;
		obit = -1;
		(void) getnextart(TRUE);
		return FALSE;

		/* skip forwards */
	case '+':
caseplus:
		if (*bptr == '\0')
			strcat(bptr, "1");
		rfq = 0;
		if (cflag)
			clear(bit);
		saveart;
		last = bit;
		for (i = 0; i < atol(bptr); i++) {
			nextbit();
			if ((bit > pngsize) || (rflag && bit < 1))
				break;
		}
		if (!cflag) {
			putc('\n', ofp);
			FCLOSE(fp);
		}
		obit = -1;
		break;

	/* exit - time updated to that of most recently read article */
	case 'q':
		EOL();
		return TRUE;

	/* exit - no time update. */
	case 'x':
		EOL();
		xxit(0);

	/* cancel the article. */
	case 'c':
		(void) cancel_command();
		break;

	/* escape to shell */
	case '!':
		fwait(fsubr(ushell, bptr, (char *)NULL));
		fprintf(ofp, "\n");
		hdr();
		break;

	/* mail reply */
	case 'r':
		(void) reply_command();
		break;

	/* send to some system */
	case 'X':
		xmit_command();
		break;
	/* mark the rest of the articles in this group as read */
	case 'K':
		saveart;
		while (bit <= pngsize && bit >= minartno) {
			clear(bit);
			nextbit();
		}
		FCLOSE(fp);
		break;

	/* next newsgroup */
	case 'P':
		*bptr = '-';
	case 'N':
		FCLOSE(fp);
		if (next_ng_command())
			return TRUE;
		break;

	case 'b':	/* back up 1 article */
		i = bit - 1;
		goto tryartnum;
	case '0':	/* specific no. */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		sscanf(--bptr, "%ld", &i);
		if (i == 0) {
			fprintf(ofp, "Bad article no.\n");
			break;
		}
		if (i > pngsize) {
			fprintf(ofp, "Not that many articles.\n");
			break;
		}
tryartnum:
		readmode = SPEC;
		abs = TRUE;
		bit = i;
		obit = -1;
		if (!cflag) {
			putc('\n', ofp);
			FCLOSE(fp);
		}
		rfq = 0;
		break;

	/* specific message ID. */
	case '<':
		ptr1 = findhist(--bptr);
		if (ptr1 == NULL) {
			fprintf(ofp, "No such article: %s.\n", bptr);
			break;
		}
		ptr2 = index(ptr1, '\t');
		ptr3 = index(++ptr2, '\t');
		ptr2 = index(++ptr3, ' ');
		if (ptr2)
			*ptr2 = '\0';
		ptr2 = index(ptr3, '/');
		*ptr2++ = '\0';
		abs = TRUE;
		if (cflag)
			clear(bit);
		else {
			FCLOSE(fp);
			putc('\n', ofp);
		}
		saveart;
		strcpy(ogroupdir, ptr3);
		if (strcmp(groupdir, ogroupdir)) {
			strcpy(bfr, groupdir);
			selectng(ogroupdir, TRUE);
			strcpy(groupdir, ogroupdir);
			strcpy(ogroupdir, bfr);
			back();
		}
		sscanf(ptr2, "%ld", &bit);
		oobit = obit;
		obit = -1;
		(void) getnextart(TRUE);
		rfq = 0;
		break;

	/* follow-up article */
	case 'f':
		if (*bptr == '-')
			tfilename = ofilename1;
		else
			tfilename = filename;
		sprintf(bfr,"%s/%s %s", BIN, "postnews", tfilename);
		system(bfr);
		break;

	/* erase - pretend we haven't seen this article. */
	case 'e':
		if (rfq || *bptr == '-') {
			if (strcmp(groupdir, ogroupdir)) {
				i = bit;
				strcpy(bfr, groupdir);
				selectng(ogroupdir, FALSE);
				set(oobit);
				fprintf(ofp,"Holding article %ld newsgroup %s\n", oobit, ogroupdir),
				strcpy(groupdir, ogroupdir);
				selectng(bfr, FALSE);
				bit = i;
			} else {
				fprintf(ofp,"Holding article %ld\n", oobit);
				set(oobit);
			}
		} else {
			fprintf(ofp,"Holding article %ld\n", bit);
			set(bit);
			goto caseplus;	/* skip this article for now */
		}
		break;

	case 'H':
	case 'h':
		if (!hflag)
			dash(8, ofp);
		if (*bptr == '-') {
			if (oobit > 0)
				fprintf(ofp, "Article %ld:\n", oobit);
			hprint(hold, ofp, 1 + (bptr[-1]=='H'));
		} else {
			fprintf(ofp, "Article %ld of %ld: %s\n",
				rfq ? oobit : bit, pngsize, h->ident);
			hprint(h, ofp, 1 + (bptr[-1]=='H'));
		}
		if (!hflag)
			dash(8, ofp);
		break;

	case '#':
		fprintf(ofp, "Article %ld of %ld: newsgroup %s\n",
			rfq ? oobit : bit, pngsize, rfq ? ogroupdir : groupdir);
		break;

		/* error */
	case '?':
		help(ofp);
		break;
	default:
		fprintf(ofp, "? for commands.\n");
		break;
	}

	return FALSE;
}

cancel_command()
{
	tfilename = filename;
	hptr = h;
	if (*bptr == '-') {
		if (*ofilename1) {
			tfilename = ofilename1;
			hptr = hold;
		}
		bptr++;
	}
	EOL();
	readmode = SPEC;
	strcpy(rcbuf, hptr->path);
	ptr1 = index(rcbuf, ' ');
	if (ptr1)
		*ptr1 = 0;
	if (uid != ROOTID && strcmp(username, rcbuf)) {
		fprintf(ofp, "Can't cancel what you didn't write.\n");
		return FALSE;
	}
	if (!cancel(ofp, hptr, 0) && hptr == h) {
		clear(bit);
		saveart;
		nextbit();
		obit = -1;
		if (!cflag)
			putc('\n', ofp);
		FCLOSE(fp);
	}
	return TRUE;
}

reply_command()
{
	register char	*pathptr;
	int edit = 1;
	char *ed;
	FILE *tfp;
	char	curberk[BUFLEN];
	char *replyname();
	char subj[BUFLEN];
	char folbuf[BUFLEN];
	struct stat statb;
	long creatm;

	hptr = h;
	while (*bptr && index("d-", *bptr)) {
		switch (*bptr) {
		/* Followup the previous article. */
		case '-':
			hptr = hold;
			break;

		/* Don't edit the headers */
		case 'd':
			edit = 0;
			break;
		}
		bptr++;
	}
	EOL();
	ptr1 = index(MAILPARSER, ' ');
	if (ptr1)
		*ptr1 = '\0';
	if (edit && access(MAILPARSER, 1)) {
#ifdef IHCC
		fprintf(stderr, "Can't edit headers, 'recmail' missing.\n");
#else
		fprintf(stderr, "Can't edit headers without %s\n", MAILPARSER);
#endif
		edit = 0;
	}
	if (ptr1)
		*ptr1 = ' ';

	*rcbuf = '\0';
	*curberk = '\0';
	pathptr = replyname(hptr);;
	for (ptr1 = address, ptr2 = pathptr; *ptr2; ptr1++, ptr2++) {
		if (index("\"\\$", *ptr2))
			*ptr1++ = '\\';
		*ptr1 = *ptr2;
	}
	*ptr1 = '\0';

	folbuf[0] = '\0';		/* References */
	if (hptr->followid[0]) {
		strcpy(folbuf, hptr->followid);
		strcat(folbuf, ", ");
	}
	strcat(folbuf, hptr->ident);

	strcpy(subj, hptr->title);	/* Subject */
	while (isspace(*bptr))
		bptr++;
	if (*bptr != '\0')
		strcpy(subj, bptr);
	if (!prefix(subj, "Re:")){
		strcpy(bfr, subj);
		sprintf(subj, "Re: %s", bfr);
	}
	if (!edit) {
		fprintf(ofp, "To: %s\n", pathptr);
		ed = index(MAILER, '%');
		if (ed && ed[1] == 's')
			fprintf(ofp, "Subject: %s\n", subj);
		fflush(ofp);
	}

	/* Put the user in the editor to create the body of the followup. */
	if (edit) {
		strcpy(tf, tft);
		mktemp(tf);

		ed = getenv("EDITOR");
		if (ed == NULL)
			ed = DFTEDITOR;

		if ((tfp = fopen(tf, "w")) == NULL) {
			perror(tf);
			creatm = 0L;
		} else {
			fprintf(tfp, "To: %s\n", pathptr);
			fprintf(tfp, "Subject: %s\n", subj);
			fprintf(tfp, "References: %s\n\n", folbuf);
			fstat(fileno(tfp), &statb);
			creatm = statb.st_mtime;
			fclose(tfp);
		}

		sprintf(edcmdbuf, "%s %s", ed, tf);
		system(edcmdbuf);
		strcpy(rcbuf, MAILPARSER);
		strcat(rcbuf, " -t");
		strcat(rcbuf, " < ");
		strcat(rcbuf, tf);
		if (access(tf, 4) || stat(tf, &statb)) {
			fprintf(stderr, "Reply not sent: no input file.\n");
			return FALSE;
		}
		if (statb.st_mtime == creatm) {
			fprintf(stderr, "Reply not sent: cancelled.\n");
			unlink(tf);
			return FALSE;
		}
		fprintf(ofp,"Sending reply.\n");
		fflush(stdout);
		if (fork() == 0) {
			system(rcbuf);
			unlink(tf);
			_exit(0);
		}
	} else {
		sprintf(rcbuf, MAILER, hptr->title);
		sprintf(bfr, "%s %s", rcbuf, address);
		system(bfr);
	}
	hdr();
	return TRUE;
}

xmit_command()
{
	tfilename = filename;
	if (*bptr == '-') {
		if (*ofilename1)
			tfilename = ofilename1;
		bptr++;
	}
	if (*bptr != '\0' && *bptr != ' ') {
		fprintf(ofp, "Bad system name.\n");
		return;
	}
	while (*bptr == ' ')
		bptr++;
	if (*bptr == '\0') {
		fprintf(ofp, "Missing system name.\n");
		return;
	}
	if (s_find(&srec, bptr) == NULL) {
		fprintf(ofp, "%s not in SYSFILE\n", bptr);
		return;
	}
	transmit(&srec, tfilename);
}

next_ng_command()
{
	obit = -1;
	if (!*bptr || *bptr == '-') {
		if (cflag)
			clear(bit);
		else
			putc('\n', ofp);
		if (*bptr)
			actdirect = BACKWARD;
		rfq = 0;
		saveart;
		if (nextng()) {
			if (actdirect == BACKWARD)
				fprintf(ofp, "Can't back up.\n");
			else
				return TRUE;
		}
		return FALSE;
	}
	while (isspace(*bptr))
		bptr++;
	if (!validng(bptr)) {
		fprintf(ofp, "No such group.\n");
		return FALSE;
	}
	if (cflag)
		clear(bit);
	else
		putc('\n', ofp);
	readmode = SPEC;
	rfq = 0;
	saveart;
	back();
	selectng(bptr, TRUE);
	return FALSE;
}

caesar_command()
{
	char	temp[BUFLEN];
	FILE	*pfp, *popen();

	fprintf(stderr, "Caesar decoding:\n");
	sprintf(temp, "%s/%s", LIB, "caesar");
	if (*bptr) {
		strcat(temp, " ");
		strcat(temp, bptr);
	}
	if (NLINES(h, fp) > LNCNT && *PAGER) {
		strcat(temp, " | ");
		strcat(temp, PAGER);
	}
	pfp = popen(temp, "w");
	tprint(fp, pfp, FALSE);
	FCLOSE(fp);
	pclose(pfp);
}

/*
 * Show the user the tail, if any, of the message on file
 * descriptor fd, and close fd.  The digester is considered,
 * and the pager is used if appropriate.
 */
showtail(fd)
FILE *fd;
{
	if (fd == NULL)
		return;

	if (dgest) {
		digest(fd, ofp, h);
	} else if (!lflag && !pflag && !eflag) {
		pprint(fd);
	}
	fclose(fd);
}

/*
 * Print out the rest of the article through the pager.
 */
pprint(fd)
FILE *fd;
{
#ifdef PAGE
	/* Filter the tail of long messages through PAGER. */
	if (NLINES(h, fd) > LNCNT && *PAGER) {
		if (!index(PAGER, FMETA)) {
			FILE *pfp, *popen();

			pfp = popen(PAGER, "w");
			if (pfp == NULL)
				pfp = ofp;
			/*
			 * What follows is an attempt to prevent the
			 * next message from scrolling part of this
			 * message off the top of the screen before
			 * the poor luser can read it.
			 */
			tprint(fd, pfp, FALSE);
			pclose(pfp);
		}
		else
			pout(ofp);
		holdup = TRUE;
	}
	else
#endif
		tprint(fd, ofp, FALSE);
}

/*
 * Find the next article we want to consider, if we're done with
 * the last one, and show the header.
 */
getnextart(minus)
int minus;
{
 	int noaccess;
 	register DIR *dirp;
 	register struct direct *dir;
 	long nextnum, tnum;

 	noaccess = 0;

	if (minus)
		goto nextart2;	/* Kludge for "-" command. */

	if (bit == obit)	/* Return if still on same article as last time */
		return 0;

	sigtrap = FALSE;

nextart:
#ifdef DEBUG
	fprintf(stderr,"nextart:\n");
#endif DEBUG
	dgest = 0;

	if (bit < minartno && !rflag)
		bit = minartno;

	/* If done with this newsgroup, find the next one. */
	while (ngsize <= 0 || ((long) bit > ngsize) || (rflag && bit < minartno)) {
		if (nextng()) {
			if (actdirect == BACKWARD) {
				fprintf(ofp, "Can't back up.\n");
				actdirect = FORWARD;
				continue;
			} else
				if (rfq++ || pflag || cflag)
					return 1;
		}
		if (rflag)
			bit = ngsize + 1;
		else
			bit = minartno - 1;
		if (uflag) {
			time_t now;
			(void) time(&now);
			if (now - timelastsaved > 5*60 /* 5 minutes */) {
				fprintf(stderr,"[Saving .newsrc]\n");
				writeoutrc();
				timelastsaved = now;
			}
		}
	}

nextart2:
#ifdef DEBUG
	fprintf(stderr, "article: %s/%ld\n", groupdir, bit);
#endif
	if (rcreadok)
		rcreadok = 2;	/* have seen >= 1 article */
	sprintf(filename, "%s/%ld", dirname(groupdir), bit);
	if (rfq && goodone[0])
		strcpy(filename, goodone);
	if (sigtrap) {
		if (sigtrap == SIGHUP)
			return 1;
		if (!rcreadok)
			xxit(0);
		fprintf(ofp, "Abort (n)?  ");
		fflush(ofp);
		gets(bfr);
		if (*bfr == 'y' || *bfr == 'Y')
			xxit(0);
		sigtrap = FALSE;
	}
#ifdef DEBUG
	fprintf(stderr, "filename = '%s'\n", filename);
#endif
	/* Decide if we want to show this article. */
 	if ((fp = fopen(filename, "r")) == NULL) {
 		/* since there can be holes in legal article numbers, */
 		/* we wait till we hit 5 consecutive bad articles */
 		/* before we haul off and scan the directory */
 		if (++noaccess < 5)
 			goto badart;
		noaccess = 0;
 		dirp = opendir(dirname(groupdir));
 		if (dirp == NULL) {
			if (errno != EACCES)
				fprintf(stderr,"Can't open %s", dirname(groupdir));
 			goto badart;
 		}
 		nextnum = rflag ? minartno - 1 : ngsize + 1;
 		while ((dir = readdir(dirp)) != NULL) {
 			tnum = atol(dir->d_name);
 			if (tnum <= 0)
 				continue;
 			if (rflag ? (tnum > nextnum && tnum < bit)
 				  : (tnum < nextnum && tnum > bit))
 				nextnum = tnum;
 		}
 		closedir(dirp);
 		if (rflag ? (nextnum >= bit) : (nextnum <= bit))
 			goto badart;
#ifdef DEBUG
		fprintf(stderr,"nextnum = %ld\n",nextnum);
#endif DEBUG
 		do {
 			clear(bit);
 			nextbit();
 		} while (rflag ? (nextnum < bit) : (nextnum > bit));
 		obit = -1;
 		abs = FALSE;
 		goto nextart;
 	} else
 		noaccess = 0;

 	if (ignorenews || hread(h, fp, TRUE) == NULL
		|| (!rfq && !aselect(h, abs))) {
 badart:
#ifdef DEBUG
		fprintf(stderr, "Bad article '%s'\n", filename);
#endif
		FCLOSE(fp);
		clear(bit);
		obit = -1;
		nextbit();
		abs = FALSE;
		goto nextart;
	}
	abs = FALSE;
	actdirect = FORWARD;
	news = TRUE;
	hdr();
	if (pflag)
		tprint(fp, ofp, FALSE);
	else if (cflag && !lflag && !eflag) {
		fflush(ofp);
		pprint(fp);
	}
	if (cflag || lflag || eflag || pflag) {
		sigtrap = FALSE;
		FCLOSE(fp);
	}
	obit = bit;
	return 0;
}

/*
 * Print out whatever the appropriate header is
 */
hdr()
{
	char *briefdate();

	if (rfq)
		return;

	if (lflag || eflag) {
		hprint(h, ofp, 0);
		return;
	}

	/* Print out a header */
	if (ngrp) {
		pngsize = ngsize;
		ngrp--;
		nghprint(groupdir);
	}
	if (!hflag)
		fprintf(ofp, "Article %ld of %ld, %s.\n",
			bit, pngsize, briefdate(h->subdate));
	hprint(h, ofp, pflag ? 1 : 0);
}

nghprint(title)
char *title;
{
	char *tstr = "Newsgroup ";
	int l = strlen(title) + strlen(tstr);

	fprintf(ofp, "\n");
	if (!hflag) {
		dash(l, ofp);
		fprintf(ofp, "%s%s\n", tstr, title);
		dash(l, ofp);
	} else {
		fprintf(ofp, "%s%s, ", tstr, title);
		if (bit == pngsize)
			fprintf(ofp, "%ld\n", pngsize);
		else
			fprintf(ofp, "%ld-%ld\n", bit, pngsize);
	}
	fprintf(ofp, "\n");
}

/*
 * Routine to catch a continue signal.
 */
catchcont(sig)
int sig;
{
	signal(sig, catchcont);
	sigtrap = sig;
	fflush(ofp);
#ifdef SIGCONT
	if (fp && sig == SIGCONT)
		hdr();
	if (sig != SIGCONT)
#endif SIGCONT
		putc('\n', ofp);
	if (canlongjmp)
		longjmp(sigjmpbuf,1);
}
