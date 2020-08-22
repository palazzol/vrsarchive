/*
 * readr - visual news interface.
 */

#ifndef lint
static char	*SccsId = "@(#)visual.c	1.12	9/5/84";
static char Author[] = "@(#)visual interface written by Kenneth Almquist";
#endif !lint

#include "rparams.h"
#define GGRMAIL
#ifdef USG
#include <termio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#else
#include <sgtty.h>
#endif USG

#include <errno.h>
#if defined(BSD4_2) || defined(BSD4_1C)
#include <sys/dir.h>
#else
#include "ndir.h"
#endif
#ifdef BSD4_2
#define BIT(_a) (1<<((_a)-1))
#endif
#ifdef MYDB
#include "db.h"
#endif MYDB

#include <errno.h>

extern int errno;

#ifdef SIGTSTP
#include <setjmp.h>
#endif

#define ARTWLEN	(ROWS-2)/* number of lines used to display article */
#define even(cols) ((cols&1) ? cols + 1 : cols)
#ifdef STATTOP
#define PRLINE	0	/* prompter line */
#define SPLINE	1	/* secondary prompt line */
#define ARTWIN	2	/* first line of article window */
#define SECPRLEN 81	/* length of secondary prompter */
#else
#define PRLINE	(ROWS-1)/* prompter line */
#define SPLINE	(ROWS-2)/* secondary prompt line */
#define ARTWIN	0	/* first line of article window */
#define SECPRLEN 100	/* length of secondary prompter */
#endif

#define PIPECHAR '|'	/* indicate save command should pipe to program */
#define META	0200	/* meta character bit (as in emacs) */
/* print (display) flags */
#define HDRONLY	0001	/* print header only */
#define NOPRT	0002	/* don't print at all */
#define NEWART	0004	/* force article display to be regenerated */
#define HELPMSG	0010	/* display currently contains help message */
/* prun flags */
#define CWAIT	0001	/* type "continue?" and wait for return */
#define BKGRND	0002	/* run process in the background */
/* values of curflag */
#define CURP1	1	/* cursor after prompt */
#define CURP2	2	/* cursor after secondary prompt */
#define CURHOME	3	/* cursor at home position */
/* flags for vsave routine */
#define SVHEAD	01	/* write out article header */
#define OVWRITE	02	/* overwrite the file if it already exists */
/* other files */

#define	saveart	oobit = bit;strcpy(ofilename1, filename);strcpy(ogroupdir, groupdir);hptr = h;h = hold;hold = hptr;ongsize = pngsize
#define NLINES(h, fp) (h->numlines[0] ? h->intnumlines : (h->intnumlines=linecnt(fp),sprintf(h->numlines, "%d", h->intnumlines), h->intnumlines))

/* terminal handler stuff */
extern int _junked;
#define clearok(xxx, flag) _junked = flag
extern int COLS;
extern int ROWS;
extern int hasscroll;

FILE *tmpfile();
char *getmailname();
char *findparent();
int onint(), onquit();
int onstop();

/*
 * Kludge: space so that routines can access beyond
 * the end of arrays without messing me up.
 */
static char junk[64];

char *Progname = "vnews";		/* for xerror */

/* variables shared between vnews routines */
static char linebuf[LBUFLEN];		/* temporary workspace */
static FILE *tfp;			/* temporary file */
static char tfname[] = "/tmp/vnXXXXXX";	/* name of temp file */
static long artbody;			/* offset of body into article */
static int quitflg;			/* if set, then quit */
static int erased;			/* current article has been erased */
static int artlines;			/* # lines in article body */
static int artread;			/* entire article has been read */
static int hdrstart;			/* beginning of header */
static int hdrend;			/* end of header */
static int lastlin;			/* number of lines in tempfile */
static int tflinno = 0;			/* next line in tempfile */
static int maxlinno;			/* number of lines in file + folded */
static char secpr[SECPRLEN];		/* secondary prompt */
static char prompt[30];			/* prompter */
static short prflags;			/* print flags (controls updscr) */
static short curflag;			/* where to locate cursor */
static int dlinno;			/* top line on screen */
static char timestr[20];		/* current time */
static int ismail;			/* true if user has mail */
static char *mailf;			/* user's mail file */
static int alflag;			/* set if unprocessed alarm signal */
static int atend;			/* set if at end of article */
static char cerase;			/* erase character */
static char ckill;			/* kill character */
static char cintr;			/* interrupt character */
int ospeed;				/* terminal speed */
static int intflag;			/* set if interrupt received */

#ifdef SIGTSTP
static int reading;			/* to keep stupid BSD from restarting reads */
jmp_buf intjmp, alrmjmp;
#endif SIGTSTP

#ifdef MYDB
static int hasdb;			/* true if article data base exists */
#endif

#ifdef DIGPAGE
static int endsuba;			/* end of sub-article in digest */
#endif

#ifdef MYDEBUG
FILE *debugf;				/* file to write debugging info on */
#endif

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
static char edcmdbuf[128];
static int rfq = 0;			/* for last article		*/
static long ongsize;			/* Previous ngsize		*/
static long pngsize;			/* Printing ngsize		*/
static char *bptr;			/* temp pointer.		*/
static char *tfilename;			/* temporary file name 		*/
static char ofilename1[BUFLEN];		/* previous file name		*/
static struct hbuf hbuf1, hbuf2; 	/* for minusing			*/
static struct hbuf *h = &hbuf1,		/* current header		*/
		*hold = &hbuf2,		/* previous header		*/
		*hptr;			/* temporary			*/
static char *ptr1, *ptr2, *ptr3;	/* for reply manipulation	*/
static int  news = 0;
static int  aabs = FALSE;		/* TRUE if we asked absolutely	*/
static char *ed, tf[100];
static long oobit;			/* last bit, really		*/
static int dgest = 0;
static FILE *fp;			/* current article to be printed*/

readr()
{

#ifdef MYDEBUG
	debugf = fopen("DEBUG", "w");
	setbuf(debugf, (char *)NULL);
#endif
	if (aflag) {
		if (*datebuf) {
			if ((atime = cgtdate(datebuf)) == -1)
				xerror("Cannot parse date string");
		} else
			atime = 0;
	}

	if (sigtrap)
		xxit(1);
	mktemp(tfname);
	close(creat(tfname,0666));
	if ((tfp = fopen(tfname, "w+")) == NULL)
		xerror("Can't create temp file");
	unlink(tfname);
	mailf = getmailname();
#ifdef MYDB
	if (opendb() >= 0) {
		hasdb = 1;
		fputs("Using article data base\n", stderr);	/*DEBUG*/
		getng();
	}
#endif
	ttysave();
	signal(SIGINT, onint);
	signal(SIGQUIT, onquit);
	if (sigtrap)
		xxit(1);
	ttyraw();
	timer();

	/* loop reading articles. */
	fp = NULL;
	obit = -1;
	nextng();
	quitflg = 0;
	while (quitflg == 0) {
		if (getnextart(FALSE))
			break;
		strcpy(goodone, filename);
		if (sigtrap)
			return;
		vcmd();
	}

	if (news)
		botscreen();
	ttycooked();
	if (!news)
		fprintf(stderr, "No news.\n");
}

