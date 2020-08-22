/*
Copyright (c) 1979  Mike Gancarz
Permission is hereby granted to freely distribute, copy, modify and otherwise
use this software for nonprofit purposes as long as this copyright appears
in the file and remains intact.
*/

#include <stdio.h>
#include <signal.h>
#ifdef __STDC__
#include <termios.h>
#else
#include <sgtty.h>
#endif
/*
#define DEBUG 1	
*/

#define NAMSIZ 20	/* number of characters in name */
#define NROUNDS 15	/* number of rounds in the fight */
#define LINE 80
#define YES 1
#define NO 0
#define HARD 1		/* severity of punch */
#define SOFT 0		/*    "     "    "   */
#define NOKILL -1	/* killer instinct not in effect */
#define NOKD 0		/* no knockdown */
#define OFFNAME		bx[rt[offense].bxptr].lname
#define DEFNAME		bx[rt[flip(offense)].bxptr].lname
#define BX0NAM		bx[rt[0].bxptr].lname
#define BX1NAM		bx[rt[1].bxptr].lname
#define CUTNAM		bx[rt[i].bxptr].lname
#define RTOFF		rt[offense]
#define RTDEF		rt[flip(offense)]
#define BXOFF		bx[rt[offense].bxptr]
#define BXDEF		bx[rt[flip(offense)].bxptr]
#define BXI		bx[rt[i].bxptr]
#define J1WIN		(rt[0].cj1scr > rt[1].cj1scr ? 0 : 1)
#define J2WIN		(rt[0].cj2scr > rt[1].cj2scr ? 0 : 1)
#define REFWIN		(rt[0].crscr > rt[1].crscr ? 0 : 1)
#define J1WSCR		rt[(rt[0].cj1scr > rt[1].cj1scr ? 0 : 1)].cj1scr
#define J2WSCR		rt[(rt[0].cj2scr > rt[1].cj2scr ? 0 : 1)].cj2scr
#define REFWSCR		rt[(rt[0].crscr > rt[1].crscr ? 0 : 1)].crscr
#define J1LSCR		rt[(rt[0].cj1scr > rt[1].cj1scr ? 1 : 0)].cj1scr
#define J2LSCR		rt[(rt[0].cj2scr > rt[1].cj2scr ? 1 : 0)].cj2scr
#define REFLSCR		rt[(rt[0].crscr > rt[1].crscr ? 1 : 0)].crscr
#define CARD		act[ccard]
#define LCARD		act[lcard]
#define toupper(c)	(islower(c) ? ((c) - ('a' - 'A')) : (c))
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define iswhite(c)	((c) <= ' ' || 0177 <= (c)) /* ASCII ONLY */
#define rnd80		((rand() % 80) + 1)
#define flip(n)		(n ? 0 : 1)

typedef char byte;
