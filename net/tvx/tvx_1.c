/* ------------------------ tvx_1.c ------------------------------------- */
/* ========================================================================

	TVX - A full screen editor in C

	Originally developed by:

		Dr. Bruce E. Wampler
		University of New Mexico
		Department of Computer Science
		Farris Engineering Center
		Albuquerque, NM 87131

		uucp:   ..{ucbvax | gatech | ihnp4!lanl}!unmvax!wampler

	Public Domain version released July 1, 1985
		Direct comments, bug reports, suggestions to
		Bruce Wampler at above address.

	Converted from Ratfor to C January 1981 (note: since the editor
	    was originally in Ratfor, there are certain remnants of the
	    original structure left over.  There are a lot of things that
	    could have been done better if written in C originally.
	    Note also that this editor was originally designed in
	    1979 when TECO was state of the art.  The influence of
	    TECO on this editor will be apparent to anyone who has
	    used TECO.

	    So it goes.


	PLEASE! If you are making additional modifications, use the
	    indentation scheme used here (line up {,}'s!!!) instead
	    of the unmaintainable indentation used by K&R!.
	    Also, please mark your changes with initials and date!

	Description of files required: (names lower case on unix)

	TVX_1.C	   - main part of code (part 1), mostly os/terminal independent
	TVX_2.C	   - main part of code (part 2), mostly os/terminal independent
	TVX_LEX.C  - defaults, some os dependent stuff in here.  Major
		     changes in defaults can be fixed by recompiling this file.
	TVX_IO.C   - almost all I/O, including screen, confined to this file.
	TVX_LIB.C  - misc library routines needed by TVX.
	TVX_IBM.C  - IBM-PC specific code, specifically the screen driver
	  (TVX_IBM.ASM - hand optimized version of TVX_IBM.C)
	TVX_UNIX.C - contains unix specific code, including termcap driver
	TVX_CFG.C  - used to build CONFIG.TVX file for -c switch
        TVX_PTCH.C - used to permanently patch tvx with config file

	TVX_DEFS.IC - #define's for version, os, terminal, defaults
        TVX_GLBL.IC - global data structures
	TVX_TERM.IC - definitions for various terminals and systems

	Most distributions will contain other useful files as well.

============================================================================ */

#include "tvx_defs.ic"		/* note tv_defs.ic will #include stdio.h */
#include "tvx_glbl.ic"

/* =============================>>> MAIN <<<============================= */
  main (argc,argv)
  int argc;
  char *argv[];
  {

    checkos();		/* check operating system version */
    force_tty = FALSE;	/* won't usually force tty mode */

    tvinit();
    ttinit();		/* initialize tt: */
    trmini();		/* init terminal if needed */
    csrcmd();		/* make cursor command cursor */

    fopenx(argc,argv);	/* open the file, maybe change switches */

    tvidefs();		/* set defaults */
    opnbak();		/* may or may not be null routine */

    edit();		/* edit the file */

    clobak();		/* may be null routine */

    file_exit();	/* clean up files */

    ttymode = FALSE;

    if (*dest_file)
	remark(dest_file);	/* echo final file name */
    else
      {
	prompt("R/O, no changes: ") ; remark(orig_file);
      }

    reset();		/* reset anything necessary */
    quit();
  }

/* =============================>>> ASK <<<============================= */
  ask(msg,rply,rcnt)
  char *msg,*rply;
  int rcnt;
  {			/* get a reply, via tty if necessary */
    int oldtty;

    oldtty = ttymode;
    ttymode = FALSE;	/* force echo on */
    prompt(msg);
    reply(rply,rcnt);
    ttymode = oldtty;	/* back how it was */
  }

/* =============================>>> BEGLIN <<<============================= */
  beglin()
  {  /* beglin - move cursor to beginning of current line */

    SLOW int xf;

    curchr = *(lines+curlin) + 1;	/* point to current character */
    xf = findx();	/* this line needed to make the next */
			/* call eval order independent, if you wondered */
    tvxy(xf,tvy);	/* and move cursor */
  }

/* =============================>>> BOTPAG <<<============================= */
  botpag()
  { /* botpag - move cursor to bottom of current page (buffer) */

    curlin = nxtlin-1;		/* the last real line of text */
    curchr = *(lines+curlin) + 1; /* the first char of that line */
    endlin();			/* goto end of the line */
    newscr();			/* update the screen */
  }

/* ============================>>> CHK_RPT_NR <<<============================ */
  chk_rpt_nr(val)
  int val;
  {		/* see if val is in valid range */

    if (val < 0 || val > REPEATBUFS)	/* out of range */
      {
	tverrb("Bad rpt buff # ");
	return (FALSE);
      }
    else
	return (TRUE);
  }

