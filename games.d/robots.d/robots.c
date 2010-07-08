#define _POSIX_SOURCE
/*
 * Written by Allan R. Black, Strathclyde University.
 * This program may be freely distributed provided that
 * this comment is not removed and that
 * no profit is gained from such distribution.
*/

/*
 *	@(#)robots.c	1.7	3/21/84 A. R. Black
 *	%W%b	%G%	V. R. Slyngstad
*/

/* 1.7a modified by Stephen J. Muir at Lancaster University. */
/* 1.7b modified by Vincent R. Slyngstad for XENIX Release 3 */

#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>
#ifndef TIOCGLTC
#ifdef __STDC__
#include <termios.h>
#else
#include <sys/ttold.h>
#endif
#endif
#ifdef M_XENIX
#  include <sys/locking.h>
#  define flock(fd,flag)	locking(fd,flag,0L)
#  define LOCK_EX		LK_LOCK
#  define LOCK_UN		LK_UNLCK
#else
#  include <unistd.h>
#  define flock(fd,flag)	0	/*lockf(fd,flag,0L)*/
#  ifndef  LOCK_EX
#    define LOCK_EX		F_LOCK
#    define LOCK_UN		F_ULOCK
#  endif
#endif

#define MIN_ROBOTS	10
#define MAX_ROBOTS	500
#define MIN_VALUE	10
#define MAX_FREE	3

#define VERT	'|'
#define HORIZ	'-'
#define ROBOT	'='
#define SCRAP	'@'
#define ME	'I'
#define MUNCH	'*'
#define DOT	'.'

#define LEVEL		(level+1)

#define MSGPOS		39
#define RVPOS		51

#ifdef __STDC__
#define STR(x)		#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(GAMLIB) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMLIB)/x"
#endif
#define HOF_FILE	FILENM(robots_hof)
#define TMP_FILE	FILENM(robots_tmp)

#define NUMSCORES	20
#define NUMNAME		"Twenty"

#define TEMP_DAYS	7
#define TEMP_NAME	"Week"

#define MAXSTR		100
#define MAXINT		((int)(((unsigned)-1)>>1))

extern char *getlogin();
extern struct passwd *getpwnam();
#ifndef CYGWIN
extern long lseek();
#endif
extern char *malloc();
extern char *strcpy();
extern char *strncpy();
extern long time();
extern int _putchar();

struct scorefile {
	int	s_uid;
	long	s_score;
	char	s_name[MAXSTR];
	bool	s_eaten;
	int	s_level;
	long	s_days;
};

#define FILE_SIZE	((unsigned)NUMSCORES*sizeof(struct scorefile))

#define SECSPERDAY	86400

#define BEL	007
#define CTLH	010
#define CTLJ	012
#define CTLK	013
#define CTLL	014
#define CTLY	031
#define CTLU	025
#define CTLB	002
#define CTLN	016
#define CTLR	022

char msgbuf[1000];
char whoami[MAXSTR];

int	my_x, my_y;
int	new_x, new_y;

int	level = 0;
long	score = 0;
int	free_teleports = 0;
int	free_per_level = 1;
bool	dead = FALSE;
bool	last_stand;

int	count;
char	com, cmd_ch;
bool	running, adjacent, first_move, bad_move;
int	dots = 0;

int	robot_value = MIN_VALUE;
int	max_robots = MIN_ROBOTS;
int	RobotsAlive;
struct passwd	*pass;

struct robot {
	bool	alive;
	int	x;
	int	y;
} RobotList[MAX_ROBOTS];

long	seed;

#ifdef TIOCGLTC
struct ltchars	ltc;
char	dsusp;
#endif

SIG_T	interrupt();

char	*forbidden [] =
	{ "root",
	  "daemon",
	  "uucp",
	  "ingres",
	  0
	};

