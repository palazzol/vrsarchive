/*
 * Usenet header parsing and generation (see RFCs 850 & 822;
 *	for a second opinion, see The Hideous Name by Pike & Weinberger).
 *
 * Headers are parsed and modified and copied in one pass.
 * Nevertheless, this file is split into two pieces: header parsing
 * and remembering (aka eating) and header copying (aka munging).
 * The split is marked and the two pieces are fairly independent.
 *
 * --- a quick tour of RFC 850 headers.  Hold on tight ---
 *
 * mandatory headers follow:
 * Path: ucbvax!ukc!decvax!mcvax!ih*!kaist!mh*!ho*!brahms!nsu	# must prepend to
 * From: whosit@brahms.b.uc.berkeley.edu.BERKELEY.EDU.uucp (Arthur ``Two-Sheds'' Jackson)
 * Newsgroups: talk.philosophy.meaning.meaning
 * Subject: Re: RE: re: rE: Orphaned Response - (nf)	# previously Title:
 * Date: Sat, 25-Dec-86 04:05:06 GMT	# previously Posted:
 * Message-ID: <unique@brahms.b.uc.berkeley.edu.BERKELEY.EDU.uucp> # previously Article-I.D.:
 *
 * optional headers follow:
 * Relay-Version: C;utcs			# must replace; must be 1st (recently demoted)
 * Posting-Version: version C alpha;site utcs.uucp	# (recently demoted)
 * Date-Received: Sat, 25-Jan-87 12:34:56 GMT	# must replace or snuff * # previously Received:
 * Organization: UCB society for arguing the meaning of meaning
 * Distribution: ucb
 * Sender: twosheds@arpa.b.uc.berkeley.edu.BERKELEY.EDU.uucp
 * Followup-To: talk.tv.i.love.lucy
 * Control: newgroup talk.philosophy.meaning.meaning.meaning	# magic; not for the uninitiated
 * References: <345.wanker@isi-wankers.gov> <123.toadsexers@kcl-cs.uk>
 * Reply-To: info-wankers@wankvax.b.uc.berkeley.edu.BERKELEY.EDU.uucp
 * Expires: Sun, 31-dec-99 23:59:59 GMT
 * Approved: kinsey@Uchicago.uucp		# the mark of the moderator
 * Lines: 6766					# redundant & pointless `wc -l`
 *
 * new headers not (yet?) in RFC850 follow:
 * Summary: It was a dark and stormy night,	# retain
 * Keywords: toadsexing, ether, tetanus		# retain
 * Nf-*: transmitted via Notesviles		# might snuff
 *
 * abusive & silly non-RFC-850 headers follow:
 * Sccs-Id: @(#) 5.2.vax.2.women.only  rob  3/5/83
 * Hideous-Name: psuvax1!rhea::@brl.arpa:ucbvax!mit-mc%udel-relay.arpa@chris:umcp-cs::udel-relay%csnet-relay.vision.ubc.cdn
 * Shoe-Size: 8
 * Header-Questions-To: mark@pavo.cb.d.osg.cb.cbosgd.att.com.uucp
 * Upas-To: eric@vax.b.uc.berkeley.edu.BERKELEY.EDU.uucp
 * V9-Capable-Machines-To: utstat!geoff
 * Uglix-Version: 5.3.vax.1.4/7/85.21:37:45.binary.only
 *
 * --- Ah well, it will only get worse. ---
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include "news.h"
#include "headers.h"

#ifndef DEFDIST
#define DEFDIST "world"		/* default Distribution: */
#endif
#ifndef DEFMSGID
#define DEFMSGID "<wanker@isi-wankers.isi.usc.edu>"
#endif

#define JUNK "junk"
#define ALL "all"

#define OLDCNTRL "all.all.ctl"

/*
 * Derivation: 752 is a large header, 120 is approx. size of *-Version because
 *	this rnews throws them away.  HDRMEMSIZ can be too small if
 *	memory is tight and will only hurt performance.
 */
