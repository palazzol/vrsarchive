#ifndef lint
static char *sccsid = "@(#)init.c	1.2 11/28/84";
#endif

#include "externs.h"
#include <pwd.h>

initialize(startup)
	char startup;
{
	register struct objs *p;
	int die();

	puts("Version 4.2, fall 1984.");
	puts("First Adventure game written by His Lordship, the honorable");
	puts("Admiral D.W. Riggle\n");
	srand(getpid());
	getutmp(uname);
	wordinit();
	initrms();
	if (startup) {
		location = dayfile;
		direction = NORTH;
		gtime = 0;
		snooze = CYCLE * 1.5;
		position = 22;
		setbit(wear, PAJAMAS);
		fuel = TANKFULL;
		torps = TORPEDOES;
		for (p = dayobjs; p->room != 0; p++)
			setbit(location[p->room].objects, p->obj);
	} else
		restore();
	wiz = wizard(uname);
	signal(SIGINT, die);
}

getutmp(uname)
	char *uname;
{
	struct passwd *ptr, *getpwuid();

	ptr = getpwuid(getuid());
	strcpy(uname, ptr ? ptr->pw_name : "");
}

char *list[] = {	/* hereditary wizards */
	"riggle",
	"chris",
	"edward",
	"comay",
	"yee",
	"dmr",
	"ken",
	0
};

char *badguys[] = {
	"wnj",
	"root",
	"ted",
	0
};

wizard(uname)
	char *uname;
{
	char flag;

	if (flag = checkout(uname))
		printf("You are the Great wizard %s.\n", uname);
	return flag;
}

checkout(uname)
	register char *uname;
{
	register char **ptr;

	for (ptr = list; *ptr; ptr++)
		if (strcmp(*ptr, uname) == 0)
			return 1;
	for (ptr = badguys; *ptr; ptr++)
		if (strcmp(*ptr, uname) == 0) {
			printf("You are the Poor anti-wizard %s.  Good Luck!\n",
				uname);
			CUMBER = 3;
			WEIGHT = 9;	/* that'll get him! */
			clock = 10;
			setbit(location[7].objects, WOODSMAN);	/* viper room */
			setbit(location[20].objects, WOODSMAN);	/* laser " */
			setbit(location[13].objects, DARK);	/* amulet " */
			setbit(location[8].objects, ELF);	/* closet */
			return 0;	/* anything else, Chris? */
		}
	return 0;
}
