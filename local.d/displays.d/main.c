/*
 *	The main loop.
*/
#define GLOBAL
#include "globals.h"
#include <stdio.h>

extern char *optarg;

main(argc, argv)
int argc;
char *argv[];
{ char opt;

  while ((opt = getopt(argc, argv, "td:")) != EOF) {
    switch (opt) {
      case 't':
        traceit++;
        break;
      case 'd':
        displays = atoi(optarg);
        if (displays > 0)
          break;
      default:
        fprintf(stderr, "Usage: %s [-t] [-d <displays>]\n", argv[0]);
        exit(1);
    }
  }
  pipe_init();		/* Init the ptys for each display and the keyboard */
  while (1)
    pchar();		/* Is there any output waiting?	*/
}
