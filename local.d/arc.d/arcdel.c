/*
 *	arcdel.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCDEL
 * 
 * Version 2.09, created on 02/03/86 at 22:53:27
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to delete entries
 *          in an archive.
 */

#include "arc.h"

INT delarc(argc,argv)                  /* remove files from archive */
INT argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
{
    struct heads hdr;                  /* header data */
    INT del;                           /* true to delete a file */
    INT did[MAXARG];                   /* true when argument used */
    INT n;                             /* index */

    if (!argc)                         /* she must specify which */
        abort("You must tell me which files to delete!");

    for (n=0; n<argc; n++)             /* for each argument */
        did[n] = 0;                    /* reset usage flag */
    rempath(argc,argv);                /* strip off paths */

    openarc(1);                        /* open archive for changes */

    while (readhdr(&hdr,arc))          /* while more entries in archive */
    {
        del = 0;                       /* reset delete flag */
        for (n=0; n<argc; n++)         /* for each template given */
        {
            if (match(hdr.name,argv[n]))
            {
                del = 1;               /* turn on delete flag */
                did[n] = 1;            /* turn on usage flag */
                break;                 /* stop looking */
            }
        }

        if (del)                       /* skip over unwanted files */
        {
            fseek(arc,hdr.size,1);
            if (note)
                printf("Deleting file: %s\n",hdr.name);
        }
        else                           /* else copy over file data */
        {
            writehdr(&hdr,new);        /* write out header and file */
            filecopy(arc,new,hdr.size);
        }
    }

    hdrver = 0;                        /* special end of archive type */
    writehdr(&hdr,new);                /* write out archive end marker */
    closearc(1);                       /* close archive after changes */

    if (note)
    {
        for (n=0; n<argc; n++)         /* report unused arguments */
        {
            if (!did[n])
            {
                printf("File not found: %s\n",argv[n]);
                nerrs++;
            }
        }
    }
}
