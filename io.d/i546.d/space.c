/*	Copyright (c) 1984, 1987 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license agreement
 *	or nondisclosure agreement with Intel Corporation and may not be
 *	copied nor disclosed except in accordance with the terms of that
 *	agreement.
 *
 *	Copyright 1983, 1987 Intel Corporation
*/
#ident	"@(#)c546.c	1.9"

/*
 * " $Header: /home/Vince/cvs/io.d/i546.d/space.c,v 1.1 1990-02-26 21:59:50 vrs Exp $";
 * c546.c
 *	iSBC 546 Specific Configuration file.
 *
 *	I001	vrs	04 Dec 86
 *		Fix up baud rate support.  Only problem left is that 134
 *		baud is not exactly 134.5 baud.
 *	I002	pats	12 Nov 87
 *		Add i546_time to track time from last interrupt (per board).
 */

#include "sys/types.h"
#include "sys/param.h"
#include "sys/termio.h"
#include "sys/tty.h"
#include "sys/i546.h"	/* 546/48 drive structures and constants */
	
/*
 * To configure the maximum number of 546 boards in the system,
 * simply change NUM546. There is a maximum of 4 and a minimum of 1
 */
#define NUM546	2
int	N546 = NUM546;

/*
 * The physical starting addresses of the i546 boards'
 * memory mapped I/O space.
 */
struct	i546cfg	i546cfg[] = {
	/* board # 1 */
		0xF90000,	/* base address */
		0x08A6,		/* wakeup port */
		3,		/* interrupt level */
	/* board # 2 */
		0xFA0000,
		0x08A7,
		2,
	/* board # 3 */
		0xFB0000,	/* base address */
		0x08A5,		/* wakeup port */
		4,		/* interrupt level */
	/* board # 4 */
		0xF80000,	/* base address */
		0x08A4,		/* wakeup port */
		64,		/* interrupt level */
	/* board # 5 */
		0xFC0000,	/* base address */
		0x08A0,		/* wakeup port */
		3,		/* interrupt level */
	/* board # 6 */
		0xFD0000,	/* base address */
		0x08A1,		/* wakeup port */
		3,		/* interrupt level */
	/* board # 7 */
		0xFE0000,	/* base address */
		0x08A2,		/* wakeup port */
		3,		/* interrupt level */
	/* board # 8 */
		0xFF0000,	/* base address */
		0x08A3,		/* wakeup port */
		3,		/* interrupt level */
};

/*
 * baud rate translation table
 */
int	i546baud[CBAUD+1] = {
	    US_B0,    US_B50,    US_B75,   US_B110,
	  US_B134,   US_B150,   US_B200,   US_B300,
	  US_B600,  US_B1200,  US_B1800,  US_B2400,
	 US_B4800,  US_B9600, US_B19200, US_B38400
};

/*
 * Standard tty structure.
 * Device driver's private data. Space for i546LINES structures
 * is allocated for each board configured in the driver.
 */
struct	tty	  i546_tty[NUM546*i546LINES];

/*
 * Line and board state data for the iSBC 546/48.
 * Space allocated for each board configured in the driver.
 */
struct	i546board i546board[NUM546];
time_t	i546_time[NUM546];	/* I002 */
