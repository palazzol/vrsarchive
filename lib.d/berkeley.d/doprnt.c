/*
 *	%W% %G%
 *	_doprnt() is an undocumented routine internal to the implementation
 *	of the printf family in many versions of UNIX.  It was never intended
 *	that the user should call it, but many programs do.  This is a version
 *	which calls _print(), the comparable function internal to the XENIX
 *	library.
 *
 *	Wish users would learn to write varargs functions properly.  Of course,
 *	we don't take our own advice by parsing 'fmt' below.  That would have
 *	us re-implementing printf().  Usually all the user really wanted was
 *	a couple of simple variations easily implemented with varargs.
*/
#include <stdio.h>

extern FILE *_pfile;		/* Internal to printf implementation	*/

_doprnt(fmt, args, file)
char *fmt;
int **args;
FILE *file;
{ _pfile = file;		/* Set to print wherever user said	*/
  _print(fmt, &args);		/* Don't ask				*/
}