/*
 * Read and execute a command.
 */

vcmd() {
	register c;
	char *p;
	long count;
	int countset;

	appfile(fp, dlinno + ARTWLEN + 1);
#ifdef DIGPAGE
	endsuba = findend(dlinno);
	if (artlines > dlinno + ARTWLEN
	 || endsuba > 0 && endsuba < artlines
#else
	if (artlines > dlinno + ARTWLEN
#endif
	 || (prflags & HDRONLY) && artlines > hdrend) {
		atend = 0;
		if (prflags&HDRONLY)
			strcpy(prompt,"more? ");
		else
			sprintf(prompt,"more(%d%%)? ",((dlinno+ARTWLEN-hdrend)*100)/maxlinno);
	} else {
		atend = 1;
		strcpy(prompt, "next? ");
		if (!erased)
			clear(bit);		/* article read */
	}
	curflag = CURP1;
	p = prompt + strlen(prompt);
	countset = 0;
	count = 0;
	while ((c = vgetc()) >= '0' && c <= '9' || c==cerase || c ==ckill) {
		if (c == cerase) {
			count /= 10;
			if (count == 0)
				countset = 0;
			continue;
		}
		if (c == ckill) {
			countset = 0;
			count = 0;
			continue;
		}
		count = (count * 10) + (c - '0');
		sprintf(p, "%ld", count);
		countset = 1;
	}
	if (c == '\033') {			/* escape */
		strcat(prompt, "M-");
		c = vgetc();
		if (c != cintr)
			c |= META;
	}
	secpr[0] = '\0';
	if (countset == 0)
		count = 1;
	docmd(c, count);
	if (c != '?' && c != 'H')		/* UGGH */
		prflags &=~ HELPMSG;
	if (dlinno > hdrstart)
		prflags &=~ HDRONLY;
}


/*
 * Process one command, which has already been typed in.
 */
docmd(c, count)
char c;
long count;
{
	int i;
	long nart;
	char *findhist();

	switch (c) {

	/* Show more of current article, or advance to next article */
	case '\n':
	case ' ':
	case '\06':	/* Control-F for vi compat */
		prflags &=~ NOPRT;
		if (atend)
			goto next;
		else if (prflags & HDRONLY) {
			prflags &=~ HDRONLY;
			if (hasscroll)
				dlinno = hdrstart;}
#ifdef DIGPAGE
		else if (endsuba > 0)
			dlinno = endsuba;
#endif
		else if ((appfile(fp, dlinno + 2 * ARTWLEN), artread)
		 && hasscroll && artlines - dlinno <= ARTWLEN + 2)
			dlinno = artlines - ARTWLEN;
		else
			dlinno += ARTWLEN;
		break;

	/* No.  Go on to next article. */
	case '.':	/* useful if you have a keypad */
next:	case 'n':
		readmode = NEXT;
		FCLOSE(fp);
		clear(bit);
		saveart;
		nextbit();
		break;


	/* Back up count pages */
	case META|'v':
	case '\2':	/* Control-B */
		dlinno -= ARTWLEN * count;
		if (dlinno < 0)
			dlinno = 0;
		break;

	/* forward half a page */
	case '\4':	/* Control-D, as in vi */
		dlinno += ARTWLEN/2 * count;
		break;

	/* backward half a page */
	case '\25':	/* Control-U */
		dlinno -= ARTWLEN/2 * count;
		if (dlinno < 0)
			dlinno = 0;
		break;

	/* forward count lines */
	case '\16':	/* Control-N */
	case '\32':	/* Control-Z */
		dlinno += count;
		break;

	/* backwards count lines */
	case '\20':	/* Control-P */
	case '\31':	/* Control-Y */
		dlinno -= count;
		if (dlinno < 0)
			dlinno = 0;
		break;

	/* Turn displaying of article back on */
	case 'l':
	case 'd':
		prflags &=~ NOPRT;
		break;

	/* display header */
	case 'h':
		dlinno = hdrstart;
		prflags |= HDRONLY;
		prflags &=~ NOPRT;
		break;

	/*
	 * Unsubscribe to the newsgroup and go on to next group
	 */

	case 'U':
	case 'u':
		strcat(prompt, "u");
		c = vgetc();
		if (c == 'g') {
			obit = -1;
			FCLOSE(fp);
			zapng = TRUE;
			saveart;
			if (nextng()) {
				if (actdirect == BACKWARD)
					msg("Can't back up.");
				else
					quitflg = 1;	/* probably unnecessary */
			}
		} else {
			if (c != cintr && c != ckill)
				msg("Illegal command");
		}
		break;

		/* Print the current version of news */
	case 'v':
		msg("News version: %s", news_version);
		break;


	/* Decrypt joke.  Always does rot 13 */
	case 'D':
		appfile(fp, 32767);
		for (i = hdrend ; i < artlines ; i++) {
			register char ch, *p;
			tfget(linebuf, i);
			for (p = linebuf ; (ch = *p) != '\0' ; p++) {
				if (ch >= 'a' && c <= 'z')
					*p = (ch - 'a' + 13) % 26 + 'a';
				else if (ch >= 'A' && c <= 'Z')
					*p = (ch - 'A' + 13) % 26 + 'A';
			}
			tfput(linebuf, i);
		}
		prflags |= NEWART;
		prflags &=~ (HDRONLY|NOPRT);
		break;

		/* write out the article someplace */
		/* w writes out without the header */
	case 's':
	case 'w': {
		char *grn = groupdir;
		int wflags;

		msg("file: ");
		curflag = CURP2;
		while ((wflags = vgetc()) == ' ');
		if (wflags == cintr) {
			secpr[0] = '\0';
			break;
		}
		if (wflags == '|') {
			linebuf[0] = '|';
			if (prget("| ", linebuf+1))
				break;
		} else {
			pushback(wflags);
			if (prget("file: ", linebuf))
				break;
		}
		wflags = 0;
		if (c == 's')
			wflags |= SVHEAD;
		if (count != 1)
			wflags |= OVWRITE;
		bptr = linebuf;
		while( *bptr == ' ')
			bptr++;	/* strip leading spaces */

		if (*bptr != PIPECHAR && *bptr != '/') {
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
				bptr[0] = '\0';
			if (bptr[0])
				strcat(bptr, "/");
			if (hetyped[0] != '\0')
				strcat(bptr, hetyped);
			else
				strcat(bptr, "Articles");
		}
		vsave(bptr, wflags);
		break;
	}

		/* back up  */
	case '-':
		aabs = TRUE;
		if (!*ofilename1) {
			msg("Can't back up.");
			break;
		}
		FCLOSE(fp);
		hptr = h;
		h = hold;
		hold = hptr;
		strcpy(bfr, filename);
		strcpy(filename, ofilename1);
		strcpy(ofilename1, bfr);
		obit = bit;
		if (strcmp(groupdir, ogroupdir)) {
			strcpy(bfr, groupdir);
			selectng(ogroupdir, FALSE);
			strcpy(groupdir, ogroupdir);
			strcpy(ogroupdir, bfr);
			ngrp = 1;
			back();
		}
		bit = oobit;
		oobit = obit;
		obit = -1;
		getnextart(TRUE);
		return FALSE;

		/* skip forwards */
	case '+':
caseplus:	if (count == 0)
			break;
		saveart;
		last = bit;
		for (i = 0; i < count; i++) {
			nextbit();
			if ((bit > pngsize) || (rflag && bit < 1))
				break;
		}
		FCLOSE(fp);
		obit = -1;
		break;

	/* exit - time updated to that of most recently read article */
	case 'q':
		quitflg = 1;
		break;

	case 'x':
		onquit();
		break;

	/* cancel the article. */
	case 'c':
		strcpy(prompt, "cancel [n]? ");
		if (vgetc() != 'y') {
			msg("Article not cancelled");
			break;
		}
		cancel_command();
		break;

	/* escape to shell */
	case '!': {
		register char *p;
		int flags;

		p = linebuf;
		if (prget("!", p))
			break;
		flags = CWAIT;
		if (*p == '\0') {
			strcpy(linebuf, SHELL);
			flags = 0;
		}
		while (*p) p++;
		while (p > linebuf && p[-1] == ' ')
			p--;
		if (*--p == '&') {
			*p = '\0';
			flags = BKGRND;
		} else if (*p == '|') {
			*p = '\0';
			sprintf(bfr, "(%s)|mail '%s'", linebuf, username);
			strcpy(linebuf, bfr);
			flags |= BKGRND;
		} else {
			prflags |= NOPRT;
		}
		shcmd(linebuf, flags);
		break;
	}

	/* mail reply */
	case 'r':
		reply();
		break;


	/* next newsgroup */
	case 'N':
		FCLOSE(fp);
		if (next_ng_command())
			quitflg = 1;
		break;

	/*  mark the rest of the articles in this group as read */
	case 'K':
		saveart;
		while (bit <= ngsize && bit > minartno) {
			clear(bit);
			nextbit();
		}
		FCLOSE(fp);
		break;

	/* Print the full header */
	case 'H':
		{
		if (fp == NULL) {
			msg("No current article");
			break;
		}
		move(ARTWIN, 0);
		fseek(fp, 0L, 0);
		for (i=0;i<ARTWLEN;i++) {
			if (fgets(linebuf, COLS, fp) == NULL)
				break;
			if (linebuf[0] == '\n')
				break;
			linebuf[COLS] = '\0';
			addstr(linebuf);
		}
		for(;i<ARTWLEN; i++)
			addstr(linebuf);
		prflags |= HELPMSG|NEWART;
		}
		break;
	case 'b':	/* backup 1 article */
		count = bit - 1;
		/* NO BREAK */

	case 'A':	/* specific number */
		if (count > pngsize) {
			msg("not that many articles");
			break;
		}
		readmode = SPEC;
		aabs = TRUE;
		bit = count;
		obit = -1;
		FCLOSE(fp);
		break;

	/* display parent article */
	case 'p':
#ifdef MYDB
		if (hasdb && (ptr3 = findparent(h->ident, &nart)) != NULL) {
			msg("parent: %s/%ld", ptr3, nart);	/*DEBUG*/
			updscr();				/*DEBUG*/
			goto selectart;
		}
#endif
		if (h->followid[0] == '\0') {
			msg("no references line");
			break;
		}
		if ((ptr1 = rindex(h->followid, ' ')) != NULL)
			ptr1++;
		else
			ptr1 = h->followid;
		strcpy(linebuf, ptr1);
		msg("%s", linebuf);
		curflag = CURP2;
		updscr();		/* may take this out later */
		goto searchid;

	/* specific message ID. */
	case '<':
		/* could improve this */
		linebuf[0] = '<';
		if (prget("<", linebuf+1))
			break;
searchid:	secpr[0] = '\0';
		if (index(linebuf, '@') == NULL && index(linebuf, '>') == NULL) {
			ptr1 = linebuf;
			if (*ptr1 == '<')
				ptr1++;
			ptr2 = index(ptr1, '.');
			if (ptr2 != NULL) {
				*ptr2++ = '\0';
				sprintf(bfr, "<%s@%s.UUCP>", ptr2, ptr1);
				strcpy(linebuf, bfr);
			}
		}
		if (index(linebuf, '>') == NULL)
			strcat(linebuf, ">");

		ptr1 = findhist(linebuf);
		if (ptr1 == NULL) {
			msg("%s not found", linebuf);
			break;
		}
		ptr2 = index(ptr1, '\t');
		ptr3 = index(++ptr2, '\t');
		ptr2 = index(++ptr3, ' ');
		if (ptr2)
			*ptr2 = '\0';
		ptr2 = index(ptr3, '/');
		*ptr2++ = '\0';
		sscanf(ptr2, "%ld", &nart);

		/*
		 * Go to a given article.  Ptr3 specifies the newsgroup
		 * and nart specifies the article number.
		 */
selectart:	aabs = TRUE;
		FCLOSE(fp);
		saveart;
		strcpy(ogroupdir, ptr3);
		if (strcmp(groupdir, ogroupdir)) {
			strcpy(bfr, groupdir);
			selectng(ogroupdir, TRUE);
			strcpy(groupdir, ogroupdir);
			strcpy(ogroupdir, bfr);
			back();
		}
		bit = nart;
		oobit = obit;
		obit = -1;
		getnextart(TRUE);
		rfq = 0;
		break;

	/* follow-up article */
	case 'f':
		sprintf(bfr, "%s/%s %s", BIN, "postnews", goodone);
		shcmd(bfr, CWAIT);
		break;

	/* erase - pretend we haven't seen this article. */
	case 'e':
		erased = 1;
		set(bit);
		goto caseplus;	/* skip this article for now */

	case '#':
		msg("Article %ld of %ld", rfq ? oobit : bit, pngsize);
		break;

		/* error */
	case '?':
		{
			FILE *helpf;
			sprintf(linebuf, "%s/vnews.help", LIB);
			if ((helpf = fopen(linebuf, "r")) == NULL) {
				msg("Can't open help file");
				break;
			}
			move(ARTWIN, 0);
			while (fgets(linebuf, LBUFLEN, helpf) != NULL)
				addstr(linebuf);
			fclose(helpf);
			prflags |= HELPMSG|NEWART;
		}
		break;

	default:
		if (c != ckill && c != cintr)
			msg("Illegal command");
		break;
	}

	return FALSE;
}

cancel_command()
{
	tfilename = filename;
	/*readmode = SPEC; bug? */
	strcpy(rcbuf, h->path);
	ptr1 = index(rcbuf, ' ');
	if (ptr1)
		*ptr1 = 0;
	if (uid != ROOTID && strcmp(username, rcbuf)) {
		msg("Can't cancel what you didn't write.");
		return;
	}
	if (!cancel(stderr, h, 0)) {
		clear(bit);
		saveart;
		nextbit();
		obit = -1;
		fp = NULL;
	}
	FCLOSE(fp);
}

/*
 * Generate replies
 */

reply()
{
	char *arg[4];
	register FILE *rfp;
	char subj[132];
	char *nogomsg;
	register char *p;
	char *replyname();
	struct stat statb;
	time_t creatm;

	/* Put the user in the editor to create the body of the reply. */
	ed = getenv("EDITOR");
	if (ed == NULL || *ed == '\0')
		ed = DFTEDITOR;
	if (ed == NULL) {
		msg("You don't have an editor");
		return;
	}

	arg[0] = "/bin/sh";
	arg[1] = "-c";

	strcpy(tf, tft);
	mktemp(tf);
	close(creat(tf,0666));
	if ((rfp = fopen(tf, "w")) == NULL) {
		msg("Can't create %s", tf) ;
		return;
	}
	strcpy(subj, h->title);
	if (!prefix(subj, "Re:")){
		strcpy(bfr, subj);
		sprintf(subj, "Re: %s", bfr);
	}

#ifdef INTERNET
	if (h->sender[0])
		p = h->sender;
	else
#endif
		p = replyname(h);
	fprintf(rfp, "To: %s\n", p);
	fprintf(rfp, "Subject: %s\n", subj);
	fprintf(rfp, "In-reply-to: your article %s\n", h->ident);
	sprintf(rcbuf, "exec %s -t < %s; rm %s", MAILPARSER, tf, tf);
	nogomsg = "Mail not sent";
	putc('\n', rfp);
	fstat(fileno(rfp), &statb);
	creatm = statb.st_mtime;
	fclose(rfp);

	sprintf(edcmdbuf, "exec %s %s", ed, tf);
	arg[2] = edcmdbuf;
	arg[3] = NULL;
	if (prun(arg, 0) != 0) {
		msg("Couldn't run editor");
		unlink(tf);
		return;
	}

	if (access(tf, 4) || stat(tf, &statb)) {
		msg("%s: no input file.", nogomsg);
		unlink(tf);
		return;
	}
	if (statb.st_mtime == creatm) {
		msg("%s: cancelled.", nogomsg);
		unlink(tf);
		return;
	}

	arg[2] = rcbuf;
	arg[3] = NULL;
	prun(arg, BKGRND);
	prflags |= NOPRT;
}

next_ng_command()
{
	obit = -1;
	if (prget("group? ", linebuf))
		return FALSE;
	bptr = linebuf;
	if (!*bptr || *bptr == '-') {
		if (*bptr)
			actdirect = BACKWARD;
		saveart;
		if (nextng()) {
			if (actdirect == BACKWARD)
				msg("Can't back up.");
			else
				return TRUE;
		}
		return FALSE;
	}
	while (isspace(*bptr))
		bptr++;
	if (!validng(bptr)) {
		msg("No such group.");
		return FALSE;
	}
	readmode = SPEC;
	saveart;
	back();
	selectng(bptr, TRUE);
	return FALSE;
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
	long atol();

	noaccess = 0;
	if (minus)
		goto nextart2;	/* Kludge for "-" command. */

	if (bit == obit)	/* Return if still on same article as last time */
		return 0;

nextart:
	if (news) {
		curflag = CURHOME;
		_amove(0, 0);
		vflush();
	}
	dgest = 0;

	/* If done with this newsgroup, find the next one. */
	while (ngsize <= 0 || ((long) bit > ngsize) || (rflag && bit < minartno)) {
		if (nextng()) {
			if (actdirect == BACKWARD) {
				msg("Can't back up.");
				actdirect = FORWARD;
				continue;
			}
			else /* if (rfq++ || pflag || cflag) */
				return 1;
		}
		if (rflag)
			bit = ngsize + 1;
		else
			bit = -1;
		noaccess = 2;
	}

	/* speed things up by not searching for article -1 */
	if (bit < 0) {
		bit = minartno - 1;
		nextbit();
		aabs = FALSE;
		goto nextart;
	}

nextart2:
	if (rcreadok)
		rcreadok = 2;	/* have seen >= 1 article */
	sprintf(filename, "%s/%ld", dirname(groupdir), bit);
	if (rfq && goodone[0])	/* ??? */
		strcpy(filename, goodone);
	if (sigtrap == SIGHUP)
		return 1;
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
				msg("Can't open %s", dirname(groupdir));
			goto nextart;
		}
		nextnum = rflag ? minartno - 1 : ngsize + 1;
		while ((dir = readdir(dirp)) != NULL) {
			if (!dir->d_ino)
				continue;
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
		do {
			clear(bit);
			nextbit();
		} while (rflag ? (nextnum < bit) : (nextnum > bit));
		obit = -1;
		aabs = FALSE;
		goto nextart;
	} else
		noaccess = 0;

	if (hread(h, fp, TRUE) == NULL || (!rfq && !aselect(h, aabs))) {
badart:
		FCLOSE(fp);
		clear(bit);
		obit = -1;
		nextbit();
		aabs = FALSE;
		goto nextart;
	}
	aabs = FALSE;
	actdirect = FORWARD;
	news = TRUE;
	{	/* strip off any notesfile header */
		register c;
		register char *p = h->title + strlen(h->title) - 5;
		if (p > h->title
		 && (strcmp(p, " (nf)") == 0 || strcmp(p, "(nf)\"") == 0)) {
			if ((c = getc(fp)) != '#') {
				ungetc(c, fp);
			} else {
				while ((c = getc(fp)) != '\n' && c != EOF);
				while ((c = getc(fp)) != '\n' && c != EOF);
				while ((c = getc(fp)) != '\n' && c != EOF);
			}
		}
	}
	artbody = ftell(fp);
	fmthdr();
	artlines = lastlin;
	artread = 0;
	prflags |= NEWART;
	prflags &=~ NOPRT;
	if (! cflag && hdrend < ARTWLEN && !cflag)
		prflags |= HDRONLY;
	dlinno = 0;
	maxlinno = NLINES(h, fp);
	erased = 0;

	obit = bit;
	return 0;
}

