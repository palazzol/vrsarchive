/*	Copyright (c) 1984, 1987 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license agreement
 *	or nondisclosure agreement with Intel Corporation and may not be
 *	copied nor disclosed except in accordance with the terms of that
 *	agreement.
 *
 *	Copyright 1983, 1986, 1987 Intel Corporation
*/

#ident  "@(#)i546.c	1.13 - 87/03/20"
/************************************************************************
 *
 *	Name:
 *		iSBC546 device driver for UNIX V.3 on `386
 *
 *	Purpose:
 *
 *		This is the set of routines that constitute the device
 *		for the iSBC 546 Intelligent Serial Interface Controller.
 *      It just happens to also work on the 188, 547, and 548
 *      async controllers.
 *
 *	Edit History:
 *		rickb	Original Development
 *      	radams	21 Mar 86
 *		modified for UNIX V.3
 *      I000	radams	9 Nov 86
 *		Make initial line parameters match manual.  'Implementation
 *		dependant' t_cflag made to match System V defaults.
 *      I001	radams	9 Nov 86
 *		Fix a bug restarting output after TIMEOUT.
 *      I002	radams	9 Nov 86
 *		Move process wakeup into start, where t_outq is actually
 *		drained.
 *      I003	radams	9 Nov 86
 *		Don't drop DTR on carrier loss -- let close do it.
 *      I004	radams	9 Nov 86
 *		Remove extraneous code to clear buffer.
 *      I005	vrs	11 Nov 86
 *		Make BUSY truly reflect the state of the board for ttywait().
 *		Implement T_RFLUSH and T_WFLUSH operations.  These rely on
 *		at most one pending OUTPUT per board, so fix up i546start().
 *		This also obsoletes ln->l_ocnt, so just use BUSY everywhere.
 *	I006	radams	14 Nov 86
 *		Add TTIXON and fixed T_UNBLOCK so it would start up either
 *		the board or the terminal.  Fixes INSTOP hang problem.
 *		In general, the relation between the 546 onboard queue
 *		and the clist is wrong -- inbound flow control is poorly done.
 *		There should be a T_INPUT call in the ttread routine.
 *		(You listening AT&T?).
 *	I007	radams  14 Nov 86
 *		There was a sleep in i546param waiting for BUSY.  This
 *		caused the ioctl TCSETA to sleep when it shouldn't.  Fix
 *		was to remove the sleep from i546param.
 *	I008	radams  14 Nov 86
 *		Misc bugs.  Call to l_input with old params.  Caused
 *		SIGINTs not to happen all the time.  Added FNDELAY to
 *		open (been reading the SVID).  Setting of CARR_ON moved
 *		to intr routine and open waits for state to change.
 *		If IXANY, the starting char is not thrown away unless it's
 *		a CSTART or a CSTOP (I looked in iu.c).
 *	I009	radams 19 Nov 86
 *		Added two stop bit setting in param().
 *	I010	radams 21 Nov 86
 *		Force CARR_ON when FNDELAY is set for an open.  This is
 *		because the ttxxx routines will not allow writes unless
 *		CARR_ON is on.
 *      I011	vrs	30 Nov 86
 *		Modify I005 to always use the entire buffer at l_oba.  We
 *		can do this since no other output can be in the output buffer.
 *		This gives a slight performance gain, and cleans up the code
 *		to boot (ln->l_opb now unreferenced).
 *      I012	vrs	02 Dec 86
 *		The 54X boards report output complete when they have delivered
 *		the last character to the USART.  The USART has a two character
 *		buffer, so that it is not safe to reprogram it (i546param)
 *		until two full character times after BUSY goes clear.  I added
 *		a hack in i546ioctl() to insure two character times have
 *		elapsed before calling i546param().  I also changed the name
 *		of i546tty to i546_tty to adhere to the new naming conventions.
 *	I013	radams	02 Dec 86
 *		Removed a indent level in i546param().  Change i546param()
 *		to set CARR_ON if the line is CLOCAL.  Code in open() fixed
 *		to set CARR_ON differently and do FNDELAY correctly (redoes
 *		some of I008 and I010).  Added CARRF to l_state to track
 *		real carrier state.  Macro SETCARR(tp) checks CLOCAL and CARRF
 *		to set the correct value of CARR_ON.
 *	I014	vrs	03 Dec 86
 *		Support all baud rates in SVID.  Support for 134.5 baud is
 *		only approximate due to board limitations.
 *	I015	vrs	04 Dec 86
 *		A param command can cancel output without causing an output
 *		complete notification.  This means we must clear WFLUSH in
 *		i546param().
 *	I016	vrs	09 Dec 86
 *		Fixed a problem where the SETCARR macro was looking in the
 *		wrong place for the CLOCAL flag and a bug with stty 0 which
 *		caused a panic.
 *	I017	vrs	12 Dec 86
 *		Rewrote i546start (again) to use the l_output routine rather
 *		than fiddling clists directly.  This makes us compatible with
 *		line disciplines other than 0.  Note, however, that the TOY
 *		clock can still be manipulated only via line discipline 0.
 *	I018	vrs	15 Dec 86
 *		Fixed a number of problems with SPCOND interrupts which
 *		prevented recovery when the rawq filled up.
 *	I019	vrs	20 Dec 86
 *		Assume CLOCAL for the printer and clock ports.
 *		Return only data bits when parity is enabled (a la i8251).
 *		The 546 only supports one input parity mode (null replacement);
 *		Therefore, PARENB => INPCK, IGNPAR.
 *		Insure that an T_UNBLOCK occurs when a T_RFLUSH is done.
 *		Allow interrupts once per controller response, to prevent a
 *		a problem where interrupts were left masked for multiple
 *		milliseconds.
 *	I020	vrs	06 Jan 87
 *		Keep the hardware and software state of the line synchronized
 *		even when returning errors to the user.
 *		Make the SETCARR macro sensitive to both edges of CARRF.  The
 *		software carrier flag was not getting cleared by carrier loss
 *		interrupts.
 *	I021	vrs	01 Feb 87
 *		Add hooks to profile transmit, receive, and modem interrupts.
 *	I022	vrs	27 Feb 87
 *		Add hooks to support 386 boards with more than 15.5 Mb of RAM.
 *	I023	rajiv	21 Jul 87
 *		Changed an spl0 to spl6.  This will allow console interrupts 
 *		to get in but not disk interrupts.
 *	I024	rajiv	10 Aug 87
 *		Added support for handling multiple boards per interrupt.
 *	I025	adams	01 Sep 87
 *		Added support for new 546 firmware input parity error handling.
 *	I026	vrs	09 Sep 87
 *		Notify ttywait() of carrier loss to prevent waiting forever
 *		for output to drain.
 *	I027	vrs	10 Sep 87
 *		Remove INCLOSE hack and use new ISOPEN handshake implemented
 *		in the line discipline.
 *	I028	vrs	14 Sep 87
 *		Tweaks to make lint happy.  Remove dead cl_to_b() routine.
 *		Removed references to non-existant nclist structure.
 *		Fixed broken version stamp.
 *	I029	vrs	17 Sep 87
 *		Remove MINORMSK.  SVID says the minor number selects the line
 *		without any magic bits.
 *	I030	vrs	20 Sep 87
 *		Performance tweaks.  Recoded peek() and poke() as asm inline
 *		"copy_bytes", removed peekb().
 *	I031	vrs	20 Oct 87
 *		Don't presume that the user does not want the contents of the
 *		reserved fields when doing a clock ioctl and use copyin() and
 *		copyout() as intended.
 *	I032	vrs	20 Oct 87
 *		Moved copy_bytes into inline.h so everyone can use it.
 *	I033	pats	14 Nov 87
 *		Added interrupt timeout kludge to support multiple boards per
 *		interrupt.
 *	I034	vrs	14 Nov 87
 *		Fixed driver to return only data bits (as per I019), more lint
 *		corrections.
 *	I035	pats	01 Dec 87
 *		Fixed 546proc(RFLUSH) to flush input both in buffer
 *		and on board.
 *		Changed argument of i546params from major# + minor# (dev_t) to 
 *		minor# (int).
 *	I036	cw	07 Jan 88
 *		Made changes in i546tmout() so additional interrupts are masked
 *		out while servicing a current one with i546intr().
 *
 *  Taken from Intel internal sources.  Last update was I002.1 (3/21/86)
 *
 *  The minor device number is coded as follows:
 *        minor = board*12 + port
 *  where
 *        board = position that the board is configured for
 *        port = port number on board (0..n).  For the 546, ports
 *                 0-3 are the async ports, port 4 is the parallel
 *                 printer port, and 5 is the calender "port".
 *  So, for a 546 board that is configured as board 1 (default jumpering),
 *  the devices set up are:
 *        mknod /dev/ttyi0 c 15 0   ; terminal port 0
 *        mknod /dev/ttyi1 c 15 1   ; terminal port 1
 *        mknod /dev/ttyi2 c 15 2   ; terminal port 2
 *        mknod /dev/ttyi3 c 15 3   ; terminal port 3
 *        mknod /dev/lp    c 15 4   ; parallel port
 *        mknod /dev/clock c 15 5   ; access to onboard calendar/clock
 *************************************************************************/

