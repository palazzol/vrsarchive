/*
 *	%W% %G%
 *	Berkeley style file.h - Usually want O_ symbols from fcntl.h.
*/
#include <sys/types.h>
#include <fcntl.h>
#include <sys/locking.h>

#define flock(fd, mode)	locking(fd, mode, 0L)
#define LOCK_EX	LK_LOCK
#define LOCK_UN	LK_UNLCK
