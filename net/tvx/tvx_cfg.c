/* ------------------------------- tvx_cfg.c ------------------------ */
#include "tvx_defs.ic"

#define BL remark("")

    char synofr[20] =	/* from table */
      {' ',13,']',000,000,000,000,000,000,000,00,00,00,00,00,00,00,00,00,00};
    char synoto[20] =		/* translate to table */
      {'r','d','{',00,000,0,000,00,00,000,00,00,00,00,00,00,00,00,00,00};
    char funkey = 0;		/* leading char for function key */
    char funchar[50] =	/* code sent by function key */
      {
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
      };
    char funcmd[50] =	/* equivalent command */
      {
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
	000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
      };

/* define standard command set */

    char lexsym[LEXVALUES+1] = { E0, 'r', 'l', 'd', 'u',
      4, 21, 8, 'k', 'i', 11, '"', '\'', ',', '.', 'f', 6, 'p',
      'b', 'e', 2, 'v', 'o', '/', 's', 'g', 23, 25, 24, ';', '&',
      'm', ':', '=', 7, 9, '{', 'a', 16, '?', 'h', 20, 'c', 'j', 't',
      'n', 14, 15, 5, 18, '#', '*', '(', ')' };

    char newlex[LEXVALUES+1] ;

    char autoin, dsplin, scroll, xcases, warplm, wildch, funesc;
    char rp[80];

    FILE *f, *fopen();

  main()
  {

    FAST int i, val;

    cls();
    remark("TVX define a configuration file -- Version 11/12/85");
    BL;
    for (;;)
      {
	prompt("Enter name of file to save configuration in: ");
	reply(rp,79);
	if ((f = fopen(rp,FILEWRITE)) == 0)
	    continue;
	else
	    break;
      }


    remark("Standard commands settable by this program:");

    BL;
remark("Commands (n => count allowed):");
remark("nA Append lines  B Buff begin   ^B File begin   nC Change chrs  nD Down");
remark("n^D Down column  E Buff end     nF Find         ^F Find-file     G Get save buf");
remark("^G Unkill       nH Half page    nI Insert        J Jump back    nK Kill ch");
remark("n^K Kill line   nL Left          M Mem stat     nN Note loc     n^N Reset loc");
remark("nO Open line    nP Page         ^P Print screen ^Q Quit         nR Right");
remark("nS Save lines   ^S Read file    nT Tidy         ^T Abort        nU Up");
remark("n^U Up column    V Verify      n^W Write buff   nBS Del ch      n; Re-Find");
remark(" / Delete last   = Change last   ' Del ln beg     \" Del ln end   , Ln beg");
remark(" . Line end     nTAB Word rt    n{ Word left    n& Rpt agn");
remark("? Help           * Ins find pat n:p Set param p ^O Op. System");
remark("n^E Edit rptbuf  ^R Restore rpt n#k Execute rpt n k times");
remark("Note: <> repeat, @ command file, and ESCAPE cannot be changed.");
    BL;

    for (;;)
      {
	BL;
	prompt("Use standard command definitions? (y/n) ");
	lreply(rp,10);
	if (*rp == 'y')
	  {
	    for (i=0 ; i <= LEXVALUES ; ++i)
		fputc(lexsym[i],f);		/* write to file */
	    goto LEXDONE;
	  }
	else if (*rp == 'n')
	    break;
      }

CAGAIN:
    cls();
    remark("You now must re-define all 47 commands.  You may simply enter");
    remark("the key of the command (followed by a RETURN), or the decimal");
    remark("code of the key, followed by a RETURN.");
    BL;
/* define new commands */
    for (i=0 ; i <= LEXVALUES ; ++i)
	newlex[i] = 0;		/* clear  */
    set(1,"cursor right (r)");
    set(2,"cursor left (l)");
    set(3,"cursor down (d)");
    set(4,"cursor up (u)");
    set(5,"cursor down in col (^d)");
    set(6,"cursor up in col (^u)");
    set(7,"del prev char (backspace)");
    set(8,"kill character (k)");
    set(9,"enter insert mode (i)");
    set(10,"kill a line (^k)");
    set(11,"delete to end of line (\")");
    set(12,"delete to beginning of line (')");
    set(13,"cursor to beginning of line (,)");
    set(14,"cursor to end of line (.)");
    set(15,"find (f)");
    set(16,"find across buffer (^f)");
    set(17,"scroll screen one page (p)");
    set(18,"beginning of buffer (b)");
    set(19,"end of buffer (e)");
    set(20,"beginning of file (^b)");
    set(21,"verify screen (v)");
    set(22,"open line (o)");
    set(23,"delete last thing (/)");
    set(24,"save line in save buff (s)");
    set(25,"get save buffer (g)");
    set(26,"write buffer (^w)");
    set(27,"save external file in save buffer (^y)");
    set(28,"exit (^x)");
    set(29,"find again (;)");
    set(30,"execute repeat buffer (&)");
    set(31,"memory status line (m)");
    set(32,"set parameter (:)");
    set(33,"delete last thing, enter insert (=)");
    set(34,"get back last killed line (^g)");
    set(35,"word rigth (tab)");
    set(36,"word left ({)");
    set(37,"append line to save buffer (a)");
    set(38,"print screen (^p)");
    set(39,"help (?)");
    set(40,"scroll page half screen (h)");
    set(41,"abort edit session (^t)");
    set(42,"change characters (c)");
    set(43,"jump back (j)");
    set(44,"tidy (fill to margin) (t)");
    set(45,"note position (n)");
    set(46,"return to noted position (^n)");
    set(47,"'push' to operating system (^O)");
    set(48,"Edit repeat buffer (^E)");
    set(49,"Restore repeat buffer (^R)");
    set(50,"Execute repeat buffer k (#)");
    set(51,"Insert last find match (*)");

    cls();
remark("Commands have been defined. You can start over if you made any mistakes.");
    remark("");
    prompt("Are they ok? (y/n) ");
    lreply(rp,10);
    if (*rp == 'n')
	goto CAGAIN;
    
    for (i=0 ; i <= LEXVALUES ; ++i)
      {
	fputc(newlex[i],f);		/* write to file */
	lexsym[i] = newlex[i];
      }

LEXDONE:
	syno();
	funkeys();

    cls();
    prompt("Use autoindent (n default) (y/n): ");
    lreply(rp,10);
    if (*rp == 'y')
	fputc(1,f);
    else 
	fputc(0,f);

    BL;
    prompt("Home display line: (1-66, 16 default): ");
    rdint(&val);
    if (val > 66 || val <= 0)
	fputc(16,f);
    else
    	fputc(val,f);

    BL;
    prompt("Scroll window (0 default): ");
    rdint(&val);
    if (val > 24)
	val = 0;
    fputc(val,f);

    BL;
    prompt("Find case (e=exact,a=any, any default): "); 
    lreply(rp,10);
    if (*rp == 'e')
        fputc(1,f);
    else
        fputc(0,f);

    BL;

    prompt("Auto line wrap width (0 default): ");
    rdint(&val);
    if (val > 79)
	val = 0;
    fputc(val,f);

    BL;
    prompt("Use wild cards (y default) (y/n)? ");
    lreply(rp,10);
    if (*rp == 'n')
	fputc(0,f);
    else 
	fputc(1,f);

    BL;
    prompt("Use BACKUP.LOG file (n default) (y/n)? ");
    lreply(rp,10);
    if (*rp == 'y')
        fputc(1,f);
    else
        fputc(0,f);
    
#ifdef MSDOS
    BL;
    remark("The editor can recognize Ctrl-z as EOF, or it can ignore ^Z and");
    remark("just use the standard MS-DOS end of file mark.");
    prompt("Should the editor recognize Ctrl-Z as EOF? (y/n) ");
    lreply(rp,10);
    if (*rp == 'y')
        fputc(1,f);
    else
        fputc(0,f);
#endif

    cls();
    remark("Configuration file created.");
    fclose(f);
  }