/* =============================>>> CMDERR <<<============================= */
  cmderr(chr)
  char chr;
  {	/* cmderr - invalid command entered */

    static char badcmd[] = "Bad command:   ";

    if (chr >= ' ')
      {
	badcmd[13] = chr; 	/* stick in after : */
	badcmd[14] = ' ';
      }
    else
      {
	badcmd[13] = '^';
	badcmd[14] = chr + '@';
      }
    tverrb(badcmd);
  }

/* =============================>>> COMBIN <<<============================= */
  combin()
  { /* combin - combine current line with next line
		update screen -	cursor assumed to be on curlin */

    SLOW int to,from,xf;
    SLOW BUFFINDEX newl,k1,k2;

    if (curlin+1 >= nxtlin)		/* can't combine */
	return (FALSE);
    if (nxtsav-nxtchr < ALMOSTOUT)	/* check if need g.c. */
	if (! gbgcol())
	    return (FALSE);
    newl = nxtchr;			/* where next char goes */
    stcopy(buff,*(lines+curlin),buff,&nxtchr); /* copy over current line */
    curchr = nxtchr;			/* update the curchr */
    k1 = *(lines+curlin);	  	/* we will kill this line */
    *(lines+curlin) = newl;		/* remember where it is */
    stcopy(buff,*(lines+curlin+1)+1,buff,&nxtchr); /* append the next line */
    ++nxtchr;				/* fix nxtchr */
    to = curlin+1;
    k2 = *(lines+to);			/* we will kill this line */
    for (from=curlin+2; from < nxtlin ; )	/* copy line to end */
      {
	*(lines+to++) = *(lines+from++);
      }
    --nxtlin;		/* update line ptr */
    kline(k1);		/* kill the old lines now */
    kline(k2);
    if (tvdlin <= dsplin)	/* not at end of buffer */
      {
	tvescr();		/* erase rest of screen */
	tvxy(1,tvy);	/* fix it up */
	tvtype(curlin,min(tvlins-tvdlin+1,nxtlin-curlin));
      }
    else			/* at end of buffer */
	newscr();

    xf = findx();
    tvxy(xf,tvy); /* home cursor */

    return (TRUE);
  }

/* =============================>>> CTRLCH <<<============================= */
  ctrlch(chr)
  char chr;
  { /* ctrlch - echoes a control character for search and lex */

    if (chr >= ' ')
	tvcout(chr);	/* echo as is */
    else if (chr == CR)		/* carriage return may be special */
      {
	tvcout(CR);
#ifdef USELF
	tvcout(LF);	/*$$$ some machines need LF */
#endif
      }
    else if (chr == ESC)	/* escape as $ */
	tvcout('$');
    else			/* echo most chars as '^x' */
      {
	tvcout('^');
	tvcout(chr+'@');
      }
  }

/* =============================>>> DELNXT <<<============================= */
  int delnxt(cnt)
  int cnt;
  {  /* delnxt - delete next n characters  */

    char clower();
    static char chdel;
    SLOW int abscnt,newx;
    SLOW BUFFINDEX to;
    SLOW char ans[2];
    FAST int i;

    abscnt = cnt;	/* remember absolute value of cnt */
    if (cnt > 100 || cnt < -100)		/* make sure about this! */
      {
	tvclr();
	ask("Kill that many for sure? (y/n) ",ans,1);
	verify(1);
	if (clower(ans[0]) != 'y')
	    return (TRUE);
       }

    if (cnt > 0)		/* deleting forewards */
      {
	chdel = *(buff+curchr); /* remember the char we are deleting */
	for (i=1; curlin < nxtlin && i <= cnt; ++i) /* don't pass end of buff */
	  {
	    if (*(buff+curchr)==ENDLINE)	/* combine when end of line */
	      {
		if (! combin())
		  {
		    return (FALSE);
		  }
	      }
	    else		/* deleting one character */
	      {
		to=curchr;	/* overwrite current line */
		stcopy(buff,curchr+1,buff,&to); /* copy the rest of the line */
		for (++to; *(buff+to) != BEGLINE && to < nxtchr; ++to)
		    *(buff+to)=GARBAGE; /* mark the garbage characters */
	      }
	  }
      }
    else if (cnt < 0)		/* deleting backwards */
      {
	abscnt=(-cnt);
	chdel = *(buff+curchr-1); /* remember the char we are deleting */
	for (i=cnt; curlin >= 1 && i<0; ++i)	/* don't go past start */
	  {
	    if (*(buff+curchr-1)==BEGLINE)	/* deleting line separator */
	      {
		if (curlin > 1) 	/* not past beginning */
		  {
		    dwnlin(-1);		/* go up one line */
		    endlin();		/* get end of the line */
		    if (!combin())	/* and combine */
		      {
			return (FALSE);
		      }
		  }
	      }
	    else			/* killing a normal character */
	      {
		to=curchr-1;		/* overwrite in place */
		stcopy(buff,curchr,buff,&to); /* copy the rest of the line */
		for (++to; *(buff+to)!=BEGLINE && to < nxtchr; ++to)
		    *(buff+to)=GARBAGE; /* mark the garbage characters */
		--curchr;
	      }
	  }
      }
    newx=findx();		/* where cursor will go */
    tvxy(newx,tvy);		/* reposition cursor */
    if (chdel < ' ' || abscnt != 1)
	tvelin();		/* erase rest of the line */
    else			/* need to check for tabs following */
      {
	for (i = curchr ; *(buff+i)!=ENDLINE ; ++i)
	    if (*(buff+i) < ' ')
	      {
		tvelin();	/* need to erase the line */
		break;
	      }
      }
    tvtyln(curchr);		/* retype the rest */
    if (chdel >= ' ' && abscnt == 1 && last_col_out < tvcols)
	tvcout(' ');		/* "erase" last char on line */
    tvxy(newx,tvy);		/* restore the cursor */

    return (TRUE);
  }