main(argc,argv)
	int argc;
	char *argv[];
{
	register char *x, **xx;

	setbuf(stdout, 0);
	(void) initscr();
	(void) crmode();
	(void) noecho();
	if(argc > 1) {
		if(argv[1][0] == '-') {
			switch(argv[1][1]) {
			case 's':
				quit(FALSE);
			}
		}
	}
	if ((x = getlogin ()) == 0 || (pass = getpwnam (x)) == 0)
	{ (void) printf ("Who the hell are you?\n");
	  (void) exit (1);
	}
	(void) strcpy(whoami,x);
	for (xx = forbidden; *xx; ++xx)
		if (strcmp (whoami, *xx) == 0)
		{ (void) printf ("only individuals may play robots\n");
		  (void) exit (1);
		}
	seed = time((long *)0)+pass->pw_uid;
	(void) signal(SIGQUIT,interrupt);
	(void) signal(SIGINT,interrupt);
#ifdef TIOCGLTC
	(void) ioctl(1,TIOCGLTC,&ltc);
	dsusp = ltc.t_dsuspc;
	ltc.t_dsuspc = ltc.t_suspc;
	(void) ioctl(1,TIOCSLTC,&ltc);
#endif
	for(;;) {
		count = 0;
		running = FALSE;
		adjacent = FALSE;
		last_stand = FALSE;
		if(rnd(free_per_level) < free_teleports) {
			free_per_level++;
			if(free_per_level > MAX_FREE) free_per_level = MAX_FREE;
		}
		free_teleports += free_per_level;
		leaveok(stdscr,FALSE);
		draw_screen();
		put_robots();
		do {
			my_x = rndx();
			my_y = rndy();
			move(my_y,my_x);
		} while(winch(stdscr) != ' ');
		addch(ME);
		for(;;) {
			scorer();
			if(RobotsAlive == 0) break;
			command();
			robots();
			if(dead) munch();
		}
		if (!level)
			(void) nice (2);
		(void) sprintf(msgbuf,"%d robots are now scrap heaps",
					max_robots);
		msg();
		leaveok(stdscr,FALSE);
		move(my_y,my_x);
		refresh();
		(void) readchar();
		level++;
	}
}

draw_screen()
{
	register int x, y;
	clear();
	for(y = 1; y < LINES-2; y++) {
		mvaddch(y,0,VERT);
		mvaddch(y,COLS-1,VERT);
	}
	for(x = 0; x < COLS; x++) {
		mvaddch(0,x,HORIZ);
		mvaddch(LINES-2,x,HORIZ);
	}
}

put_robots()
{
	register struct robot *r, *end;
	register int x, y;
	robot_value += level*5+rnd(level*10);
	max_robots += level*3+rnd(level*5);
	if(max_robots > MAX_ROBOTS) max_robots = MAX_ROBOTS;
	RobotsAlive = max_robots;
	end = &RobotList[max_robots];
	for(r = RobotList; r < end; r++) {
		for(;;) {
			x = rndx();
			y = rndy();
			move(y,x);
			if(winch(stdscr) == ' ') break;
		}
		r->x = x;
		r->y = y;
		r->alive = TRUE;
		addch(ROBOT);
	}
}