#include "sys/types.h"
#include "sys/param.h"		/* kernel global parameters */
#include "sys/sysmacros.h"
#include "sys/errno.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/user.h"		/* user structures (system) */
#include "sys/systm.h"		/* more kernel parameters */
#include "sys/conf.h"		/* device switch description */
#include "sys/termio.h"		/* terminal parameter definitions */
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/ioctl.h"		/* io control commands and parameters */
#include "sys/clockcal.h"	/* clock/calendar interface and ioctl */
#ifndef lint
#include "sys/inline.h"
#endif
#include "sys/immu.h"
#include "sys/sysinfo.h"
#include "sys/i546.h"		/* hardware structure and local commands */

#ifndef lint
static char i546copyright[] = "Copyright 1983, 1986, 1987 Intel Corp.";
#endif

extern caddr_t sptalloc();		/* I028 Starts */
extern int copyin();
extern int copyout();
extern int getc();
extern int timeout();			/* I034 */
extern int ttiocom();			/* I034 */

extern void dri_printf();
#ifdef lint
extern void copy_bytes();		/* I032 */
extern void flushtlb();
extern void outb();
#endif
extern void signal();
extern void spinwait();
extern void splx();
extern void sptfree();
extern void ttinit();
extern void ttyflush();
extern void ttywait();
extern void wakeup();			/* I028 Ends */

#ifdef DEBUG546
int	i546debug = 0;			/* debug output control */
int	trbufcount = 0;
char *i546cmds[] = {"ZERO", "INITIALIZE", "ENABLE", "DISABLE",
	"CONFIGURE", "TRANS BUFFER", "ABORT TRANS", "SUSPEND TRANS",
	"RESUME TRANS", "ASSERT DTR", "SET CTS/CD", "CLEAR CTS/CD",
	"SET DSR REP", "CLEAR DSR REP", "SET RI REP", "CLEAR RI REP",
	"CLEAR DTR", "SET BREAK", "CLEAR BREAK", "DOWNLOAD",
	"EXECUTE", "CLR REC BUF" };
char *i546msgs[] = {"ZERO", "TRANS COMP", "INPUT AVAIL", "DOWNLOAD COMP",
	"CARRIER DETECT", "CARRIER LOSS", "INIT RESP", "AUTOBAUD COMP",
	"SPEC CHAR REC", "DSR DETECT", "DSR LOST", "RI DETECT",
	"RI LOST" };
#define DDEBUG(x,y) if(i546debug&(x))dri_printf y
#define DINIT 1		/* initialization */
#define DCALL 2		/* driver call and start */
#define DINTR 4		/* interrupt time */
#define DIOCTL 8	/* IOCTL calls */
#define DMSG 16		/* messages as they go to and from the board */
#define DLOCAL 32	/* local, one time debugging stuff */
#else
#define DDEBUG(x,y)
#endif

/* set CARR_ON flag correctly */
/* "correctly" is on if CLOCAL and to true line state if not */
#define SETCARR(tx,lx)	if ((tx)->t_cflag & CLOCAL || (lx)->l_state&CARRF) \
				(tx)->t_state |= CARR_ON; \
			else \
				(tx)->t_state &= ~CARR_ON;	/* I020 */
/* Routine for putting one chararacter into the input c-list */
#define PutInputChar(tx, c) \
	*(tx)->t_rbuf.c_ptr = (c); \
	(tx)->t_rbuf.c_count--; \
	(*linesw[(tx)->t_line].l_input)((tx), L_BUF);

/*
 * These variables are defined in c546.c
 */
extern	int    N546;			/* number of 546s, this config */
extern	struct tty	 i546_tty[];	/* i546LINES lines per 546 */
extern	struct i546board i546board[];	/* board structure */
extern	struct i546cfg   i546cfg[];	/* board configuration addresses */
extern  int    i546baud[];		/* baud rate translation table */
extern	time_t	time;			/* I033 system time */
extern	time_t	i546_time[];		/* I033 output char watchdog timeout */

/*
 * Variables local to the driver
 */
int		i546sleep;		/* semaphore of sleeping processes */
static char	i546tmrun = 0;		/* I033 timer set up? */
#define MAXTIME	2			/* I033 2 seconds */

void i546tmout();			/* I033 Catches interrupt snafus */
char *i546type[] = {
	"iSBC 546",
	"iSBC 188",
	"iSBC 547/548",
	"iSBC 546"
};


/*
 * This procedure initializes the iSBC 546/48 when the call to dinit is
 * made. This procedure is done ONCE ONLY in the following sequence:
 * 	initialize the iSBC 546/48 structures to point at the board,
 *      calculate the addresses of the line structures' input buffer
 *	address, and output buffer address.
 *	Report the location and status of each board at the console.
 *
 * TITLE:	i546init
 *
 * CALL:	i546init();
 *
 */

