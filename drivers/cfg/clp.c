/*
 * clp.c
 *	Line-printer specific configuration.
 *
 * This split out from c.c to avoid name-clashing with other device-
 * specific configuration files.
*/
#include "../h/types.h"
#include "../h/tty.h"
#include "../h/lp.h"

int lp_slo = 500;	/* timeout for slow printers = 10*HZ */

/*
 * This table gives the interrupt level and  I/O addresses
 * for each possible line-printer.  The driver procedure entry-
 * points are configured in c.c
 */

struct	lpcfg lpcfg[NLP] = {
/*   Level, port A, port B, port C, Control */
	71,  0xc8,    0xca,   0xcc,    0xce
};

/*
 *	This table holds flags which allow a line discipline to be
 *	attached (or not) to the parallel port.
*/
struct tty lptty[NLP];
