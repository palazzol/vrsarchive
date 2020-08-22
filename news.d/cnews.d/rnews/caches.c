/*
 * cache control
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

int
loadcaches()				/* reload in-core caches from disk */
{
	return artload();
}

int
synccaches()				/* force in-core caches to disk */
{
	return artsync();
}
