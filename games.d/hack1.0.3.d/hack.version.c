/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.version.c - version 1.0.3 */
/* $Header: /home/Vince/cvs/games.d/hack1.0.3.d/hack.version.c,v 1.2 1987-06-13 22:24:05 root Exp $ */

#include	"date.h"

doversion(){
	pline("%s 1.0.3 - last edit %s.", (
#ifdef QUEST
		"Quest"
#else
		"Hack"
#endif
		), datestring);
	return(0);
}
