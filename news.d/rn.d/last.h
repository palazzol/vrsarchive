/* $Header: /home/Vince/cvs/news.d/rn.d/last.h,v 1.1 1990-02-26 22:32:16 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:50  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:42:22  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT char *lastngname INIT(Nullch);	/* last newsgroup read, from .rnlast file */
EXT long lasttime INIT(0);	/* time last rn was started up */
EXT long lastactsiz INIT(0);	/* size of active file when rn last started up */

void	last_init();
void    writelast();
