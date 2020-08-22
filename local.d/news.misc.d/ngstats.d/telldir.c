/*
	telldir -- C library extension routine

	last edit:	09-Jul-1983	D A Gwyn
*/

#include	<dir.h>

extern long	lseek();

long
telldir( dirp )
	DIR	*dirp;			/* stream from opendir() */
	{
	return lseek( dirp->dd_fd, 0L, 1 ) - (long)dirp->dd_size
	     + (long)dirp->dd_loc;
	}
