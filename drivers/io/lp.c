/*
 * $Header: /home/Vince/cvs/drivers/io/lp.c,v 1.2 1986-10-05 14:15:57 root Exp $
 * Copyright (C) 1983 Intel Corp.
 * All rights reserved. No part of this program or publication
 * may be reproduced, transmitted, transcribed, stored in a
 * retrieval system, or translated into any language or computer
 * language, in any form or by any means, electronic, mechanical,
 * magnetic, optical, chemical, manual or otherrwise without the
 * prior written permission of Intel Corporation, 3065 Bowers
 * Avenue, Santa Clara, California, 95051.
 * Attn: Software License Administration.
 */
char lp286copyright[] = "(C) 1983 Intel Corp.";
static char lpvers[] = "@(#) lp driver $Revision: 1.2 $";
extern int lp_slo;	/* timeout for slow printers */
int lp_tmout;
/*
 * This is a set of procedures that implement a centronics line printer
 * driver for xenix. 
 *
 * NOTE: Multiple line printers have not been tested yet.
 *
 * TITLE:	lp330.c
 *
 * Written by	Jim Chorn
 * 	   on	3/4/82
 *
 * History:
 * 	modified 6/7/83 by Ken Shoemaker for xenix 286 support on
 *	286/10 board
 *
 *      modified 6/1/84 by Wilfried Schneider.
 *	Sometimes interupts were lost in the interupt routine and
 *	the interupt flip-flop could not be reset
 */

#include "../h/param.h"
#include "../h/dir.h"		/* system directory structures */
#include "../h/user.h"		/* user structures (system)	*/
#include "../h/systm.h"		/* system */
#include "../h/conf.h"		/* system configuration */
#include "../h/i8259.h"		/* some pic commands from system */
#include "../h/buf.h"		/* buffer management def's */
#include "../h/lp.h"		/* structures & defines */

struct lp_softc lps[NLP];
extern struct lpcfg lpcfg[NLP];	/* in config;four port addrs per 8255*/
struct lp_softc *lplbs;     /* only one line printer... */
struct lpcfg *lplbd;  /* others? 71 different levels is unrealistic */ 

/*
 * This procedure probes for an 8255 printer port ;disables it's interrupt
 * initializes the interface and finally figures out whether the port is valid
 *
 * TITLE:       lpinit
 *
 * CALL:        lpinit()
 *
 * INTERFACES:	dinit
 *
 * CALLS:	none
 *
 * History:
 *
 */
lpinit()
{
	register struct lpcfg *pdev;
	register struct lp_softc *sc;
	int printer,alive;

/*
 * Much of this code is really superfluous in a 286 system because
 * there really is only one line printer supported (because of
 * programming differences between the on board parallel port and
 * off board parallel printer ports).  Thus NLP must always be defined
 * either 0 or 1.  Note also the magic cookies at the end of the
 * routine.  These are required because some of the bits in port C
 * of the on board 8255 are used for other system level things. 
 * Because of this, port C should never be explicitly written to, rather
 * use the bit set/reset feature of the 8255 to strobe any line of port
 * C and leave all other bits alone.
 */

#ifdef DEBUG
	printf("lpinit... Probing\n");
#endif
	for(printer=0; printer<NLP; printer++) {
		sc = &lps[printer];
		pdev = &lpcfg[printer];
		outb(pdev->control, PT_INIT);	/* init the 8255 */
		outb(pdev->p_porta, TEST);	/* test pattern */
		if((inb(pdev->p_porta) != TEST)) /* board not there */
			alive = 0;
		else
			alive = PALIVE;
		printf("Lp       Based %x level %d %s.\n",
			pdev->p_porta,pdev->p_level,
			alive ? "found" : "NOT found");
		sc->lp_state |= alive;
		lplbs = sc;
		lplbd = pdev;
	}
	outb(pdev->p_portc,PT_OVERRIDE); /* we need to always have
	   this bit set for proper system operation */
	outb(pdev->control,SET_PRINTER_ACK); /* prime interrupt catcher */
}


/*
 * This procedure opens a line printer. 
 * NOTE: multiple line printers have not been tested yet but should work!
 *(for 286 configuration, multiple line printers may not work because
 * of programming differences between on-board parallel I/O and off-
 * board parallel I/O)
 *
 * TITLE:       lpopen
 *
 * CALL:        lpopen(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:       lp_canon
 *
 * History:
 *	modified by Wilfried Schneider
 *	Lpopen returns EBUSY, if the error bit or the busy bit are
 *	high or, if the acknowledge bit is low.
 *
 * Note that if the minor device is odd, then the printer is opened
 * for capital letters only, whereas if it is even, then both upper
 * and lower case are supported.
 */