/*
 * Print out whatever the appropriate header is
 */
fmthdr() {
	char *briefdate();

	lastlin = 0;
	if (ngrp) {
		pngsize = ngsize;
		ngrp--;
		if (!hflag) {
			sprintf(linebuf, "Newsgroup %s", groupdir);  tfappend(linebuf);
			tfappend("");
		}
	}
	hdrstart = lastlin;
	if (!hflag) {
		sprintf(linebuf, "Article %s %s",
			h->ident, briefdate(h->subdate));
		tfappend(linebuf);
	}
	vhprint(h, pflag ? 1 : 0);
	sprintf(linebuf, "(%d lines)", NLINES(h, fp)); tfappend(linebuf);
	tfappend("");
	hdrend = lastlin;
}

/*
 * Print the file header to the temp file.
 */
vhprint(hp, verbose)
register struct hbuf *hp;
int	verbose;
{
	register char	*p1, *p2;
	int	i;
	char	fname[BUFLEN];
	char *tailpath();

	fname[0] = '\0';		/* init name holder */

	p1 = index(hp->from, '(');	/* Find the sender's full name. */
	if (p1 == NULL && hp->path[0])
		p1 = index(hp->path, '(');
	if (p1 != NULL) {
		strcpy(fname, p1+1);
		p2 = index(fname, ')');
		if (p2 != NULL)
			*p2 = '\0';
	}

	sprintf(linebuf, "Subject: %s", hp->title);
	if ((i = strlen(linebuf) - 7) > 9
	 && strcmp(linebuf + i, " - (nf)") == 0
	 && (strncmp(linebuf+9, "Re: ", 4) != 0 || i < 9+39))
		linebuf[i] = '\0';		/* clobber "- (nf)" */
	tfappend(linebuf);
	if (!hflag && hp->keywords[0])
		sprintf(linebuf, "Keywords: %s", hp->keywords), tfappend(linebuf);
	if (verbose) {
		sprintf(linebuf, "From: %s", hp->from); tfappend(linebuf);
		sprintf(linebuf, "Path: %s", hp->path); tfappend(linebuf);
		if (hp->organization[0])
			sprintf(linebuf, "Organization: %s", hp->organization), tfappend(linebuf);
	}
	else {
		if (p1 != NULL)
			*--p1 = '\0';		/* bump over the '(' */
#ifdef INTERNET
		/*
		 * Prefer Path line if it's in internet format, or if we don't
		 * understand internet format here, or if there is no reply-to.
		 */
		sprintf(linebuf, "From: %s", hp->from);
#else
		sprintf(linebuf, "Path: %s", tailpath(hp));
#endif
		if (fname[0] || hp->organization[0]) {
			strcat(linebuf, " (");
			if (fname[0] == '\0') {
				strcpy(fname,hp->from);
				p2 = index(fname,'@');
				if (p2)
					*p2 = '\0';
			}
			strcat(linebuf, fname);
			if (hp->organization[0] && !hflag) {
				strcat(linebuf, " @ ");
				strcat(linebuf, hp->organization);
			}
			strcat(linebuf, ")");
		}
		tfappend(linebuf);
		if (p1 != NULL)
			*p1 = ' ';
		if (hp->ctlmsg[0]) {
			sprintf(linebuf, "Control: %s", hp->ctlmsg);
			tfappend(linebuf);
		}
	}

	strcpy(bfr, hp->nbuf);
	ngdel(bfr);
	if (verbose) {
		sprintf(linebuf, "Newsgroups: %s", bfr); tfappend(linebuf);
		sprintf(linebuf, "Date: %s", hp->subdate); tfappend(linebuf);
		if (hp->sender[0])
			sprintf(linebuf, "Sender: %s", hp->sender), tfappend(linebuf);
		if (hp->replyto[0])
			sprintf(linebuf, "Reply-To: %s", hp->replyto), tfappend(linebuf);
		if (hp->followto[0])
			sprintf(linebuf, "Followup-To: %s", hp->followto), tfappend(linebuf);
	}
	else if (strcmp(bfr, groupdir) != 0)
		sprintf(linebuf, "Newsgroups: %s", bfr), tfappend(linebuf);
}




