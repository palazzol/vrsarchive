/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *      This file contains all the standard commands that are not placed
 *    anywhere else for any reason.
 */

/*
 *      The 'for' command , this is fairly straight forward , but
 *    the way that the variable is not allowed to be indexed is
 *    dependent on the layout of variables in core.
 *      Most of the fiddly bits of code are so that all the variables
 *    are of the right type (real / integer ). The code for putting
 *    a '1' in the step for default cases is not very good and could be
 *    improved.
 *      A variable is accessed by its displacement from 'earray'
 *    it is this index that speeds execution ( no need to search through
 *    the variables for a name ) and that enables the next routine to be
 *    so efficient.
 */

forr()
{
	register struct forst *p;
	register memp   l;
	register char   *r;
	char    vty;
	value   start;
	value   end;
	value   step;

	l=getname();
	vty=vartype;
	if(l<earray)                    /* string or array element */
		error(2);               /* variable required */
	if(getch()!='=')
		error(SYNTAX);
	r= (char *)(l - earray);        /* index */
	eval();                         /* get the from part */
	putin(&start,vty);              /* convert and move the right type */
	if(getch()!=TO)
		error(SYNTAX);
	eval();                         /* the to part */
	putin(&end,vty);
	if(getch()==STEP)
		eval();                 /* the step part */
	else {
		point--;                /* default case */
		res.i=1;
		vartype = 01;
	}
	putin(&step,vty);
	check();                                /* syntax check */
	for(p=(forstp)vvend,p--;p>=(forstp)bstk;p--) /* have we had it */
		if(p->fr && p->fnnm == r)       /* in a for loop before */
			goto got;          /* if so then reset its limits */
	p= (forstp)vvend;
	vvend += sizeof(struct forst);  /* no then allocate a */
	mtest(vvend);                   /* new structure on the stack */
	p->fnnm=r;
	p->fr= 01+vty;
got:    p->elses=elsecount;             /* set up all information for the */
	p->stolin=stocurlin;            /* next routine */
	p->pt=point;
	vartype=vty;
#ifndef V6C
	p->final = end;
	p->step = step;
	res = start;
#else
	movein(&end,&p->final);        /* move the variables to the correct */
	movein(&step,&p->step);         /* positions */
	movein(&start,&res);
#endif
#ifdef  LNAMES
	l = (int)r + earray;                    /* force it back */
#endif
	putin(l,vty);
	normret;
}

/*
 *      the 'next' command , this does not need an argument , if there is
 *    none then the most deeply nested 'next' is accessed. If there is
 *    a list of arguments then the variable name is accessed and a search
 *    is made for it. ( next_without_for error ). Then the step is added
 *    to the varable and the result is compared to the final. If the loop
 *    is not ended then the stack is set to the end of this 'for' structure
 *    and a return is executed. Otherwise the stack is popped and a return
 *    to the required line is performed.
 */


next()
{
	register struct forst *p;
	register value  *l;
	register char   *r;
	register int    c;

	c=getch();
	point--;
	if(istermin(c)){                /* no argument */
		for( p = (forstp)vvend , p-- ; p >= (forstp)bstk ; p--)
			if(p->fr){
				l =  (value *)(p->fnnm + (int) earray);
				goto got;
			}
		error(18);      /* no next */
	}
for(;;){
	l= (value *)getname();
	r= (memp)((memp)l - earray);
	for(p= (forstp)vvend , p-- ; p >= (forstp)bstk ; p--)
		if(p->fr &&p->fnnm == r)
			goto got;
	error(18);                      /* next without for */
got:    vartype=p->fr-1;
	if(vartype){
#ifndef pdp11
#ifdef  VAX_ASSEM                       /* if want to use assembler */
		l->i += p->step.i;
		asm("        bvc nov");         /* it is a lot faster.... */
		    error(35);
		asm("nov:");
#else
		register long   m = p->step.i;
		if( (m += l->i) > 32767 || m < -32768 )
			error(35);
		else l->i = m;
#endif
#else
		foreadd(p->step.i,l);
#endif
		if(p->step.i < 0){
			if( l->i >= p->final.i)
				goto nort;
			else goto rt;
		}
		else if( l->i <= p->final.i)
			goto nort;
	}
	else {
		fadd(&p->step, l );
		if(p->step.i <0){               /* bit twiddling */
#ifndef SOFTFP
			if( l->f >= p->final.f)
				goto nort;
			else goto rt;
		}
		else if( l->f <= p->final.f)
			goto nort;
#else
			if(cmp(l,&p->final)>=0 )
				goto nort;
			goto rt;
		}
		else  if(cmp(l,&p->final)<= 0)
			goto nort;
#endif
	}
rt:     vvend=(memp)p;                  /* don't loop - pop the stack */
	if(getch()==',')
		continue;
	else point--;
	break;
nort:
	if(stocurlin=p->stolin)                 /* go back to the 'for' */
		curline=stocurlin->linnumb;     /* need this for very */
	else runmode=0;                         /* obscure reasons */
	point = p->pt;
	elsecount=p->elses;
	vvend = (memp) (p+1);
	break;
	}
	normret;
}

/*
 *      The 'gosub' command , This uses the same structure as 'for' for
 *    the storage of data. A gosub is identified by the flag 'fr' in
 *    the 'for' structure being zero. This just gets the line on which
 *    we are on and sets up th structure. Gosubs from immeadiate mode
 *    are dealt with and this is one of the obscure reasons for the
 *    the comment and code in 'return' and 'next'.
 */

gosub()
{
	register struct forst   *p;
	register lpoint l;

	l=getline();
	check();
	p = (forstp) vvend;
	vvend += sizeof(struct forst);
	mtest(vvend);
	runmode=1;
	p->fr=0;
	p->fnnm=0;
	p->elses=elsecount;
	p->pt=point;
	p->stolin=stocurlin;
	stocurlin=l;
	curline=l->linnumb;
	point= l->lin;
	elsecount=0;
	return(-1);     /* return to execute the next instruction */
}