/* =============================>>> DWNCOL <<<============================= */
  dwncol(cnt)
  int cnt;
  { /* dwncol - move down in column */

    SLOW int curcol,l,oldef,needns;

    needns = FALSE;
    if (leftmg > 1)		/* handle right virtual screen different */
      {
	oldef=echof;
	needns = TRUE;
	echof = FALSE;
      }

    if (oldlex==VDOWNCOL || oldlex==VUPCOL)	/* several in a row? */
	curcol=oldcol;		/* pick up old value */
    else
      {
	curcol = curchr - *(lines+curlin);	/* calculate the current column */
	oldcol = curcol;
      }
    dwnlin(cnt);		/* go down given lines */
    if (curlin>=1 && curlin<nxtlin && curcol>1)	/* not at ends? */
      {
	l = strlen(buff + ((*(lines+curlin)) + 1) );
	right(min(curcol-1,l));
      }

    if (needns)			/* needed new screen */
      {
	echof=oldef;
	newscr();
      }
  }

/* =============================>>> DWNLIN <<<============================= */
  dwnlin(cnt)
  int cnt;
  { /* dwnlin - move dot down cnt lines */

    SLOW int oldlin,change;

    if (curlin==nxtlin-1 && cnt > 0)	/* down from last line? */
      {
	endlin();
	return;
      }
    oldlin=curlin;		/* remember where we started from */
    curlin=max(min(curlin+cnt,nxtlin-1),1);	/* move down lines */
    curchr = *(lines+curlin)+1;	/* point to the current character */
    change=curlin-oldlin;	/* calculate how many lines changed */
    update(change);		/* update the screen */

  }

