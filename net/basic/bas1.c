/*
 * BASIC by Phil Cockcroft
 */
/*
 *      This file contains the main routines of the interpreter.
 */


/*
 *      the core is arranged as follows: -
 * -------------------------------------------------------------------  - - -
 * | file    |  text   |  string | user  | array |  simple    |  for/ | unused
 * | buffers |   of    |  space  | def   | space |  variables | gosub | memory
 * |         | program |         | fns   |       |            | stack |
 * -------------------------------------------------------------------  - - -
 * ^         ^         ^         ^       ^       ^            ^       ^
 * filestart fendcore  ecore     estring edefns  earray       vend    vvend
 *                        ^eostring           ^estarr
 */

#define         PART1
#include        "bas.h"
#undef          PART1

/*
 *      The main program , it sets up all the files, signals,terminal
 *      and pointers and prints the start up message.
 *      It then calls setexit().
 * IMPORTANT NOTE:-
 *              setexit() sets up a point of return for a function
 *      It saves the local environment of the calling routine
 *      and uses that environment for further use.
 *              The function reset() uses the information saved in
 *      setexit() to perform a non-local goto , e.g. poping the stack
 *      until it looks as though it is a return from setexit()
 *      The program then continues as if it has just executed setexit()
 *      This facility is used all over the program as a way of getting
 *      out of functions and returning to command mode.
 *      The one exception to this is during error trapping , The error
 *      routine must pop the stack so that there is not a recursive call
 *      on execute() but if it does then it looks like we are back in
 *      command mode. The flag ertrap is used to signal that we want to
 *      go straight on to execute() the error trapping code. The pointers
 *      must be set up before the execution of the reset() , (see error ).
 *              N.B. reset() NEVER returns , so error() NEVER returns.
 */

main(argc,argv)
char    **argv;
{
	register i;
	catchsignal();
	startfp();              /* start up the floating point hardware */
	setupfiles(argc,argv);
	setupterm();            /* set up files after processing files */
	ecore = fendcore+sizeof(xlinnumb);
	( (lpoint) fendcore )->linnumb=0;
	clear(DEFAULTSTRING);
	prints("Phil's Basic version v1.8\n");
	setexit();
	if(ertrap)
		goto execut;
	docont();
	runmode=0;              /* say we are in immeadiate mode */
	if(cursor)              /* put cursor on a blank line */
		prints(nl);
	prints("Ready\n");
	do{
		do{
			trapped=0;
			*line ='>';
			edit(1,1,1);
		}while( trapped || ( !(i=compile(1)) && !linenumber ));
		if(linenumber)
			insert(i);
	}while(linenumber);
	if(inserted){
		inserted=0;
		clear(DEFAULTSTRING);
		closeall();
	}
	vvend=bstk;             /* reset the gosub stack */
	errortrap=0;            /* disable error traps */
	intrap=0;               /* say we are not in the error trap */
	trapped=0;              /* say we haven't got a cntrl-c */
	cursor=0;               /* cursor is at start of line */
	elsecount=0;            /* disallow elses as terminators */
	curline=0;              /* current line is zero */
	point=nline;            /* start executing at start of input line */
	stocurlin=0;           /* start of current line is null- see 'next' */
execut: execute();              /* execute the line */
	return(-1);             /* see note below */
}

/*
 *      Execute will return by calling reset and so if execute returns then
 *    there is a catastrophic error and we should exit with -1 or something
 */

/*
 *      compile converts the input line (in line[]) into tokenised
 *    form for execution(in nline). If the line starts with a linenumber
 *    then that is converted to binary and is stored in 'linenumber' N.B.
 *    not curline (see evalu() ). A linenumber of zero is assumed to
 *    be non existant and so the line is executed immeadiately.
 *      The parameter to compile() is an index into line that is to be
 *    ignored, e.g. the prompt.
 */

