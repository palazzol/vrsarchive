#define _POSIX_SOURCE
/*
 *	arcdos.c	1.2
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	BSD specific code provided by: Jeff Chiu
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	5/15/87
 *
 */

/*
 * ARC - Archive utility - ARCDOS
 * 
 * Version 1.43, created on 11/09/85 at 22:24:44
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains certain DOS level routines that assist
 *          in doing fancy things with an archive, primarily reading and
 *          setting the date and time last modified.
 * 
 *          These are, by nature, system dependant functions.  But they are
 *          also, by nature, very expendable.
 */

#include "arc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

INT getstamp(f,date,time)              /* get a file's date/time stamp */
FILE *f;                               /* file to get stamp from */
unsigned INT *date, *time;             /* storage for the stamp */
{
    struct stat buf;
    struct tm *tmbuf;

    fstat(fileno(f),&buf);
    tmbuf=localtime(&buf.st_mtime);
    
    *date = ((tmbuf->tm_year-80)<<9) + ((tmbuf->tm_mon+1)<<5) + tmbuf->tm_mday;
    *time = (tmbuf->tm_hour<<11) + (tmbuf->tm_min<<5) + (tmbuf->tm_sec>>1);;
}

struct utimbuf {
    time_t	actime;
    time_t	modtime;
};

INT setstamp(file,date,time)           /* set a file's date/time stamp */
char *file;                            /* file to set stamp on */
unsigned INT date, time;               /* desired date, time */
{
    struct utimbuf times;
    struct tm *tmbuf;
    long m_time;
    int yr, mo, dy, hh, mm, ss, leap, days = 0;

    /*
     * These date conversions look a little wierd, so I'll explain.
     * UNIX bases all file modification times on the number of seconds
     * elapsed since Jan 1, 1970, 00:00:00 GMT.  Therefore, to maintain
     * compatibility with MS-DOS archives, which date from Jan 1, 1980,
     * with NO relation to GMT, the following conversions must be made:
     * 		the Year (yr) must be incremented by 10;
     *		the Date (dy) must be decremented by 1;
     *		and the whole mess must be adjusted by TWO factors:
     *			relationship to GMT (ie.,Pacific Time adds 8 hrs.),
     *			and whether or not it is Daylight Savings Time.
     * Also, the usual conversions must take place to account for leap years,
     * etc.
     *                                     C. Seaman
     */

    yr = (((date >> 9) & 0x7f) + 10);  /* dissect the date */
    mo = ((date >> 5) & 0x0f);
    dy = ((date & 0x1f) - 1);

    hh = ((time >> 11) & 0x1f);        /* dissect the time */
    mm = ((time >> 5) & 0x3f);
    ss = ((time & 0x1f) * 2);

    leap = ((yr+1970)/4);              /* Leap year base factor */

    /* How many days from 1970 to this year? */
    days = (yr * 365) + (leap - 492);

    switch(mo)			       /* calculate expired days this year */
    {
    case 12:
        days += 30;
    case 11:
        days += 31;
    case 10:
        days += 30;
    case 9:
        days += 31;
    case 8:
        days += 31;
    case 7:
        days += 30;
    case 6:
        days += 31;
    case 5:
        days += 30;
    case 4:
        days += 31;
    case 3:
        days += 28;                    /* account for leap years */
        if ((leap * 4) == (yr+1970) && (yr+1970) != 2000)
            ++days;
    case 2:
        days += 31;
    }

    /* convert date & time to seconds relative to 00:00:00, 01/01/1970 */
    m_time = ((days + dy) * 86400) + (hh * 3600) + (mm * 60) + ss;

#ifdef BSD
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    if (tz.tz_dsttime != 0)
        m_time -= 3600;

    m_time += tz.tz_minuteswest * 60;  /* account for timezone differences */
#else
    {
#ifndef __STDC__
    extern long timezone;
#endif
    tmbuf = localtime(&m_time);        /* check for Daylight Savings Time */
    if (tmbuf->tm_isdst != 0)
        m_time -= 3600;

    m_time += timezone;                /* account for timezone differences */
    }
#endif

    times.actime = m_time;             /* set the stamp on the file */
    times.modtime = m_time;
    utime(file,&times);
}
