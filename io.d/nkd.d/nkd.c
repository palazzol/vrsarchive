/*
 *	Bottom half (hardware manipulation) for new EGA driver
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
#include "sys/immu.h"
#include "sys/termio.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/at_ansi.h"
#include "sys/nkd.h"

extern struct tty *serialregister();
caddr_t nkd_bitmap;

#ifdef later
extern struct kdvtinfo	nkd_devinfo[];	/* device specific state */
#endif
struct tty *nkd_tty;

nkdparse(bufp, cnt)
caddr_t bufp;
int cnt;
{	static short row, col;
#define XLOW	0
#define XHIGH	1
#define YLOW	2
#define YHIGH	3
#define COLOR	4
	static int state = XLOW;
	int bit;

	while (cnt--) {
		switch (state) {
		case XLOW:
			col = *bufp++;
			state = XHIGH;
			break;
		case XHIGH:
			col |= (*bufp++) << 8;
			state = YLOW;
			break;
		case YLOW:
			row = *bufp++;
			state = YHIGH;
			break;
		case YHIGH:
			row |= (*bufp++) << 8;
			state = COLOR;
			break;
		case COLOR:
			/*
			 *	Now set the pixel, since we know row, col, and color.
			*/
			state = XLOW;				/* For next time */
			bit = 8 + (col&7);
			col  = row*80 + (col>>3);	/* Compute pixel byte offset */
			if (col > EGA_SIZE)
				break;					/* Error, continue loop */
			outw(0x3CE, (1<<bit)|8);	/* Set bit mask register */
			tenmicrosec();
			outw(0x3CE, (2<<8)|5);		/* Set write mode 2	*/
			tenmicrosec();
			outw(0x3CE, (0<<8)|3);		/* Set function select register	*/
			nkd_bitmap[col] = nkd_bitmap[col], *bufp++;
		}
	}
}

nkdproc(tp, cmd)
register struct tty *tp;
int cmd;
{	struct ccblock *tbuf = &tp->t_tbuf;
	int port = tp - nkd_tty;

	switch (cmd) {
	case T_OUTPUT:	/* Start output if possible */
		if (tp->t_state&(BUSY|TIMEOUT|TTSTOP)) 
			break;		/* Output being delayed or in progress */
		if (tbuf->c_ptr == NULL || tbuf->c_count == 0)
			if (!(CPRES & (*linesw[tp->t_line].l_output)(tp)))
				break;	/* Nothing queued yet */
		tp->t_state |= BUSY;	/* Device is active */
#ifdef later
		if (kd_devinfo[tp-nkd_tty].dmode == KD_TEXT)
			ansiparse(&kd_devinfo[tp-nkd_tty].as, tbuf->c_ptr, tbuf->c_count);
		else
#endif
			nkdparse(tbuf->c_ptr, tbuf->c_count);
		tbuf->c_count = 0;
		tp->t_state &= ~BUSY;	/* Device is finished */
		break;
	case T_TIME:				/* Timeout complete */
		tp->t_state &= ~TIMEOUT;
		nkdproc(tp, T_OUTPUT);
		break;
	case T_SUSPEND:
		tp->t_state |= TTSTOP;	/* Stop after next character */
		break;
	case T_RESUME:
		tp->t_state &= ~TTSTOP;	/* Resume after T_SUSPEND */
		nkdproc(tp, T_OUTPUT);
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
			tbuf->c_count = 0;
		}
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
		cmn_err(CE_NOTE, "kd unit %d: unknown cmd 0x%x\n", tp-nkd_tty, cmd);
	}
}

/*
 *	Process an interrupt from one of the keyboards.  Note that the output
 *	is done synchronously, and we never see output interrupts.
*/
/*ARGSUSED*/
nkdintr(lvl)
{	register struct tty *tp;
#ifdef later
	int i, status;

	for (i = 0, tp = nkd_tty; i < nkd_ports; i++, tp++) {
		if (tp->t_state & BUSY) {		/* We care about this one */
			status = inb(nkd_stat(i));
			if (NLP_NOPAPER(status)) {
				cmn_err(CE_NOTE, "kd unit %d is out of paper\n");
				continue;
			}
			if (NLP_OFFLINE(status)) {
				cmn_err(CE_NOTE, "kd unit %d is offline\n");
				continue;
			}
			if (NLP_ERROR(status))
				cmn_err(CE_WARN, "output error on kd unit %d\n");
			if (NLP_IDLE(status)) {
				tp->t_state &= ~BUSY;	/* Output to controller complete */
				nkdproc(tp, T_OUTPUT);	/* Start some more output */
			} else
				cmn_err(CE_WARN, "Interrupt with no work on kd unit %d\n");
		}
	}
#endif
}

/*
 *	Request a connection to a remote device.  CARR_ON indicates that such
 *	a connection exists.
*/
void
nkdconnect(tp)
register struct tty *tp;
{	int port = tp - nkd_tty;

	tp->t_state |= CARR_ON;		/* Fake carrier detect */
}

/*
 *	Register the device with the top end
*/
nkdinit()
{
	nkd_tty = serialregister(1, nkdproc, nkdconnect);
	nkd_bitmap = phystokv((caddr_t)COLOR_BASE);
}
