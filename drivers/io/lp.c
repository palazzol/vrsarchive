/*
 * $Header: /home/Vince/cvs/drivers/io/lp.c,v 1.3 1986-10-05 14:16:51 root Exp $
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
char lp286copyright[] = "(C) 1985 Intel Corp.";
static char lpvers[] = "@(#) lp driver $Revision: 1.3 $";
/*
 * This is a set of procedures that implement a parallel port
 * driver for xenix. 
 *
 * NOTE: Multiple parallel ports have not been tested yet.
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
#ifdef lint
#define far
#define near
#endif lint
#include "../h/param.h"
#include "../h/dir.h"		/* system directory structures */
#include "../h/user.h"		/* user structures (system)	*/
#include "../h/systm.h"		/* system */
#include "../h/conf.h"		/* system configuration */
#include "../h/i8259.h"		/* some pic commands from system */
#include "../h/buf.h"		/* buffer management def's */
#include "../h/tty.h"		/* structures & defines */
#include "../h/lp.h"		/* structures & defines */

extern nlp;			/* in config -- number of parallel ports */
extern struct lpcfg lpcfg[];	/* in config;four port addrs per 8255*/
extern struct tty lptty[];	/* in config;four port addrs per 8255*/
int lp_tmout;

/*
 * This procedure probes for an 8255 parallel port ;disables it's interrupt
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
	int port,alive;

/*
 * Much of this code is really superfluous in a 286 system because
 * there really is only one parallel port supported (because of
 * programming differences between the on board parallel port and
 * off board parallel ports).  Thus NLP must always be defined
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
	for(port=0; port<nlp; port++) {
		pdev = &lpcfg[port];
		outb(pdev->control, PT_INIT);	/* init the 8255 */
		outb(pdev->p_porta, TEST);	/* test pattern */
		if((inb(pdev->p_porta) != TEST)) {/* board not there */
			alive = 0;
			pdev->p_porta = 0;	/* zorch for future ref. */
		} else
			alive = 1;
		printf("Lp       Based %x level %d %s.\n",
			pdev->p_porta,pdev->p_level,
			alive ? "found" : "NOT found");
	}
	outb(pdev->p_portc,PT_OVERRIDE); /* we need to always have this bit
					    set for proper system operation */
	outb(pdev->control,SET_PRINTER_ACK); /* prime interrupt catcher */
}

/*
 * This procedure starts/resumes the actual output to the parallel port.
 *
 * TITLE:	lpstart
 *
 * CALL:	lpstart(tp)
 *
 * INTERFACES:	lpintr, lpwrite
 *
 * CALLS:
 *
 * History:	part of line disipline upgrade
*/
lpstart(tp)
	register struct tty *tp;
{
	struct lpcfg *pdev = &lpcfg[tp-lptty];
	int spl;
	int c;

	spl = splcli();
	if (tp->t_state & (BUSY|TIMEOUT|TTSTOP)) {
		splx(spl);
		return;
	}
	tp->t_state |= BUSY;
	if ((c = getc(&tp->t_outq)) >= 0) {
		outb(pdev->p_porta,c);		/* output the character */
		outb(pdev->control,ONSTROBE);
		outb(pdev->control,OFFSTROBE);
	} else
		tp->t_state &= ~BUSY;
	if (tp->t_state & OASLP) {
		if (tp->t_outq.c_cc <= LPLWAT) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
		}
	}
	if (tp->t_state & TTIOW) {
		if (tp->t_outq.c_cc == 0) {
			tp->t_state &= ~TTIOW;
			wakeup((caddr_t)&tp->t_oflag);
		}
	}
	splx(spl);
}

/*
 * This procedure does device dependent operations on the parallel port.
 *
 * TITLE:	lpproc
 *
 * CALL:	lpproc(tp,cmd)
 *
 * INTERFACES:	XENIX (line discipline routines)
 *
 * CALLS:	lpstart
 *
 * History:	part of line disipline upgrade
*/
lpproc(tp,cmd)
	register struct tty *tp;
	int cmd;
{
	switch (cmd) {
		case T_RFLUSH:	/* Flush input queue		*/
		case T_BLOCK:	/* Block input via stop-char	*/
		case T_UNBLOCK:	/* Unblock input via start-char	*/
		case T_BREAK:	/* Send BREAK			*/
		case T_TIME:	/* Time out is over		*/
			tp->t_state &= ~TIMEOUT;
				/* FALL THROUGH!!		*/
		case T_WFLUSH:	/* Flush output queue		*/
		case T_RESUME:	/* Resume output		*/
			tp->t_state &= ~TTSTOP;
				/* KEEP FALLING!!		*/
		case T_OUTPUT:	/* Start output			*/
			lpstart(tp);
			return;
		case T_SUSPEND:	/* Suspend output		*/
			tp->t_state |= TTSTOP;
			return;
	}
}

/*
 * This procedure opens a parallel port.
 * NOTE: multiple parallel ports have not been tested yet but should work!
 * (for 286 configuration, multiple parallel ports may not work because
 * of programming differences between on-board parallel I/O and off-
 * board parallel I/O)
 *
 * TITLE:       lpopen
 *
 * CALL:        lpopen(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:
 *
 * History:
 *	modified by Wilfried Schneider
 *	Lpopen returns EBUSY, if the error bit or the busy bit are
 *	high or, if the acknowledge bit is low.
*/
lpopen(dev,flag)
	dev_t dev;
	int flag;

