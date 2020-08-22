/* $Header: /home/Vince/cvs/news.d/rn.d/kfile.h,v 1.1 1990-02-26 22:32:13 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:47  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:42:00  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#define KF_GLOBAL 0
#define KF_LOCAL 1

#ifdef KILLFILES
EXT FILE *globkfp INIT(Nullfp);		/* global article killer file */
EXT FILE *localkfp INIT(Nullfp);	/* local (for this newsgroup) */
					/*  article killer file */
#endif

void	kfile_init();
int	do_kfile();
void	kill_unwanted();
int	edit_kfile();
void	open_kfile();
void    kf_append();
void	setthru();

