/* $Header: /home/Vince/cvs/news.d/rn.d/rcln.h,v 1.1 1990-02-26 22:32:34 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:19:06  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:45:52  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#ifdef DEBUGGING
EXT ART_NUM ngmax[MAXRCLINE];
#endif

void    rcln_init();
#ifdef CATCHUP
    void	catch_up();
#endif
int	addartnum();
#ifdef MCHASE
    void	subartnum();
#endif
void	prange();
void	set_toread();
void	checkexpired();