{
	register int unit,stat;
	register struct tty *tp;

#ifdef DEBUG
	printf("lpopen of device %x, flag %x\n",dev,flag);
#endif
 	unit = LPUNIT(dev);
	tp = &lptty[unit];
	if (unit >= nlp || (lpcfg[unit].p_porta == 0)) {
		u.u_error = ENXIO;
		return;
	}
	if (tp->t_state & ISOPEN) {
		u.u_error = EBUSY;
		return;
	}
	outb(lpcfg[unit].control,CL_PR_ACK); /* re-enable port interrupts */
	outb(lpcfg[unit].control,SET_PRINTER_ACK);
	stat=inb(lpcfg[unit].p_portb);
	if ((stat & (PR_ACK_BAR | PR_BUSY | PR_ERROR)) != PR_ACK_BAR) {
		u.u_error = EBUSY;
		return;
	}
	tp->t_proc = lpproc;
	ttinit(tp);
	tp->t_state |= CARR_ON;
	(*linesw[tp->t_line].l_open)(dev,tp);
	if (minor(dev) & 1)		/* Kludge for compatibility	*/
		tp->t_oflag |= OLCUC;	/* Let's ditch this in R5	*/
	tp->t_oflag |= ONLCR;		/* Let's ditch this in R5 too	*/
}

/*
 * This procedure closes the parallel port.
 *
 * TITLE:       lpclose
 *
 * CALL:        lpclose(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	line discipline close
 *
 * History:
 *
*/
lpclose(dev,flag)
	dev_t dev;
	int flag;
{
	register struct tty *tp  = &lptty[LPUNIT(dev)];
#ifdef DEBUG
	printf("closing parallel port, device %x, flags %x\n",dev,flag);
#endif
	/*
	 *  We check OASLP here as a kludge to prevent the following
	 *  scenario:
	 *	1) User write blocks.
	 *	2) Output device goes offline or develops an error condition.
	 *	3) User is tired of waiting and hits delete.
	 *	4) Exit is called, which calls close.
	 *	5) Line discipline close waits for output to drain.
	*/
	if (tp->t_state & OASLP)
		while (getc(&tp->t_outq) >= 0) ;
	(*linesw[tp->t_line].l_close)(tp);
}

/*
 * This procedure preforms the users write request.
 *
 * TITLE:       lpwrite
 *
 * CALL:        lpwrite(dev);
 *
 * INTERFACES:	xenix
 *
 * CALLS:	ttwrite
 *
 * History:
 *
*/
lpwrite(dev)
	dev_t	dev;
{
	register struct tty *tp = &lptty[LPUNIT(dev)];
#ifdef DEBUG
	printf("called lpwrite on device %x\n",dev);
#endif
	(*linesw[tp->t_line].l_write)(tp);
}

/*
 * This procedure does ioctl operations on the parallel port.
 *
 * TITLE:       lpioctl
 *
 * CALL:        lpiotcl(dev,cmd,addr,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	ttiocom
 *
 * History:	Added to allow line disciplines for parallel ports to
 *		be selected (and deselected).
 *
*/
lpioctl(dev,cmd,addr,oflag)
	dev_t dev;
	int cmd;
	faddr_t addr;
	int oflag;
{
	register struct tty *tp  = &lptty[LPUNIT(dev)];
#ifdef DEBUG
	printf("ioctl for parallel port, device %x, flags %x\n",dev,flag);
#endif
	ttiocom(tp,cmd,addr,dev);
}

/*
 * This procedure resets the output device after an error.
 *
 * TITLE:	lprestart
 *
 * CALL:	lprestart(tp)
 *
 * INTERFACES:	lpintr, XENIX
 *
 * CALLS:
 *
 * History:	part of line disipline upgrade
*/
lprestart(tp)
	register struct tty *tp;
{
	register struct lpcfg *pdev;
	int stat;

	pdev = &lpcfg[tp-lptty];
	stat = inb(pdev->p_portb);
	if ((stat & (PR_ACK_BAR | PR_BUSY | PR_ERROR)) != PR_ACK_BAR) {
		printf("Lpintr: lp port %d not ready\n",tp-lptty);
		timeout(lprestart,tp,LPTIMEO);
		return;
	}
	outb(pdev->control,CL_PR_ACK); /* re-enable interrupts */
	outb(pdev->control,SET_PRINTER_ACK);
	lpstart();
}

/*
 * This procedure responds to interrupts as required, depending on the status
 * of the hardware.
 *
 * TITLE:       lpintr
 *
 * CALL:        lpintr(level)
 *
 * INTERFACES:  device interrupt
 *
 * CALLS:	lpoutput
 *
 * History: modified for 286/10 board
 *
 *	modified by Wilfried Schneider.
 *	1) If the acknowledge line is low, the interupt flip-flop
 *	can not be reset(r/s flipflop).
*/
lpintr(level)
	int level;

{
	register struct tty *tp;
	register struct lpcfg *pdev;
	int stat;

	for (pdev=lpcfg; pdev->p_level != level; pdev++);
	tp = &lptty[pdev-lpcfg];
	tp->t_state &= ~BUSY;
	stat=inb(pdev->p_portb);
	if ((stat & (PR_ACK_BAR | PR_BUSY | PR_ERROR)) != PR_ACK_BAR) {
		printf("Lpintr: lp port %d not ready\n",tp-lptty);
		timeout(lprestart,tp,LPTIMEO);
		return;
	}
	outb(pdev->control,CL_PR_ACK); /* re-enable interrupts */
	outb(pdev->control,SET_PRINTER_ACK);
#ifdef DEBUG
	printf("LP intr! level %d state %x, read status = %x\n",
		level,tp->lp_state,phstat);
#endif
	lpstart(tp);		/* Restart output */
}
