static char *sccsid = "@(#)w.c	4.4 (Berkeley) 6/5/81";

/*
 * w - print system status (who and what)
 *
 * This program is similar to the systat command on Tenex/Tops 10/20
 * It needs read permission on /dev/mem, /dev/kmem, and /dev/swap.
*/

#include <sys/param.h>
#include <a.out.h>
#include <stdio.h>
#include <ctype.h>
#include <utmp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/var.h>

#define	DIV60(x)	((x+30)/60)	/* x/60 rounded			*/ 
#define	TTYEQ		(tty == pr[i].w_tty)
#define IGINT		(1+3*1)		/* Ignoring SIGINT and SIGQUIT	*/
#define NMAX		sizeof(utmp.ut_name)
#define LMAX		sizeof(utmp.ut_line)
#define ARGWIDTH	33		/* Characters left on line	*/

struct pr {
	short	w_pid;			/* Process id			*/
	char	w_flag;			/* Process flags		*/
	short	w_size;			/* Process size			*/
	int	w_igintr;		/* INTR+3*QUIT, 0=die, 1=ign, 2=catch */
	time_t	w_time;			/* CPU time used - this process	*/
	time_t	w_ctime;		/* CPU time used - children	*/
	dev_t	w_tty;			/* Tty device of process	*/
	char	w_comm[DIRSIZ+1];	/* user.u_comm, null terminated	*/
} *pr;

struct	nlist nl[] = {
	{ "_proc" },
#define	X_PROC		0
	{ "_swplo" },
#define	X_SWPLO		1
	{ "_bootime" },
#define	X_BOOTIME	2
	{ "_v" },
#define	X_V		3
	{ "_gdt" },
#define	X_GDT		4
	{ 0 },
};
#define gdt	(nl[X_GDT].n_value)

int	debug;			/* True if -d flag: debugging output	*/
char	firstchar;		/* First char of name of prog (argv[0])	*/
int	header = 1;		/* True if -h flag: don't print heading	*/
int	lflag = 1;		/* True if -l flag: long style output	*/
int	login;			/* True if invoked as login shell	*/
char *	sel_user;		/* Login of particular user selected	*/

int	kmem;			/* Kernel memory			*/
int	mem;			/* Physical memory			*/
int	swap;			/* Swap space				*/
FILE	*ut;			/* Current logins file			*/

dev_t	tty;			/* Current terminal device		*/
char	doing[520];		/* Command associated with terminal	*/
int	idle;			/* Number of minutes terminal is idle	*/
time_t	jobtime;		/* Total cpu time visible this terminal	*/
time_t	proctime;		/* Cpu time of command in `doing'	*/

struct	proc *aproc;		/* Pointer to the proc structure	*/
time_t	bootime;		/* Time of last reboot			*/
struct	proc mproc;		/* Readpr() area for proc structure	*/
time_t	now;			/* The current time of day		*/
struct	tm *nowt;		/* Current time as time struct		*/
int	nproc;			/* Total proc slots this kernel		*/
int	np;			/* Number of processes currently active	*/
int	nusers;			/* Number of users logged in now	*/
daddr_t	swplo;			/* Base of swap space this kernel	*/
struct	user up;		/* Getu() area for U structure		*/
time_t	uptime;			/* Elapsed time since last reboot	*/
struct	utmp utmp;		/* Main() area for current login	*/
struct	var v;			/* Configurable parameters this kernel	*/

char *	calloc();
int	fread();
char *	strrchr();
struct	tm *localtime();

