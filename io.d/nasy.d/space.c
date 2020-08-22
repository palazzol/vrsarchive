/*
 *	Serial port configuration
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
#include "sys/nasy.h"

ushort nasy_base[] = {
	0x03F8,		/* COM1 */
	0x02F8,		/* COM2 */
	0x02E8,		/* COM3 */
	0x02E0,		/* COM4 */
};
#define NASY_PORTS (sizeof(nasy_base)/sizeof(nasy_base[0]))

int nasy_ports = NASY_PORTS;
struct nasystate nasy_state[NASY_PORTS];