compile(fl)
int     fl;
{
	register char   *p,*q;
	register struct tabl    *l;
	unsigned lin=0;
	char    charac;
	char    *eql(),*k;
	p= &line[fl];
	q=nline;
	while(*p++ ==' ');
	p--;
	while(isnumber(*p)){                    /* get line number */
		if(lin >= 6553)
			error(7);
		lin = lin*10 + (*p++ -'0');
	}
	while(*p==' ')
		*q++ = *p++;
	if(!*p){
		linenumber =lin;
		return(0);      /* no characters on the line */
	}
	while(*p){
		if(*p=='"' || *p=='`'){         /* quoted strings */
			charac= *p;
			*q++ = *p++;
			while(*p && *p != charac)
				*q++ = *p++;
			if(*p)
				*q++= *p++;
			continue;
		}
		if(*p < '<' && *p != '\''){     /* ignore all characters */
			*q++ = *p++;            /* that couldn't be used */
			continue;               /* in reserved words */
		}
		for(l=table ; l->string ; l++)  /* search the table */
			if(*p != *(l->string) ) /* for the right entry */
				continue;
			else if(k = eql(p,l->string)){  /* if found then */
#ifdef  LKEYWORDS
				if( isletter(*p) ){
					if(p!= &line[fl] && isletter(*(p-1)) )
						continue;
					if( isletter(*k) && l->chval != FN)
						continue;
				}
#endif
				*q++ = l->chval;    /* replace by a token */
				p = k;
				if(l->chval== REM || l->chval== QUOTE ||
							l->chval == DATA)
					while(*p)
						*q++ = *p++;
				goto more;      /* dont compile comments */
			}                       /* or data */
		*q++ = *p++;
	more:   ;
	}
	*q='\0';
	linenumber=lin;
	return(q-nline);                /* return length of line */
}

/*
 *      eql() returns true if the strings are the same .
 *    this routine is only called if the first letters are the same.
 *    hence the increment of the pointers , we don't need to compare
 *    the characters they point to.
 *      To increase speed this routine could be put into machine code
 *    the overheads on the function call and return are excessive
 *    for what it accomplishes. (it fails most of the time , and
 *    it can take a long time to load a large program ).
 */

char    *
eql(p,q)
register char   *p,*q;
{
	p++,q++;
	while(*q)
		if(*p++ != *q++){
#ifdef  SCOMMS
			if(*(p-1) == '.')
				return(p);
#endif
			return(0);
		}
	return(p);
}

/*
 *      Puts a line in the table of lines then sets a flag (inserted) so that
 *    the variables are cleared , since it is very likely to have moved
 *    'ecore' and so the variables will all be corrupted. The clearing
 *    of the variables is not done in this routine since it is only needed
 *    to clear the variables once and that is best accomplished in main
 *    just before it executes the immeadiate mode line.
 *      If the line existed before this routine is called then it is deleted
 *    and then space is made available for the new line, which is then
 *    inserted.
 *      The structure of a line in memory has the following structure:-
 *              struct olin{
 *                      unsigned linnumb;
 *                      unsigned llen;
 *                      char     lin[1];
 *                      }
 *      The linenumber of the line is stored in linnumb , If this is zero
 *    then this is the end of the program (all searches of the line table
 *    terminate if it finds the linenumber is zero.
 *      The variable 'llen' is used to store the length of the line (in
 *    characters including the above structure and any padding needed to
 *    make the line an even length.
 *      To search through the table of lines then:-
 *              start at 'fendcore'
 *              IF linnumb is zero THEN terminate search
 *                ELSE IF linnumb is the required line THEN
 *                      found line , terminate
 *                ELSE
 *                      goto next line ( add llen to the current pointer )
 *                      repeat loop.
 *      The line is in fact stored in lin[] , To the C compiler this
 *    is a one character array but since the lines are more than one
 *    character long (usually) it is fooled into using it as a variable
 *    length array ( impossible in 'pure' C ).
 *      The pointers used by the program storage routines are:-
 *              fendcore = start of text storage segment
 *              ecore = end of text storage
 *                    = start of data segment (string space ).
 *    strings are stored after the text but before the numeric variables
 *    only 512 bytes are allocated at the start of the program for strings
 *    but clear can be called to get more core for the strings.
 */

