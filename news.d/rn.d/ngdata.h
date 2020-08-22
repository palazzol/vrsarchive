/* $Header: /home/Vince/cvs/news.d/rn.d/ngdata.h,v 1.1 1990-02-26 22:32:24 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:57  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:44:48  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT FILE *actfp INIT(Nullfp);	/* the active file */
EXT bool writesoft INIT(FALSE);	/* rewrite the soft pointer file? */
EXT int softtries INIT(0), softmisses INIT(0);

#ifdef CACHEFIRST
    EXT ART_NUM abs1st[MAXRCLINE];	/* 1st real article in newsgroup */
#else
# ifdef MININACT
    EXT ART_NUM abs1st INIT(0);
# endif
#endif


void	ngdata_init();
ART_NUM	getngsize();
ACT_POS findact();
ART_NUM	getabsfirst();
ART_NUM	getngmin();
