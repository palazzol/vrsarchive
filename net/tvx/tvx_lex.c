/* -------------------------- tvx_lex.c ----------------------------------- */
#include "tvx_defs.ic"

/* -------------------------- GLOBALS GO HERE -------------------------------*/
#define EXTERN

#include "tvx_glbl.ic"

   char clower(),cupper();

/* =============================>>> CHECKOS <<<============================= */
  checkos()
  {
	/* check if ok operating system */
#ifdef MSDOS
    if ((bdos(0x30,0) & 0xff) < 2)		/* !!! cii-86 dependent */
      {
	remark("TVX requires MS-DOS 2.0 or later");
	exit();
      }
#endif
  }

/* =============================>>> STARTM <<<============================= */
  startm()
  {
    prompt("TVX - Full Screen Editor ");
    prompt(VERSION); prompt("Terminal: ");
    remark(cversn);
    remark("");
    remark("? for help");
    remark("");
  }

/* =============================>>> MEMORY <<<============================= */
  memory()
  { /* memory - print memory left */
 
    SLOW int nxt,chrsavail;
    SLOW unsigned int tmp;

    char value[10],msg[85],*cp,stemp[80];
 
    nxt = 0;			/* where message goes */
 
    stcopy(" Chrs left:",0,stemp,&nxt);	/* add ' Chars: ' */

#ifdef LASL
    tmp = max(nxtsav - nxtchr - BUFFLIMIT,0);
#else
    tmp = nxtsav - nxtchr;
#endif

#ifdef INT16
    if (tmp > 30000)		/* handle "neg" size */
      {
	stemp[nxt++] = '+';
	tmp -= 30000;
      }
#endif

    itoa(tmp,value);
    stcopy(value,0,stemp,&nxt);	/* the value */
 
    stcopy(" Last line:",0,stemp,&nxt);	/* add ' Lines: ' */
    itoa(nxtlin-1,value);
    stcopy(value,0,stemp,&nxt);	/* add the count */
 
#ifdef STATCURLINE
    stcopy(" Cur line:",0,stemp,&nxt); /* add ' Cur line: ' */
    itoa(curlin,value);
#endif
#ifdef STATREPEAT
    stcopy(" Rpt:",0,stemp,&nxt); /* add ' Rpt: ' */
    itoa(rptuse+1,value);
    stcopy(value,0,stemp,&nxt);	/* the count */
 
    stcopy("/",0,stemp,&nxt); /* add '/' */
    itoa(REPEATBUFS,value);
#endif
    stcopy(value,0,stemp,&nxt);
    chrsavail=80-strlen(stemp);
    nxt=0;			/* where message goes */
    cp = (*dest_file ? dest_file : orig_file);
    stcopy(cp,max(strlen(cp)-chrsavail,0),msg,&nxt);	/* the file name */
    if (strlen(VERSION) <= chrsavail-nxt)
     {
       stcopy(VERSION,0,msg,&nxt); /* TVX */
         if (strlen(cversn) <= chrsavail-nxt)
           stcopy(cversn,0,msg,&nxt);  /* terminal type */
     }
       stcopy(stemp,0,msg,&nxt);   /* rest of string */
 
    tverr(msg); 	/* display line on screen */
 }
 