int lp_canon();
int lptout();

lpopen(dev,flag)
	dev_t dev;
	int flag;

{
	register int unit,stat;
	register struct lp_softc *sc;

#ifdef DEBUG
	printf("lpopen of device %x, flag %x\n",dev,flag);
#endif
 	unit = LPUNIT(dev);
	sc = &lps[unit];
	if (unit >= NLP || ((sc->lp_state & PALIVE) == 0)) {
		u.u_error = ENXIO;
		return;
	}
	sc->lp_addr = &lpcfg[unit];
	if (sc->lp_state & OPEN) {
		u.u_error = EBUSY;
		return;
	}
	outb(lpcfg[unit].control,CL_PR_ACK); /* re-enable printer interrupts */
	outb(lpcfg[unit].control,SET_PRINTER_ACK);
	stat=inb(lpcfg[unit].p_portb);
	if ((stat & (PR_BUSY | PR_ERROR)) || (~stat & PR_ACK_BAR)) {
		u.u_error = EBUSY;
		return;
	}
	sc->lp_state |= OPEN;
	sc->lp_flags = minor(dev) & 0x07;
	if (minor(dev) > 1)
		lp_tmout = 0;
	else {
		lp_tmout = lp_slo;
		timeout(lptout, (caddr_t)dev, lp_tmout);
	}
	lp_canon(dev,'\f');			/* start buffering rolling */
}

/*
 * This procedure preforms the users write request. The user data is moved
 * into canons buffer. Canon expands tabs,etc. and xfers the data to the 
 * output buffer for output to the device.
 *
 * TITLE:       lpwrite
 *
 * CALL:        lpwrite(dev);
 *
 * INTERFACES:	xenix
 *
 * CALLS:	lp_canon,iomove
 *
 * History:
 *
 */

lpwrite(dev)
	dev_t	dev;

{
	register char c;
#ifdef DEBUG
	printf("called lpwrite on device %x\n",dev);
#endif
	
	while (u.u_count) {
		c = cpass(); /* get the character from user */
#ifdef DEBUG
		printf("-%x",c);
#endif
		lp_canon(dev,c); /* and pass it to buffer */
	}
}



/*
 * This procedure closes the line printer.
 *
 * TITLE:       lpclose
 *
 * CALL:        lpclose(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	lp_canon
 *
 * History:
 *
 */

lpclose(dev,flag)
	dev_t dev;
	int flag;

{
	register struct lp_softc *sc;
	
#ifdef DEBUG
	printf("closing line printer, device %x, flags %x\n",dev,flag);
#endif
	sc  = &lps[LPUNIT(dev)];
	lp_canon(dev,'\f');
	sc->lp_state &= ~OPEN;
}


/*
 * This procedure adjusts the special characters if needed.
 * A \n is replaced by '\r\n' to acommadate the centronics 
 * printer which trashes the input buffer
 * on linefeed chars and formfeed chars. A \f is replaced with '\r\n\f'
 * if the printer supports the formfeed and '\r' linesleft +1 '\n' if
 * the \f is not supported.
 *
 * TITLE:	lp_canon
 *
 * CALL:	lp_canon(dev,char)
 *
 * INTERFACES:  lpwrite,lpopen
 *
 * CALLS:	lp_outchar
 *
 * History:
 *
 */
int	lp_outchar();

lp_canon(dev,c)
	dev_t	 dev;
	register int	 c;