i546init()
{
register int  bnum;
register struct i546board *bd;

	DDEBUG(DINIT, ("i546-init\n"));

	/*
	 * probe each board configured in the system
	 * if it exists sety up the driver's copy
	 * of firmware values and offsets.
	 */
	for (bnum=0; bnum < N546; bnum++) {
		bd = &i546board[bnum];
		i546check(bd, &i546cfg[bnum]);
		dri_printf("%s at %x%x board %d ",
			i546type[bd->b_type],
			(ushort)(i546cfg[bnum].c_addr>>16),
			(ushort)(i546cfg[bnum].c_addr&0xffff),bnum);
		if (bd->b_alive & ALIVE) {
			dri_printf("v%d.%d ", (bd->b_version>>4),
				(bd->b_version&0x0F));
			if (bd->b_alive & I546ERROR) {
				dri_printf("<<exception code %d>> Not ",
					(bd->b_addr->ConfidenceTestResult));
				bd->b_alive &= ~ALIVE;
			}
			dri_printf("Initialized\n");
		} else {
			dri_printf("Not Found\n");
		}
	}
}


/*
 * This procedure verifies that a isbc546 board is presently
 * configured by sending a reset command to the board, if the
 * board is reset successfully (status byte has a value of 1,
 * indicating the command is accepted), the board is there; otherwise,
 * the board is considered not there. 
 * Information on the aliveness of the board is returned as flags
 * in bd->b_alive.
 *
 * TITLE:	i546check
 *
 * CALL:	i546check(&board, &configinfo);
 *
 */

i546check(bd, cf)
	struct i546cfg   *cf;
	struct i546board *bd;
{

	DDEBUG(DINIT, ("i546-check\n"));
	if (cf->c_addr == (long)0x0)
		return;
	bd->b_port = cf->c_port;
	/*
	 * Some 386 boards support more than 0xF80000 bytes of RAM.  For these
	 * processors, maxclick will be greater than 15.5 Mb, and Multibus
	 * addresses start at processor address 0xBF000000.
	*/
	if (ctob(maxclick+1) > 0xF80000)	/* I022 */
		cf->c_addr |= 0xBF000000;	/* I022 */
	/*
	 * if the board is configured in the system
	 * set up selector in kernels memory map for board
	 * reset board and see if it answers
	 * else assume its not there
	 */
	outb(bd->b_port, RESET);
	spinwait(5*1000);		/* wait for selftest to finish */
	/* the base address is different for some boards so we try
	 * to find it at two different addresses
	 */
	if (!tryboard(bd, cf->c_addr))
		(void) tryboard(bd, cf->c_addr+BDSTART);
}

tryboard(bd, baseaddr)
	register struct i546board *bd;
	long baseaddr;
{
	register struct i546line  *ln;
	unsigned output_base;
	unsigned output_line;
	int	alive;
	int	lines, count;

	alive = bd->b_alive = 0;
	if ((bd->b_addr=(struct i546control *)sptalloc(btoc(I546LIMIT),
			PG_RW|PG_P, baseaddr>>PNUMSHFT&PGFNMASK, 0)) == 0) {
		/* don't have the memory to allocate map */
		dri_printf("NO MAP\n");
		return(0);
	}
	/*
	 * if present, first determine it passed all tests
	 * then send initialize message and mark lines
	 * that are present.
	 */
	if ((bd->b_addr->BoardType != 0)
	&&  (bd->b_addr->BoardType < I546TYPES)) {
		bd->b_type = bd->b_addr->BoardType;
		bd->b_version = bd->b_addr->Version;
		if ((bd->b_addr->CompletionFlag&0xff) != 0xff
		||  (bd->b_addr->ConfidenceTestResult&0xff) != 0xff)
			alive |= (I546ERROR|ALIVE);
		else {
			bd->b_msg.m_type = INIT;
			bd->b_msg.m_cnt = iSBX354s;
			i546io_spin(bd, &bd->b_msg);
			spinwait(150);
			if (!i546get_cmd(bd, &bd->b_msg))
				bd->b_msg.m_type = 0;
			outb(bd->b_port, INTR_CLR);
			lines = bd->b_msg.m_cnt;

			/*
			 * mark the line structure
			 * for each configured port on board
			 * this configures in the optional
			 * iSBX 354 multimodules.
			 */
			if (bd->b_msg.m_type == INTCMP) {
				alive |= ALIVE;
				if (bd->b_type == I188BOARD) {
					output_base = OUTBASE_12LINES;
					output_line = OUTSIZE_12LINES;
				} else {
					output_base = OUTBASE_8LINES;
					output_line = OUTSIZE_8LINES;
				}
				lines = bd->b_msg.m_cnt;
				for (count=0; count<i546LINES; count++) {
					if (lines&(1<<count)) {
						ln = &bd->b_line[count];
						ln->l_state = ALIVE;
						ln->l_oba = output_base + (count*output_line);
						ln->l_obsiz = output_line;
						ln->l_iba = INBUFBASE + (count*INLINSIZ);
						ln->l_ibsiz = INLINSIZ;
					}
				}
#ifdef DEBUG546
				{
					/* this places the ASCII chars of the logical address
					 * into the memory of the 586.  This is useful for
					 * debugging.  The addresses are "backwards" but the
					 * code is simple.
					 */
					char *bbp; int i;
					bbp = (char *)&bd->b_addr->DSfiller[0];
					while ((int)bbp < (((int)bd->b_addr)+32000)) {
						for (i=(int)bbp; i!=0; bbp++) {
							*bbp = i%16>9 ? 'a'+i%16-10 : '0'+i%16;
							i /= 16;
						}
					}
				}
#endif
			}
		}
	}
	if (!alive) {
		sptfree(bd->b_addr, btoc(I546LIMIT), 0);
		bd->b_addr = NULL;
		/* the sptfree will leave stuff in the look aside buffer */
		flushtlb();
	}
	bd->b_alive = alive;
	return(alive);
}


/*
 * This procedure sets up the baud rate and the parameter of the device.
 * The code depends on having the ttystructure filled out before a call is made
 * to i546param. This is the sequence of events;
 *	check for valid speed
 *	set up the baud rate and parameter
 *
 * TITLE:	i546param
 *
 * CALL:	i546param(unit);
 *
 */

