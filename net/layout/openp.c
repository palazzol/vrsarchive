/*  openp, fopenp  --  search pathlist and open file
 *
 *  Usage:
 *	i = openp (path,file,complete,mode)
 *	f = fopenp (path,file,complete,fmode)
 *	int i,mode;
 *	FILE *f;
 *	char *path,*file,*complete,*fmode;
 *
 *  Openp searches for "file" in the pathlist "path";
 *  when the file is found and can be opened by open()
 *  with the specified "mode", then the full filename
 *  is copied into "complete" and openp returns the file
 *  descriptor.  If no such file is found, openp returns -1.
 *  Fopenp performs the same function, using fopen() instead
 *  of open() and fmode instead of mode; it returns 0 if no
 *  file is found.
 *
 *  HISTORY
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.
 *
 */

#include <stdio.h>

int open();
int searchp();

static int mod,value;
static char *fmod;
static FILE *fvalue;

static int func (fnam)
char *fnam;
{
	value = open (fnam,mod);
	return (value < 0);
}

static int ffunc (fnam)
char *fnam;
{
	fvalue = fopen (fnam,fmod);
	return (fvalue == 0);
}

int openp (path,file,complete,mode)
char *path, *file, *complete;
int mode;
{
	register char *p;
	mod = mode;
	if (searchp(path,file,complete,func) < 0)  return (-1);
	return (value);
}

FILE *fopenp (path,file,complete,fmode)
char *path, *file, *complete, *fmode;
{
	register char *p;
	fmod = fmode;
	if (searchp(path,file,complete,ffunc) < 0)  return (0);
	return (fvalue);
}
