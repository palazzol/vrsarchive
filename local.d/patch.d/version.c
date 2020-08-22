/* $Header: /home/Vince/cvs/local.d/patch.d/version.c,v 1.1 1986-12-23 13:53:48 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.0  86/09/17  15:40:11  lwall
 * Baseline for netwide release.
 * 
 */

#include "EXTERN.h"
#include "common.h"
#include "util.h"
#include "INTERN.h"
#include "patchlevel.h"
#include "version.h"

/* Print out the version number and die. */

void
version()
{
    extern char rcsid[];

#ifdef lint
    rcsid[0] = rcsid[0];
#else
    fatal3("%s\nPatch level: %d\n", rcsid, PATCHLEVEL);
#endif
}