#ifndef HDRMEMSIZ
#ifndef notdef
#define HDRMEMSIZ 8192			/* room for headers */
#else
#define HDRMEMSIZ (752-120)
#endif	/* notdef */
#endif	/* HDRMEMSIZ */

	/* from here down are recognised in eatheaders() */
static char ctlnm[] =	"Control:";
static char distrnm[] =	"Distribution:";
static char appnm[] =	"Approved:";
static char subjnm[] =	"Subject:";
static char msgnm[] =	"Message-ID:";
static char artidnm[] =	"Article-I.D.:";	/* obsolete Message-ID: */
	/* from here down recognised by mungehdrs() */
static char pathnm[] =	"Path:";		/* so we can extend it (damn) */
static char ngsnm[] = "Newsgroups:";		/* to clone for Xref */
	/* down to here recognised by eatheaders() */
static char xrefnm[] = "Xref:";			/* to *replace* Xref (damn!)*/
	/*
	 * the following noxious headers are deleted because
	 * neighbours still send them and they are big.
	 * in an ideal world, they wouldn't be sent and thus
	 * we wouldn't need to delete them.
	 */
/* TODO: what about Posted: and Article-I.D.:? */
static char datercvnm[] = "Date-Received:";	/* so we can snuff it */
static char rcvnm[] = "Received:";		/* obsolete Date-Received: */
static char rlyversnm[] = "Relay-Version:";	/* so we can snuff it */
static char postversnm[] = "Posting-Version:";	/* so we can snuff it */
	/* down to here recognised by mungehdrs() */


/* TODO: permit multiples of hdrprs and hpsp */
struct hdrparsestate {
	char **prevvalp;	/* points at previous header value string */
	short prevhist;		/* previous line was a header line */
	short nextcont;		/* next line must be a header continuation */
	short newnextcont;	/* nextcont for next line, based on this line */
	char hdraccum[HDRMEMSIZ+MAXLINE];
	char *endlnp;		/* pointer to end of input buffer - 1 */
};
static struct hdrparsestate hdrprs = { 0, NO, NO, 0 };	/* parser state */
static struct hdrparsestate *hpsp = &hdrprs;
/* end of parser state */


static int debug = NO;

/* forward decls */
char *skipsp();

/*
 * --- common header code start ---
 */

hdrdebug(state)
int state;
{
	debug = state;
}

hdrinit(hdrs)			/* zero all pointers in hdrs */
register struct headers *hdrs;
{
	hdrs->h_subj = NULL;
	hdrs->h_ngs = NULL;
	hdrs->h_files[0] = '\0';
	hdrs->h_distr = NULL;
	hdrs->h_ctlcmd = NULL;
	hdrs->h_approved = NULL;
	hdrs->h_msgid = NULL;
	hdrs->h_artid = NULL;
	hdrs->h_expiry = NULL;
	hdrs->h_path = NULL;
	hdrs->h_tmpf[0] = '\0';
	hdrs->h_unlink = NO;
	hdrs->h_filed = NO;
	hdrs->h_xref = NO;
	hdrs->h_octlchked = NO;
	hdrs->h_oldctl = NO;
	hdrs->h_accum = NULL;
	hdrs->h_charswritten = 0;
}

static int
oldctl(hdrs)			/* true iff ngs are OLDCNTRL (cache in hdrs) */
register struct headers *hdrs;
{
	if (!hdrs->h_octlchked) {		/* don't know yet */
		/* TODO: special case this, avoid ngmatch */
		hdrs->h_oldctl = ngmatch(OLDCNTRL, hdrs->h_ngs);
		hdrs->h_octlchked = YES;	/* now we know */
	}
	return hdrs->h_oldctl;
}

