/*
 * save and restore routines
 *
 * @(#)save.c	6.2 (Berkeley) 5/28/84
 */

#include <curses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "rogue.h"

typedef struct stat STAT;

extern char version[], encstr[];
extern char end[];
extern int end_win;

extern char *getenv();
extern char *sbrk();
extern char *strcpy();
extern gettmode();
extern setterm();
extern tstp();

STAT sbuf;

/*
 * save_game:
 *	Implement the "save game" command
 */
save_game()
{
	register FILE *savef;
	register int c;
	char buf[MAXSTR];

	/*
	 * get file name
	 */
	mpos = 0;
over:
	if (file_name[0] != '\0')
	{
		for (;;)
		{
			msg("Save file (%s)? ", file_name);
			c = getchar();
			mpos = 0;
			if (c == ESCAPE)
			{
				msg("");
				return FALSE;
			}
			else if (c == 'n' || c == 'N' || c == 'y' || c == 'Y')
				break;
			else
				msg("Please answer Y or N");
		}
		if (c == 'y' || c == 'Y')
		{
			strcpy(buf, file_name);
			goto gotfile;
		}
	}

	do
	{
		mpos = 0;
		msg("File name: ");
		buf[0] = '\0';
		if (get_str(buf, stdscr) == QUIT)
		{
quit:
			msg("");
			return FALSE;
		}
		mpos = 0;
gotfile:
		/*
		 * test to see if the file exists
		 */
		if (stat(buf, &sbuf) >= 0)
		{
			for (;;)
			{
				msg("File exists.  Do you wish to overwrite it?");
				mpos = 0;
				if ((c = readchar()) == ESCAPE)
					goto quit;
				if (c == 'y' || c == 'Y')
					break;
				else if (c == 'n' || c == 'N')
					goto over;
				else
					msg("Please answer Y or N");
			}
			msg("File name: %s", buf);
		}
		strcpy(file_name, buf);
		if ((savef = fopen(file_name, "w")) == NULL)
			msg("Cannot open");
	} while (savef == NULL);

	/*
	 * write out encrpyted file (after a stat)
	 * The fwrite is to force allocation of the buffer before the write
	 */
	save_file(savef);
	return TRUE;
}

/*
 * auto_save:
 *	Automatically save a file.  This is used if a HUP signal is
 *	recieved
 */
auto_save()
{
	register FILE *savef;
	register int i;

	for (i = 0; i < NSIG; i++)
		signal(i, SIG_IGN);
	if (file_name[0] != '\0' && (savef = fopen(file_name, "w")) != NULL)
		save_file(savef);
	endwin();
	exit(1);
}

/*
 * save_file:
 *	Write the saved game on the file
 */
save_file(savef)
register FILE *savef;
{
	/*
	 * close any open score file
	 */
	close(fd);
	move(LINES-1, 0);
	refresh();
	fstat(fileno(savef), &sbuf);
	/*
	 * DO NOT DELETE.  This forces stdio to allocate the output buffer
	 * so that malloc doesn't get confused on restart
	 */
	fwrite("junk", 1, 5, savef);

	fseek(savef, 0L, 0);
	end_win = TRUE;
	encwrite(version, (unsigned)(end-version), savef);
	fclose(savef);
}

/*
 * restore:
 *	Restore a saved game from a file with elaborate checks for file
 *	integrity from cheaters
 */
restore(file, envp)
register char *file;
char **envp;
{
	register int inf;
	register bool syml;
	register char *sp;
	extern char **environ;
	char buf[MAXSTR];
	STAT sbuf2;

	if (strcmp(file, "-r") == 0)
		file = file_name;

#ifdef SIGTSTP
	/*
	 * If a process can be suspended, this code wouldn't work
	 */
# ifdef SIG_HOLD
	signal(SIGTSTP, SIG_HOLD);
# else
	signal(SIGTSTP, SIG_IGN);
# endif
#endif

	if ((inf = open(file, 0)) < 0)
	{
		printf("Cannot open %s\n", file);
		return FALSE;
	}
	fstat(inf, &sbuf2);
	syml = symlink(file);
	if (
#ifdef WIZARD
		!wizard &&
#endif
		unlink(file) < 0)
	{
		printf("Cannot unlink file\n");
		return FALSE;
	}

	fflush(stdout);
	encread(buf, (unsigned)(strlen(version)+1), inf);
	if (strcmp(buf, version) != 0)
	{
		printf("Saved game is out of date.\n");
		return FALSE;
	}

	fflush(stdout);
	lseek(inf, 0L, 0);
	encread(version, (unsigned)(end-version), inf);
	/*
	 * we do not close the file so that we will have a hold of the
	 * inode for as long as possible
	 */

#ifdef WIZARD
	if (!wizard)
#endif
		if (sbuf2.st_ino != sbuf.st_ino || sbuf2.st_dev != sbuf.st_dev)
		{
			printf("Saved game is not in the same file.\n");
			return FALSE;
		}
		else if (sbuf2.st_ctime - sbuf.st_ctime > 15)
		{
			printf("File has been touched, so this score won't be recorded\n");
			noscore = TRUE;
		}
	mpos = 0;
	mvprintw(0, 0, "%s: %s", file, ctime(&sbuf2.st_mtime));

	/*
	 * defeat multiple restarting from the same place
	 */
#ifdef WIZARD
	if (!wizard)
#endif
		if (sbuf2.st_nlink != 1 || syml)
		{
			printf("Cannot restore from a linked file\n");
			return FALSE;
		}

	if (pstats.s_hpt <= 0)
	{
		printf("\"He's dead, Jim\"\n");
		return FALSE;
	}
#ifdef SIGTSTP
	signal(SIGTSTP, tstp);
#endif

	environ = envp;
	gettmode();
	if ((sp = getenv("TERM")) == NULL)
		sp = Def_term;
	setterm(sp);
	strcpy(file_name, file);
	setup();
	if (del_obj != NULL)
	{
		inpack--;
		if (del_obj->o_count > 1)
			del_obj->o_count--;
		else
			detach(pack, del_obj);
	}
	clearok(curscr, TRUE);
	srand(getpid());
	msg("File name: %s", file);
	playit();
	/*NOTREACHED*/
}

/*
 * encwrite:
 *	Perform an encrypted write
 */
encwrite(start, size, outf)
register char *start;
unsigned int size;
register FILE *outf;
{
	register char *ep;

	ep = encstr;

	while (size--)
	{
		putc(*start++ ^ *ep++, outf);
		if (*ep == '\0')
			ep = encstr;
	}
}

/*
 * encread:
 *	Perform an encrypted read
 */
encread(start, size, inf)
register char *start;
unsigned size;
register inf;
{
	register char *ep;
	register int read_size;

	if ((read_size = read(inf, start, (int)size)) == -1 || read_size == 0)
		return read_size;

	ep = encstr;

	while (size--)
	{
		*start++ ^= *ep++;
		if (*ep == '\0')
			ep = encstr;
	}
	return read_size;
}
