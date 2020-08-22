/*
 * rextern - external definitions for readnews
 */

#ifndef lint
static char	*SccsId = "@(#)rextern.c	2.12	8/28/84";
#endif !lint

#include "rparams.h"

int	uid, gid;			/* real user/group I.D.		*/
int	duid, dgid;			/* effective user/group I.D.	*/
int	sigtrap;			/* set if signal trapped	*/
int	savmask;			/* old umask			*/
int	mode;				/* mode of news program		*/
struct hbuf header;			/* general-use header structure	*/
char	bfr[LBUFLEN];			/* general-use scratch area	*/

#ifndef ROOTID
int	ROOTID;				/* special users id #		*/
#endif

char	*outfile = "/tmp/M1XXXXXX";	/* output file for -M and -c	*/
char	*infile = "/tmp/M2XXXXXX";	/* -T output from Mail		*/
int	ngrp, line = -1;

char	filename[BUFLEN], coptbuf[BUFLEN], datebuf[BUFLEN];
char	titlebuf[BUFLEN];
char	afline[BUFLEN];
FILE	*rcfp, *actfp;
time_t	atime;
char	newsrc[BUFLEN], groupdir[BUFLEN], *rcline[LINES], rcbuf[LBUFLEN];
char	bitmap[BITMAPSIZE/8], *argvrc[LINES];
long	bit, obit, last;
int	readmode = NEXT;
int	actdirect = FORWARD;	/* read direction in ACTIVE file */
int	rcreadok = FALSE;	/* NEWSRC has been read OK */
int	zapng = FALSE;		/* ! out this newsgroup on next updaterc */
long	ngsize;			/* max article # in this newsgroup */
long	minartno;		/* min article # in this newsgroup */

#ifndef SHELL
char	*SHELL;
#endif

#ifndef MAILER
char	*MAILER;
#endif

char	PAGER[BUFLEN];
