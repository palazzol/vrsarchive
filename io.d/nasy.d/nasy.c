/*
 *	Bottom half (hardware manipulation) for PC AT async ports.
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
#include "sys/nasy.h"

extern int nasy_ports;
extern ushort nasy_base[];
extern struct nasystate nasy_state[];
extern struct tty *serialregister();

struct tty *nasy_tty;

nasyproc(tp, cmd)
register struct tty *tp;
int cmd;
{	struct ccblock *tbuf = &tp->t_tbuf;
	int port = tp - nasy_tty;

	switch (cmd) {
	case T_OUTPUT:	/* Start output if possible */
		if (tp->t_state&(BUSY|TIMEOUT|TTSTOP))
			break;		/* Output being delayed or in progress */
		if (tp->t_state & TTXON) {
			tp->t_state &= ~TTXON;
		} else {
			if (tp->t_state & TTXOFF) {
				tp->t_state &= ~TTXOFF;
			} else {
				/* Otherwise, just try to initiate more output */
				if (tbuf->c_ptr == NULL || tbuf->c_count == 0) {
					if (!(CPRES & (*linesw[tp->t_line].l_output)(tp)))
						break;
				}
			}
		}
		/* TBD OUTPUT *(tbuf->c_ptr++) */
		tp->t_state |= BUSY;	/* Device is active */
		if (--tbuf->c_count == 0)
			tbuf->c_ptr -= tbuf->c_size;
		break;
	case T_TIME:				/* Timeout complete */
		tp->t_state &= ~TIMEOUT;
		nasyproc(tp, T_OUTPUT);
		break;
	case T_SUSPEND:
		tp->t_state |= TTSTOP;	/* Stop after next character */
		break;
	case T_RESUME:
		tp->t_state &= ~TTSTOP;	/* Resume after T_SUSPEND */
		nasyproc(tp, T_OUTPUT);
		break;
	case T_BLOCK:				/* Block input */
		tp->t_state &= ~TTXON;	/* Don't send XON, if still waiting to */
		tp->t_state |= TBLOCK;	/* Input is blocked */
		tp->t_state |= TTXOFF;	/* Rig to send CSTOP at next opportunity */
		nasyproc(tp, T_OUTPUT);/* Send CSTOP now if possible */
		break;
	case T_UNBLOCK:				/* Unblock input */
		tp->t_state &= ~(TTXOFF|TBLOCK); /* Cancel CSTOP, unlock input */
		tp->t_state |= TTXON;	/* Rig to send CSTART at next opportunity */
		nasyproc(tp, T_OUTPUT);/* Send CSTART now if possible */
		break;
	case T_RFLUSH:				/* Flush input */
		if (tp->t_state&TBLOCK)
			nasyproc(tp, T_UNBLOCK); /* We don't save any, so just unblock */
		break;
	case T_WFLUSH:				/* Flush output */
		/*
		 *	Renormalize c_ptr so the buffer can be re-used.
		*/
		if (tbuf->c_ptr && tbuf->c_count) {
			tbuf->c_ptr -= tbuf->c_size-tbuf->c_count;
		}
		tbuf->c_count = 0;
		nasyproc(tp, T_RESUME);
		break;
	case T_BREAK:				/* Send a break */
		break;
	case T_INPUT:				/* Start input */
		break;
	case T_DISCONNECT:			/* Close connection */
		/* TBD */
		break;
	case T_PARM:				/* Set line parameters */
		/* TBD */
		break;
	case T_SWTCH:				/* What does this do? */
		break;
	default:					/* "Cannot happen" */
		cmn_err(CE_NOTE, "nasy unit %d: unknown cmd 0x%x\n", tp-nasy_tty, cmd);
	}
}

/*
 *	Process an interrupt from one of the nasy ports.
*/
/*ARGSUSED*/
nasyintr(lvl)
{	register struct tty *tp;
	int i, status;

	for (i = 0, tp = nasy_tty; i < nasy_ports; i++, tp++) {
		status = nasy_status(i);
		/*
		 * Check output first, since input affects XMT_EMPTY (by
		 * initiating echo).
		*/
		if ((status&nasy_TxRDY) && (tp->t_state&BUSY)) {
			sysinfo.xmtint++;
			tp->t_state &= ~BUSY;		/* Output to controller complete */
			nasyproc(tp, T_OUTPUT);	/* Start some more output */
		}
		if (status & nasy_RxRDY) {
			char	lbuf[3], c;	/* local character buffer */
			short	lcnt;		/* count of chars in lbuf */

			sysinfo.rcvint++;
			/* TBD get the character into c */
			/* TBD check for parity error */
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
						nasyproc(tp, T_RESUME);
				} else {
					/* not stopped yet */
					if (c == CSTOP)
						nasyproc(tp, T_SUSPEND);
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
nasyconnect(tp)
register struct tty *tp;
{	int port = tp - nasy_tty;

	/* TBD */
}

/*
 *	This is used only by the kernel for error messages.
*/
nasyco(c)
char	c;
{

	intr_disable();
	while (!(nasy_status(0)&nasy_TxRDY))
		tenmicrosec();					/* Wait until xmit buffer is empty */
	/* TBD put c to device */
	if (c == '\n')
		nasyco('\r');					/* Turn LF into LF-CR */
	intr_restore();
}

/*
 *	This is used by floppy kernels at boot time.
*/
int
nasyci()
{	char c;

	intr_disable();
	if (!(nasy_status(0) & nasy_RxRDY))
		return(-1);						/* No character available */
	c = nasy_data_in(0);				/* Get the character */
	intr_restore();
	return(c);							/* Return the character */
}

nasyinit()
{	int i;

	nasy_tty = serialregister(nasy_ports, nasyproc, nasyconnect,
							   nasyci, nasyco);
	for (i = 0; i < nasy_ports; i++) {
		if (0) /* TBD Probe for each port */
			nasy_state[i].state |= nasy_ALIVE;
	}
}

#ifdef lint
/*
 *	Reference each routine that the kernel does, to keep lint happy.
*/
main()
{
	nasyinit();
	nasyco((char)nasyci());
	nasyopen(0, 0);
	nasyread(0);
	nasyintr(0);
	nasywrite(0);
	nasyioctl(0, 0, (caddr_t)0, 0);
	nasyclose(0, 0);
	return 0;
}
#endif /*lint*/
