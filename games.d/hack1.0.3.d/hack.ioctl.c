/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.ioctl.c - version 1.0.2 */

/* This cannot be part of hack.tty.c (as it was earlier) since on some
   systems (e.g. MUNIX) the include files <termio.h> and <sgtty.h>
   define the same constants, and the C preprocessor complains. */
#include <stdio.h>
#include "config.h"
#ifndef SYS5
#include	<sgtty.h>
#else
#include	<sys/types.h>
#include	<termio.h>	/* also includes part of <sgtty.h> */
#ifndef TCGETA
#include	<sys/ioctl.h>
#endif
#ifdef __STDC__
#include <sys/ttold.h>
#endif
struct termio termio;
#endif
#ifdef TIOCGLTC
struct ltchars ltchars, ltchars0;
#endif

getioctls() {
#ifdef TIOCGLTC
	(void) ioctl(fileno(stdin), (int) TIOCGLTC, (char *) &ltchars);
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars0);
#else
	(void) ioctl(fileno(stdin), (int) TCGETA, &termio);
#endif
}

setioctls() {
#ifdef TIOCSLTC
	(void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars);
#else
	(void) ioctl(fileno(stdin), (int) TCSETA, &termio);
#endif
}

#ifdef SUSPEND		/* implies BSD */
dosuspend() {
#include	<signal.h>
#ifdef SIGTSTP
	if(signal(SIGTSTP, SIG_IGN) == SIG_DFL) {
		settty((char *) 0);
		(void) signal(SIGTSTP, SIG_DFL);
		(void) kill(0, SIGTSTP);
		gettty();
		setftty();
		docrt();
	} else {
		pline("I don't think your shell has job control.");
	}
#else
	pline("Sorry, it seems we have no SIGTSTP here. Try ! or S.");
#endif
	return(0);
}
#endif