main(argc, argv)
	char **argv;
{
	int days, hrs, mins;
	register int i, j;
	char *cp;
	register int curpid, empty;
	char obuf[BUFSIZ];

	setbuf(stdout, obuf);
	login = (argv[0][0] == '-');
	cp = strrchr(argv[0], '/');
	firstchar = login ? argv[0][1] : (cp==0) ? argv[0][0] : cp[1];
	cp = argv[0];	/* for Usage */

	while (argc > 1) {
		if (argv[1][0] == '-') {
			for (i=1; argv[1][i]; i++) {
				switch(argv[1][i]) {

				case 'd':
					debug++;
					break;

				case 'h':
					header = 0;
					break;

				case 'l':
					lflag++;
					break;

				case 's':
					lflag = 0;
					break;

				case 'u':
				case 'w':
					firstchar = argv[1][i];
					break;

				default:
					printf("Bad flag %s\n", argv[1]);
					exit(1);
				}
			}
		} else {
			if (!isalnum(argv[1][0]) || argc > 2) {
				printf("Usage: %s [ -hlsuw ] [ user ]\n", cp);
				exit(1);
			} else
				sel_user = argv[1];
		}
		argc--; argv++;
	}

	if ((kmem = open("/dev/kmem", 0)) < 0) {
		fprintf(stderr, "No kmem\n");
		exit(1);
	}
	nlist("/xenix", nl);
	if (nl[0].n_type==0) {
		fprintf(stderr, "No namelist\n");
		exit(1);
	}

	if (firstchar != 'u')
		readpr();

	ut = fopen("/etc/utmp","r");
	if (header) {
		/* Print time of day */
		time(&now);
		nowt = localtime(&now);
		prtat(nowt);

		/*
		 * Print how long system has been up.
		 * (Found by looking for "bootime" in kernel)
		 */
		lseek(kmem, (long)nl[X_BOOTIME].n_value, 0);
		read(kmem, (char *)&bootime, sizeof (bootime));

		uptime = now - bootime;
		days = uptime / (60l*60*24);
		uptime %= (60l*60*24);
		hrs = uptime / (60*60);
		uptime %= (60*60);
		mins = DIV60(uptime);

		printf("  up");
		if (days > 0)
			printf(" %d day%s,", days, (days > 1) ? "s" : "");
		if (hrs > 0 && mins > 0) {
			printf(" %2d:%02d,", hrs, mins);
		} else {
			if (hrs > 0)
				printf(" %d hr%s,", hrs, hrs>1?"s":"");
			if (mins > 0)
				printf(" %d min%s,", mins, mins>1?"s":"");
		}

		/* Print number of users logged in to system */
		while (fread((char *)&utmp, sizeof(utmp), 1, ut)) {
			if (utmp.ut_name[0] != '\0')
				nusers++;
		}
		rewind(ut);
		printf("  %d users\n", nusers);
		if (firstchar == 'u')
			exit(0);

		/* Headers for rest of output */
		if (lflag)
			printf("USER     TTY       LOGIN@  IDLE   JCPU   PCPU  WHAT\n");
		else
			printf("USER    TTY  IDLE  WHAT\n");
		fflush(stdout);
	}


	for (;;) {	/* for each entry in utmp */
		if (fread((char *)&utmp, sizeof(utmp), 1, ut) == NULL) {
			fclose(ut);
			exit(0);
		}
		if (utmp.ut_name[0] == '\0')
			continue;	/* that tty is free */
		if (sel_user && strncmp(utmp.ut_name, sel_user, NMAX) != 0)
			continue;	/* we wanted only somebody else */

		gettty();
		jobtime = 0;
		proctime = 0;
		strcpy(doing, "-");	/* default act: normally never prints */
		empty = 1;
		curpid = -1;
		idle = findidle();
		for (i = 0; i < np; i++) { /* for each process on this tty */
			if (!(TTYEQ))
				continue;
			jobtime += pr[i].w_time + pr[i].w_ctime;
			proctime += pr[i].w_time;
			if (debug) {
				printf("\t\t%d\t%s", pr[i].w_pid, pr[i].w_comm);
				if ((j=pr[i].w_igintr) > 0)
					if (j==IGINT)
						printf(" &");
					else
						printf(" & %d %d", j%3, j/3);
				printf("\n");
			}
			if (empty && pr[i].w_igintr!=IGINT) {
				empty = 0;
				curpid = -1;
			}
			if (pr[i].w_pid>curpid
			&& (pr[i].w_igintr!=IGINT || empty)) {
				curpid = pr[i].w_pid;
				strcpy(doing, pr[i].w_comm);
			}
		}
		putline();
	}
	if (login)
	  sleep(2);
}

/* figure out the major/minor device # pair for this tty */
gettty()
{
	char ttybuf[20];
	struct stat statbuf;

	ttybuf[0] = 0;
	strcpy(ttybuf, "/dev/");
	strcat(ttybuf, utmp.ut_line);
	stat(ttybuf, &statbuf);
	tty = statbuf.st_rdev;
}

/*
 * putline: print out the accumulated line of info about one user.
 */
putline()
{
	/* print login name of the user */
	printf("%-*.*s ", NMAX, NMAX, utmp.ut_name);

	/* print tty user is on */
	if (lflag)
		/* long form: all (up to) LMAX chars */
		printf("%-*.*s", LMAX, LMAX, utmp.ut_line);
	else {
		/* short form: 2 chars, skipping 'tty' if there */
		if (utmp.ut_line[0] == 't'
		&& utmp.ut_line[1]  == 't'
		&& utmp.ut_line[2]  == 'y')
			printf("%-2.2s", &utmp.ut_line[3]);
		else
			printf("%-2.2s", utmp.ut_line);
	}

	if (lflag)
		/* print when the user logged in */
		prtat(localtime(&utmp.ut_time));

	/* print idle time */
	prttime((time_t)idle," ");

	if (lflag) {
		/* print CPU time for all processes & children */
		prttime(DIV60(jobtime)," ");
		/* print cpu time for interesting process */
		prttime(DIV60(proctime)," ");
	}

	/* what user is doing, either command tail or args */
	printf(" %-.32s\n",doing);
	fflush(stdout);
}

