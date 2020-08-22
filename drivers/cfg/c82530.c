/*
 *	%Z% %M% %I% %D% %Q%
 *
 *	This Module contains Proprietary Information of Microsoft
 *	Corporation and AT&T, and should be treated as Confidential.
 */



#include "../h/i82530.h"

/*
 * 82530 specific configuration table:
 *	This file declares the port and interrupt level table
 *	which the 82530 device driver uses.
 *	Channel B follows channel A.
 * written by Phil Barrett	12/17/82
 *
 */
struct i530cfg i530cfg[] =
/*	82530	82530	interrupt */
/*	ctrl	data	number	level	  */
{
	0xDC,	0xDE,	0x06,	/* Ch A */
	0xD8,	0xDA,	0x06};	/* Ch B */
