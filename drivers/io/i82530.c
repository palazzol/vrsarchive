/*
 * Copyright (C) 1983, 1984, 1985  Intel Corp.
 * 
 * INTEL CORPORATION PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with Intel Corporation and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */
char i82530copyright[] = "Copyright 1983, 1984, 1985 Intel Corp.";

/*
 * THIS FILE CONTAINS CODE WHICH IS SPECIFIC TO THE
 * INTEL 286/100 COMPUTER AND MAY REQUIRE MODIFICATION
 * WHEN ADAPTING XENIX TO NEW HARDWARE.
 */

/*
 * Intel 82530 SCC device driver for a 286/100 onboard device.
 *
 *
 * This set of procedures implements the 82530 device driver. 
 * Procedures provided:
 *	i530init -- initialize devices
 *	i530open -- open a line
 *	i530close - close a line
 *	i530intr -- service an 82530 interrupt
 *	i530proc -- handle I/O on the specified line
 *	i530read -- starts a read from the specified line
 *	i530write - starts a write to the specified line
 *	i530ioctl - device level control interface
 *
 * Internal routines are:
 *	i530param - parameter interface
 *	i530start -- start output on the specified channel
 *	i530wakeup - wakeup process sleeping on tty state (i530proc)
 *
 * Hardware configuration:
 *	the 82530 has the following:
 *		2 channels (somewhat independent)
 *		1 level on an 8259 interrupt controller
 *
 * Modems are not supported by this driver.
 *
 * Note on debugging this driver: 
 *	Because the 82530 on-board the 286/100 is also the
 *	the console (monitor uses channel B) there are 
 *	extreme problems with printing out info on channel
 *	B while trying to debug this driver, thus was born
 *	tprintf (see tdebug.c). When recompiled with DEBUGA on
 *	the output rate is SUBSTANTIALLY reduced; do not be
 *	alarmed. To view the debug trace, display the contents
 *	of trbuf via the D command.
 *
 * Author: P Barrett
 *
 *
 *  Modification History:
 *
 *  12/8/83 	llk	I001
 *	Various improvements in the code to insure that the chip,
 *	which is fairly flaky, doesn't hang or produce garbage.
 *	Also various improvements for clarity of code.
 * 
 *  12/8/83	llk	I002
 *	Added to fix the control-S, control-Q problem--namely,
 *	control-S was hanging the channels.
 *
 *  12/8/83	llk	I003
 *	System was hanging when going multi-user if both channels
 * 	were enabled.  The problem: the SAV bit for channel B
 *	affects channel A as well; therefore, don't turn it off
 * 	when closing the channel.
 *
 *  12/8/83	llk	I004
 *	Removed redundant call to ttyclose routine.
 *
 *  12/8/83	llk	I005
 *	Mutex in w530 and r530.  
 *
 *  12/20/83	llk	I006
 *	Ported to system III kernel
 *
 *  01/09/84	llk	I007
 *	Removed multiple 8274 support for efficiency
 *	Recoded w530 and r530 in assembler
 *
 *  01/09/84	llk	I008
 *	Added parity support
 *
 *  05/15/85	bjm
 *	rewritten to support the 82530 on the 286/100 MB II board
 *
 */

#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/systm.h"
#include "../h/conf.h"
#include "../h/tty.h"
#include "../h/buf.h"
#include "../h/i82530.h"
#include "../h/i8259.h"
#include "../h/ioctl.h"

#ifdef lint
/* ARGSUSED */
/* VARARGS1 */
void tprintf(s) char *s; {}
#endif lint

struct	tty	i530tty[2];		/* 2 channels per chip (I006 renamed) */
extern	struct	i530cfg i530cfg[];	/* port addrs for 82530 (I006 renamed) */
int	i530alive;			/* is it alive? */

/*
 * This procedure initializes the 82530 in the system when called
 * from dinit early in system initialization. It performs the 
 * following:
 *	initialize the 82530 so that all that is needed
 *	is to enable the transmitter and receiver
 *
 * TITLE:	i530init
 *
 * CALL:	i530init()
 *
 * INTERFACES:	dinit
 *
 * CALLS:	w530 -- program 82530 register
 *
 * change history:
 *
 */

