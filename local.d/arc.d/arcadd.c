#define _POSIX_SOURCE
/*
 *	arcadd.c	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

/*
 * ARC - Archive utility - ARCADD
 * 
 * Version 3.39, created on 02/05/86 at 22:21:53
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to add files to an archive.
 */

#include "arc.h"
#include <sys/types.h>
#include <sys/stat.h>

INT addarc(argc,argv,move,update,fresh)/* add files to archive */
INT argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
INT move;                              /* true if moving file */
INT update;                            /* true if updating */
INT fresh;                             /* true if freshening */
{
    char *buf;                         /* pathname buffer */
    char *i, *strrchr();               /* string indexing junk */
    INT n;                             /* indices */
    struct stat fbuf;                  /* file information structure */
    struct heads hdr;                  /* file header data storage */
    INT addfile();

    openarc(1);                        /* open archive for changes */

    for (n=0; n<argc; ++n)
    {
        stat(argv[n],&fbuf);           /* find out what kind of file it is */

        /* if file is a regular file, try to add it */
        if (((fbuf.st_mode >> 12) == 8) || ((fbuf.st_mode >> 12) == 0))
        {
            if (i=strrchr(buf=argv[n], '/')) buf=i+1;
            addfile(argv[n],buf,update,fresh);
        }
        else
        {
            printf("Invalid file type: %s\n",argv[n]);
            ++nerrs;
        }
    }

    /* quit if no changes to make */
    if (nerrs == argc)
    {
        fclose(arc);
        fclose(new);
        abort("I have no work to do!");
    }

    /* now we must copy over all files that follow our additions */

    while (readhdr(&hdr,arc))          /* while more entries to copy */
    {
        writehdr(&hdr,new);
        filecopy(arc,new,hdr.size);
    }

    hdrver = 0;                        /* archive EOF type */
    writehdr(&hdr,new);                /* write out our end marker */
    closearc(1);                       /* close archive after changes */

    if (move) for (n=0; n<argc; ++n)   /* if this was a move */
    {
        stat(argv[n],&fbuf);           /* make sure file can be removed */
        if (((fbuf.st_mode >> 12) == 8) || ((fbuf.st_mode >> 12) == 0))
        {
            if (unlink(argv[n]) && warn)
            {
                printf("Cannot unsave %s\n",argv[n]);
                ++nerrs;
            }
        }
    }
}

static INT addfile(path,name,update,fresh) /* add named file to archive */
char *path;                            /* path name of file to add */
char *name;                            /* name of file to add */
INT update;                            /* true if updating */
INT fresh;                             /* true if freshening */
{
    struct heads nhdr;                 /* data regarding the new file */
    struct heads ohdr;                 /* data regarding an old file */
    FILE *f, *fopen();                 /* file to add, opener */
    long starts, ftell();              /* file locations */
    INT upd = 0;                       /* true if replacing an entry */
    register int i;

    if (!(f=fopen(path,"r")))          /* quit if we can't open the file */
    {
        if (warn)
            printf("Cannot read file: %s\n",path);
        nerrs++;
        fclose(f);
        return;
    }

    /* fill the header structure with information about the new file */

    /* zero out name array - necessary because of strncpy */
    for (i = 0; i < FNLEN1; i++) nhdr.name[i] = '\0';

    /* save filename, using 14 or 12 char template, based on ibmpc option */
    strncpy(nhdr.name,name,((ibmpc) ? FNLEN2 : FNLEN1)-1);
    nhdr.size = 0;                     /* clear out size storage */
    nhdr.crc = 0;                      /* clear out CRC check storage */
    getstamp(f,&nhdr.date,&nhdr.time);

    /* position archive to spot for new file */

    if (arc)                           /* if adding to existing archive */
    {
        starts = ftell(arc);           /* where are we? */
        while (readhdr(&ohdr,arc))     /* while more files to check */
        {
            if (strcmp(ohdr.name,nhdr.name) == 0)
            {
                upd = 1;               /* replace existing entry */
                if (update || fresh)   /* if updating or freshening */
                {
                    if (nhdr.date<ohdr.date ||
                       (nhdr.date == ohdr.date && nhdr.time <= ohdr.time))
                    {
                        fseek(arc,starts,0);
                        fclose(f);
                        return;        /* skip if not newer */
                    }
                }
            }

            if (strcmp(ohdr.name,nhdr.name) >= 0)
                break;                 /* found our spot */

            writehdr(&ohdr,new);       /* entry preceeds update; keep it */
            filecopy(arc,new,ohdr.size);
            starts = ftell(arc);       /* now where are we? */
        }

        if (upd)                       /* if an update */
        {
            if (note)
            {
                printf("Updating file: %-14s  ",name);
                fflush(stdout);
            }
            fseek(arc,ohdr.size,1);
        }
        else if (fresh)                /* else if freshening */
        {
            fseek(arc,starts,0);       /* then do not add files */
            fclose(f);
            return;
        }
        else                           /* else adding a new file */
        {
            if (note)
            {
                printf("Adding file:   %-14s  ",name);
                fflush(stdout);
            }
            fseek(arc,starts,0);       /* reset for next time */
        }
    }

    else                               /* no existing archive */
    {
        if (fresh)                     /* cannot freshen nothing */
        {
            fclose(f);
            return;
        }
        else if (note)                 /* else adding a file */
        {
            printf("Adding file:   %-14s  ",name);
            fflush(stdout);
        }
    }

    starts = ftell(new);               /* note where header goes */
    hdrver = ARCVER;                   /* anything but end marker */
    writehdr(&nhdr,new);               /* write out header skeleton */
    pack(f,new,&nhdr);                 /* pack file into archive */
    fseek(new,starts,0);               /* move back to header skeleton */
    writehdr(&nhdr,new);               /* write out real header */
    fseek(new,nhdr.size,1);            /* skip over data to next header */
    fclose(f);                         /* all done with the file */
}