command()
{
	register int x, y;
retry:
	move(my_y,my_x);
	refresh();
	if(last_stand) return;
	bad_move = FALSE;
	if(!running) {
		cmd_ch = read_com();
		switch(cmd_ch) {
		case CTLH:
		case CTLJ:
		case CTLK:
		case CTLL:
		case CTLY:
		case CTLU:
		case CTLB:
		case CTLN:
			cmd_ch |= 0100;
			adjacent = TRUE;
		case 'H':
		case 'J':
		case 'K':
		case 'L':
		case 'Y':
		case 'U':
		case 'B':
		case 'N':
			cmd_ch |= 040;
			running = TRUE;
			first_move = TRUE;
		case 't':
		case 'T':
		case 's':
		case 'S':
		case 'm':
		case 'M':
		case '?':
		case 'd':
		case 'D':
		case CTLR:
			count = 0;
		}
	}
	switch(cmd_ch) {
	case '.':
		return;
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case 'y':
	case 'u':
	case 'b':
	case 'n':
		do_move(cmd_ch);
		break;
	case 't':
	case 'T':
	teleport:
		new_x = rndx();
		new_y = rndy();
		move(new_y,new_x);
		switch(winch(stdscr)) {
		case ROBOT:
		case SCRAP:
		case ME:
			goto teleport;
		}
		if(free_teleports > 0) {
			for(x = new_x-1; x <= new_x+1; x++) {
				for(y = new_y-1; y <= new_y+1; y++) {
					move(y,x);
					if(winch(stdscr) == ROBOT) goto teleport;
				}
			}
			free_teleports--;
		}
		break;
	case 's':
	case 'S':
		last_stand = TRUE;
		leaveok(stdscr,TRUE);
		return;
	case 'm':
	case 'M':
	case '?':
		good_moves();
		goto retry;
	case 'd':
	case 'D':
		if(dots < 2) {
			dots++;
			put_dots();
		} else {
			erase_dots();
			dots = 0;
		}
		goto retry;
	case 'q':
	case 'Q':
		quit(FALSE);
	case CTLR:
		clearok(curscr,TRUE);
		(void) wrefresh(curscr);
		goto retry;
	default:
		bad_move = TRUE;
	}
	if(bad_move) {
		if(running) {
			if(first_move) (void) putchar(BEL);
			running = FALSE;
			adjacent = FALSE;
			first_move = FALSE;
		} else {
			(void) putchar(BEL);
		}
		refresh();
		count = 0;
		goto retry;
	}
	first_move = FALSE;
	mvaddch(my_y,my_x,' ');
	my_x = new_x;
	my_y = new_y;
	move(my_y,my_x);
	if(winch(stdscr) == ROBOT) munch();
	mvaddch(my_y,my_x,ME);
	refresh();
}

read_com()
{
	if(count == 0) {
		if (dots)
		{ put_dots ();
		  move (my_y, my_x);
		  refresh ();
		}
		if(isdigit(com = readchar())) {
			count = com-'0';
			while(isdigit(com = readchar())) count = count*10+com-'0';
		}
		if (dots)
			erase_dots ();
	}
	if(count > 0) count--;
	return(com);
}

readchar()
{
	static char buf[1];
	while(read(0,buf,sizeof buf) <= 0);
	return(buf[0]);
}

do_move(dir)
	char dir;
{
	register int x, y;
	new_x = my_x+xinc(dir);
	new_y = my_y+yinc(dir);
	move(new_y,new_x);
	switch(winch(stdscr)) {
	case VERT:
	case HORIZ:
	case SCRAP:
		bad_move = TRUE;
	}
	if(adjacent & !first_move) {
		for(x = new_x-1; x <= new_x+1; x++) {
			for(y = new_y-1; y <= new_y+1; y++) {
				move(y,x);
				switch(winch(stdscr)) {
				case ROBOT:
				case SCRAP:
					bad_move = TRUE;
					return;
				}
			}
		}
	}
}

put_dots()
{
	register int x, y;
	for(x = my_x-dots; x <= my_x+dots; x++) {
		if (x < 1 || x > COLS - 2)
			continue;
		for(y = my_y-dots; y <= my_y+dots; y++) {
			if (y < 1 || y > LINES - 3)
				continue;
			move(y,x);
			if(winch(stdscr) == ' ') addch(DOT);
		}
	}
}

erase_dots()
{
	register int x, y;
	for(x = my_x-dots; x <= my_x+dots; x++) {
		if (x < 1 || x > COLS - 2)
			continue;
		for(y = my_y-dots; y <= my_y+dots; y++) {
			if (y < 1 || y > LINES - 3)
				continue;
			move(y,x);
			if(winch(stdscr) == DOT) addch(' ');
		}
	}
}

good_moves()
{
	register int x, y;
	register int test_x, test_y;
	register char *m, *a;
	static char moves[] = "hjklyubn.";
	static char ans[sizeof moves];
	a = ans;
	for(m = moves; *m; m++) {
		test_x = my_x+xinc(*m);
		test_y = my_y+yinc(*m);
		move(test_y,test_x);
		switch(winch(stdscr)) {
		case ' ':
		case ME:
			for(x = test_x-1; x <= test_x+1; x++) {
				for(y = test_y-1; y <= test_y+1; y++) {
					move(y,x);
					if(winch(stdscr) == ROBOT) goto bad;
				}
			}
			*a++ = *m;
		}
	bad:;
	}
	*a = 0;
	if(ans[0]) {
		a = ans;
	} else {
		a = "Forget it!";
	}
	(void) mvprintw(LINES-1,MSGPOS,"%*.*-s",RVPOS-MSGPOS,RVPOS-MSGPOS,a);
}

