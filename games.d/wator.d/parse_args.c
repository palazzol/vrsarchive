/*
 * parse_args.c
 *
 * Typical argument parsing.  Random initialization chosen if no args.
 */

#include <stdio.h>
#include "wator.h"

parse_args(argc, argv)
	int argc;
	char *argv[];
{
	register int i;
	register char *p;
	int n, num;

	/*
	 * Set up defaults, they usually get used.
	 */

	lines = L_MAX;	/* Can be changed when we read terminal info. */
	wide = W_MAX;
	num_fish = r_num(50, 275);
	num_shark = r_num(15, 50);
	f_breed = r_num(2,5);
	s_breed = r_num(5,12);
	starve = r_num(4, 8);

	if (argc == 1)
		return;

	i = n = 1;
	while (i < argc) {
		if (argv[i][0] == '-') {
			p = argv[i++];
			p++;
			while (*p) {
				switch(*p) {
				case 'h':
					usage();
					exit(0);
				case 's':
					if ((stat = fopen("wator.stat", "w")) == NULL) {
						printf("Cannot open wator.stat\n");
						exit(1);
					}
				default:
					printf("Unknown flag '%c'\n", *p);
					hmsg();
					exit(1);
				}
				p++;
			}
		} else {
			if (sscanf(argv[i], "%d", &num) != 1) {
				printf("Bad arg '%s'\n", argv[i]);
				hmsg();
				exit(1);
			}
			if (num < 1) {
				printf("Value too small.\n");
				hmsg();
				exit(1);
			}
			if (num >= (L_MAX-2)*(W_MAX-2)) {
				printf("Value too large.\n");
				hmsg();
				exit();
			}
			i++;
			switch(n) {
			case 1:
				num_shark = num;
				break;
			case 2:
				num_fish = num;
				break;
			case 3:
				s_breed = num;
				break;
			case 4:
				f_breed = num;
				break;
			case 5:
				starve = num;
				break;
			default:
				printf("Too many args.\n");
				hmsg();
				exit(1);
			}
			n++;
		}
	}

	if (n > 1 && n < 6) {
		printf("Too few args.\n");
		hmsg();
		exit(1);
	}

	/*
	 * Limit initial population to keep equialent of one column empty.
	 */

	if (num_fish + num_shark  > (L_MAX-1)*(W_MAX-1)) {
		printf("Too many sharks and fish.  Maximum total is %d.\n",
		  (L_MAX-1)*(W_MAX-1));
		hmsg();
		exit(1);
	}
}

hmsg()
{
	printf("Use option '-h' for help message.\n");
}

