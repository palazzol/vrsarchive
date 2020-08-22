/* -------------------------- tvx_unix.c ------------------------------ */
#include "tvx_defs.ic"
#include "tvx_glbl.ic"

#define TEMPEXT ".$$1"		/* temporary file */
#define BACKEXT ".B"		/* backup file */
#define SWITCH '-'
#define FILESEP '.'

/* define USETMP if you want intermediate workfiles built on
   /tmp.  Otherwise, they will be built on the same directory as
   the original file.  This latter method is often a bit faster,
   especially when exiting if /tmp is on a different volume than
   the destination file (which makes mv COPY the file rather than
   just renameing. */

/* #define USETMP */			/* define if create temp files on /tmp */

#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>


/* --------------  terminal I/O stuff --------------- */

static struct sgttyb sgb;
static struct tchars tch;
static struct ltchars ltc;

#define Ioctl ioctl
#define Read read
#define Write write

/* ------------- file mode stuff ---------------------- */
#include <sys/stat.h>
  static struct stat info;		/* structure to get info */

/* ------------- misc stuff ---------------------- */

  extern int errno;
  extern char **environ;


#ifdef TERMCAP			/* routines needed for termcap */
/* ------------- termcap stuff ---------------------- */
  char PC;
  char *BC;
  char *UP;
  char TERM[40];
  short ospeed;

  static char Tcm[80];		/* special entry for cm */
  static char empty[2];
  static char Tbc[20];
  static char Tup[20];

  static int	Tco,			/* number of columns per line */
		Tli;			/* number of lines */

  static char tcbuff[1024];		/* buffer to hold termcap entry */
 

/* ==========================>>> gettermcap  <<<========================= */
  gettermcap()
  {
    char *tp;
    char *getenv();
    char entry[80];		/* scratch buffer for entry */

    empty[0] = 0;

    ospeed = sgb.sg_ospeed;	/* get the speed */

    if ((tp = getenv("TERM")) == NULL)
      {
	goto FORCETTY;
      }
    strcpy(TERM,tp);		/* copy to our TERM */

    if (tgetent(tcbuff,TERM) < 1)
      {
	goto FORCETTY;
      }

/*	read required termcap entries, save in appropriate TVX arrays */

    if (!gettcap("cm",Tcm))
      {
	goto FORCETTY;
      }

    if (!gettcap("ce",entry))
      {
	goto FORCETTY;
      }
    if (!capcpy(celin,entry,7))	/* copy entry to end of line */
      {
	goto FORCETTY;
      }
    
    gettcap("cd",entry);		/* clear to end of display */
    capcpy(cescr,entry,7);

    gettcap("al",entry);		/* insert a line (add line) */
    capcpy(ciline,entry,7);

    gettcap("dl",entry);	/* delete a line */
    capcpy(ckline,entry,7);

    if (!gettcap("sr",entry))	/* reverse scroll */
      {
	strcpy(ctopb,ciline);	/* add line works the same */
      }
    else
	capcpy(ctopb,entry,7);

    gettcap("ve",entry);		/* stand cursor changer end */
    capcpy(ccsrcm,entry,7);
    gettcap("vs",entry);		/* stand cursor changer begin */
    capcpy(ccsrin,entry,7);

    gettcap("se",entry);		/* stand out end */
    capcpy(cbolde,entry,7);

    gettcap("so",entry);		/* begin standout */
    capcpy(cboldb,entry,7);

    cerred[0] = 7;				/* bell for sure */
    gettcap("vb",entry);		/* visual bell? */
    if (*entry)
	capcpy(cerred,entry,7);

    if (!capcpy(&cversn[1],TERM,10))		/* copy name to version */
	strcpy(cversn,"TERMCAP");

    if ((Tco = tgetnum("co")) < 0)	/* # of cols */
	Tco = 79;		/* assume 80 x 24 */
    if ((Tli = tgetnum("li")) < 0)	/* # of lines */
	Tli = 24;		/* assume 80 x 24 */

    tvhardlines = tvlins = Tli;	/* number of lines */
    tvcols = Tco - 1; /* set col val (-1 avoids all the line wrap crap )*/
    if (tvhardlines != 24 || tvhardlines != 25)	/* strange terminal */
      {
	ddline = (tvlins / 2) + 1;
	setdscrl();		/* calculate scroll */
      }

    gettcap("bc",entry);		/* get backspace character */
    if (!*entry)
      {
	Tbc[0] = 8; Tbc[1] = 0;
      }
    else
	capcpy(Tbc,entry,19);
    BC = Tbc;
    gettcap("up",entry);		/* get backspace character */
    if (!*entry)
      {
	Tup[0] = 0;
      }
    else
	capcpy(Tup,entry,19);
    UP = Tup;
    gettcap("pc",entry);		/* get the pad character */
    PC = *entry;

    gettcap("is",entry);		/* initialization string */
    tcapcs(entry);			/* send the intialization string */

    gettcap("ti",entry);		/* cm initialization string */
    tcapcs(entry);			/* send the intialization string */

    return;

FORCETTY:
   force_tty = TRUE;
   remark("Unable to set up for video terminal, tty mode assumed.");
   strcpy(cversn,"tty");
  }

