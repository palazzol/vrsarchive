/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.version.c - version 1.0.3 */
/* $Header: /home/Vince/cvs/games.d/hack1.0.3.d/hack.version.c,v 1.1 1986-09-02 19:42:17 root Exp $ */

#include	"date.h"

doversion(){
	pline("%s 1.0.3 - last edit %s.", (
#ifdef QUEST
		"Quest"
#else
		"Hack"
#endif QUEST
		), datestring);
	return(0);
}