int
hdrmutate(hdrs, buffer, tfp)		/* eat & munge headers */
struct headers *hdrs;
char *buffer;
FILE **tfp;
{
    	eatheaders(hdrs, buffer);	/* mungehdrs needs n_ngs set */
	return mungehdrs(buffer, tfp, hdrs);	/* save or write hdr */
}

/*
 * --- header parsing and remembering starts here ---
 */

/*
 * Reset internal state of header parser.
 * (Empty the stomach of partially-digested headers.  Waarrrgggh!)
 */
hdrwretch()
{
	hpsp->prevvalp = NULL;
	hpsp->prevhist = NO;
	hpsp->nextcont = NO;
}

/*
 * Parse RFC822/850 header into "hdrs".  Retain significant values.
 * Assumes ishdr has been called first.
 */
eatheaders(hdrs, line)
register struct headers *hdrs;
register char *line;
{
	/*
	 * One would really like to use a loop through a structure here,
	 * but it's hard because one can't initialise a static struct
	 * with e.g. &hdrs->h_path.
	 */
	if (!contin(line) &&
	    !hdrmatch(line, pathnm, STRLEN(pathnm), &hdrs->h_path) &&
	    !hdrmatch(line, msgnm,  STRLEN(msgnm),  &hdrs->h_msgid) &&
	    !hdrmatch(line, artidnm,STRLEN(artidnm),&hdrs->h_artid) &&	/* obs. */
	    !hdrmatch(line, subjnm, STRLEN(subjnm), &hdrs->h_subj) &&
	    !hdrmatch(line, ngsnm,  STRLEN(ngsnm),  &hdrs->h_ngs) &&
	    !hdrmatch(line, distrnm,STRLEN(distrnm),&hdrs->h_distr) &&
	    !hdrmatch(line, appnm,  STRLEN(appnm),  &hdrs->h_approved) &&
	    !hdrmatch(line, ctlnm,  STRLEN(ctlnm),  &hdrs->h_ctlcmd)) {
		static char *dummy = NULL;

		/*
		 * silly header - just set hpsp->prevvalp for contin()
		 * so that unrecognised headers may be continued.
		 */
		if (dummy != NULL)
			free(dummy);
		dummy = strsave("");	/* may be realloced in contin() */
		hpsp->prevvalp = &dummy;	/* ditto */
	}
	hpsp->nextcont = hpsp->newnextcont;	/* set nextcont for next line */
}

hdrdeflt(hdrs)				/* default missing header values */
register struct headers *hdrs;
{
	/*
	 * if strsave ever returns NULL on failure, instead of exiting,
	 * then the following calls need to check for failure.
	 */
	if (hdrs->h_ngs == NULL)
		hdrs->h_ngs = strsave(JUNK);
	if (hdrs->h_msgid == NULL && hdrs->h_artid != NULL)
		hdrs->h_msgid = strsave(hdrs->h_artid);
	if (hdrs->h_msgid == NULL)
		hdrs->h_msgid = strsave(DEFMSGID);
	if (hdrs->h_expiry == NULL)
		hdrs->h_expiry = strsave("-");	/* - means "default" */
	if (hdrs->h_subj == NULL)
		hdrs->h_subj = strsave("");

	/*
	 * Control message backwards compatibility, and I mean *backwards*.
	 * We're talking stone age here; we're probably talking A news: if
	 * no Control: header exists and the newsgroup matches all.all.ctl,
	 * use the Subject: as the control message.
	 *
	 * Since RFC 850 is vague on the subject, we will henceforth
	 * internally (but not on disk), treat the Newsgroup: value as
	 * foo.bar, not foo.bar.ctl.  This simplifies the rest of
	 * processing somewhat.
	 */
	if (hdrs->h_ctlcmd == NULL && oldctl(hdrs)) {
		hdrs->h_ctlcmd = strsave(hdrs->h_subj);
		hdrs->h_ngs[strlen(hdrs->h_ngs) - STRLEN(".ctl")] = '\0';
	}

	if (hdrs->h_ctlcmd != NULL)		/* control message */
		hdrs->h_octlchked = NO;		/* invalidate old comparison */
	if (hdrs->h_distr == NULL)
		hdrs->h_distr = strsave(DEFDIST);
}