/* =============================>>> SHOSET  <<<============================= */
  shoset()
  {  /* show repeat buffer, help if available */

#ifdef HELP
    static char rp[2];
    FAST int i;
    SLOW char *cp, *msg;
    SLOW int fields, oldtty, hnum;
    SLOW unsigned tmp;

#ifdef FULLHELP
    struct help_msg 
      {
	char *hmsg;
	char Vmsg;
      };

    static struct help_msg cmddes[] =	/* messages for help */
      {
	{"nApnd to sv buff",	VSAPPEND},
	{" Buffer beg     ", 	 VTOP},
	{" File beg",		 VFBEGIN},
	{"nChange chars   ",	VCHANGE},
	{"nDown line      ",	 VDOWNLINE},
	{"nDown column",	 VDOWNCOL},
	{" Buffer end     ",	VBOTTOM},
	{"nEdit rpt buffer",	 VEDITRPT},
	{"nFind",		 VSEARCH},
	{" Find cross-buff",	VNEXT},
	{" Get save buffer",	 VGET},
	{" Unkill lastline",	 VUNKILL},
	{"nHalf page      ",	VHALFP},
	{"nInsert (to ESC)",	 VINSERT},
	{" Jump back",		 VJUMP},
	{"nKill character ",	VDELNEXT},
	{"nKill line      ",	 VKILLALL},
	{"nLeft",		 VLEFT},
	{" Memory status  ",	VMEMORY},
	{"nNote location  ",	 VNOTELOC},
	{"nReset loc",		 VRETNOTE},
	{"nOpen line      ",	VOPENLINE},
	{" Call Opr system",	 VSYSTEM},
	{"nPage",		 VFLIP},
	{" Print screen   ",	VPRINTS},
	{"nRight          ",    VRIGHT},
	{" Restore rpt buf",	 VSTORERPT},
	{"nSave lines     ",	VSAVE},
	{"nTidy, fill text",	 VTIDY},
	{" Abort",		 VABORT},
	{"nUp             ",	VUPLINE},
	{"nUp column      ",	 VUPCOL},
	{" Verify screen",	 VVERIFY},
	{"nWrite buffer   ",	VWPAGE},
	{" Exit, end edit ",	 VQUIT},
	{"nYank file",		 VYANK},
	{"nDel prev char  ",	VDELLAST},
	{"nFind again     ",	 VSAGAIN},
	{" Del last",		 VREMOVE},
	{" Change last    ",	VRMVINS},
	{" Del to line beg",	 VKILLPREV},
	{" Del to line end",	 VKILLREST},
	{" Line begining  ",	VBEGLINE},
	{" Line end       ",	 VENDLINE},
	{"nWord right",		 VMVWORD},
	{"nWord left      ",	VMVBWORD},
	{"nRepeat again   ",	 VXREPEAT},
	{"nk Exec rpt k n times", VEXECRPT},
	{"n p Set param p ",	VSETPARS},
	{" Help           ",	 VHELP},
	{" Insert find pat",	 VINSPAT},
	{"/",0}			/* last variable entry */
      };
#endif

    oldtty = ttymode;
    ttymode = FALSE;
    if (!oldtty)
	tvclr();

    prompt("Parameter : cur val (1=y, 0=n)    Prev 16 cmds:");
    for (hnum = 0,i = old_cindex ; hnum < 16 ; ++hnum)
      {
	shocout(old_cmds[i]);
	i = ++i % 16;
      }

    remark("");
    prompt("A-Autoindent: "); wtint(autoin);
    prompt("  T-TTY mode: "); wtint(ttymode);
    prompt("  E-Expand tabs: "); wtint(tabspc); remark("");

    prompt("F-Find: ignore case: "); 
	if (xcases)
	    tvcout('0');
	else
	    tvcout('1');

    prompt("    M-Match wild cards: "); wtint(use_wild); remark("");
    prompt("U-User wild card set: "); remark(user_set);

    prompt("D disp line:"); wtint(dsplin); 
    prompt("  S-Scroll window:"); wtint(scroll);
    prompt("  V-Virtual window:"); wtint(tvlins); 
	prompt("/"); wtint(tvhardlines);
    prompt("  W-Wrap width:"); wtint(wraplm); remark("");
    prompt("O-Output file: "); prompt(dest_file);
    prompt("  (input file is: ");prompt(orig_file); remark(")");
    remark("");

    prompt("Find: |");
    for (cp = sbuff ; *cp ; ++cp)
      {
	shocout(*cp);
      }
    remark("|");
    remark("");
    prompt("Max chars: ");
    tmp = mxbuff;
#ifdef INT16
    if (tmp > 30000)		/* handle "neg" size */
      {
	prompt("30000+");
	tmp -= 30000;
      }
#endif
    wtint(tmp); prompt("  Max lines: ");    wtint(mxline);
    prompt("  Cur line: "); wtint(curlin);
    if (usecz)
	prompt("    ^Z for EOF");
    remark("");
    remark("");

    prompt("R-Repeat buffer: ");wtint(rptuse+1);
    remark("   All repeat buffers : <contents>:");
    for (i = 0 ; i < REPEATBUFS ; ++ i)
      {
	fields = 5;
	shocout('#') ; shocout(i+'1') ; prompt(": <");
	for (cp = &rptbuf[i][0] ; *cp ; ++cp)
	  {
	    shocout(*cp);
	    ++fields;		/* how many letters */
	    if (*cp < ' ')
		++fields;
	    if (fields >= (tvcols - 6))
	      {
		prompt("+more+");
		break;
	      }
	  }
	remark("");
      }

    ttymode = oldtty;
    memory();

#ifdef FULLHELP
    tvxy(1,22);

    ask("Press space to exit, anything else for command list ",rp,1);

    if (*rp == ' ')
      {
	ttymode = oldtty;
	verify(1);
	return;
      }

    if (!oldtty)
	tvclr();

    remark("Commands (n => count allowed):");
    for (hnum = fields = 0  ; ; ++hnum )
      {
	prompt("   ");
	cp = cmddes[hnum].hmsg;
	if (*cp == '/')	/* end of variable list */
	    break;
	else
	    shocout(*cp);	/* show n or blank */
	msg = ++cp;		/* where message is, skipping 'n' field */
	while (*cp)		/* ship to lex value */
	    ++cp;
	i = cmddes[hnum].Vmsg;		/* get the lexical index */
	shocout(cupper(lexsym[i]));	/* show the command */
	if (lexsym[i] >= ' ')
	    shocout(' ');	/* skip space for no '^' */

	shocout(' ');		/* space to separate */
	prompt(msg);		/* and show the message */
	if (++fields == 3)	/* bump fields, see if need newline */
	  {
	    fields = 0;
	    remark("");
	  }
      }
    remark("");
    remark("   n<>$$ Rpt loop        @  Invoke cmd file   $  Escape");
    remark("Wild cards:");
remark("^A-alphanumeric  ^D-digit  ^L-letter  ^O-other,(not-^A)  ^P-punctuation");
remark("^X-any character ^U-user set  -- ^W-word of ^..  ^N-not in word of ^..");

#endif

    if (!oldtty)
	tvxy(1,24);
    ask("Press any key to resume ",rp,1);

    ttymode = oldtty;

    verify(1);
#else
    tverr(&rptbuf[rptuse][0]);
#endif
  }

