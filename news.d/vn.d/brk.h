/*
** vn news reader.
**
** brk.h - codes for sig_set routine
**
** see copyright disclaimer / history in vn.c source file
*/

/*
	state flags for handling breaks / values for sig_set calls.
	BRK_IN, BRK_SESS, BRK_READ and BRK_OUT are the states.  All
	but BRK_INIT are used as calls to sig_set.  BRK_RFIN indicates
	a return from BRK_READ to BRK_SESS (no jump location passed),
*/
#define BRK_INIT 0		/* initial value, indicating uncaught signals */
#define BRK_IN 1		/* in NEWSRC / article scanning phase */
#define BRK_SESS 2		/* in page interactive session */
#define BRK_READ 3		/* reading articles */
#define BRK_RFIN 4		/* finished reading, return to old mode */
#define BRK_OUT 5		/* NEWSRC updating phase */

#define BRK_PR "really quit ? "
