/* $Header: /home/Vince/cvs/news.d/rn.d/respond.h,v 1.1 1990-02-26 22:32:39 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:19:12  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:47:50  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

EXT char *savedest INIT(Nullch);	/* value of %b */
EXT ART_POS savefrom INIT(0);		/* value of %B */
EXT char *headname INIT(Nullch);

#define SAVE_ABORT 0
#define SAVE_DONE 1

void	respond_init();
int	save_article();
int	cancel_article();
void	reply();
void	followup();
void	invoke();
