/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *  This file contains the routines to get a variable from its name
 *  To dimension arrays and assignment to a variable.
 *
 *      A variable name consists of a letter followed by an optional
 *    letter or digit followed by the type specifier.
 *      A type specifier is a '%' for an integer a '$' for a string
 *    or is absent if the variable is a real ( Default ).
 *      An integer variable also has the top bit of its second letter
 *    set this is used to distinguish between real and integer variables.
 *      A variable name can be optionally followed by a subscript
 *    turning the variable into a subscripted variable.
 *    A subscript is specified by a list of indexes in square brackets
 *    e.g.  [1,2,3] , a maximum of three subscripts may be used.
 *    All arrays must be specified before use.
 *
 *      The variable to be accessed has its name in the array nm[],
 *    and its type in the variable 'vartype'.
 *
 *      'vartype' is very important as it is used all over the place
 *
 *      The value in 'vartype' can have the following values:-
 *              0:      real variable (Default ).
 *              1:      integer variable.
 *              2:      string variable.
 *
 */

#ifdef  V6
#define LBRACK  '['
#define RBRACK  ']'
#else
#define LBRACK  '('
#define RBRACK  ')'
#endif

/*
 * getnm will return with nm[] and vartype set appropriately but without
 * any regard for subscript parameters. Called by dimensio() only.
 */

getnm()
{
#ifdef  LNAMES
	register char   *p,*q;
	register struct entry   *ep,*np;
	register int    c;
	register int    l;
	nam[0]=c=getch();
	if(!isletter(c))
		error(VARREQD);
	p = &nam[1];
	for(l =c,c = *point;isletter(c) || isnumber(c) ; c = *++point)
		if(p < &nam[MAXNAME-1] ){
			l +=c;
			*p++ = c;
		}
	*p = 0;
	for(np = 0,ep=hshtab[l%HSHTABSIZ]; ep ; np = ep,ep=ep->link)
		if(l == ep->ln_hash)
			for(p = ep->_name,q = nam ; *q == *p++ ; )
				if(!*q++)
					goto got;
	ep = (struct entry *)xpand(&enames,sizeof(struct entry));
	if(!np)
		hshtab[l%HSHTABSIZ] = ep;
	else
		np->link = ep;
	for(p = ep->_name , q = nam ; *p++ = *q++ ; );
	ep->ln_hash = l;
got:
	nm = (char *)ep - estring;
#else
	register int    c;
	nm=c=getch();
	if(!isletter(c))
		error(VARREQD);
	c= *point;
	if(isletter(c) ||isnumber(c)){
		nm |= c<<8;
		do
			c= *++point;
		while(isletter(c) || isnumber(c));
	}
#endif
	vartype=0;
	if(c=='$'){
		point++;
		vartype=02;
	}
	else if(c=='%'){
		point++;
		vartype++;
		nm |=0200<<8;
	}
}

/*
 *      getname() will return a pointer to a variable with vartype
 *    set to the correct type. If the variable is subscripted getarray
 *    is called and the subscripts are evaluated and depending upon
 *    the type of variable the index into that array is returned.
 *      Any simple variable that is not already declared is defined
 *    and has a value of 0 or null (for strings) assigned to it.
 *      In all instances a valid pointer is returned.
 */