/* =============================>>> EDIT   <<<============================= */
  edit()
  { /*	edit - main editing routine */

    SLOW int lexval,lexcnt,succ, lastln, itmp;
    SLOW int noteloc[10], ni, lex_def;

    static int ins_set[] =
      {
	VINSERT, VOPENLINE, VQUIT, VABORT, VFBEGIN, VGET, VYANK, 0
      };

    static int jump_set[] =	/* commands to not reset jump memory */
      {
	VJUMP, VMEMORY, VHELP, VNOTELOC, VPRINTS, 0
      };

    static char lexchr;

    startm();
    remark("Reading file...");

    rdpage();			/* read a page into the buffer */

    tvclr();			/* clear the screen */

    if (curlin >= 1)
	tvtype(curlin,tvlins);	/* type out lines */

    tvxy(1,1);			/* and rehome the cursor */
    waserr = FALSE;		/* no errors to erase yet */

    if (curlin<1)
	tverr("Buffer empty");

    lexval = UNKNOWN;		/* so can remember 1st time through */
    useprint = FALSE;		/* not to printer */
    succ=TRUE;			/* assume success initially */

    lastln = curlin;		/* remember where we were */
    for (ni = 0 ; ni < 10 ; noteloc[ni++] = curlin)
	;			/* init noteloc */
    do
      {
	oldlex = lexval;		/* remember last command */
	if (! succ)
	    echof = TRUE;		/* resume echo when error */
	lex_def = lex(&lexval,&lexcnt,&lexchr,succ);	/* get command input */
	if (waserr)
	    fixend();
	waserr=FALSE;
	succ=TRUE;
	if (lexval == UNKNOWN)
	  {
	    cmderr(lexchr);
	    succ = FALSE;	/* announce failure to lex */
	  }
	else
	  {
	    if (curlin < 1)	/* make sure legal command for empty buffer */
	      {

		if (!inset(lexval,ins_set))
		  {
		    tverrb("Can't, buffer empty. Insert 1st ");
		    succ=FALSE;
		    continue;
		  }
	      }
	    if (!inset(lexval,jump_set))
		lastln=curlin;		/* let user look at help w/o changing */

	    switch (lexval)
	    {
case 1: 			/* right */
	    right(lexcnt);
	    break;
case 2: 			/* left */
	    right(-lexcnt);
	    break;
case 3: 			/* down line */
	    dwnlin(lexcnt);
	    break;
case 4: 			/* up line */
	    dwnlin(-lexcnt);
	    break;
case 5: 			/* down in column */
	    dwncol(lexcnt);
	    break;
case 6: 			/* up in column */
	    dwncol(-lexcnt);
	    break;
case 7: 			/* delete last character */
	    succ = delnxt(-lexcnt);
	    break;
case 8: 			/* delete next character */
	    succ = delnxt(lexcnt);
	    break;
case 9: 			/* insert */
	    succ = insert(lexcnt,lex_def);
	    break;
case 10:			/* kill a line */
	    killin(lexcnt);
	    break;
case 11:			/* kill rest of line */
	    krest();
	    break;
case 12:			/* kill previous part of line */
	    kprev();
	    break;
case 13:			/* move to beginning of line */
	    beglin();
	    break;
case 14:			/* move to end of the line */
	    endlin();
	    break;
case 15:			/* search for a pattern */
	    succ = search(lexcnt,TRUE);
	    break;
case 16:			/* search for next part of a pattern */
	    succ = snext(lexcnt,TRUE);
	    break;
case 17:			/* flip screen */
	    dwnlin(min(lexcnt*tvlins,nxtlin-curlin+1));
	    break;
case 18:			/* goto top of page */
	    toppag();
	    break;
case 19:			/* goto to bottom of page */
	    botpag();
	    break;
case 20:			/* goto real beginning of the file */
	    succ = fbeg();
	    break;
case 21:			/* verify */
	    verify(lexcnt);
	    break;
case 22:			/* open new line */
	    openln(lexcnt);
	    succ = insert(1,TRUE); /* go into insert mode, insert mode */
	    break;
case 23:			/* delete last thing manipulated */
	    succ = rmvlst();
	    break;
case 24:			/* save lines in move buffer */
	    succ = save(lexcnt,FALSE);
	    break;
case 25:			/* get move buffer */
	    succ = getsav();
	    break;
case 26:			/* read in next page of file */
	    wtpage(lexcnt);	/* write out the current page */
	    succ = rdpage();	/* read in the next */
	    tvclr();
	    if (succ || lexcnt < 0)
		verify(1);
	    break;
case 27:			/* append external file to save buffer */
	    succ = addfil(lexcnt);
	    break;
case 28:			/* quit */
	    tvclr();
	    remark("Exit");
	    goto lquit;
case 29:			/* search again */
	    succ = search(lexcnt,FALSE); /* FALSE => don't read search string */
	    break;
case 30:			/* execute repeat buffer again */
	    if (lexcnt != 1)
		echof=FALSE;	/* turn off echo */
	    rptcnt[rptuse] = lexcnt > 0 ? lexcnt : (-lexcnt);
	    break;
case 31:			/* print memory status, etc. */
	    memory();
	    break;
case 32:			/* change a parameter */
	    setpar(lexcnt);
	    break;
case 33:			/* remove last and enter insert mode */
	    if ((succ = rmvlst()))
		succ = insert(1,TRUE);
	    break;
case 34:			/* unkill last line killed */
	    succ = unkill();
	    break;
case 35:			/* jump over a word */
	    wordr(lexcnt);
	    break;
case 36:			/* neg jump over word */
	    wordr(-lexcnt);
	    break;
case 37:			/* append to save buffer */
	    succ = save(lexcnt,TRUE);
	    break;
case 38:			/* print screen */
	    scrprint();
	    break;
case 39:			/* show repeat buffer + help*/
	    shoset();
	    break;
case 40:			/* flip screen half page */
	    dwnlin( min((lexcnt*tvlins)/2 , nxtlin-curlin+1) );
	    break;
case 41:			/* abort */
	    abort();
	    break;
case 42:			/* change characters */
	    if ((succ = delnxt(lexcnt)))
		succ = insert(1,TRUE);
	    break;
case 43:			/* jump back to last location */
	    itmp = curlin;
	    curlin = lastln;
	    curchr = *(lines+curlin)+1;	/* point to the current character */
	    verify(1);
	    lastln = itmp;
	    break;
case 44:			/* tidy up screen */
	    succ = neaten(lexcnt);
	    break;
case 45:			/* save current location */
	    if (lexcnt < 1 || lexcnt > 9)
		lexcnt = 0;
	    noteloc[lexcnt] = curlin;
	    break;
case 46:			/* return to noted location */
	    itmp = curlin;
	    if (lexcnt < 1 || lexcnt > 9)
		lexcnt = 0;
	    if (noteloc[lexcnt] >= nxtlin)
	      {
		tverrb("Line no longer there ");
		noteloc[lexcnt] = curlin;
	      }
	    else
	      {
		curlin = noteloc[lexcnt];
		curchr = *(lines+curlin)+1; /* point to the current character */
		verify(1);
		lastln = itmp;
	      }
	    break;

case 47:
	    opsystem();		/* call operating system */
	    break;

case 48:
	    if (lex_def)		/* default 1 passed */
		lexcnt = rptuse + 1;	/* use current repeat loop */
	    succ = edit_rpt(lexcnt);	/* edit repeat buffer */
	    break;

case 49:
	    succ = store_rpt(lexcnt);	/* store repeat buffer */
	    break;

case 50:
	    succ = exec_rpt(lexcnt);	/* execute repeat buffer */
	    break;

case 51:
	    succ = ins_pat(lexcnt);
	    break;
case 52:
	    succ = user_1(lexcnt);	/* user function 1 */
	    break;

case 53:
	    succ = user_2(lexcnt);	/* user function 2 */
	    break;
	    }  			/* end of switch */
	    continue;		/* next iteration of do loop */
	  } /* end of else */
      } /* end of do loop */
    while (1);

lquit:
    for ( wtpage(1) ; rdpage() ; wtpage(1) )	/* write whole file */
	;
    tvclr();
  }

