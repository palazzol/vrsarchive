/*
 *	The main loop.
*/
#define GLOBAL
#include "globals.h"

extern void done();

main(argc, argv)
{
	if (argc > 1)
		trace++;
	pipe_init();		/* Init the ptys for each vdisp	*/
	while (1)
		pchar();	/* Is there any output waiting?	*/
}
