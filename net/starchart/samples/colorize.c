/* To add colors to neb.star, consider the following short program */

#include <stdio.h>

main()
{
  char inbuf[100];

  while (gets(inbuf, 100)) {
    if (inbuf[14] == 'C') { /* Cluster */
	inbuf[16] = 'y';
	inbuf[17] = '9';
    };
    if (inbuf[14] == 'N') { /* Nebula */
	inbuf[16] = 'g';
	inbuf[17] = '9';
    };
    if (inbuf[14] == 'G') { /* Galaxy */
	inbuf[16] = 'r';
	inbuf[17] = '9';
    };
    puts(inbuf);
  }
}