i530init(){
	unsigned	porta, portb;
	unsigned	tc_low_a, tc_hi_a, tc_low_b, tc_hi_b;
	
#ifdef	DEBUGA
	tprintf("Config table: %x %x %x\n",
		i530cfg[CH_A].m_ctrl,	i530cfg[CH_A].m_data,
		i530cfg[CH_A].m_intlev );
#endif


	porta = i530cfg[CH_A].m_ctrl;
	portb = i530cfg[CH_B].m_ctrl;

	/*
	 * we will reset the 82530 and then initialize it with the
	 * same parameters as the iSDM 286. To do this we must first
	 * save the current BAUD rate counters.
	 */
	tc_low_a = r530(porta, M_REG12);
	tc_hi_a  = r530(porta, M_REG13);
	tc_low_b = r530(portb, M_REG12);
	tc_hi_b  = r530(portb, M_REG13);

	/*
	 *	Delay 1 character time before the 82530 is reset, to
	 *	allow any kernel printf to complete.  If the console
	 *	is not the first init routine called, the last character
	 *	from the previous 'device found' message is still in
	 *	transit.
	*/
	delay(10);	/* This should be enough for low baud rates */

	/*
	 *   I001
	 * software reset to 82530 because hardware
	 * reset doesn't necessarily reset the 82530.
	 */
	w530(porta, M_REG9,M_RESET);	
	delay(1);	/* Need some extra delay here - vrs */

	/*
	 * write vector to channel A w reg 2,
	 * read it back.
	 * if same as written, chip exists.
	 */
	w530(porta,M_REG2,TEST_VECT);
	if( r530(porta,M_REG2) == TEST_VECT)
	{
		/*
		 *   I001
		 */
		/*
		 * initialize 82530 with iSDM parameters
		 *
		 * the master interrupt enable bit is set, but all
		 * interrupt sources are masked until a channel is opened.
		 */

		w530(portb,M_REG4,M_1STOP|M_16X);
		/*
		 *	We enable interrupts on port B (only) because
		 *	if we dont, the monitor will find the console
		 *	disabled when we try to do kernel debugging.
		*/
		w530(portb,M_REG1,M_RX_INT|M_TX_INT_EN|M_EXT_EN);
		w530(portb,M_REG2, 0);
		w530(portb,M_REG3,M_RX_8BPC|M_RX_EN);
		w530(portb,M_REG5,M_TX_8BPC|M_RTS|M_DTR|M_TX_EN);
		w530(portb,M_REG6,0);
		w530(portb,M_REG7,0);
		w530(portb,M_REG10,0);
		w530(portb,M_REG11,M_RX_TX_CLKS);
		w530(portb,M_REG14, M_BRG_SRC);
		w530(portb,M_REG12, tc_low_b);
		w530(portb,M_REG13, tc_hi_b);
		w530(portb,M_REG14, M_BRG_SRC|M_BRG_EN);
		w530(portb,M_REG15,0);

		w530(porta,M_REG4,M_1STOP|M_16X);
		w530(porta,M_REG1,0); /* disable channel interrupts */
		w530(porta,M_REG3,M_RX_8BPC|M_RX_EN);
		w530(porta,M_REG5,M_TX_8BPC|M_TX_EN);
		w530(porta,M_REG6,0);
		w530(porta,M_REG7,0);
		w530(porta,M_REG10,0);
		w530(porta,M_REG11,M_RX_TX_CLKS);
		w530(porta,M_REG14, M_BRG_SRC);
		w530(porta,M_REG12, tc_low_a);
		w530(porta,M_REG13, tc_hi_a);
		w530(porta,M_REG14, M_BRG_SRC|M_BRG_EN);
		w530(porta,M_REG15,0);

		/* enable interrupts from the chip, however all sources
		 * are masked until an open.
		 * also set vector includes status and non vectored mode.
		 */
		w530(portb,M_REG9,M_NV|M_MIE);
		delay(1);	/* Need some extra delay here - vrs */

		i530alive=1;
		printf("Console Found\n");

		/*
		 * flush possible garbage out of input FIFOs
		 */
		while ( (r530(porta,M_REG0) & M_CHAR_AV) != 0 )
			r530(porta,M_REG8);

		while ( (r530(portb,M_REG0) & M_CHAR_AV) != 0 )
			r530(portb,M_REG8);

	}
	else
	{
		i530alive=0;
		printf("82530 NOT Found\n");
	}
};


