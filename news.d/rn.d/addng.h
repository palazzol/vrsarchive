/* $Header: /home/Vince/cvs/news.d/rn.d/addng.h,v 1.1 1990-02-26 22:31:38 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:20  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:34:48  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

void	addng_init();
#ifdef FINDNEWNG
    bool	newlist();
    long	birthof();
    bool	scanactive();
#endif
