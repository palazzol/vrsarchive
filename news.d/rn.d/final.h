/* $Header: /home/Vince/cvs/news.d/rn.d/final.h,v 1.1 1990-02-26 22:32:00 vrs Exp $
 * 
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:36  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:38:17  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

/* cleanup status for fast exits */

EXT bool panic INIT(FALSE);		/* we got hung up or something-- */
					/*  so leave tty alone */
EXT bool rc_changed INIT(FALSE);	/* need we rewrite .newsrc? */
EXT bool doing_ng INIT(FALSE);		/* do we need to reconstitute */
					/* current rc line? */

EXT char int_count INIT(0);		/* how many interrupts we've had */

/* signal catching routines */

int	int_catcher();
int	sig_catcher();
#ifdef SIGTSTP
    int	stop_catcher();
    int	cont_catcher();
#endif

void	final_init();
void	finalize();