xinc(dir)
	char dir;
{
	switch(dir) {
	case 'h':
	case 'y':
	case 'b':
		return(-1);
	case 'l':
	case 'u':
	case 'n':
		return(1);
	case 'j':
	case 'k':
	default:
		return(0);
	}
}

yinc(dir)
	char dir;
{
	switch(dir) {
	case 'k':
	case 'y':
	case 'u':
		return(-1);
	case 'j':
	case 'b':
	case 'n':
		return(1);
	case 'h':
	case 'l':
	default:
		return(0);
	}
}

robots()
{
	register struct robot *r, *end, *find;
	end = &RobotList[max_robots];
	for(r = RobotList; r < end; r++) {
		if(r->alive) {
			mvaddch(r->y,r->x,' ');
		}
	}
	for(r = RobotList; r < end; r++) {
		if(r->alive) {
			r->x += sign(my_x-r->x);
			r->y += sign(my_y-r->y);
			move(r->y,r->x);
			switch(winch(stdscr)) {
			case ME:
				addch(MUNCH);
				dead = TRUE;
				break;
			case SCRAP:
				r->alive = FALSE;
				score += robot_value;
				RobotsAlive--;
				break;
			case ROBOT:
				for(find = RobotList; find < r; find++) {
					if(find->alive) {
						if(r->x == find->x && r->y == find->y) {
							find->alive = FALSE;
							break;
						}
					}
				}
				r->alive = FALSE;
				addch(SCRAP);
				score += robot_value*2;
				RobotsAlive -= 2;
				break;
			case MUNCH:
				break;
			default:
				addch(ROBOT);
			}
		}
	}
}

munch()
{
	scorer();
	(void) sprintf(msgbuf,"MUNCH! You're robot food");
	msg();
	leaveok(stdscr,FALSE);
	mvaddch(my_y,my_x,MUNCH);
	move(my_y,my_x);
	refresh();
	(void) readchar();
	quit(TRUE);
}

quit(eaten)
	bool eaten;
{
	move(LINES-1,0);
	refresh();
	scoring(eaten);
	endwin();
	(void) putchar('\n');
#ifdef TIOCGLTC
	ltc.t_dsuspc = dsusp;
	(void) ioctl(1,TIOCSLTC,&ltc);
#endif
	(void) exit(0);
}

scoring(eaten)
	bool eaten;
{
	static char buf[MAXSTR];
	(void) sprintf(buf,"for this %s",TEMP_NAME);
	record_score(eaten,TMP_FILE,TEMP_DAYS,buf);
	(void) printw("[Press return to continue]");
	(void) refresh();
	(void) readchar();
	record_score(eaten,HOF_FILE,MAXINT,"of All Time");
	(void) refresh();
}

record_score(eaten,fname,max_days,type_str)
	bool eaten;
	char *fname;
	int max_days;
	char *type_str;
{
	int fd;
	void (*action)();
	action = signal(SIGINT,SIG_IGN);
	if((fd = open(fname,2)) < 0) {
		perror(fname);
	} else {
		if(flock(fd,LOCK_EX) < 0) {
			perror(fname);
		} else {
			do_score(eaten,fd,max_days,type_str);
			flock(fd,LOCK_UN);
		}
		(void) close(fd);
	}
	(void) signal(SIGINT,action);
}