/* =============================>>> SHOCOUT <<<============================= */
  shocout(c)
  char c;
  {

    if (c < ' ')
      {
	ttwt('^'); ttwt(c + '@');
      }
    else
	ttwt(c);
  }

/* =============================>>> LEXGET <<<============================= */
  lexget(chr)
  char *chr;
  {  /* lexget - get a character for lex, possibly from repeat buffer */

    SLOW char tmp;
l10:
    if (rptcnt[rptuse] > 0)	/* in a repeat buffer? */
      {
	*chr=rptbuf[rptuse][nxtrpt[rptuse]];	/* pick up from repeat buffer */
	if (*chr == SLOOPEND) 	/* at end of rpt buff */
	  {
	    nxtrpt[rptuse] = 0;	/* start <> loop over */
	    if (--rptcnt[rptuse] == 0 && !echof)	/* all done with loop */
	      {
		echof = TRUE;	/* turn on echo again */
		newscr();	/* update screen after repeat */
	      }
	    goto l10;		/* loop again */
	  }
	++nxtrpt[rptuse];	/* bump to next char in loop */
      }
    else			/* not in loop, get from keyboard */
      {
	gkbd(&tmp);	/* picks up one character from the keyboard */
	*chr = old_cmds[old_cindex] = tmp;
	old_cindex = ++old_cindex % 16;
      }
  }