static int
iscontin(s)				/* is s an RFC 822 header continuation? */
register char *s;
{
	return hpsp->nextcont || hpsp->prevhist && iswhite(*s);
}

int
ishdr(s)				/* is s an RFC 822 header line? */
char *s;
{
	register char *cp = s;

	if (iscontin(s))
		hpsp->prevhist = YES;
	else {
		register int c;

		/* look for first of NUL, whitespace, colon */
		while ((c = *cp) != '\0' && !(isascii(c) && isspace(c)) &&
		    c != ':')
			++cp;
		hpsp->prevhist = (c == ':' && cp > s);	/* colon not 1st char */
	}
	/*
	 * If this is a header line and there is no trailing newline,
	 * assume fgets couldn't fit a very long header into the buffer
	 * for header lines, so the next line fgets sees must be a
	 * continuation of this line.
	 */
	if (hpsp->prevhist) {
#ifndef DIRTYHDRCONT
		INDEX(cp, '\n', cp);
		hpsp->newnextcont = (cp == NULL);	/* no \n -> continue */
#else	/* DIRTYHDRCONT */
		hpsp->newnextcont =
			*hpsp->endlnp != '\0' && *hpsp->endlnp != '\n';
#endif	/* DIRTYHDRCONT */
	} else
		hpsp->newnextcont = NO;
	return hpsp->prevhist;
}

int
contin(line)		/* append continuation value to old value */
char *line;
{
	/*
	 * If there is a previous header value and this line starts
	 * with whitespace other than a newline, realloc *hpsp->prevvalp
	 * with enough space for the old value, the new value and a NUL.
	 * Then append the new value.
	 */
	if (hpsp->prevvalp != NULL && iscontin(line)) {
		char *valp = /* skipsp */ (line);	/* continuation value */

		/* hpsp->prevvalp was previously set in hdrmatch() or eatheaders() */
		*hpsp->prevvalp = realloc(*hpsp->prevvalp,
			(unsigned)strlen(*hpsp->prevvalp) + strlen(valp) + 1);
		if (*hpsp->prevvalp == NULL)
			warning("realloc failed in contin", "");
		else {
			(void) strcat(*hpsp->prevvalp, valp);
			trim(*hpsp->prevvalp);	/* remove trailing newline */
		}
		return YES;
	} else
		return NO;
}

/*
 * Match line with keyword (return truth value).
 * If it matches, store the value in *malloc'ed memory* (N.B.)
 * and set *ptrp to point there.  freeheader() will free this memory.
 */
int
hdrmatch(line, keyword, keylen, ptrp)
register char *line, *keyword;
register int keylen;			/* an optimisation */
register char **ptrp;			/* make it point at valuep */
{
#ifdef notdef
	register int keylen = strlen(keyword);	/* the slower way */
#endif
	register int match = STREQN(line, keyword, keylen);

	if (match && *ptrp != NULL)	/* value already set */
		free(*ptrp);		/* return storage */
	if (match && (*ptrp = strsave(skipsp(&line[keylen]))) != NULL) {
		trim(*ptrp);		/* remove trailing new line */
		hpsp->prevvalp = ptrp;	/* for contin() */
	}
	return match;
}

freeheaders(hdrs)		/* free (assumed) malloced storage */
register struct headers *hdrs;
{
	nnfree(&hdrs->h_subj);
	nnfree(&hdrs->h_ngs);
	nnfree(&hdrs->h_distr);
	nnfree(&hdrs->h_ctlcmd);
	nnfree(&hdrs->h_approved);
	nnfree(&hdrs->h_msgid);
	nnfree(&hdrs->h_artid);
	nnfree(&hdrs->h_expiry);
	nnfree(&hdrs->h_path);
}