do_score(eaten,fd,max_days,type_str)
	bool eaten;
	int fd, max_days;
	char *type_str;
{
	register struct scorefile *position;
	register struct scorefile *remove, *sfile, *eof;
	int uid;
	struct scorefile *oldest, *this;
	long this_day, x, limit;

	this_day = max_days ? time((long *)0)/SECSPERDAY : 0;
	limit = this_day-max_days;
	sfile = (struct scorefile *)(malloc(FILE_SIZE));
	eof = &sfile[NUMSCORES];
	this = 0;
	for(position = sfile; position < eof; position++) {
		position->s_score = 0;
		position->s_days = 0;
	}
	(void) read(fd,(char *)sfile,FILE_SIZE);
	remove = 0;
	if(score > 0) {
		uid = pass->pw_uid;
		oldest = 0;
		x = limit;
		for(position = eof-1; position >= sfile; position--) {
			if(position->s_days < x) {
				x = position->s_days;
				oldest = position;
			}
		}
		position = 0;
		for(remove = sfile; remove < eof; remove++) {
			if ((position == 0) && (score > remove->s_score))
				position = remove;
#ifndef ALLSCORES
			if (remove->s_uid == uid)
			{ if (remove->s_days < limit)
				oldest = remove;
			  else
			 	break;
			}
#endif
		}
		if(remove < eof) {
			if(position == 0 && remove->s_days < limit) position = remove;
		} else if(oldest) {
			remove = oldest;
			if(position == 0) {
				position = eof-1;
			} else if(remove < position) {
				position--;
			}
		} else if(position) {
			remove = eof-1;
		}
		if(position) {
			if(remove < position) {
				while(remove < position) {
					*remove = *(remove+1);
					remove++;
				}
			} else {
				while(remove > position) {
					*remove = *(remove-1);
					remove--;
				}
			}
			position->s_score = score;
			(void) strncpy(position->s_name,whoami,MAXSTR);
			position->s_eaten = eaten;
			position->s_level = LEVEL;
			position->s_uid = uid;
			position->s_days = this_day;
			this = position;
			(void) lseek(fd,0L,0);
			(void) write(fd,(char *)sfile,FILE_SIZE);
			(void) close(fd);
		}
	}
	clear();
	(void) printw(
#ifdef ALLSCORES
		"\nTop %s Scores %s:\n",
#else
		"\nTop %s Robotists %s:\n",
#endif
		NUMNAME,
		type_str
	);
	(void) printw("Rank   Score    Name\n");
	count = 0;
	for(position = sfile; position < eof; position++) {
		if(position->s_score == 0) break;
		if(position == this)
			(void) standout();
		if (position->s_days >= limit)
		{ (void) printw ("%-6d %-8ld %s: %s on level %d.",
			  ++count,
			  position->s_score,
			  position->s_name,
			  position->s_eaten ? "eaten" : "chickened out",
			  position->s_level
			 );
#ifdef OLDSCORES
		} else
		{ (void) printw ("*OLD*  %-8ld %s: %s on level %d.",
			  position->s_score,
			  position->s_name,
			  position->s_eaten ? "eaten" : "chickened out",
			  position->s_level
			 );
#endif
		}
		if(position == this)
			(void) standend();
#ifndef OLDSCORES
		if (position->s_days >= limit)
#endif
			(void) addch ('\n');
	}
}

scorer()
{
	static char	infobuf [6];
	register int	x, y;
	if ((y = free_teleports-free_per_level) < 0)
		y = 0;
	x = free_teleports-y;
	(void) sprintf(infobuf,"%d+%d",x,y);
	if (y == 0)
		infobuf[1] = '\0';
	move(LINES-1,0);
	clrtoeol();
	(void) printw("<%s> level: %d    score: %ld",infobuf,LEVEL,score);
	(void) mvprintw(LINES-1,RVPOS,"robots: %d    value: %d",RobotsAlive,robot_value);
}

rndx()
{
	return(rnd(COLS-2)+1);
}

rndy()
{
	return(rnd(LINES-3)+1);
}

rnd(mod)
	int mod;
{
	if(mod <= 0) return(0);
	return((((seed = seed*11109+13849) >> 16) & 0xffff) % mod);
}

sign(x)
	register int x;
{
	if(x < 0) return(-1);
	return(x > 0);
}

msg()
{
	mvaddstr(LINES-1,MSGPOS,msgbuf);
	clrtoeol();
	refresh();
}

SIG_T
interrupt()
{
	quit(FALSE);
}
