/* $Header: /home/Vince/cvs/news.d/rn.d/help.h,v 1.1 1990-02-26 22:32:05 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:40  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:39:19  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

void	help_init();
int	help_ng();
int	help_art();
int	help_page();
#ifdef ESCSUBS
    int	help_subs();
#endif
