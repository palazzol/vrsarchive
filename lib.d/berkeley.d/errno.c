/*
 *	%W% %G%
 *	Routines to handle error reporting.
*/
#include <errno.h>
#include <stdio.h>	/* Only for NULL	*/

int errno;

static char *errmsgs[] = {
  "Error 0\n",
  "Not super-user\n",
  "No such file or directory\n",
  "No such process\n",
  "Interrupted system call\n",
  "I/O error\n",
  "No such device or address\n",
  "Arg list too long\n",
  "Exec format error\n",
  "Bad file number\n",
  "No children\n",
  "No more processes\n",
  "Not enough core\n",
  "Permission denied\n",
  "Bad address\n",
  "Block device required\n",
  "Mount device busy\n",
  "File exists\n",
  "Cross-device link\n",
  "No such device\n",
  "Not a directory\n",
  "Is a directory\n",
  "Invalid argument\n",
  "File table overflow\n",
  "Too many open files\n",
  "Not a typewriter\n",
  "Text file busy\n",
  "File too large\n",
  "No space left on device\n",
  "Illegal seek\n",
  "Read only file system\n",
  "Too many links\n",
  "Broken pipe\n",
  "Math arg out of domain of function\n",
  "Math result not representable\n",
  "File system needs cleaning\n",
  "Would deadlock\n",
  "Not a name file\n",
  "Not available\n",
  "Is a name file\n",
  "Message too long\n",
  "Protocol wrong type for socket\n",
  "Protocol not available\n",
  "Protocol not supported\n",
  "Socket type not supported\n",
  "Operation not supported on socket\n",
  "Protocol family not supported\n",
  "Address family not supported by protocol family\n",
  "Address already in use\n",
  "Can't assign requested address\n",
  "Network is down\n",
  "Network is unreachable\n",
  "Network dropped connection on reset\n",
  "Software caused connection abort\n",
  "Connection reset by peer\n",
  "No buffer space available\n",
  "Socket is already connected\n",
  "Socket is not connected\n",
  "Can't send after socket shutdown\n",
  "Too many references: can't splice\n",
  "Connection timed out\n",
  "Connection refused\n",
  "Too many levels of symbolic links\n",
  "File name too long\n",
  "Host is down\n",
  "No route to host\n",
  "Directory not empty\n",
  "Too many processes\n",
  "Too many users\n",
  "Disc quota exceeded\n",
  "Operation would block\n",
  "Operation now in progress\n",
  "Operation already in progress\n",
  "Socket operation on non-socket\n",
  "Destination address required\n"
};

int sys_nerr = sizeof(errmsgs)/sizeof(errmsgs[0]);

char *
errstr(errn)
int errn;
{ if ((errn < 0) || (errn > sys_nerr))
    return((char *)NULL);
  return(errmsgs[errn]);
}

void
perror(msg1)
char *msg1;
{ register char *msg2;
  register unsigned l;

  msg2 = errstr(errno);
  if (msg2 == NULL)
    msg2 = "Unknown Error";
  if ((msg1 != NULL) && (strlen(msg1) != 0)) {
    l = strlen(msg1);
    write(2, msg1, l);
    write(2, ": ", 2);
  }
  l = strlen(msg2);
  write(2, msg2, l);
}