i546param(unit)
int unit;
{
	register struct tty	  *tp;
	register struct i546board *bd;
	struct i546line  *ln;
	int	bnum, param, l_num, s;
	int	speed;

	DDEBUG(DCALL, ("i546-param unit %d ::",unit));
	bnum = unit / i546LINES;	/* calculate the board number */
	tp = &i546_tty[unit];		/* set up ptr. of the tty struct */
	bd = &i546board[bnum];
	l_num = unit % i546LINES;	/* get the line no. */
	ln = &bd->b_line[l_num];
	/*
	 * Critical region
	 */
	s = SPL();
	/* I007: calling routine must wait for BUSY if they require */
	/*
	 * translate index into hardware dependent number
	 */
	speed = i546baud[tp->t_cflag&CBAUD];	/* I014 */
	/*
	 * if speed is zero disable line else
	 * setup configuration message with current
	 * tty configuration and send to board
	 */
	if (speed == 0) {			/* I014 */
		tp->t_cflag = (tp->t_cflag&~CBAUD)|B9600; /* I020 */
		speed = i546baud[B9600];		  /* I020 */
		/*
		 * only disable modem lines
		 */
		if (!(tp->t_cflag&CLOCAL)) {
			bd->b_msg.m_type = DTRCLR;
			bd->b_msg.m_line = l_num;
			i546io(bd);		/* I013, I016 */
						/* I020 don't return here */
		} else
			u.u_error = EINVAL;	/* I020 don't return here */
		/*
		 * I020 Fall through to match hw state to sw state.
		*/
	}
	/*
	 * construct a complete line
	 * configure message
	 */
	bd->b_msg.m_type = CONFIG;
	bd->b_msg.m_line = l_num;
	bd->b_msg.m_cnt = speed;
	param = LNDISP;				/* I009 */
	if (tp->t_cflag & CSTOPB)		/* I009 */
		param |= STBITS2;		/* I009 */
	else					/* I009 */
		param |= STBITS1;		/* I009 */
	SETCARR(tp, ln);			/* I013 */
	/* set parity bits in "param"  */
	if (tp->t_cflag & PARENB) {
		/*
		 * I025: support for 546 firmware that supports "better" input
		 * parity processing.  Previous versions of this driver knew
		 * that the board would not support PARMRK, IGNPAR, and INPCK
		 * so, if you specified PARENB, it would force PARMRK off and
		 * INPCK and IGNPAR on so that your flags would look like what
		 * the board was really going to do to you.  With the firmware
		 * update, these modes can now be supported so this odd driver
		 * behavior can be removed.
		*/ 
		/* tp->t_iflag &= ~PARMRK;		/* I019 Can't do this	*/
		/* tp->t_iflag |= INPCK|IGNPAR;	/* I019 Always do these	*/
		if (tp->t_cflag & PARODD)
			param |= PODD;
		else
			param |= PEVEN;
		if (tp->t_iflag & INPCK) {
			if (tp->t_iflag & IGNPAR) {
				param |= PEDEL;
			} else {
				if (tp->t_iflag & PARMRK) {
					param |= PEMARK;
				} else {
					/* the firmware does not support this mode so we
					 *  must mark the character and turn it into a null
					 *  later.
					 */
					param |= PEMARK;
				}
			}
		} else {
			param |= PEACCEPT;
		}
	} else
		param |= PNO;
	/* Set character size bits in "param" */
	switch (tp->t_cflag & CSIZE) {
		default:			/* I020 */
			u.u_error = EINVAL;	/* I020 Rig to return error */
			tp->t_cflag = (tp->t_cflag&~CSIZE)|CS8; /* I020 */
			/*
			 * I020 FALL THROUGH to align the hardware and software
			 * state of the line to some consistent (CS8) state.
			*/
		case CS8:
			param |= C8BITS;
			ln->l_mask = 0xff;		/* I019 */
			break;
		case CS7:
			param |= C7BITS;
			ln->l_mask = 0x7f;		/* I019 */
			break;
		case CS6:
			param |= C6BITS;
			ln->l_mask = 0x3f;		/* I019 */
			break;
	}
	(*tp->t_proc)(tp, T_WFLUSH);	/* I015 */
	bd->b_msg.m_buf[0] = SPCHAR;
	bd->b_msg.m_buf[1] = SPHIWAT;
	bd->b_msg.m_buf[2] = NULL;
	bd->b_msg.m_buf[3] = (unsigned)tp->t_cc[VINTR];
	bd->b_msg.m_buf[3] |= (((unsigned)tp->t_cc[VQUIT])<<8);
	bd->b_msg.m_buf[4] = (unsigned)tp->t_cc[VKILL];
	bd->b_msg.m_buf[4] |= (((unsigned)tp->t_cc[VQUIT])<<8); /* I018 */
	bd->b_msg.m_ptr = param;
	i546io(bd);
	DDEBUG(DCALL, ("i546-param: unit=%d, param=%x\n", unit, param));
	splx(s);
}

/*
 * This procedure opens one of the i546LINES lines on the iSBC 546 board
 * for use by a user.  Note the implicit handling of signals, relying on
 * the "half-open" code to call i546close() and thus disable the line.
 * It might have been more obvious to have used PCATCH, but that would
 * have meant more code in more places. 
 *
 *
 * TITLE:	i546open
 *
 * CALL:	i546open(dev, flag);
 *
 */

int i546proc();
int i546s_input();

/* ARGSUSED */
i546open(dev, flag)
dev_t	dev;
int	flag;
{
	register struct tty       *tp;
	register struct i546board *bd;
	struct i546line  *ln;
	int	 	unit;
	int		bnum, s, l_num;

	unit = minor(dev);		/* I029 */
	bnum = unit / i546LINES;
	l_num = unit % i546LINES;
	bd = &i546board[bnum];
	ln = &bd->b_line[l_num];

	DDEBUG(DCALL, ("i546-open unit %d ::",unit));
	if (unit >= (N546*i546LINES)) {		/* not enough tp's */
		u.u_error = ENXIO;
		return;
	}
	if ((bd->b_alive != ALIVE) || ((ln->l_state&ALIVE) != ALIVE)) {
		u.u_error = ENXIO;
		return;
	}

	tp = &i546_tty[unit];			/* pointer to the tty struct */
	tp->t_proc = i546proc;
	/*
	 * if line not already open
	 * initialize the line's tty structure
	 */
	s = SPL();

	if ((tp->t_state & ISOPEN) == 0 ) {
		ln->l_state &= ~CARRF;				/* I013 */
		tp->t_state &= ~CARR_ON;			/* I013 */
		ttinit(tp);
		/* I000 leave ttinit values except baud rate */
		tp->t_cflag = B9600 | (tp->t_cflag&~CBAUD);	/* I000 */
		/* set initial line parameters for printer */	/* I019 */
		if (bd->b_type == I546BOARD && LINNO(dev) == 4)	/* I019 */
			tp->t_cflag |= CLOCAL;			/* I019 */
		/* set initial line parameters for clock */	/* I019 */
		if (bd->b_type == I546BOARD && LINNO(dev) == 5)	/* I019 */
			tp->t_cflag |= CLOCAL;			/* I019 */
		ln->l_obc = 0;		/* I005 No ln->l_ocnt anymore */
		/* I033 Start */
		if (!i546tmrun) {
			i546_time[bnum] = time;
			i546tmout((caddr_t)0);
		} 
		/* I033 End */
		/* we "know" the device is not BUSY (I007) */
		i546param(unit);
	}

	/* send enable message to board */
	bd->b_msg.m_type = ENABLE;
	bd->b_msg.m_line = l_num;
	i546io(bd);

	/*
	 * assert DTR signal and sleep waiting for a carrier detect
	 * interrupt to wake the process up only if it is a modem
	 * line. DTR must be asserted after the line is enabled (see
	 * firmware EPS) and before a carrier detect can be sensed.
	 */
	bd->b_msg.m_type = DTRAST;
	bd->b_msg.m_line = l_num;
	i546io(bd);
	SETCARR(tp, ln);				/* I013 */
	if (!(flag & FNDELAY)) {			/* I013 */
		while (!(tp->t_state & CARR_ON))	/* I013 */
			(void) sleep((caddr_t)&tp->t_rawq, TTIPRI); /* I013 */
	}
	splx(s);
	/* I008: CARR_ON setting moved to intr */
	(*linesw[tp->t_line].l_open)(tp);	
}


/*
 * This procedure performs the close operation on one of the devices of the
 * 546. A close masks the device on board; reinstalls the flags that state
 * the device is closed; calls l_close to do the operation.
 *
 * TITLE:	i546close
 *
 * CALL:	i546close(dev, flag);
 *
 */