/* =============================>>> capcpy  <<<============================= */
  capcpy(to,from,len)
  char *to, *from;
  int len;
  {		/* copy a capability, checking length */
    if (strlen(from) > len)
      {
	*to = 0;
	return (FALSE);
      }
    else
	strcpy(to,from);
    return (TRUE);
  }

/* =============================>>> gettcap  <<<============================= */
  gettcap(cap,area)
  char *cap, *area;
  {
    char **cpp, *cp;

    cpp = &cp;		/* I think */
    cp = area;
    *area = 0;		/* assume null entry */

    tgetstr(cap,cpp);	/* get the capability */
    return (*area);		/* return 1st char */
	
  }

/* =============================>>> tcapcs  <<<============================= */
  tcapcs(str)
  char *str;
  {
 	/* send a termcap generated control string to terminal */

    register char *cp;
    int ttwt();

    if (!(echof && !bakflg && !ttymode))
	return;
    if (!*str)		/* don't send null strings */
	return;
    cp = str;
    tputs(cp,1,ttwt);

  }

/* =============================>>> tcapxy  <<<============================= */
  tcapxy(x,y)
  int x,y;
  {
	/* move cursor to x,y */

   char *tgoto();

   tcapcs(tgoto(Tcm,x-1,y-1));	/* send the string, adjusting x,y */

  }
#endif   /* termcap */


/* =============================>>> ttinit  <<<============================= */
  ttinit()
  {
    struct sgttyb nsgb;
    struct tchars ntch;
    struct ltchars nltc;

    (void) Ioctl(0, TIOCGETP, &sgb);
    (void) Ioctl(0, TIOCGETP, &nsgb);
    (void) Ioctl(0, TIOCGETC, &tch);
    (void) Ioctl(0, TIOCGETC, &ntch);
    (void) Ioctl(0, TIOCGLTC, &ltc);

    nsgb.sg_flags |= CBREAK;
    nsgb.sg_flags &= ~(CRMOD|ECHO|LCASE|TANDEM);

    ntch.t_intrc = -1;  /* interrupt */
    ntch.t_quitc = -1;  /* quit */

/* the following two lines control flow control */

#ifndef FLOWCONTROL
    ntch.t_startc = -1; /* start output */
    ntch.t_stopc = -1;  /* stop output */
#endif

    ntch.t_eofc = -1;   /* end-of-file */
    ntch.t_brkc = -1;   /* input delimiter (like nl) */

    nltc.t_suspc = -1;  /* stop process signal */
    nltc.t_dsuspc = -1; /* delayed stop process signal */
    nltc.t_rprntc = -1; /* reprint line */
    nltc.t_flushc = -1; /* flush output (toggles) */
    nltc.t_werasc = -1; /* word erase */
    nltc.t_lnextc = -1; /* literal next character */

    (void) Ioctl(0, TIOCSETP, &nsgb);
    (void) Ioctl(0, TIOCSETC, &ntch);
    (void) Ioctl(0, TIOCSLTC, &nltc);

#ifdef TERMCAP
    gettermcap();			/* set up terminal characteristics */
#endif

    info.st_mode = -1;			/* no mode stuff yet */
 }


