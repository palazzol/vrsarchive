/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *      This file contains the numeric evaluation routines and some
 *    of the numeric functions.
 */

/*
 *      evalint() is called by a routine that requires an integer value
 *    e.g. string functions. It will always return an integer. If
 *    the result will not overflow an integer -1 is returned.
 *      N.B. most ( all ) routines assume that a negative return is an
 *    error.
 */


evalint()
{
	eval();
	if(vartype)
		return(res.i);
	if(conv(&res))
		return(-1);
	return(res.i);
}

/*
 *      This structure is only ever used by eval() and so is not declared
 *    in 'bas.h' with the others.
 */


struct  m {
	value   r1;
	int     lastop;
	char    value;
	char    vty;
	};

/*
 *      eval() will evaluate any numeric expression and return the result
 *    in the UNION 'res'.
 *      A valid expression can be any numeric expression or a string
 *    comparison expression e.g. "as" <> "gh" . String expressions can
 *    themselves be used in relational tests and also be used with the
 *    logical operators. e.g. "a" <> "b" and "1" <> a$ is a valid
 *    expression.
 */

eval()
{
	extern   (*mbin[])();
	register int    i;
	register int    c;
	register struct    m    *j;
	value   *pp;
	char    firsttime=1;
	char    minus=0,noting=0;
	struct   m      restab[6];

	checksp();
	j=restab;
	j->value=0;

for(;;){
	c=getch();
	if(c=='-' && firsttime){
		if(minus)
			error(SYNTAX);
		minus++;
		continue;
	}
	else if(c==NOTT){
		if(noting)
			error(SYNTAX);
		noting++;
		firsttime++;
		continue;
	}
	else if(c&0200){
		if(c<MINFUNC || c>MAXFUNC)      /* we have a function */
			goto err1;      /* possibly a string function */
		if(c>= RND )                    /* functions that don't */
			(*functs[c-RND])();     /* require arguments */
		else  {
			if(*point++ !='(')
				error(SYNTAX);  /* functions that do */
			(*functb[c-MINFUNC])();
			if(getch()!=')')
				error(SYNTAX);
		}
	}
	else if(isletter(c)){
		char    *sp = --point;

		pp= (value *)getname();         /* we have a variable */
		if(vartype== 02){       /* a string !!!!!! */
			if(firsttime){  /* no need for checktype() since */
				point = sp;     /* we know it's a string */
				stringcompare();
				goto ex;
			}
			else error(2);          /* variable required */
		}
#ifdef  V6C
		getv(pp);
#else
		res = *pp;
#endif
	}
	else if(isnumber(c) || c=='.'){
		point--;
		if(!getop())            /* we have a number */
			error(36);      /* bad number */
	}
	else if(c=='('){                /* bracketed expression */
		eval();                 /* recursive call of eval() */
		if(getch()!=')')
			error(SYNTAX);
	}
	else  {
err1:           /* get here if the function we tried to access was not   */
		/* a legal maths func. or a string variable */
		/* stringcompare() will give a syntax error if not a valid */
		/* string. therefore this works ok */
		point--;
		if(!firsttime)
			error(SYNTAX);
		stringcompare();
	}
ex:
	if(minus){                      /* do the unary minus */
		minus=0;
		negate();
	}
	if(noting){                     /* do the not */
		noting=0;
		notit();
	}
	i=0;
	switch(c=getch()){              /* get the precedence of the */
		case    '^':    i++;    /* operator */
		case    '*':
		case    '/':
		case    MODD:   i++;
		case    '+':
		case    '-':    i++;
		case    EQL:            /* comparison operators */
		case    LTEQ:
		case    NEQE:
		case    LTTH:
		case    GTEQ:
		case    GRTH:   i++;    /* logical operators */
		case    ANDD:
		case    ORR:
		case    XORR:   i++;
	}
	if(i>2)
		firsttime = 0;
ame:    if(j->value< (char)i){          /* current operator has higher */
		(++j)->lastop=c;                        /* precedence */
#ifndef V6C
		j->r1 = res;
#else
		push(&j->r1);  /* block moving */
#endif
		j->value=i;
		j->vty=vartype;
		continue;
	}
	if(! j->value ){                /* end of expression */
		point--;
		return;
	}
	if(j->vty!=vartype){            /* make both parameters */
		if(vartype)             /* the same type */
			cvt(&res);
		else
			cvt(&j->r1);    /* if changed then they must be */
		vartype=0;              /* changed to reals */
	}
	(*mbin[(j->value<<1)+vartype])(&j->r1,&res,j->lastop);
	j--;                    /* execute it then pop the stack and */
	goto ame;               /* deal with the next operator */
	}
}