memp getname()
{
	memp    getstring();
#ifdef  LNAMES
	register char   *p,*q;
	register struct entry   *ep;
	register int    c;
	register struct vardata *pt;
	struct  entry   *np;
	register int    l;
	nam[0]=c=getch();
	if(!isletter(c))
		error(VARREQD);
	p = &nam[1];
	for(l =c,c = *point;isletter(c) || isnumber(c) ; c = *++point)
		if(p < &nam[MAXNAME-1] ){
			l +=c;
			*p++ = c;
		}
	*p = 0;
	for(np = 0,ep=hshtab[l%HSHTABSIZ]; ep ; np = ep,ep=ep->link)
		if(l == ep->ln_hash)
			for(p = ep->_name,q = nam ; *q == *p++ ; )
				if(!*q++)
					goto got;
	ep = (struct entry *)xpand(&enames,sizeof(struct entry));
	if(!np)
		hshtab[l%HSHTABSIZ] = ep;
	else
		np->link = ep;
	for(p = ep->_name ,q = nam ; *p++ = *q++ ; );
	ep->ln_hash = l;
got:
	nm = (char *)ep - estring;
#else
	register int    c;
	register struct vardata *pt;

	nm=c=getch();
	if(!isletter(c))
		error(VARREQD);
	c= *point;
	if(isletter(c) ||isnumber(c)){
		nm |=c<<8;
		do{ c= *++point; }while(isletter(c) || isnumber(c));
	}
#endif
	vartype=0;
	if(c=='$'){
		vartype=02;
		if(*++point==LBRACK)
			getarray();
		return(getstring());
	}
	else if(c=='%'){
		point++;
		vartype++;
		nm |= 0200<<8;
	}
	if(*point==LBRACK)
		return( (memp) getarray());
#ifdef  LNAMES
	/*
	 * now do hashing of the variables
	 */
	if( (c = varshash[l % HSHTABSIZ]) >= 0){
		pt = (vardp)earray;
		for(pt += c; pt < (vardp) vend;pt++)
			if(pt->nam ==nm )
				return( (memp) &pt->dt);
		/*
		 * not found ****
		 */
	}
	/*
	 * really look for it - will force varshash to be the lowest
	 * value. The hassle of chaining.
	 */
	if(chained)
		for(pt = (vardp)earray; pt < (vardp) vend;pt++)
			if(pt->nam ==nm ){
				varshash[l % HSHTABSIZ] = pt - (vardp)earray;
				return((memp) &pt->dt);
			}
		/*
		 * not found ****
		 */
	pt= (vardp) xpand(&vend,sizeof(struct vardata));
	if(c < 0)
		varshash[l % HSHTABSIZ] = pt - (vardp)earray;
#else
	for(pt = (vardp)earray;  pt < (vardp) vend;pt++)
		if(pt->nam ==nm )
			return( (memp) &pt->dt);
	pt= (vardp) xpand(&vend,sizeof(struct vardata));
#endif
	pt->nam=nm;
	return( (memp) &pt->dt);
}

/*
 *      getstring() returns a pointer to a string structure if the string
 *    is not declared then it is defined.
 */

memp
getstring()
{
	register struct stdata  *p;
	vartype=02;
	for(p= (stdatap)estdt ; p < (stdatap)estring ; p++)
		if(p->snam == nm )
			return( (memp) p);
	if( estdt - sizeof(struct stdata) < eostring){
		garbage();
		if(estdt - sizeof(struct stdata) <eostring)
			error(OUTOFSTRINGSPACE);
	}
	p = (stdatap)estdt;
	--p; estdt = (memp)p;
	p->snam = nm;
	p->stpt=0;
	return( (memp) p);
}

/*
 *      getarray() evaluates the subscripts of an array and the tries
 *    to access it. getarray() returns different things dependent
 *    on the type of variable. For an integer or real then the pointer to
 *    the element of the array is returned.
 *      For a string array element then the nm[] array is filled out
 *    with a unique number and then getstring() is called to access it.
 *      The variable hash (in the strarr structure ) is used as the
 *    offset to the next array if the array is real or integer, but
 *    is the base for the unique number to access the string structure.
 *
 *      This is a piece of 'hairy' codeing.
 */

getarray()
{
	register struct strarr  *p;
	register int     l;
	short   *m;
	int     c;
	int     i=1;
	register int     j=0;
	char    vty;
#ifdef  LNAMES
	memp    savee;
#endif

	point++;
	vty=vartype;
	if(vty==02){
		for(p= (strarrp) edefns ; p < (strarrp) estarr ; p++)
			if(p->snm ==nm )
				goto got;
	}
	else {
		for( p = (strarrp) estarr ; p < (strarrp)earray ;
					p = (strarrp)((memp)p + p->hash) )
			if(p->snm ==nm )
				goto got;
	}
	error(19);
got:    m = p->dim;
	i=1;
	do{
#ifdef  LNAMES
		savee = edefns;
#endif
		l=evalint()-baseval;
#ifdef  LNAMES
		p = (strarrp)((memp)p + (edefns - savee));
#endif
		if(l >= *m || l <0)
			error(17);
		j= l + j * *m;
		if((c=getch())!=',')
			break;
		m++,i++;
	} while(i <= p->dimens);
	if(i!=p->dimens || c!=RBRACK)
		error(16);
	vartype=vty;
	if(vty==02){
		j += p->hash;
		j |= 0100000;
		nm = j;
	}
	else {
		j <<= (vty ? 1 : 3 );
		p++;
		return( (int) ((char *)p+j) );
	}
}

