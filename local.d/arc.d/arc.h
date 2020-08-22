/*
 *	arc.h	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

/*
 * ARC - Archive utility - ARC Header
 * 
 * Version 2.14, created on 02/03/86 at 22:48:29
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description: 
 *          This is the header file for the ARC archive utility.  It defines
 *          global parameters and the references to the external data.
 */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>

#define EXTERN
#define INT short

#define ARCMARK 26              /* special archive marker */
#define ARCVER 8                /* archive header version code */
#define STRLEN 100              /* system standard string length */
#define FNLEN1 15               /* file name length (UNIX) */
#define FNLEN2 13               /* file name length (DOS) */
#define MAXARG 25               /* maximum number of arguments */

/*
 * redefine string functions if BSD
 */
#ifdef BSD
#define strchr index            /* index function */
#define strrchr rindex          /* reverse index function */
#endif /* BSD */

/*
 * include the header structure
 * (dependent on previously defined constants)
 */

#include "arcs.h"

EXTERN INT keepbak;             /* true if saving the old archive */
EXTERN INT warn;                /* true to print warnings */
EXTERN INT note;                /* true to print comments */
EXTERN INT ibmpc;               /* true to make IBM compatibile */
EXTERN INT bose;                /* true to be verbose */
EXTERN INT nocomp;              /* true to suppress compression */
EXTERN char arctemp[STRLEN];    /* arc temp file prefix */
EXTERN char *password;          /* encryption password pointer */
EXTERN INT nerrs;               /* number of errors encountered */

EXTERN char hdrver;             /* header version */

EXTERN FILE *arc;               /* the old archive */
EXTERN FILE *new;               /* the new archive */

EXTERN char arcname[STRLEN];    /* storage for archive name */
EXTERN char bakname[STRLEN];    /* storage for backup copy name */
EXTERN char newname[STRLEN];    /* storage for new archive name */
EXTERN unsigned INT arcdate;    /* archive date stamp */
EXTERN unsigned INT arctime;    /* archive time stamp */