nnfree(mempp)				/* free non-null pointer's memory */
register char **mempp;			/* pointer to malloc'ed ptr. */
{
	if (*mempp != NULL) {
		free(*mempp);
		*mempp = NULL;
	}
}

int
emitxref(tf, hdrs)		/* splat out an Xref: line from Newsgroups: */
register FILE *tf;
struct headers *hdrs;
{
	register char *slashp;
	int status = ST_OKAY;
	char xrefs[MAXLINE];

	if (!hdrs->h_xref) {		/* this article has no Xref: yet */
		hdrs->h_xref = YES;
		(void) strcpy(xrefs, hdrs->h_files);
		/* turn slashes into colons for the benefit of rn */
		for (slashp = xrefs; (slashp = index(slashp, FNDELIM)) != NULL; )
			*slashp++ = ':';
		if (fprintf(tf, "%s %s %s\n", xrefnm, hostname(), xrefs) == EOF)
			status = fulldisk(status|ST_DROPPED,
				(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
	}
	return status;
}

/*
 * --- header munging (copying) starts here ---
 */

/*
 * Copy headers and munge a few.  Assumes eatheaders has been called.
 *
 * Don't copy Date-Received nor *-Version nor Xref.
 * Prepend hostname! to Path: value.
 * Recognise Newsgroups: and if more than one, generate Xref: &
 * leave holes for the article numbers - fileart will fill them in.
 * (Make rn look in history instead?)
 *
 * (Header munging should be a felony.  When they make me dictator,
 * it will be: punishable by having to use 4.2BSD networking.)
 *
 * New strategy: pile up headers into a static buffer until
 * end of buffer (next line might not fit) [checked in hdrsave()]; end of
 * headers, file or byte count [checked in cparttofp].
 * During reading, discard swill headers.  Path: is munged on output.
 * Copy (save) or discard header lines.
 */
int
mungehdrs(buffer, tfp, hdrs)
register char *buffer;
register FILE **tfp;
struct headers *hdrs;
{
	struct vilesthdrs {
		char *vh_name;
		unsigned vh_len;
	};
	register struct vilesthdrs *vhp;
	static struct vilesthdrs vilest[] = {
		datercvnm,	STRLEN(datercvnm),
		rcvnm,		STRLEN(rcvnm),
		rlyversnm,	STRLEN(rlyversnm),
		postversnm,	STRLEN(postversnm),
		xrefnm,		STRLEN(xrefnm),
		NULL,		0
	};

	if (debug)
		(void) fputs(buffer, stderr);
	/*
	 * Toss the most vile of the trash headers.
	 * In an ideal world, this code wouldn't exist.
	 */
	for (vhp = vilest; vhp->vh_name != NULL; vhp++)
		if (STREQN(buffer, vhp->vh_name, (int)vhp->vh_len))
			return ST_OKAY;
	/* wasn't vile; save it.  *tfp may be NULL.  Be Prepared. */
	return hdrsave(buffer, hdrs, tfp);	/* may set *tfp */
}

/*
 * If headers already dumped (hdrs->h_filed), just write to *tfp.
 * If there is room, stash "hdr" away until Newsgroups: is seen,
 * then open the first article link (on *tfp)
 * and dump the saved headers to it.
 * Copy into hdrs->h_accum (TODO: read in directly, iff high on profile).
 * TODO: may want an end-of-accum pointer for speed, iff high on profile.
 */
int
hdrsave(hdr, hdrs, tfp)
char *hdr;
register struct headers *hdrs;
FILE **tfp;
{
	int status = ST_OKAY;
	unsigned hdrlen = strlen(hdr);

	if (hdrs->h_filed)			/* *tfp != NULL */
		return emithdr(hdrs, hdr, *tfp);
	if (hdrs->h_accum == NULL) {
		hdrs->h_accum = hpsp->hdraccum;	/* primitive storage allocation */
		hdrs->h_accum[0] = '\0';
		hdrs->h_bytesleft = sizeof hpsp->hdraccum;
	}
	if (hdrs->h_bytesleft > hdrlen) {	/* it fits! */
		(void) strcat(hdrs->h_accum, hdr);	/* whomp it on the end */
		hdrs->h_bytesleft -= hdrlen;
	} else {				/* no room; barf out headers */
		status |= hdrdump(tfp, hdrs, NO);	/* don't trigger fileart */
		if (*tfp != NULL)
			status |= emithdr(hdrs, hdr, *tfp);
	}
	return status;
}

static int
emithdr(hdrs, hdr, tf)	/* munge Path: else just dump the header (hdr) */
register struct headers *hdrs;
char *hdr;
FILE *tf;
{
	register int status = ST_OKAY;

	if (STREQN(hdr, pathnm, STRLEN(pathnm))) {	/* Path: */
		register int hdrbytes;
		register char *oldpath;

		oldpath = skipsp(&hdr[STRLEN(pathnm)]);
		hdrbytes = fprintf(tf, "%s %s!", pathnm, hostname());
		if (hdrbytes == EOF || fputs(oldpath, tf) == EOF)
			status = fulldisk(status|ST_DROPPED,
				(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
		else
			hdrs->h_charswritten += hdrbytes + strlen(oldpath);
	} else {					/* ordinary header */
		if (fputs(hdr, tf) == EOF)
			status = fulldisk(status|ST_DROPPED,
				(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
		else
			hdrs->h_charswritten += strlen(hdr);
	}
	return status;
}

/*
 * Barf out headers after opening on *tfp either a temporary file (using
 * mktemp(3)) or the first article link, based on the h_ngs & nxtartnum();
 * set h_tmpf to which ever name is opened.  Modify Path: value on the way.
 */
int
hdrdump(tfp, hdrs, allhdrsseen)
FILE **tfp;
register struct headers *hdrs;
int allhdrsseen;
{
	int status = ST_OKAY;

	if (hdrs->h_filed)
		return status;
	/*
	 * If all headers were seen & the group was not an old backward-
	 * compatible control group (which won't exist), then open the
	 * first link, link to the rest, generate Xref:, else open a
	 * temporary name and write the article there
	 * (it will get filed later in insart()).
	 */
	if (allhdrsseen && hdrs->h_ngs != NULL && !oldctl(hdrs))
		status |= fileart(hdrs, tfp, 1);
	else {
		(void) strcpy(hdrs->h_tmpf, SPOOLTMP);
		(void) mktemp(hdrs->h_tmpf);	/* make a temporary name */
		hdrs->h_unlink = 1;		/* unlink it when done */
		if ((*tfp = fopen(hdrs->h_tmpf, "w")) == NULL) {
			warning("can't open temporary name `%s'", hdrs->h_tmpf);
			status |= ST_DROPPED;
		}
	}
	if (*tfp != NULL) {
		register char *line, *nlp;
		register int saved;

		/* this is a deadly tedious job and I really should automate it */
		for (line = hdrs->h_accum; line != NULL && line[0] != '\0';
		     line = nlp) {
		     	/*
		     	 * Could this call on INDEX be eliminated without
		     	 * restricting the number of header lines?
		     	 */
		     	INDEX(line, '\n', nlp);
			if (nlp != NULL) {
				++nlp;		/* byte after \n is NUL or text */
				saved = *nlp;
				*nlp = '\0';	/* will be restored below */
			}
			if (emithdr(hdrs, line, *tfp) == EOF)	/* dump saved headers */
				status = fulldisk(status|ST_DROPPED,
					(hdrs->h_unlink? hdrs->h_tmpf: hdrs->h_files));
		     	if (nlp != NULL)
		     		*nlp = saved;	/* restore  */
		}
	}
#ifdef notdef
	hdrs->h_accum = NULL;		/* primitive memory deallocation */
#endif
	return status;
}
