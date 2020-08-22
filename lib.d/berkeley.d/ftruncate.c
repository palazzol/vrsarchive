/*
 *	%W% %G%
 *	The ftruncate() call truncates a given file to a given size.  This
 *	is essentially equivalent to the chsize() call.
 *
 *	It would be faster to #define ftruncate chsize in some include file.
 *	Unfortunately there is no include file which is a likely candidate,
 *	so a reliable emulation requires a real function.
*/

int
ftruncate(fd, size)
int fd;
long size;
{ return(chsize(fd, size));
}
