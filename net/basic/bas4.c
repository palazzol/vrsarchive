/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *      Stringeval() will evaluate a string expression of any
 *    form. '+' is used as the concatenation operator
 *
 *      gblock and gcursiz are used as global variables by the
 *    string routines. Gblock contains the resultant string while
 *    gcursiz holds the length of the resultant string ( even if not
 *    put in gblock ).
 *      For routines that need more than one result e.g. mid$ instr$
 *    then one result at least is put on the stack while the other
 *    ( possibly ) is put in gblock.
 */

/*
 *      The parameter to stringeval() is a pointer to where the
 *    result will be put.
 */


stringeval(gblck)
char    *gblck;
{
	int     cursiz=0;
	memp    l;
	int     c;
	char    charac;
	register char   *p,*q;
	register int    i;
	int     m[2];
	char    chblock[256];
	char    *ctime();
	checksp();
	q=chblock;
for(;;){
	gcursiz=0;
	c=getch();
	if(c&0200){             /* a string function */
		if(c==DATE){            /* date does not want a parameter */
			time(m);
			p=ctime(m);
			gcursiz=24;
		}
		else {
			if(c<MINSTRING || c>MAXSTRING)
				error(11);
			if(*point++!='(')
				error(1);
			(*strngcommand[c-MINSTRING])();
			if(getch()!=')')
				error(1);
			p=gblock;       /* string functions return with */
		}                       /* result in gblock */
	}
	else if(c=='"' || c=='`'){      /* a quoted string */
		charac=c;
		p=point;
		while(*point && *point!= charac){
			gcursiz++;
			point++;
		}
		if(*point)
			point++;
	}
	else if(isletter(c)){           /* a string variable */
		point--;
		l=getname();
		if(vartype!=02)
			error(SYNTAX);
		if(p= ((stdatap)l)->stpt)           /* newstring routines */
			gcursiz= *p++ &0377;
	}
	else
		error(SYNTAX);
   /* all routines return to here with the string pointed to by p */
	if(cursiz+gcursiz>255)
		error(9);
	i=gcursiz;
	if(getch()!='+')
		break;
	cursiz += i;
	if(i)  do
		 *q++ = *p++;
	       while(--i);
	}
	point--;                        /* the following code is */
	if(!cursiz){                    /* horrible but it speeds */
		if(p==gblck)            /* execution by reducing the amount */
			return;         /* of movement of strings */
		cursiz=gcursiz;
	}
	else {
		cursiz+=gcursiz;
		if(i) do
			*q++ = *p++;
		      while(--i);
		p=chblock;
	}
	q=gblck;
	gcursiz=cursiz;
	if(i=cursiz)
	      do
		*q++ = *p++;
	      while(--i);
}

/*
 *      stringassign() will put the sting in gblock into the string
 *    pointed to by p.
 *      It will call the garbage collection routine as neccasary.
 */

stringassign(p)
struct  stdata *p;
{
	register char   *q,*r;
	register int    i;

	p->stpt=0;
	if(!gcursiz)
		return;
	if(estdt-eostring <gcursiz+1){
		garbage();
		if(estdt-eostring <gcursiz+1)
			error(3);       /* out of string space */
	}
	p->stpt=eostring;
	q=eostring;
	i=gcursiz;
	*q++ = i;
	r= gblock;
	do
		*q++ = *r++;
	while(--i);
	eostring=q;
}

/*
 *      This will collect all unused strings and free the space
 *    It works that is about all tha can be said for it.
 */

garbage()               /* new string routine */
{
	register char   *p,*q;
	register struct stdata  *r;
	register int     j;

	p=ecore;
	q=ecore;
	while(p<eostring){
		j= (*p&0377)+1;
		for(r = (stdatap)estdt ; r < (stdatap)estring ; r++)
			if(r->stpt==p)
				if(q==p){
					p+=j;
					q=p;
					goto more;
				}
				else  {
					r->stpt=q;
					do{
						*q++ = *p++;
					  }while(--j);
					goto more;
				}
		p+=j;
more:           ;
	}
	eostring=q;
}

/*
 *      The following routines implement string functions they are all quite
 *    straight forward in operation.
 */

strng()
{
	int     m;
	register char   *q,*p;
	int    cursiz=0;
	int     siz;
	register int     i;
	char    chblock[256];

	checksp();
	stringeval(chblock);
	cursiz=gcursiz;
	if(getch()!=',')
		error(1);
	m=evalint();
	if(m>255 || m <0)
		error(10);
	if(!cursiz){
		gcursiz=0;
		return;
	}
	siz=m;
	if((unsigned)(cursiz * siz) >255)
		error(9);
	gcursiz= cursiz *siz;
	p=gblock;
	while(siz--)
		for(q=chblock,i=cursiz;i--;)
			*p++ = *q++;
}