/* ARGSUSED */
i546close(dev, flag)
dev_t	dev;
int	flag;
{
	register struct tty 	  *tp;
	register struct i546board *bd;
	int	 unit;
	int	 bnum, l_num, s;

	unit = minor(dev);		/* I029 */

	DDEBUG(DCALL, ("i546 close unit %d ::",unit));

	tp = &i546_tty[unit];		/* set up ptr. to the tty struct */
	bnum = unit / i546LINES;	/* get the board number */
	bd = &i546board[bnum];
	l_num = unit % i546LINES;	/* get the line number */

	/*
	 * wait for output buffer to drain
	 * then disable port
	 */
	(*linesw[tp->t_line].l_close)(tp);
	s = SPL();
	if (tp->t_state & ISOPEN)	/* I027 */
		return;			/* I027 */
	if (tp->t_cflag & HUPCL) {
		bd->b_msg.m_type = DISABL;
		bd->b_msg.m_line = l_num;
		i546io(bd);
	}
	splx(s);
}


/*
 * This procedure interfaces the read request with the system read operation.
 *
 * TITLE:	i546read
 *
 * CALL:	i546read(dev)
 *
 */

i546read(dev)
dev_t	dev;
{
	register struct tty 	 *tp;			
	int unit, s;

	unit = minor(dev);		/* I029 */

	DDEBUG(DCALL, ("i546-read on unit %d ::",unit));

	tp = &i546_tty[unit];
	/*
	 * if the restart input subroutine is set and the raw
	 * queue is drained then restart input interrupts
	 */
	s = SPL();
	/* I006: INSTOP starting moved to T_UNBLOCK */
	/* request system read */
	(*linesw[tp->t_line].l_read)(tp);
	splx(s);
}


/*
 * This procedure is the compliment of the i546read routine. A call is
 * made to ttwrite which watches the output queue for characters and
 * gets the characters in the queue out to the device.
 *
 * TITLE:	i546write
 *
 * CALL:	i546write(dev);
 *
 */

i546write(dev)
dev_t	dev;
{
	register struct tty *tp;			
	register unit;

	unit = minor(dev);		/* I029 */

	DDEBUG(DCALL, ("i546-write on unit %d ::",unit));
	tp = &i546_tty[unit];			
	/*
	 * pass characters to output queue
	 * and start output on device
	 */
	(*linesw[tp->t_line].l_write)(tp);		
}

/*
 * This routine is used to restart the input sequence of a
 * 546 line. We tell the 546 that we have read 0 chars
 * from it's buffer and that causes an input available interrupt.
 * May only be called by a task time procedure
 *
 * TITLE:	i546s_input
 *
 * CALL:	i546s_input(tp)
 *
 */
i546s_input(tp)
register struct tty *tp;
{
	register struct i546board *bd;
	int l_num, unit, s;

	unit = tp - i546_tty;
	bd = &i546board[unit/i546LINES];
	l_num = unit % i546LINES;

	/*
	 * send an INPUT COMMAND of 0 bytes to the 546
	 * This is to get interrupts started again
	 */
	s = SPL();
	bd->b_msg.m_type = INPUT;
	bd->b_msg.m_line = l_num;
	bd->b_msg.m_ptr = bd->b_line[l_num].l_ibp;
	bd->b_msg.m_cnt = 0;
	i546io(bd);
	splx(s);
}


/* I017 i546s_output no longer needed */
/*
 * This procedure is called with interrupts off (spltty) when the
 * iSBC 546 interrupts.
 *	Warning:
 *		The interrupt routine must repeat its scan and read all
 *		messages from the board before it exits the interrupt
 *		routine. It is possible to get a spurious interrupt
 *		immediately after the message queue of a board has been
 *		purged.
 *
 * TITLE:	i546intr
 *
 * CALL:	i546intr(level);
 *
 */
i546intr(level)
int	level;
{
	register struct tty 		*tp;
	register struct i546line 	*ln;
	struct i546board	*bd;
	struct  i546msg fr_msg, to_msg;
	int 	bnum, count, nch, l_num;
	int	s, max;
	unsigned	char	c;
	unsigned int gotboard, gotmessage;	/* I024 Start */

	DDEBUG(DINTR, ("i546-intr\n"));
do_again:
	gotmessage = gotboard = 0;
	for (bnum=0, bd=0; bnum < N546; bnum++) {
		if(i546cfg[bnum].c_level != level)
			continue;
		if( ! (i546board[bnum].b_alive & ALIVE))
			continue;
		gotboard = 1;
		bd = &i546board[bnum];		/* I024 End */

		/*
		 * signal the 546/48 to clear its interrupt then
		 * get each message from the board's queue and
		 * service the command from the board
		 * repeat until no outstanding messages from
		 * the board are left in the queue
		 */	
		outb(bd->b_port, INTR_CLR);
		i546_time[bnum] = time;			/* I033 */
		while ((i546get_cmd(bd, &fr_msg)) != CLEAR) {
			/* Allow interrupts once per iteration I019 */
			gotmessage = 1;
			s = spl6();			/* I019 I024 */
			splx(s);			/* I019 */
			/* wakeup processes waiting on an event */
			if (i546sleep) {
				i546sleep = 0;
				wakeup((caddr_t)&i546sleep);
			}
			/*
			 * setup required pointers
			 */
			l_num = fr_msg.m_line;
			ln = &bd->b_line[l_num];
			tp = &i546_tty[((bnum*i546LINES) + l_num)];
			DDEBUG(DINTR, ("board %d line %d\n",bnum, l_num));
			/*
			 * service status command on line
			 * according to type
			 */
			switch(fr_msg.m_type) {

			/*
			 * Input Available:	
			 *	Set the input request indicator		
			 *      Read in the input from the input buffer
			 *	Send the number of bytes input, line    
			 *	number, and INPUT command to the board, 
			 */
			case INAVIL:
				sysinfo.rcvint++;	/* I021 */
				/* wakeup((caddr_t)&tp->t_rawq); */
			
				if (ln->l_state & INFLUSH) {	 /* I035 */
					ln->l_state &= ~INFLUSH; /* I035 */
					nch = fr_msg.m_cnt;	 /* I035 */
				} else {
					ln->l_ibp = fr_msg.m_ptr - BDSTART;
					ln->l_ibc = fr_msg.m_cnt;
					if ((count = (TTYHOG-1) - tp->t_rawq.c_cc) < 1) {
						ln->l_state |= INSTOP;
						tp->t_state |= TBLOCK;
						break;
					}
					ln->l_ibc = (ln->l_ibc > count ? count : ln->l_ibc);
					ln->l_state |= INBUSY;
					max = (ln->l_ibsiz - 1) + ln->l_iba;
					for (nch=0; ln->l_ibc != 0; ln->l_ibc--, nch++) {
						c = *(((unsigned char *)bd->b_addr)
							+ ln->l_ibp);
						if ((++ln->l_ibp) > max)
							ln->l_ibp = ln->l_iba;
						ProcessInputChar(tp, ln, c);
					}
					ln->l_state &= ~INBUSY;
				}
				to_msg.m_type = INPUT;
				to_msg.m_line = l_num;
				to_msg.m_cnt = nch;
				i546io_spin(bd, &to_msg);
				(*tp->t_proc)(tp, T_OUTPUT);
				break; 
			/*
			 * Output Complete:
			 */
			case OUTCMP:
				sysinfo.xmtint++;	/* I021 */
				DDEBUG(DINTR, ("i546intr: in OUTCMP case. count is %d.\n", fr_msg.m_cnt));
				tp->t_state &= ~BUSY;		/* I005 */
				ln->l_obc -= fr_msg.m_cnt;
				(*tp->t_proc)(tp, T_OUTPUT);	/* I017 */
				break;
			/*
			 * Carrier Detect:
			 *	Wakeup the process
			 */
			case ONCARR:
				sysinfo.mdmint++;	/* I021 */
				ln->l_state |= CARRF;		/* I013 */
				SETCARR(tp, ln);		/* I013 */
				wakeup((caddr_t)&tp->t_rawq);
				break;
			/*
			 * Carrier Loss:
			 *	Signal the process
			 *	Reset the CARR_ON state of the line
			 */
			case OFCARR:
				sysinfo.mdmint++;	/* I021 */
				ln->l_state &= ~CARRF;		/* I013 */
				if ((tp->t_state&CARR_ON)  && !(tp->t_cflag&CLOCAL)) {
					signal(tp->t_pgrp,SIGHUP);
					/* I003 Drop DTR in close, not here */
				}
				SETCARR(tp, ln);		/* I013 */
				wakeup((caddr_t)&tp->t_oflag);	/* I026 */
				break;
			/*
			 * Special condition character recognized.
			 * This only happens when the board's input buffer
			 * fills above a preset threshold.
			 * If the line is in cooked mode and quit or intr
			 * character is received it is handled immdeiately.
			 */
			case SPCOND:
				sysinfo.rcvint++;	/* I021 */
				wakeup((caddr_t)&tp->t_rawq);
				if (tp->t_lflag&ICANON) {
					c = fr_msg.m_cnt;
					if (c == tp->t_cc[VKILL])	/* I018 */
						ttyflush(tp, FREAD);	/* I018 */
					if (tp->t_rbuf.c_ptr != NULL) {	/* I018 */
						PutInputChar(tp, c);
					}
				}
				break;
			default:
				dri_printf("Invalid iSBC 546/48 message: type %d",fr_msg.m_type);
				dri_printf(" board %d, line %d\n",bnum, l_num);
				break;
			}
		}
	} /* I024 Start */
	if(!gotboard) {
		dri_printf(" Spurious iSBC 546/48 Interrupt\n");
		return;
	}
	if(gotmessage) {
		goto do_again;
	} /* I024 End */
}

