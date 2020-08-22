/* $Header: /home/Vince/cvs/games.d/warp.d/intrp.h,v 1.1 1987-07-26 10:17:03 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 7.0.1.1  86/12/12  16:59:45  lwall
 * Baseline for net release.
 * 
 * Revision 7.0  86/10/08  15:12:27  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

EXT char *origdir INIT(Nullch);		/* cwd when warp invoked */
EXT char *homedir INIT(Nullch);		/* login directory */
EXT char *dotdir INIT(Nullch);		/* where . files go */
EXT char *logname INIT(Nullch);		/* login id */
EXT char *hostname INIT(Nullch);	/* host name */
EXT char *realname INIT(Nullch);	/* real name from /etc/passwd */

void    intrp_init();
char	*filexp();
char	*dointerp();
void	interp();
char	*getrealname();
