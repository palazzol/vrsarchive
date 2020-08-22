/* $Header: /home/Vince/cvs/news.d/rn.d/ngsrch.h,v 1.1 1990-02-26 22:32:26 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:59  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:44:56  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#ifdef NGSEARCH
#define NGS_ABORT 0
#define NGS_FOUND 1
#define NGS_INTR 2
#define NGS_NOTFOUND 3

EXT bool ng_doread INIT(FALSE);		/* search read newsgroups? */
#endif

void	ngsrch_init();
#ifdef NGSEARCH 
    int		ng_search();
    bool	ng_wanted();
#endif
#ifdef NGSORONLY
    char	*ng_comp();
#endif
