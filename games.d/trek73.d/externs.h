#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/externs.h,v 1.5 2002-11-22 04:12:22 Vincent Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/externs.h,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/externs.h,v 1.5 2002-11-22 04:12:22 Vincent Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  1987/12/25 21:54:02  vrs
 * SYSV -> SYS5 (use CONFIG)
 *
 * Version 1.3  87/12/25  20:52:02  vrs
 * Check in 4.0 version from the net
 * 
 * Revision 1.1  87/10/09  11:06:05  11:06:05  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: externs.h
 *
 * External declarations for TREK73
 *
 */

/* UNIX include files needed for most (if not all) of the C files */
#include <stdio.h>
#ifdef SYS5
#include <string.h>
#endif /*SYS5*/
#ifdef BSD
#include <strings.h>
#endif /*BSD*/
#include <math.h>
#ifdef __STDC__
#define round Round
#endif

/* UNIX extern declarations of functions used which do not
   return int plus any extern variable declarations    */
extern char	*getenv(), *malloc();
extern long	time();
extern unsigned	sleep(), alarm();
extern void	exit(), free(), perror();
#ifdef SYS5
extern void	srand();
#endif /*SYS5*/
extern char	*optarg;

/*
 * Now continue with declarations specific to TREK73
 */

#include "structs.h"

extern char	*Gets(), *vowelstr();
extern char	*sysname[S_NUMSYSTEMS];
extern char	*statmsg[S_NUMSYSTEMS + 1];
extern char	*feds[];
extern char	*options;

extern float	init_p_turn[MAXPHASERS][MAXPHASERS];
extern float	init_t_turn[MAXTUBES][MAXTUBES];
extern float	segment, timeperturn;
extern float	rectify(), bearing(), round();

extern char	home[256];
extern char	savefile[256];
extern char	captain[30];
extern char	class[3];
extern char	com[30];
extern char	com_delay[6];
extern char	empire[30];
extern char	engineer[30];
extern char	foeclass[3];
extern char	foename[30];
extern char	foerace[30];
extern char	foestype[30];
extern char	helmsman[30];
extern char	nav[30];
extern char	racename[20];
extern char	savefile[256];
extern char	science[30];
extern char	sex[20];
extern char	shipname[30];
extern char	shutup[HIGHSHUTUP];
extern char	slots[HIGHSLOT];
extern char	title[9];

extern int	parsit();
extern int	cmdarraysize;
extern int	corbomite;
extern int	defenseless;
extern int	enemynum;
extern int	global;
extern int	high_command;
extern int	reengaged;
extern int	restart;
extern int	shipnum;
extern int	silly;
extern int	surrender;
extern int	surrenderp;
extern int	teletype;
extern int	terse;
extern int	time_delay;
extern int	trace;
extern char	can_cloak;
extern double	e_bpv;

extern char	Input[BUFSIZ];
extern char	*Inptr;
extern char	parsed[BUFSIZ];

extern struct ship_stat	us;
extern struct ship_stat	them;

extern int 	(*strategies[])();
extern int	rangefind();

extern struct cmd		*scancmd(), cmds[];
extern struct race_info		aliens[MAXFOERACES];
extern struct damage		p_damage, a_damage;
extern struct list		*newitem(), head, *tail;
extern struct ship		*shiplist[10], *ship_name();
extern struct ship_stat		stats[];