/* =============================>>> EDIT_RPT <<<============================= */
  edit_rpt(val)
  int val;
  {			/* copy repeat buffer val into buffer for editing */

    SLOW char *cp;
    SLOW int start_line;
    
    if (val == 0)
	val = rptuse+1;

    if (!chk_rpt_nr(val))
        return FALSE;

    --val;		/* change to relative */

    beglin();		/* start by moving to beginning of current line */
    start_line = curlin;	/* where we started */

    
    ins_chr('#'); ins_chr(val+'1'); ins_chr(':');
	/* start with number */
    ins_chr('<');	/* insert start of repeat loop */

    for (cp = &rptbuf[val][0] ; *cp ; ++cp)
	ins_chr(*cp);
    ins_chr(27); ins_chr(27);	/* make a way for store_rpt to find end */

    ins_chr(CR);		/* terminate line */
    curlin = start_line;
    curchr = *(lines+curlin)+1;
    verify(1);
    
    return (TRUE);

  }

/* =============================>>> ENDLIN <<<============================= */
  endlin()
  { /* endlin - move cursor to end of the line */

    FAST int cnt;
    SLOW BUFFINDEX i;

    cnt=0;
    for (i=curchr; *(buff+i)!=ENDLINE; ++i)	/* find end of line */
	++cnt;
    right(cnt); 	/* move to end of line */
  }

/* =============================>>> EXEC_RPT <<<============================= */
  exec_rpt(knt)
  int knt;
  {			/* this is combination of k:r,n& */
    static char chr;
    static int val;

    if (! grptch(&chr))		/* get buffer # (k) to use */
	return (FALSE);

    val = chr - '0';		/* convert to 0 to 9 */

    if (!chk_rpt_nr(val))
        return FALSE;

    if (val > 0)		/* change to specific buffer */
	rptuse=val-1;	/* adjust for 0 index int */

    if (knt != 1)
	echof = FALSE;	/* turn off echo */

    rptcnt[rptuse] = knt > 0 ? knt : (-knt);

    return (TRUE);
  }

/* =============================>>> FINDDL <<<============================= */
  finddl(ibeg,cnt)
  int *ibeg,*cnt;
  {  /* finddl - find the display line
	known: current line, calculate where it would go on the screen */

    if (curlin <= dsplin)
      { 			/* it is in first part of the display */
	*ibeg = 1;
	*cnt = min(tvlins,nxtlin-1);
	tvdlin = curlin;		/* update the display line */
      }
    else if (nxtlin-curlin <= tvlins-dsplin)	/* at bottom of display */
      {
	*ibeg = max(1,nxtlin-tvlins);
	*cnt = min(tvlins,nxtlin-1);
	tvdlin=min(curlin,tvlins-(nxtlin-curlin)+1);
      }
    else			/* normal case: in middle */
      {
	*ibeg=max(1,curlin-dsplin+1);
	*cnt=min(tvlins,nxtlin-(*ibeg));
	tvdlin=dsplin;
      }
 }

/* =============================>>> FINDX  <<<============================= */
  int findx()
  {  /* findx - find the x position of the current character
		handles spacing for tabs, control characters etc */

    SLOW BUFFINDEX i;
    SLOW int pos,lmold;

    pos = 0;
    for (i = *(lines+curlin)+1; i<=curchr; ++i)
	if (*(buff+i-1)<' ' && *(buff+i-1)>0)  /* cur pos depends on last chr */
	    if (*(buff+i-1)==TAB)		/* handle tabs */
		for (++pos ; ((pos-1) % 8)!=0; ++pos)
		    ;
	    else		/* control characters (echoed as ^X) */
		pos += 2;	/* 2 spaces for other control character */
	else			/* normal character */
	    ++pos;

    lmold = leftmg;		/* old left margin */
    for (;;)
      {
	if (pos < leftmg)	/* won't fit on screen */
	    leftmg -= 16;	/* shift left */
	else if (pos >= tvcols+leftmg)
	    leftmg += 16;
	else
	    break;
      }

    if (leftmg != lmold)		/* this handles screen shift */
	newscr();

    return (pos-leftmg+1);
  }

