/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

extern char		GAMES[], LOG[], *File;

extern long		start_time;
extern int		clock, safe_planes, test_mode;

extern FILE		*filein, *fileout;

extern SCREEN		screen, *sp;

extern LIST		air, ground;

extern DISPLACEMENT	displacement[MAXDIR];

extern PLANE		*findplane(), *newplane();

extern long		time();
