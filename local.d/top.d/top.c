char *copyright = "Copyright (c) 1984, William LeFebvre";

/*
 *  Top users display for Berkeley Unix
 *  Version 1.4
 *
 *  This program may be freely redistributed to other Unix sites, but this
 *  comment MUST remain intact.
 *
 *  Copyright (c) 1984, William LeFebvre, Rice University
 *
 *  This program is designed to run on either Berkeley 4.1 or 4.2 Unix.
 *  Compile with the preprocessor variable "FOUR_TWO" set to get an
 *  executable that will run on Berkeley 4.2 Unix.
 */

#include <curses.h>
#include <stdio.h>
#include <pwd.h>
#include <nlist.h>
#include <signal.h>
#ifdef FOUR_TWO
#include <machine/pte.h>
#else
#include <sys/pte.h>
#endif
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/dk.h>
#include <sys/vm.h>

/* obvious file names */

#define VMUNIX	"/vmunix"
#define KMEM	"/dev/kmem"
#define MEM	"/dev/mem"

/*
 *  The number of users in /etc/passwd CANNOT be greater than Table_size!
 *  If you get the message "table overflow: too many users", then increase
 *  Table_size.  Since this is the size of a hash table, things will work
 *  best if it is a prime number that is about twice the number of users.
 */

#define Table_size	421

/* Number of lines of header information on the standard screen */
# define Header_lines	5

/* wish list for kernel symbols */

struct nlist nlst[] = {
    { "_avenrun" },
#define X_AVENRUN	0
    { "_ccpu" },
#define X_CCPU		1
    { "_cp_time" },
#define X_CP_TIME	2
    { "_hz" },
#define X_HZ		3
    { "_mpid" },
#define X_MPID		4
    { "_nproc" },
#define X_NPROC		5
    { "_proc" },
#define X_PROC		6
    { 0 },
};

/* useful externals */
extern int errno;
extern char *sys_errlist[];
extern int LINES;

/* signal handling routines */
int leave();
int onalrm();

/* file descriptors for memory devices */
int kmem;
int mem;

int nproc;
int mpid;

/* kernel "hz" variable -- clock rate */
long hz;

/* All this is to calculate the cpu state percentages */

long cp_time[CPUSTATES];
long cp_old[CPUSTATES];
long cp_change[CPUSTATES];
long total_change;
long cp_time_offset;
long mpid_offset;
long avenrun_offset;

double ccpu;
double logcpu;
double avenrun[3];

struct proc *proc;
struct proc *pbase;
struct proc *pp;
struct proc **pref;
struct proc **prefp;

struct user u;

/* Verbose process state names */

char *state_name[] = {
    "", "sleeping", "ABANDONED", "running", "starting", "zombie", "stopped"
};

/* process state names for the "STATE" column of the display */

char *state_abbrev[] = {
    "", "sleep", "WAIT", "run", "idl", "zomb", "stop"
};

/* cpu state names for percentages */

char *cpu_state[] = {
    "user", "nice", "system", "idle"
};

/* routines that don't return int */

struct passwd *getpwent();
char *username();
char *ctime();
char *rindex();

int proc_compar();
double log();
double exp();

main(argc, argv)

int  argc;
char *argv[];

