/*static char *sccsid = "@(#)cp.c	4.1 (Berkeley) 10/1/80";
 *
 * copy oldfile newfile
 *	stolen from cp.c
 */

#define	BSIZE	1024
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
struct	stat	stbuf1, stbuf2;
char	iobuf[BSIZE];

#include "espel.h"
extern	int	Debug;

int	iflag = 0;	/* interactive flag. If this flag is set,
			 * the user is queried before files are
			 * destroyed by cp.
			 */


copy(from, to)
char *from, *to;
{
	int fold, fnew, n;
	register char *p1, *p2, *bp;
	int mode;
	char c,i;
	if ((fold = open(from, 0)) < 0) {
		printf("copy: cannot open %s\n", from);
		return(1);
	}
	DEBUG(7,"file opened: %s\n", from);
	fstat(fold, &stbuf1);
	mode = stbuf1.st_mode;
	/* is target a directory?
	if (stat(to, &stbuf2) >=0 &&
	   (stbuf2.st_mode&S_IFMT) == S_IFDIR) {
		p1 = from;
		p2 = to;
		bp = iobuf;
		while(*bp++ = *p2++)
			;
		bp[-1] = '/';
		p2 = bp;
		while(*bp = *p1++)
			if (*bp++ == '/')
				bp = p2;
		to = iobuf;
	}
	*/
	if (stat(to, &stbuf2) >= 0) {
		if (stbuf1.st_dev == stbuf2.st_dev &&
		   stbuf1.st_ino == stbuf2.st_ino) {
			printf("copy: cannot copy file to itself.\n");
			return(1);
		} else if (iflag) {
			printf ( "overwrite %s? ", to);
			i = c = getchar();
			while (c != '\n' && c != EOF)
				c = getchar();
			if (i != 'y')
				return(1);
		}
	}
	if ((fnew = creat(to, mode)) < 0) {
		printf( "copy: cannot create %s\n", to);
		close(fold);
		return(1);
	}
	DEBUG(7,"file created: %s\n", to);
	while(n = read(fold,  iobuf,  BSIZE)) {
		if (n < 0) {
			fprintf(stderr, "copy: read error");
			close(fold);
			close(fnew);
			return(1);
		} else
			if (write(fnew, iobuf, n) != n) {
				fprintf(stderr, "copy: write error");
				close(fold);
				close(fnew);
				return(1);
			}
	}
	close(fold);
	close(fnew);
	return(0);
}