/*      left$ string function */

leftst()
{
	int     l1;
	register int    i;
	register char   *p,*q;
	int     cursiz;
	char    chblock[256];

	checksp();
	stringeval(chblock);
	cursiz=gcursiz;
	if(getch()!=',')
		error(SYNTAX);
	l1=evalint();
	if(l1<0 || l1 >255)
		error(10);
	i=l1;
	if(l1>cursiz)
		i=cursiz;
	p=chblock;
	q=gblock;
	if(gcursiz=i) do
		   *q++ = *p++;
	      while(--i);
}

/*      right$ string function */

rightst()
{
	int     l1,l2;
	register int    i;
	register char   *p,*q;
	int     cursiz;
	char    chblock[256];

	checksp();
	stringeval(chblock);
	cursiz=gcursiz;
	if(getch()!=',')
		error(SYNTAX);
	l1=evalint();
	if(l1<0 || l1 >255)
		error(10);
	l2= cursiz-l1;
	i=l1;
	if(i>cursiz){
		i=cursiz;
		l2=0;
	}
	p= &chblock[l2];
	q= gblock;
	if(gcursiz=i) do
		*q++ = *p++;
	      while(--i);
}

/*
 *      midst$ string function:-
 *              can have two or three parameters , if third
 *              parameter is missing then a value of cursiz
 *              is used.
 */

midst()
{
	int     l1,l2;
	int    cursiz;
	register int     i;
	register char   *q,*p;
	char    chblock[256];

	checksp();
	stringeval(chblock);
	cursiz=gcursiz;
	if(getch()!=',')
		error(1);
	l1=evalint()-1;
	if(getch()!=','){
		point--;
		l2=255;
	}
	else
		l2=evalint();
	if(l1<0 || l2<0 || l1 >255 || l2 >255)
		error(10);
	l2+=l1;
	if(l2>cursiz)
		l2=cursiz;
	if(l1>cursiz)
		l1=cursiz;
	i= l2-l1;
	p=gblock;
	q= &chblock[l1];
	if(gcursiz=i) do
		  *p++ = *q++;
	      while(--i);
}

/*      ermsg$ string routine , returns the specified error message */

estrng()
{
	register char   *p,*q,*r;
	int     l;

	l=evalint();
	if(l<1 || l> MAXERR)
		error(22);
	p=ermesg[l-1];
	q=gblock;
	r=p;
	while(*q++ = *p++);
	gcursiz= p-r-1;
}

/*      chr$ string function , returns character from the ascii value */

chrstr()
{
	register int    i;

	i=evalint();
	if(i<0 || i>255)
		error(FUNCT);
	*gblock= i;
	gcursiz=1;
}

/*      str$ string routine , returns a string representation
 *      of the number given. There is NO leading space on positive
 *      numbers.
 */

nstrng()
{
	register char   *p,*q;

	eval();
	gcvt();
	if(*gblock!=' ')
		return;
	q=gblock;
	p= gblock+1;
	while(*q++ = *p++);
	gcursiz= --q -gblock;
}

/*      val() maths function , returns the value of a string. If
 *    no numeric value is used then a value of zero is returned.
 */

val()
{
	register char   *tmp,*p;
	register minus=0;

	stringeval(gblock);
	gblock[gcursiz]=0;
	p=gblock;
	while(*p++ == ' ');
	if(*--p=='-'){
		p++;
		minus++;
	}
	if(!isnumber(*p) && *p!='.'){
		res.i=0;
		vartype=01;
		return;
	}
	tmp=point;
	point=p;
	if(!getop()){
		point=tmp;
		error(36);
	}
	point=tmp;
	if(minus)
		negate();
}

/*      instr() maths function , returns the index of the first string
 *    in the second. Starting either from the first character or from
 *    the optional third parameter position.
 */

instr()
{
	int     cursiz1;
	int     cursiz2;
	register char   *p,*q,*r;
	int     i=0;
	char    chbl1ck[256];
	char    chbl2ck[256];

	checksp();
	stringeval(chbl1ck);
	cursiz1=gcursiz;
	if(getch()!=',')
		error(SYNTAX);
	stringeval(chbl2ck);
	cursiz2=gcursiz;
	if(getch()==','){
		i=evalint()-1;
		if(i<0 || i>255)
			error(10);
	}
	else
		point--;
	cursiz2-=cursiz1;
	vartype=01;
	r= &chbl2ck[cursiz1+i];
	for(;i<=cursiz2;i++,r++){
		p= chbl1ck;
		q= &chbl2ck[i];
		while(q < r && *p== *q)
			p++,q++;
		if( q == r ){
			res.i = i+1;
			return;
		}
	}
	res.i = 0;
}