ProcessInputChar(tp, ln, c)
	register struct tty *tp;
	register struct i546line *ln;
	register unsigned char c;
{
	if (tp->t_rbuf.c_ptr != NULL) {
		switch (ln->l_state & (RECFF|RECFFNUL)) {
			/*
			 *  This code handles the parity marking case when INPCK is on
			 *  IGNPAR is off and PARMRK is off -- ie, input parity
			 *  errors are read as nulls.  The 546 will only mark
			 *  parity errors so we keep the state of the mark in RECFF and
			 *  RECFFNUL.  RECFF goes on when a mark (0xFF) is received.
			 *  RECFFNUL goes on when the next character is a NUL.  The
			 *  character after that will be the parity error char and will
			 *  get turned to NUL if the flags warrent that.  If a NUL does
			 *  not follow the MARK and marking is enabled the character is
			 *  passed through.
			 *  Note that RECFF is only set when marking is being turned to
			 *  NULs.
			 */
			case 0:
				/* no flags.  See if the char is the mark character */
				if (c == 0xFF) {
					if (tp->t_iflag&INPCK && !(tp->t_iflag&IGNPAR)) {
						ln->l_state |= RECFF;
						DDEBUG(DLOCAL, ("ProcessChar: marking FF\n"));
						return;
					}
				}
				break;
			case RECFF:
				/* last character was a mark char */
				if (c == 0) {
					/* received the NUL after the MARK */
					ln->l_state |= RECFFNUL;
					DDEBUG(DLOCAL, ("ProcessChar: NUL after mark\n"));
					return;
				} else {
					/* MARK not followed by NUL.  Another MARK means one MARK */
					if (c != 0xFF) {
						PutInputChar(tp, 0xFF); /* put in ignored MARK */
						/* process received char by falling through */
					}
					ln->l_state &= ~(RECFF|RECFFNUL);
					DDEBUG(DLOCAL, ("ProcessChar: non-NUL after mark\n"));
				}
				break;
			case RECFF|RECFFNUL:
				/* received both MARK and NUL.  This is the bad char. */
				ln->l_state &= ~(RECFF|RECFFNUL);
				if (tp->t_iflag & PARMRK) {
					DDEBUG(DLOCAL, ("ProcessChar: sending marked parity\n"));
					PutInputChar(tp, 0xFF);
					PutInputChar(tp, 0);
					/* Return char without ISTRIP */
					PutInputChar(tp, c&ln->l_mask);/*I034*/
					return;
				} else {
					DDEBUG(DLOCAL, ("ProcessChar: changing marked to NUL\n"));
					c = 0;
					/* process NUL char by falling through */
				}
				break;
		}
		c &= ln->l_mask;	/* I034 ignore non-data bits */
		if (tp->t_iflag & ISTRIP)
			c &= 0177;
		if (tp->t_iflag&IXON) {
			/* XON/XOFF processing for output control */
			if (tp->t_state & TTSTOP) {
				/* output stopped, see if time to startup */
				if (c==CSTART || tp->t_iflag&IXANY) {
					(*tp->t_proc)(tp, T_RESUME);
					/* I008: fall through for throw away check */
				}
			} else {
				/* see if we are to stop output */
				if (c == CSTOP) {
					(*tp->t_proc)(tp, T_SUSPEND);
					/* I008: fall through for throw away check */
				}
			}
			/* if IXON then start and stop chars are not read */
			if (c==CSTART || c==CSTOP)
				return;
		}
		/*
		 * process input character
		 * mask to return only data bits
		*/
		PutInputChar(tp, c);	/* I034 */
	}
}

/*
 * This procedure starts output on a line if needed.  i546start gets characters
 * from the line disciplint to tp->t_tbuf, then outputs the characters to the
 * line and sets the BUSY flag.  The busy flag gets unset when the characters
 * have been transmitted [by i546intr()].  This procedure is called at task
 * time by the writing process, and asynchronously at interrupt time by the
 * interrupt service routine. 
 *
 * TITLE:	i546start
 *
 * CALL:	i546start(tp)
 *
 */