{
	struct lp_softc	*sc;
	register int logcol, physcol, physline;
 
	sc = &lps[LPUNIT(dev)];
	if (sc->lp_flags & CAP) {	/* special character support */

		register int	c2;

		if (c>='a' && c<='z')
			c += 'A'-'a'; else
		switch (c) {

		case '{':
			c2 = '(';
			goto esc;

		case '}':
			c2 = ')';
			goto esc;

		case '`':
			c2 = '\'';
			goto esc;

		case '|':
			c2 = '!';
			goto esc;

		case '~':
			c2 = '^';

		esc:
			lp_canon(dev,c2);	/* do adjusts; print char */
			sc->lp_logcol--;	/* simulate a \b */
			c = '-';		/* set over strike char */
		}
	}
	logcol = sc->lp_logcol;
	physcol = sc->lp_physcol;
	physline = sc->lp_phline;

	if (c == ' ')
		logcol++;
	else switch(c) {

	case '\t':
		logcol = (logcol+8) & ~7;	/* tab to tab stop */
		break;

	case '\f':
		if (sc->lp_phline == 0 && physcol == 0)
			break;		/* don't need one at tof  */
		/* fall into ... */

	case '\n':
		if (c == '\f') {		/* do a \f */
			lp_outchar(sc, '\r');	/* support no feed */
			if (sc->lp_flags & CAP) {
				physline = MAXLINE - physline -1;
				c = '\n';	/* \r \n...\n => lp */
				while(physline-- > 0)
					lp_outchar(sc, c);
			}else			/* fuzzyness is */
				lp_outchar(sc,'\n');/* \r \n \f => lp */
			sc->lp_phline = 0;
		}else{
			if(logcol > 0)		/* add \r if needed */
				lp_outchar(sc,'\r');/* \r \n => lp */
			sc->lp_phline++;
		}
		physcol = 0;
		/* fall into ... */

	case '\r':
		lp_outchar(sc,c);		/* \r => lp */
		logcol = 0;
		break;


	case '\b':
		if (logcol > 0)
			logcol--;
		break;

	default:
		if (logcol < physcol) {
			lp_outchar(sc, '\r');
			physcol = 0;
		}
		if (logcol < MAXCOL) {
			while (logcol > physcol) {
				lp_outchar(sc, ' ');
				physcol++;
			}
			lp_outchar(sc, c);
			physcol++;
		}
		logcol++;
	}
	if (logcol > MAXCOL)		/* ignore long lines  */
		logcol = MAXCOL;
	if (physline >= (MAXLINE -1))/* reset line marker for long pages */
		sc->lp_phline = 0;
	sc->lp_logcol = logcol;
	sc->lp_physcol = physcol;
}

/*
 * This procedure picks up the characters from the output
 * buffer and sends them to the device as fast as the device can handle
 * them.
 *
 * TITLE:	lp_outchar
 *
 * CALL:	lp_outchar(sc,c)
 *
 * INTERFACES:	lp_canon
 *
 * CALLS: 	lpoutput, lpintr, 
 *
 * History:
 *	modified by Wilfried Schneider.
 *	If the interupt flip-flop could not be reset in the
 *	interupt routine (ACK-line down), poll the line, until	
 *	it is high again and reset the flip-flop.
 *
 */

lp_outchar(sc,c)
	register struct lp_softc *sc;
	int	c;

{
	int	s, stat;
	register struct lpcfg *lpdev;
#ifdef DEBUG
	printf("+%x",c);
#endif
	s = SPL();
	lpdev=sc->lp_addr;
	while(sc->lp_outq.c_cc >= LPHWAT) {	/* check water marks */
		if ((sc->lp_state & RESET) == 0) {
			while ((((stat=inb(lpdev->p_portb))&PR_ACK_BAR) == 0) ||
				(stat & PR_BUSY))
				sleep((caddr_t)&lbolt,LPPRI);
			lpintr(lpdev->p_level);
		} else {
			sc->lp_state |= ASLP;
			sleep((caddr_t)sc,LPPRI);
		}
	}
/* now output chars, the condition will usually be satisfied
 * only for the first line of characters or until the first
 * timeout, after that they will go into the clist and get
 * printed by lpintr
 */
	if((sc->lp_outq.c_cc <= 0) && ((sc->lp_state & TOUT) == 0))
		lpoutput(sc->lp_addr,sc,c);
	else
		while(putc(c,&sc->lp_outq))	/* or buffered */
			sleep((caddr_t)&lbolt,LPPRI);
	splx(s);
}

/*
 * This procedure checks the status of the hardware. And flushes the printer's
 * character queue if there is something in it. It also check the watermarks.
 *
 * TITLE:       lpintr
 *
 * CALL:        lpintr(level)
 *
 * INTERFACES:  device interrupt, lp_ outchar
 *
 * CALLS:	lpoutput
 *
 * History: modified for 286/10 board
 *
 *	modified by Wilfried Schneider.
 *	1) If the acknowledge line is low, the interupt flip-flop
 *	can not be reset(r/s flipflop).  So if it is too long
 *	low it must be polled(see lp_outchar).
 *	2) The interupt routine empties the output queue.  In this time
 *	an interupt could occur. Before leaving the interupt routine
 *	the status must be checked. If the status has changed,
 *	the interupt routine is called again.
 *
 */