/* =============================>>> FIXEND  <<<============================= */
  fixend()
  { /* fixend - fix the error message line */

    SLOW int lastl;

    lastl = curlin+(tvlins-tvdlin);	/* the last line on the display */
    tvxy(1,tvhardlines); 		/* get to last line */
    tvelin();
    if (lastl < nxtlin && tvlins == tvhardlines)  /* only if really there */
	tvtype(lastl,1);		/* write it out */
    if (curlin >= 1)
	tvhdln();			/* restore cursor */
    else
	tvxy(1,1);
   }

/* =============================>>> GBGCOL <<<============================= */
  int gbgcol()
  { /* gbgcol - retrieve unused space in buff */

    FAST int i;
    SLOW int lastln;
    SLOW BUFFINDEX nxtbad, nxtgud, to, from, whfrom, offset, newlin;

    tverrb("Compacting buffer ");	/* let the user know, it might take a while */
    offset = curchr - *(lines+curlin);	/* need to reset curchr later */

    for (nxtbad=1 ; *(buff+nxtbad)!=GARBAGE && nxtbad < nxtchr; ++nxtbad)
	    ;		/* find first space to free */
    nxtgud=nxtbad;
    lastln = 1; 	/* where to start search */
    do
      {
	to=nxtbad;
	for (from=nxtgud; *(buff+from)==GARBAGE && from<nxtchr; ++from)
	    ;			/* find the next non-garbage character */

/*  nxtbad pts to first junk character,
    nxtgud pts to next possibly good character */

	if (from >= nxtchr)
	    break;		/* at the end of the buffer */
	whfrom=from;		/* where it came from */
	newlin = to;		/* remember start */
	do
	  {
	    *(buff+to) = *(buff+from++);	/* copy good stuff up */
	  }
	while (*(buff+to++)!=ENDLINE);

	nxtbad=to ; nxtgud=from;

/*  now find the old line
    following algorithm assumes next line is likely to
    be near the previous line */

	for (i=lastln ; i<nxtlin ; ++i)	/* start where last looked */
	    if (*(lines+i)==whfrom)
	      {
		*(lines+i)=newlin;	/* point to new position */
		if (curlin==i)
		    curchr=newlin+offset;	/* fix curchr if need be */
		break;
	      }

	if (i >= nxtlin)	/* not found in second half */
	  {
	    for (i=1 ; i < lastln ; ++i)
		if (*(lines+i)==whfrom)
		  {
		    *(lines+i)=newlin;		/* point to new position */
		    if (curlin==i)
			curchr=newlin+offset;	/* fix curchr if need be */
		    break;
		  }
	    if (i >= lastln)		/* make sure we really found it */
	      {
		tverrb("Compactor lost. Quit NOW! ");
		for (i=1 ; i < 32000 ; ++i)
		    ;
		return (FALSE);
	      }
	  }
	lastln = i;			/* start at next line down */
      }
    while (nxtgud < nxtchr);

    for (to=nxtbad ; to<=nxtchr ; )
	*(buff+to++)=GARBAGE;

    nxtchr=nxtbad;			/* update the next free character */
    tverr("Compactor done");
    return (nxtsav-nxtchr >= 50);
 }

/* =============================>>> GETSAV <<<============================= */
  int getsav()
  { /* ## getsav - get text from save buffer */

    FAST int to,from;
    SLOW BUFFINDEX fromch;
    SLOW int newlin;

    if (mxbuff-nxtsav+savlin >= nxtsav-nxtchr)	/* g.c. */
	if (!gbgcol())
	  {
	    tverrb("No get room ");
	    return (FALSE);
	  }

    if (nxtsav==mxbuff)		/* nothing to save */
      {
	return (TRUE);
      }

    if (mxbuff-nxtsav+savlin >= nxtsav-nxtchr || mxline-nxtlin <= savlin)
      { 			/* room to get save buffer? */
	tverrb("No get room ");
	return (FALSE);		/* no room to save */
      }

/* check if in middle of line */
    if (curchr > lines[curlin]+1)
	ins_chr(CR);

/*   # move down line to make space for new */
    from=nxtlin-1;
    nxtlin=nxtlin+savlin;
    to=nxtlin-1;
    while (from >= curlin)	/* copy line ptrs down right amnt. */
	*(lines+(to--)) = *(lines+(from--));

    newlin=curlin;		/* will insert new lines here */
    curlin=to+1;
    fromch = mxbuff;		/* where taking saved stuff from */
    for ( ; newlin < curlin; ++newlin)
      {
	*(buff+nxtchr)=BEGLINE;	/* insert begline character */
	*(lines+newlin) = nxtchr++; /* update line ptrs to new line */
	do			/* copy stuff from save buffer */
	  {
	    *(buff+nxtchr++) = *(buff+fromch);
	  }
	while (*(buff+fromch--));
      }
    oldlen=0;
    savlen=savlin;
    newscr();
    return (TRUE);
  }

