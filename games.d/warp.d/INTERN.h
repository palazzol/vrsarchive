/* $Header: /home/Vince/cvs/games.d/warp.d/INTERN.h,v 1.1 1987-07-26 10:16:11 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 7.0.1.1  86/12/12  16:51:45  lwall
 * Guarded the undefs.
 * 
 * Revision 7.0  86/10/08  15:11:37  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

#ifdef EXT
#undef EXT
#endif
#define EXT

#ifdef INIT
#undef INIT
#endif
#define INIT(x) = x

#define DOINIT
