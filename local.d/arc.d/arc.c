/*
 *	arc.c	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

#ifndef	lint
static char	sccsid[] = "@(#)arc.c	1.2";
#endif	/* lint */


/*
 * ARC - Archive utility
 * 
 * Version 5.12, created on 02/05/86 at 22:22:01
 * 
 * (C) COPYRIGHT 1985,86 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This program is a general archive utility, and is used to maintain
 *          an archive of files.  An "archive" is a single file that combines
 *          many files, reducing storage space and allowing multiple files to
 *          be handled as one.
 * 
 *     Instructions:
 *          Run this program with no arguments for complete instructions.
 * 
 *     Programming notes:
 *          ARC Version 2 differs from version 1 in that archive entries
 *          are automatically compressed when they are added to the archive,
 *          making a separate compression step unecessary.  The nature of the
 *          compression is indicated by the header version number placed in
 *          each archive entry, as follows:
 * 
 *          1 = Old style, no compression
 *          2 = New style, no compression
 *          3 = Compression of repeated characters only
 *          4 = Compression of repeated characters plus Huffman SQueezing
 *          5 = Lempel-Zev packing of repeated strings (old style)
 *          6 = Lempel-Zev packing of repeated strings (new style)
 *          7 = Lempel-Zev Williams packing with improved has function
 *          8 = Dynamic Lempel-Zev packing with adaptive reset
 * 
 *          Type 5, Lempel-Zev packing, was added as of version 4.0
 * 
 *          Type 6 is Lempel-Zev packing where runs of repeated characters
 *          have been collapsed, and was added as of version 4.1
 * 
 *          Type 7 is a variation of Lempel-Zev using a different hash
 *          function which yields speed improvements of 20-25%, and was
 *          added as of version 4.6
 * 
 *          Type 8 is a different implementation of Lempel-Zev, using a
 *          variable code size and an adaptive block reset, and was added
 *          as of version 5.0
 * 
 *          Verion 4.3 introduced a temporary file for holding the result
 *          of the first crunch pass, thus speeding up crunching.
 * 
 *        { Version 4.4 introduced the ARCTEMP environment string, so that }
 *        { the temporary crunch file may be placed on a ramdisk.  Also    }
 *        { added was the distinction bewteen Adding a file in all cases,  }
 *        { and Updating a file only if the disk file is newer than the    }
 *        { corresponding archive entry.                                   }
 *           (NOT USED IN THIS RELEASE OF THE SYSTEM V VERSION)
 * 
 *          The compression method to use is determined when the file is
 *          added, based on whichever method yields the smallest result.
 * 
 */
#include "arc.h"

main(argc,argv)                        /* system entry point */
int argc;                              /* number of arguments */
char *argv[];                          /* pointers to arguments */
{
    char opt = 0;                      /* selected action */
    char *a;                           /* option pointer */
    char *upper();                     /* case conversion routine */
    INT dups = 0;                      /* duplicate argument counter */
    SIG_T onintr();                    /* funtion to call when SIGNAL caught */

    /* Basic signal trapping */
    signal(SIGHUP,SIG_IGN);
    signal(SIGINT,onintr);
    if (signal(SIGQUIT,SIG_IGN) != SIG_IGN) signal(SIGQUIT,onintr);

    warn = 1;
    note = 1;

    if (argc<3)
    {
        printf("%s - Archive Utility, 5.12\n",argv[0]);
        printf("System V Version 1.2, 5/15/87\n\n");
        printf("Usage: %s [-]{amufdxeplvtc}[biswn][g<password>]",argv[0]);
        printf(" <archive> [<filename> ... ]\n");
        printf("Where:   a   = add files to archive\n");
        printf("         m   = move files to archive\n");
        printf("         u   = update files in archive\n");
        printf("         f   = freshen files in archive\n");
        printf("         d   = delete files from archive\n");
        printf("         x,e = extract files from archive\n");
        printf("         p   = copy files from archive to standard output\n");
        printf("         l   = list files in archive\n");
        printf("         v   = verbose listing of files in archive\n");
        printf("         t   = test archive integrity\n");
        printf("         c   = convert entry to new packing method\n\n");
        printf("         b   = retain backup copy of archive\n");
        printf("         i   = maintain IBM(TM) PC compatible archive\n");
        printf("         s   = suppress compression (store only)\n");
        printf("         w   = suppress warning messages\n");
        printf("         n   = suppress notes and comments\n");
        printf("         g   = Encrypt/decrypt archive entry\n\n");
        exit(1);
    }

    /* avoid any case problems with command options */
    upper(argv[1]);                /* convert it to uppercase */

    /* parse the option argument */

    for (a=argv[1]; *a; a++)
    {
        switch(*a)
        {
        case 'A':                       /* if a known command */
        case 'M':
        case 'U':
        case 'F':
        case 'D':
        case 'X':
        case 'E':
        case 'P':
        case 'L':
        case 'V':
        case 'T':
        case 'C':
            if (opt)                    /* do we have one yet? */
                arcabort("Cannot mix %c and %c",opt,*a);
            else
                opt = *a;               /* else remember it */
            break;
        case 'B':                       /* retain backup copy */
            keepbak = 1;
            break;
        case 'I':                       /* IBM compatibility */
            ibmpc = 1;
            break;
        case 'W':                       /* suppress warnings */
            warn = 0;
            break;
        case 'N':                       /* suppress notes and comments */
            note = 0;
            break;
        case 'G':                       /* garble */
            password = a+1;
            while (*a)
                a++;
            a--;
            break;
        case 'S':                       /* storage kludge */
            nocomp = 1;
            break;
        case '-':                       /* UNIX option marker */
            break;
        default:
            arcabort("%c is an unknown command",*a);
        }
    }

    /* get out if no option made it through */
    if (!opt)
        arcabort("I don't know what to do!");

    /* create archive names (arctemp, arcname, newname, bakname) */
    makenames(argv[2]);

    /* strip off used args */
    argc -=3;
    argv +=3;

    /* if file args, sort them */
    if (argc)
    {
        dups = sortarg(argc,argv);
        argc -= dups;
    }

    /* act on whatever action command was given */

    switch(opt)                        /* action depends on command */
    {
    case 'A':                          /* Add */
    case 'M':                          /* Move */
    case 'U':                          /* Update */
    case 'F':                          /* Freshen */
        addarc(argc,argv,(opt=='M'),(opt=='U'),(opt=='F'));
        break;
    case 'D':                          /* Delete */
        delarc(argc,argv);
        break;
    case 'E':                          /* Extract */
    case 'X':                          /* eXtract */
    case 'P':                          /* Print */
        extarc(argc,argv,(opt=='P'));
        break;
    case 'V':                          /* Verbose list */
        bose = 1;
    case 'L':                          /* List */
        lstarc(argc,argv);
        break;
    case 'T':                          /* Test */
        tstarc();
        break;
    case 'C':                          /* Convert */
        cvtarc(argc,argv);
    }

    exit(nerrs);
}
