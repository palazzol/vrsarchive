#include <stdio.h>
#include <signal.h>
#define GLOBAL
#include "globals.h"

extern void done();

main(argc, argv)
char **argv;
{
	keybd_init();		/* Init the keyboard fd		*/
	pty_init();		/* Init the ptys for each vdisp	*/
	signal(SIGHUP, done);
	signal(SIGTERM, done);
	/*
	 * Main loop
	 */
	while (1) {
		kchar();	/* Is there any input waiting?	*/
		pchar();	/* Is there any output waiting?	*/
	}
}