/*
 *      dimensio() executes the dim command. It sets up the strarr structure
 *    as needed. If the array is a string array then only the structure
 *    is filled in. This means that elements of a string array do not have
 *    storage allocated until assigned to. If the array is real or integer
 *    then the array is allocated space as well as the strarr array.
 *      This is why the hash element is needed so as to be able to access
 *    the next array.
 */


dimensio()
{
	int     dims[3];
	int     nmm;
	long    j;
	int     c;
	char    vty;
	register int     i;
	register int    *r;
	register struct strarr *p;
for(;;){
	r=dims;
	i=0;
	j=1;
	getnm();
	nmm = nm;
	vty=vartype;            /* save copy of type of array */
	if(*point++!=LBRACK)
		error(SYNTAX);
	do{
		*r=evalint() + 1 - baseval;
#ifndef pdp11
		if( (j *= *r) <= 0 || j > 32767)
#else
		if( (j=dimmul( (int)j , *r)) <= 0)
#endif
			error(17);
		if((c=getch())!=',')
			break;
		r++;i++;
	}while(i<3);
	if(i ==3 || c!=RBRACK)
		error(16);
	i++;
	if(vty== 02){
		for(p= (strarrp) edefns ;p < (strarrp) estarr;p++)
			if(p->snm == nmm )
				error(20);
		if(j+shash > 32767)
			error(17);
		p = (strarrp) xpand(&estarr,sizeof(struct strarr));
		p->hash= shash;
		shash+=j;
	}
	else   {
		for(p = (strarrp)estarr ; p < (strarrp)earray ;
					p = (strarrp)((memp)p + p->hash) )
			if(p->snm == nmm )
				error(20);
		j<<= (vty ? 1 : 3);
		j += sizeof(struct strarr);
#ifdef  ALIGN4
		j = (j + 3) & ~03;
#endif
		if(nospace(j))
			error(17);
		p = (strarrp) xpand(&earray,(int)j);
		p->hash = j;    /* offset to next array */
	}
	p->snm = nmm;       /* fill in common stuff */
	p->dimens=i;
	p->dim[0]=dims[0];
	p->dim[1]=dims[1];
	p->dim[2]=dims[2];
	if(getch()!=',')        /* any more arrays */
		break;
	}
	point--;
	normret;
}

/*
 *      Assign() is called if there is no keyword at the start of a
 *    statement ( Default assignment statement ) and by let.
 *    it just calls the relevent evaluation routine and leaves all the
 *    hard work to stringassign() and putin() to actualy assign the variables.
 */

assign()
{
	register memp   p;
	register char   vty;
	register int    c;
	int     i;
	value   t1;
	extern  int     (*mbin[])();
#ifdef  LNAMES
	memp    savee;
#endif

	p= getname();
	vty=vartype;
	if(vty==02){
		if(getch()!='=')
			error(4);
		stringeval(gblock);
		stringassign( (stdatap)p );
		return;
	}
#ifdef  LNAMES
	savee = edefns;
#endif
	if((c = getch()) != '='){
		i = 6;
		switch(c){
		default:
			error(4);
		case '*':
		case '/':
			i += 2;
			break;
		case '+':
		case '-':
			break;
		}
		if(*point++ != '=')
			error(4);
#ifndef V6C
		t1 = *((value *)p);
#else
		movein(p,&t1);
#endif
		eval();
		if(vty != vartype){
			if(vty)
				cvt(&t1);
			else
				cvt(&res);
			vartype = 0;
		}
		(*mbin[i+vartype])(&t1,&res,c);
	}
	else
		eval();
#ifdef  LNAMES
	/*
	 * cope with adding new names - pushes space up
	 */
	p += edefns - savee;
#endif
	putin(p,vty);
}