#ifdef MYDB


char *
findparent(id, num)
char *id;
long *num;
{
	register char *rcp;
	struct artrec a;
	char idbuf[BUFSIZE];
	char *ngname();

	strcpy(idbuf, id);
	rcp = idbuf;
	while(*++rcp)
		if (isupper(*rcp))
			*rcp = tolower(*rcp);

	if (lookart(id, &a) == DNULL)
		return NULL;
	if (a.parent == DNULL)
		return NULL;
	readrec(a.parent, &a);
	*num = a.groups[0].artno;
	return ngname(a.groups[0].newsgroup);
}

#endif


/*
 * Append file to temp file, handling control characters, folding lines, etc.
 * We don't grow the temp file to more than nlines so that a user won't have
 * to wait for 20 seconds to read in a monster file from net.sources.
 * What we really want is coroutines--any year now.
 */

#define ULINE 0200
static char *maxcol;

appfile(iop, nlines)
register FILE *iop;
{
	register int c;
	register char *icol;	/* &linebuf[0] <= icol <= maxcol */

	if (artread || artlines >= nlines || iop == NULL)
		return;
	maxcol = linebuf;
	icol = linebuf;
	while ((c = getc(iop)) != EOF) {
		switch (c) {
		case ' ':
			if (icol == maxcol && icol < linebuf + LBUFLEN - 1) {
				*icol++ = ' ';
				maxcol = icol;
			} else {
				icol++;
			}
			break;
		case '\t':
			icol = (icol - linebuf &~ 07) + 8 + linebuf;
			growline(icol);
			break;
		case '\b':
			if (icol > linebuf) --icol;
			break;
		case '\n':
			outline();
			if (artlines >= nlines)
				return;
			icol = linebuf;
			break;
		case '\r':
			icol = linebuf;
			break;
		case '\f':
			outline(); outline(); outline();
			if (artlines >= nlines)
				return;
			icol = linebuf;
			break;
		default:
			if (c < ' ' || c > '~')
				break;
			else if (icol >= linebuf + LBUFLEN - 1)
				icol++;
			else if (icol == maxcol) {
				*icol++ = c;
				maxcol = icol; }
			else if (*icol == ' ')
				*icol++ = c;
			else if (c == '_')
				*icol++ |= ULINE;
			else
				*icol++ = (c | ULINE);
			break;
		}
	}
	if (maxcol != linebuf)		/* file not terminated with newline */
		outline();
	artread++;
}

