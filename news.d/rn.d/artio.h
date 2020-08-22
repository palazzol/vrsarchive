/* $Header: /home/Vince/cvs/news.d/rn.d/artio.h,v 1.1 1990-02-26 22:31:43 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:23  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:35:43  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT ART_POS artpos INIT(0);	/* byte position in article file */

EXT ART_LINE artline INIT(0);		/* current line number in article file */
EXT FILE *artfp INIT(Nullfp);		/* current article file pointer */
EXT ART_NUM openart INIT(0);		/* what is the currently open article number? */
#ifdef LINKART
    EXT char *linkartname INIT(nullstr);/* real name of article for Eunice */
#endif

void	artio_init();
FILE	*artopen();			/* open an article unless already opened */