i546start(tp)
register struct tty *tp;
{
	register struct i546board	*bd;
	register struct i546line	*ln;
        int	 s, l_num, unit;
	int 	 bytes;
	
	s = SPL();
	unit = tp - i546_tty;		/* get the unit number */
	DDEBUG(DCALL, ("i546start: called on unit %d\n",unit));
	if (tp->t_state & BUSY) {	/* I018 */
		splx(s);
		return;
	}
	l_num = unit % i546LINES;	/* get the line number */
	bd = &i546board[unit/i546LINES];/* set pointer to board structure */
	ln = &bd->b_line[l_num];	/* set up ptr. to line struct */
	/*
	 * Check for characters indirectly through the
	 * linesw table.  If there are any, ship them out.
	 */
	if (CPRES & (*linesw[tp->t_line].l_output)(tp)) {	/* I017 */
		bytes = tp->t_tbuf.c_count;			/* I017 */
		DDEBUG(DCALL, ("i546start: %d on unit %d\n",bytes, unit)); /*I017*/
		copy_bytes(tp->t_tbuf.c_ptr, ((char *)bd->b_addr)+ln->l_oba, bytes);
		bd->b_msg.m_type = OUTPUT;
		bd->b_msg.m_line = l_num;
		bd->b_msg.m_ptr = ln->l_oba + BDSTART;		/* I011 */
		bd->b_msg.m_cnt = bytes;
		i546io_spin(bd, &bd->b_msg);
		ln->l_obc += bytes;
		tp->t_state |= BUSY;
	}
	/* I017 wakeup now done by l_output */ 
	splx(s);
	return;
}


/*
 * This procedure handles the ioctl system calls for such things as baud rate
 * changes and various hardware control changes from the initial set up.
 * Currently only baud rate changes and terminal mode changes are supported.
 *
 * TITLE:	i546ioctl
 *
 * CALL:	i546ioctl(dev, comd, addr, flag)
 *
 */

#define ADDRTYPE caddr_t

/* ARGSUSED */
i546ioctl(dev, comd, addr, flag)
register dev;
ADDRTYPE addr;
int comd, flag;
{
	register struct	tty *tp;
	struct	clkcal ck;
	struct	i546board *bd = &i546board[BRDNO(dev)];
	struct	i546line *ln = &i546board[BRDNO(dev)].b_line[LINNO(dev)];
	struct	clist	*cl;
	int	unit;
	int	s;

	unit = minor(dev);		/* I029 */
	DDEBUG(DIOCTL, ("i546-ioctl unit %d ::",unit));
	tp = &i546_tty[unit];
	/*
	 * I002
	 * Special case clock and parallel port status
	 * functions for 546 board.
	 */
	switch(comd) {
	/*
	 * Get current time or set the clock/calandar.
	 */
	case TIME_GET:
		DDEBUG(DIOCTL, ("i546-ioctl: got to TIME_GET.\n"));
		if (bd->b_type == I546BOARD && LINNO(dev) == 5) {
			s = SPL();
			ttyflush(tp, FREAD|FWRITE);
			i546s_input(tp);
			tp->t_state |= IASLP;
			DDEBUG(DIOCTL, ("i546-ioctl: went to sleep in time_get.\n"));
			(void) sleep((caddr_t)&tp->t_rawq, TTIPRI);
			DDEBUG(DIOCTL, ("i546-ioctl: woke up in time_get.\n"));
			if (tp->t_rawq.c_cc == CLKSIZE) {
				cl = &tp->t_rawq;
				getcn (cl, (ADDRTYPE) &ck, (unsigned)CLKSIZE);
				(void) copyout((caddr_t)&ck, addr, (unsigned)CLKSIZE);/*I031*/
			} else {
				u.u_error = EIO;
			}
			splx(s);
		} else
			u.u_error = EINVAL;
		break;
	case TIME_SET:
		DDEBUG(DIOCTL, ("i546-ioctl: got to TIME_SET.\n"));
		if (bd->b_type == I546BOARD && LINNO(dev) == 5) {
			if (copyin(addr, &ck, CLKSIZE) < 0)
				break;
			s = SPL();
			ttyflush(tp, FREAD|FWRITE);
			ck.ck_reserv2 = ck.ck_month;
			copy_bytes(&ck,((char *)bd->b_addr)+ln->l_oba,CLKSIZE);
			bd->b_msg.m_type = OUTPUT;
			bd->b_msg.m_line = LINNO(dev);
			bd->b_msg.m_ptr = ln->l_oba + BDSTART;	/* I011 */
			bd->b_msg.m_cnt = CLKSIZE;
			tp->t_state |= BUSY;	/* I005 */
			i546io(bd);
			splx(s);
		} else
			u.u_error = EINVAL;
		break;

	/*
	 * Get the parallel status and return
	 * status to the user.
	 *	NOTE:  this case has never been debugged and I will
	 *		very nearly guarantee that it doesn't work.
         *				jdh 2/26/86
	 */
	case LPSTAT_GET:
		DDEBUG(DIOCTL, ("i546-ioctl: got to LPSTAT_GET.\n"));
		if (bd->b_type == I546BOARD && LINNO(dev) == 4) {
			s = SPL();
			ttyflush(tp, FREAD|FWRITE);
			i546s_input(tp);
			tp->t_state |= IASLP;
			(void) sleep((caddr_t)&tp->t_rawq, TTIPRI);
			cl = &tp->t_rawq;
			getcn (cl, (ADDRTYPE) &ck, 1);
			(void) copyout(&ck, addr, 1);
			splx(s);
		} else {
			u.u_error = EINVAL;
		}
		break;
	/*
	 * modify the paramerters in the tty structure
	 * then reconfigure the line as required.
	 */
	default:
		(void) ttiocom(tp, comd, addr, dev);	/* I034 */
		switch (comd) {
		case TCSETAW:
		case TCSETAF:
			ttywait(tp);			/* I012 (voodoo) */
		case TCSETA:
			i546param(unit);
			break;
		default:
			break;
		}
		break;
	}
}



/*
 * This function is the common interface for line discipline
 * routines to call the device driver. It performs device
 * specific I/O functions.
 *
 * TITLE:	i546proc
 *
 * CALL:	i546proc(tp, comd)
 *
 */