growline(col)
char *col;
{
	while (maxcol < col && maxcol < linebuf + LBUFLEN - 1)
		*maxcol++ = ' ';
}

outline()
{
	*maxcol = '\0';
	if (strncmp(linebuf, ">From ", 6) == 0) {
		register char *p;
		for (p = linebuf ; (*p = p[1]) != '\0' ; p++);
	}
	tfappend(linebuf);
	if (maxcol > linebuf)
		artlines = lastlin;
	maxcol = linebuf;
}

prget(prompter, buf)
char *prompter, *buf;
{
	char *p, *q, *r;
	int c, lastc;

	curflag = CURP2;
	r = buf;
	lastc = '\0';
	for (;;) {
		*r = '\0';
		p = secpr;
		for (q = prompter ; *q ; q++)
			*p++ = *q;
		for (q = buf ; *q ; q++) {
			if (p < &secpr[SECPRLEN-1] && *q >= ' ' && *p <= '~')
				*p++ = *q;
		}
		*p = '\0';
		c = vgetc();
		if (c == '\n' || c == cintr) {
			break;
		}
		if (c == cerase) {
			if (lastc == '\\')
				r[-1] = c;
			else if (r > buf)
				r--;
		} else if (c == ckill) {
			if (lastc == '\\')
				r[-1] = c;
			else
				r = buf;
		} else {
			*r++ = c;
		}
		lastc = c;
	}
	curflag = CURHOME;
	secpr[0] = '\0';
	return (c == cintr);
}



/*
 * Execute a shell command.
 */

shcmd(cmd, flags)
char *cmd;
{
	char *arg[4];

	arg[0] = SHELL, arg[1] = "-c", arg[2] = cmd, arg[3] = NULL;
	return prun(arg, flags);
}


