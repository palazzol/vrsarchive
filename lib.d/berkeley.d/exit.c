/*
 *	%W% %G%
 *	Hook to get sockets closed properly on exit().  This would be better
 *	done in the kernel, but...
*/
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

extern void _cleanup();
extern int _exit();

int
exit(status)
int status;
{  int i;

   _cleanup();			/* Call fclose on everything in stdio	*/
   for (i = 0; i < NOFILE; i++)
     close(i);			/* Close all the sockets and files	*/
   return(_exit(status));	/* Do an exit syscall			*/
}
