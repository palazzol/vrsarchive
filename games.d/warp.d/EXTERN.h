/* $Header: /home/Vince/cvs/games.d/warp.d/EXTERN.h,v 1.1 1987-07-26 10:15:59 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 7.0.1.1  86/12/12  16:46:50  lwall
 * Guarded the undefs.
 * 
 * Revision 7.0  86/10/08  15:11:31  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

#ifdef EXT
#undef EXT
#endif
#define EXT extern

#ifdef INIT
#undef INIT
#endif
#define INIT(x)

#ifdef DOINIT
#undef DOINIT
#endif
