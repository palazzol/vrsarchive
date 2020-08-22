/* $Header: /home/Vince/cvs/news.d/rn.d/ngstuff.h,v 1.1 1990-02-26 22:32:29 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:19:01  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:45:12  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#define NN_NORM 0
#define NN_INP 1
#define NN_REREAD 2
#define NN_ASK 3

void	ngstuff_init();
int	escapade();
int	switcheroo();
int	numnum();
int	perform();