/*
 *      The rest of the routines in this file evaluate functions and are
 *    relatively straight forward.
 */

tim()
{
	time(&overfl);

#ifndef SOFTFP
	res.f = overfl;
	vartype = 0;
#else
	over(0,&res);           /* convert from long to real */
#endif
}

rnd()
{
	static  double  recip32 = 32767.0;
	value   temp;
	register int    rn;

	rn = rand() & 077777;
	if(*point!='('){
		res.i=rn;
		vartype=01;
		return;
	}
	point++;
	eval();
	if(getch()!=')')
		error(SYNTAX);
#ifdef  PORTABLE
	if(vartype ? res.i : res.f){
#else
	if(res.i){
#endif
		if(!vartype && conv(&res))
			error(FUNCT);
		res.i= rn % res.i + 1;
		vartype=01;
		return;
	}
#ifndef SOFTFP
	res.f = (double)rn / recip32;
#else
	temp.i=rn;
	cvt(&temp);
#ifndef V6C
	res = *( (value *)( &recip32 ) );
#else
	movein(&recip32,&res);
#endif
	fdiv(&temp,&res);            /* horrible */
#endif
	vartype =0;
}

/*
 *      This routine is the command 'random' and is placed here for some
 *    unknown reason it just sets the seed to rnd to the value from
 *    the time system call ( is a random number ).
 */

random()
{
	long    m;
	time(&m);
	srand((int)m);
	normret;
}

erlin()
{
	res.i = elinnumb;
	vartype=01;
	if(res.i < 0 ){                      /* make large linenumbers */
#ifndef SOFTFP
		res.f = (unsigned)elinnumb;
		vartype = 0;
#else
		overfl=(unsigned)elinnumb;      /* into reals as they */
		over(0,&res);                   /* overflow integers */
#endif
	}
}

erval()
{
	res.i =ecode;
	vartype=01;
}

sgn()
{
	eval();
#ifdef  PORTABLE
	if(!vartype){
		if(res.f < 0)
			res.i = -1;
		else if(res.f > 0)
			res.i = 1;
		else res.i = 0;
		vartype = 1;
		return;
	}
#endif
	if(res.i<0)             /* bit twiddling */
		res.i = -1;     /* real numbers have the top bit set if */
	else if(res.i>0)        /* negative and the top word is non-zero */
		res.i= 1;       /* for all non-zero numbers */
	vartype=01;
}

abs()
{
	eval();
#ifdef  PORTABLE
	if(!vartype){
		if(res.f < 0)
			negate();
		return;
	}
#endif
	if(res.i<0)
		negate();
}

len()
{
	stringeval(gblock);
	res.i =gcursiz;
	vartype=01;
}

ascval()
{
	stringeval(gblock);
	if(!gcursiz)
		error(FUNCT);
	res.i = *gblock & 0377;
	vartype=01;
}

sqrtf()
{
#ifndef SOFTFP
	double  sqrt();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifdef  PORTABLE
	if(res.f < 0)
#else
	if(res.i < 0)
#endif
		error(37);      /* negative square root */
#ifndef SOFTFP
	res.f = sqrt(res.f);
#else
	sqrt(&res);
#endif
}

logf()
{
#ifndef SOFTFP
	double  log();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifdef  PORTABLE
	if(res.f <= 0)
#else
	if(res.i <= 0)
#endif
		error(38);      /* bad log value */
#ifndef SOFTFP
	res.f = log(res.f);
#else
	log(&res);
#endif
}

expf()
{
#ifndef SOFTFP
	double  exp();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifndef SOFTFP
	if(res.f > 88.02969)
		error(39);
	res.f = exp(res.f);
#else
	if(!exp(&res))
		error(39);      /* overflow in exp */
#endif
}

pii()
{
#ifndef SOFTFP
	res.f = pivalue;
#else
	movein(&pivalue,&res);
#endif
	vartype=0;
}

/*
 *      This routine will deal with the eval() function. It has to do
 *    a lot of moving of data. to enable it to 'compile' an expression
 *    so that it can be evaluated.
 */


evalu()
{
	register char   *tmp;
	char    chblck1[256];
	char    chblck2[256];

	checksp();
	if(evallock>5)
		error(43);      /* mutually recursive eval */
	evallock++;
	stringeval(gblock);
	gblock[gcursiz]=0;
	strcpy(nline,chblck2);          /* save nline */
	line[0]='\01';                  /* stop a line number being created */
	strcpy(gblock,&line[1]);
	compile(0);
	strcpy(&nline[1],chblck1);    /* restore nline ( eval in immeadiate */
	strcpy(chblck2,nline);        /* mode ). */
	tmp=point;
	point=chblck1;
	eval();
	if(getch())
		error(SYNTAX);
	point=tmp;
	evallock--;
}

ffn()
{
	register struct  deffn   *p;
	value   ovrs[3];
	value   nvrs[3];
	char    vttys[3];
	char    *spoint;
	register int    i;
	if(!isletter(*point))
		error(SYNTAX);
	getnm();
#ifdef  LNAMES
	for(p = (deffnp)enames ; p < (deffnp)edefns ;
					p = (deffnp)((memp)p + p->offs) )
#else
	for( p = (deffnp)estring ; p < (deffnp)edefns ;
					p = (deffnp)((memp)p + p->offs) )
#endif
		if(p->dnm ==nm )
			goto got;
	error(UNDEFFN);
got:
	for(i=0;i<p->narg;i++)  /* save values */
#ifndef V6C
		ovrs[i] = *((value *) (p->vargs[i] + earray) );
#else
		movein( (double *)(p->vargs[i] + earray) ,&ovrs[i]);
#endif
	if(p->narg){
		if(*point++!='(')
			error(SYNTAX);
		for(i=0;;){
			eval();
#ifndef V6C
			nvrs[i] = res;
#else
			movein(&res,&nvrs[i]);
#endif
			vttys[i] = vartype;
			if(++i >= p->narg )
				break;
			if( getch() != ',' )
				error(SYNTAX);
		}
		if( getch() != ')' )
			error(SYNTAX);
	}                               /* got arguments in nvrs[] */

	for(i=0;i<p->narg;i++){         /* put in new values */
#ifndef V6C
		res = nvrs[i];
#else
		movein(&nvrs[i],&res);
#endif
		vartype=vttys[i];
		putin((value *)(p->vargs[i] + earray),((p->vtys>>i)&01));
	}
	spoint=point;
	point=p->exp;
	eval();
	for(i=0;i<p->narg;i++)
#ifndef V6C
		*( (value *)(p->vargs[i] + earray)) = ovrs[i];
#else
		movein(&ovrs[i], (double *) (p->vargs[i] + earray) );
#endif
	if(getch())
		error(SYNTAX);
	point= spoint;
	i= p->vtys>>4;
	if(vartype != (char)i){
		if(vartype)
			cvt(&res);
		else if(conv(&res))
			error(INTOVER);
		vartype=i;
	}
}

/* int() - return the greatest integer less than x */

intf()
{
#ifndef SOFTFP
	double  floor();
	eval();
	if(!vartype)
		res.f = floor(res.f);
	if(!conv(&res))
		vartype=01;
#else
	value   temp;
	static  double  ONE = 1.0;

	eval();
	if(vartype)             /* conv and integ truncate not round */
		return;
#ifdef  PORTABLE
	if(res.f>=0){
#else
	if(res.i>=0){                   /* positive easy */
#endif
		if(!conv(&res))
			vartype=01;
		else integ(&res);
		return;
	}
#ifndef V6C
	temp = res;
#else
	movein(&res,&temp);
#endif
	integ(&res);
	if(cmp(&res,&temp)){            /* not got an integer subtract one */
#ifndef V6C
		res = *((value *)&ONE);
#else
		movein(&ONE,&res);
#endif
		fsub(&temp,&res);
		integ(&res);
	}
	if(!conv(&res))
		vartype=01;
#endif                                  /* not floating point */
}

peekf(sp)
{
	register char   *p;
#ifndef pdp11
	register long   l;
	eval();
	if(vartype)
		cvt(&res);
	l = res.f;
	if(res.f > 0x7fff000 || res.f < 0)      /* check this */
		error(FUNCT);
	p = (char *)l;
#else
	eval();
	if(!vartype && conv(&res))
		error(FUNCT);
	p= (char *)res.i;               /* horrible - fix for a Vax */
#endif
	vartype=01;
	if(p>vvend && p < (char *)&sp )
		res.i=0;
	else res.i = *p & 0377;
}

poke(sp)                /* sp = approx position of stack */
{                                       /* can give bus errors */
#ifndef pdp11                           /* why are you poking any way ??? */
	register long   l;
#endif
	register char   *p;
	register int    i;
	eval();
	if(getch()!=',')
		error(SYNTAX);
#ifndef pdp11
	if(vartype)
		cvt(&res);
	l = res.f;
	if(res.f > 0x7fff000 || res.f < 0)      /* check this */
		error(FUNCT);
	p = (char *)l;
#else
	if(!vartype && conv(&res))
		error(FUNCT);
	p= (char *)res.i;
#endif
	i= evalint();
	check();
	if(i<0)
		error(FUNCT);
	if(p< vvend || p > (char *)&sp)
		*p = i;
	normret;
}

sinf()
{
#ifndef SOFTFP
	double  sin();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifndef SOFTFP
	res.f = sin(res.f);
#else
	sin(&res);
#endif
}

cosf()
{
#ifndef SOFTFP
	double  cos();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifndef SOFTFP
	res.f = cos(res.f);
#else
	cos(&res);
#endif
}

atanf()
{
#ifndef SOFTFP
	double  atan();
#endif
	eval();
	if(vartype)
		cvt(&res);
	vartype=0;
#ifndef SOFTFP
	res.f = atan(res.f);
#else
	atan(&res);
#endif
}

/*
 * the "system" function, returns the status of the command it executes
 */


ssystem()
{
	register int    i;
	register int    (*q)() , (*p)();
	int     (*signal())();
	char    *s;
	int     status;
#ifdef  SIGTSTP
	int     (*t)();
#endif

	stringeval(gblock);             /* get the command */
	gblock[gcursiz] = 0;

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
		execl(s, "sh (from basic)", "-c", gblock, 0);
		exit(-1);                       /* problem */
	}
	if(i != -1){
		p=signal(SIGINT,SIG_IGN);       /* ignore some signals */
		q=signal(SIGQUIT, SIG_IGN);
		while(i != wait(&status) );     /* wait on the 'child' */
		signal(SIGINT,p);               /* resignal to what they */
		signal(SIGQUIT,q);              /* were before */
						/* in a mode fit for basic */
		set_term();                     /* reset terminal modes */
		rset_term(0);
		i = status;
	}
#ifdef  SIGTSTP
	signal(SIGTSTP, t);
#endif
	vartype = 1;
	res.i = i;
}
