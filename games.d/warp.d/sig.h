/* $Header: /home/Vince/cvs/games.d/warp.d/sig.h,v 1.1 1987-07-26 10:18:46 vrs Exp $ */

/* $Log: not supported by cvs2svn $
 * Revision 7.0  86/10/08  15:13:32  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

void sig_catcher();
#ifdef SIGTSTP
void cont_catcher();
void stop_catcher();
#endif
void mytstp();
void sig_init();
void finalize();
