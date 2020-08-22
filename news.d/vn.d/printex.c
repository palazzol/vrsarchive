/*
** vn news reader.
**
** printex.c - print fatal error message and exit.
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <setjmp.h>
#include "config.h"
#include "tty.h"

extern int errno;	/* unix error number */

/*
	error/abnormal condition cleanup and abort routine
	pass stack to printf
*/
printex (s,a,b,c,d,e,f)
char *s;
long a,b,c,d,e,f;
{
	static int topflag=0;
	if (topflag == 0)
	{
		++topflag;
		term_set (STOP);
		tty_set (COOKED);
		fflush (stdout);
		fprintf (stderr,s,a,b,c,d,e,f);
		fprintf (stderr," (error code %d)\n",errno);
		vns_exit(1);
		stat_end(-1);
		exit (1);
	}
	else
		fprintf (stderr,s,a,b,c,d,e,f);
}
