/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

#include "include.h"
#include <sgtty.h>

struct sgttyb tty_start, tty_new;
char erase_char, kill_char;

int interval;

main(ac, av)
	char	*av[];
{
	int			seed;
	int			f_usage = 0, f_list = 0, f_showscore = 0;
	int			f_printpath = 0;
	char			*File = NULL;
	char			*name, *ptr;
	extern int		update(), quit(), log_score();
	extern char		*default_game(), *okay_game();

	start_time = seed = time((long *)0);

	name = *av++;
	while (*av) {
#ifndef SAVEDASH
		if (**av == '-') 
			*++*av;
		else
			break;
#endif
		ptr = *av++;
		while (*ptr) {
			switch (*ptr) {
			case '?':
			case 'u':
				f_usage++;
				break;
			case 'l':
				f_list++;
				break;
			case 's':
			case 't':
				f_showscore++;
				break;
			case 'p':
				f_printpath++;
				break;
			case 'r':
				seed = atoi(*av);
				av++;
				break;
			case 'f':
			case 'g':
				File = *av;
				av++;
				break;
			default: 
				fprintf(stderr, "Unknown option '%c'\n", *ptr,
					name);
				f_usage++;
				break;
			}
			ptr++;
		}
	}
	srandom(seed);

	if (f_usage)
		fprintf(stderr, 
		    "Usage: %s -[u?lstp] [-[gf] game_name] [-r random seed]\n",
			name);
	if (f_showscore)
		log_score(1);
	if (f_list)
		list_games();
	if (f_printpath) {
		char	buf[100];

		strcpy(buf, SPECIAL_DIR);
		buf[strlen(buf) - 1] = '\0';
		puts(buf);
	}
		
	if (f_usage || f_showscore || f_list || f_printpath)
		exit(0);

	if (File == NULL)
		File = default_game();
	else
		File = okay_game(File);

	if (File == NULL || read_file(File) < 0)
		exit(1);

	init_gr();
	setup_screen(sp);

	addplane();

	signal(SIGINT, quit);
	signal(SIGQUIT, quit);
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif
#ifdef SIGSTOP
	signal(SIGSTOP, SIG_IGN);
#endif
	signal(SIGHUP, log_score);
	signal(SIGTERM, log_score);

	ioctl(fileno(stdin), TIOCGETP, &tty_start);
	erase_char = tty_start.sg_erase;
	kill_char = tty_start.sg_kill;
	bcopy(&tty_start, &tty_new, sizeof(tty_new));
	tty_new.sg_flags |= CBREAK;
	tty_new.sg_flags &= ~ECHO;
	ioctl(fileno(stdin), TIOCSETP, &tty_new);

	signal(SIGALRM, update);
	interval = 0;
	alarm(sp->update_secs);
	for (;;) {
		if (getcommand() != 1)
			planewin();
		else {
			alarm(0);
			update();
			interval = sp->update_secs;
			alarm(sp->update_secs);
		}
	}
}

read_file(s)
	char	*s;
{
	extern FILE	*yyin;
	int		retval;

	File = s;
	yyin = fopen(s, "r");
	if (yyin == NULL) {
		perror(s);
		return (-1);
	}
	retval = yyparse();
	fclose(yyin);

	if (retval != 0)
		return (-1);
	else
		return (0);
}

char	*
default_game()
{
	FILE		*fp;
	static char	File[256];
	char		line[256], games[256];

	strcpy(games, SPECIAL_DIR);
	strcat(games, GAMES);

	if ((fp = fopen(games, "r")) == NULL) {
		perror(games);
		return (NULL);
	}
	if (fgets(line, sizeof(line), fp) == NULL) {
		fprintf(stderr, "%s: no default game available\n", games);
		return (NULL);
	}
	fclose(fp);
	line[strlen(line) - 1] = '\0';
	strcpy(File, SPECIAL_DIR);
	strcat(File, line);
	return (File);
}

char	*
okay_game(s)
	char	*s;
{
	FILE		*fp;
	static char	File[256];
	char		*ret = NULL, line[256], games[256];

	strcpy(games, SPECIAL_DIR);
	strcat(games, GAMES);

	if ((fp = fopen(games, "r")) == NULL) {
		perror(games);
		return (NULL);
	}
	while (fgets(line, sizeof(line), fp) != NULL) {
		line[strlen(line) - 1] = '\0';
		if (strcmp(s, line) == 0) {
			strcpy(File, SPECIAL_DIR);
			strcat(File, line);
			ret = File;
			break;
		}
	}
	fclose(fp);
	if (ret == NULL) {
		test_mode = 1;
		ret = s;
		fprintf(stderr, "%s: %s: game not found\n", games, s);
		fprintf(stderr, "Your score will not be logged.\n");
		sleep(2);	/* give the guy time to read it */
	}
	return (ret);
}

list_games()
{
	FILE		*fp;
	char		line[256], games[256];
	int		num_games = 0;

	strcpy(games, SPECIAL_DIR);
	strcat(games, GAMES);

	if ((fp = fopen(games, "r")) == NULL) {
		perror(games);
		return (-1);
	}
	puts("available games:");
	while (fgets(line, sizeof(line), fp) != NULL) {
		printf("	%s", line);
		num_games++;
	}
	fclose(fp);
	if (num_games == 0) {
		fprintf(stderr, "%s: no games available\n", games);
		return (-1);
	}
	return (0);
}