/*
 *      The 'return' command this just searches the stack for the
 *    first gosub/return it can find, pops the stack to that level
 *    and returns to the correct point. Deals with returns to
 *    immeadiate mode, as well.
 */

retn()
{
	register struct forst   *p;

	check();
	for(p= (forstp)vvend , p-- ; p >= (forstp)bstk ; p--)
		if(!p->fr && !p->fnnm)
			goto got;
	error(21);              /* return without gosub */
got:
	elsecount=p->elses;
	point=p->pt;
	if(stocurlin=p->stolin)
		curline=stocurlin->linnumb;
	else runmode=0;                 /* return to immeadiate mode */
	vvend= (memp)p;
	normret;
}

/*
 *      The 'run' command , run will execute a program by putting it in
 *    runmode and setting the start address to the start of the program
 *    or to the optional line number. It clears all the variables and
 *    closes all files.
 */

runn()
{
	register lpoint p;
	register unsigned l;

	l=getlin();
	check();
	p = (lpoint)fendcore;
	if(l== (unsigned)(-1) )
		goto got;
	else for(;p->linnumb; p = (lpoint)((memp) p + lenv(p)) )
		if(l== p->linnumb)
			goto got;
	error(6);               /* undefined line */
got:
	clear(DEFAULTSTRING);   /* zap the variables */
	closeall();
	if(!p->linnumb)                 /* no program so return */
		reset();
	curline=p->linnumb;     /* set up all the standard pointers */
	stocurlin=p;
	point=p->lin;
	elsecount=0;
	runmode=1;
	return(-1);             /* return to execute the next instruction */
}

/*
 *      The 'end' command , checks its syntax ( no parameters ) then
 *    gets out of what we were doing.
 */

endd()
{
	check();
	reset();
}

/*
 *      The 'goto' command , simply gets the required line number
 *    and sets the pointers to it. If in immeadiate mode , go into
 *    runmode and zap the stack .
 */

gotos()
{
	register lpoint p;
	p=getline();
	check();
	curline=p->linnumb;
	point=p->lin;
	stocurlin=p;
	elsecount=0;
	if(!runmode){
		runmode++;
		vvend=bstk;     /* zap the stack */
	}
	return(-1);
}

/*
 *      The 'print' command , The code for this routine is rather weird.
 *    It works ( well ) for all types of printing ( including files ),
 *    but it is a bit 'kludgy' and could be done better ( I don't know
 *    how ). Every expression must be followed by a comma a semicolon
 *    or the end of a statement. To get it all to work was tricky but it
 *    now does and that is all that can be said for it.
 *      The use of filedes assumes that an integer has the same size as
 *      a structure pointer. If this is not the case. This system will not
 *      work ( nor will most of the rest of the interpreter ).
 */

print()
{
	int     i;
	register int     c;
	extern  write(),putfile();
	static  char    spaces[]="                ";    /* 16 spaces */
	register int    (*outfunc)();   /* pointer to the output function */
	register int    *curcursor;     /* pointer to the current cursor */
					/* 'posn' if a file, or 'cursor' */
	int     Twidth;                 /* width of the screen or of the */
	filebufp filedes;               /* file. BLOCKSIZ if a file */

	c=getch();
	if(c=='#'){
		i=evalint();
		if(getch()!=',')
			error(SYNTAX);
		filedes=getf(i,_WRITE);
		outfunc= putfile;               /* see bas6.c */
		curcursor= &filedes->posn;
		Twidth = BLOCKSIZ;
		c=getch();
	}
	else {
		outfunc= write;
		curcursor= &cursor;
		filedes = (filebufp)1;
		Twidth = ter_width;
	}
	point--;

for(;;){
	if(istermin(c))
		break;
	else if(c==TABB){                       /* tabing */
		point++;
		if(*point++!='(')
			error(SYNTAX);
		i=evalint();
		if(getch()!=')')
			error(SYNTAX);
		while(i > *curcursor+16 && !trapped){
			(*outfunc)(filedes,spaces,16);
			*curcursor+=16;
		}
		if(i> *curcursor && !trapped){
			(*outfunc)(filedes,spaces,i- *curcursor);
			*curcursor = i;
		}
		*curcursor %= Twidth;
		c=getch();
		goto outtab;
	}
	else if(c==',' || c==';'){
		point++;
		goto outtab;
	}
	else if(checktype())
		stringeval(gblock);
	else {
		eval();
		gcvt();
	}
	(*outfunc)(filedes,gblock,gcursiz);
	*curcursor = (*curcursor + gcursiz) % Twidth;
	c=getch();
outtab: if(c==',' ||c==';'){
		if(c==','){
			(*outfunc)(filedes,spaces,16-(*curcursor%16));
			*curcursor=(*curcursor+(16- *curcursor%16)) % Twidth;
		}
		c=getch();
		point--;
		if(istermin(c))
			normret;
	}
	else if(istermin(c)){
		point--;
		break;
	}
	else error(SYNTAX);
	}

	(*outfunc)(filedes,nl,1);
	*curcursor=0;
	normret;
}

/*
 *      The 'if' command , no real problems here but the 'else' part
 *    could do with a bit more checking of what it's going over.
 */

