/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *      This file contains subroutines used by many commands
 */

/*      stringcompare will compare two strings and return a valid
 *    logical value
 */

stringcompare()
{
	char    chblock[256];
	register int    i;
	register char   *p,*q;
	int     cursiz;
	int     reslt=0;
	int     c;

	checksp();
	stringeval(chblock);
	cursiz=gcursiz;
	if(! (c=getch()) )
		error(SYNTAX);
	stringeval(gblock);
	if(i = ((cursiz > gcursiz) ? gcursiz : cursiz) ){
		/*
		 * make i the minimum of gcursiz and cursiz
		 */
		gcursiz -= i; cursiz -= i;
		p=chblock; q=gblock;    /* set pointers */
		do{
			if(*p++ != *q++){       /* do the compare */
				if( (*(p-1) & 0377) > (*(q-1) & 0377) )
					reslt++;
				else
					reslt--;
				compare(c,reslt);
				return;
			}
		}while(--i);
	}
	if(cursiz)
		reslt++;
	else if(gcursiz)
		reslt--;
	compare(c,reslt);
}

/*      given the comparison operator 'c' then returns a value
 *    given that 'reslt' has a value of:-
 *              0:      equal
 *              1:      greater than
 *             -1:      less than
 */

compare(c,reslt)
register int     c;
register int    reslt;
{
	vartype=01;
	if(c==EQL){
		if(!reslt)
			goto true;
	}
	else if(c==LTEQ){
		if( reslt<=0)
			goto true;
	}
	else if(c==NEQE){
		if( reslt)
			goto true;
	}
	else if(c==LTTH){
		if( reslt<0)
			goto true;
	}
	else if(c==GTEQ){
		if( reslt>=0)
			goto true;
	}
	else if(c==GRTH){
		if( reslt>0)
			goto true;
	}
	else
		error(SYNTAX);
	res.i=0;        /* false */
	return;
true:
	res.i = -1;
}

/*      converts a number in 'res' to a string in gblock
 *    the string will have a space at the start if it is positive
 */

