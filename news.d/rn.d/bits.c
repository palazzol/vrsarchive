/* $Header: /home/Vince/cvs/news.d/rn.d/bits.c,v 1.1 1990-02-26 22:31:50 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.2  87/12/29  14:33:12  vrs
 * Remove preprocessor fluff and incorrect cast on lhs of assignment
 * 
 * Version 1.1  87/12/29  10:18:29  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:36:15  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#include "EXTERN.h"
#include "common.h"
#include "rcstuff.h"
#include "head.h"
#include "util.h"
#include "final.h"
#include "rn.h"
#include "cheat.h"
#include "ng.h"
#include "artio.h"
#include "intrp.h"
#include "ngdata.h"
#include "rcln.h"
#include "kfile.h"
#include "INTERN.h"
#include "bits.h"

#ifdef DBM
#    ifdef NULL
#	undef NULL
#    endif
#    include <dbm.h>
#endif
MEM_SIZE ctlsize;			/* size of bitmap in bytes */

void
bits_init()
{
#ifdef DELAYMARK
    dmname = savestr(filexp(RNDELNAME));
#else
    ;
#endif
}

/* checkpoint the .newsrc */

void
checkpoint_rc()
{
#ifdef DEBUGGING
    if (debug & DEB_CHECKPOINTING) {
	fputs("(ckpt)",stdout);
	fflush(stdout);
    }
#endif
    if (doing_ng)
	restore_ng();			/* do not restore M articles */
    if (rc_changed)
	write_rc();
#ifdef DEBUGGING
    if (debug & DEB_CHECKPOINTING) {
	fputs("(done)",stdout);
	fflush(stdout);
    }
#endif
}

/* reconstruct the .newsrc line in a human readable form */

void
restore_ng()
{
    register char *s, *mybuf = buf;
    register ART_NUM i;
    ART_NUM count=0;
    int safelen = LBUFLEN - 16;

    strcpy(buf,rcline[ng]);		/* start with the newsgroup name */
    s = buf + rcnums[ng] - 1;		/* use s for buffer pointer */
    *s++ = rcchar[ng];			/* put the requisite : or !*/
    *s++ = ' ';				/* put the not-so-requisite space */
    for (i=1; i<=lastart; i++) {	/* for each article in newsgroup */
	if (s-mybuf > safelen) {	/* running out of room? */
	    safelen *= 2;
	    if (mybuf == buf) {		/* currently static? */
		*s = '\0';
		mybuf = safemalloc((MEM_SIZE)safelen + 16);
		strcpy(mybuf,buf);	/* so we must copy it */
		s = mybuf + (s-buf);
					/* fix the pointer, too */
	    }
	    else {			/* just grow in place, if possible */
		char *newbuf;

		newbuf = saferealloc(mybuf,(MEM_SIZE)safelen + 16);
		s = newbuf + (s-mybuf);
		mybuf = newbuf;
	    }
	}
	if (!was_read(i))		/* still unread? */
	    count++;			/* then count it */
	else {				/* article was read */
	    ART_NUM oldi;

	    sprintf(s,"%ld",(long)i);	/* put out the min of the range */
	    s += strlen(s);		/* keeping house */
	    oldi = i;			/* remember this spot */
	    do i++; while (i <= lastart && was_read(i));
					/* find 1st unread article or end */
	    i--;			/* backup to last read article */
	    if (i > oldi) {		/* range of more than 1? */
		sprintf(s,"-%ld,",(long)i);
					/* then it out as a range */
		s += strlen(s);		/* and housekeep */
	    }
	    else
		*s++ = ',';		/* otherwise, just a comma will do */
	}
    }
    if (*(s-1) == ',')			/* is there a final ','? */
	s--;				/* take it back */
    *s++ = '\0';			/* and terminate string */
#ifdef DEBUGGING
    if (debug & DEB_NEWSRC_LINE && !panic) {
	printf("%s: %s\n",rcline[ng],rcline[ng]+rcnums[ng]) FLUSH;
	printf("%s\n",mybuf) FLUSH;
    }
#endif
    free(rcline[ng]);			/* return old rc line */
    if (mybuf == buf) {
	rcline[ng] = safemalloc((MEM_SIZE)(s-buf)+1);
					/* grab a new rc line */
	strcpy(rcline[ng], buf);	/* and load it */
    }
    else {
	mybuf = saferealloc(mybuf,(MEM_SIZE)(s-mybuf)+1);
					/* be nice to the heap */
	rcline[ng] = mybuf;
    }
    *(rcline[ng] + rcnums[ng] - 1) = '\0';
    if (rcchar[ng] == NEGCHAR) {	/* did they unsubscribe? */
	printf(unsubto,ngname) FLUSH;
	toread[ng] = TR_UNSUB;		/* make line invisible */
    }
    else
	/*NOSTRICT*/
	toread[ng] = (ART_UNREAD)count;		/* remember how many unread there are */
}

