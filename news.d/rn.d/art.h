/* $Header: /home/Vince/cvs/news.d/rn.d/art.h,v 1.1 1990-02-26 22:31:41 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:22  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:35:29  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

/* do_article() return values */

#define DA_NORM 0
#define DA_RAISE 1
#define DA_CLEAN 2
#define DA_TOEND 3

void	art_init();
int	do_article();
int	page_switch();
bool	innermore();
