/*
 *	Bottom half (hardware manipulation) for 8751 microcontroller ports.
 *
 *	The i8751 interrupt generation has a primitive sort of interrupt logic
 *	with two interesting characteristics:
 *		1)	All interrupt sources are ORed into the interrupt request line.
 *		2)	Output complete requests an interrupt at all times when output
 *			is complete (i.e., level instead of edge triggers int. request).
 *	The upshot of this is that we must disable output complete interrupts
 *	when output is complete.  Failure to do this will cause the interrupt
 *	request to be held, and we will not see input interrupt requests.
 *
 *	The 8751 also has a bug that causes it to remember CSTOP characters and 
 *	ignore them until a CSTART is recieved.  This means the user must sometimes
 *	type a CSTART-CSTOP sequence to get the output to stop.
*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/inline.h"
#include "sys/cmn_err.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/termio.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/sysinfo.h"
#include "sys/ics.h"
#include "sys/i8751.h"

extern int i8751_ports;
extern ushort i8751_base[];
extern struct i8751state i8751_state[];
extern struct tty *serialregister();

struct tty *i8751_tty;

i8751proc(tp, cmd)
register struct tty *tp;
int cmd;
{	struct ccblock *tbuf = &tp->t_tbuf;
	int port = tp - i8751_tty;
	struct ics_struct ics;

	switch (cmd) {
	case T_OUTPUT:	/* Start output if possible */
		ics.slot_id = MY_SLOT_ID;
		ics.count = 1;
		if (tp->t_state&(BUSY|TIMEOUT|TTSTOP)) {
			if (!(tp->t_state&BUSY)&&(i8751_state[port].state&i8751_OUTPUT)) {
				/* No longer doing output, need to disable i8751_XMT_EMPTY */
				ics.reg_id = i8751_int_enable(port);
				ics.buffer[0] = i8751_RCV_FULL;
				ics_rw(WRITE_ICS, &ics, KERNEL);
				i8751_state[port].state &= ~i8751_OUTPUT;
			}
			break;		/* Output being delayed or in progress */
		}
		if (tp->t_state & TTXON) {
			tp->t_state &= ~TTXON;
			ics.buffer[0] = CSTART;
		} else {
			if (tp->t_state & TTXOFF) {
				tp->t_state &= ~TTXOFF;
				ics.buffer[0] = CSTOP;
			} else {
				/* Otherwise, just try to initiate more output */
				if (tbuf->c_ptr == NULL || tbuf->c_count == 0) {
					if (!(CPRES & (*linesw[tp->t_line].l_output)(tp))) {
						if (i8751_state[port].state&i8751_OUTPUT) {
							/* Need to disable i8751_XMT_EMPTY */
							ics.reg_id = i8751_int_enable(port);
							ics.buffer[0] = i8751_RCV_FULL;
							ics_rw(WRITE_ICS, &ics, KERNEL);
							i8751_state[port].state &= ~i8751_OUTPUT;
						}
						break;
					}
				}
				ics.buffer[0] = *(tbuf->c_ptr++);
				if (--tbuf->c_count == 0)
					tbuf->c_ptr -= tbuf->c_size;
			}
		}
		ics.reg_id = i8751_data_out(port);
		ics_rw(WRITE_ICS, &ics, KERNEL);
		if (!(i8751_state[port].state & i8751_OUTPUT)) {
			ics.reg_id = i8751_int_enable(port);
			ics.buffer[0] = i8751_RCV_FULL | i8751_XMT_EMPTY;
			ics_rw(WRITE_ICS, &ics, KERNEL);
			i8751_state[port].state |= i8751_OUTPUT;
		}
		tp->t_state |= BUSY;	/* Device is active */
		break;
	case T_TIME:				/* Timeout complete */
		tp->t_state &= ~TIMEOUT;
		i8751proc(tp, T_OUTPUT);
		break;
	case T_SUSPEND:
		tp->t_state |= TTSTOP;	/* Stop after next character */
		break;
	case T_RESUME:
		tp->t_state &= ~TTSTOP;	/* Resume after T_SUSPEND */
		i8751proc(tp, T_OUTPUT);
		break;
	case T_BLOCK:				/* Block input */
		tp->t_state &= ~TTXON;	/* Don't send XON, if still waiting to */
		tp->t_state |= TBLOCK;	/* Input is blocked */
		tp->t_state |= TTXOFF;	/* Rig to send CSTOP at next opportunity */
		i8751proc(tp, T_OUTPUT);/* Send CSTOP now if possible */
		break;
	case T_UNBLOCK:				/* Unblock input */
		tp->t_state &= ~(TTXOFF|TBLOCK); /* Cancel CSTOP, unlock input */
		tp->t_state |= TTXON;	/* Rig to send CSTART at next opportunity */
		i8751proc(tp, T_OUTPUT);/* Send CSTART now if possible */
		break;
	case T_RFLUSH:				/* Flush input */
		if (tp->t_state&TBLOCK)
			i8751proc(tp, T_UNBLOCK); /* We don't save any, so just unblock */
		break;
	case T_WFLUSH:				/* Flush output */
		/*
		 *	Renormalize c_ptr so the buffer can be re-used.
		*/
		if (tbuf->c_ptr && tbuf->c_count) {
			tbuf->c_ptr -= tbuf->c_size-tbuf->c_count;
		}
		tbuf->c_count = 0;
		i8751proc(tp, T_RESUME);
		break;
	case T_BREAK:				/* Send a break */
		break;
	case T_INPUT:				/* Start input */
		break;
	case T_DISCONNECT:			/* Close connection */
		ics.slot_id = MY_SLOT_ID;
		ics.reg_id = i8751_int_enable(port);
		ics.count = 1;
		ics.buffer[0] = 0;
		ics_rw(WRITE_ICS, &ics, KERNEL);
		break;
	case T_PARM:				/* Set line parameters */
		/*
		 *	The hardware can change baud rates, but we don't do it here.
		 *	This is because the 8751 has already sensed the correct line
		 *	characteristics, and higher levels keep trying to set 9600
		 *	baud, right or not.
		*/
		break;
	case T_SWTCH:				/* What does this do? */
		break;
	default:					/* "Cannot happen" */
		cmn_err(CE_NOTE, "8751 unit %d: unknown cmd 0x%x\n", tp-i8751_tty, cmd);
	}
}