/* =============================>>> LEX    <<<============================= */
  lex(lexval,lexcnt,lexchr,parsok)
  int *lexval,*lexcnt,parsok;
  char *lexchr;
  { /* ##  lex - gets command input from terminal, and scans for
       #  its lexical value.  Returns a count if given.  Also handles
       #  repeat loops. */
 
    SLOW int count, lex_default;
    FAST int i;
    SLOW int neg, newln;

    static char chr,cmdchr,tchr;


    lex_default = TRUE;

    if (!parsok)		/* abort if error in <> */
      {
	if (rptcnt[rptuse] > 0)	/* in loop? */
	  {
	    newscr();	/* clear screen, send message */
	    tverrb("<> not complete ");
	  }
	rptcnt[rptuse]=0;	/* abort loop if going */
	nxtrpt[rptuse]=0;
      }
l10:
    for (;;) 
      { 			/* need this loop to support <> */
	count = 1;		/* default count is 1 */

	lexget(&chr);		/* fetch a character */
	if (rptcnt[rptuse]>0 && chr==SLOOPBEG)	/* detect nesting */
	  {
	    nxtrpt[rptuse] = 0 ; rptcnt[rptuse] = 0 ; echof=TRUE;
	    newscr();	/* update anything so far */
	    tverrb("?No nesting ");
	    continue;
	  }
 
	if ((chr>='0' && chr<='9') || chr=='-')	/* a number */
	  {
	    count = 0;  lex_default = FALSE;
	    neg = FALSE;	/* handle negative counts */
	    if (chr=='-')
		neg=TRUE;
	    else 
		count = chr-'0';	/* convert to int value */
	    for (;;) 
	      {
		if (rptcnt[rptuse] > 0)	/* have to handle rptbuf special */
		  {
		    chr=rptbuf[rptuse][nxtrpt[rptuse]];
		    ++nxtrpt[rptuse];
		  }
		else 
		    lexget(&chr);
		if (chr>='0' && chr<='9')	/* another number? */
		    count = count*10+chr-'0';
		else			/* done with number */
		    break;
	      }
	    if (neg)			/* fix if it was negative */
		count = min(-count ,-1);
	  }
	cmdchr = clower(chr);	/* fold to one case */
	if (cmdchr == SLOOPBEG)	/* starting a loop? */
	  {
	    lex_default = TRUE;			/* don't let lex count be def */
	    rptcnt[rptuse] = (count < 0) ? (-count) : count;	/* save count */
	    ins_mode = TRUE;			/* so ttymode works */
	    tvmsg("repeat[",FALSE);		/* echo 'repeat[k]: n<' */
	    wtint(rptuse+1); prompt("]: ");
	    wtint(rptcnt[rptuse]);

	    tvcout(SLOOPBEG);
	    nxtrpt[rptuse]=0;	/* begin inserting at beginning */
	    newln = FALSE;	/* no new line echo yet */
	    do			/* fetch repeat chars until get > */
	      {
		gkbd(&chr);	/* fetch a char */
		if (chr==delkey)	/* allow editing */
		  {
		    if (nxtrpt[rptuse] > 0)	/* don't go past start */
		      {
			--nxtrpt[rptuse];	/* wipe out last */
			if ((tchr = rptbuf[rptuse][nxtrpt[rptuse]])==CR)
			  {
			    tvcout(CR);	/* going to newline */
#ifdef USELF
			    tvcout(LF);
#endif
			    newln = TRUE;		/* new line now */
			  }
			else if (!newln)
			  {
			    tvcout(BACKSPACE);	/* back over character */
			    tvcout(' ');
			    tvcout(BACKSPACE);
			    if (tchr < ' ' && tchr != 27)
			      {
				tvcout(BACKSPACE);	/* back over char */
				tvcout(' ');
				tvcout(BACKSPACE);
			      }
			  }
			else		/* have passed new line start */
			  {
			    ctrlch(rptbuf[rptuse][nxtrpt[rptuse]]);
			    tvcout('\\');
			  }
		      }
		    else
			tvcout(BELL);	/* trying to rubout too much */
		    continue;
		  }
		else		/* a control character detected */
		    ctrlch(chr);	/* echo */
 
		rptbuf[rptuse][nxtrpt[rptuse]]=chr;	/* stuff in current rpt buff. */
		++nxtrpt[rptuse];	/* bump count */
		if (nxtrpt[rptuse] >= 100)	/* only allow 100 chars! */
		  {
		    newscr();
		    tverrb("100 chars only");
		    nxtrpt[rptuse]=0 ; rptcnt[rptuse]=0;
		    ins_mode = FALSE;
		    goto l10;	/* bail out */
		  }
	      }
	    while (!( chr==ESC && rptbuf[rptuse][nxtrpt[rptuse]-2]==ESC &&
	      rptbuf[rptuse][nxtrpt[rptuse]-3]==SLOOPEND));	/* end do loop */

	    ins_mode = FALSE;		/* get ttymode right */

	    if (rptcnt[rptuse] > 1 || newln)	/* positive count? */
		echof = FALSE;	/* turn off echoing */
	    else		/* 0 count */
	      {
		fixend();
		tvhdln();	/* get back where we were */
	      }

	    rptbuf[rptuse][nxtrpt[rptuse]-2]=0;
	    lstrpt[rptuse]=nxtrpt[rptuse]-3;	/* bump back to end of buffer */
	    nxtrpt[rptuse]=0;	/* back for scan now */
	    continue;		/* now execute the loop */
	  }
#ifdef VB
	else if (cmdchr == '@')	/*$$$	indirect files! */
	  {
	    opnatf();
	    continue;
	  }
#endif
	for (i=0 ; synofr[i]!=0 ; ++i)
	  if (synofr[i]==cmdchr)
	     cmdchr=synoto[i];		/* allow synonyms */
 
	*lexval = UNKNOWN;	/* assume unknown command */
	for (i = 1 ; i<= LEXVALUES ; ++i)	/* scan all possible cmds */
	    if (cmdchr == lexsym[i])	/* found it */
	      {
		*lexval = i;
		break;
	      }
	*lexcnt = count;		/* return good stuff */
	*lexchr = chr;
	return (lex_default);		/* let know if gave back default */
      }					/* end of for(;;) */
  }
/* ------------------------ tvx_lex.c --------------------------------- */
