#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/main.c,v 1.7 2002-11-22 04:12:23 Vincent Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/main.c,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/main.c,v 1.7 2002-11-22 04:12:23 Vincent Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  1990/04/04 21:31:34  vrs
 * Changes for V.4 and ANSI C
 *
 * Version 1.5  87/12/25  21:43:59  vrs
 * Remove tokens after preprocessor directives
 * 
 * Version 1.4  87/12/25  20:50:57  vrs
 * Check in 4.0 version from the net
 * 
 * Revision 1.1  87/10/09  11:08:01  11:08:01  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: main.c
 *
 * Originally written (in HP-2000 BASIC) by
 *	William K. Char, Perry Lee, and Dan Gee
 *
 * Rewritten in C by
 *	Dave Pare (sdcsvax!sdamos!mr-frog)
 * 		  (mr-frog@amos.ling.ucsd.edu)
 *		and
 *	Christopher Williams (ucbvax!ucbmerlin!williams)
 *			     (williams@merlin.berkeley.edu)
 *
 * Corrected, Completed, and Enhanced by
 *	Jeff Okamoto	(hpccc!okamoto)
 *			(hpccc!okamoto@hplabs.hp.com)
 *	Peter Yee	(ucbvax!yee)
 *			(yee@ucbarpa.berkeley.edu)
 *	Matt Dillon	(ucbvax!dillon)
 *			(dillon@ucbvax.berkeley.edu)
 *	Dave Sharnoff	(ucbvax!ucbcory!muir)
 *			(muir@cogsci.berkeley.edu)
 *	Joel Duisman
 *	    and
 *	Roger J. Noe    (uniq!rjnoe)
 *			(ihnp4!uniq!rjnoe@berkeley.edu)
 *
 * Main Loop
 *
 * main, alarmtrap, quitgame, buffering
 *
 */

#include "externs.h"
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#ifdef __STDC__
#include <termios.h>
#endif

static jmp_buf	jumpbuf;

SIG_T
quitgame(dummy)
{
	char answer[20];
	unsigned timeleft;

	timeleft = alarm(0);
	(void) signal(SIGINT, SIG_IGN);
	puts("\n\nDo you really wish to stop now?  Answer yes or no:");
	(void) Gets(answer, sizeof(answer));
	if(answer[0] == '\0' || answer[0] == 'y' || answer[0] == 'Y')
		exit(0);
	(void) signal(SIGINT, quitgame);
	if(timeleft)
		(void) alarm((unsigned)timeleft);
	return;
}

SIG_T
alarmtrap(sig)
int sig;
{
	register int i;

	if (sig) {
		puts("\n** TIME **");
		(void) signal(sig, alarmtrap);
#ifdef __STDC__
		tcflush(fileno(stdin), TCIFLUSH);
#else
		stdin->_cnt = 0;
#endif
	}
	for (i = 1; i <= shipnum; i++)
		shiplist[i]->strategy(shiplist[i]);
	if (!(is_dead(shiplist[0], S_DEAD)))
		printf("\n");
	(void) move_ships();
	(void) check_targets();
	(void) misc_timers();
	(void) disposition();
	longjmp(jumpbuf, 1);
}

main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
#ifdef __STDC__
	setbuf(stdin, 0);
#endif
	if (buffering(stdout) < 0)
		perror("cannot fstat stdout");
	(void) signal(SIGALRM, alarmtrap);
	(void) signal(SIGINT, quitgame);
	srandom(time(0));
	time_delay = DEFAULT_TIME;
	set_save();
	options = getenv("TREK73OPTS");
	if (options != NULL)
		parse_opts(options);
	get_comlineopts(argc, argv);
	if (restart && savefile[0] != '\0') {
		restore(savefile, envp);	/* Will not return */
		exit(1);
	}
	name_crew();
	init_ships();
	(void) mission();
	(void) alert();
	playit();
	/*NOTREACHED*/
}

/*
 * Main loop
 */
playit()
{
	struct cmd		*scancmd();
	register struct ship	*sp;
#ifndef PARSER
	char			buf1[30];
#endif /*PARSER*/
	struct cmd		*cp;
	int			loop;
	char			*ch;

	(void) setjmp(jumpbuf);
	sp = shiplist[0];
	if (!(is_dead(sp, S_DEAD))) {
next:
		for (loop = 0; loop < HIGHSHUTUP; loop++)
			shutup[loop] = 0;
		fflush(stdin);
		printf("\n%s: Code [1-%d] ", captain, high_command);
		fflush(stdout);
		(void) alarm((unsigned) time_delay);
#ifdef PARSER
		(void) Gets(Input, sizeof(Input));
		if (Input[0] != '\0') {
#else
		(void) Gets(buf1, sizeof(buf1));
		if (buf1[0] != '\0') {
#endif /*PARSER*/
			(void) alarm(0);
#ifdef PARSER
			Inptr = Input;
			parsed[0] = '\0';
			ch = Inptr;
			while (isspace(*ch))
				ch++;
			if (isalpha(*ch))
				yyparse();
			else
				strcpy(parsed, Input);
#endif /*PARSER*/
#ifdef PARSER
			cp = scancmd(parsed);
#else
			cp = scancmd(buf1);
#endif /*PARSER*/
			if (cp != NULL) {
				(*cp->routine)(sp);
				if (cp->turns == FREE)
					goto next;
			} else
				printf("\n%s: %s, I am unable to interpret your last utterance.\n", science, title);
		} else
			(void) alarm(0);
	}
	ch = ch;		/* LINT */
	alarmtrap(0);
	/* This point is never reached since alarmtrap() always concludes
	   with a longjmp() back to the setjmp() above the next: label */
	/*NOTREACHED*/
}


/* buffering: Determine whether or not stream is to be buffered.  If
   it's a character-special device, any buffering in effect will remain.
   If it's not a character-special device, then stream will be
   unbuffered.  There are many ways to decide what to do here.  One
   would have been to make it unbuffered if and only if
   !isatty(fileno(stream)).  This is usually implemented as a single
   ioctl() system call which returns true if the ioctl() succeeds, false
   if it fails.  But there are ways it could fail and still be a tty.
   Then there's also examination of stream->_flag.  UNIX is supposed to
   make any stream attached to a terminal line-buffered and all others
   fully buffered by default.  But sometimes even when isatty()
   succeeds, stream->_flag indicates _IOFBF, not _IOLBF.  And even if it
   is determined that the stream should be line buffered, setvbuf(3S)
   doesn't work right (in UNIX 5.2) to make it _IOLBF.  So about the
   only choice is to do a straightforward fstat() and ascertain
   definitely to what the stream is attached.  Then go with old reliable
   setbuf(stream, NULL) to make it _IONBF.  The whole reason this is
   being done is because the user may be using a pipefitting program to
   collect a "transcript" of a session (e.g. tee(1)), or redirecting to
   a regular file and then keeping a tail(1) going forever to actually
   play the game.  This assures that the output will keep pace with the
   execution with no sacrifice in efficiency for normal execution. [RJN]
*/

#include <sys/types.h>
#include <sys/stat.h>

int
buffering(stream)
FILE	*stream;
{
	struct stat	st;
	if (fstat(fileno(stream), &st) < 0)
		return -1;
	if ((st.st_mode & S_IFMT) != S_IFCHR)
		setbuf(stream, NULL);
	return 0;
}