/*
 *	Process an interrupt from one of the i8751 ports.
*/
/*ARGSUSED*/
i8751intr(lvl)
{	register struct tty *tp;
	int i, status;
	struct ics_struct ics;

	for (i = 0, tp = i8751_tty; i < i8751_ports; i++, tp++) {
		ics.slot_id = MY_SLOT_ID;
		ics.reg_id = i8751_status(i);
		ics.count = 1;
		ics_rw(READ_ICS, &ics, KERNEL);
		status = ics.buffer[0];
		/*
		 * Check output first, since input affects XMT_EMPTY (by
		 * initiating echo).
		*/
		if ((status&i8751_XMT_EMPTY) && (tp->t_state&BUSY)) {
			sysinfo.xmtint++;
			tp->t_state &= ~BUSY;		/* Output to controller complete */
			i8751proc(tp, T_OUTPUT);	/* Start some more output */
		}
		if (status & i8751_RCV_FULL) {
			char	lbuf[3], c;	/* local character buffer */
			short	lcnt;		/* count of chars in lbuf */

			sysinfo.rcvint++;
			/* get the character */
			ics.reg_id = i8751_data_in(i);
			ics_rw(READ_ICS, &ics, KERNEL);
			c = *(ics.buffer);
			lcnt = 1;	/* Default to input one byte */
			lbuf[0] = c;
			if (tp->t_iflag&ISTRIP)
				lbuf[0] &= 0177;
			else {
				if (c == 0377 && tp->t_iflag&PARMRK) {
					lbuf[1] = 0377;
					lcnt = 2;
				}
			}
			c &= 0177;	/* Make further tests easy */
			if (tp->t_iflag & IXON) {
				if (tp->t_state & TTSTOP) {
					/* got a ctl-q or resume char */
					if (c == CSTART || tp->t_iflag&IXANY)
						i8751proc(tp, T_RESUME);
				} else {
					/* not stopped yet */
					if (c == CSTOP)
						i8751proc(tp, T_SUSPEND);
				}
				if (c == CSTART || c == CSTOP)
					return; /*throw away*/
			}
			if (tp->t_rbuf.c_ptr != NULL) {
				while (lcnt) {
					*tp->t_rbuf.c_ptr = lbuf[--lcnt];
					tp->t_rbuf.c_count--;
					(*linesw[tp->t_line].l_input)(tp, L_BUF);
				}
			}
		}
	}
}

