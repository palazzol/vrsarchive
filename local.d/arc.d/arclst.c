/*
 *	arclst.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCLST
 * 
 * Version 2.34, created on 02/03/86 at 22:56:57
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to list the contents
 *          of an archive.
 */
 
#include "arc.h"

static INT lstfile();

INT lstarc(argc,argv)                  /* list files in archive */
INT argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
{
    struct heads hdr;                  /* header data */
    INT list;                          /* true to list a file */
    INT did[MAXARG];                   /* true when argument was used */
    long tnum, tlen, tsize;            /* totals */
    INT n;                             /* index */

    tnum = tlen = tsize = 0;           /* reset totals */

    for(n=0; n<argc; n++)              /* for each argument */
        did[n] = 0;                    /* reset usage flag */
    rempath(argc,argv);                /* strip off paths */

    printf("Name            Length  ");
    if(bose)
        printf("  Stowage    SF   Size now");
    printf("  Date     ");
    if(bose)
        printf("  Time    CRC");
    printf("\n");

    printf("==============  ========");
    if(bose)
        printf("  ========  ====  ========");
    printf("  =========");
    if(bose)
         printf("  ======  ====");
    printf("\n");

    openarc(0);                        /* open archive for reading */

    if(argc)                           /* if files were named */
    {
        while(readhdr(&hdr,arc))       /* process all archive files */
        {
            list = 0;                  /* reset list flag */
            for(n=0; n<argc; n++)      /* for each template given */
            {
                if(match(hdr.name,argv[n]))
                {
                    list = 1;          /* turn on list flag */
                    did[n] = 1;        /* turn on usage flag */
                    break;             /* stop looking */
                }
            }

            if(list)                   /* if this file is wanted */
            {
                lstfile(&hdr);         /* then tell about it */
                tnum++;                /* update totals */
                tlen += hdr.length;
                tsize += hdr.size;
            }

            fseek(arc,hdr.size,1);     /* move to next header */
        }
    }

    else while(readhdr(&hdr,arc))      /* else report on all files */
    {
        lstfile(&hdr);
        tnum++;                        /* update totals */
        tlen += hdr.length;
        tsize += hdr.size;
        fseek(arc,hdr.size,1);         /* skip to next header */
    }

    closearc(0);                       /* close archive after reading */

    printf("        ======  ========");
    if (bose)
        printf("            ====  ========");
    printf("\n");
    printf("Total   %6ld  %8ld  ",tnum,tlen);
    if (bose)
    {
        if (tlen > 0)
            printf("          %3ld%%",100L - (100L*tsize)/tlen);
        else
            printf("            0%%");
        printf("  %8ld  ",tsize);
    }
    printf("\n");

    if(note)
    {
        for(n=0; n<argc; n++)          /* report unused args */
        {
            if(!did[n])
            {
                printf("File not found: %s\n",argv[n]);
                nerrs++;
            }
        }
    }
}

static INT lstfile(hdr)                /* tell about a file */
struct heads *hdr;                     /* pointer to header data */
{
    INT yr, mo, dy;                    /* parts of a date */
    INT hh, mm;                        /* parts of a time */

    static char *mon[] =               /* month abbreviations */
    {    "Jan",    "Feb",    "Mar",    "Apr",
         "May",    "Jun",    "Jul",    "Aug",
         "Sep",    "Oct",    "Nov",    "Dec"
    };

    yr = (hdr->date >> 9) & 0x7f;      /* dissect the date */
    mo = (hdr->date >> 5) & 0x0f;
    dy = hdr->date & 0x1f;

    hh = (hdr->time >> 11) & 0x1f;     /* dissect the time */
    mm = (hdr->time >> 5) & 0x3f;

    printf("%-14s  %8ld  ",hdr->name,hdr->length);

    if(bose)
    {
        switch(hdrver)
        {
        case 1:
        case 2:
            printf("   --   ");
            break;
        case 3:
            printf(" Packed ");
            break;
        case 4:
            printf("Squeezed");
            break;
        case 5:
        case 6:
        case 7:
            printf("crunched");
            break;
        case 8:
            printf("Crunched");
            break;
        default:
            printf("Unknown!");
        }

        if (hdr->length > 0)
            printf("  %3d%%",100L - (100L*hdr->size)/hdr->length);
        else
            printf("    0%%");
        printf("  %8ld  ",hdr->size);
    }

    printf("%2d %3s %02d", dy, mon[mo-1], (yr+80)%100);

    if(bose)
        printf("  %2d:%02d%c  %04x",
              ((hh)?(hh>12?hh-12:hh):12), mm, (hh>11?'p':'a'),
              (unsigned INT)hdr->crc);

    printf("\n");
}