/*
 * This routine sets up the baud rate and the parameter of the device.
 * The tty structure must have been filled out prior to calling this
 * routine. The algorithm:
 *	- validate speed
 *	- program the timer
 *	- program for the desired parameters
 *
 * TITLE:	i530param
 *
 * CALL:	i530param(dev)
 *
 * INTERFACES:	i530ioctl, i530open	
 *
 * change history:
 *	I006	llk	1/10/84
 *	added support for parameter change
 *
 */
#define	MAXBAUDS	15
int	i530baud[] = {
	i530_B0,	i530_B50,	i530_B75,	i530_B110,
	i530_B134,	i530_B150,	i530_B200,	i530_B300,
	i530_B600,	i530_B1200,	i530_B1800,	i530_B2400,
	i530_B4800,	i530_B9600,	i530_B19200,	0};
int	i530speed[2];		/* track record */

int	i530bpc[4] = {0, M_RX_6BPC, M_RX_7BPC, M_RX_8BPC};

i530param(dev)
dev_t	dev;
{
	register unsigned port;		/* port to reprogram */
	struct	tty *tp;
	register int s;			/* new speed and mutex var */
	int	unit,
		speed;
	short	f;			/* used for misc temps */
	int	bpc,cmd4;		/* commands to send to port reg */
	int	tc;			/* 82530 time constant */

	unit = dev & MINORMSK;
	port = i530cfg[unit].m_ctrl;	/* port address of 82530 */
	tp = &i530tty[unit];
	s = (int)tp->t_cflag&CBAUD;	/* I006 new tty struct field */
#ifdef	DEBUGA
	tprintf("i530param: d=%x,u=%x,p=%x,tp=%x,s=%x\n",
		dev,unit,port,tp,s );
#endif
	if(s==0)
	{
		/* hang up signal -- modems not supported */
		u.u_error = ENXIO;
		return;
	}

	if(s != i530speed[unit])	/* if speed change not redundant... */
	{
		i530speed[unit] = s;	/* set to new speed */
	
		/* test for valid speed */
		if (s >= MAXBAUDS)			/* exceeds table */
		{
			u.u_error = EINVAL;		/* early retirement */
			return;
		}
		speed = i530baud[s];	/* speed <-- actual BAUD rate */
		if (speed == 0)		/* if invalid entry */
		{
			u.u_error = EINVAL;		/* early retirement */
			return;
		}
		s = splcli();
		w530(port, M_REG14, M_BRG_SRC); /* disable BAUD rate generator */
		/*
		 * the following formula is taken from the 82530 technical
		 * manual. It is correct for a 16x clock and an SCC clock
		 * of 9.8304 MHz.
		 */
		tc = (BRG_CONSTANT / (long) speed ) -2;
		w530(port, M_REG12, LOW(tc));
		w530(port, M_REG13, HIGH(tc));
		w530(port, M_REG14, M_BRG_SRC|M_BRG_EN);
		splx(s);
	}

	/* 
	 * I006 set up parameters
	 */
	
	f = tp->t_cflag;	/* control flag */

	/* calculate mode control command wr4 */
	cmd4 = M_16X | ((f&CSTOPB)? M_2STOP:M_1STOP);	/* clock, stop */
	if (f& PARENB)
		cmd4 |= (M_PAR_EN |((f&PARODD) ? 0:M_PAR_EVEN));/* parity */

	/* calculate receive bpc for wr3 */
	/* transmit bpc for wr5 is (bpc >> 1) */
	bpc = i530bpc[(f>>4) &0x3];	

	s = splcli();
	w530(port,M_REG4,cmd4);
	w530(port,M_REG1,M_RX_INT|M_TX_INT_EN|M_EXT_EN); /* enable interrupts */
	w530(port,M_REG3, bpc | ((f&CREAD) ? M_RX_EN:0));
	w530(port,M_REG5, (bpc>>1) | M_RTS|M_DTR| M_TX_EN);
	splx(s);

}


