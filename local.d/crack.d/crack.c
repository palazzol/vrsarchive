#include <stdio.h>
#include <pwd.h>

extern FILE *fopen();
extern FILE *popen();

char *list[] = {
  0,				/* Filled in with user's login name	*/
  0,				/* Filled in with system name		*/
#include "primo.h"	/* These have worked before			*/
#ifdef TRYHARD
#include "crack.h"	/* These haven't worked yet			*/
#endif
  0
};

main(argc, argv)
int argc;
char *argv[];
{ register char **p;
  register char *cp;
  static char name[BUFSIZ];
  static char system[10];
  char *passwd;
  FILE *fd;

  if (argc == 1) {
    argv[1] = "/etc/passwd";	/* argv[1] was the arglist delimiter	*/
    fd = popen("uuname -l", "r");
    fgets(system, sizeof(system), fd); /* Read system name		*/
    pclose(fd);			/* Wait for child process		*/
    list[1] = system;
  } else
    list[1] = argv[1];		/* Assume file is named after system	*/
  fd = fopen(argv[1], "r");
  if (fd == 0) {
    perror(argv[1]);
    exit(1);
  }
  while (fgets(name, sizeof(name), fd) != 0) {
    for (cp = name; *++cp != ':';) ;
    *cp = '\0';
    passwd = cp+1;
    while (*++cp != ':') ;
    *cp = '\0';
    printf("User %s has ", name);
    fflush(stdout);
    if (*passwd == '\0') {
      printf("no password\n");
      continue;
    }
    if (strlen(passwd) != 13) {
      printf("an invalid password\n");
      continue;
    }
    list[0] = name;		/* Try login name first			*/
    for (p = list; *p; p++) {
      if (strcmp(crypt(*p, passwd), passwd) == 0) {
        printf("password '%s'\n", *p);
	break;
      }
    }
    if (*p == 0) {
      printf("an unknown password\n");
      continue;
    }
  }
}
