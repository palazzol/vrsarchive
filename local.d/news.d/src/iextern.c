/*
 * iextern - external definitions for inews.
 */

#ifndef lint
static char	*SccsId = "@(#)iextern.c	2.11	8/28/84";
#endif !lint

#include "iparams.h"

int	uid, gid;			/* real user/group I.D. */
int	duid, dgid;			/* effective user/group I.D. */
int	sigtrap;			/* set if signal trapped */
int	savmask;			/* old umask */
int	mode;				/* mode of news program */
struct hbuf header;			/* general-use header structure */
char	bfr[LBUFLEN];			/* general-use scratch area */
char	nbuf[LBUFLEN];			/* local newsgroup buffer */
char	filename[NAMELEN];		/* general-use file name */

#ifndef ROOTID
int	ROOTID;				/* special users id # */
#endif

char	*DFLTNG = "general";		/* default newsgroup */
FILE	*infp;				/* input file-pointer */
FILE	*actfp;				/* active newsgroups file pointer */
int	tty;				/* set if infp is a tty */
char	*PARTIAL = "dead.article";	/* place to save partial news */
char	*SHELL = "/bin/sh";		/* shell for inews to use	*/
int	defexp = FALSE;			/* set if def. expiration date used */
int	is_ctl;				/* true for a control message */
