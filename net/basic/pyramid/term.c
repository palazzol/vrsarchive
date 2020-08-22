/*
 * BASIC by Phil Cockcroft
 */
/*
 * machine dependent terminal info
 */

#include "pyramid/conf.h"
#include <sgtty.h>

#ifndef SCOPE
#define SCOPE 0
#endif

struct  sgttyb  nsttyb, osttyb;
struct  tchars  ntchr,otchr;
#ifdef  UCB_NTTY
struct  ltchars nltchr,oltchr;
#endif

extern  int     ter_width;
extern  char    noedit;

static  int     got_mode;

setu_term()
{
	register i;
	char    *p, *getenv();

	ioctl(0,TIOCGETP,&osttyb);
	ioctl(0,TIOCGETC,&otchr);
	nsttyb = osttyb;
	ntchr = otchr;
#ifdef  TIOCSLPN
	osttyb.sg_length = DEFLENGTH;
	nsttyb.sg_length = 0;
	if(ter_width <= 0)
		ter_width = osttyb.sg_width & 0377;
	osttyb.sg_width = DEFPAGE;
	nsttyb.sg_width = 0;
#endif
	ntchr.t_brkc = -1;
	ntchr.t_eofc = -1;
	ntchr.t_intrc = CTRLINT;
	ntchr.t_quitc = CTRLQUIT;
	if( (p = getenv("TERM")) && strcmp(p, "ucl7009") == 0){
		ntchr.t_startc = -1;
		ntchr.t_stopc = -1;
	}
	i = osttyb.sg_flags & ( LCASE | XTABS);
	nsttyb.sg_flags = CBREAK | ANYP | i;
	osttyb.sg_flags = ECHO | ANYP | CRMOD | SCOPE | i;
#ifdef  UCB_NTTY
	ioctl(0,TIOCGLTC,&oltchr);
	nltchr = oltchr;                /* is this needed ?? */
	nltchr.t_suspc = -1;
	nltchr.t_dsuspc = -1;
	nltchr.t_rprntc = -1;
	nltchr.t_flushc = -1;
	nltchr.t_werasc = -1;
	nltchr.t_lnextc = -1;
#endif
	if(ter_width <= 0)
		ter_width=DEFPAGE;
	got_mode = 1;
}

set_term()
{
	if(noedit || !got_mode)
		return;
	ioctl(0,TIOCSETN,&nsttyb);
	ioctl(0,TIOCSETC,&ntchr);
#ifdef  UCB_NTTY
	ioctl(0,TIOCSLTC,&nltchr);
#endif
}

rset_term(type)
{

	if(noedit || !got_mode)
		return;
#ifdef  TIOCSLPN
	if(type)
		osttyb.sg_width = ter_width;
#endif
	ioctl(0,TIOCSETN,&osttyb);
	ioctl(0,TIOCSETC,&otchr);
#ifdef  UCB_NTTY
	ioctl(0,TIOCSLTC,&oltchr);
#endif
}