prun(args, flags)
char **args;
{
	int pid;
	int i;
	int (*savequit)();
	char *env[100], **envp;
	char a[BUFLEN + 2];
	extern char **environ;
	int pstatus, retval;

	if (!(flags & BKGRND)) {
		botscreen();
		ttycooked();
#ifdef SIGTSTP
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
#endif
	}
	while ((pid = fork()) == -1)
		sleep(1);		/* must not clear alarm */
	if (pid == 0) {
		for (i = 3 ; i < 20 ; i++)
			close(i);
		if (flags & BKGRND) {
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
#ifdef SIGTSTP
			signal(SIGTSTP, SIG_IGN);
			signal(SIGTTIN, SIG_IGN);
			signal(SIGTTOU, SIG_IGN);
#endif
			close(0); close(1);
			open("/dev/null", 2);
			dup(0);
		}
		/* set $A */
		sprintf(a, "A=%s", filename);
		env[0] = a;
		for (envp = env + 1 ; *environ != NULL && envp < env + 98 ; environ++)
			if ((*environ)[0] != 'A' || (*environ)[1] != '=')
				*envp++ = *environ;
		*envp = NULL;

		umask(savmask);
		execve(args[0], args, env);
		fprintf(stderr, "%s: not found\n", args[0]);
		exit(20);
	}
	if (!(flags & BKGRND)) {
		savequit = signal(SIGQUIT, SIG_IGN);
		while ((i = wait(&pstatus)) != pid && (i != -1 || errno == EINTR))
			;
		if (i == -1)
			retval = 1;
		else
			retval = pstatus;
		if (flags & CWAIT) {
			fprintf(stderr, "continue? ");
			while ((errno = 0, i = getchar()) != '\n'
				&& (i != EOF || errno == EINTR));
		}
		signal(SIGQUIT, savequit);
		ttyraw();
		clearok(curscr, 1);
#ifdef SIGTSTP
		signal(SIGTSTP, onstop);
		signal(SIGTTIN, onstop);
		signal(SIGTTOU, onstop);
#endif
		return retval;
	} else
		return 0;
}

#ifdef DIGPAGE


/*
 * Find end of current subarticle in digest.
 */

findend(l)
{
	register i;
	register char *p;

	for (i = l ; i < l + ARTWLEN && i < lastlin ; i++) {
		tfget(linebuf, i);
		for (p = linebuf ; *p == '-' ; p++);
		if (p > linebuf + 24)
			return i + 1;
	}
	return 0;
}

#endif


/*** Routines for handling temporary file ***/

/*
 * Append to temp file.
 * Long lines are folded.
 */

tfappend(tline)
char *tline;
{
	while (strlen(tline) > COLS) {
		tfput(tline, lastlin++);
		tline += COLS;
		maxlinno++;
	}
	tfput(tline, lastlin++);
}


tfput(tline, linno)
char *tline;
{
	register char *p;
	register FILE *rtfp;		/* try to make it a litte faster */
	register int i;

	p = tline, i = even(COLS);
	tfseek(linno, 1);
	rtfp = tfp;
	while (--i >= 0) {
		if (*p)
			putc(*p++, rtfp);
		else
			putc('\0', rtfp);
	}
	tflinno++;
}


tfget(tline, linno)
char *tline;
{
	tfseek(linno, 0);
	fread(tline, even(COLS), 1, tfp);
	tline[COLS] = '\0';
	tflinno++;
}


tfseek(linno, wrflag)
{
	static int lastwrflag = 1;

	if (linno != tflinno || wrflag != lastwrflag) {
		fseek(tfp, (long)linno * even(COLS), 0);
		tflinno = linno;
		lastwrflag = wrflag;
	}
}

/* VARARGS1 */
msg(s, a1, a2, a3, a4)
char *s;
{
	sprintf(secpr, s, a1, a2, a3, a4);
}


/*
 * Update the display.
 * The display is entirely controlled by this routine,
 * which means that this routine may get pretty snarled.
 */

static int savelinno = -1;		/* dlinno on last call to updscr */
static int savepr;			/* prflags on last call */

updscr()
{
	int count;
	int i;

	if (checkin())
		return;
	if ((prflags & HELPMSG) == 0
	 && (dlinno != savelinno || savepr != prflags)
	 && quitflg == 0) {
		if (dlinno != savelinno)
			prflags &=~ NOPRT;
		count = ARTWLEN;
		if (prflags & NOPRT)
			count = 0;
		if ((prflags & HDRONLY) && count > hdrend)
			count = hdrend - dlinno;
#ifdef DIGPAGE
		if (endsuba > 0 && count > endsuba - dlinno)
			count = endsuba - dlinno;
#endif
		if ((prflags & NEWART) == 0)
			ushift(ARTWIN, ARTWIN+ARTWLEN-1, dlinno - savelinno);
		if (count > lastlin - dlinno)
			count = lastlin - dlinno;
		for (i = ARTWIN ; i < ARTWIN + ARTWLEN ; i++)
			clrline(i);
		for (i = 0 ; i < count ; i++) {
			tfget(linebuf, dlinno + i);
			mvaddstr(ARTWIN + i, 0, linebuf);
		}
		prflags &=~ NEWART;
		savepr = prflags;
		savelinno = dlinno;
	}
	clrline(SPLINE), clrline(PRLINE);
#ifdef STATTOP
	mvaddstr(PRLINE, 0, prompt);
#else
	if (strlen(secpr) <= COLS)
		mvaddstr(PRLINE, 0, prompt);
#endif
	mvaddstr(PRLINE, 48, timestr);
	mvaddstr(PRLINE, 20, groupdir);
	addch(' '); addnum(bit); addch('/'); addnum(pngsize); addch(' ');
	if (ismail)
		mvaddstr(PRLINE, 62, ismail > 1? "MAIL" : "mail");
	mvaddstr(SPLINE, 0, secpr);
	if (curflag == CURP1)
		move(PRLINE, strlen(prompt));
	else if (curflag == CURHOME)
		move(0, 0);
	refresh();
}


addnum(n)
register long n;
{
	if (n >= 10)
		addnum(n / 10);
	addch((char)(n % 10 + '0'));
}



/*** alarm handler ***/

/*
 * Called on alarm signal.
 * Simply sets flag, signal processed later.
 */

onalarm()
{
#ifdef SIGTSTP
	int dojump = reading;

	reading = FALSE;
	alflag++;
	if (dojump)
		longjmp(alrmjmp, 1);
#else !SIGTSTP
	alflag++;
#endif
}


/*
 * Process alarm signal (or start clock)
 */