/*
 * This procedure opens the specified line. The file structure is
 * initialized and the tty open routine is called.
 *
 * TITLE:	i530open
 *
 * CALL:	i530open(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	i530param, ttyopen (from lineswitch), w530
 *
 * change history:
 *
 */
int	i530proc();
/* ARGSUSED */
i530open(dev,flag)
dev_t	dev;
int	flag;
{
	register struct	tty *tp;		/* tty structure ptr */
	register int	unit;

	unit = dev & MINORMSK;		/* unit <-- minor code*/
#ifdef	DEBUGA
	if(unit&01) 	/* check only channel B */
		tprintf("i530open:d=%x,u=%x\n", dev, unit );
#endif

	if(unit > 1){
		u.u_error = ENXIO;		/* outside table */
		return;
	}

	if(i530alive == 0)			/* alive? */
	{
		u.u_error = ENXIO;		/* nope */
		return;
	}

	tp = &i530tty[unit];			/* get tty struct */
	tp->t_addr = unit;
	tp->t_proc = i530proc;
	if(dev & MODEMMSK){
		u.u_error = ENXIO;	/* no modem support */
		return;
	}

	if((tp->t_state & ISOPEN) == 0)	/* if not already open, */
	{
		/* I006 --next few lines modified for new tty struct */
		ttinit(tp);		/* set up chars */
		tp->t_oflag = OPOST|ONLCR;
		tp->t_iflag = ICRNL|ISTRIP|IXON;
		tp->t_lflag = ECHO|ICANON|ISIG;
		tp->t_cflag = B9600|CS8|CREAD|HUPCL;
		i530param(dev);		/* set baud rate */
	};

	/*
	 * somebody else got exclusive use?
	 */
	if( (tp->t_lflag & XCLUDE) && (u.u_uid != 0) )
	{
		u.u_error = EBUSY;	/* Yup!, sorry charlie */
		return;
	}

	tp->t_state |= CARR_ON;
	(*linesw[tp->t_line].l_open)(tp);

}



/*
 * This procedure closes the specified line. It turns off the ability
 * of the device to receive or transmit characters and generate
 * interrupts. ttyclose (lineswitch) is called to do machine indep-
 * endent close actions.
 *
 * TITLE:	i530close
 *
 * CALL:	i530close(dev,flag)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	ttyclose (lineswitch)
 *
 * change history:
 *
 */
/* ARGSUSED */
i530close(dev,flag)
dev_t	dev;
int	flag;	/* unused */
{
	register struct	tty *tp;	/* tty structure */
	register unit;
	int	s;

	unit = dev & MINORMSK;
	tp = (struct tty *)&i530tty[unit];
#ifdef	DEBUGA
	if(unit&01) /* test for channel B */
	tprintf("i530close:d=%x,u=%x,tp=%x\n",
		dev,unit,tp);
#endif
	if(unit <= 1)		/* valid unit number?? */
	{			/* yes! */
		if(tp->t_cflag & HUPCL) {	/* hang up signal */
			tp->t_state &= ~CARR_ON;
		}
		(*linesw[tp->t_line].l_close)(tp);   /* I004 */
		/*
		 * turn off TX Interrupt Enable and RX Enable
		 */
		s = splcli();
		w530(i530cfg[unit].m_ctrl,M_REG1,M_RX_INT);   /* I003 */
		w530(i530cfg[unit].m_ctrl,M_REG3,0);
		splx(s);
	}
	tp->t_addr = 0;
}


/*
 * This procedure starts a read from the specified device.
 *
 * TITLE:	i530read
 *
 * CALL:	i530read(dev)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	ttread (via lineswitch)
 *
 * change history:
 *
 */
i530read(dev)
dev_t	dev;
{
	register struct	tty *tp;	/* tty structure */
	register int	unit;		/* unit number */

	unit = dev & MINORMSK;	/* get unit number */
	tp = (struct tty *) &i530tty[unit];
#ifdef	DEBUGA
	if(unit&01) /* test for channel B */
	tprintf("i530read:d=%x,u=%x,tp=%x\n", dev,unit,tp );
#endif
	(*linesw[tp->t_line].l_read)(tp);	/* l_read does read */
}


/*
 * This procedure starts a write to the specified device.
 *
 * TITLE:	i530write
 *
 * CALL:	i530write(dev)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	ttwrite (via lineswitch)
 *
 * change history:
 *
 */