{
    int i;
    int total_procs;
    int active_procs;
    int proc_brkdn[7];
    int bytes;
    int topn = Default_TOPN;
    int delay = Default_DELAY;
    int change;
    long cputime;
    long curr_time;
    long time();
    char do_cpu = 0;
    char *myname = "top";

    /* get our name */
    if (argc > 0)
    {
	if ((myname = rindex(argv[0], '/')) == 0)
	{
	    myname = argv[0];
	}
	else
	{
	    myname++;
	}
    }

    /* check for time delay option */
    if (argc > 1 && argv[1][0] == '-')
    {
	if (argv[1][1] != 's')
	{
	    fprintf(stderr, "Usage: %s [-sn] [number]\n", myname);
	    exit(1);
	}
	delay = atoi(&(argv[1][2]));
	argc--;
	argv++;
    }

    /* get count of top processes to display (if any) */
    if (argc > 1)
    {
	topn = atoi(argv[1]);
    }

    /* open kmem and mem */
    if ((kmem = open(KMEM, 0)) < 0)
    {
	perror(KMEM);
	exit(1);
    }
    if ((mem = open(MEM, 0)) < 0)
    {
	perror(MEM);
	exit(1);
    }

    /* get the list of symbols we want to access in the kernel */
    errno = 0;
    nlist(VMUNIX, nlst);
    if (nlst[0].n_type == 0)
    {
	fprintf(stderr, "%s: can't nlist image\n", VMUNIX);
	exit(1);
    }

    /* get the symbol values out of kmem */
    getkval(nlst[X_PROC].n_value,  &proc,  sizeof(int), nlst[X_PROC].n_name);
    getkval(nlst[X_NPROC].n_value, &nproc, sizeof(int), nlst[X_NPROC].n_name);
    getkval(nlst[X_CCPU].n_value,  &ccpu,  sizeof(int), nlst[X_CCPU].n_name);
    getkval(nlst[X_HZ].n_value,    &hz,    sizeof(int), nlst[X_HZ].n_name);

    /* some calculations we use later */

    cp_time_offset = nlst[X_CP_TIME].n_value;
    mpid_offset = nlst[X_MPID].n_value;
    avenrun_offset = nlst[X_AVENRUN].n_value;

    /* this is used in calculating WCPU -- calculate it ahead of time */
    logcpu = log(ccpu);

    /* allocate space for proc structure array and array of pointers */
    bytes = nproc * sizeof(struct proc);
    pbase = (struct proc *)sbrk(bytes);
    pref  = (struct proc **)sbrk(nproc * sizeof(struct proc *));

    /* Just in case ... */
    if (pbase == (struct proc *)NULL || pref == (struct proc **)NULL)
    {
	fprintf(stderr, "%s: can't allocate sufficient memory\n", myname);
    }

    /* initialize the hashing stuff */
    init_hash();

    /* initialize curses and screen (last) */
    initscr();
    erase();
    clear();
    refresh();

    /* setup signal handlers */
    signal(SIGINT, leave);
    signal(SIGQUIT, leave);

    /* can only display (LINES - Header_lines) processes */
    if (topn > LINES - Header_lines)
    {
	printw("Warning: this terminal can only display %d processes...\n",
	    LINES - Header_lines);
	refresh();
	sleep(2);
	topn = LINES - Header_lines;
	clear();
    }

    /* main loop ... */

    while (1)		/* while(1)'s are forever ... */
    {
	/* read all the proc structures in one fell swoop */
	getkval(proc, pbase, bytes, "proc array");

	/* get the cp_time array */
	getkval(cp_time_offset, cp_time, sizeof(cp_time), "_cp_time");

	/* get load average array */
	getkval(avenrun_offset, avenrun, sizeof(avenrun), "_avenrun");

	/* get mpid -- process id of last process */
	getkval(mpid_offset, &mpid, sizeof(mpid), "_mpid");

	/* count up process states and get pointers to interesting procs */
	total_procs = 0;
	active_procs = 0;
	bzero(proc_brkdn, sizeof(proc_brkdn));
	for (pp = pbase, i = 0; i < nproc; pp++, i++)
	{
	    /* place pointers to each valid proc structure in pref[] */
	    /* (processes with SSYS set are system processes) */
	    if (pp->p_pid != 0 && pp->p_stat != 0 && (pp->p_flag & SSYS) == 0)
	    {
		total_procs++;
		proc_brkdn[pp->p_stat]++;
		if (pp->p_stat != SZOMB)
		{
		    pref[active_procs++] = pp;
		}
	    }
	}

	/* display the load averages */
	printw("last pid: %d;  load averages", mpid);
	{
	    for (i = 0; i < 3; i++)
	    {
		printw("%c %4.2f",
		    i == 0 ? ':' : ',',
		    avenrun[i]);
	    }
	}

	/*
	 *  Display the current time.
	 *  "ctime" always returns a string that looks like this:
	 *  
	 *	Sun Sep 16 01:03:52 1973
	 *      012345678901234567890123
	 *	          1         2
	 *
	 *  We want indices 11 thru 18 (length 8).
	 */

	curr_time = time(0);
	move(0, 79-8);
	printw("%-8.8s\n", &(ctime(&curr_time)[11]));

	/* display process state breakdown */
	printw("%d processes", total_procs);
	for (i = 1; i < 7; i++)
	{
	    if (proc_brkdn[i] != 0)
		printw("%c %d %s%s",
			i == 1 ? ':' : ',',
			proc_brkdn[i],
			state_name[i],
			(i == SZOMB) && (proc_brkdn[i] > 1) ? "s" : "");
	}

	/* calculate percentage time in each cpu state */
	printw("\nCpu states: ");
	if (do_cpu)	/* but not the first time */
	{
	    total_change = 0;
	    for (i = 0; i < CPUSTATES; i++)
	    {
		/* calculate changes for each state and overall change */
		if (cp_time[i] < cp_old[i])
		{
		    /* this only happens when the counter wraps */
		    change = (int)
			((unsigned long)cp_time[i]-(unsigned long)cp_old[i]);
		}
		else
		{
		    change = cp_time[i] - cp_old[i];
		}
		total_change += (cp_change[i] = change);
	    }
	    for (i = 0; i < CPUSTATES; i++)
	    {
		printw("%s%4.1f%% %s",
			i == 0 ? "" : ", ",
			((float)cp_change[i] / (float)total_change) * 100.0,
			cpu_state[i]);
		cp_old[i] = cp_time[i];
	    }
	}
	else
	{
	    /* we'll do it next time */
	    for (i = 0; i < CPUSTATES; i++)
	    {
		printw("%s      %s",
			i == 0 ? "" : ", ",
			cpu_state[i]);
		cp_old[i] = 0;
	    }
	    do_cpu = 1;
	}
	printw("\n");

	if (topn > 0)
	{
	    printw("\n  PID USERNAME PRI NICE   SIZE   RES STATE   TIME   WCPU    CPU COMMAND\n");
    
	    /* sort by cpu percentage (pctcpu) */
	    qsort(pref, active_procs, sizeof(struct proc *), proc_compar);
    
	    /* now, show the top whatever */
	    for (prefp = pref, i = 0; i < topn; prefp++, i++)
	    {
		register struct proc *p = *prefp;
    
		if (p->p_time == 0)
		{
		    fprintf(stderr, "assumption failed!\n");
		}
		if (getu(p) == -1)
		{
		    strcpy(u.u_comm, "<swapped>");
		    cputime = 0;
		}
		else
		{
		    cputime = 
#ifdef FOUR_TWO
			(u.u_ru.ru_utime.tv_sec + u.u_ru.ru_stime.tv_sec);
#else
			(int)((float)(u.u_vm.vm_utime + u.u_vm.vm_stime)/hz);
#endif
		}
		printw("%5d %-8.8s %3d %4d%6dK %4dK %-5s%4d:%02d %5.2f%% %5.2f%% %-14.14s\n",
		    p->p_pid,
		    username(p->p_uid),
		    p->p_pri - PZERO,
		    p->p_nice - NZERO,
		    (p->p_tsize + p->p_dsize + p->p_ssize) >> 1,
		    p->p_rssize >> 1,
		    state_abbrev[p->p_stat],
		    cputime / 60l,
		    cputime % 60l,
		    (100.0 * p->p_pctcpu / (1.0 - exp(p->p_time * logcpu))),
		    (100.0 * p->p_pctcpu),
		    u.u_comm);
	    }
	}
	refresh();

	/* wait ... */
	signal(SIGALRM, onalrm);
	alarm(delay);
	pause();

	/* clear for new display */
	erase();
    }
}