insert(lsize)
register int    lsize;
{
	register lpoint p;
	register unsigned l;
	inserted=1;                  /* say we want the variables cleared */
	l= linenumber;
	for(p= (lpoint) fendcore ; p->linnumb; p=(lpoint)((memp)p+lenv(p)))
		if(p->linnumb >= l ){
			if(p->linnumb != l )
				break;
			l=lenv(p);      /* delete the old line */
			bmov( (short *)p, (int)l);
			ecore -= l;
			break;
		}
	if(!lsize)                      /* line has no length */
		return;
	lsize += sizeof(struct olin);
#ifdef  ALIGN4
	lsize = (lsize + 03) & ~03;
#else
	if(lsize&01)
		lsize++;                /* make length of line even */
#endif
	mtest(ecore+lsize);             /* get the core for it */
	ecore += lsize;
	bmovu( (short *)p,lsize);       /* make space for the line */
	strcpy(nline,p->lin);           /* move the line into the space */
	p->linnumb=linenumber;          /* give it a linenumber */
	p->llen=lsize;                  /* give it its offset */
}

/*      This routine will move the core image down so deleteing a line */

bmov(a,b)
register short  *a;
int     b;
{
	register short  *c,*d;
	c= (short *)ecore;
	d= (short *)((char *)a  + b );
	do{
		*a++ = *d++;
	}while(d<c);
}

/*      This will move the text image up so that a new line can be inserted */

bmovu(a,b)
register short  *a;
int     b;
{
	register short  *c,*d;
	c= (short *) ecore;
	d= (short *) (ecore-b);
	do{
		*--c = *--d;
	}while(a<d);
}

/*
 *      The interpreter needs three variables to control the flow of the
 *    the program. These are:-
 *              stocurlin : This is the pointer to the start of the current
 *                          line it is used to index the next line.
 *                          If the program is in immeadiate mode then
 *                          this variable is NULL (very important for 'next')
 *              point:      This points to the current location that
 *                          we are executing.
 *              curline:    The current line number ( zero in immeadiate mode)
 *                          this is not needed for program exection ,
 *                          but is used in error etc. It could be made faster
 *                          if this variable is not used....
 */

/*
 *      The main loop of the execution of a program.
 *      It does the following:-
 *              FOR(ever){
 *                      save point so that resume will go to the right place
 *                      IF cntrl-c THEN stop
 *                      IF NOT a reserved word THEN do_assignment
 *                              ELSE IF legal command THEN execute_command
 *                      IF return is NORMAL THEN
 *                              BEGIN
 *                                  IF terminator is ':' THEN continue
 *                                  ELSE IF terminator is '\0' THEN
 *                                         goto next line ; continue
 *                                  ELSE IF terminator is 'ELSE' AND
 *                                              'ELSES' are enabled THEN
 *                                                  goto next line ; continue
 *                              END
 *                      ELSE IF return is < NORMAL THEN continue
 *                                      ( used by goto etc. ).
 *                      ELSE IF return is > NORMAL THEN
 *                           ignore_rest_of_line ; goto next line ; continue
 *                      }
 *      All commands return a value ( if they return ). This value is NORMAL
 *    if the command is standard and does not change the flow of the program.
 *    If the value is greater than zero then the command wants to miss the
 *    rest of the line ( comments and data ).
 *      If the value is less than zero then the program flow has changed
 *    and so we should go back and try to execute the new command ( we are
 *    now at the start of a command ).
 */

execute()
{
	register int    i,c;
	register lpoint p;

	ertrap=0;                       /* stop recursive error trapping */
again:
	savepoint=point;
	if(trapped)
		dobreak();
	if(!((c=getch())&0200)){
		point--;
		assign();
		goto retn;
	}
	if(c>=MAXCOMMAND)
		error(8);
	if((i=(*commandf[c&0177])())==NORMAL){  /* execute the command */
retn:           if((c=getch())==':')
			goto again;
		else if(!c){
elseret:                if(!runmode)            /* end of immeadiate line */
				reset();
			p = stocurlin;
			p = (lpoint)((memp)p + lenv(p)); /* goto next line */
			stocurlin=p;
			point=p->lin;
			if(!(curline=p->linnumb)) /* end of program */
				reset();
			elsecount=0;               /* disable `else`s */
			goto again;
		}
		else  if(c==ELSE && elsecount)  /* `else` is a terminator */
				goto elseret;
		error(SYNTAX);
	}
	if(i < NORMAL)
		goto again;     /* changed execution position */
	else
		goto elseret;   /* ignore rest of line */
}

