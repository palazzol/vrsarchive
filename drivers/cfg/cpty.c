/*
 *	%W%	%G%	*/
 * Pseudo-teletype Driver
 * Declarations for statically allocated structures
 *
*/
#include "../h/param.h"
#include "../h/systm.h"
#include "../h/ioctl.h"
#include "../h/tty.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/conf.h"
#include "../h/file.h"
#include "../h/proc.h"
#include "../h/uio.h"
#include "../h/kernel.h"

#define NPTY	32		/* Number of pseudo-terminals	*/

#if NPTY == 1
#undef	NPTY
#define	NPTY	32		/* crude XXX */
#endif
/*
 * pts == /dev/tty[pP]?
 * ptc == /dev/ptp[pP]?
*/
struct	tty pt_tty[NPTY];
struct	pt_ioctl {
	int	pt_flags;
	int	pt_gensym;
	struct	proc *pt_selr, *pt_selw;
	int	pt_send;
} pt_ioctl[NPTY];
int npty = NPTY;
