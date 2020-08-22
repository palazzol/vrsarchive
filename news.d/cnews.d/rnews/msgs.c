/*
 * print common messages
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

int
fulldisk(status, file)		/* complain once & set ST_DISKFULL */
int status;
char *file;
{
	if (!(status&ST_DISKFULL))
		(void) warning("disk filled when writing %s", file);
	return status | ST_DISKFULL;
}