/*
 *  signal handlers
 */

leave()			/* exit under normal conditions -- INT handler */

{
    move(LINES - 1, 0);
    refresh();
    endwin();
    exit(0);
}

quit(status)		/* exit under duress */

int status;

{
    endwin();
    exit(status);
}

onalrm()

{
    return(0);
}

/*
 *  comparison function for "qsort"
 */

proc_compar(p1, p2)

struct proc **p1;
struct proc **p2;

{
    if ((*p1)->p_pctcpu < (*p2)->p_pctcpu)
    {
	return(1);
    }
    else
    {
	return(-1);
    }
}

/*
 *  These routines handle uid to username mapping.
 *  They use a hashing table scheme to reduce reading overhead.
 */

struct hash_el {
    int  uid;
    char name[8];
};

#define    H_empty	-1

struct hash_el hash_table[Table_size];

init_hash()

{
    register int i;
    register struct hash_el *h;

    for (h = hash_table, i = 0; i < Table_size; h++, i++)
    {
	h->uid = H_empty;
    }

    setpwent();
}

char *username(uid)

int uid;

{
    int index;
    register int found;
    char *name;

    /* This is incredibly naive, but it'll probably get changed anyway */
    index = hashit(uid);
    while ((found = hash_table[index].uid) != uid)
    {
	if (found == H_empty)
	{
	    /* not here -- get it out of passwd */
	    index = get_user(uid);
	    break;		/* out of while */
	}
	index = index++ % Table_size;
    }
    return(hash_table[index].name);
}

