/*
 * BASIC by Phil Cockcroft
 */
#include "bas.h"

/* this file contains all the routines that were originally done in assembler
 * these routines only require a floating point emulator to work.
 * To speed things up some routines could be put into assembler and some
 * could be made into macros. the relevent routines are labeled as such
 */

#ifndef VAX_ASSEM       /* if done in assembler don't bring it in */
/* AS */

/* get a single character from the line pointed to by getch() */

getch()
{
	register char   *p;

	p = point;
	while(*p++ == ' ');
	point = p;
	return(*--p & 0377);
}

/* AS  #define  ELSE 0351 */

check()         /* check to see no garbage at end of command */
{
	register char   *p;
	register char   c;

	p = point;
	while(*p++ == ' ');
	if(! (c = *--p) || c == ':' || (c == (char)ELSE && elsecount)){
		point = p;
		return;
	}
	error(SYNTAX);          /* not a terminator - error */
}
#endif

#ifndef SOFTFP
fpcrash()
{
	error(34);      /* arithmetic overflow */
}
#endif

int     (*fpfunc)();

startfp()
{
#ifndef SOFTFP
	fpfunc = fpcrash;       /* will call error(34) on overflow */
#else
	fpfunc = 0;
#endif
}

/* AS */

/* compare two values. return 0 if equal -1 if first less than second
 * or 1 for vice versa.
 */

cmp(p,q)
register value  *p,*q;
{
	if(vartype){
		if(p->i == q->i)
			return(0);
		else if(p->i < q->i)
			return(-1);
		return(1);
	}
	if(p->f == q->f)
		return(0);
	else if(p->f< q->f )
		return(-1);
	return(1);
}

/* the arithmetic operation jump table */


/* all the routines below should be put into AS */

int     fandor(), andor(), comop(), fads(), ads(),
	fmdm(), mdm(), fexp(), ex();

int     (*mbin[])() = {
	0,0,
	fandor,
	andor,
	comop,
	comop,
	fads,
	ads,
	fmdm,
	mdm,
	fexp,
	ex,
	};

typedef value   *valp;

ex(p,q,c)               /* integer exponentiation */
valp    p,q;
{
	cvt(p);
	cvt(q);
	vartype = 0;
	fexp(p,q,c);
}

fmdm(p,q,c)             /* floating * / mod */
valp    p,q;
{
	double  floor(),x;
	if(c == '*'){
		fmul(p,q);
		return;
	}
	if(q->f == 0)
		error(25);
	if(c=='/')
		fdiv(p,q);
	else  {         /* floating mod - yeuch */
		if( (x = p->f/q->f) < 0)
			q->f = p->f + floor(-x) * q->f;
		else
			q->f = p->f - floor(x) * q->f;
	}
}

mdm(p,q,c)              /* integer * / mod */
valp    p,q;
{
	register long    l;
	register short  ll;

	l = p->i;
	if(c=='*'){
		l *= q->i;
#ifdef  VAX_ASSEM
		ll = l;
		{ asm("bvc mdmov"); }
			q->f = l;
			vartype = 0;
			{ asm("ret"); }         /* could be 'return' */
		{ asm("mdmov: "); }
		q->i = ll;
#else
		if(l > 32767 || l < -32768){    /* overflow */
			q->f = l;
			vartype = 0;
		}
		else q->i = l;
#endif
		return;
	}
	if(!q->i)                       /* zero divisor error */
		error(25);
	ll = p->i % q->i;
	if(c == '/'){
		if(ll){
			q->f = (double)l / q->i;
			vartype = 0;
		}
		else
			q->i = p->i / q->i;
	}
	else
		q->i = ll;
}

fads(p,q,c)             /* floating + - */
valp    p,q;
{
	if(c=='+')
		fadd(p,q);
	else
		fsub(p,q);
}

ads(p,q,c)              /* integer + - */
valp    p,q;
{
	register long   l;
#ifdef  VAX_ASSEM
	register short  ll;
#endif

	l = p->i;
	if(c == '+')
		l += q->i;
	else
		l -= q->i;
#ifdef  VAX_ASSEM
		ll = l;
		{ asm("bvc adsov"); }
			q->f = l;
			vartype = 0;
			{ asm("ret"); }         /* could be 'return' */
		{ asm("adsov: "); }
		q->i = ll;
#else
	if(l > 32767 || l < -32768){    /* overflow */
		q->f = l;
		vartype = 0;
	}
	else
		q->i = l;
#endif
}

comop(p,q,c)                    /* comparison operations */
valp    p,q;
{
	compare(c,cmp(p,q));
}

fandor(p,q,c)                   /* floating logical AND/OR/XOR */
register valp    p,q;
{
	vartype = 01;
#ifdef  PORTABLE
	p->i = ((p->f != 0.0) ? -1 : 0);
	q->i = ((q->f != 0.0) ? -1 : 0);
#else
	p->i = (p->i ? -1 : 0);
	q->i = (q->i ? -1 : 0);
#endif
	andor(p,q,c);
}

andor(p,q,c)                    /* integer logical */
valp    p,q;
{
	register i,j;

	i = p->i;
	j = q->i;
	if(c == ANDD)           /* and */
		i &= j;
	else if(c == ORR)       /* or */
		i |= j;
	else
		i ^= j;         /* xor */
	q->i = i;
}

/* down to about here */

/* MACRO */

putin(p,var)            /* convert + put the value in res into p */
memp    p;
char    var;
{
	if(vartype != var){
		if(var){
			if(conv(&res))
				error(35);
		}
		else
			cvt(&res);
	}
	if(var)
		((value *)p)->i = res.i;
	else
		((value *)p)->f = res.f;
}

/* MACRO */

negate()                /* negate the value in res */
{
	if(vartype){
		if(res.i == -32768){    /* special case */
			res.f = 32768;
			vartype = 0;
		}
		else
			res.i = -res.i;
	}
	else
		res.f = -res.f;
}

/* MACRO */

notit()                 /* logical negation */
{
	if(vartype){
		res.i = ~res.i;
		return;
	}
	vartype = 01;
#ifdef  PORTABLE
	if(res.f)
		res.i = 0;
	else
		res.i = -1;
#else
	if(res.i)
		res.i = 0;
	else
		res.i = -1;
#endif
}

fexp(p,q,c)                     /* floating exponentiation */
valp    p,q;
{
	double  x,log(),exp();

	if(p->f < 0)
		error(41);
	else if(q->f == 0.0)
		q->f = 1.0;
	else if(p->f == 0.0)            /* could use pow - but not on v6 */
		q->f = 0.0;
	else {
		if( (x = log(p->f) * q->f) > 88.02969) /* should be bigger */
			error(40);
		q->f = exp(x);
	}
}
