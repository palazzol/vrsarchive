/*
 *	Copyright 1988 Vincent R. Slyngstad
*/
#ifndef lint
static char my_copyright[] = "Copyright 1988 Vincent R. Slyngstad\n";
#endif
/*
 *	Generic serial device top half
 *
 *	Every device driver consists of a top half and a bottom half.
 *	The top half works in the context of the user's process to queue
 *	work for the hardware.  The bottom half works at interrupt time
 *	directly with the hardware to dequeue and execute this work.
 *
 *	This driver is an attempt to build a single, correct copy of
 *	the top half that everyone can share.
 *
 *	Each driver codes its own initialization, proc, and connect
 *	routines.  During device driver initialization a call to
 *	serialregister should be done to obtain some tty structures.  
*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/conf.h"
#include "sys/termio.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/serial.h"

extern struct conssw conssw;	/* Contains console device number */

extern struct tty serial_tty[];
extern struct serialdevice serial_device[];
extern int serial_ports;		/* Number of tty structures configured */
int serial_next = 0;			/* None allocated yet */

/*
 *	This routine registers a bottom half with the top half
 *	The top half registers the device and allocates the
 *	tty structures.
*/
struct tty *
serialregister(ports, proc, connect, ci, co)
int ports;
int (*proc)();		/* Does anybody actually use the return value?	*/
void (*connect)();
void (*co)();
int (*ci)();
{	register struct serialdevice *dp;
	struct tty *tp;

	if (serial_next+ports > serial_ports)
		return((struct tty *)0);
	tp = serial_tty + serial_next;
	dp = serial_device + serial_next;
	serial_next += ports;
	while (ports-- > 0) {
		dp->proc = proc;
		dp->connect = connect;
		dp->co = co;
		dp->ci = ci;
		dp++;
	}
	return(tp);
}

/*
 *	Open the device
 *		if !open
 *			Init data structures
 *			Request a connection
 *			Configure connection
 *		Wait for connection
*/
serialopen(dev, flag)
dev_t dev;
int flag;
{	register struct tty *tp = serial_tty + minor(dev);
	int oldspl = spltty();

	if (!(tp->t_state&ISOPEN)) {
		if (serial_device[minor(dev)].connect == 0) {
			u.u_error = ENXIO;
			return;
		}
		tp->t_proc = serial_device[minor(dev)].proc;/* Line Disc clear this?*/
		ttinit(tp);
		tp->t_cflag = (tp->t_cflag&~CBAUD) | B9600;
		if (dev == conssw.co_dev) {
			tp->t_iflag = BRKINT|IGNPAR|ICRNL|ISTRIP|IXON|IXANY;
			tp->t_oflag = OPOST|ONLCR|TAB3;
			tp->t_lflag = ECHO|ECHOE|ECHOK|ICANON|ISIG;
		}
		/*
		 *	Someday this should be (*tp->t_proc)(tp, T_CONNECT);
		 *	The only reason I can figure why it is not already
		 *	is that someone was thinking of RS-232 devices which
		 *	can assert DTR in the param routine.
		*/
		(*serial_device[minor(dev)].connect)(tp);
		(void) (*tp->t_proc)(tp, T_PARM);
	}
	if (!(flag&FNDELAY)) {
		while (!(tp->t_state&CARR_ON)) {
			sleep(&tp->t_rawq, TTIPRI);
		}
	}
	(*linesw[tp->t_line].l_open)(tp);
	splx(oldspl);
}

serialclose(dev)
dev_t dev;
{	register struct tty *tp = serial_tty + minor(dev);
	int oldspl = spltty();

	(*linesw[tp->t_line].l_close)(tp);
	if (tp->t_cflag&HUPCL)
		(void) (*tp->t_proc)(tp, T_DISCONNECT);
	splx(oldspl);
}

/*
 *	Call the top level of the line discipline for read/write
*/
serialread(dev)
dev_t dev;
{	register struct tty *tp = serial_tty + minor(dev);
	(*linesw[tp->t_line].l_read)(tp);
}

serialwrite(dev)
dev_t dev;
{	register struct tty *tp = serial_tty + minor(dev);
	(*linesw[tp->t_line].l_write)(tp);
}

serialioctl(dev, cmd, arg, mode)
dev_t dev;
int cmd;
caddr_t arg;
int mode;
{	register struct tty *tp = serial_tty + minor(dev);
	int oldspl = spltty();

	if (ttiocom(tp, cmd, arg, mode))
		(void) (*tp->t_proc)(tp, T_PARM);
	splx(oldspl);
}

serialco(ch)
char ch;
{
		(*serial_device[0].co)(ch);
}

int
serialci()
{
		return((*serial_device[0].ci)());
}
