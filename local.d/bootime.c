/*
 * bootime - Set "time of system boot" in kernel
 *
 * This program sets the time at which the kernel believes it was
 * booted.  It is intended to be run out of /etc/rc after 'asktime'
 * so '_bootime' hold the time the system was rebooted instead of
 * the time the system went down.
*/

#include <stdio.h>
#include <a.out.h>

struct	nlist nl[] = {
	{ "_bootime" },
#define	X_BOOTIME	0
	{ 0 },
};
int	kmem;			/* Kernel memory			*/
long	bootime;		/* Time of last reboot			*/
long	lseek();
long	time();

main()
{ if ((kmem = open("/dev/kmem", 1)) < 0) {
    (void) fprintf(stderr, "No kmem\n");
    (void) exit(1);
  }
  (void) nlist("/xenix", nl);
  if (nl[0].n_type == 0) {
    (void) fprintf(stderr, "No namelist\n");
    (void) exit(1);
  }
  (void) time(&bootime);	/* Get time now				*/
  (void) lseek(kmem, (long)nl[X_BOOTIME].n_value, 0);
  (void) write(kmem, (char *)&bootime, sizeof bootime);
}