/* mark an article unread, keeping track of toread[] */

void
onemore(artnum)
ART_NUM artnum;
{
#ifdef DEBUGGING
    if (debug && artnum < firstart) {
	printf("onemore: %d < %d\n",artnum,firstart) FLUSH;
	return;
    }
#endif
    if (ctl_read(artnum)) {
	ctl_clear(artnum);
	++toread[ng];
    }
}

/* mark an article read, keeping track of toread[] */

void
oneless(artnum)
ART_NUM artnum;
{
#ifdef DEBUGGING
    if (debug && artnum < firstart) {
	printf("oneless: %d < %d\n",artnum,firstart) FLUSH;
	return;
    }
#endif
    if (!ctl_read(artnum)) {
	ctl_set(artnum);
	if (toread[ng] > TR_NONE)
	    --toread[ng];
    }
}

/* mark an article as unread, making sure that firstart is properly handled */
/* cross-references are left as read in the other newsgroups */

void
unmark_as_read(artnum)
ART_NUM artnum;
{
    check_first(artnum);
    onemore(artnum);
#ifdef MCHASE
    if (!parse_maybe(artnum))
	chase_xrefs(artnum,FALSE);
#endif
}

#ifdef DELAYMARK
/* temporarily mark article as read.  When newsgroup is exited, articles */
/* will be marked as unread.  Called via M command */

void
delay_unmark(artnum)
ART_NUM artnum;
{
    if (dmfp == Nullfp) {
	dmfp = fopen(dmname,"w");
	if (dmfp == Nullfp) {
	    printf(cantcreate,dmname) FLUSH;
	    sig_catcher(0);
	}
    }
    oneless(artnum);			/* set the correct bit */
    dmcount++;
    fprintf(dmfp,"%ld\n",(long)artnum);
}
#endif

/* mark article as read.  If article is cross referenced to other */
/* newsgroups, mark them read there also. */

void
mark_as_read(artnum)
ART_NUM artnum;
{
    oneless(artnum);			/* set the correct bit */
    checkcount++;			/* get more worried about crashes */
    chase_xrefs(artnum,TRUE);
}

/* make sure we have bits set correctly down to firstart */

void
check_first(min)
ART_NUM min;
{
    register ART_NUM i = firstart;

    if (min < absfirst)
	min = absfirst;
    if (min < i) {
	for (i--; i>=min; i--)
	    ctl_set(i);		/* mark as read */
	firstart = min;
    }
}

/* bring back articles marked with M */

#ifdef DELAYMARK
void
yankback()
{
    register ART_NUM anum;

    if (dmfp) {			/* delayed unmarks pending? */
#ifdef VERBOSE
	printf("\nReturning %ld Marked article%s...\n",(long)dmcount,
	    dmcount == 1 ? nullstr : "s") FLUSH;
#endif
	fclose(dmfp);
	if (dmfp = fopen(dmname,"r")) {
	    while (fgets(buf,sizeof buf,dmfp) != Nullch) {
		anum = (ART_NUM)atol(buf);
		/*NOSTRICT*/
		onemore(anum);             /* then unmark them */
#ifdef MCHASE
		chase_xrefs(anum,FALSE);
#endif
	    }
	    fclose(dmfp);
	    dmfp = Nullfp;
	    UNLINK(dmname);		/* and be tidy */
	}
	else {
	    printf(cantopen,dmname) FLUSH;
	    sig_catcher(0);
	}
    }
    dmcount = 0;
}
#endif
    