timer()
{
	time_t tod;
	int hour;
	int i;
	struct tm *t;
	struct stat statb;
	struct tm *localtime();
	static char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	static long oldmsize = 1000000L;
	static int rccount = 10;

	alflag = 0;
	signal(SIGALRM, onalarm);
	(void) time(&tod);
	t = localtime(&tod);
	i = 60 - t->tm_sec;
	alarm(i > 30? 30 : i);			/* reset alarm */
	hour = t->tm_hour % 12;
	if (hour == 0)  hour = 12;
	sprintf(timestr, "%.3s %d %d:%02d",
		months + 3 * t->tm_mon, t->tm_mday, hour, t->tm_min);
#ifdef GGRMAIL
	if (mailf == NULL || stat(mailf, &statb) < 0) {
		statb.st_size = 0;
	}
	if(statb.st_size > oldmsize) {
		ismail = 1;
		beep();
	} else if (statb.st_size < oldmsize) {
		ismail = 0;
	}
#else
	ismail = 0;
	if (mailf != NULL && stat(mailf, &statb) >= 0 && statb.st_size > 0L) {
		ismail = 1;
		if (oldmsize < statb.st_size) {
			ismail = 2;		/* new mail */
			beep();
		}
	} else {
		statb.st_size = 0L;
	}
#endif
	oldmsize = statb.st_size;
	if (uflag && !xflag && --rccount < 0) {
		writeoutrc();
		if (secpr[0] == '\0')
			strcpy(secpr, ".newsrc updated");
		rccount = 10;
	}
}


char *
getmailname()
{
	static char mailname[32];
	register char *p;

	if( (p = getenv("MAIL")) != NULL)
		return p;
	if (username[0] == '\0' || strlen(username) > 15)
		return NULL;
#ifdef USG
	sprintf(mailname, "/usr/mail/%s", username);
#else
	sprintf(mailname, "/usr/spool/mail/%s", username);
#endif
	return mailname;
}



/*** Terminal I/O ***/

#define INBUFSIZ 8

char inbuf[INBUFSIZ];			/* input buffer */
char outbuf[BUFSIZ];			/* output buffer */
int innleft = 0;			/* # of chars in input buffer */
int outnleft = BUFSIZ;			/* room left in output buffer */
char *innext;				/* next input character */
char *outnext = outbuf;			/* next space in output buffer */
#ifdef USG
int oflags;				/* fcntl flags (for nodelay read) */
#endif


/*
 * Input a character
 */

vgetc()
{
	register c;
#if defined(BSD4_2) || defined(BSD4_1C)
	int readfds, exceptfds;
#endif

recurse:
	if (--innleft >= 0) {
		c = *innext++;
	} else {
		if (alflag)
			timer();
		updscr();	/* update the display */
		for (;;) {
			if (innleft > 0 || alflag)
				goto recurse;
			intflag = 0;
#ifdef USG
			if (oflags & O_NDELAY) {
				oflags &=~ O_NDELAY;
				fcntl(0, F_SETFL, oflags);
			}
#endif
#ifdef SIGTSTP
			if (setjmp(alrmjmp))
				continue;
			if (setjmp(intjmp))
				return cintr;
			reading = TRUE;
#endif SIGTSTP
#if defined(BSD4_2) || defined(BSD4_1C)
			/* Use a select because it can be interrupted. */
			readfds = 1; exceptfds = 1;
			select(1, &readfds, (int *)0, &exceptfds, (int *)0);
			if (!(readfds & 1))
				break;
#endif
			innleft = read(0, inbuf, INBUFSIZ);
#ifdef SIGTSTP
			reading = FALSE;
#endif SIGTSTP
			if (innleft > 0)
				break;
			if (innleft == 0) {
				quitflg++;
				return cintr;
			}
			if (errno != EINTR)
				abort();	/* "Can't happen" */
			if (intflag) {
				intflag--;
				return cintr;
			}
		}
		innext = inbuf + 1;
		innleft--;
		c = inbuf[0];
	}
#ifndef USG
#ifndef CBREAK
	c &= 0177;
	if (c == '\034')	/* FS character */
		onquit();
#endif
#endif
	if (c == '\f') {
		clearok(curscr, 1);
		goto recurse;
	}
	if (c == '\r')
		c = '\n';
	return c;
}


/*
 * Push a character back onto the input stream.
 */

pushback(c)
{
	if (innext <= inbuf)
		abort();
	*--innext = c;
	innleft++;
}

/*
 * Check for terminal input
 */

checkin()
{
#ifdef FIONREAD
	int count;
#endif
#ifdef STATTOP
	if (innleft > 0)
#else
	if (innleft > 0 || alflag)
#endif
		return 1;
#if defined(USG) || defined(FIONREAD)
	if (ospeed == B9600)
		return 0;
	vflush();
	if (ospeed <= B300)
		ttyowait();
#ifdef USG
	if ((oflags & O_NDELAY) == 0) {
		oflags |= O_NDELAY;
		fcntl(0, F_SETFL, oflags);
	}
	if ((innleft = read(0, inbuf, INBUFSIZ)) > 0) {
		innext = inbuf;
		return 1;
	}
#endif
#ifdef FIONREAD
	count = 0;			/* in case FIONREAD fails */
	ioctl(0, FIONREAD, (char *)&count);
	if (count)
		return 1;
#endif
#endif
	return 0;
}



/*
 * flush terminal input queue.
 */

clearin()
{
#ifdef USG
	ioctl(0, TCFLSH, (char *)0);
#else
#ifdef TIOCFLUSH
	ioctl(0, TIOCFLUSH, (char *)0);
#else
	struct sgttyb tty;
	ioctl(0, TIOCGETP, &tty);
	ioctl(0, TIOCSETP, &tty);
#endif
#endif
	innleft = 0;
}

vputc(c)
{
	if (--outnleft < 0) {
		vflush();
		outnleft--;
	}
	*outnext++ = c;
}

/*
 * Flush the output buffer
 */

vflush()
{
	register char *p;
	register int i;
	unsigned oalarm;

	oalarm = alarm(0);
	for (p = outbuf ; p < outnext ; p += i) {
		if ((i = write(1, p, outnext - p)) < 0) {
			if (errno != EINTR)
				abort();	/* "Can't happen" */
			i = 0;
		}
	}
	outnleft = BUFSIZ;
	outnext = outbuf;
	alarm(oalarm);
}




/*** terminal modes ***/

#ifdef USG
static struct termio oldtty, newtty;

/*
 * Save tty modes
 */

ttysave()
{
	if (ioctl(1, TCGETA, &oldtty) < 0)
		xerror("Can't get tty modes");
	newtty = oldtty;
	newtty.c_iflag &=~ (INLCR|IGNCR|ICRNL);
	newtty.c_oflag &=~ (OPOST);
	newtty.c_lflag &=~ (ICANON|ECHO|ECHOE|ECHOK|ECHONL);
	newtty.c_lflag |=  (NOFLSH);
	newtty.c_cc[VMIN] = 1;
	newtty.c_cc[VTIME] = 0;
	cerase = oldtty.c_cc[VERASE];
	ckill = oldtty.c_cc[VKILL];
	cintr = oldtty.c_cc[VINTR];
	ospeed = oldtty.c_cflag & CBAUD;
	initterm();
}


/*
 * Set tty modes for visual processing
 */

ttyraw()
{
	while (ioctl(1, TCSETAF, &newtty) < 0 && errno == EINTR)
		;
	rawterm();
}

ttyowait()
{	/* wait for output queue to drain */
	while (ioctl(1, TCSETAW, &newtty) < 0 && errno == EINTR)
		;
}

/*
 * Restore tty modes
 */

