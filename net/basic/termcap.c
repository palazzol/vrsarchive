/*
 * BASIC by Phil Cockcroft
 */
#include        "bas.h"

#define         COMPILE
#include        "cursor.c"
#undef          COMPILE

/*
 * Handle the termcap file
 */
#define BUFSIZ	1024

char    *tskip();
char    *tgetstr();
char    *getenv();


#define CE      (sstrs[0])
#define DC      (sstrs[1])
#define DM      (sstrs[2])
#define DO      (sstrs[3])
#define ED      (sstrs[4])
#define EI      (sstrs[5])
#define IC      (sstrs[6])
#define IM      (sstrs[7])
#define ND      (sstrs[8])
#define tUP     (sstrs[9])
#define CL      (sstrs[10])


#define AM      (sflags[0])
#define BS      (sflags[1])
#define HC      (sflags[2])
#define NC      (sflags[3])
#define BW      (sflags[4])


char    o_CLEARSCR[33] = "\014";

set_cap()
{
	char ltcbuf[BUFSIZ];
	char    *type = getenv("TERM");
	char    *aoftspace;
	register char   *namp,*fp,**sp;
	char    sflags[5];
	char    *sstrs[11];
	char    tspace[128];
	int     i,j;

	if ( ! type || ! *type )
		type = "xx";
	if (tgetent(ltcbuf, type) != 1)
		strcpy("uk|dumb:", ltcbuf);
	aoftspace = tspace;
	namp = "ambshcncbw";
	fp = sflags;
	do {
		*fp++ = tgetflag(namp);
		namp += 2;
	} while (*namp);
	namp = "cedcdmdoedeiicimndupcl";
	sp = sstrs;
	do {
		*sp++ = tgetstr(namp, &aoftspace);
		namp += 2;
	} while (*namp);
	i = tgetnum("co");
	if(i > 0 && i < 1000)
		ter_width = i;
/*
 * now check to see if we are can use the editor. If so set up values
 */
	if( !BS /* || !BW */ || HC || NC ){
		noedit++;
		return;
	}
/*  &CE, &DC, &DM, &DO, &ED, &EI, &IC, &IM, &ND, &UP, */
	for(i= 0 ; i < 11 ; i++){
		if(!(namp = sstrs[i]) )
			j = 0;
		else
			j = strlen(namp);
		if(j > 9 ){
			if(i != 10 || j >= 33)     /* clear scr */
				j = 0;
		}
		switch(i){
		case 0: fp = o_DEOL;            /* ce */
			break;
		case 1: fp = o_DELCHAR;         /* dc */
			break;
		case 3: if(!j)
				continue;
			fp = o_DOWN2;           /* do */
			break;
		case 6: fp = o_INSCHAR;         /* ic */
			break;
		case 9: fp = o_UP;              /* up */
			break;
		case 10:                        /* clear screen */
			if(!j)
				continue;
			fp = o_CLEARSCR;        /* clear screen */
			break;
		default:
			continue;
		}
		if(!j)
			*fp = 0;
		else do {
			*fp++ = *namp++;
		} while(--j);
	}
}
