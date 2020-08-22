/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"
#ifdef  V7
#include <sys/ioctl.h>
#endif

/*
 *      This file contains all the routines to implement terminal
 *    like files.
 */

/*
 *      setupfiles is called only once, it finds out how many files are
 *    required and allocates buffers for them. It will also execute
 *    'silly' programs that are given as parameters.
 */

setupfiles(argc,argv)
char    **argv;
{
	register int    fp;
	register int     nfiles=2;
	register struct filebuf *p;
	char    *q;
	extern  memp    sbrk();

#ifdef  NOEDIT
	noedit=1;
#endif
	while(argc > 1 ){
		q = *++argv;
		if(*q++ !='-')
			break;
		if(isnumber(*q)){
			nfiles= atoi(q);
			if(nfiles<0 || nfiles > MAXFILES)
				nfiles=2;
		}
		else if(*q=='x')
			noedit=1;
		else if(*q=='e')
			noedit=0;
		argc--;
	}
	filestart= sbrk(0);
	fendcore= filestart+(sizeof(struct filebuf) * nfiles);
	brk(fendcore+sizeof(xlinnumb) );        /* allocate enough core */
	for(p = (filebufp)filestart ; p < (filebufp)fendcore ; p++){
		p->filedes=0;
		p->userfiledes=0;
		p->use=0;
		p->nleft=0;
	}
		/* code added to execute silly programs */
	if(argc <= 1)
		return;
	if((fp=open(*argv,0))!=-1)
		runfile(fp);
	prints("file not found\n");
	_exit(1);
}

/*
 *      This routine executes silly programs. It has to load up
 *    the program and then simulate the environment as is usually seen
 *    in main. It works....
 */

runfile(fp)
{
	int    firsttime;
	register lpoint p;

	bsetupterm();            /* set up terminal - now done after files */
	ecore= fendcore+sizeof(xlinnumb);
	( (lpoint) fendcore )->linnumb=0;
	firsttime=1;           /* flag to say that we are just loading */
	setexit();              /* the file at the moment */
	if(ertrap)              /* setexit is the return for error */
		goto execut;    /* and execute */
	if(!firsttime)          /* an error or cntrl-c */
		quit();
	firsttime=0;
	readfi(fp);
	clear(DEFAULTSTRING);
	p= (lpoint)fendcore;
	stocurlin=p;
	if(!(curline=p->linnumb))       /* is this needed - yes */
		quit();
	point= p->lin;
	elsecount=0;
	runmode=1;                      /* go and run it */
execut:
	execute();
}

/* commands implemented are :-
	open / creat
	close
	input
	print
*/

/* syntax of commands :-
	open "filename" for input as <filedesc>
	open "filename" [for output] as <filedesc>
	close <filedesc> ,[<filedesc>]
	input #<filedesc> , v1 , v2 , v3 ....
	print #<filedesc> , v1 , v2 , v3 ....
	*/

/* format of file buffers    added 17-12-81
	struct  {
		int     filedes;        / * Unix file descriptor
		int     userfiledes;    / * name by which it is used
		int     posn;           / * position of cursor in file
		int     dev;            / * dev and inode are used to
		int     inode;          / * stop r/w to same file
		int     use;            / * r/w etc. + other info
		int     nleft;          / * number of characters in buffer
		char    buf[BLOCKSIZ];  / * the actual buffer
		} file_buffer ;

	The file_buffers are stored between the end of initialised data
      and fendcore. uses sbrk() at start up.

	At start up there are two buffer spaces allocated.
*/

/*
 *      The 'open' command it allocates file descriptors and buffer
 *    space then sets about opening the file and checking weather the
 *    the file is opened already and then checks to see if that file
 *    was opened for reading or writing.  It stops files being read and
 *    written at the same time
 */