/*      space$ string function returns a string of spaces the number
 *    of which is the argument to the function
 */

space()
{
	register int    i;
	register char   *q;

	i=evalint();
	if(i<0 || i>255)
		error(10);
	if(gcursiz=i){
		q= gblock;
		do{
			*q++ =' ';
		}while(--i);
	}
}

/* get$() read a single character from a file */

getstf()
{
	register struct filebuf *p;
	register i;

	i=evalint();
	if(!i){
		if(noedit)        /* illegal function with silly terminals */
			error(11);
		if(!trapped){
			set_term();
			*gblock=readc();
			rset_term(0);
		}
		if(!trapped)
			gcursiz=1;
		else
			gcursiz =0;
	}
	else {
		p=getf(i,_READ);
		if(!(i = filein(p,gblock,1)) )
			error(30);
		gcursiz=i;
	}
}


/*      mid$() when on the left of an assignment */
/* can have optional third argument */

/*      a$ = "this is me"
 * mid$(a$,2) = "hello"         ->   a$ = "thello"
 * mid$(a$,2,5) = "hello"       ->   a$ = "thellos me"
 */

lhmidst()
{
	char    chbl1ck[256];
	char    chbl2ck[256];
	int     cursiz,rhside,i1,i2;
	memp    pt;
	register char   *p,*q;
	register int    i;

	if(*point++ !='(')
		error(SYNTAX);
	pt=getname();
	if(vartype!=02)
		error(VARREQD);
	if(getch()!=',')
		error(SYNTAX);
	i1=evalint()-1;
	if(getch()!=','){
		i2=255;
		point--;
	}
	else
		i2= evalint();
	if(i2<0 || i2>255 || i1<0 || i1>255)
		error(10);
	if(getch()!=')' )
		error(SYNTAX);
	if(getch()!='=')
		error(4);
	cursiz=0;
	if(p= ((stdatap)pt)->stpt){
		cursiz=i= *p++ & 0377;
		q=chbl1ck;
		do{
			*q++ = *p++;
		}while(--i);
	}
	if(i1>cursiz)
		i1=cursiz;
	i2+=i1;
	if(i2>cursiz)
		i2=cursiz;
	rhside= cursiz -i2;
	if(i=rhside){
		p=chbl2ck;
		q= &chbl1ck[i2];
		do{
			*p++ = *q++;
		}while(--i);
	}
	stringeval(gblock);
	check();
	if(gcursiz+rhside+i1>255)
		error(9);
	p= &chbl1ck[i1];
	q= gblock;
	if(i=gcursiz)
		do{             /* what a lot of data movement */
			*p++ = *q++;
		}while(--i);
	gcursiz+=i1;
	q=chbl2ck;
	if(i=rhside)
		do{
			*p++ = *q++;
		}while(--i);
	gcursiz+=rhside;
	p=gblock;
	q=chbl1ck;
	if(i=gcursiz)
		do{
			*p++ = *q++;
		}while(--i);
	stringassign( (stdatap)pt );    /* done it !! */
	normret;
}

#ifdef  _BLOCKED

/* mkint(a$)
 * routine to make the first 2 bytes of string into a integer
 * for use with formatted files.
 */

mkint()
{
      register short  *p = (short *)gblock;
      stringeval(gblock);
      if(gcursiz < sizeof(short) )
	      error(10);
      res.i = *p;
      vartype = 01;
}

/* ditto for string to double */

mkdouble()
{
      stringeval(gblock);
      if(gcursiz < sizeof(double) )
	      error(10);
#ifndef V6C
      res = *( (value *)gblock);
#else
      movein(gblock,&res);
#endif
      vartype = 0;
}

/*
 * mkistr$(x%)
 * convert an integer into a string for use with disk files
 */

mkistr()
{
      register short  *p = (short *)gblock;
      eval();
      if(!vartype && conv(&res))
	      error(FUNCT);
      *p = res.i;
      gcursiz = sizeof(short);
}

/* mkdstr$(x)
 * ditto for doubles.
 */

mkdstr()
{
      eval();
      if(vartype)
	      cvt(&res);
#ifndef V6C
      *((value *)gblock) = res;
#else
      movein(&res,gblock);
#endif
      gcursiz = sizeof(double);
}
#else
mkdstr(){}
mkistr(){}
mkint(){}
mkdouble(){}
#endif