/* ===============================>>> FUNKEYS <<<========================*/
  funkeys()
  {

    FAST int j,i,val;
    SLOW int fun;

FAGAIN:
    cls();
    remark("You may now define up to 49 function keys to be translated to");
    remark("commands OR letters.  This translation will take place before");
    remark("the editor gets the character at any level  -- thus the translation");
    remark("will apply equally to command mode and insert mode.  The translation");
    remark("assumes each function key generates a 2 character sequence.  The");
    remark("first character is an 'escape' character that must be the same for");
    remark("each key.  If the 'escape' character is really ESC, then you must");
    remark("also define one function key to have ESC as its translation value.");
    remark("When asked for the function key, simply press the key, followed by");
    remark("RETURN.  Enter either the character or decimal value of the translation.");

    for (i = 0 ; i < 50 ; ++i)
      {
	funchar[i] = funcmd[i] = 0;
      }

    BL;
    prompt("Do you want to define any function keys? (y/n) ");
    lreply(rp,10);
    if (*rp == 'n')
	goto WTFUN;

    BL;
    remark("Now, please press ANY function key so the program can identify");
    prompt("the 'function key escape' code (followed by a RETURN): ");
    reply(rp,10);
    funesc = *rp;		/* this should be the escape char */
    if (funesc == 27)
      {
	BL;
	remark("IMPORTANT:  You MUST define a function key to send an ESCAPE (decimal 27).");
	remark("If you don't, then you won't be able to end insert mode or repeat loops.");
	remark("The program doesn't have logic to make sure you do this, so don't forget!");
	BL;
      }

    for (i = 0 ; i < 50 ; ++i)
      {
FUNAGAIN:
	prompt("Press function key to define (RETURN only to exit): ");
 	rp[1] = 0;
	reply(rp,10);
	fun = rp[1];
	if (rp[1] == 0)
	    break;
	for (j = 0 ; j < 50 ; ++j)
	  {
	    if (fun == funchar[j])
	      {
		remark("That's been used already, try again.");
		goto FUNAGAIN;
	      }
	  }
	funchar[i] = fun;
	prompt("Now enter the character/command it gets translated to: ");
	reply(rp,10);
	val = getval(rp);
	funcmd[i] = val;
      }

    cls();
remark("Functions have been defined. You can start over if you made any mistakes.");
    remark("");
    prompt("Are they ok? (y/n) ");
    lreply(rp,10);
    if (*rp == 'n')
	goto FAGAIN;

WTFUN:
    for (i = 0 ; i < 50 ; ++i)
      {
	fputc(funchar[i],f);
      }
    for (i = 0 ; i < 50 ; ++i)
      {
	fputc(funcmd[i],f);
      }
    fputc(funesc,f);
  }
  