fopen()
{
	char    chblock[256];
	register struct filebuf *p;
	register struct filebuf *q;
	register int     c;
	int     i;
	int     append=0;
	int     bl = 0;
	int     mode= _READ;
	struct  stat    inod;

	stringeval(chblock);
	chblock[gcursiz]=0;
	c=getch();
	if(c== FOR){
		c=getch();
		if(c== OUTPUT)
			mode = _WRITE;
		else if(c== APPEND){
			append++;
			mode = _WRITE;
		}
		else if(c== TERMINAL)
			mode = _TERMINAL;
		else if(c != INPUT)
			error(SYNTAX);
		c=getch();
	}
	if(c!= AS)
		error(SYNTAX);
	i=evalint();
#ifdef  _BLOCKED
	if(getch() == ','){
		bl = evalint();
		if(bl <= 0 || bl > 255)
			error(10);
	}
	else
		point--;
#endif
	check();

/* here we have mode set. i is the file descriptor 1-9
   now check to see if already allocated then allocate the descriptor
   and open file etc. */

	if(i<1 || i>MAXFILES)
		error(29);
	for(q=0,p = (filebufp)filestart ; p < (filebufp)fendcore ; p++){
		if(i== p->userfiledes)
			error(29);
		else if(!p->userfiledes && !q)
			q=p;
	}
	if(!(p=q))              /* out of file descriptors */
		error(31);

/*   code to check to see if file is open twice */

	if(stat(chblock,&inod)!= -1){
		if( (inod.st_mode & S_IFMT) == S_IFDIR)
			if(mode== _READ )  /* cannot deal with directories */
				error(15);
			else
				error(14);
		for(q = (filebufp)filestart ; q < (filebufp)fendcore ; q++)
			if(q->userfiledes && q->inodnumber== inod.st_ino &&
						q->device== inod.st_dev){
				if(mode== _READ ){
					if( q->use & mode )
						break;
					error(15);
				}
				else
					error(14);
			}
	}
	else if(mode == _TERMINAL)              /* terminals */
		error(15);
	if(mode == _READ){
		if( (p->filedes=open(chblock,0))== -1)
			error(15);
	}
	else  if(mode == _TERMINAL){
#ifdef  _BLOCKED                        /* can't block terminals */
		if(bl)
			error(15);
#endif
		if((p->filedes = open(chblock,2)) == -1)
			error(15);
		mode |= _READ | _WRITE;
	}
	else  {
		if(append){
			p->filedes=open(chblock,1);
#ifndef V6C
			lseek(p->filedes, 0L, 2);
#else
			seek(p->filedes,0,2);
#endif
		}
		if(!append || p->filedes== -1)
			if((p->filedes=creat(chblock,0644))== -1)
				error(14);
	}
	p->posn = 0;
	fstat(p->filedes,&inod);
#ifdef  V7
	ioctl(p->filedes,FIOCLEX,0);    /* close on exec */
#endif
	p->device= inod.st_dev;         /* fill in all relevent details */
	p->inodnumber= inod.st_ino;
	p->userfiledes= i;
#ifdef  _BLOCKED
	if(bl){
		p->blocksiz = bl;
		mode |= _BLOCKED;
	}
#endif
	p->nleft=0;
	p->use=mode;
	normret;
}

/*      the 'close' command it runs through the list of file descriptors
 *    and flushes all buffers and closes the file and clears all
 *    relevent entry in the structure
 */

fclosef()
{
	register struct filebuf *p;
	for(;;){
		p=getf(evalint(),(_READ | _WRITE) );
		if(p->use & _WRITE )
			f_flush(p);
		close(p->filedes);
		p->filedes=0;
		p->userfiledes=0;
		p->nleft=0;
		p->use=0;
		if(getch()!=',')
			break;
	}
	point--;
	normret;
}

/* the 'seek' command thought to be neccasary
 */

fseek()
{
	register struct filebuf *p;
	register int    j;
	register long    l;

	if(getch() != '#')
		error(SYNTAX);
	p = getf(evalint(),(_READ | _WRITE));   /* get file */
	if(getch() != ',')
		error(SYNTAX);
	eval();
	if(getch() != ',')
		error(SYNTAX);
	if(!vartype && conv(&res))
		error(FUNCT);
#ifdef  _BLOCKED
	if(p->use & _BLOCKED)
#ifndef pdp11
		l = res.i * p->blocksiz;
#else
		{ register k = 0;                 /* fast multiply for non */
		for(l = 0 ; k < 8 ; k++)             /* vax systems. this */
			if(p->blocksiz & (1<<k) )    /* won't bring in the */
				l += (long)res.i << k;  /* library */
		}
#endif
	else                    /* watch this. note the indents */
#endif                          /* it is right */
	l = res.i;
	j = evalint();
	check();
	if(j < 0 || j > 5)      /* out of range */
		error(FUNCT);
	if(p->use & _WRITE)     /* flush out all buffered output */
		f_flush(p);
	if(j >=3){
		j -= 3;
		l <<= 10;       /* blocks are 1024 */
	}
#ifndef V6C
	lseek(p->filedes, l ,j);
#else
	if(l > 512)
		seek(p->filedes, (int)(l >> 9) , j + 3);
	seek(p->filedes,(int)l & 0777 ,j);
#endif
	p->posn = 0;
	p->nleft = 0;
	p->use &= ~_EOF;
	normret;
}


/*      the 'eof' maths function eof is true if writting to the file
 *    or if the _EOF flag is set.
 */