/*
 *	Request a connection to a remote device.  CARR_ON indicates that such
 *	a connection exists.
*/
void
i8751connect(tp)
register struct tty *tp;
{	int port = tp - i8751_tty;
	struct ics_struct ics;

	ics.slot_id = MY_SLOT_ID;
	ics.reg_id = i8751_int_enable(port);
	ics.count = 1;
	ics.buffer[0] |= i8751_XMT_EMPTY;
	ics_rw(WRITE_ICS, &ics, KERNEL);
	tp->t_state |= CARR_ON;		/* Fake carrier detect */
}

/*
 *	This is used only by the kernel for error messages.
*/
i8751co(c)
char	c;
{	struct ics_struct ics;

	intr_disable();
	ics.slot_id = MY_SLOT_ID;
	ics.reg_id = i8751_status(0);		/* Console wired to 8751 port 0 */
	ics.count = 1;
	while (ics_rw(READ_ICS, &ics, KERNEL), !(*ics.buffer & i8751_XMT_EMPTY))
		tenmicrosec();					/* Wait until xmit buffer is empty */
	ics.reg_id = i8751_data_out(0);		/* Console wired to 8751 port 0 */
	ics.buffer[0] = c;
	ics_rw(WRITE_ICS, &ics, KERNEL);	/* Output the character */
	if (c == '\n')
		i8751co('\r');					/* Turn LF into LF-CR */
	intr_restore();
}

/*
 *	This is used by floppy kernels at boot time.
*/
int
i8751ci()
{	struct ics_struct ics;

	intr_disable();
	ics.slot_id = MY_SLOT_ID;
	ics.reg_id = i8751_status(0);		/* Console wired to 8751 port 0 */
	ics.count = 1;
	while (ics_rw(READ_ICS, &ics, KERNEL), !(*ics.buffer & i8751_RCV_FULL))
		return(-1);						/* No character available */
	ics.reg_id = i8751_data_in(0);		/* Console wired to 8751 port 0 */
	ics_rw(READ_ICS, &ics, KERNEL);		/* Input the character */
	intr_restore();
	return(ics.buffer[0]);				/* Return the character */
}

i8751init()
{	int i;

	i8751_tty = serialregister(i8751_ports, i8751proc, i8751connect,
							   i8751ci, i8751co);
	for (i = 0; i < i8751_ports; i++) {
		/*
		 *	Probe for each port, remember if it was alive
		*/
		i8751_state[i].reg = ics_find_rec(MY_SLOT_ID, i8751_base[i]);
		if (i8751_state[i].reg != 0xFFFF)
			i8751_state[i].state |= i8751_ALIVE;
	}
}

#ifdef lint
/*
 *	Reference each routine that the kernel does, to keep lint happy.
*/
main()
{
	i8751init();
	i8751co((char)i8751ci());
	i8751open(0, 0);
	i8751read(0);
	i8751intr(0);
	i8751write(0);
	i8751ioctl(0, 0, (caddr_t)0, 0);
	i8751close(0, 0);
	return 0;
}
#endif /*lint*/