/* ===============================>>> GETVAL <<<========================*/
  getval(str)
  char *str;
  {
    /* return one byte value */

    if (*str >= '0' && *str <= '9')
	return (atoi(str));
    else
	return (*str & 0377);
  }

/* ===============================>>> SET <<<========================*/
  set(indx,msg)
  int indx;
  char *msg;
  {
	/* set newlex[indx] to a new value */

    SLOW int val,i;

SAGAIN:
    prompt("Enter new command for "); prompt(msg); prompt(": ");
    reply(rp,10);
    val = getval(rp);

    if (val == 0)
      {
	remark("Invalid value, try again");
	goto SAGAIN;
      }
    for (i = 1 ; i <= LEXVALUES ; ++i)
      {
	if (val == newlex[i])
	  {
	    remark("That value has been already used.  Try again.");
	    goto SAGAIN;
	  }
      }
    newlex[indx] = val;	/* save value */
    
  }

/* ===============================>>> SYNO <<<========================*/
  syno()
  {

    FAST int j, i, valfrom, valto, found;
    
SAGAIN:
    cls();
    remark("You may now define up to 19 synonyms.  For example, you might");
    remark("want to define a space to be a synonym for right, or RETURN");
    remark("the same as down.  You must use unused values, however.  You");
    remark("can't use a existing command as a synonym.  You may enter the");
    remark("character followed by a RETURN, or the decimal value of the key.");

    for (i = 0 ; i < 20 ; ++i)
      {
	synofr[i] = synoto[i] = 0;
      }

    for (i = 0 ; i < 19 ; ++i)
      {
SYNAGAIN:
	BL;
	prompt("Enter the new synonym (RETURN when done): ");
	reply(rp,10);
	valfrom = getval(rp);
	if (valfrom == 0)
	    break;
	for (j = 1 ; j <= LEXVALUES ; ++j)
	  {
	    if (lexsym[j] == valfrom)
	      {
		remark("That is already a command! Try again.");
		goto SYNAGAIN;
	      }
	  }
	prompt("Enter the equivalent command: ");
	reply(rp,10);
	valto = getval(rp);
	for (j = 1, found = FALSE ; j <= LEXVALUES ; ++j)
	  {
	    if (lexsym[j] == valto)
	        found = TRUE;
	  }
	if (!found)
	  {
	    remark("That is not a defined command. Try again.");
	    goto SYNAGAIN;
	  }

	synofr[i] = valfrom;
	synoto[i] = valto;
      }
    cls();

remark("Synonyms have been defined. You can start over if you made any mistakes.");
    remark("");
    prompt("Are they ok? (y/n) ");
    lreply(rp,10);
    if (*rp == 'n')
	goto SAGAIN;

    for (i = 0 ; i < 20 ; ++i)
        fputc(synofr[i],f);
    for (i = 0 ; i < 20 ; ++i)
        fputc(synoto[i],f);

  }