eofl()
{
	register struct filebuf *p;

	p=getf(evalint(),(_READ | _WRITE) );
	vartype=01;
	if( p->use & ( _EOF | _WRITE) ){
		res.i = -1;
		return;
	}
	if(!p->nleft){
		p->posn = 0;
		if( (p->nleft= read(p->filedes,p->buf,BLOCKSIZ)) <= 0){
			p->nleft=0;
			p->use |= _EOF;
			res.i = -1;
			return;
		}
	}
	res.i =0;
}

/*      the 'posn' maths function returns the current 'virtual' cursor
 *    in the file. If the file descriptor is zero then the screen
 *    cursor is accessed.
 */

fposn()
{
	register struct filebuf *p;
	register i;

	i=evalint();
	vartype=01;
	if(!i){
		res.i =cursor;
		return;
	}
	p=getf(i,(_READ | _WRITE) );
	if(p->use & _WRITE)
		res.i = p->posn;
	else
		res.i = 0;
}

/*      getf() returns a pointer to a file buffer structure. with the
 *    relevent file descriptor and with the relevent access permissions
 */

struct  filebuf *
getf(i,j)
register i;     /* file descriptor */
register j;     /* access permission */
{
	register struct filebuf *p;

	if(i == 0)
		error(29);
	j &= ( _READ | _WRITE ) ;
	for(p= (filebufp)filestart ; p < (filebufp)fendcore ; p++)
		if(p->userfiledes==i && ( p->use & j) )
			return(p);
	error(29);      /* unknown file descriptor */
}

/*      flushes the file pointed to by p */

f_flush(p)
register struct filebuf *p;
{
	if(p->nleft ){
		write(p->filedes,p->buf,p->nleft);
		p->nleft=0;
	}
}

/*      will flush all files , for use in 'shell' and in quit */

flushall()
{
	register struct filebuf *p;
	for(p = (filebufp)filestart ; p < (filebufp)fendcore ; p++)
		if(p->nleft && ( p->use & _WRITE ) ){
			write(p->filedes,p->buf,p->nleft);
			p->nleft=0;
		}
}

/*      closes all files and clears the relevent bits of info
 *    used in clear and new.
 */

closeall()
{
	register struct filebuf *p;
	flushall();
	for(p= (filebufp)filestart ; p < (filebufp)fendcore ; p++)
		if(p->userfiledes){
			close(p->filedes);
			p->filedes=0;
			p->userfiledes=0;
			p->nleft=0;
			p->use=0;
		}
}

/*      write to a file , same as write in parameters (see print )
 */

putfile(p,q,i)
register struct filebuf *p;
register char   *q;
int     i;
{
	register char   *r;
	if(!i)
		return;
	r= &p->buf[p->nleft];
	do{
		if(p->nleft >= BLOCKSIZ ){
			f_flush(p);
			r= p->buf;
		}
		*r++ = *q++;
		p->nleft++;
	}while(--i);
	if(p->use & _TERMINAL)
		f_flush(p);
}

/* gets a line into q (MAX 512 or j) from file p terminating with '\n'
 * or _EOF returns number of characters read.
 */

filein(p,q,j)
register struct filebuf *p;
register char *q;
{
	register char   *r;
	register int     i=0;

	if(p->use & _TERMINAL)          /* kludge for terminal files */
		p->use &= ~_EOF;
	else if(p->use & _EOF)
		return(0);              /* end of file */
#ifdef  _BLOCKED
	if(p->use & _BLOCKED)
		j = p->blocksiz;
#endif
	r= &p->buf[p->posn];
	for(;;){
		if(!p->nleft){
			r=p->buf;
			if( (p->nleft= read(p->filedes,p->buf,BLOCKSIZ)) <=0){
				p->nleft=0;     /* a read error */
				p->use |= _EOF; /* or end of file */
				break;
			}
		}
		*q= *r++;
		p->nleft--;
		if(++i == j){
			q++;
			break;
		}
#ifdef  _BLOCKED
		if(*q++ == '\n' && !(p->use & _BLOCKED) ){
#else
		if(*q++ =='\n'){
#endif
			q--;
			break;
		}
		if(i>=512){             /* problems */
			p->posn= r - p->buf;
			error(32);
		}
	}                               /* end of for loop */
	*q=0;
	if(p->use & _TERMINAL){
		p->nleft = 0;
		p->posn = 0;
	}
	else
		p->posn = r - p->buf;
#ifdef  _BLOCKED
	if( (p->use & _BLOCKED) && j != i){
		p->use |= _EOF;
		p->nleft = 0;
		return(0);
	}
#endif
	return(i);
}