i546proc(tp, comd)
register struct tty *tp;
register int comd;
{
	int s;
	int unit, l_num;
	struct i546board *bd;
	struct i546line  *ln;

	unit = tp - i546_tty;
	bd = &i546board[unit/i546LINES];
	l_num = unit % i546LINES;	/* get the line number */
	ln = &bd->b_line[l_num];

	DDEBUG(DCALL, ("i546-proc unit %d, cmd %d ::", unit, comd));
	switch (comd) {
	/* I005 flush output queue */
	case T_WFLUSH:
		s = SPL();
		if (tp->t_state & BUSY) {
			bd->b_msg.m_type = RESUME;
			bd->b_msg.m_line = l_num;
			i546io(bd);
			tp->t_state &= ~TTSTOP;
			bd->b_msg.m_type = ABORT;
			bd->b_msg.m_line = l_num;
			i546io(bd);
		}
		splx(s);
		break;
	/* resume output */
	case T_RESUME:
		s = SPL();
		bd->b_msg.m_type = RESUME;
		bd->b_msg.m_line = l_num;
		i546io(bd);
		tp->t_state &= ~TTSTOP;	/* I005 Don't clear busy */
		splx(s);
		break;
	/* suspend output */
	case T_SUSPEND:
		s = SPL();
		tp->t_state |= TTSTOP;
		bd->b_msg.m_type = SUSPND;
		bd->b_msg.m_line = l_num;
		i546io(bd);
		splx(s);
		break;
	/* send stop character */
	case T_BLOCK:
		s = SPL();				/* I010 */
		if ((putc(CSTOP, &tp->t_outq)) == 0)
			/* blocked and TTXON says to unblock with an XON */
			tp->t_state |= TBLOCK | TTXON;  /* I006 */
		splx(s);				/* I010 */
		break;
	/* I005 flush input queue */
	case T_RFLUSH:
		s = SPL();
		ln->l_state &= ~(RECFF|RECFFNUL);
		if (ln->l_state & INSTOP) {	/* I035 */
			ln->l_state |= INFLUSH; /* I035 */
		} 				/* I035 */
		splx(s);
		/* I019 FALL THROUGH */
	/* send start character */
	case T_UNBLOCK:
		s = SPL();				/* I010 */
		/* I006: if blocked because of XOFF sent, try sending XON */
		if (tp->t_state & TTXON)		/* I006 */
			if ((putc(CSTART, &tp->t_outq)) == 0)
				tp->t_state &= ~TTXON;
		/* I006: if blocked because of clist full, try starting */
		if (ln->l_state & INSTOP) {		/* I006 */
			i546s_input(tp);		/* I006 */
			ln->l_state &= ~INSTOP;		/* I006 */
		}					/* I006 */
		if (!(tp->t_state & TTXON))		/* I006 */
			tp->t_state &= ~TBLOCK;		/* I006 */
		splx(s);				/* I010 */
		break;
	/* remove timeout condition */
	case T_TIME:
		tp->t_state &= ~TIMEOUT;
		break;
	/* output break condition on line */
	case T_BREAK:
		s = SPL();
		tp->t_state |= TTSTOP;
		while (tp->t_state&BUSY)	/* I005 */
			(void) sleep((caddr_t)&tp->t_oflag, TTOPRI);
		bd->b_msg.m_type = BRKSET;
		bd->b_msg.m_line = l_num;
		i546io(bd);
		(void) timeout(wakeup,(caddr_t)&tp->t_oflag, HZ/4);/*I034*/
		(void) sleep((caddr_t)&tp->t_oflag, TTOPRI);
		bd->b_msg.m_type = BRKCLR;
		bd->b_msg.m_line = l_num;
		i546io(bd);
		tp->t_state &= ~TTSTOP;
		splx(s);
		break;
	/* start output */
	case T_OUTPUT:
		break;
	case T_PARM:			
		i546param(unit); 
		break;
	case T_SWTCH:			
		break;
	default:
		break;
	}
	/* I005 Let i546start sweat BUSY */
	if (((tp->t_state&(TTSTOP|TIMEOUT)) == 0) && !(ln->l_state&INBUSY))
		i546start(tp);
}
 

/*
 * TITLE:	i546io
 *
 * CALL:	i546io(bd)
 *
 */
i546io(bd)
register struct i546board *bd;
{
	/*
	 * if all message slots are in use
	 * wait on a interrupt to signal
	 * a free message slot
	 */
	while ((i546snd_cmd(bd, &bd->b_msg)) == 0) {
		i546sleep++;
		(void) sleep((caddr_t)&i546sleep, TTIPRI);
	}
}

i546io_spin(bd, buf)
	register struct i546board *bd;
	struct i546msg *buf;
{
	while ((i546snd_cmd(bd, buf)) == 0)
		;
}

/*
 *  Procedure to get put one command into the output queue.  If the
 *  queue is full, zero is returned.  Non-zero is returned if
 *  a command is succesfully placed.  A "copy_bytes" is used to place
 *  the command rather than a structure assignment because the
 *  546 board can not handle 32 bit MultiBus I data accesses.
 *
 * TITLE: i546snd_cmd
 *
 * CALL: i546snd_cmd(board, buffer)
 */
i546snd_cmd(brd, buf)
	struct i546board *brd;
	struct i546msg *buf;
{
	register struct i546control *ptr;

	DDEBUG(DMSG, ("i546snd_cmd: cmd=%s, ln=%x, cnt=%d, ptr=%x, ",
		i546cmds[buf->m_type], buf->m_line, buf->m_cnt, buf->m_ptr));
	DDEBUG(DMSG, ("b=%x,%x,%x,", buf->m_buf[0], buf->m_buf[1], buf->m_buf[2]));
	DDEBUG(DMSG, ("b=%x,%x\n", buf->m_buf[3], buf->m_buf[4]));
	ptr = brd->b_addr;
	if (((ptr->InQTail + 1) % i546QSIZE) == ptr->InQHead)
		return(0);	/* return 0 if queue is full */
	copy_bytes(buf, &ptr->InQ[ptr->InQTail], sizeof(struct i546msg));
	ptr->InQTail = (ptr->InQTail + 1) % i546QSIZE;
	outb(brd->b_port, 0x02);
	return(1);		/* return 1 if success */
}

/*
 *  Procedure to get one message from the input queue.  If the
 *  queue is empty, zero is returned.  Non-zero is returned if
 *  a message is succesfully fetched.  A "copy_bytes" is used to get
 *  the message rather than a structure assignment because the
 *  546 board can not handle 32 bit MultiBus I data accesses.
 *
 * TITLE: i546get_cmd
 *
 * CALL: i546get_cmd(board, buffer)
 */
i546get_cmd(brd, buf)
	struct i546board *brd;
	struct i546msg *buf;
{
	register struct i546control *ptr;

	ptr = brd->b_addr;
	if (ptr->OutQTail == ptr->OutQHead)
		return(0);	/* return 0 if queue is empty */
	copy_bytes(&ptr->OutQ[ptr->OutQHead], buf, sizeof(struct i546msg));
	ptr->OutQHead = (ptr->OutQHead + 1) % i546QSIZE;
	DDEBUG(DMSG, ("i546get_cmd: msg=%s, ln=%x, cnt=%d, ptr=%x\n",
		i546msgs[buf->m_type], buf->m_line, buf->m_cnt, buf->m_ptr));
	return(1);		/* return 1 if success */
}

/*	I033 Start
 * TITLE:	i546tmout
 *
 * CALL:	i546tmout(dummy);
 *
 * INTERFACES:	i546open, callout
 *
 * CALLS:	i546intr, timeout
 *
 * History:
 *
 */
/* ARGSUSED */
void
i546tmout(notused)
caddr_t notused; /* dummy variable that callout will supply */
{
	register time_t diff;
	register int    dev;
	int		s;

	for (dev=0; dev < N546; dev++) {
		if( ! (i546board[dev].b_alive & ALIVE))
			continue;
		diff = time - i546_time[dev];
		if (diff > MAXTIME)  {
			/*
			 * Assume we missed an interrupt:
			 * force output of next char
			 */
			s = SPL();    /* I036 - mask out other interrupts */
			i546intr(i546cfg[dev].c_level);
			splx(s);      /* I036 - restore interrupt to previous
					 priority level */
		}
	}
	i546tmrun = 1;
	(void) timeout(i546tmout, (caddr_t)0, HZ);	/* I034 */
}
/* I033 End */

/* do 'cnt' getc's and place the characters into 'buf' */
getcn(cq, buf, cnt)
	register struct clist *cq;
	register char *buf;
	register int cnt;
{
	while (--cnt >= 0)
		*(buf++) = getc(cq);
}

/* I028: cl_to_b() removed */
#ifdef lint
/*
 *	I028: Reference each routine that the kernel does, to keep lint happy.
*/
main()
{
	i546init();
	i546open(0, 0);
	i546read(0);
	i546intr(0);
	i546write(0);
	i546ioctl(0, 0, (caddr_t)0, 0);
	i546close(0, 0);
	return 0;
}
#endif /*lint*/
