/*
 *	arcusq.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCUSQ
 * 
 * Version 3.13, created on 01/30/86 at 20:11:42
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to expand a file
 *          which was packed using Huffman squeezing.
 * 
 *          Most of this code is taken from an USQ program by Richard
 *          Greenlaw, which was adapted to CI-C86 by Robert J. Beilstein.
 */

#include "arc.h"

/* stuff for Huffman unsqueezing */

#define ERROR (-1)
#define SPEOF 256                      /* special endfile token */
#define NUMVALS 257                    /* 256 data values plus SPEOF */

struct u_nd {                          /* decoding tree */
  INT child[2];                        /* left, right */
} u_node[NUMVALS];                     /* use large buffer */

static INT bpos;                       /* last bit position read */
static INT curin;                      /* last byte value read */
static INT numnodes;                   /* number of nodes in decode tree */

static INT get_int(f)                  /* get an integer */
FILE *f;                               /* file to get it from */
{
    INT i;

    i = getc_unp(f);
    return((short)(i | (getc_unp(f)<<8)));
}

INT init_usq(f)                        /* initialize Huffman unsqueezing */
FILE *f;                               /* file containing squeezed data */
{
    INT i;                             /* node index */

    bpos = 99;                         /* force initial read */

    numnodes = get_int(f);

    if (numnodes<0 || numnodes>=NUMVALS)
        abort("File has an invalid decode tree");

    /* initialize for possible empty tree (SPEOF only) */

    u_node[0].child[0] = -(SPEOF + 1);
    u_node[0].child[1] = -(SPEOF + 1);

    for (i=0; i<numnodes; ++i)         /* get decoding tree from file */
    {
        u_node[i].child[0] = get_int(f);
        u_node[i].child[1] = get_int(f);
    }
}

INT getc_usq(f)                        /* get byte from squeezed file */
FILE *f;                               /* file containing squeezed data */
{
    INT i;                             /* tree index */

    /* follow bit stream in tree to a leaf */

    for (i=0; i>=0; )                  /* work down(up?) from root */
    {
        if (++bpos>7)
        {
            if ((curin=getc_unp(f)) == ERROR)
                return(ERROR);
            bpos = 0;

            /* move a level deeper in tree */
            i = u_node[i].child[1&curin];
        }
        else
            i = u_node[i].child[1 & (curin >>= 1)];
    }

    /* decode fake node index to original data value */

    i = -(i + 1);

    /* decode special endfile token to normal EOF */

    i = (i==SPEOF) ? EOF : i;
    return(i);
}