/* =============================>>> ttrd_unix  <<<============================= */
  ttrd_unix()
  {
           char c;

    Read(0, &c, 1);
    return(c);
  }

/* =============================>>> ttwtln  <<<============================= */
  ttwtln(cbuf,cnt)
  char *cbuf;
  int cnt;
  {
    if (echof && !bakflg && !ttymode)
	Write(1, cbuf, cnt);
  }

/* =============================>>> ttwt  <<<============================= */
  ttwt(c)
  char c;
  {
    if (ttymode)
	return;
    Write(1, &c, 1);
  }

/* =============================>>> ttclos  <<<============================= */
  ttclos()
  {

#ifdef TERMCAP
    char entry[80];

    gettcap("te",entry);		/* cm end up string */
    tcapcs(entry);			/* send it */

#endif
    (void) Ioctl(0, TIOCSETP, &sgb);
    (void) Ioctl(0, TIOCSETC, &tch);
    (void) Ioctl(0, TIOCSLTC, &ltc);
  }

/* =============================>>> ttosinit  <<<============================= */
  ttosinit()
  {			/* need a special version for not doing termcap */
    struct sgttyb nsgb;
    struct tchars ntch;
    struct ltchars nltc;
    char entry[80];		/* scratch buffer for entry */

    (void) Ioctl(0, TIOCGETP, &sgb);
    (void) Ioctl(0, TIOCGETP, &nsgb);
    (void) Ioctl(0, TIOCGETC, &tch);
    (void) Ioctl(0, TIOCGETC, &ntch);
    (void) Ioctl(0, TIOCGLTC, &ltc);

    nsgb.sg_flags |= CBREAK;
    nsgb.sg_flags &= ~(CRMOD|ECHO|LCASE|TANDEM);

    ntch.t_intrc = -1;  /* interrupt */
    ntch.t_quitc = -1;  /* quit */

/* the following two lines control flow control */

#ifndef FLOWCONTROL
    ntch.t_startc = -1; /* start output */
    ntch.t_stopc = -1;  /* stop output */
#endif

    ntch.t_eofc = -1;   /* end-of-file */
    ntch.t_brkc = -1;   /* input delimiter (like nl) */

    nltc.t_suspc = -1;  /* stop process signal */
    nltc.t_dsuspc = -1; /* delayed stop process signal */
    nltc.t_rprntc = -1; /* reprint line */
    nltc.t_flushc = -1; /* flush output (toggles) */
    nltc.t_werasc = -1; /* word erase */
    nltc.t_lnextc = -1; /* literal next character */

    (void) Ioctl(0, TIOCSETP, &nsgb);
    (void) Ioctl(0, TIOCSETC, &ntch);
    (void) Ioctl(0, TIOCSLTC, &nltc);

#ifdef TERMCAP
    gettcap("is",entry);		/* initialization string */
    tcapcs(entry);			/* send the intialization string */

    gettcap("ti",entry);		/* cm initialization string */
    tcapcs(entry);			/* send the intialization string */
#endif
 }

/* ==========================>>> unix_sys  <<<============================= */
  unix_sys()
  {
    char rp[150];
    int oldtty;

    tvclr();			/* clear the screen */
    oldtty = ttymode; ttymode = FALSE;
DO_UNIX:
    remark("Unix command interface"); remark("");
    remark("Enter Unix command line: ");
    reply(rp,149);
    reset();		/* reset terminal to unix mode */

    system(rp);

    ttosinit();		/* reset terinal to our mode */

    remark("");
    remark("");

    prompt("Any key to continue with edit (! for another Unix command): ");
    reply(rp,1);

    trmini();		/* this has to be here or screen is cleared! */
    if (*rp == '!')
	goto DO_UNIX;

    ttymode = oldtty;
    verify(1);
  }