ttycooked()
{
	cookedterm();
	while (ioctl(1, TCSETAF, &oldtty) < 0 && errno == EINTR)
		;
	oflags &=~ O_NDELAY;
	fcntl(0, F_SETFL, oflags) ;
}

#else

static struct sgttyb oldtty, newtty;

/*
 * Save tty modes
 */

ttysave()
{
#ifdef CBREAK
	struct tchars tchars;	/* special characters, including interrupt */
#endif
#ifdef SIGTSTP
	/* How to get/change terminal modes in a job control environment.
	   This code is right from the 4.1 bsd jobs(3) manual page.
	 */
	int tpgrp, getpgrp();

retry:
#ifdef BSD4_2
	sigblock(BIT(SIGTSTP)|BIT(SIGTTIN)|BIT(SIGTTOU));
#else !BSD4_2
	signal(SIGTSTP, SIG_HOLD);
	signal(SIGTTIN, SIG_HOLD);
	signal(SIGTTOU, SIG_HOLD);
#endif !BSD4_2
	if (ioctl(2, TIOCGPGRP, (char *)&tpgrp) < 0)
		goto nottty;
	if (tpgrp != getpgrp(0)) { /* not in foreground */
		signal(SIGTTOU, SIG_DFL);
#ifdef BSD4_2
		sigsetmask(sigblock(0) & ~BIT(SIGTTOU));
#endif BSD4_2
		kill(0, SIGTTOU);
		/* job stops here waiting for SIGCONT */
		goto retry;
	}
	signal(SIGTTIN, onstop);
	signal(SIGTTOU, onstop);
	signal(SIGTSTP, onstop);
#ifdef BSD4_2
	sigsetmask(sigblock(0) & ~(BIT(SIGTSTP)|BIT(SIGTTIN)|BIT(SIGTTOU)));
#endif BSD4_2
#endif SIGTSTP
	if (ioctl(1, TIOCGETP, (char *)&oldtty) < 0)
nottty:		xerror("Can't get tty modes");
	newtty = oldtty;
	newtty.sg_flags &=~ (CRMOD|ECHO|XTABS);
#ifdef CBREAK
	newtty.sg_flags |= CBREAK;
	ioctl(1, TIOCGETC, (char *)&tchars);
	cintr = tchars.t_intrc;
#else !CBREAK
	newtty.sg_flags |= RAW;
	cintr = '\0177';	/* forcibly this on V6 systems */
#endif !CBREAK
	cerase = oldtty.sg_erase;
	ckill = oldtty.sg_kill;
	ospeed = oldtty.sg_ospeed;
	initterm();
}


/*
 * Set tty modes for visual processing
 */

ttyraw()
{
	while (ioctl(1, TIOCSETN, (char *)&newtty) < 0 && errno == EINTR)
		;
	rawterm();
}

ttyowait()
{	/* wait for output queue to drain */
#ifdef TIOCDRAIN	/* This ioctl is a local mod on linus */
	ioctl(1, TIOCDRAIN, (char *)0);
#endif
}


/*
 * Restore tty modes
 */

ttycooked()
{
	cookedterm();
	while (ioctl(1, TIOCSETN, (char *)&oldtty) < 0 && errno == EINTR)
		;
}

#endif



/*** signal handlers ***/

onint() {
#ifdef SIGTSTP
	int dojump = reading;

	reading = FALSE;
#endif SIGTSTP
	if (!news) {
		ttycooked();
		xxit(1);
	}
	signal(SIGINT, onint);
	clearin();			/* flush input queue */
#ifdef SIGTSTP
	if (dojump)
		longjmp(intjmp, 1);
#endif SIGTSTP
	intflag++;
}

onquit()
{
	botscreen();
	vflush();
	ttycooked();
#ifdef SORTACTIVE
	unlink(ACTIVE);
#endif SORTACTIVE
#ifdef COREDUMP
	abort();
#else
	exit(0);
#endif
}

#ifdef SIGTSTP

onstop(signo)
int signo;
{
	/* restore old terminal state */
	botscreen();
	vflush();
	ttycooked();
	signal(signo, SIG_DFL);
#ifdef BSD4_2
	sigblock(BIT(SIGALRM)|BIT(SIGINT));
	sigsetmask(sigblock(0) & ~BIT(signo));
#endif BSD4_2
	kill(0, signo);	/* stop here until continued */

	fprintf(stderr,"Vnews restarted.");
	signal(signo, onstop);
	/* restore our special terminal state */
	ttyraw();
	clearok(curscr, 1);
	updscr();
#ifdef BSD4_2
	sigsetmask(sigblock(0) & ~(BIT(SIGALRM)|BIT(SIGINT)));
#endif BSD4_2
}

#endif


/*** stolen from rfuncs2.c and modified ***/

vsave(to, flags)
register char *to;
{
	register FILE *ufp;
	int	isprogram = 0;
	int	isnew = 1;
	long	saveoff;
	char	temp[20];
	char	*fname;
	char	prog[BUFLEN + 24];

	saveoff = ftell(fp);
	fseek(fp, artbody, 0);
	fname = to;
	if (*to == PIPECHAR) {
		if (strlen(to) > BUFLEN) {
			msg("Command name too long");
			goto out;
		}
		flags |= OVWRITE;
		strcpy(temp, "/tmp/vnXXXXXX");
		mktemp(temp);
		fname = temp;
		_amove(ROWS - 1, 0);
		vflush();
	}
	if ((flags & OVWRITE) == 0) {
		ufp = fopen(fname, "r");
		if (ufp != NULL) {
			fclose(ufp);
			isnew = 0;
		}
	}
	umask(savmask);

	if (*to == PIPECHAR)
		isprogram++;
	if ((ufp = fopen(fname, (flags & OVWRITE) == 0? "a" : "w")) == NULL) {
		msg("Cannot open %s", fname);
		goto out;
	}
	/*
	 * V7MAIL code is here to conform to V7 mail format.
	 * If you need a different format to be able to
	 * use your local mail command (such as four ^A's
	 * on the end of articles) substitute it here.
	 */
	if (flags & SVHEAD) {
#ifdef V7MAIL
		h->subtime = cgtdate(h->subdate);
		fprintf(ufp, "From %s %s",
#ifdef INTERNET
				h->from,
#else
				h->path,
#endif
					ctime(&h->subtime));
#endif
		hprint(h, ufp, 2);
#ifdef V7MAIL
		tprint(fp, ufp, TRUE);
		putc('\n', ufp);	/* force blank line at end (ugh) */
#else
		tprint(fp, ufp, FALSE);
#endif
	} else {
		tprint(fp, ufp, FALSE);
	}

	fclose(ufp);
	if (isprogram) {
		sprintf(prog, "(%s)<%s", to + 1, fname);
		shcmd(prog, CWAIT);
		prflags |= NOPRT;
	} else {
		if ((flags & OVWRITE) == 0)
			msg("file: %s %s", to, isnew ? "created" : "appended");
		else
			msg("file: %s written", to);
	}

out:
	if (isprogram) {
		unlink(fname);
	}
	umask(N_UMASK);
	fseek(fp, saveoff, 0);
}