/*
 *      The error routine , this is called whenever there is any error
 *    it does some tidying up of file descriptors and sets the error line
 *    number and the error code. If there is error trapping ( errortrap is
 *    non-zero and in runmode ), then save the old pointers and set up the
 *    new pointers for the error trap routine.
 *    Otherwise print out the error message and the current line if in
 *    runmode.
 *      Finally call reset() ( which DOES NOT return ) to pop
 *    the stack and to return to the main routine.
 */

error(i)
int     i;                      /* error code */
{
	register lpoint p;
	if(readfile){                   /* close file descriptor */
		close(readfile);        /* from loading a file */
		readfile=0;
	}
	if(pipes[0]){                   /* close the pipe (from chain ) */
		close(pipes[0]);        /* if an error while chaining */
		pipes[0]=0;
	}
	evallock=0;                     /* stop the recursive eval message */
	ecode=i;                        /* set up the error code */
	if(runmode)
		elinnumb=curline;       /* set up the error line number */
	else
		elinnumb=0;
	if(runmode && errortrap && !inserted ){ /* we have error trapping */
		estocurlin=stocurlin;   /* save the various pointers */
		epoint=savepoint;
		eelsecount=elsecount;
		p=errortrap;
		stocurlin=p;            /* set up to execute code */
		point=p->lin;
		curline=p->linnumb;
		saveertrap=p;           /* save errortrap pointer */
		errortrap=0;            /* disable further error traps */
		intrap=1;               /* say we are trapped */
		ertrap=1;               /* we want to go to execute */
	}
	else  {                         /* no error trapping */
		if(cursor){
			prints(nl);
			cursor=0;
		}
		prints(ermesg[i-1]);            /* error message */
		if(runmode){
			prints(" on line ");
			prints(printlin(curline));
		}
		prints(nl);
	}
	reset();                /* no return - goes to main */
}

/*
 *      This is executed by the ON ERROR construct it checks to see
 *    that we are not executing an error trap then set up the error
 *    trap pointer.
 */

errtrap()
{
	register lpoint p;
	p=getline();
	check();
	if(intrap)
		error(8);
	errortrap=p;
}

/*
 *      The 'resume' command , checks to see that we are actually
 *    executing an error trap. If there is an optional linenumber then
 *    we resume from there else we resume from where the error was.
 */

resume()
{
	register lpoint p;
	register unsigned i;
	if(!intrap)
		error(8);
	i= getlin();
	check();
	if(i!= (unsigned)(-1) ){
		for(p=(lpoint)fendcore;p->linnumb;p=(lpoint)((memp)p+lenv(p)))
			if(p->linnumb==i)
				goto got;
		error(6);               /* undefined line */
got:            stocurlin= p;                   /* resume at that line */
		curline= p->linnumb;
		point= p->lin;
		elsecount=0;
	}
	else  {
		stocurlin=estocurlin;          /* resume where we left off */
		curline=elinnumb;
		point=epoint;
		elsecount=eelsecount;
	}
	errortrap=saveertrap;                   /* restore error trapping */
	intrap=0;                               /* get out of the trap */
	return(-1);                             /* return to re-execute */
}

/*
 *      The 'error' command , this calls the error routine ( used in testing
 *    an error trapping routine.
 */

doerror()
{
	register i;
	i=evalint();
	check();
	if(i<1 || i >MAXERR)
		error(22);      /* illegal error code */
	error(i);
}

/*
 *      This routine is used to clear space for strings and to reset all
 *    other pointers so that it effectively clears the variables.
 */