iff()
{
	register int    elsees;
	register int    c;
	register char   *p;

	eval();
	if(getch()!=THEN)
		error(SYNTAX);
#ifdef  PORTABLE
	if(vartype ? res.i : res.f){
#else
	if(res.i ){                     /* naughty bit twiddleing */
#endif
		c=getch();              /* true */
		point--;
		elsecount++;            /* say `else`s are allowed */
		if(isnumber(c))         /* if it's a number then */
			gotos();        /* execute a goto */
		return(-1);             /* return to execute another ins. */
	}
	for(elsees = 0, p= point; *p ; p++) /* skip all nested 'if'-'else' */
		if(*p==(char)ELSE){         /* pairs */
			if(--elsees < 0){
				p++;
				break;
			}
		}
		else if(*p==(char)IF)
			elsees++;
	point = p;                      /* we are after the else or at */
	if(!*p)
		normret;
	while(*p++ == ' ');             /* end of line */
	p--;                            /* ignore the space after else */
	if(isnumber(*p))                /* if number then do a goto */
		gotos();
	return(-1);
}

/*
 *      The 'on' command , this deals with everything , it has to do
 *    its own searching so that undefined lines are not accessed until
 *    a 'goto' to that line is actually required.
 *    Deals with on_gosubs from immeadiate mode.
 */

onn()
{
	unsigned lnm[128];
	register unsigned *l;
	register lpoint p;
	register forstp pt;
	int     m;
	int     i;
	int     c;
	int     k;

	if(getch()==ERROR){
		if(getch()!=GOTO)
			error(SYNTAX);
		errtrap();      /* do the trapping of errors */
		normret;
	}
	else point--;
	m=evalint();
	if((k=getch())!= GOTO && k != GOSUB)
		error(SYNTAX);
	for(l=lnm,i=1;;l++,i++){        /* get the line numbers */
		if( (*l = getlin()) == (unsigned)(-1) )
			error(5);       /* line number required */
		if(getch()!=',')
			break;
	}
	point--;
	check();
	if(m<1 || m> i)                 /* index is out of bounds */
		normret;                /* so return */
	c= lnm[m-1];
	for(p = (lpoint)fendcore ; p->linnumb ;
					p = (lpoint)((memp)p + lenv(p)) )
		if(p->linnumb==c)
			goto got;
	error(6);                       /* undefined line */
got:    if(k== GOSUB) {
		pt=(forstp)vvend;               /* fix the gosub stack */
		vvend += sizeof(struct forst);
		mtest(vvend);
		pt->fnnm=0;
		pt->fr=0;
		pt->elses=elsecount;
		pt->pt=point;
		pt->stolin=stocurlin;
	}
	if(!runmode){
		runmode++;
		if(k==GOTO)             /* gotos in immeadiate mode */
			vvend=bstk;
	}
	stocurlin=p;
	curline=p->linnumb;
	point= p->lin;
	elsecount=0;
	return(-1);
}

/*
 *      The 'cls' command , neads to set the terminal into 'rare' mode
 *    so that there is no waiting on the page clearing ( form feed ).
 */

cls()
{
	extern  char    o_CLEARSCR[];

	set_term();
	puts(o_CLEARSCR);
	putch(0);       /* flush it out */
	rset_term(0);
	cursor = 0;
	normret;
}

/*
 *      The 'base' command , sets the start index for arrays to either
 *      '0' or '1' , simple.
 */

base()
{
	register int    i;
	i=evalint();
	check();
	if(i && i!=1)
		error(28);      /* bad base value */
	baseval=i;
	normret;
}

/*
 *      The 'rem' and '\'' command , ignore the rest of the line
 */

rem() {  return(GTO); }

/*
 *      The 'let' command , all the work is done in assign , the first
 *    getch() is to get the pointer in the right place for assign().
 */

lets()
{
	assign();
	normret;
}

/*
 *      The 'clear' command , clears all variables , closes all files
 *    and allocates the required amount of storage for strings,
 *    maximum is 32K.
 */

clearl()
{
	register int    i;

	i=evalint();
	check();
	if(i < 0 || i + ecore > MAXMEM)
		error(12);      /* bad core size */
	clear(i);
	closeall();
	normret;
}

/*
 *      The 'list' command , can have an optional two arguments and
 *    a dash is also used.
 *      Most of this routine is the getting of the arguments. All the
 *    actual listing is done in listl() , This routine should call write()
 *    and not clr(), but then the world is not perfect.
 */

list()
{
	register unsigned l1,l2;
	register lpoint p;
	l1=getlin();
	if(l1== (unsigned)(-1) ){
		l1=0;
		l2= -1;
		if(getch()=='-'){
			if( (l2 = getlin()) == (unsigned)(-1) )
				error(SYNTAX);
		}
		else point--;
	}
	else  {
		if(getch()!='-'){
			l2= l1;
			point--;
		}
		else
			l2 = getlin();
	}
	check();
	for(p= (lpoint)fendcore ; p->linnumb < l1 ;
					p = (lpoint)((memp)p + lenv(p)) )
		if(!p->linnumb)
			reset();
	if(l1== l2 && l1 != p->linnumb )
			reset();
	while(p->linnumb && p->linnumb <=l2 && !trapped){
		l1=listl(p);
		line[l1++] = '\n';
		write(1,line,(int)l1);
		p = (lpoint)((memp)p + lenv(p));
	}
	reset();
}

/*
 *      The routine that does the listing of a line , it searches through
 *    the table of reserved words if it find a byte with the top bit set,
 *    It should ( ha ha ) find it.
 *      This routine could run off the end of line[] since line is followed
 *    by nline[] this should not cause any problems.
 *      The result is in line[].
 */

listl(p)
lpoint p;
{
	register char   *q;
	register struct tabl *l;
	register char    *r;

	r=strcpy(printlin(p->linnumb) ,line);  /* do the linenumber */
	for(q= p->lin; *q && r < &line[MAXLIN]; q++){
		if(*q &(char)0200)              /* reserved words */
			for(l=table;l->chval;l++){
				if((char)(l->chval) == *q){
					r=strcpy(l->string,r);
					break;
				}
			}
		else if(*q<' '){                /* do special characters */
			*r++ ='\\';
			*r++ = *q+ ('a'-1);
		}
		else {
			if(*q == '\\')          /* the special character */
				*r++ = *q;
			*r++ = *q;              /* non special characters */
		}
	}
	if(r >= &line[MAXLIN])                  /* get it back a bit */
		r = &line[MAXLIN-1];
	*r=0;
	return(r-line);                 /* length of line */
}

/*
 *      The 'stop' command , prints the message that it has stopped
 *    and then exits the 'user' program.
 */

stop()
{
	check();
	dostop(0);
}

/*
 *      Called if trapped is set (by control-c ) and just calls dostop
 *    with a different parameter to print a slightly different message
 */

dobreak()
{
	dostop(1);
}

/*
 *      prints out the 'stopped' or 'breaking' message then exits.
 *    These two functions were lumped together so that it might be
 *    possible to add a 'cont'inue command at a latter date ( not
 *    implemented yet ) - ( it is now ).
 */

dostop(i)
{
	if(cursor){
		cursor=0;
		prints(nl);
	}
	prints( (i) ? "breaking" : "stopped" );
	if(runmode){
		prints(" at line ");
		prints(printlin(curline));
		if(!intrap){            /* save environment */
			cancont=i+1;
			conpoint=point;
			constolin=stocurlin;
			concurlin=curline;
			contelse=elsecount;
			conerp=errortrap;
		}
	}
	prints(nl);
	reset();
}

/*      the 'cont' command - it seems to work ?? */

cont()
{
	check();
	if( contpos && !runmode){
		point = conpoint;       /* restore environment */
		stocurlin =constolin;
		curline = concurlin;
		elsecount = contelse;
		errortrap = conerp;
		vvend= bstk;
		bstk = vend;
		mtest(vvend);           /* yeuch */
		runmode =1;
		if(contpos==1){
			contpos=0;
			normret;        /* stopped */
		}
		contpos=0;              /* ctrl-c ed */
		return(-1);
	}
	contpos=0;
	error(CANTCONT);
}

/*
 *      The 'delete' command , will only delete the required lines if it
 *    can find the two end lines. stops ' delete 1' etc. as a slip up.
 *      very slow algorithm. But who cares ??
 */

delete()
{
	register lpoint p1,p2;
	register unsigned i2;

	p1=getline();
	if(getch()!='-')
		error(SYNTAX);
	p2=getline();
	check();
	if(p1>p2)
		reset();
	i2 = p2->linnumb;
	do{
		linenumber = p1->linnumb;
		insert(0);
	}while(p1->linnumb && p1->linnumb <= i2 );
	reset();
}

/*
 *      The 'shell' command , calls the v7 shell as an entry into unix
 *    without going out of basic. Has to set the terminal in a decent
 *    mode , else 'ded' doesn't like it.
 *      Clears out all buffered file output , so that you can see what
 *    you have done so far, and sets your userid to your real-id
 *    this stops people becoming unauthorised users if basic is made
 *    setuid ( for games via runfile of the command file ).
 */

shell()
{
	register int    i;
	register int    (*q)() , (*p)();
	int     (*signal())();
	char    *s;
#ifdef  SIGTSTP
	int     (*t)();
#endif

	check();
	flushall();
#ifdef  SIGTSTP
	t = signal(SIGTSTP, SIG_DFL);
#endif
#ifdef  VFORK
	i = vfork();
#else
	i=fork();
#endif
	if(i==0){
		rset_term(1);
		setuid(getuid());               /* stop user getting clever */
#ifdef  V7
		s = getenv("SHELL");
		if(!s || !*s)
			s = "/bin/sh";
#else
		s = "/bin/sh";
#endif
		execl(s,"sh (from basic)",0);
		exit(-1);                       /* problem */
	}
	else if(i== -1)
		prints("cannot shell out\n");
	else {                                  /* daddy */
		p=signal(SIGINT,SIG_IGN);       /* ignore some signals */
		q=signal(SIGQUIT, SIG_IGN);
		while(i != wait(0) && i != -1); /* wait on the 'child' */
		signal(SIGINT,p);               /* resignal to what they */
		signal(SIGQUIT,q);              /* were before */
	}                                       /* in a mode fit for basic */
#ifdef  SIGTSTP
	signal(SIGTSTP, t);
#endif
	normret;
}

/*
 *      The 'edit' command , can only edit in immeadiate mode , and with the
 *    specified line ( maybe could be more friendly here , no real need to
 *    since the editor is the same as on line input.
 */

editl()
{
	register lpoint p;
	register int    i;

	p= getline();
	check();
	if(runmode || noedit)
		error(13);      /* illegal edit */
	i=listl(p);
	edit(0,i,0);            /* do the edit */
	if(trapped)             /* ignore it if exited via cntrl-c */
		reset();
	i=compile(0);
	if(linenumber)          /* ignore it if there is no line number */
		insert(i);
	reset();                /* return to 'ready' */
}

/*
 *      The 'auto' command , allows input of lines with automatic line
 *    numbering. Most of the code is to do with getting the arguments
 *    otherwise the loop is fairly simple. There are three ways of getting
 *    out of this routine. cntrl-c will exit the routine immeadiately
 *    If there is no linenumber then it also exits. If the line typed in is
 *    terminated by an ESCAPE character the line is inserted and the routine
 *    is terminated.
 */

dauto()
{
	register unsigned start , end , i1;
	unsigned int      i2;
	long    l;
	int     c;
	i2=autoincr;
	i1=getlin();
	if( i1 != (unsigned)(-1) ){
		if(getch()!= ','){
			point--;
			i2=autoincr;
		}
		else {
			i2=getlin();
			if(i2 == (unsigned)(-1) )
				error(SYNTAX);
		}
	}
	else
		i1=autostart;
	check();
	start=i1;
	autoincr=i2;
	end=i2;
	for(;;){
		i1= strcpy(printlin(start),line) - line;
		line[i1++]=' ';
		c=edit(0,i1,i1);
		if(trapped)
			break;
		i1=compile(0);
		if(!linenumber)
			break;
		insert(i1);
		if( (l= (long)start+end) >=65530){
			autostart=10;
			autoincr=10;
			error(6);       /* undefined line number */
		}
		start+=end;
		autostart=l;
		if(c == ESCAPE )
			break;
	}
	reset();
}

/*
 *      The 'save' command , saves a basic program on a file.
 *    It just lists the lines adds a newline then writes them out
 */

save()
{
	register lpoint p;
	register int    fp;
	register int    i;

	stringeval(gblock);     /* get the name */
	gblock[gcursiz]=0;
	check();
	if((fp=creat(gblock,0644))== -1)
		error(14);              /* cannot creat file */
	for(p= (lpoint)fendcore ; p->linnumb ;
					p = (lpoint)((memp) p + lenv(p)) ){
		i=listl(p);
		line[i++]='\n';
		write(fp,line,i);       /* could be buffered ???? */
	}
	close(fp);
	normret;
}

/*
 *      The 'old' command , loads a program from a file. The old
 *    program (if any ) is wiped.
 *      Most of the work is done in readfi, ( see also error ).
 */

old()
{
	register int    fp;

	stringeval(gblock);
	gblock[gcursiz]=0;              /* get the file name */
	check();
	if((fp=open(gblock,0))== -1)
		error(15);              /* can't open file */
	ecore= fendcore+sizeof(xlinnumb);       /* zap old program */
	( (lpoint) fendcore)->linnumb=0;
	readfi(fp);                     /* read the new file */
	reset();
}

/*
 *      The 'merge' command , similar to 'old' but does not zap the old
 *    program so the two files are 'merged' .
 */

merge()
{
	register int    fp;

	stringeval(gblock);
	gblock[gcursiz]=0;
	check();
	if((fp=open(gblock,0))== -1)
		error(15);
	readfi(fp);
	reset();
}

/*
 *      The routine that actually reads in a file. It sets up readfile
 *    so that if there is an error ( linenumber overflow ) , then error
 *    can pick up the pieces , else the number of file descriptors are
 *    reduced and can ( unlikely ), run out of them so stopping any file
 *    being saved or restored , ( This is the reason that all files are
 *    closed so meticulacly ( see 'chain' and its  pipes ).
 */

readfi(fp)
{
	register char   *p;
	int     i;
	char    chblock[BLOCKSIZ];
	int     nleft=0;
	register int    special=0;
	register char   *q;

	readfile=fp;
	inserted=1;     /* make certain variables are cleared */
	p=line;         /* input into line[] */
	for(;;){
		if(!nleft){
			q=chblock;
			if( (nleft=read(fp,q,BLOCKSIZ)) <= 0)
				break;
		}
		*p= *q++;
		nleft--;
		if(special){
			special=0;
			if(*p>='a' && *p<='~'){
				*p -= ('a'-1);
				continue;
			}
		}
		if(*p =='\n'){
			*p=0;
			i=compile(0);
			if(!linenumber)
				goto bad;
			insert(i);
			p=line;
			continue;
		}
		else if(*p<' ')
			goto bad;
		else if(*p=='\\')
			special++;
		if(++p > &line[MAXLIN])
			goto bad;
	}
	if(p!=line)
		goto bad;
	close(fp);
	readfile=0;
	return;

bad:    close(fp);              /* come here if there is an error */
	readfile=0;             /* that readfi() has detected */
	error(23);              /* stops error() having to tidy up */
}

/*
 *      The 'new' command , This deletes any program and clears all
 *    variables , can take an extra parameter to say how many files are
 *    needed. If so then clears the number of buffers ( default 2 ).
 */

neww()
{
	register int    i,c;
	register struct filebuf *p;
	register memp   size;

	c=getch();
	point--;
	if(!istermin(c)){
		i=evalint();
		check();
		closeall();             /* flush the buffers */
		if(i<0 || i> MAXFILES)
			i=2;
		fendcore= filestart + (sizeof(struct filebuf) * i );
		size = fendcore + sizeof(xlinnumb);
		size = (char *) ( ((int)size + MEMINC) & ~MEMINC);
		brk(size);
		for(p = (filebufp)filestart ; p < (filebufp)fendcore ; p++){
			p->filedes=0;
			p->userfiledes=0;
			p->use=0;
			p->nleft=0;
		}
	}
	else
		check();
	autostart=10;
	autoincr=10;
	baseval=1;
	ecore= fendcore + sizeof(xlinnumb);
	( (lpoint)fendcore )->linnumb=0;
	clear(DEFAULTSTRING);
	closeall();
	reset();
}

/*
 *      The 'chain' command , This routine chains the program.
 *      all simple numeric variables are kept. ( max of 4 k ).
 *      all other variables are cleared.
 *      runs the loaded file
 *      files are kept open
 *
 *      error need only check pipe[0] to see if it is to be closed.
 */

chain()
{
	register int     fp;
	register int     size;
	register char    *p;
	int     ssize,nsize;
#ifdef  LNAMES
	register struct  entry  *ep,*np;
	register int    *xp;
#endif

	stringeval(gblock);
	check();
	gblock[gcursiz]=0;
	size= vend- earray;
#ifdef  LNAMES
	nsize = enames - estring;               /* can only save offsets */
	if(nsize + size >= 4096)                /* cos ecore/estring might */
#else                                           /* change */
	if(size >= 4096 )
#endif
		error(42);              /* out of space for varibles */
	if((fp=open(gblock,0))== -1)
		error(15);
	ssize= estring- ecore;          /* amount of string space */
	pipe(pipes);
	write(pipes[1],earray,size);    /* check this */
#ifdef  LNAMES
	write(pipes[1],estring,nsize);
#endif
	close(pipes[1]);
	pipes[1]=0;
	ecore= fendcore + sizeof(xlinnumb);     /* bye bye old file */
	( (lpoint)fendcore )->linnumb=0; /* commited to new file now */
	readfi(fp);
	clear(ssize);
	errortrap=0;
	inserted=0;                     /* say we don't actually want to */
	p= xpand(&vend,size);           /* clear variables on return */
	read(pipes[0],p,size);
#ifdef  LNAMES
	p = xpand(&enames,nsize);
	read(pipes[0],p,nsize);
	/*
	 * now rehash the symbol table
	 * cos it gets munged when it moves
	 */
	for(ep = (struct entry *)estring; ep < (struct entry *)enames; ep++){
		ep->link = 0;
		for(p = ep->_name,size = 0; *p ; size += *p++);
		ep->ln_hash = size;
		if(np = hshtab[size %= HSHTABSIZ]){
			for(;np->link ;np = np->link);
			np->link = ep;
		}
		else
			hshtab[size] = ep;
	}
	/*
	 * must zap varshash - because of above
	 */
	for( xp = varshash ; xp < &varshash[HSHTABSIZ] ; *xp++ = -1);
	chained = 1;
#endif
	close(pipes[0]);                /* now have data back from pipe */
	pipes[0]=0;
	stocurlin= (lpoint)fendcore;
	if(!(curline=stocurlin->linnumb))
		reset();
	point= stocurlin->lin;
	elsecount=0;
	runmode=1;
	return(-1);                     /* now run the file */
}

/* define a function def fna() - can have up to 3 parameters */

deffunc()
{
	struct  deffn   fn;     /* temporary place for evaluation */
	register struct deffn *p;
	register int     i=0;
	int     c;
	char    *j;
	register char   *l;

	if(getch() != FN)
		error(SYNTAX);
	if(!isletter(*point))
		error(SYNTAX);
	getnm();
	if(vartype == 02)
		error(VARREQD);
	fn.dnm = nm;
#ifdef  LNAMES
	for(p = (deffnp)enames ; p < (deffnp)edefns ;
#else
	for(p = (deffnp)estring ; p < (deffnp)edefns ;
#endif
					p = (deffnp)( (memp)p + p->offs) )
		if(p->dnm == nm )
			error(REDEFFN); /* redefined functions */
	fn.vtys=vartype<<4;     /* save return type of function */
	if(*point=='('){        /* get arguments */
		point++;
		for(;i<3;i++){
			l=getname();
			if( l < earray)
				error(VARREQD);
			fn.vargs[i]= l - earray;
			fn.vtys |= vartype <<i;  /* save type of arguments */
			if((c=getch())!=',')
				break;
		}
		if(c!= ')')
			error(SYNTAX);
		i++;
	}
	if(getch()!='=')
		error(SYNTAX);
	fn.narg=i;
	l = point;
	while(*l++ == ' ');
	point = --l;
	while(!istermin(*l))    /* get rest of expression */
		l++;
	if(l==point)
		error(SYNTAX);
	i= l - point + sizeof(struct deffn);
#ifdef  ALIGN4
	i = (i + 03) & ~03;
#else
	if(i&01)                /* even up space requirement */
		i++;
#endif
	p= (deffnp) xpand(&edefns,i );          /* get the space */
#ifndef V6C
	*p = fn;
	p->offs = i;
#else
	p->dnm = fn.dnm;                    /* put all values in */
	p->offs=i;
	p->narg=fn.narg;
	p->vtys= fn.vtys;
	p->vargs[0]=fn.vargs[0];
	p->vargs[1]=fn.vargs[1];
	p->vargs[2]=fn.vargs[2];
#endif
	j= p->exp;
	while( point<l)         /* store away line */
		*j++ = *point++;
	*j=0;
	normret;
}

/* the repeat part of the repeat - until loop */
/* now can have a construct like  'repeat until eof(1)'. */
/* It might be of use ?? it's a special case */


rept()
{
	register struct forst   *p;
	register int    c;
	register char   *tp;

	if(getch() == UNTIL){
		tp = point;     /* save point */
		eval();         /* calculate the value */
		check();        /* check syntax */
#ifdef  PORTABLE
		while((vartype ? (!res.i) :(res.f == 0)) && !trapped){
#else
		while(!res.i && !trapped){ /* now repeat the loop until <>0 */
#endif
			point = tp;
			eval();
		}
		normret;
	}
	point--;
	check();
	p= (forstp)vvend;
	vvend += sizeof(struct forst);
	mtest(vvend);
	p->pt = point;
	p->stolin = stocurlin;
	p->elses = elsecount;
	p->fr = 0;              /* make it look like a gosub like */
	p->fnnm = (char *)01;   /* distinguish from gosub's */
	normret;
}

/* the until bit of the command */

untilf()
{
	register struct forst   *p;
	eval();
	check();
	for(p= (forstp)vvend , p-- ; p >= (forstp)bstk ; p--)
		if(!p->fr)
			goto got;
	error(48);
got:
	if(p->fnnm != (char *)01 )
		error(51);
#ifdef  PORTABLE
	if(vartype ? (!res.i) : (res.f == 0)){
#else
	if(!res.i){             /* not true so repeat loop */
#endif
		elsecount = p->elses;
		point = p->pt;
		if(stocurlin = p->stolin)
			curline = stocurlin->linnumb;
		else runmode =0;
		vvend = (memp)(p+1);    /* pop all off stack up until here */
	}
	else
		vvend = (memp)p;        /* pop stack if finished here. */
	normret;
}

/* while part of while - wend construct. This is like repeat until unless
 * loop fails on the first time. (Yeuch - next we need syntax checking on
 * input ).
 */

whilef()
{
	register char    *spoint = point;
	register lpoint lp;
	register struct forst   *p;
	lpoint  get_end();
	eval();
	check();
#ifdef  PORTABLE
	if(vartype ? res.i : res.f){
#else
	if(res.i){  /* got to go through it once so make it look like a */
		    /* repeat - until */
#endif
		p= (forstp)vvend;
		vvend += sizeof(struct forst);
		mtest(vvend);
		p->pt = spoint;
		p->stolin = stocurlin;
		p->elses = elsecount;
		p->fr = 0;              /* make it look like a gosub like */
		p->fnnm = (char *)02;   /* distinguish from gosub's */
		normret;
	}
	lp=get_end();                   /* otherwise find a wend */
	check();
	if(runmode){
		stocurlin =lp;
		curline = lp->linnumb;
	}
	normret;
}

/* the end part of a while loop - wend */

wendf()
{
	register struct forst   *p;
	char    *spoint =point;
	check();
	for(p= (forstp)vvend , p-- ; p >= (forstp)bstk ; p--)
		if(!p->fr)
			goto got;
	error(49);
got:
	if( p->fnnm != (char *)02 )
		error(51);
	point = p->pt;
	eval();
#ifdef  PORTABLE
	if(vartype ? (!res.i) : (res.f == 0)){
#else
	if(!res.i){                     /* failure of the loop */
#endif
		vvend= (memp)p;
		point = spoint;
		normret;
	}
	vvend = (memp)(p+1);            /* pop stack after an iteration */
	elsecount = p->elses;
	if(stocurlin = p->stolin)
		curline = stocurlin->linnumb;
	else runmode=0;
	normret;
}

/* get_end - search from current position until found a wend statement - of
 * the correct nesting. Keeping track of elses + if's(Yeuch ).
 */

lpoint
get_end()
{
	register lpoint lp;
	register char   *p;
	register int    c;
	int     wcount=0;
	int     rcount=0;
	int     flag=0;

	p= point;
	lp= stocurlin;
	if(getch()!=':'){
		if(!runmode)
			error(50);
		lp = (lpoint)((memp)lp +lenv(lp));
		if(!lp->linnumb)
			error(50);
		point = lp->lin;
		elsecount=0;
	}
	for(;;){
		c=getch();
		if(c==WHILE)
			wcount++;
		else if(c==WEND){
			if(--wcount <0)
				break;  /* only get out point in loop */
		}
		else if(c==REPEAT)
			rcount++;
		else if(c==UNTIL){
			if(--rcount<0)
				error(51);      /* bad nesting */
		}
		else if(c==IF){
			flag++;
			elsecount++;
		}
		else if(c==ELSE){
			flag++;
			if(elsecount)
				elsecount--;
		}
		else if(c==REM || c==DATA || c==QUOTE){
			if(!runmode)
				error(50);      /* no wend */
			lp = (lpoint)((memp)lp +lenv(lp));
			if(!lp->linnumb)
				error(50);      /* no wend */
			point =lp->lin;
			elsecount=0;
			flag=0;
			continue;
		}
		else for(p=point;!istermin(*p);p++)
			if(*p=='"' || *p=='`'){
				c= *p++;
				while(*p && *p != (char) c)
					p++;
				if(!*p)
					break;
			}
		if(!*p++){
			if(!runmode)
				error(50);
			lp = (lpoint)((memp)lp +lenv(lp));
			if(!lp->linnumb)
				error(50);
			point =lp->lin;
			elsecount=0;
			flag=0;
		}
		else
			point = p;
	}
	/* we have found it at this point - end of loop */
	if(rcount || (lp!=stocurlin && flag) )
		error(51);      /* bad nesting or wend after an if */
	return(lp);             /* not on same line */
}

#ifdef  RENUMB

/*
 * the renumber routine. It is a three pass algorithm.
 *      1) Find all line numbers that are in text.
 *         Save in table.
 *      2) Renumber all lines.
 *         Fill in table with lines that are found
 *      3) Find all line numbers and update to new values.
 *
 *      This routine eats stack space and also some code space
 *      If you don't want it don't define RENUMB.
 *      Could run out of stack if on V7 PDP-11's
 *      ( On vax's it does not matter. Also can increase MAXRLINES.)
 *      MAXRLINES can be reduced if not got split i-d. If this is
 *      the case then probarbly do not want this code anyway.
 */

#define MAXRLINES       500     /* the maximum number of lines that */
				/* can be changed. Change if neccasary */

renumb()
{
	struct  ta {
		unsigned linn;
		unsigned toli;
		} ta[MAXRLINES];

	struct  ta      *eta = ta;
	register struct ta *tp;
	register char   *q;
	register lpoint p;

	unsigned l1,start,inc;
	int     size,sl,pl;
	char    onfl,chg,*r,*s;
	long    numb;

	start = 10;
	inc = 10;
	l1 = getlin();
	if(l1 != (unsigned)(-1) ){              /* get start line number */
		start = l1;
		if(getch() != ',')
			point--;
		else {
			l1 = getlin();          /* get increment */
			if(l1 == (unsigned)(-1))
				error(5);
			inc = l1;
		}
	}
	check();                /* check rest of line */
	numb = start;           /* set start counter */
	for(p=(lpoint)fendcore; p->linnumb ;p=(lpoint)((char *)p+lenv(p))){
		numb += inc;
		if(numb >= 65530 )      /* check line numbers */
			error(7);       /* line number overflow */
		onfl = 0;               /* flag to deal with on_goto */
		for(q = p->lin; *q ; q++){      /* now find keywords */
			if( !(*q & (char)0200 ))        /* not one */
				continue;               /* ignore */
			if(*q == (char) ON){            /* the on keyword */
				onfl++;                 /* set flag */
				continue;
			}               /* check items with optional numbers*/
			if(*q == (char)ELSE || *q == (char)THEN ||
				*q == (char)RESUME || *q == (char)RESTORE
					|| *q == (char) RUNN ){
				q++;
				while(*q++ == ' ');
				q--;
				if(isnumber(*q))        /* got one ok */
					goto ok1;
			}
			if(*q != (char) GOTO && *q != (char)GOSUB)
				continue;       /* can't be anything else */
			q++;
		ok1:                            /* have a label */
			do{
				while(*q++ == ' ');
				q--;                    /* look for number */
				if( !isnumber(*q) ){
				      prints("Line number required on line ");
					prints(printlin(p->linnumb));
					prints(nl);             /* missing */
					goto out1;
				}
				for(l1 = 0; isnumber(*q) ; q++) /* get it */
					if(l1 >= 6553)
						error(7);
					else l1 = l1 * 10 + *q - '0';
				for(tp  = ta ; tp < eta ; tp++) /* already */
					if(tp->linn == l1)      /* got it ? */
						break;
				if(tp >= eta ){        /* add another entry */
					tp->linn = l1;
					tp->toli = -1;
					if(++eta >= &ta[MAXRLINES])
						error(24);   /* out of core */
				}
				if(!onfl)               /* check flag */
					break;          /* get next item */
				while(*q++== ' ');      /* if ON and comma */
			}while( *(q-1) ==',');
			if(onfl)
				q--;
			onfl =0;
			q--;
		}
	out1:   ;
	}
	numb = start;           /* reset counter */
	for(p= (lpoint)fendcore ; p->linnumb ;p=(lpoint)((char *)p+lenv(p)) ){
		for(tp = ta ; tp < eta ; tp++)          /* change numbers */
			if(tp->linn == p->linnumb){
				tp->toli = numb;  /* inform of new number */
				break;
			}
		p->linnumb = numb;
		numb += inc;
	}
	for(p= (lpoint)fendcore ; p->linnumb ;p=(lpoint)((char *)p+lenv(p)) ){
		onfl = 0;
		chg = 0;                        /* set if line changed */
		for(r = nline, q = p->lin ; *q ; *r++ = *q++){
			if(  r >= &nline[MAXLIN])  /* overflow of line */
				break;
			if( !(*q & (char) 0200 )) /* repeat search for */
				continue;         /* keywords */
			if(*q == (char) ON){
				onfl++;
				continue;
			}
			if(*q == (char)ELSE || *q == (char)THEN ||
				*q == (char)RESUME || *q == (char)RESTORE
					|| *q == (char) RUNN ){
				*r++ = *q++;
				while(*q == ' ' && r < &nline[MAXLIN] )
					*r++ = *q++;
				if(isnumber(*q)) /* got optional line number*/
					goto ok2;
			}
			if(*q != (char) GOTO && *q != (char)GOSUB)
				continue;
			*r++ = *q++;
			for(;;){
				while(*q == ' ' && r < &nline[MAXLIN] )
					*r++ = *q++;
			ok2: ;
				if(r>= &nline[MAXLIN] )
					break;
				for(l1 = 0 ; isnumber(*q) ; q++) /* get numb*/
					l1 = l1 * 10 + *q - '0';
				if(l1 == 0)         /* skip if not found */
					goto out;   /* never happen ?? */
				for(tp = ta ; tp < eta ; tp++)
					if(tp->linn == l1)
						break;
				if(tp->linn != tp->toli)
					chg++;       /* number has changed */
				if(tp >= eta || tp->toli == (unsigned)(-1) ){
					prints("undefined line: ");
					prints(printlin(l1));
					prints(" on line ");
					prints(printlin(p->linnumb));
					prints(nl);     /* can't find it */
					goto out;
				}
				s = printlin(tp->toli); /* get new number */
				while( *s && r < &nline[MAXLIN])
					*r++ = *s++;
				if(r >= &nline[MAXLIN] )
					break;
				if(onfl){       /* repeat if ON statement */
					while(*q == ' ' && r < &nline[MAXLIN])
						*r++ = *q++;
					if(*q == ','){
						*r++ = *q++;
						continue;
					}
				}
				break;
			}
			onfl = 0;
			if(r >= &nline[MAXLIN])
				error(32);      /* line length overflow */
		}
		if(!chg)                /* not changed so don't put back */
			continue;
		inserted =1;            /* say we have changed it */
		for(*r = 0, r = nline; *r++ ;);
		r--;
		size = (r - nline) + sizeof(struct olin); /* get size */
#ifdef  ALIGN4
		size = (size + 03) & ~03;
#else
		if(size & 01)                   /* even it up */
			size++;
#endif
		if(size != lenv(p) ){           /* size changed. insert */
			pl = p->linnumb;        /* save line number */
			sl = lenv(p);           /* save length */
			bmov((short *)p,sl);    /* compress core */
			ecore -= sl;            /* shrink it */
			mtest(ecore+size);      /* get more core */
			ecore += size;          /* add it */
			bmovu((short *)p,size);   /* expand core */
			p->linnumb = pl;        /* restore line number */
			lenv(p) = size;         /* set size */
		}
		strcpy(nline,p->lin);   /* copy back new line */
	out:    ;
	}
	reset();
}
#else
renumb(){}
#endif  /* RENUMB */

/* the load command. Load a dump image. Works fastwer than save/old */

#define MAGIC1          013121
#define MAGIC2          027212

loadd()
{
	register int     nsize;
	register fp;
	int     header[3];

	stringeval(gblock);
	check();
	gblock[gcursiz] = 0;
	if( (fp = open(gblock,0))< 0)
		error(14);
	if(read(fp,(char *)header,sizeof(int)*3) != sizeof(int)*3){
		close(fp);
		error(23);      /* bad load / format file */
	}
	if(header[0] != MAGIC1 && header[1] != MAGIC2){
		close(fp);
		error(23);
	}
	ecore = fendcore + sizeof(xlinnumb);
	mtest(ecore);           /* good bye old image */
	((lpoint)fendcore)->linnumb = 0;
	inserted = 1;
	readfile = fp;
	mtest(ecore+header[2]);
	readfile = 0;
	ecore += header[2];
	nsize = read(fp,fendcore,header[2]);
	close(fp);
	if(nsize != header[2]){
		ecore = fendcore + sizeof(xlinnumb);
		mtest(ecore);
		((lpoint)fendcore)->linnumb = 0;
		error(23);
	}
	reset();
}

/* write out the core to the file */

dump()
{
	register int     nsize;
	register fp;
	int     header[3];

	stringeval(gblock);
	check();
	gblock[gcursiz] = 0;
	if( (fp = creat(gblock,0644))< 0)
		error(15);
	header[0] = MAGIC1;
	header[1] = MAGIC2;
	nsize = ecore - fendcore;
	header[2] = nsize;
	write(fp,(char *)header,sizeof(int)*3);
	write(fp,fendcore,nsize);
	close(fp);
	normret;
}
