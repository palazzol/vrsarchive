/*  Selected parts from Wagar's rand.c
    Copyright (c) 1982 Steven L. Wagar.  All rights reserved. */
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>

double  RandSeed,floor(),pow();

double frac(d)          /* returns the fractional part of a double */
double d;
{
        return (d-floor(d));
}

rninit()                /* seed is of form .mmmsss (m = millisecs, s = secs) */
{
        struct timeb tbuf;

        ftime(&tbuf);
        RandSeed = tbuf.millitm/1000.0 + frac(tbuf.time/1000.0) / 1000.0;
}

double rn()             /* remainder of hairy exponential */
{
        return (RandSeed = frac(pow(RandSeed*4.32 + 3.52, 3.64)));
}