clear(stringsize)
int     stringsize;     /* size of string space */
{
#ifdef  LNAMES
	register struct entry   **p;
	register int    *ip;

	for(p = hshtab ; p < &hshtab[HSHTABSIZ];)    /* clear the hash table*/
		*p++ = 0;
	for(ip = varshash ; ip < &varshash[HSHTABSIZ]; )
		*ip++ = -1;
#endif
#ifdef  ALIGN4
	estring= &ecore[stringsize& ~03];       /* allocate string space */
#else
	estring= &ecore[stringsize& ~01];       /* allocate string space */
#endif
	mtest(estring);                         /* get the core */
	shash=1;                                /* string array "counter" */
	datapoint=0;                           /* reset the pointer to data */
	contpos=0;
#ifdef  LNAMES
	chained = 0;                            /* reset chained flag */
	estdt=enames=edefns=earray=vend=bstk=vvend=estarr=estring;
#else
	estdt=edefns=earray=vend=bstk=vvend=estarr=estring;
#endif
			/* reset variable pointers */
	eostring=ecore;                         /* string pointer */
	srand(0);                               /* reset the random number */
}                                               /* generator */

/*
 *      mtest() is used to set the amount of core for the current program
 *    it uses brk() to ask the system for more core.
 *      The core is allocated in 1K chunks, this is so that the program does
 *    not spend most of is time asking the system for more core and at the
 *    same time does not hog more core than is neccasary ( be friendly to
 *    the system ).
 *      Any test that is less than 'ecore' is though of as an error and
 *    so is any test greater than the size that seven memory management
 *    registers can handle.
 *      If there is this error then a test is done to see if 'ecore' can
 *    be accomodated. If so then that size is allocated and error() is called
 *    otherwise print a message and exit the interpreter.
 *      If the value of the call is less than 'ecore' we have a problem
 *    with the interpreter and we should cry for help. (It doesn't ).
 */

mtest(l)
memp    l;
{
	register memp   m;
	static   memp   maxmem;                 /* pointer to top of memory */

#ifdef  ALIGN4
	if( (int)l & 03){
		prints("Illegal allignment\n");
		quit();
	}
#endif
	m = (memp)(((int)l+MEMINC)&~MEMINC);    /* round the size up */
	if(m==maxmem)                           /* if allocated then return */
		return;
	if(m < ecore || m > MAXMEM || brk(m) == -1){ /* problems*/
		m= (memp) (((int)ecore +DEFAULTSTRING+MEMINC )&~MEMINC);
		if(m <= MAXMEM && brk(m)!= -1){
			maxmem= m;              /* oh, safe */
			clear(DEFAULTSTRING);   /* zap all pointers */
			error(24);              /* call error */
		}
		prints("out of core\n");        /* print message */
		quit();                         /* exit flushing buffers */
	}
	maxmem=m;                               /* set new limit */
}

/*
 *      This routine is called to test to see if there is enough space
 *    for an array. The result is true if there is no space left.
 */

nospace(l)
long    l;
{
#ifndef pdp11
	if(l< 0 || vvend+l >= MAXMEM)
#else
	if(l< 0 || l >65535L || (long)vvend+l >= 0160000L)
#endif
		return(1);
	return(0);      /* we have space */
}

/*
 *      This routine is called by the routines that define variables
 *    to increase the amount of space that is allocated between the
 *    two end pointers of that 'type'. It uses the fact that all the
 *    variable pointers are in a certain order (see bas.h ). It
 *    increments the relevent pointers and then moves up the rest of
 *    the data to a new position. It also clears the area that it
 *    has just allocated and then returns a pointer to the space.
 */

memp xpand(start,size)
register memp   *start;
int     size;
{
	register short  *p,*q;
	short   *bottom;
	bottom = (short *) (*start);
	p= (short *)vvend;
	do{
		*start++ += size;
	}while( start <= &vvend);
	mtest(vvend);
	start= (memp *)bottom;
	q= (short *)vvend;
	do{
		*--q = *--p;
	}while(p > (short *)start);
	do{
		*--q=0;
	}while(q > (short *)start);
	return( (memp) start);
}

/*
 *      This routine tries to set up the system to catch all the signals that
 *    can be produced. (except kill ). and do something sensible if it
 *    gets one. ( There is no way of producing a core image through the
 *    sending of signals).
 */