/* =============================>>> GRPTCH <<<============================= */
  int grptch(chr)
  char *chr;
  { /* grptch - gets a char from repeat buffer or gkbd */

    SLOW char tmpchr;

    if (rptcnt[rptuse]>0)	/* need to fetch from repeat buffer */
	if (nxtrpt[rptuse] > lstrpt[rptuse])
	  {
	    return (FALSE);
	  }
	else
	  {
	    *chr=rptbuf[rptuse][nxtrpt[rptuse]];
	    ++nxtrpt[rptuse];
	  }
    else
      {
	gkbd(&tmpchr);	/* read the character from the keyboard */
	*chr=tmpchr;
      }
    return (TRUE);
  }

/* =============================>>> ins_pat  <<<============================= */
  ins_pat(lexcnt)
  int lexcnt;
  {
    SLOW char *chrp;

    if (!*pat_buff)
	return (FALSE);
    for (chrp = pat_buff ; *chrp ; )	/* simply insert pattern buffer */
      {
	if (!ins_chr(*chrp++))	/* make sure it works */
	    return (FALSE);
      }

    return (TRUE); 
  }

/* =============================>>> save_pat  <<<============================= */
  save_pat()
  {  /* save the find pattern, based on oldlen */

    SLOW int i;
    SLOW char *chrp;

    
    if (oldlen <= 0)
      {
	pat_buff[0] = 0;
	return;				/* nothing to save */
      }

    for (i = 1 ; i <= oldlen ; ++i)	/* first, move left */
      {
	--curchr;
	if (*(buff+curchr) == BEGLINE)
	  {
	    if (curlin > 1)
	      {
		--curlin;
		for (curchr = *(lines+curlin) ; *(buff+curchr)!=ENDLINE ;
		  ++curchr)
		    ;		/* bump curchr to end of the line */
	      }
	    else
	      {
		++curchr;
		break;
	      }
	  }
      }

     /* now save, go back right */

    chrp = pat_buff;			/* put in pattern buffer */

    for (i = 1 ; i <= oldlen ; ++i)
      {
	if (*(buff+curchr)==ENDLINE)
	  {
	    if (curlin+1 >= nxtlin)
		break;		/* don't go beyond end! */
	    ++curlin;
	    curchr = *(lines+curlin)+1;
	    *chrp++ = CR;	/* make a cr */
	  }
	else
	  {
	    if ((chrp - 100) < pat_buff)	/* make sure enough room */
		*chrp++ = *(buff+curchr);
	    ++curchr;
	  }
      }
    *chrp = 0;					/* terminate */
  }

/* =============================>>> INSET <<<============================= */
  inset(val,set)
  int val,*set;
  {
     /* return true if val is in set set */

    while (*set)
	if (val == *set++)
	    return TRUE;
    return FALSE;
  }

/* =============================>>> ins_chr <<<============================= */
  ins_chr(ival)
  int ival;
  {
    return insert(ival,FALSE);		/* force insert */
  }

