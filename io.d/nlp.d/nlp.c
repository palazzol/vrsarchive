/*
 *	Bottom half (hardware manipulation) for parallel ports
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
#include "sys/nlp.h"

extern void nulldev();

extern int nlp_ports;
extern ushort nlp_base[];
extern struct nlpstate nlp_state[];
extern struct tty *serialregister();

struct tty *nlp_tty;

nlp_500usec()
{	register int i = 50;

	while (i--)
		tenmicrosec();
}

nlpproc(tp, cmd)
register struct tty *tp;
int cmd;
{	struct ccblock *tbuf = &tp->t_tbuf;
	int port = tp - nlp_tty;

	switch (cmd) {
	case T_OUTPUT:	/* Start output if possible */
		if (tp->t_state&(BUSY|TIMEOUT|TTSTOP)) 
			break;		/* Output being delayed or in progress */
		if (tbuf->c_ptr == NULL || tbuf->c_count == 0)
			if (!(CPRES & (*linesw[tp->t_line].l_output)(tp)))
				break;	/* Nothing queued yet */
		tp->t_state |= BUSY;	/* Device is active */
		outb(nlp_data(port), *(tbuf->c_ptr++));
		outb(nlp_cmnd(port), SELECT|NO_RESET|STROBE);	/* Strobe, 500usec */
		nlp_500usec();			/* Delay 500 nanoseconds */
		outb(nlp_cmnd(port), SELECT|NO_RESET|INTERRUPT);/* Reset the strobe */
		if (--tbuf->c_count == 0)
			tbuf->c_ptr -= tbuf->c_size;
		break;
	case T_TIME:				/* Timeout complete */
		tp->t_state &= ~TIMEOUT;
		nlpproc(tp, T_OUTPUT);
		break;
	case T_SUSPEND:
		tp->t_state |= TTSTOP;	/* Stop after next character */
		break;
	case T_RESUME:
		tp->t_state &= ~TTSTOP;	/* Resume after T_SUSPEND */
		nlpproc(tp, T_OUTPUT);
		break;
	case T_BLOCK:				/* Block input */
		break;
	case T_UNBLOCK:				/* Unblock input */
		break;
	case T_RFLUSH:				/* Flush input */
		break;
	case T_WFLUSH:				/* Flush output */
		/*
		 *	Renormalize c_ptr so the buffer can be re-used.
		*/
		if (tbuf->c_ptr && tbuf->c_count) {
			tbuf->c_ptr -= tbuf->c_size-tbuf->c_count;
		}
		tbuf->c_count = 0;
		break;
	case T_BREAK:				/* Send a break */
		break;
	case T_INPUT:				/* Start input */
		break;
	case T_DISCONNECT:			/* Close connection */
		break;
	case T_PARM:				/* Set line parameters */
		break;
	case T_SWTCH:				/* What does this do? */
		break;
	default:					/* "Cannot happen" */
		cmn_err(CE_NOTE, "lp unit %d: unknown cmd 0x%x\n", tp-nlp_tty, cmd);
	}
}

/*
 *	Process an interrupt from one of the printer ports.
 *
 *	For output side status reporting, there are three states:
 *	printer idle, printer ready, and printer busy.  Note that
 *	tp->t_state&BUSY must be true whenever the printer is not
 *	idle.  Ideally, we would dequeue whenever the printer was
 *	ready, and note printer idle via the BUSY flag.
*/
/*ARGSUSED*/
nlpintr(lvl)
{	register struct tty *tp;
	int i, status;

	for (i = 0, tp = nlp_tty; i < nlp_ports; i++, tp++) {
		if (tp->t_state & BUSY) {		/* We care about this one */
			status = inb(nlp_stat(i));
			if (NLP_NOPAPER(status)) {
				cmn_err(CE_NOTE, "lp unit %d is out of paper\n");
				continue;
			}
			if (NLP_OFFLINE(status)) {
				cmn_err(CE_NOTE, "lp unit %d is offline\n");
				continue;
			}
			if (NLP_ERROR(status))
				cmn_err(CE_WARN, "output error on lp unit %d\n");
			if (NLP_IDLE(status)) {
				tp->t_state &= ~BUSY;	/* Output to controller complete */
				nlpproc(tp, T_OUTPUT);	/* Start some more output */
			} else
				cmn_err(CE_WARN, "Interrupt with no work on lp unit %d\n");
		}
	}
}

/*
 *	Request a connection to a remote device.  CARR_ON indicates that such
 *	a connection exists.
*/
void
nlpconnect(tp)
register struct tty *tp;
{	int port = tp - nlp_tty;
	/*
	 *	Note: a common mistake is to reset the printer hardware
	 *	(as opposed to the controller) here.  This has the effect
	 *	on many printers of flushing output from some previous
	 *	job, due to inadequate low control between the controller
	 *	and the printer.
	*/
	outb(nlp_cmnd(port), SELECT|NO_RESET|INTERRUPT);
	tenmicrosec();
	tp->t_state |= CARR_ON;		/* Fake carrier detect */
}

nlpinit()
{	int i;

	nlp_tty = serialregister(nlp_ports, nlpproc, nlpconnect, nulldev, nulldev);
	for (i = 0; i < nlp_ports; i++) {
		/*
		 *	Probe for each port, remember if it was alive
		*/
		outb(nlp_data(i), 0x55);		/* Write a R/W register	*/
		tenmicrosec();
		if (inb(nlp_data(i)) == 0x55)	/* Did it work?	*/
			nlp_state[i].state |= ALIVE;/* Yes */
		outb(nlp_cmnd(i), SELECT);		/* Reset hardware */
		nlp_500usec();					/* Wait 500usec */
		outb(nlp_cmnd(i), SELECT|NO_RESET|INTERRUPT);/* Clear reset */
	}
}