i530write(dev)
dev_t	dev;
{
	register struct	tty *tp;	/* tty structure */
	register int	unit;		/* unit number */

	unit = dev & MINORMSK;	/* get unit number */
	tp = (struct tty *) &i530tty[unit];
#ifdef	DEBUGA
	if(unit&01) /* test for channel B */
	tprintf("i530write:d=%x,u=%x,tp=%x\n", dev,unit,tp );
#endif
	(*linesw[tp->t_line].l_write)(tp);	/* l_write does write */
}

/* debug variables */
#ifdef 	DEBUGA
int	u0_74 = 0;
int	u1_74 = 0;
int	tx_74 = 0;
int	sp_74 = 0;
int	si_74 = 0;
int	ex_74 = 0;
int	rx_74 = 0;
int	sr_74 = 0;
int	de_74 = 0;
int	to_74 = 0;
#endif
/*
 * This procedure is called by xenix with interrupts off (splcli??)
 * when the 82530 interrupts the processor. The 82530 specified by the
 * interrupt level is polled. Since the 82530 has two channels, a
 * determination is made as to which channel interrupted. The appro-
 * priate tty structure is obtained and the lineswitch ttystart is
 * called (which in turn calls i530proc). 
 *
 * TITLE:	i530intr
 *
 * CALL:	i530intr(level)
 *
 * INTERFACES:	xenix
 *
 * CALLS:	line discipline input routine; i530start,inb,w530,r530
 *
 * change history:
 *	12/8/83	llk	restructured the routine for clarity
 *			(and correctness in unlikely cases).
 *			also made vars above into #ifdef DEBUGA.
 *	1/11/84 llk	added parity handling
 *
 * algorithm:
 *	scan i530cfg for appropriate entry
 *	unit <-- index of entry
 * 	if channel B int then
 * 		unit <-- unit+1
 *	tp <-- 82530tty[unit]
 * 	switch on interrupt vector
 * 	case Tx buffer empty:
 *		if channel not open
 *			reset transmit interrupts
 *			send end of interrupt 
 *			break
 *		start ouput on line
 *		wakeup procs sleeping on ouput queue if low water 
 *		send end of interrupt
 * 	case ext/stat change:
 *		reset external interrupts
 *		send end of interrupt
 * 	case Rx ready:
 *		input the char
 *		reset external interrupts
 *		if channel not open
 *			break
 *		call lineswitch interrupt routine
 * 	case spec Rx:
 *		if not interrupt pending
 *			return
 *		input the char
 *		mask error type into upper bits of char
 *		reset error
 *		send end of interrupt
 *		if channel not open
 *			break
 *		call lineswitch interrupt routine
 * 	default:		impossible
 *
 */
