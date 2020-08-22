/***************************************************************\
*								*
*	PCB program						*
*								*
*	Main section						*
*								*
*	(c) 1985		A. Nowatzyk			*
*								*
\***************************************************************/

#include <stdio.h>
#include "pparm.h"
#define mainpgm 1		/* This is the main pgm    */
#include "pcdst.h"

main (argc, argv)
    int argc;
    char *argv[];
{

    if (argc == 2 && !strcmp (argv[1], "-b")) {
	printf ("PCB is running in batch-autoroute mode\n");
	batch = 1;
    }

    if (argc == 2 && !strcmp (argv[1], "-s")) {
	printf ("PCB is running in batch-straight mode\n");
	batch = 2;
    }

    init ();			/* initialize environement */

    rdnl ();			/* read net list	   */


    if (batch && !ck_placed ())
	err ("-Please place components first", 0, 0, 0, 0);
	    
    if      (batch == 1)
	autor ();		/* start autorouting	   */
    else if (batch == 2)
	straight_all ();	/* straight wires	   */
    else
	cmd_loop ();		/* edit loop		   */

    printf ("Have a nice day\n");
    finish ();
}
