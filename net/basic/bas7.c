/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

#define         COMPILE
#include        "cursor.c"
#undef          COMPILE

/*
 *     this file conatins the user interface e.g. the line editor
 */

#define PADC    0400    /* the character output for padding */
			/* more than 0377 but can still be passed to putc */

/*      read a single character */

readc()
{
	char    c=RETURN;

#ifdef  BSD42
	if(!setjmp(ecall)){
		ecalling = 1;
		if(!read(0,&c,1)){
			ecalling = 0;
			quit();
		}
		ecalling = 0;
	}
#else
	if(!read(0,&c,1))               /* reading from a pipe exit on eof */
		quit();
#endif
	return(c&0177);
}

/*      sets up the terminal structures so that the editor is in rare
 *    with no paging or line boundries and no echo
 *      Also sets up the user modes so that they are sensible when
 *    we exit. ( friendly ).
 */

setupterm()
{
	set_cap();
	setu_term();
}


/*   the actual editor pretty straight forward but.. */

edit(fl,fi,fc)
{
	register int    cursr;
	register char   *q;
	register char   *p;
	int     c;
	int     quitf=0;        /* say we have finished the edit */
	int     special;
	int     lastc;
	int     inschar =1;

	set_term();
	for(p= &line[fi]; p<= &line[MAXLIN] ;)
		*p++ = ' ';
	*p=0;
	write(1,line,fi);
	cursr=fi;
	if(noedit){
		for(p= &line[cursr];p< &line[MAXLIN] ; ){
			c=readc();
			if(c=='\n' || trapped)
				break;
			else if(c >=' ' )
				*p++ =c;
			else if(c == ESCAPE)
				break;
		}
		while(c != '\n' && c != ESCAPE && !trapped)
			c=readc();
	}
	else
	do{
		putch(0);       /* flush the buffers */
		lastc = lastch(fl);
		c=readc();
		if(c >= ' ' && c < '\177'){
			if( cursr < MAXLIN && ( inschar && lastc < MAXLIN || !inschar) ){
				if(cursr < lastc && inschar){
					p= &line[MAXLIN];
					q= p-1;
					while(p> &line[cursr])
						*--p= *--q;
					if(*o_INSCHAR)
						puts(o_INSCHAR);
					else
						inchar(cursr,lastc,c);
				}
				putch(c);
				line[cursr++]=c;
				continue;
			}
		}
		else switch( (c <' ') ? _in_char[c] : _in_char[32] ){
case    i_LEFT:
		if(cursr==fl)
			break;
		cursr--;
		puts(o_LEFT);
		continue;
case    i_CLEAR:                /* control l  - redraw  */
		puts(o_RETURN);
		cursr=lastc;
		for(p= line; p< &line[cursr];)
			putch(*p++);
		deol(cursr);
		continue;
case    i_DELLINE:              /* control b - zap line */
		if(cursr==fl && lastc == fl)
			break;
		puts(o_RETURN);
		p=line;
		while(p<&line[fl])
			putch(*p++);
		deol(cursr);
		p= &line[fl];
		while(p<&line[MAXLIN])
			*p++ = ' ';
		cursr=fl;
		continue;
case    i_DELCHAR:
		if(cursr >= lastc )
			break;
		goto rubit;
case    i_RUBOUT:
		if(cursr==fl)
			break;
		puts(o_LEFT);
		cursr--;
		if(!inschar)
			continue;
	rubit:
		if(cursr <= lastc ){
			if(*o_DELCHAR)
				puts(o_DELCHAR);
			p= &line[cursr];
			q= p+1;
			while(q < &line[MAXLIN] )
				*p++ = *q++;
			*p= ' ';
		}
		if(!*o_DELCHAR)
			delchar(cursr,lastc);
		continue;
case    i_UP:
		if(cursr-ter_width< fl)
			break;
		if(*o_UP)
			puts(o_UP);
		else for(special = 0; special < ter_width ; special++)
			puts(o_LEFT);
		cursr -= ter_width;
		continue;
case    i_DOWN1:
		if(cursr+ter_width > MAXLIN )
			break;
		puts(o_DOWN2);
		cursr+=ter_width;
		continue;
case    i_CNTRLD:
		if( (c = readc()) >= ' ' || _in_char[c] != i_CNTRLD)
			break;
		putch(0);
		cursor= (cursor+cursr)%ter_width;
		quit();
case    i_INSCHAR:
		inschar = !inschar;
		continue;
case    i_RIGHT:
		if(cursr>= MAXLIN)
			break;
		putch(line[cursr++]);
		continue;
case    i_LLEFT:
		if(cursr <= fl)
			break;
		do{
			puts(o_LEFT);
		}while(((--cursr) &07) && cursr > fl);
		continue;
case    i_RRIGHT:
		if(cursr>= MAXLIN)
			break;
		do{
			putch(line[cursr++]);
		}while((cursr&07) && cursr < MAXLIN);
		continue;
case    i_DELSOL:       /* delete to start of line */
		if(cursr==fl)
			break;
		special = cursr;
		cursr = fl;
		goto delit;     /* same code as del word almost */
case    i_DELWORD:         /* control w - del word */
		if(cursr==fl)
			break;
		special=cursr;
		do{
			cursr--;
		}while(cursr>fl &&(line[cursr-1]!=' ' || line[cursr]==' '));
	delit:
		q= &line[special];
		p= &line[cursr];
		while(q < &line[MAXLIN] )
			*p++ = *q++;
		while(p < &line[MAXLIN]){
			puts(o_LEFT);
			*p++ = ' ';
			if(*o_DELCHAR && --special <= lastc )
				puts(o_DELCHAR);
		}
		if(!*o_DELCHAR)
			delchar(cursr,lastc);
		continue;
case    i_BACKWORD:             /* back word */
		if(cursr==fl)
			break;
		do{
			puts(o_LEFT);
			cursr--;
		}while(cursr>fl && (line[cursr-1]!=' ' || line[cursr]==' ' ));
		continue;
case    i_NEXTWORD:     /* next word */
		if(cursr >= MAXLIN || cursr > lastc  || lastc == fl)
			break;
		do{
			putch(line[cursr++]);
		}while(cursr < MAXLIN && cursr <= lastc &&
			 (line[cursr]==' '|| line[cursr-1]!=' ' ) );
		continue;
case    i_DEOL:
		if(cursr >= lastc )
			break;
		for(p= &line[cursr];p < &line[MAXLIN];)
			*p++ = ' ';
		deol(cursr);
		continue;
case    i_ESCAPE:
case    i_RETURN:
case    i_DOWN2:
		while(cursr< lastc)
			putch(line[cursr++]);
		puts(o_RETURN);
		puts(o_DOWN2);
		quitf++;
		continue;
default:
		break;
		}
		puts(o_PING);
	}while(!quitf && !trapped);
	putch(0);
	line[lastch(fl)]=0;
/*   special characters are dealt with here- null is never returned */
	for(p=line,q=line,special=0;*p;p++){
		if(special){
			special=0;
			if(*p>='a' && *p<='~')
				*q++ = *p -('a'-1);
			else *q++ = *p;
		}
		else if(*p=='\\')
			special++;
		else *q++ = *p;
	}
	*q=0;
	cursor=0;
	rset_term(0);
	return(c);
}

