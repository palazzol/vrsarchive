/*
 *	arccvt.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCCVT
 * 
 * Version 1.16, created on 02/03/86 at 22:53:02
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to convert archives to use
 *          newer file storage methods.
 */

#include "arc.h"

static char tempname[STRLEN];          /* temp file name */

INT cvtarc(argc,argv)                  /* convert archive */
INT argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
{
    struct heads hdr;                  /* file header */
    INT cvt;                           /* true to convert current file */
    INT did[MAXARG];                   /* true when argument was used */
    INT n;                             /* index */
    INT cvtfile();

    sprintf(tempname,"%s.cvt",arctemp);

    openarc(1);                        /* open archive for changes */

    for (n=0; n<argc; n++)             /* for each argument */
         did[n] = 0;                   /* reset usage flag */
    rempath(argc,argv);                /* strip off paths */

    if (argc)                          /* if files were named */
    {
        while (readhdr(&hdr,arc))      /* while more files to check */
        {
            cvt = 0;                   /* reset convert flag */
            for (n=0; n<argc; n++)     /* for each template given */
            {
                if (match(hdr.name,argv[n]))
                {
                    cvt = 1;           /* turn on convert flag */
                    did[n] = 1;        /* turn on usage flag */
                    break;             /* stop looking */
                }
            }

            if (cvt)                   /* if converting this one */
                cvtfile(&hdr);         /* then do it */
            else                       /* else just copy it */
            {
                writehdr(&hdr,new);
                filecopy(arc,new,hdr.size);
            }
        }
    }
    else
        while (readhdr(&hdr,arc))      /* else convert all files */
            cvtfile(&hdr);

    hdrver = 0;                        /* archive EOF type */
    writehdr(&hdr,new);                /* write out our end marker */
    closearc(1);                       /* close archive after changes */

    if (note)
    {
        for (n=0; n<argc; n++)         /* report unused args */
        {
            if (!did[n])
            {
                printf("File not found: %s\n",argv[n]);
                nerrs++;
            }
        }
    }
}

static INT cvtfile(hdr)                /* convert a file */
struct heads *hdr;                     /* pointer to header data */
{
    long starts, ftell();              /* where the file goes */
    FILE *tmp, *fopen();               /* temporary file */

    if (!(tmp=fopen(tempname,"w+")))
        abort("Unable to create temporary file %s",tempname);

    if (note)
    {
        printf("Converting file: %-14s   reading, ",hdr->name);
        fflush(stdout);
    }

    unpack(arc,tmp,hdr);               /* unpack the entry */
    fseek(tmp,0L,0);                   /* reset temp for reading */
    starts = ftell(new);               /* note where header goes */
    hdrver = ARCVER;                   /* anything but end marker */
    writehdr(hdr,new);                 /* write out header skeleton */
    pack(tmp,new,hdr);                 /* pack file into archive */
    fseek(new,starts,0);               /* move back to header skeleton */
    writehdr(hdr,new);                 /* write out real header */
    fseek(new,hdr->size,1);            /* skip over data to next header */
    fclose(tmp);                       /* all done with the file */
    if (unlink(tempname) && warn)
    {
        printf("Cannot unsave %s\n",tempname);
        nerrs++;
    }
}