int	wakeup();
i530intr(level)
int	level;
{
	struct	i530cfg	*mp_a;		/* I/O address */
	register struct	tty *tp;	/* tty structure */
	register struct	i530cfg *mp;	/* I/O addresses */
	int	c;
	int	rr;			/* read reg */
	int	unit;

#ifdef DEBUG
	if (! (inb(0xC2) & (1<<level)))
		printf("error --- level %d not masked\n",level);
#endif

	mp_a = (struct i530cfg *)&i530cfg[CH_A];
	
	if(mp_a->m_intlev != level)
	{
		/* error, not valid level */
		printf("82530 interrupt error - level=%x\n",level);
#ifdef	DEBUGA
		tprintf("invalid interrupt level\n");
		sp_74++;
#endif
		/* make sure intr are back on */
		/* reset external intr */
		w530(mp_a->m_ctrl,M_REG0,M_RS_EX_INT);
		/* reset highest IUS */
		w530(mp_a->m_ctrl,M_REG0,M_EOI);
		return;
	}

	/* loop through all interrupts currently active on the chip */
	while ( r530(mp_a->m_ctrl,M_REG3) != 0) {

		mp = mp_a;
		/* ch. B rr2 status in vector */
		rr = r530(i530cfg[CH_B].m_ctrl,M_REG2);
		/*
		 * determine which channel we are talking to
		 */
		if((rr&M_CHA) == 0) {
			unit = 1;	/* channel B interrupt */
			mp++;
#ifdef	DEBUGA
			u1_74++;
#endif
		} else {
			unit = 0;	/* channel A interrupt */
#ifdef	DEBUGA
			u0_74++;	
#endif
		}
#ifdef	DEBUGA
		tprintf("i530intr: rr=%x\n", rr );
		tprintf("unit=%d mp=%x mp_a=%x\n",unit,mp,mp_a);
#endif
	
		tp = &i530tty[unit];	/* tp <-- pointer to tty struct */
	
		/* I001: moved check for tty open into switch */

		switch( (rr>>1) & 0x7 )	/* on interrupt vector in SR 2 */
		{
		case 0:	/* channel B, Transmitter buffer empty */
		case 4:	/* channel A, Transmitter buffer empty */
			w530(mp->m_ctrl,M_REG0,M_RS_TX_INT);

			/*
			 * Code from I001 moved [vrs]:
			 *	Want BUSY bit to reflect hardware state.
			 *	Want to count all tx interrupts w/DEBUGA
			*/
			tp->t_state &= ~BUSY;		/* clear busy bit */
#ifdef	DEBUGA
			tx_74++;	/* temp */
#endif
			/* check if chan open  (From I001 above) [vrs] */
			if(((tp->t_state&ISOPEN)==0) || (tp->t_state & TTSTOP))
				break;


			/* the following call to start routine should probably
			 * be made through i530proc (with the parameter 
			 * T_OUTPUT) for modularity.  However, in the interest
			 * of efficiency... 
			 */
			i530start(tp);			/* start output */

			/*
			 * if proc is sleeping on outq and low water mark is
			 * reached, wake up proc so he can put more chars into
			 * the output queue
		 	 */
			if((tp->t_state & OASLP) &&
			   (tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD])) {
				tp->t_state &= ~OASLP;	/* clear sleep bit */
				wakeup((caddr_t)&tp->t_outq);
			}
			break;
		
		case 1:	/* channel B, ext/status change */
		case 5:	/* channel A, ext/status change */
#ifdef	DEBUGA
			ex_74++;
#endif
			w530(mp->m_ctrl,M_REG0,M_RS_EX_INT);
			break;
		case 2:	/* channel B, Receiver Ready */
		case 6:	/* channel A, Receiver Ready */
			do {
#ifdef	DEBUGA
				rx_74++;
#endif
				c = inb(mp->m_data) & 0xFF ;
				if((tp->t_state&ISOPEN) == 0)  break; /* I001 */
				(*linesw[tp->t_line].l_input)(tp,c,0);
			} while ( (inb(mp->m_ctrl) & M_CHAR_AV) != 0);
	
			break;
		
		case 3:	/* channel B, Special Receive */
		case 7:	/* channel A, Special Receive */
#ifdef	DEBUGA
			sr_74++;
#endif

			/* check if intr pending or not  I001  */
			if( (r530(mp_a->m_ctrl,M_REG0)&INT_PENDING) == 0) {
#ifdef	DEBUGA
				sp_74++;
#endif
				/* reset external intr */
				w530(mp_a->m_ctrl,M_REG0,M_RS_EX_INT);
				/* reset highest IUS */
				w530(mp_a->m_ctrl,M_REG0,M_EOI);
				return;
			}

			/* I001: spec rcv, 
			 * but input and pass it to the tty rtns anyway
			 */
			c = inb(i530cfg[unit].m_data) & 0xFF;
	
			/* I006: mask error code into char */
			rr = r530(mp->m_ctrl, M_REG1);
			if (rr & M_PERROR)
				c |= PERROR;
			if (rr & M_FRERROR)
				c |= FRERROR;
			if (rr & M_OVERRUN)
				c |= OVERRUN;

			w530(mp->m_ctrl,M_REG0,M_ERR_RES);

			if((tp->t_state&ISOPEN) == 0)  break;    /* I001 */

			(*linesw[tp->t_line].l_input)(tp,c,0);
			break;
		default:
#ifdef	DEBUGA
			de_74++;
#endif
			/* error condition -- can't happen */
			break;
		}
	}
	w530(mp_a->m_ctrl,M_REG0,M_RS_EX_INT);	/* reset external intr */
	w530(mp_a->m_ctrl,M_REG0,M_EOI);	/* reset highest IUS */
}


