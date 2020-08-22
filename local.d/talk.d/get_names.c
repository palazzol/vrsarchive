/*
 *	@(#)get_names.c	1.3 12/3/84
*/
#include "talk.h"

extern char *getlogin();
extern char *ttyname();
extern char *strcat();
extern char *strcpy();
extern char *strchr();
extern char *strrchr();

/*
 *	This routine looks up which terminal the user is on.
*/
find_user()
{   union {
        struct utmp ubuf;
        char buf[1];
    } u;
    int fd;
    char utmp[SYS_SIZE+10];

    strcpy(utmp, other_system);
    strcat(utmp, "/etc/utmp");
    if ((fd = open(utmp, 0)) == -1) {
        perror(utmp);
        (void) exit(1);
    }
    while (read(fd, u.buf, sizeof u.ubuf) == sizeof(u.ubuf)) {
        if (strncmp(u.ubuf.ut_name, other_name, sizeof u.ubuf.ut_name) == 0) {
            (void) strcpy(other_tty, u.ubuf.ut_line);
            (void) close(fd);
            return;
        }
    }
    (void) close(fd);
    (void) fprintf(stderr,"Your party is not logged in\n");
    (void) exit(1);
}

/*
 * Determine the local and remote user, system, and tty.
*/
get_names(argc, argv)
int argc;
char *argv[];
{   char *p;
    FILE *fd;

    if (argc < 2 ) {
	(void) printf("Usage:  talk user[@system] [ttyname]\n");
	(void) exit(1);
    }
    if (!isatty(0)) {
	(void) printf("Standard input must be a tty, not a pipe or a file\n");
	(void) exit(1);
    }
    (void) strcpy(my_name,getlogin());
    /*
     *	Get the network node name (if any).  This name has the format //mumble
     *	where mumble is the system name.  It is used to form path names to
     *	remote file systems (to contact remote users).
    */
    fd = popen("net name", "r");
    if (fd == NULL)
        *my_system = '\0';		/* No network			*/
    else {
        (void) fgets(my_system,sizeof my_system, fd);
        my_system[strlen(my_system)-1] = '\0';
    }
    (void) strcpy(my_tty,strrchr(ttyname(0), '/') + 1);
    if (my_name == NULL) {
	(void) printf("You don't exist. Go away.\n");
	(void) exit(1);
    }
    if ((p = strchr(argv[1], '@')) == NULL) {
        (void) strcpy(other_name,argv[1]);
        (void) strcpy(other_system,my_system);
    } else {
        *p++ = '\0';
        (void) strcpy(other_name,argv[1]);
        (void) strcpy(other_system,"//");
        (void) strcat(other_system,p);
    }
    if (argc > 2)
	(void) strcpy(other_tty,argv[2]);	/* tty name is arg 2 */
    else
	find_user();
}
