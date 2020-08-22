/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

/*
 *      This file contains the routines for input and read since they
 *    do almost the same they can use a lot of common code.
 */

/*
 *      input can have a text string, which it outputs as a prompt
 *    instead of the usual '?'. If input is from a file this
 *    facility is not permitted ( what use anyway ? ).
 *
 *      added 28-oct-81
 */

input()
{
	register char   *p;
	register int    i;
	memp    l;
	register filebufp infile=0;
	char    lblock[512];
	int     firsttime=0;
	int     c;
	char    vty;
	char    *getstrdt(),*getdata();

	c=getch();
	if(c=='"'){
		i=0;
		p=line;
		while(*point && *point != '"'){
			*p++ = *point++;
			i++;
		}
		if(*point)
			point++;
		if(getch()!=';')
			error(SYNTAX);
		*p=0;
		firsttime++;
	}
	else if(c=='#'){
		i=evalint();
		if(getch()!=',')
			error(SYNTAX);
		infile=getf(i,_READ);
	}
	else
		point--;
	l=getname();
	vty=vartype;
for(;;){
	if(!infile){
		if(!firsttime){
			*line='?';
			i=1;
		}
		firsttime=0;
		edit(i,i,i);
		if(trapped){
			point=savepoint; /* restore point to start of in. */
			return(-1);     /* will trap at start of this in. */
		}
		bstrcpy(&line[i],lblock);
	}
	else if(! filein(infile,lblock,512) )
		error(30);
	p= lblock;
ex3:    while(*p++ ==' ');      /* ignore leading spaces */
	if(!*--p && vty!=02)
		continue;
	p= ((vty==02)?(getstrdt(p)) :( getdata(p)));
	if(p){
		while(*p++ == ' ');
		p--;
	}
	if(!p || (*p!=',' && *p)){
		if(infile)
			error(26);
		prints("Bad data redo\n");
		continue;
	}
	if(vartype == 02)
		stringassign( (stdatap)l );
	else
		putin(l,vty);
	if(getch()!=',')
		break;
	l=getname();
	vty=vartype;
	if(*p==','){
		p++;
		goto ex3;
	}
	}
	point--;
	normret;
}

/* valid types for string input :-
 * open quote followed by any character until another quote or the end of line
 * no quote followed by a sequence of characters except a quote
 * terminated by a comma (or end of line).
 */

/*      the next two routines return zero on error and a pointer to
 *    rest of string on success.
 */

/*      read string data routine */

char    *
getstrdt(p)
register char   *p;
{
	register char *q;
	register int    cursiz=0;
	char    charac;

	q=gblock;
	if(*p=='"' || *p=='`' ){
		charac= *p++;
		while(*p!= charac && *p ){
			*q++ = *p++;
			if(++cursiz>255)
				return(0);
		}
		if(*p)
			p++;
		gcursiz=cursiz;
		return(p);
	}
	while( *p && *p!=',' && *p!='"' && *p!='`'){
		*q++ = *p++;
		if(++cursiz>255)
			return(0);
	}
	gcursiz=cursiz;
	return(p);
}

/*      read number routine */

char    *
getdata(p)
register char   *p;
{
	register char    *tmp;
	register int     minus=0;
	if(*p=='-'){
		p++;
		minus++;
	}
	if(!isnumber(*p) && *p!='.')
		return(0);
	tmp=point;
	point=p;
	if(!getop()){
		point=tmp;
		return(0);
	}
	p=point;
	point=tmp;
	if(minus)
		negate();
	return(p);
}

/* input a whole line of text (into a string ) */

