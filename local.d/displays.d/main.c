/*
 *	The main loop.
*/
#define GLOBAL
#include "globals.h"

extern void done();

main()
{
	pipe_init();		/* Init the ptys for each vdisp	*/
	while (1)
		pchar();	/* Is there any output waiting?	*/
}
