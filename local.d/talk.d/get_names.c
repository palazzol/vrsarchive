/*
 *	@(#)get_names.c	1.1 12/2/84
*/

#include "talk.h"

extern char *getlogin();
extern char *ttyname();
extern char *strcpy();
extern char *strrchr();

char my_name[NAME_SIZE];
char my_tty[TTY_SIZE];

char other_name[NAME_SIZE];
char other_tty[TTY_SIZE];

/*
 * Determine the local and remote user and tty.
*/
get_names(argc, argv)
int argc;
char *argv[];
{
    if (argc < 2 ) {
	(void) printf("Usage:  talk user [ttyname]\n");
	(void) exit(-1);
    }
    if (!isatty(0)) {
	(void) printf("Standard input must be a tty, not a pipe or a file\n");
	(void) exit(-1);
    }
    (void) strcpy(my_name,getlogin());
    (void) strcpy(my_tty,strrchr(ttyname(0), '/') + 1);
    if (my_name == NULL) {
	(void) printf("You don't exist. Go away.\n");
	(void) exit(-1);
    }
    (void) strcpy(other_name,argv[1]);
    if (argc > 2)
	(void) strcpy(other_tty,argv[2]);	/* tty name is arg 2 */
    else
	*other_tty = '\0';
}