/* =============================>>> get_mode <<<============================= */
  get_mode(f)
  FILE *f;
  {		/* gets access mode of open file f */

    char rp[10];

    info.st_mode = -1;	/* assume no mode */

    if (newfil)
	return;
    if (fstat(fileno(f),&info) != 0)
      {
	info.st_mode = -1;	/* assume no mode */
	return;
      }
    info.st_mode &= 07777;	/* zap extraneous stuff*/
    if (((info.st_mode & 0222) == 0) && !rdonly)  /* no write permission */
      {
	prompt("No write permission for file, edit R/O? (y/n) ");
	ureply(rp,1);
	if (*rp == 'Y')
	    rdonly = TRUE;
	else
	  {
	    reset();
	    exit(999);
	  }
      }
  }

/* =============================>>> set_mode <<<============================= */
  set_mode(f)
  FILE *f;
  {		/* sets access mode of open file f */
    if (newfil || info.st_mode == -1)
	return;
    if (fchmod(fileno(f),info.st_mode) != 0)
	tverrb("Unable to set file mode, umask will be used.");
  }

/* ==========================>>> expand_name <<<============================ */
  expand_name(n)
  char *n;
  {		/* expands unix file names */
    char tmp[FNAMESIZE+1];

    if ((*n == '~') && (n[1] == '/'))
      {
	strcpy(tmp,getenv("HOME"));
	scopy(n,1,tmp,strlen(tmp));
	strcpy(n,tmp);
      }
  }

/* =============================>>> ren_file <<<=========================== */
  ren_file(old,new)
  char *old, *new;
  {
    int pid;
    static char *mvarg[4];
    static int status;

    if (rename(old,new) != 0)
      {
	mvarg[0] = "/bin/mv";
	mvarg[1] = old;
	mvarg[2] = new;
	mvarg[3]=0;
	pid=fork();
	if (pid == 0)
	  {
	    execve("/bin/mv",mvarg,environ);
            tverrb("Error trying to start mv utility");
	    _exit(999);
	  }
	wait(&status);
	if (status > 255)		/* error return */
	  {
	    prompt(old) ; prompt(" not renamed to "); remark(new);
	    prompt("Edited file found as: "); remark(old);
	  }
      }
  }

/* =============================>>> temp_name <<<=========================== */
  temp_name(n,first)
  char *n;
  int first;
  {
	/* generates a temporary name from n.  Depending on value of
	   first, it will either add a 1 or 2 to name */
    SLOW int i;

#ifdef USETMP
    SLOW char pidno[20];
    long pidint;

    if (first)			/* create full temp name */
      {
	*n = 0;
	pidint=getpid();
	itoa(pidint,pidno);
	strcpy(n,"/tmp/tvx1");
	scopy(pidno,0,n,9);
      }
    else			/* alternate between 1 and 2 */
      {
	if (n[8] == '1')
	    n[8] = '2';
	else
	    n[8] = '1';
      }
#else
    if (first)
      {
	if ((i = rindex(n,FILESEP)) > 0)	/* see if extenstion */
	    scopy(TEMPEXT,0,n,i);		/* make .bak */
	else
	  {
	    scopy(TEMPEXT,0,n,strlen(n));	/* just add on */
	  }
      }
    else
      {
	i = strlen(n);
	if (n[i-1] == '1')
	    n[i-1] = '2';
	else
	    n[i-1] = '1';
      }
#endif
  }

#ifndef SUN
/* =============================>>> USER_1 <<<============================= */
  user_1(knt)
  int knt;
  {
    knt = 0;
    return (TRUE);
  }

#else
/* =============================>>> USER_1 <<<============================= */
  user_1(knt)
  int knt;
  {
	/* for suns, re-initialize window */
#ifdef TERMCAP
    gettermcap();		/* set up terminal characteristics */
    tvidefs();		/* and reset defs */
#endif
    verify(1);
    return (TRUE);
  }
#endif

/* =============================>>> USER_2 <<<============================= */
  user_2(knt)
  int knt;
  {
    knt = 0;
    return (TRUE);
  }
/* -------------------------- tvx_unix.c ------------------------------ */