linput()
{

	register char   *p;
	register int    i;
	memp    l;
	register filebufp infile;
	char    lblock[512];
	int     c;

	c=getch();
	if(c=='#'){
		i=evalint();
		if(getch()!=',')
			error(SYNTAX);
		infile=getf(i,_READ);
		l=getname();
		if(vartype!=02)
			error(VARREQD);
		check();
		if(!(i= filein(infile,lblock,512)) )
			error(30);
		if(i>255)
			error(9);
		p=bstrcpy(lblock,gblock);
	}
	else {
		if(c=='"'){
			i=0;
			p=line;
			while(*point && *point != '"'){
				*p++ = *point++;
				i++;
			}
			if(*point)
				point++;
			if(getch()!=';')
				error(SYNTAX);
			*p=0;
		}
		else {
			point--;
			*line='?';
			i=1;
		}
		l=getname();
		if(vartype!=02)
			error(VARREQD);
		check();
		edit(i,i,i);
		if(trapped){
			point=savepoint; /* restore point to start of in. */
			return(-1);     /* will trap at start of this in. */
		}
		p=bstrcpy(&line[i],gblock);
	}
	gcursiz= p-gblock;
	stringassign( (stdatap)l );
	normret;
}

/* read added 3-12-81 */

/*
 * Read routine this should :-
 *      get variable then search for data then assign it
 *      repeating until end of command
 *              ( The easy bit. )
 */

/*
 * Getting data :-
 *      if the data pointer points to anywhere then it points to a line
 *      to a point where getch would get an end of line or the next data item
 *      at the end of a line a null string must be implemented as
 *      a pair of quotes i.e. "" , on inputing data '"'`s are significant
 *      this is no problem normally .
 *      If the read routine finds an end of line then there is bad data
 *
 */

readd()
{
	register memp   l;
	register char   *p;
	register char    vty;
	if(!datapoint)
		getmore();
	for(;;){
		l=getname();
		vty=vartype;
		p= datapoint;
		while(*p++ == ' ');
		datapoint= --p;
		if(!*p){
			getmore();
			p=datapoint;
			while(*p++ ==' ');
			p--;
		}
	/* get here the next thing should be a data item or an error */
		datapoint=p;
		if(!*p)
			error(BADDATA);
		p= ((vty==02)?(getstrdt(p)) :( getdata(p)));
		if(!p)
			error(BADDATA);
		while(*p++ == ' ');
		p--;
		if(*p!=',' && *p)
			error(BADDATA);
		if(vty == 02)
			stringassign( (stdatap)l );
		else  putin(l,vty);
		if(*p)
			p++;
		datapoint=p;
		if(getch()!=',')
			break;
	}
	point--;
	normret;
}

/*
 * This is only called when datapoint is at the end of the line
 * it is also called if datapoint is zero e.g. when this is the first call
 * to read.
 */

getmore()
{
	register lpoint p;
	register char   *q;
	if(!datapoint)
		p = (lpoint)fendcore;
	else {
		p=datastolin;
		if(p->linnumb)
			p = (lpoint)((memp)p + lenv(p));
	}
	for(;p->linnumb; p = (lpoint)((memp)p + lenv(p)) ){
		q=p->lin;
		while(*q++ == ' ');
		if(*--q == (char)DATA){
			datapoint= ++q;
			datastolin=p;
			return;
		}
	}
	datastolin=p;
	error(OUTOFDATA);
}

/*      the 'data' command it just checks things and sets up pointers
 *    as neccasary.
 */

dodata()
{
	register char    *p;
	if(runmode){
		p=stocurlin->lin;
		while(*p++ ==' ');
		if(*--p != (char) DATA)
			error(BADDATA);
		if(!datapoint){
			datastolin= stocurlin;
			datapoint= ++p;
		}
	}
	return(GTO);    /* ignore rest of line */
}

/*      the 'restore' command , will reset the data pointer to
 *     the first bit of data it finds or to the start of the program
 *     if it doesn't find any. It will start searching from a line if
 *     tthat line is given as an optional parameter
 */

restore()
{
	register unsigned i;
	register lpoint p;
	register char   *q;

	i=getlin();
	check();
	p= (lpoint)fendcore;
	if(i!= (unsigned)(-1) ){
		for(;p->linnumb; p = (lpoint)( (memp)p + lenv(p)) )
			if(p->linnumb== i)
				goto got;
		error(6);
	}
got:    datapoint=0;
	for(;p->linnumb; p = (lpoint)((memp)p + lenv(p)) ){
		q= p->lin;
		while(*q++ ==' ');
		if(*--q == (char)DATA){
			datapoint= ++q;
			break;
		}
	}
	datastolin= p;
	normret;
}