/* run down xref list and mark as read or unread */

int
chase_xrefs(artnum,markread)
ART_NUM artnum;
int markread;
{
#ifdef ASYNC_PARSE
    if (parse_maybe(artnum))		/* make sure we have right header */
	return -1;
#endif
#ifdef DBM
    {
	datum lhs, rhs;
	datum fetch();
	register char *idp;
	char *ident_buf;
	static FILE * hist_file = Nullfp;
#else
    if (
#ifdef DEBUGGING
	debug & DEB_FEED_XREF ||
#endif
	htype[XREF_LINE].ht_minpos >= 0) {
					/* are there article# xrefs? */
#endif
	char *xref_buf, *curxref;
	register char *xartnum;
	char *rver_buf = Nullch;
	static char *inews_site = Nullch;
	register ART_NUM x;
	char tmpbuf[128];

#ifdef DBM
	rver_buf = fetchlines(artnum,NGS_LINE);
					/* get Newsgroups */
	if (!index(rver_buf,','))	/* if no comma, no Xref! */
	    return 0;
	if (hist_file == Nullfp) {	/* Init. file accesses */
#ifdef DEBUGGING
	    if (debug)
		printf ("chase_xref: opening files\n");
#endif
	    dbminit(filexp(ARTFILE));
	    if ((hist_file = fopen (filexp(ARTFILE), "r")) == Nullfp)
		return 0;
	}
	xref_buf = safemalloc((MEM_SIZE)BUFSIZ);
	ident_buf = fetchlines(artnum,MESSID_LINE);
					/* get Message-ID */
#ifdef DEBUGGING
	if (debug)
	    printf ("chase_xref: Message-ID: %s\n", ident_buf);
#endif
	idp = ident_buf;
	while (*++idp)			/* make message-id case insensitive */
	    if (isupper(*idp))
	        *idp = tolower (*idp);
	lhs.dptr = ident_buf;		/* look up article by id */
	lhs.dsize = strlen(lhs.dptr) + 1;
	rhs = fetch(lhs);		/* fetch the record */
	if (rhs.dptr == NULL)		/* if null, nothing there */
	    goto wild_goose;
	fseek (hist_file, *((long *)rhs.dptr), 0);
					/* datum returned is position in hist file */
	fgets (xref_buf, BUFSIZ, hist_file);
#ifdef DEBUGGING
	if (debug)
	    printf ("Xref from history: %s\n", xref_buf);
#endif
	curxref = cpytill(tmpbuf, xref_buf, '\t') + 1;
	curxref = cpytill(tmpbuf, curxref, '\t') + 1;
#ifdef DEBUGGING
	if (debug)
	    printf ("chase_xref: curxref: %s\n", curxref);
#endif
#else
#ifdef DEBUGGING
	if (htype[XREF_LINE].ht_minpos >= 0)
#endif
	    xref_buf = fetchlines(artnum,XREF_LINE);
					/* get xrefs list */
#ifdef DEBUGGING
	else {
	    xref_buf = safemalloc((MEM_SIZE)100);
	    printf("Give Xref: ") FLUSH;
	    gets(xref_buf);
	}
#endif
#ifdef DEBUGGING
	if (debug & DEB_XREF_MARKER)
	    printf("Xref: %s\n",xref_buf) FLUSH;
#endif
	curxref = cpytill(tmpbuf,xref_buf,' ') + 1;

	/* Make sure site name on Xref matches what inews thinks site is.
	 * Check first against last inews_site.  If it matches, fine.
	 * If not, fetch inews_site from current Relay-Version line and
	 * check again.  This is so that if the new administrator decides
	 * to change the system name as known to inews, rn will still do
	 * Xrefs correctly--each article need only match itself to be valid.
	 */ 
	if (inews_site == Nullch || strNE(tmpbuf,inews_site)) {
	    char *t;

	    if (inews_site != Nullch)
		free(inews_site);
#ifndef NORELAY
	    rver_buf = fetchlines(artnum,RVER_LINE);
	    if ((t = instr(rver_buf,"; site ")) == Nullch)
#else

	    /* In version 2.10.3 of news or afterwards, the Relay-Version
	     * and Posting-Version header lines have been removed.  For
	     * the code below to work as intended, I have modified it to
	     * extract the first component of the Path header line.  This
	     * should give the same effect as did the old code with respect
	     * to the use of the Relay-Version site name.
	     */
	    rver_buf = fetchlines(artnum,PATH_LINE);
	    if ((t = instr(rver_buf,"!")) == Nullch)
#endif
		inews_site = savestr(nullstr);
	    else {
		char new_site[128];

#ifndef NORELAY
		cpytill(new_site,t + 7,'.');
#else
		cpytill(new_site,rver_buf,'!');
#endif
		inews_site = savestr(new_site);
	    }
	    if (strNE(tmpbuf,inews_site)) {
#ifdef DEBUGGING
		if (debug)
		    printf("Xref not from %s--ignoring\n",inews_site) FLUSH;
#endif
		goto wild_goose;
	    }
	}
#endif
	while (*curxref) {
					/* for each newsgroup */
	    curxref = cpytill(tmpbuf,curxref,' ');
#ifdef DBM
	    xartnum = index(tmpbuf,'/');
#else
	    xartnum = index(tmpbuf,':');
#endif
	    if (!xartnum)		/* probably an old-style Xref */
		break;
	    *xartnum++ = '\0';
	    if (strNE(tmpbuf,ngname)) {/* not the current newsgroup? */
		x = atol(xartnum);
		if (x)
		    if (markread) {
			if (addartnum(x,tmpbuf))
			    goto wild_goose;
		    }
#ifdef MCHASE
		    else
			subartnum(x,tmpbuf);
#endif
	    }
	    while (*curxref && isspace(*curxref))
		curxref++;
	}
      wild_goose:
	free(xref_buf);
#ifdef DBM
	free(ident_buf);
#endif
	if (rver_buf != Nullch)
	    free(rver_buf);
    }
    return 0;
}

int
initctl()
{
    char *mybuf = buf;			/* place to decode rc line */
    register char *s, *c, *h;
    register long i;
    register ART_NUM unread;
    
#ifdef DELAYMARK
    dmcount = 0;
#endif
    if ((lastart = getngsize(ng)) < 0)	/* this cannot happen (laugh here) */
	return -1;

    absfirst = getabsfirst(ng,lastart);	/* remember first existing article */
    if (!absfirst)			/* no articles at all? */
	absfirst = 1;			/* pretend there is one */
#ifndef lint
    ctlsize = (MEM_SIZE)(OFFSET(lastart)/BITSPERBYTE+20);
#endif
    ctlarea = safemalloc(ctlsize);	/* allocate control area */

    /* now modify ctlarea to reflect what has already been read */

    for (s = rcline[ng] + rcnums[ng]; *s == ' '; s++) ;
					/* find numbers in rc line */
    i = strlen(s);
#ifndef lint
    if (i >= LBUFLEN-2)			/* bigger than buf? */
	mybuf = safemalloc((MEM_SIZE)(i+2));
#endif
    strcpy(mybuf,s);			/* make scratch copy of line */
    mybuf[i++] = ',';			/* put extra comma on the end */
    mybuf[i] = '\0';
    s = mybuf;				/* initialize the for loop below */
    if (strnEQ(s,"1-",2)) {		/* can we save some time here? */
	firstart = atol(s+2)+1;		/* ignore first range thusly */
	s=index(s,',') + 1;
    }
    else
	firstart = 1;			/* all the bits are valid for now */
    if (absfirst > firstart) {		/* do we know already? */
	firstart = absfirst;		/* no point calling getngmin again */
    }
    else if (artopen(firstart) == Nullfp) {
					/* first unread article missing? */
	i = getngmin(".",firstart);	/* see if expire has been busy */
	if (i) {			/* avoid a bunch of extra opens */
	    firstart = i;
	}
    }
#ifdef PENDING
#   ifdef CACHESUBJ
	subj_to_get = firstart;
#   endif
#endif
    unread = lastart - firstart + 1;	/* assume this range unread */
    for (i=OFFSET(firstart)/BITSPERBYTE; i<ctlsize; i++)
	ctlarea[i] = 0;			/* assume unread */
#ifdef DEBUGGING
    if (debug & DEB_CTLAREA_BITMAP) {
	printf("\n%s\n",mybuf) FLUSH;
	for (i=1; i <= lastart; i++)
	    if (! was_read(i))
		printf("%ld ",(long)i) FLUSH;
    }
#endif
    for ( ; (c = index(s,',')) != Nullch; s = ++c) {
					/* for each range */
	ART_NUM min, max;

	*c = '\0';			/* do not let index see past comma */
	if ((h = index(s,'-')) != Nullch) {	/* is there a -? */
	    min = atol(s);
	    max = atol(h+1);
	    if (min < firstart)		/* make sure range is in range */
		min = firstart;
	    if (max > lastart)
		max = lastart;
	    if (min <= max)		/* non-null range? */
		unread -= max - min + 1;/* adjust unread count */
	    for (i=min; i<=max; i++)	/* for all articles in range */
		ctl_set(i);		/* mark them read */
	}
	else if ((i = atol(s)) >= firstart && i <= lastart) {
					/* is single number reasonable? */
	    ctl_set(i);			/* mark it read */
	    unread--;			/* decrement articles to read */
	}
#ifdef DEBUGGING
	if (debug & DEB_CTLAREA_BITMAP) {
	    printf("\n%s\n",s) FLUSH;
	    for (i=1; i <= lastart; i++)
		if (! was_read(i))
		    printf("%ld ",(long)i) FLUSH;
	}
#endif
    }
#ifdef DEBUGGING
    if (debug & DEB_CTLAREA_BITMAP) {
	fputs("\n(hit CR)",stdout) FLUSH;
	gets(cmd_buf);
    }
#endif
    if (mybuf != buf)
	free(mybuf);
    toread[ng] = unread;
    return 0;
}

void
grow_ctl()
{
    ART_NUM newlast;
    ART_NUM tmpfirst;
    MEM_SIZE newsize;
    register ART_NUM i;

    forcegrow = FALSE;
    newlast = getngsize(ng);
    if (newlast > lastart) {
	ART_NUM tmpart = art;
#ifndef lint
	newsize = (MEM_SIZE)(OFFSET(newlast)/BITSPERBYTE+2);
#else
	newsize = Null(MEM_SIZE);
#endif
	if (newsize > ctlsize) {
	    newsize += 20;
	    ctlarea = saferealloc(ctlarea,newsize);
	    ctlsize = newsize;
	}
	toread[ng] += (ART_UNREAD)(newlast-lastart);
	for (i=lastart+1; i<=newlast; i++)
	    ctl_clear(i);	/* these articles are unread */
#ifdef CACHESUBJ
	if (subj_list != Null(char**)) {
#ifndef lint
	    subj_list = (char**)saferealloc((char*)subj_list,
		  (MEM_SIZE)((OFFSET(newlast)+2)*sizeof(char *)) );
#endif
	    for (i=lastart+1; i<=newlast; i++)
		subj_list[OFFSET(i)] = Nullch;
	}
#endif
	tmpfirst = lastart+1;
	lastart = newlast;
#ifdef KILLFILES
#ifdef VERBOSE
	IF(verbose)
	    sprintf(buf,
		"%ld more article%s arrived--looking for more to kill...\n\n",
		(long)(lastart - firstart + 1),
		(lastart > firstart ? "s have" : " has" ) );
	ELSE			/* my, my, how clever we are */
#endif
#ifdef TERSE
	    strcpy(buf, "More news--killing...\n\n");
#endif
	kill_unwanted(tmpfirst,buf,TRUE);
#endif
	art = tmpart;
    }
}

