/*
 *	Generic serial device top half configuration file
*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/dir.h"
#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/systm.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/serial.h"

#define SERIAL_PORTS	8	/* Maximum number of serial ports to support	*/
int serial_ports = SERIAL_PORTS;

/*
 *	Serial device cdevsw entries get patched up with pointers into this
 *	array when serial hardware registers itself.
*/
struct tty serial_tty[SERIAL_PORTS];

/*
 *	Linkage to new bottom half routines for each tty structure
*/
struct serialdevice serial_device[SERIAL_PORTS];