#ifdef  V6
#define _exit   exit
#endif

catchsignal()
{
	extern  _exit(),quit1(),catchfp();
#ifdef  SIGTSTP
	extern  onstop();
#endif
	register int    i;
	static  int     (*traps[NSIG])()={
		quit,           /* hang up */
		trap,           /* cntrl-c */
		quit1,          /* cntrl-\ */
		_exit,
		_exit,
		_exit,
		_exit,
		catchfp,        /* fp exception */
		0,              /* kill    */
		seger,          /* seg err */
		mcore,          /* bus err */
		0,
		_exit,
		_exit,
		_exit,
		_exit,
		_exit,
		};

	for(i=1;i<NSIG;i++)
		signal(i,traps[i-1]);
#ifdef  SIGTSTP
	signal(SIGTSTP,onstop);         /* the stop signal */
#endif
}

/*
 *      this routine deals with floating exceptions via fpfunc
 *    this is a function pointer set up in fpstart so that trapping
 *    can be done for floating point exceptions.
 */

catchfp()
{
	extern  (*fpfunc)();

	signal(SIGFPE,catchfp); /* restart catching */
	if(fpfunc== 0)          /* this is set up in fpstart() */
		_exit(1);
	(*fpfunc)();
}

/*
 *      we have a segmentation violation and so should print the message and
 *    exit. Either a kill() from another process or an interpreter bug.
 */

seger()
{
	prints("segmentation violation\n");
	_exit(-1);
}

/*
 *      This does the same for bus errors as seger() does for segmentation
 *    violations. The interpreter is pretty nieve about the execution
 *    of complex expressions and should really check the stack every time,
 *    to see if there is space left. This is an easy error to fix, but
 *    it was not though worthwhile at the moment. If it runs out of stack
 *    space then there is a vain attempt to call mcore() that fails and
 *    so which produces another bus error and a core image.
 */

mcore()
{
	prints("bus error\n");
	_exit(-1);
}

/*
 *      Called by the cntrl-c signal (number 2 ). It sets 'trapped' to
 *    signify that there has been a cntrl-c and then re-enables the trap.
 *      It also bleeps at you.
 */

trap()
{
	signal(SIGINT, SIG_IGN);/* ignore signal for the bleep */
	write(1, "\07", 1);     /* bleep */
	signal(SIGINT, trap);   /* re-enable the trap */
	trapped=1;              /* say we have had a cntrl-c */
#ifdef  BSD42
	if(ecalling){
		ecalling = 0;
		longjmp(ecall, 1);
	}
#endif
}

/*
 *      called by cntrl-\ trap , It prints the message and then exits
 *    via quit() so flushing the buffers, and getting the terminal back
 *    in a sensible mode.
 */

quit1()
{
	signal(SIGQUIT,SIG_IGN);/* ignore any more */
	if(cursor){             /* put cursor on a new line */
		prints(nl);
		cursor=0;
	}
	prints("quit\n\r");     /* print the message */
	quit();                 /* exit */
}

/*
 *      resets the terminal , flushes all files then exits
 *    this is the standard route exit from the interpreter. The seger()
 *    and mcore() traps should not go through these traps since it could
 *    be the access to the files that is causing the error and so this
 *    would produce a core image.
 *      From this it may be gleened that I don't like core images.
 */

quit()
{
	flushall();                     /* flush the files */
	rset_term(1);
	if(cursor)
		prints(nl);
	exit(0);                       /* goodbye */
}

docont()
{
	if(runmode){
		contpos=0;
		if(cancont){
			bstk= vvend;
			contpos=cancont;
		}
		else
			bstk= vend;
	}
	cancont=0;
}

#ifdef  SIGTSTP
/*
 * support added for job control
 */
onstop()
{
	flushall();                     /* flush the files */
	if(cursor){
		prints(nl);
		cursor = 0;
	}
#ifdef  BSD42
	sigsetmask(0);                  /* Urgh !!!!!! */
#endif
	signal(SIGTSTP, SIG_DFL);
	kill(0,SIGTSTP);
	/* The PC stops here */
	signal(SIGTSTP,onstop);
}
#endif