/*
 * This procedure handles I/O functions.  It is called at both
 * task time by the line discipline routines, and at interrupt time
 * by i530intr().  (NOTE: at this time, i530intr() calls i530start()
 * directly.  It does not pass through this routine, since this routine
 * does nothing to start output before calling i530start().  This is
 * done this way in the interest of efficiency, since we interrupt on
 * each and every character that is outputted).  
 * i530proc handles any device dependent functions required
 * upon suspending, resuming, blocking, or unblocking output; flushing
 * the input or output queues; timing out; sending break characters,
 * or starting output. 
 *
 * TITLE:	i530proc
 *
 * CALL:	i530proc(tp,cmd)
 *
 * INTERFACES:	xenix (line discipline routines), i530intr
 *
 * CALLS:	i530start
 *
 * change history:
 *	llk	I006
 * 		Added this routine.
 *
 */
int i530wakeup();

i530proc(tp,cmd)
register struct tty *tp;
int	cmd;
{
	register struct	i530cfg *mp;
	int bpc;

	mp = (struct i530cfg *) &i530cfg[(int)tp->t_addr];

	switch(cmd)  {
	case T_RFLUSH:			/* flush input queue */
	    return;
	case T_WFLUSH:			/* flush output queue */
	case T_RESUME:			/* resume output */
	    tp->t_state &= ~TTSTOP;
	    i530start(tp);			/* start output */
	    return;
	case T_SUSPEND:			/* suspend output */
	    tp->t_state |= TTSTOP;
	    tp->t_state &= ~BUSY;	/* output no longer in progress */
	    /*
	     * this line has been moved to the interrupt routine,
	     * because we want to make sure the current char is all the
	     * way out the door before we turn off transmitter intrs.
	     * w530(mp->m_ctrl,M_REG0,M_RS_TX_INT);
	     */
	    return;
	case T_BLOCK:			/* send stop char */
	    if (putc(CSTOP, &tp->t_outq) == 0)  {
		tp->t_state |= TBLOCK;
		i530start(tp);
	    }
	    return;
	case T_UNBLOCK:			/* send start char */
	    if (putc(CSTART, &tp->t_outq) == 0)  {
		tp->t_state &= ~TBLOCK;
		i530start(tp);
	    }
	    return;
	case T_TIME:			/* time out */
	    tp->t_state &= ~TIMEOUT;
	    i530start(tp);
	    return;
	case T_BREAK:			/* send null for .25 sec */
	    bpc = i530bpc[((tp->t_cflag)>>4) &0x3];	
	    /* pull transmitter low */
	    w530(mp->m_ctrl,M_REG5,(bpc>>1)|M_RTS|M_DTR| M_TX_EN|M_BREAK);
	    /* disable receiver  */
	    w530(mp->m_ctrl,M_REG3,bpc);
	    timeout(i530wakeup, (caddr_t)tp, HZ/4);
	    sleep((caddr_t)&tp->t_state);
	    return;
	case T_OUTPUT:			/* start output */
	    i530start(tp);
	};	/* end switch */

}



/*
 * This procedure starts output on an 82530 channel if needed. i530start
 * gets a character from the character queue, outputs it to the 
 * channel and sets the BUSY flag. The BUSY flag gets reset by i530intr
 * when the character has been transmitted.
 * NOTES:
 *	this routine could also implement a 'prescarfing' algorithm
 *	where it gets all the characters from the C-list and places
 *	them in a buffer. The interrupt routine could then just take
 *	the character from the buffer and place it on the 82530 thus
 *	avoiding this routine per interrupt. 
 *
 * TITLE:	i530start
 *
 * CALL:	i530start(tp)
 *
 * INTERFACES:	i530proc
 *
 * CALLS:	splcli, putc, getc, timeout
 *
 * change history:
 *
 */
int ttrstrt();

