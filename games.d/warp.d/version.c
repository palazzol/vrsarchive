/* $Header: /home/Vince/cvs/games.d/warp.d/version.c,v 1.1 1987-07-26 10:20:12 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 7.0  86/10/08  15:14:39  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 */

#include "patchlevel.h"
#include "INTERN.h"
#include "version.h"

/* Print out the version number. */

void
version()
{
    extern char rcsid[];

    printf("%s\r\nPatch level: %d\r\n", rcsid, PATCHLEVEL);
}
