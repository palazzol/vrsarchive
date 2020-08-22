/*
 * BASIC by Phil Cockcroft
 */
/*
 * machine dependent terminal interface
 */

#include "pdp11/conf.h"
#ifdef  V7
#include <sgtty.h>
#endif

#ifndef V7

struct  term {                  /* the structure for the terms */
	char    _j[4];          /* system call */
	int     flags;          /* most of it is not needed */
	char    __j[4];
	char    width,length;
	int     ___j[8];
	} nterm, oterm;

#else

#ifndef SCOPE
#define SCOPE   0
#endif

#ifdef  TIOCOSTP
#undef  TIOCSLPN
#endif

#ifdef  TIOCSLPN
struct  lsgttyb osttyb,nsttyb;
#undef  TIOCGETP
#undef  TIOCSETN
#define TIOCGETP        TIOCGLPG
#define TIOCSETN        TIOCSLPN
#else
struct  sgttyb  osttyb,nsttyb;
#endif
struct  tchars  ntchr,otchr;
#ifdef  UCB_NTTY
struct  ltchars nltchr,oltchr;
#endif

#endif

extern  int     ter_width;
extern  char    noedit;

static  int     got_mode;

setu_term()
{
	register i;
#ifdef  V7
	char    *p, *getenv();

	p = getenv("TERM");
	ioctl(0,TIOCGETP,&osttyb);
	nsttyb=osttyb;
#ifdef  TIOCSLPN
	osttyb.lsg_length = DEFLENGTH;
	nsttyb.lsg_length = 0;
	if(ter_width <= 0)
		ter_width = osttyb.lsg_width & 0377;
	osttyb.lsg_width = DEFPAGE;
	nsttyb.lsg_width = 0;
#endif
#ifdef  TIOCOSTP
	osttyb.sg_length = DEFLENGTH;
	nsttyb.sg_length = 0;
	if(ter_width <= 0)
		ter_width = osttyb.sg_width & 0377;
	osttyb.sg_width = DEFPAGE;
	nsttyb.sg_width = 0;
#endif
	ioctl(0,TIOCGETC,&otchr);
	ntchr = otchr;                  /* do we need this ??? */
	if(p && strcmp(p, "ucl7009") == 0){
		ntchr.t_startc = -1;
		ntchr.t_stopc = -1;
	}
	ntchr.t_brkc = -1;
	ntchr.t_eofc = -1;
	ntchr.t_intrc = CTRLINT;
	ntchr.t_quitc = CTRLQUIT;
#ifdef  TIOCSLPN
	i = osttyb.lsg_flags & ( LCASE | XTABS);
	nsttyb.lsg_flags = CBREAK | ANYP | i;
	osttyb.lsg_flags = ECHO | ANYP | CRMOD | SCOPE | i;
#else
	i = osttyb.sg_flags & ( LCASE | XTABS);
	nsttyb.sg_flags = CBREAK | ANYP | i;
	osttyb.sg_flags = ECHO | ANYP | CRMOD | SCOPE | i;
#endif

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
#else
	terms(0,('t'<<8)+2,&oterm);
#ifndef V6C
	nterm = oterm;
#else
	terms(0,('t'<<8)+2,&nterm);
#endif
	nterm.width=0;
	nterm.length=0;
	i= oterm.flags & 04;
	nterm.flags= 040340 |i;
	if(ter_width <= 0)
		ter_width = oterm.width & 0377;
	oterm.width=0;
	oterm.length=DEFLENGTH;
	oterm.flags= 0730 | i;
#endif
	if(ter_width <= 0)
		ter_width=DEFPAGE;
	got_mode = 1;
}

set_term()
{
	if(noedit || !got_mode)
		return;
#ifdef  V7
	ioctl(0,TIOCSETN,&nsttyb);
	ioctl(0,TIOCSETC,&ntchr);
#ifdef  UCB_NTTY
	ioctl(0,TIOCSLTC,&nltchr);
#endif
#else
	terms(0,('t'<<8)+3,&nterm);
#endif
}

rset_term(type)
{

	if(noedit || !got_mode)
		return;
#ifdef  V7
#ifdef  TIOCOSTP
	if(type)
		osttyb.sg_width=ter_width;
#endif
#ifdef  TIOCSLPN
	if(type)
		osttyb.lsg_width=ter_width;
#endif
	ioctl(0,TIOCSETN,&osttyb);
	ioctl(0,TIOCSETC,&otchr);
#ifdef  UCB_NTTY
	ioctl(0,TIOCSLTC,&oltchr);
#endif
#else
	if(type)
		oterm.width=ter_width;
	terms(0,('t'<<8)+3,&oterm);     /* reset terminal modes */
#endif
}