/* find & return number of minutes current tty has been idle */
findidle()
{
	struct stat stbuf;
	long lastaction, diff;
	char ttyname[20];

	strcpy(ttyname, "/dev/");
	strncat(ttyname, utmp.ut_line, LMAX);
	stat(ttyname, &stbuf);
	time(&now);
	lastaction = stbuf.st_atime;
	diff = now - lastaction;
	diff = DIV60(diff);
	if (diff < 0) diff = 0;
	return(diff);
}

/*
 * prttime prints a time in hours and minutes.
 * The character string tail is printed at the end, obvious
 * strings to pass are "", " ", or "am".
 */
prttime(tim, tail)
	time_t tim;
	char *tail;
{
	register int didhrs = 0;

	if (tim >= 60) {
		printf("%3ld:", tim/60);
		didhrs++;
	} else {
		printf("    ");
	}
	tim %= 60;
	if (tim > 0 || didhrs) {
		printf(didhrs&&tim<10 ? "%02ld" : "%2ld", tim);
	} else {
		printf("  ");
	}
	printf("%s", tail);
}

/* prtat prints a 12 hour time given a pointer to a time of day */
prtat(p)
	struct tm *p;
{
	register int t, pm;

	t = p -> tm_hour;
	if (pm = (t > 11))
		t -= 12;
	if (t == 0)
		t = 12;
	prttime((time_t)(t*60 + p->tm_min), (pm ? "pm" : "am"));
}

struct user *getu()
{ int file;
  long addr;

  if (mproc.p_flag & SLOAD) {
    addr = mltoa((long) mproc.p_addr.p_caddr);
    file = mem;
  } else {
    addr = (swplo + mproc.p_addr.p_daddr) << BSHIFT;
    file = swap;
  }
  addr += OFFUSRPG;	/* Skip kernel mode stack */
  lseek(file, addr, 0);
  if (read(file, (char *)&up, sizeof(up)) != sizeof(up))
    return(NULL);
  return(&up);
}

/*
 * readpr finds and reads in the array pr, containing the interesting
 * parts of the proc and user tables for each live process.
 */
readpr()
{ int pn;

  if((mem = open("/dev/mem", 0)) < 0) {
    fprintf(stderr, "No mem\n");
    exit(1);
  }
  if ((swap = open("/dev/swap", 0)) < 0) {
    fprintf(stderr, "No drum\n");
    exit(1);
  }
  /*
   * Find base of swap
  */
  lseek(kmem, (long)nl[X_SWPLO].n_value, 0);
  read(kmem, (char *)&swplo, sizeof(swplo));
  /*
   * Get proc table size from v structure
  */
  lseek(kmem, (long)nl[X_V].n_value, 0);
  read(kmem, (char *)&v, sizeof v);
  nproc = v.v_proc;
  /*
   * Allocate for pr array
  */
  pr = (struct pr *)calloc((unsigned)nproc, sizeof (struct pr));
  np = 0;
  /*
   * Find proc table
  */
  aproc = (struct proc *)nl[X_PROC].n_value;
  /*
   * Collect data for each process
  */
  for (pn = 0; pn < nproc; pn++) {
    lseek(kmem, (long)(aproc + pn), 0);
    read(kmem, (char *)&mproc, sizeof mproc);
    /*
     * Decide if it's an interesting process
    */
    if ((mproc.p_stat == 0) || (mproc.p_pgrp == 0))
      continue;
    /*
     * Find & read in the user structure
    */
    if (getu() == NULL)
      continue;
    /*
     * Skip if no terminal
    */
    if (up.u_ttyp == NULL)
      continue;
    /*
     * Save the interesting parts
    */
    pr[np].w_pid = mproc.p_pid;
    pr[np].w_flag = mproc.p_flag;
    pr[np].w_size = mstod(up.u_tsize+up.u_dsize+up.u_ssize);
    pr[np].w_igintr =   (up.u_signal[2] == (faddr_t)1)
                    + 2*(up.u_signal[2] >  (faddr_t)1)
                    + 3*(up.u_signal[3] == (faddr_t)1)
                    + 6*(up.u_signal[3] >  (faddr_t)1);
    pr[np].w_time = up.u_utime + up.u_stime;
    pr[np].w_ctime = up.u_cutime + up.u_cstime;
    pr[np].w_tty = up.u_ttyd;
    strncpy(pr[np].w_comm, up.u_comm,DIRSIZ);
    pr[np].w_comm[DIRSIZ] = '\0';
    np++;
  }
}
