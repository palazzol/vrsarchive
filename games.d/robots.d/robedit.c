#define _POSIX_SOURCE
#include <signal.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdio.h>
#ifdef M_XENIX
#  include <sys/locking.h>
#  define flock(fd,flag)	locking(fd,flag,0L)
#  define LOCK_EX		LK_LOCK
#  define LOCK_UN		LK_UNLCK
#else
#    include <unistd.h>
#    define flock(fd,flag)	lockf(fd,flag,0L)
#    undef  LOCK_EX
#    define LOCK_EX		F_LOCK
#    undef  LOCK_UN
#    define LOCK_UN		F_ULOCK
#endif

#define MIN_ROBOTS	10
#define MAX_ROBOTS	500
#define MIN_VALUE	10
#define MAX_FREE	3

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
extern long lseek();
extern char *malloc();
extern char *strcpy();
extern char *strncpy();
extern long time();
extern void perror();
extern int _putchar();

typedef char bool;

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

char whoami[MAXSTR];

struct passwd	*pass;

struct robot {
	bool	alive;
	int	x;
	int	y;
};

main(argc,argv)
int argc;
char *argv[];
{	static char buf[MAXSTR];
	long score;
	int level;

	(void) sprintf(buf,"for this %s",TEMP_NAME);
	while (fscanf(stdin, "%ld %d %s\n", &score, &level, whoami) == 3) {
		pass = getpwnam(whoami);
		record_score(1, score, level, TMP_FILE, TEMP_DAYS);
		record_score(1, score, level, HOF_FILE, MAXINT);
	}
	return(0);
}

record_score(eaten, score, level, fname,max_days)
	bool eaten;
	char *fname;
	long score;
	int level, max_days;
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
			do_score(eaten,fd,score,level,max_days);
			flock(fd,LOCK_UN);
		}
		(void) close(fd);
	}
	(void) signal(SIGINT,action);
}

do_score(eaten,fd,score,level,max_days)
	bool eaten;
	long score;
	int fd, level, max_days;
{
	register struct scorefile *position;
	register struct scorefile *oldest, *remove, *sfile, *eof;
	int uid;
	long this_day, x, limit;

	this_day = max_days ? time((long *)0)/SECSPERDAY : 0;
	limit = this_day-max_days;
	sfile = (struct scorefile *)(malloc(FILE_SIZE));
	eof = &sfile[NUMSCORES];
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
			position->s_level = level+1;
			position->s_uid = uid;
			position->s_days = this_day;
			(void) lseek(fd,0L,0);
			(void) write(fd,(char *)sfile,FILE_SIZE);
			(void) close(fd);
		}
	}
}
