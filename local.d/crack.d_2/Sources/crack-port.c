#include "crack.h"

/*
 * This program is copyright (c) Alec Muffett 1991 except for certain
 * portions of code ("crack-fcrypt.c") copyright (c) Robert Baldwin, Icarus
 * Sparry and Alec Muffett.  The author(s) disclaims all responsibility or
 * liability with respect to it's usage or its effect upon hardware or
 * computer systems.  This software is in freely redistributable PROVIDED
 * that this notice remains intact.
 */

#ifdef CRACK_UNAME

#include <sys/utsname.h>

int
gethostname (name, namelen)
    char *name;
    int namelen;
{
    struct utsname uts;
    if (uname (&uts))
    {
	return (-1);
    }
    strncpy (name, uts.nodename, namelen - 1);
    return (0);
}
#endif				/* CRACK_UNAME */
