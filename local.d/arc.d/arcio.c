/*
 *	arcio.c	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

/*
 * ARC - Archive utility - ARCIO
 * 
 * Version 2.30, created on 02/03/86 at 22:56:00
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the file I/O routines used to manipulate
 *          an archive.
 */

#include "arc.h"

INT readhdr(hdr,f)                     /* read a header from an archive */
struct heads *hdr;                     /* storage for header */
FILE *f;                               /* archive to read header from */
{
    INT i;                             /* misc. variables */
    unsigned fnlen = (ibmpc) ? FNLEN2 : FNLEN1; /* file name length */
    unsigned char dummy[30];           /* dummy array for header storage */
    char name[FNLEN1];                 /* filename buffer */
    INT try = 0;                       /* retry counter */
    static INT first = 1;              /* true only on first read */

    if (!f)                            /* if archive didn't open */
        return(0);                     /* then pretend it's the end */
    if (feof(f))                       /* if no more data */
        return(0);                     /* then signal end of archive */

    if (fgetc(f)!=ARCMARK)             /* check archive validity */
    {
        if (warn)
        {
            printf("An entry in %s has a bad header.\n",arcname);
            nerrs++;
        }

        while (!feof(f))
        {
            try++;
            if (fgetc(f)==ARCMARK)
            {
                ungetc(hdrver=fgetc(f),f);
                if (hdrver>=0 && hdrver<=ARCVER)
                    break;
            }
        }

        if (feof(f) && first)
            abort("%s is not an archive",arcname);

        if (warn)
            printf("  %d bytes skipped.\n",try);

        if (feof(f))
            return(0);
    }

    hdrver = fgetc(f);                 /* get header version */
    if (hdrver<0)
        abort("Invalid header in archive %s",arcname);
    if (hdrver==0)
        return(0);                     /* note our end of archive marker */
    if (hdrver>ARCVER)
    {
        fread(name,sizeof(char),fnlen,f);
        printf("I don't know how to handle file %s in archive %s\n",
            name,arcname);
        printf("I think you need a newer version of ARC.\n");
        abort("Archive error");
    }

    /* amount to read depends on header type */

    if (hdrver==1)                     /* old style is shorter */
    {
        fread(hdr,sizeof(struct heads)-sizeof(long),1,f);
        hdrver = 2;                    /* convert header to new format */
        hdr->length = hdr->size;       /* size is same when not packed */
    }
    else
    {
        fread(dummy,fnlen+14,1,f);

        for (i=0;i<FNLEN1;hdr->name[i]='\0',i++);
        for (i=0;i<fnlen;hdr->name[i]=dummy[i],i++);
        hdr->size = (long)((dummy[fnlen+3]<<24) + (dummy[fnlen+2]<<16)
            + (dummy[fnlen+1]<<8) + dummy[fnlen]);
        hdr->date = (short)((dummy[fnlen+5]<<8) + dummy[fnlen+4]);
        hdr->time = (short)((dummy[fnlen+7]<<8) + dummy[fnlen+6]);
        hdr->crc  = (short)((dummy[fnlen+9]<<8) + dummy[fnlen+8]);
        hdr->length = (long)((dummy[fnlen+13]<<24) + (dummy[fnlen+12]<<16)
            + (dummy[fnlen+11]<<8) + dummy[fnlen+10]);
    }

    first = 0;
    return(1);                         /* we read something */
}

INT writehdr(hdr,f)                    /* write a header to an archive */
struct heads *hdr;                     /* header to write */
FILE *f;                               /* archive to write to */
{
    int i;
    unsigned fnlen = (ibmpc) ? FNLEN2 : FNLEN1; /* file name length */

    fputc(ARCMARK,f);                  /* write out the mark of ARC */
    fputc(hdrver,f);                   /* write out the header version */
    if (!hdrver)                       /* if that's the end */
        return;                        /* then write no more */

    for (i = strlen(hdr->name);i < fnlen;i++)
        hdr->name[i] = '\0';

    fwrite(hdr->name,1,fnlen,f);
    fputc((char)hdr->size&255,f);         fputc((char)(hdr->size>>8)&255,f);
    fputc((char)(hdr->size>>16)&255,f);   fputc((char)(hdr->size>>24)&255,f);
    fputc((char)hdr->date&255,f);         fputc((char)(hdr->date>>8)&255,f);
    fputc((char)hdr->time&255,f);         fputc((char)(hdr->time>>8)&255,f);
    fputc((char)hdr->crc&255,f);          fputc((char)(hdr->crc>>8)&255,f);
    fputc((char)hdr->length&255,f);       fputc((char)(hdr->length>>8)&255,f);
    fputc((char)(hdr->length>>16)&255,f); fputc((char)(hdr->length>>24)&255,f);

    /* note the newest file for updating the archive timestamp */

    if (hdr->date>arcdate ||
       (hdr->date==arcdate && hdr->time>arctime))
    {
        arcdate = hdr->date;
        arctime = hdr->time;
    }
}

INT filecopy(f,t,size)                 /* bulk file copier */
FILE *f, *t;                           /* from, to */
long size;                             /* number of bytes */
{
    INT putc_tst();

    while (size--)                     /* while more bytes to move */
        putc_tst(fgetc(f),t);
}

INT putc_tst(c,t)                      /* put a character, with tests */
char c;                                /* character to output */
FILE *t;                               /* file to write to */
{
    if (t)
    {
        fputc(c,t);
        if (ferror(t))
        {
            perror("system error:");
            abort("Write failed");
        }
    }
}
