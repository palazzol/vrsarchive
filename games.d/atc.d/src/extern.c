/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

#include "include.h"

char		GAMES[] =	"Game_List",
		LOG[] =		"ATC_scores";

long		start_time;
int		clock, safe_planes, test_mode;

char		*File;

FILE		*filein, *fileout;

screen_t	screen, *sp = &screen;

LIST		air, ground;

DISPLACEMENT	displacement[MAXDIR] = {
		{  0, -1 },
		{  1, -1 },
		{  1,  0 },
		{  1,  1 },
		{  0,  1 },
		{ -1,  1 },
		{ -1,  0 },
		{ -1, -1 }
};