enter_user(uid, name)

int  uid;
char *name;

{
    int length;
    int index;
    int try;
    static int uid_count = 0;

    /* avoid table overflow -- insure at least one empty slot */
    if (++uid_count >= Table_size)
    {
	fprintf(stderr, "table overflow: too many users\n");
	quit(1);
    }

    index = hashit(uid);
    while ((try = hash_table[index].uid) != H_empty)
    {
	if (try == uid)
	{
	    return(index);
	}
	index = index++ % Table_size;
    }
    hash_table[index].uid = uid;
    strncpy(hash_table[index].name, name, 8);
    return(index);
}

get_user(uid)

int uid;

{
    struct passwd *pwd;
    static char buff[20];
    int last_index;

    while ((pwd = getpwent()) != NULL)
    {
	last_index = enter_user(pwd->pw_uid, pwd->pw_name);
	if (pwd->pw_uid == uid)
	{
	    return(last_index);
	}
    }
    sprintf(buff, "%d", uid);
    return(enter_user(uid, buff));
}

hashit(i)

int i;

{
    return(i % Table_size);
}

/*
 *  All of this stuff gets things out of the memory files.
 */

/*
 *  Get the user structure for the process who's proc structure is in p.  The
 *  user structure is returned in u.
 */
getu(p)

struct proc *p;

{
    struct pte uptes[UPAGES];
    register caddr_t upage;
    register struct pte *pte;
    register nbytes, n;

    /*
     *  Check if the process is currently loaded or swapped out.  The way we
     *  get the u area is totally different for the two cases.  For this
     *  application, we just don't bother if the process is swapped out.
     */
    if ((p->p_flag & SLOAD) == 0)
    {
	return(-1);
    }

    /*
     *  Process is currently in memory, we hope!
     */
    getkval(p->p_addr, uptes, sizeof(uptes), "p->p_addr");
    upage = (caddr_t)&u;
    pte = uptes;
    for (nbytes = sizeof(u); nbytes > 0; nbytes -= NBPG)
    {
    	lseek(mem, pte++->pg_pfnum * NBPG, 0);
	n = MIN(nbytes, NBPG);
	if (read(mem, upage, n) != n)
	{
	    /* we can't seem to get to it, so pretend it's swapped out */
	    return(-1);
	}
	upage += n;
    }
    return(0);
}

/*
 *  get the value of something from /dev/kmem
 */

getkval(offset, ptr, size, refstr)

long offset;
int *ptr;
int size;
char *refstr;

{
    if (lseek(kmem, offset, 0) == -1)
    {
	fprintf(stderr, "%s: lseek to %s: %s\n",
	    KMEM, refstr, sys_errlist[errno]);
	quit(1);
    }
    if (read(kmem, ptr, size) == -1)
    {
	fprintf(stderr, "%s: reading %s: %s\n",
	    KMEM, refstr, sys_errlist[errno]);
	quit(1);
    }
}

/*
 *  outc - real subroutine that writes a character; for termcap
 */

outc(ch)

char ch;

{
    putchar(ch);
}