/*
 *      put a string out ( using putch )
 */

puts(s)
register char    *s;
{
	/*
	 * now cope with padding
	 */
	if(*s >='0' && *s <= '9'){
		register i = 0;
		do{
			i = i * 10 + *s++ -'0';
		}while(*s >= '0' && *s <= '9');
		if(*s == '.')
			s++, i++;
		if(*s == '*')   /* should only affect 1 line */
			s++;
		while(i-- > 0)
			putch(PADC);
	}
	while(*s)
		putch(*s++);
}

/*      put out a character uses buffere output of up to 256 characters
 *    It used to use a static buffer but this is a waste of space so
 *    it now uses gblock as this is never used during an edit.
 *      A value of zero for the parameter will flush the buffer.
 */

putch(c)
{
	static  nleft=0;

	if(!c || nleft>=256){
		if(nleft)
			write(1,gblock,nleft);
		nleft=0;
	}
	if(!c)
		return;
	gblock[nleft++]= c;
}

/*      lastch() returns the last character on the line used in the
 *    editor to see if any more characters can be placed on the line and
 *    by the redraw key.
 */

lastch(f)
{
	register char   *p;
	register char   *q;
	p= &line[f];
	q= &line[MAXLIN];
	while(*q==' ' && q>=p)
		q--;
	return(q-line+1);
}

/* delete from current cursor position to end of line. */

deol(cursr)
{
	register cc,i;
	if(*o_DEOL){
		puts(o_DEOL);
		return;
	}
	i = ter_width - (cursr % ter_width);
	for(cc = i ; cc ; cc--)
		putch(' ');
	for(; i ; i--)
		puts(o_LEFT);
}

/* delete nchar characters from cursr */

delchar(cursr,lc)
{
	register char   *p;
	register char   *q;
	p = &line[cursr];
	q = &line[lc];
	while(p < q )
		putch(*p++);
	q = &line[cursr];
	while(p > q ){
		if( *o_UP && p - q > ter_width ){
			puts(o_UP);
			p -= ter_width;
		}
		else {
			p--;
			puts(o_LEFT);
		}
	}
}

/* display a new character */

inchar(cursr,lastc,c)
{
	register char   *p,*q;
	p = &line[cursr+1];
	q = &line[lastc+1];
	putch(c);
	while(p < q)
		putch(*p++);
	q = &line[cursr];
	while(p > q ){
		if( *o_UP && p - q > ter_width ){
			puts(o_UP);
			p -= ter_width;
		}
		else {
			p--;
			puts(o_LEFT);
		}
	}
}