gcvt()
{
	int     sign, decpt;
	int     ndigit=9;
	register char   *p1, *p2;
	register int    i;
#ifndef SOFTFP
	char    *ecvt();
#else
	char    *necvt();
#endif

#ifdef  PORTABLE
	if(vartype==01 || !res.f){
#else
	if(vartype==01 || !res.i){ /* integer deal with them separately */
#endif
		lgcvt();
		return;
	}
#ifndef SOFTFP
	p1 = ecvt(res.f, ndigit+2, &decpt, &sign);
#else
	p1 = necvt(&res, ndigit+2, &decpt, &sign);
#endif
	if (sign)
		*gblock = '-';
	else
		*gblock = ' ';
	if(ndigit > 1){
		p2 = p1 + ndigit-1;
		do {
			if(*p2 != '0')
				break;
			ndigit--;
		}while(--p2 > p1);
	}
	p2 = &gblock[1];
/*
	for (i=ndigit-1; i>0 && *(p1+i) =='0'; i--)
		ndigit--;
*/
	if (decpt < 0 || decpt > 9){
		decpt--;
		*p2++ = *p1++;
		if(ndigit != 1){
			*p2++ = '.';
			for (i=1; i<ndigit; i++)
				*p2++ = *p1++;
		}
		*p2++ = 'e';
		if (decpt<0) {
			decpt = -decpt;
			*p2++ = '-';
		}
		if(decpt >= 10){
			*p2++ = decpt/10 + '0';
			decpt %= 10;
		}
		*p2++ = decpt + '0';
	}
	else {
		if (!decpt) {
			*p2++ = '0';
			*p2++ = '.';
		}
		for (i=1; i<=ndigit; i++) {
			*p2++ = *p1++;
			if (i==decpt && i != ndigit)
				*p2++ = '.';
		}
		while (ndigit++<decpt)
			*p2++ = '0';
	}
	*p2 =0;
	gcursiz= p2 -gblock;
}

/* integer version of above - a very simple algorithm */

lgcvt()
{
	static  char    s[7];
	register char   *p,*q;
	int     fl=0;
	register unsigned l;

	l=  res.i;
	p= &s[6];
	if((int)l <0){
		fl++;
		l= -l;
	}
	do{
		*p-- = l%10 +'0';
	}while(l/=10 );
	if(fl)
		*p ='-';
	else
		*p =' ';
	q=gblock;
	while(*q++ = *p++);
	gcursiz= --q - gblock;
}

/*      get a linenumber or if no linenumber return a -1
 *    used by all routines with optional linenumbers
 */

getlin()
{
	register unsigned l=0;
	register int    c;

	c=getch();
	if(!isnumber(c)){
		point--;
		return(-1);
	}
	do{
		if(l>=6553 )
			error(7);
		l= l*10 + (c-'0');
		c= *point++;
	}while(isnumber(c));
	point--;
	return(l);
}

/*      getline() gets a line number and returns a valid pointer
 *    to it, if there is no linenumber or the line is not there
 *    then there is an error. Used by 'goto' etc.
 */

lpoint
getline()
{
	register unsigned l=0;
	register lpoint p;
	register int    c;

	c=getch();
	if(!isnumber(c))
		error(5);
	do{
		if(l>=6553)
			error(7);
		l= l*10+(c-'0');
		c= *point++;
	}while(isnumber(c));
	point--;
	if(runmode && l >= curline)     /* speed it up a bit */
		p = stocurlin;          /* no need to search the whole lot */
	else
		p = (lpoint)fendcore;
	for(; p->linnumb ;p = (lpoint)((memp)p + lenv(p)))
		if(p->linnumb == l)
			return(p);
	error(6);
}

/*      printlin() returns a pointer to a string representing the
 *    the numeric value of the linenumber.  linenumbers are unsigned
 *    quantities.
 */

char    *
printlin(l)
register unsigned l;
{
	static char   ln[7];
	register char   *p;

	p = &ln[5];
	do{
		*p-- = l %10 + '0';
	}while(l/=10);
	p++;
	return(p);
}

/*      routine used to check the type of expression being evaluated
 *    used by print and eval.
 *      A string expression returns a value of '1'
 *      A numeric expression returns a value of '0'
 */

checktype()
{
	register char   *tpoint;
	register int    c;

	if( (c= *point) & 0200){
		if( (c&0377) >= MINFUNC)
			goto data;
		else  goto string;
	}
	if(isnumber(c) || c=='.' || c== '-' || c=='(')
		goto data;
	if(c=='"' || c=='`')
		goto string;
	if(!isletter(c))
		error(SYNTAX);
	tpoint= point;
	do{
		c= *++tpoint;
	}while(isletter(c) || isnumber(c));
	if(c!='$')
data:           return(0);
string: return(1);
}

/*      print out a message , used for all types of 'basic' messages
 */

prints(s)
char    *s;
{
	register char   *i;

	i=s;
	while(*i++);
	write(1,s,--i-s);
}

/*      copy a string from a to b returning the last address used in b
 */

char    *
strcpy(a,b)
register char   *a,*b;
{
	while(*b++ = *a++);
	return(--b);
}


#ifndef SOFTFP

/* convert an ascii string into a number. If it is possibly an integer
 * return an integer.
 * Otherwise return a double ( in res )
 * should never overflow. One day I may fix the non floating point one.
 */


#define BIG     1.701411835e37

getop()
{
	register double x = 0;
	register int    exponent = 0;
	register int    ndigits = 0;
	register int    c;
	register int    exp;
	char    decp = 0;
	char    lzeros = 0;
	int     minus;
	short   xx;

dot:    for(c = *point ; isnumber(c) ; c = *++point){
		if(!lzeros){
			if(c == '0'){ /* ignore leading zeros */
				if(decp)
					exponent--;
				continue;
			}
			lzeros++;
		}
		if(ndigits >= 15){      /* ignore insignificant digits */
			if(!decp)
				exponent++;
			continue;
		}
		if(decp)
			exponent--;
		ndigits++;
		x = x * 10 + c - '0';
	}
	if(c == '.'){
		point++;
		if(decp)
			return(0);
		decp++;
		goto dot;
	}
	if(c == 'e' || c == 'E'){
		minus = 0;
		if( (c = *++point) == '+')
			point++;
		else if(c =='-'){
			minus++;
			point++;
		}
		else if(c < '0' || c > '9')
			return(0);
		for(exp = 0, c = *point; c >= '0' && c <= '9' ; c = *++point){
			if(exp < 1000)
				exp = exp * 10 + c - '0';
		}
		if(minus)
			exponent -= exp;
		else
			exponent += exp;
	}
	while(exponent < 0){
		exponent++;
		x /= 10;
	}
	while(exponent > 0){
		exponent--;
		if(x > BIG)
			return(0);
		x *= 10;
	}
	xx = x;                 /* see if x is == an integer */
	/*
	 * shouldn't need a cast below but there is a bug in the 68000
	 * compiler which does the comparison wrong without it.
	 */
	if( (double) xx == x){
		vartype= 01;
		res.i = xx;
	} else {
		vartype = 0;
		res.f = x;
	}
	return(1);
}
#endif
