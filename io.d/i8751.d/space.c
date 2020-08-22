/*
 *	8751 port configuration
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
#include "sys/i8751.h"

ushort i8751_base[] = {
	0x0D,		/* 8751 serial port interconnect record type */
};
#define i8751_PORTS (sizeof(i8751_base)/sizeof(i8751_base[0]))

int i8751_ports = i8751_PORTS;
struct i8751state i8751_state[i8751_PORTS];
