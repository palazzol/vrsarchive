/* $Header: /home/Vince/cvs/news.d/rn.d/sw.h,v 1.1 1990-02-26 22:32:47 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:19:20  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:51:07  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#ifdef INNERSEARCH
EXT int gline INIT(0);
#endif

void    sw_init();
void    sw_file();
void    sw_list();
void	decode_switch();
void	pr_switches();
void	cwd_check();
