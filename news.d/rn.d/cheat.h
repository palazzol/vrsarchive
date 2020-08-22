/* $Header: /home/Vince/cvs/news.d/rn.d/cheat.h,v 1.1 1990-02-26 22:31:54 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:31  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:36:58  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#ifdef ARTSEARCH
EXT ART_NUM srchahead INIT(0); 	/* are we in subject scan mode? */
				/* (if so, contains art # found or -1) */
#endif

#ifdef PENDING
#   ifdef CACHESUBJ
	EXT ART_NUM subj_to_get;
#   endif
#endif

void	cheat_init();
void	look_ahead();
void	collect_subjects();