/* =============================>>> INSERT <<<============================= */
  insert(ival,how)
  int ival,how;
  { /* insert - insert a character

	if how is TRUE, then read characters from keyboard until
	get an escape, otherwise insert ival */

    SLOW BUFFINDEX from,to;
    SLOW BUFFINDEX curbuf,curend;
    SLOW int lenins, nocins, ityp, xf;
    SLOW BUFFINDEX abvchr;

    SLOW char chr;


    static int ins_msg = TRUE;	/* own variable */

    if (ins_msg)
        csrins();		/* change cursor */

    if (how)		/* how = 1 regular insert mode */
      {
	if (! grptch(&chr))	/* get char using grptch */
	    goto l9999;
	if (chr == ESC)		/* esc means done */
	  {
	    goto l1000;
	  }
      }
    else
	chr = ival;		/* use the passed value */

    if (chr==ENDLINE || chr==BEGLINE || chr==GARBAGE || (chr==ENDFILE && usecz))
	goto l9998;		/* don't allow this case! */

    if (curlin < 1)
      { 			/* buffer empty? */
	curlin=1;		/* init for initial insert */
	*(lines+1)=nxtchr;
	curchr=nxtchr+1;
	*(buff+nxtchr)=BEGLINE;
	*(buff+nxtchr+1)=ENDLINE;
	nxtchr += 2;
	nxtlin = 2;
      }

    lenins=0;			/* remember length of insert for rmvlst */

    do
      {
	if (nxtsav-nxtchr < ALMOSTOUT)
	    if (!gbgcol())
		goto l9999;	/* collect garbage if necessary */
	curbuf = *(lines+curlin);  /* pick up the pointer to current line */
	for (curend=curbuf; *(buff+curend)!=ENDLINE; ++curend)
	    ;			/* get line length */
	if (curend+1 < nxtchr)	/* not using last part of buffer */
	  {
	    if (curend-curbuf >= nxtsav-nxtchr)
		goto l9998;	/* no more room! */
	    curchr=nxtchr+(curchr-curbuf);	/* where curchr will be */
	    *(lines+curlin)=nxtchr;	/* new line goes here */
	    stcopy(buff,curbuf,buff,&nxtchr); /* copy the line to the end */
	    curend=nxtchr++;	/* reset end pointer */
	    kline(curbuf);	/* kill off the line */
	    curbuf = *(lines+curlin);	/* update beginning pointer */
	  }

/*   #	to here, ready to insert the new character at the end of the line */

	if (chr==' ' && wraplm > 1 && (tvx >= wraplm || leftmg > 1))	/* auto wrap? */
	      chr = CR;
#ifdef FILELF
	if (chr == LF && how)
	    ;			/* don't insert lfs in CR/LF systems, echo? */
	else if (chr == CR)	/* inserting a new line */
#else
	if (chr == CR)		/* inserting a new line */
#endif
	  {
	    if (nxtlin >= mxline)	/* any room? */
	      {
		tverrb("No more free lines for insert ");
		goto l9999;
	      }

	    for (from=curend; from >= curchr; --from)
		*(buff+from+2) = *(buff+from);		/* copy chars down */
	    nxtchr += 2;	/* bump nxtchr to free space */

	    *(buff+curchr) = ENDLINE;	/* mark as endline */
	    *(buff+curchr+1) = BEGLINE;	/* beginning of line */
	    ++lenins;

	    to=nxtlin;		/* move lines down */
	    for (from = nxtlin-1; from > curlin; )
	      { 		/* bump the lines down */
		*(lines+to--) = *(lines+from--);
	      }
	    ++nxtlin;	/* bump to next free line */

	    *(lines+curlin+1)=curchr+1;		/* remember where */

	    if (ins_msg)
	        fixend();				/* fix last line */
	    tvelin();	/* erase stuff after cr */

	    nocins = (leftmg > 1);	/* ciline no good if left marg > 1 */

	    dwnlin(1);	/* go down one line */

	    if (ciline[0] == 0 || nocins)
	      {
		tvescr();		/* erase the rest of the screen */
		ityp = min(tvlins-tvdlin+1,nxtlin-curlin);
	      }
	    else
	      {
		tvinsl(); 	/* insert a line */
		ityp = 1;
	      }

	    tvtype(curlin,ityp);
	    tvhdln();
	    if (ins_msg)
		csrins();			/* change cursor */

	    if (autoin && curlin > 2)		/* automatic indentation! */
	      {
		ins_msg = FALSE;		/* turn off insert message */
		abvchr = *(lines+curlin-1)+1;	/* prevous line */
		while (*(buff+abvchr)==' ' || *(buff+abvchr)==TAB)
		    if (!insert(*(buff+abvchr++),FALSE) )
		      {
			ins_msg = TRUE;
			goto l9999;
		      }
		    else if (ttymode)		/* hmm, now what? */
		      {
			ttymode = FALSE;
			ttwt(*(buff+abvchr-1));
			ttymode = TRUE;
		      }
		ins_msg = TRUE;
		fixend();
		csrins();			/* change cursor */
	      }
	  }
	else if (chr == delkey && how)
	  {
	    if (!delnxt(-1))	/* rubbing out last character */
		goto l9999;
	    --lenins;
	  }
	else			/* inserting on the current line */
	  {
	    to = nxtchr;		/* will move to nxtchr */
	    for (from = curend ; from >= curchr; )
	      {
		*(buff+to--) = *(buff+from--);
	      }
	    curend=nxtchr++;	/* end is now at curchr, bump nxtchr */
	    *(buff+curchr)=chr;	/* stick in the current character */
	    ++lenins;
	    if (tvlins < tvhardlines - 10)
	      {
		tvelin();
		ctrlch(chr);
		ctrlch('+');
	      }
	    else
	        tvtyln(curchr);	/* retype rest of the line */
	    ++curchr;		/* reset the curchr pointer */
	    xf = findx();
	    tvxy(xf,tvy);	/* reset the cursor */
	  }

/* the character has been inserted and displayed, get another maybe */

	if (how)
	    if (!grptch(&chr))
		goto l9999;
      }
    while (how && chr != ESC);	/* end of do */

    if (tvlins < tvhardlines - 10)		/* fix for slow baud */
      {
	tvelin();
        tvtyln(curchr);	/* retype rest of the line */
	xf = findx();
	tvxy(xf,tvy);	/* reset the cursor */
      }

    oldlen = lenins;
    savlen = (-1);		/* haven't saved lines */
    goto l1000;

l9998:
    tverrb("Can't insert that char ");
l9999:
    csrcmd();
    return FALSE;
l1000:

    if (ins_msg)
        fixend();
    csrcmd();
    return TRUE;
 }
/* ------------------------ tvx_1.c ------------------------------------- */
