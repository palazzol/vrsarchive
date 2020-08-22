/*
 * BASIC by Phil Cockcroft
 */
/*
 * terminal specific configuration routines for 68000's
 */
#include "m68000/conf.h"
#include <sgtty.h>

struct  sgttyb  osttyb,nsttyb;
struct  tchars  otchr,ntchr;

extern  int     ter_width;
extern  char    noedit;

#ifndef SCOPE
#define SCOPE   0
#endif

#ifdef  MCBREAK
static  char    doneset;
#endif

static  int     got_mode;

setu_term()
{
	register i;
	char    *p, *getenv();

	p = getenv("TERM");

	ioctl(0,TIOCGETP,&osttyb);
	nsttyb=osttyb;
	ioctl(0,TIOCGETC,&otchr);
	ntchr = otchr;                  /* do we need this ??? */
	ntchr.t_brkc = -1;
	ntchr.t_eofc = -1;
	ntchr.t_intrc = CTRLINT;
	ntchr.t_quitc = CTRLQUIT;
	if(p && strcmp(p, "ucl7009") == 0){
		ntchr.t_startc = -1;
		ntchr.t_stopc = -1;
	}
	i = osttyb.sg_flags & ( LCASE | XTABS);
#ifdef  MCBREAK
	nsttyb.sg_flags = CBREAK | ANYP | CRMOD | i;
#else
	nsttyb.sg_flags = CBREAK | ANYP | i;
#endif
	osttyb.sg_flags = ECHO | ANYP | CRMOD | SCOPE | i;
	if(ter_width <= 0)
		ter_width=DEFPAGE;
	got_mode = 1;
}

set_term()
{
	if(noedit || !got_mode)
		return;
#ifdef  MCBREAK
	if(doneset)
		return;
	doneset = 1;
#endif
	ioctl(0,TIOCSETN,&nsttyb);
	ioctl(0,TIOCSETC,&ntchr);
}

rset_term(type)
{
	if(noedit || !got_mode)
		return;
#ifdef  MCBREAK
	if(!type){
		/* in editing loop */
		if(doneset)
			return;
	} else
		doneset = 0;
#endif
	ioctl(0,TIOCSETN,&osttyb);
	ioctl(0,TIOCSETC,&otchr);
}