/* ===============================>>> CLS <<<========================*/
  cls()
  {
    FAST int i;
    for (i = 0 ; i < 25 ; ++i)
	remark("");
  }

#define EXTENDED	/* my own extended lib functions */
/* #define STANDARD	/* the set of standard functions i use */
#define LOCAL static	/* make all local globals, i think */

#ifdef EXTENDED
/*=============================>>> CLOWER  <<<================================*/
  char clower(ch)
  char ch;
  {
    return ((ch >='A' && ch<='Z') ? ch + ' ' : ch);
  }

/*=============================>>> CUPPER  <<<================================*/
  char cupper(ch)
  char ch;
  {
    return ((ch >= 'a' && ch <= 'z') ? ch - ' ' : ch);
  }

/* =========================>>> LOWER  <<<==============================*/
  lower(str)
  char str[];
  {
    FAST int i;

    for (i=0 ; str[i] ; ++i)
	str[i]=clower(str[i]);

  }

/*=============================>>> PROMPT <<<================================*/
  prompt(msg)
  char msg[];
  {
    printf("%s",msg);
  }


/*=============================>>> REMARK <<<================================*/
  remark(msg)
  char msg[];
  {
    printf("%s\n",msg);
  }

/*=============================>>> UPPER  <<<================================*/
  upper(str)
  char str[];
  {
    static int i;

    for (i=0 ; str[i] ; ++i)
	str[i]=cupper(str[i]);
  }


/*=============================>>> LREPLY <<<================================*/
  lreply(msg,maxc)
  char msg[];
  int maxc;
  {
    reply(msg,maxc);
    lower(msg);
  }

/*=============================>>> UREPLY <<<================================*/
  ureply(msg,maxc)
  char msg[];
  int maxc;
  {
    reply(msg,maxc);
    upper(msg);
  }

/*=============================>>> REPLY <<<================================*/
  reply(msg,maxc)
  char msg[];
  int maxc;
  {
#ifdef UNIX
    gets(msg);
#else
    ms_reply(msg,maxc);
#endif
  }

/*=============================>>> RDINT <<<================================*/
  rdint(val)
  int *val;
  {
    char chrrep[12];
    reply(chrrep,11);
    *val = atoi(chrrep);
  }
#endif
#ifdef MSDOS
/*=============================>>> MS_REPLY <<<================================*/
  ms_reply(msg,maxc)
  char msg[];
  int maxc;
  {
#define CBS 8		/* Backspace */
#define CDL1 21		/* ^U */
#define CDL2 24		/* ^X */
#define CABORT 3	/* ^C */
#define CRET 13		/* cr */
#define BACKSPACE 8

    static char ch, rp;
    static int i;


    for (i = 0 ; i < maxc ; )	/* i -> next char */
      {
	ch = bdos(7,-1) & 0377; 	/* read the character */
	if (ch == CBS)		/* back space */
	  {
	    if (i > 0)		/* must be something to delete */
	      {
		--i;		/* wipe out char */
		bdos(2,BACKSPACE); bdos(2,' '); bdos(2,BACKSPACE);
		if (msg[i] < ' ')	/* double echo ^ chrs */
		  {
		    bdos(2,BACKSPACE); bdos(2,' '); bdos(2,BACKSPACE);
		  }
	      }
	  }
#ifdef USE_WIPE
	else if (ch == CDL1 || ch == CDL2)	/* wipe whole line */
	  {
	    i = 0;		/* set for loop ++ */
	    remark("#");
	    prompt("Re-enter? ");
	  }
#endif
	else if (ch == CABORT)
	  {
	    remark("^C");
	    prompt("Exit to operating system - are you sure? (y/n) ");
	    rp = bdos(7,-1) & 0377;
	    if (rp == 'y' || rp =='Y')
	     {
		remark("y");
		exit(0);
	     }
	    remark("n");
	    msg[i] = 0;
	    prompt("Re-enter? "); prompt(msg);		/* re-echo */
	  }
	else if (ch == CRET)		/* ret, so done */
	  {
	    remark("");
	    msg[i] = 0;
	    return;
	  }
	else
	  {
	    msg[i++] = ch;
	    msg[i] = 0;			/* always 0 terminate */
	    if (ch < ' ')
	      {
		ch += '@';
		bdos(2,'^');
	      }
	    bdos(2,ch);			/* echo char */
	  }
      } /* end for */

    remark("");
  }

#endif
/* ------------------------------- tvx_cfg.c ------------------------ */
