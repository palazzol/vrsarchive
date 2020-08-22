/* $Header: /home/Vince/cvs/news.d/rn.d/init.h,v 1.1 1990-02-26 22:32:08 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:42  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:40:46  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT char *lockname INIT(nullstr);

bool	initialize();
void	lock_check();
void	newsnews_check();
void	version_check();
