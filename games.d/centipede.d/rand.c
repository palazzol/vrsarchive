/*  Selected parts from Wagar's rand.c
    Copyright (c) 1982 Steven L. Wagar.  All rights reserved. */
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#ifndef SYSV
#include <sys/timeb.h>
#endif

double  RandSeed,floor(),pow();

double frac(d)          /* returns the fractional part of a double */
double d;
{
        return (d-floor(d));
}

#ifdef SYSV
rninit()
{
    RandSeed = (double) time((long *) 0);
}

#else
rninit()                /* seed is of form .mmmsss (m = millisecs, s = secs) */
{
        struct timeb tbuf;

        ftime(&tbuf);
        RandSeed = tbuf.millitm/1000.0 + frac(tbuf.time/1000.0) / 1000.0;
} 
#endif

double rn()             /* remainder of hairy exponential */
{
        return (RandSeed = frac(pow(RandSeed*4.32 + 3.52, 3.64)));
}