i530start(tp)
register struct tty *tp;
{
	register int	c;
	struct	i530cfg *mp;
	int		s;

	mp = (struct i530cfg *) &i530cfg[(int)tp->t_addr];
#ifdef	DEBUGA
	if(tp->t_addr&01) /* test for channel B */
	tprintf("i530start:tp=%x,mp=%x\n",tp,mp);
#endif
	s = splcli();			/* no C-list interrupts */
	/* busy or timing out? or stopped??  I002 */
	if(tp->t_state&(TIMEOUT|BUSY|TTSTOP))	
	{
		splx(s);		/* yes, return (wait for */
		return;			/* a more opportune time) */
	}
	tp->t_state |= BUSY;
	splx(s);

	/*
	 * get the character to be output, do it
	 * I006 this block contains mods for sys 3 special character handling
	 */
	if( (c=getc(&tp->t_outq)) >= 0) {
		if((tp->t_oflag & OPOST)==0)	/* raw mode */
			outb(mp->m_data,c);
		else {                    /* cooked */
			if (c==0200) {
			    if((c=getc(&tp->t_outq))<0) return;
			    if(c>0200){
				tp->t_state |= TIMEOUT;
#ifdef DEBUGA
				to_74 ++;
#endif
				/* I002: output no longer in progress */
				tp->t_state &= ~BUSY;	
				w530(mp->m_ctrl,M_REG0,M_RS_TX_INT);
				timeout(ttrstrt, (caddr_t)tp, (c&0177));
				return;
			    }
			}
			outb(mp->m_data, c); 
		}	/* else cooked mode */
	} 
	else {
		/* I002: output no longer in progress */
		w530(mp->m_ctrl,M_REG0,M_RS_TX_INT);
		tp->t_state &= ~BUSY;
	}
	if(tp->t_state&TTIOW && tp->t_outq.c_cc==0) {
		tp->t_state &= ~TTIOW;
		wakeup((caddr_t)&tp->t_oflag);
	}
	if(tp->t_state&OASLP && tp->t_outq.c_cc<=ttlowat[tp->t_cflag&CBAUD]) {
		tp->t_state &= ~OASLP;
		wakeup((caddr_t)&tp->t_outq);
	}
}

/* 
 * This procedure releases the transmitter output.
 * It is used by the TCSBRK ioctl command.  After .25 sec
 * timeout (see case BREAK in i530proc), this procedure is called.
 *
 * TITLE:	i530wakeup
 *
 * CALL:	i530wakeup(addr)
 *
 * INTERFACES:	timeout (through i530proc)
 *
 * CALLS:	wakeup
 *
 * change history:
 *	llk	I006
 * 		Added this routine.
 *
 */
i530wakeup(tp)
struct tty *tp;
{
	struct i530cfg *mp;
	int bpc;

	mp = (struct i530cfg *) &i530cfg[(int)tp->t_addr];
	bpc = i530bpc[((tp->t_cflag)>>4) & 0x3];
	w530(mp->m_ctrl,M_REG5,(bpc>>1)|M_RTS|M_DTR| M_TX_EN);

	/* enable receiver, if supposed to */
	w530(mp->m_ctrl,M_REG3, bpc | ((tp->t_cflag & CREAD) ? M_RX_EN:0));
	wakeup((caddr_t)&tp->t_state);
}


/*
 * This procedure does the ioctls. The hardware dependent changes
 * supported are baud rate change, parity change, number of stop bits
 * number of bits per character, and enable/disable receiver (I008).
 *
 * TITLE:	i530ioctl
 *
 * CALL:	i530ioctl(dev, cmd, addr, flag)
 *
 * INTERFACES:	ioctl
 *
 * CALLS:	i530param, ttiocom
 *
 * change history:
 *
 */
i530ioctl(dev,cmd,addr,flag)
dev_t	dev;
int cmd, flag;
faddr_t	addr;

{
	register struct tty *tp;
	register int	unit;

	unit = dev & MINORMSK;		/* get unit */
	tp = (struct tty *) &i530tty[unit];	/* get tty */

#ifdef	DEBUGA
	tprintf("i530ioctl: d=%x,c=%x,a=%x,f=%x,tp=%x\n",
		dev,cmd,addr,flag,tp );
#endif
	if(ttiocom(tp,cmd,addr,flag)) {	/* do command */
		switch (cmd)  {
		case TIOCSETP:			/* for v7 compat */
		case TIOCSETN:			/* for v7 compat */
		case TCSETA:
		case TCSETAW:
		case TCSETAF:
			i530param(dev);		/* do it */
			break;
		default:
			break;
		}
		return;
	}
}
