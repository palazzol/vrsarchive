/*
 *	%W% %G% - Get the number of file descriptors
*/
#include <sys/param.h>

int
getdtablesize()
{ return(NOFILE);
}
