/*
 *	arcext.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCEXT
 * 
 * Version 2.18, created on 02/03/86 at 22:55:19
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to extract files from
 *          an archive.
 */

#include "arc.h"
INT extarc(argc,argv,prt)              /* extract files from archive */
INT argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
INT prt;                               /* true if printing */
{
    struct heads hdr;                  /* file header */
    INT save;                          /* true if current entry matches */
    INT did[MAXARG];                   /* true when argument was used */
    char *i, *strrchr();               /* string index */
    char *fbuf;                        /* temporary storage for name */
    INT n;                             /* index */
    INT extfile();

    for (n=0; n<argc; n++)             /* for each argument */
        did[n] = 0;                    /* reset usage flag */

    openarc(0);                        /* open archive for reading */

    if (argc)                          /* if files were named */
    {
        while (readhdr(&hdr,arc))      /* while more files to check */
        {
            save = 0;
            for (n=0; n<argc; n++)     /* for each template given */
            {
                fbuf = argv[n];
                if (i = strrchr(argv[n],'/')) fbuf = i + 1;
                if (match(hdr.name,fbuf))
                {
                    save = 1;          /* turn on save flag */
                    did[n] = 1;        /* turn on usage flag */
                    extfile(&hdr,argv[n],prt); /* extract the file */
                    break;             /* stop looking */
                }
            }

            if (!(save))               /* if not extracted, advance */
                fseek(arc,hdr.size,1); /* to next file */
        }
    }

    else while (readhdr(&hdr,arc))     /* else extract all files */
        extfile(&hdr,"",prt);

    closearc(0);                       /* close archive after reading */

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

INT extfile(hdr,path,prt)              /* extract a file */
struct heads *hdr;                     /* pointer to header data */
char *path;                            /* pointer to path name */
INT prt;                               /* true if printing */
{
    FILE *f, *fopen();                 /* extracted file, opener */
    char buf[STRLEN];                  /* input buffer */
    char fix[STRLEN];                  /* fixed name buffer */
    char *fbuf;                        /* temporary pointer to path */
    char *i, *strrchr();               /* string index */

    if (prt)                           /* printing is much easier */
    {
        unpack(arc,stdout,hdr);        /* unpack file from archive */
        printf("\f");                  /* eject the form */
        return;                        /* see? I told you! */
    }

    strcpy(fix,path);                  /* note path name template */
    if (i=strrchr(fbuf=path,'/'))      /* find start of name */
    {
        fbuf = i + 1;
        fix[strlen(path)-strlen(fbuf)] = '\0';
        strcat(fix,hdr->name);         /* replace template with name */
    }
    else
        strcpy(fix,hdr->name);         /* replace template with name */

    if (note)
        printf("Extracting file: %s\n",fix);

    if (warn)
    {
        if (f=fopen(fix,"r"))          /* see if it exists */
        {
            fclose(f);
            printf("WARNING: File %s already exists!",fix);
            while (1)
            {
                printf("  Overwrite it (y/n)? ");
                fgets(buf,STRLEN,stdin);
                *buf = toupper(*buf);
                if (*buf=='Y' || *buf=='N')
                    break;
            }
            if (*buf=='N')
            {
                printf("%s not extracted.\n",fix);
                fseek(arc,hdr->size,1);
                return;
            }
        }
    }

    if (!(f=fopen(fix,"w")))
    {
        if (warn)
        {
            printf("Cannot create %s\n",fix);
            nerrs++;
        }
        fseek(arc,hdr->size,1);
        return;
    }

    /* now unpack the file */

    unpack(arc,f,hdr);                 /* unpack file from archive */
    fclose(f);                         /* all done writing to file */
    setstamp(fix,hdr->date,hdr->time); /* set the proper date/time stamp */
}
