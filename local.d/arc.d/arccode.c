/*
 *	arccode.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/21/87
 *
 */

/*
 * ARC - Archive utility - ARCCODE
 * 
 * Version 1.02, created on 01/20/86 at 13:33:35
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains the routines used to encrypt and decrypt
 *          data in an archive.  The encryption method is nothing fancy,
 *          being just a routine XOR, but it is used on the packed data,
 *          and uses a variable length key.  The end result is something
 *          that is in theory crackable, but I'd hate to try it.  It should
 *          be more than sufficient for casual use.
 */

#include "arc.h"

static char *p;                        /* password pointer */

INT setcode()                          /* get set for encoding/decoding */
{
    p = password;                      /* reset password pointer */
}

INT code(c)                            /* encode some character */
INT c;                                 /* character to encode */
{
    if (p)                             /* if password is in use */
    {
        if (!*p)                       /* if we reached the end */
            p = password;              /* then wrap back to the start */
        return(c^*p++);                /* very simple here */
    }
    else
        return(c);                     /* else no encryption */
}
