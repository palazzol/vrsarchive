/*
 *	arcmisc.c	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

/*
 * ARC - Archive utility - ARCMISC
 * 
 * Description:
 *      This file contains miscellaneous routines for string
 *      management, file management, and program control.
 */

#include "arc.h"
#include <ctype.h>

INT rempath(nargs,arg)               /* remove paths from filenames */
INT nargs;                           /* number of names */
char *arg[];                         /* pointers to names */
{
    char *i, *strrchr();             /* string index, reverse indexer */
    INT n;                           /* index */

    for(n=0; n<nargs; n++)           /* for each supplied name */
    {
        i=strrchr(arg[n],'/');       /* search for end of path */
        if(i)                        /* if path was found */
            arg[n] = i+1;            /* then skip it */
    }
}

/* make a file name using a template */
makefnam(rawfn,template,result)
char *rawfn;                         /* the original file name */
char *template;                      /* the template data */
char *result;                        /* where to place the result */
{
    char *arc_ext = ".arc";          /* possible existing extension */
    char *i, *strrchr();             /* string indexing stuff */

    setmem(result, STRLEN, '\0');    /* need to zero out result for strncpy */

    if (i = strrchr(rawfn,'.'))      /* strip 'arc' extension from filename */
        if (strcmp(i,arc_ext) == 0) *i = '\0';

    strncpy(result,rawfn,10);        /* rebuild it using supplied template */
    strcat(result,template);
    return;
}

/*  convert a string to upper case  */
upper(s)
char *s;
{
    while (*s)
    {
        *s = toupper(*s);
        ++s;
    }
}

setmem(dest,size,c)
char *dest,c;
INT size;
{
    int i;

    for (i = 0; i < size; dest[i] = c, i++);
}

abort(s,arg1,arg2,arg3)                /* something went wrong...QUIT!! */
char *s;
{
    char tmpnam1[STRLEN], tmpnam2[STRLEN];

    sprintf(tmpnam1,"%s.crn",arctemp);
    sprintf(tmpnam2,"%s.cvt",arctemp);

    unlink(bakname);                   /* remove all possible temp files */
    unlink(newname);
    unlink(tmpnam1);
    unlink(tmpnam2);

    fprintf(stderr,"arc: ");           /* explain things to the user */
    fprintf(stderr,s,arg1,arg2,arg3);
    fprintf(stderr,"\n");
    exit(1);                           /* quit */
}

rename(o, n)
char *o, *n;
{
    return(link(o, n) || unlink(o));
}

makenames(rawfn)
char *rawfn;
{
    char pathtemp[STRLEN];             /* temporary path holder */
    char nametemp[STRLEN];             /* temporary arcname holder */
    char *buf;                         /* temporary pointer */
    char *i, *strrchr();               /* string indexing junk */
    long getpid();                     /* process id function */

    strcpy(pathtemp,rawfn);
    if (i = strrchr(buf=rawfn,'/'))    /* if names are part of paths */
    {                                  /* lots to do */
        buf=i+1;
        pathtemp[strlen(rawfn)-strlen(buf)]='\0';

        makefnam(buf,".arc",nametemp); /* make 'arcname' */
        sprintf(arcname,"%s%s",pathtemp,nametemp);

        makefnam(buf,".bak",nametemp); /* make 'bakname' */
        sprintf(bakname,"%s%s",pathtemp,nametemp);

        sprintf(arctemp,"%s.Arc%ld",pathtemp,getpid());
    }
    else                               /* not so much to do */
    {
        makefnam(rawfn,".arc",arcname);
        makefnam(rawfn,".bak",bakname);

        sprintf(arctemp,".Arc%ld",getpid());
    }
    sprintf(newname,"%s.arc",arctemp);
}

onintr()                               /* SIGNAL was caught */
{
    abort("User Requested Abort");
}

/*
 * This function sorts the command line file arguments.  Needed since
 * the add, update, etc., function does no sorting, and could result in
 * multiple archive entries for the same file name.
 */
sortarg(num,arg)                       /* sort argument list, remove dups */
int num;
char *arg[];
{
    char *temp;                        /* temporary pointer */
    INT top, seek;                     /* placeholders for sorting */
    INT dups = 0;                      /* how many duplicates are there */
    char *strrchr(), *i;               /* string indexing stuff */
    char *buf1, *buf2;                 /* pointers for strcmp to use */

    /* sort the arguments, ignoring pathnames */

    for (top = 0;top < num-1;top++)
        for (seek = top+1;seek<num;seek++)
        {
            buf1 = arg[top];
            buf2 = arg[seek];
            if (i = strrchr(arg[top],'/')) buf1 = i + 1;
            if (i = strrchr(arg[seek],'/')) buf2 = i + 1;
            if (strcmp(buf1,buf2) > 0)
            {
                temp = arg[top];
                arg[top] = arg[seek];
                arg[seek] = temp;
            }
        }

    /* find any occurences of 'arcname', and remove them */

    for (top = 0;top < num;top++)
        while (strcmp(arg[top],arcname) == 0)
        {
            for (seek = top; seek < num;seek++)
                arg[seek] = arg[seek + 1];
            arg[--num] = '\0';
            dups++;
        }

    /* find any other duplicate arguments (ignoring pathnames), */
    /* and remove the second and subsequent occurences */

    for (top = 0;top < num-1;top++)
    {
        buf1 = arg[top];
        buf2 = arg[top + 1];
        if (i = strrchr(arg[top],'/')) buf1 = i + 1;
        if (i = strrchr(arg[top + 1],'/')) buf2 = i + 1;
        while (strcmp(buf1,buf2) == 0)
        {
            for (seek = top + 1;seek < num;seek++)
                arg[seek] = arg[seek + 1];
            arg[--num] = '\0';
            buf2 = arg[top + 1];
            if (i = strrchr(arg[top + 1],'/')) buf2 = i + 1;
            dups++;
        }
    }
    return(dups);              /* tell main() how many we found */
}