lpintr(level)
	int level;

{
	register struct lp_softc *sc;
	register struct lpcfg *lpdev;
	int phstat,stat,i;

	sc = lplbs;
	lpdev = lplbd;

loop:
	phstat = stat = inb(lpdev->p_portb);	/* check printer status */
	sc->lp_state &= ~RESET;
	if (lp_tmout == lp_slo) {
		if ((phstat & (PR_ACK_BAR|PR_BUSY)) == PR_ACK_BAR) {
			outb(lpdev->control,CL_PR_ACK); /* re-enable printer interrupts */
			outb(lpdev->control,SET_PRINTER_ACK);
			sc->lp_state |= RESET;
		}
	} else {
		if (phstat & PR_ACK_BAR) {
			outb(lpdev->control,CL_PR_ACK); /* re-enable printer interrupts */
			outb(lpdev->control,SET_PRINTER_ACK);
			sc->lp_state |= RESET;
		}
	}
#ifdef DEBUG
	printf("LP intr! level %d state %x, read status = %x\n",
		level,sc->lp_state,phstat);
#endif
	if ((phstat & (PR_BUSY|PR_ERROR)) || ((phstat & PR_ACK_BAR) == 0))
		sc->lp_state |= TOUT;
	else
		sc->lp_state &= ~TOUT;
	sc->lp_state |= MOD;
	while (((sc->lp_state & TOUT) == 0) && (sc->lp_outq.c_cc > 0))
		/* output char's from clist */
		lpoutput(lpdev,sc,getc(&sc->lp_outq));
	if (sc->lp_state & ASLP) { 	/* wakeup sleeping printer */
		sc->lp_state &= ~ASLP;
		wakeup((caddr_t)sc);
	}
	if (lp_tmout != lp_slo) {
		if (((phstat=inb(lpdev->p_portb)) != stat) ||
		   ((sc->lp_state&TOUT) && !(phstat&(PR_BUSY|PR_ERROR)) &&
		   (phstat&PR_ACK_BAR)))
			goto loop;
		if (phstat & PR_ACK_BAR) {
			outb(lpdev->control,CL_PR_ACK); /* re-enable printer interrupts */
			outb(lpdev->control,SET_PRINTER_ACK);
			sc->lp_state |= RESET;
		}
	}
}


/*
 * This procedure does the actual output to the line printer.
 *
 * TITLE:	lpoutput
 *
 * CALL:	lpoutput(lpdev,sc,c)
 *
 * INTERFACES:	lp_outchar, lpintr
 *
 * CALLS:
 *
 * History: modified for 286/10 board
 *
 *	modified by Wilfried Schneider. Checking the acknowlege
 *	line was not correct. (!c changed to ~c)
 *
 */

lpoutput(lpdev,sc,c)
	register struct lpcfg *lpdev;
	register struct lp_softc *sc;
	int	c;

{
	outb(lpdev->p_porta,c);		/* output the character */
	outb(lpdev->control,ONSTROBE);
	outb(lpdev->control,OFFSTROBE);
#ifdef DEBUG
	printf("$%x",c);
#endif
/* if the busy signal is long, set the timeout bit */
	c = inb(lpdev->p_portb);  /* read the printer status */
/* note that we read it twice to give the printer a little bit
   of time to respond...*/
	c = inb(lpdev->p_portb);  /* read the printer status */
	if ((c & (PR_BUSY|PR_ERROR)) || ((~c) & PR_ACK_BAR)){
#ifdef DEBUG
		printf("timing out printer, status = %x\n",c);
#endif
		sc->lp_state |= TOUT;
	}
}

lptout(dev)
dev_t dev;
{
	register struct lp_softc *sc;
	register struct lpcfg *lpdev;
	int s, stat;

	sc = lplbs;
	lpdev = lplbd;

	if ((sc->lp_state & MOD) != 0) {
		sc->lp_state &= ~MOD;
		timeout(lptout, (caddr_t)dev, lp_tmout>>2);
		return;
	}
	s = SPL();
	if (sc->lp_outq.c_cc && (sc->lp_state & TOUT) &&
	    !((stat = inb(lpdev->p_portb)) & (PR_BUSY|PR_ERROR)))
		lpintr(sc->lp_addr->p_level);
	splx(s);

	if (!sc->lp_outq.c_cc && ((sc->lp_state & OPEN) == 0))
		return;
	timeout(lptout, (caddr_t)dev, lp_tmout);
}
