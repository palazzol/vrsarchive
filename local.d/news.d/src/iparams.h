/*
 * iparams - parameters for inews.
 */

/*	@(#)iparams.h	2.10	8/28/84	*/

#include "params.h"

/* external declarations specific to inews */
extern	char	nbuf[], *ARTICLE, *INFILE, *ALIASES, *PARTIAL;
#ifndef ROOTID
extern	int	ROOTID;
#endif

#ifdef NOTIFY
extern	char	*TELLME;
#endif NOTIFY

extern	FILE	*infp,*actfp;
extern	int	tty, flag, is_ctl;
extern	char	filename[], *DFLTNG, whatever[];
