/*
 *        This program is designed to kill any jobs which have been idle
 *      for too long.  An idle job is defined as one running on a terminal
 *      which has not been accessed (read) for more than TOOLONG minutes.
*/

#include <a.out.h>
#include <signal.h>
#include <stdio.h>
#include <utmp.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/var.h>

#define sys(n) (n*60)                   /* Minutes to internal time     */
#define min(n) ((n+30)/60)              /* Internal time to minutes     */
#define TOOLONG sys(60)                 /* Idle until job is killed     */
#define WARNING TOOLONG-sys(5)          /* Idle until warning message   */
#define IGNORE(n) (!strcmp(n,"backup"))	/* Don't kill this user		*/

int kmem;                               /* Used to open kernel memory   */
int mem;                                /* Used to open physical memory */
int swap;                               /* Used to open swap area       */
int utemp;                              /* /etc/utmp file descriptor    */
char device[80];                        /* Name of pty or tty           */
struct utmp ut;                         /* Login accounting record      */
struct stat sbuf;                       /* Status of pty or tty         */
struct user u;                          /* User structure of a process  */
char **argv;                            /* Invocation argv              */
unsigned tosleep;			/* Time to sleep                */
daddr_t	swplo;				/* Base of swap space in kernel	*/
struct nlist nl[] = {
  { "_v" },
#define X_V	0
  { "_proc" },
#define X_PROC	1
  { "_swplo" },
#define X_SWPLO	2
  { NULL },
};
char *malloc();                         /* Memory allocator             */
char *sprintf();
long lseek();
time_t time();

/*
 *        Read the user structure for a given process.  This is a lot of
 *      trouble to go to just to see if it is running on a given terminal.
*/
get_user_structure(p)
struct proc *p;
{ int file;
  long addr;

  if (p->p_flag & SLOAD) {
    addr = mltoa((long) p->p_addr.p_caddr);
    file = mem;
  } else {
    addr = (swplo + p->p_addr.p_daddr) << BSHIFT;
    file = swap;
  }
  addr += OFFUSRPG;	/* Skip kernel mode stack */
  lseek(file, addr, 0);
  read(file, (char *)&u, sizeof(u));
}

/*
 *        Destroy a job.
*/
killit()
{ static FILE *tty;                     /* Used to open a tty           */
  static struct proc *aproc;            /* Process table address        */
  static unsigned nproc;                /* Process table size           */
  static struct proc p;			/* Copy of kernel proc entry    */
  static struct var v;			/* Copy of kernel parameters    */

  if (fork() > 0) {			/* Notification may block	*/
    tty = fopen(device,"w");		/* Open terminal for writing    */
    if (tty < 0)
      fprintf(stderr,"%s: Can't warn %s\n",argv[0],device);
    else
      fprintf(tty,"\n\7Your job will be killed IMMEDIATELY.\7\n");
    fclose(tty);			/* Close login info file        */
    exit(0);				/* Done, give child to init	*/
  }					/* Child lives on to kill again	*/
  lseek(kmem, (long)nl[X_V].n_value, 0);
  read(kmem, (char *) &v, sizeof v);	/* Read kernel parameters	*/
  nproc = v.v_proc;			/* Get proc table size		*/
  aproc = (struct proc *)nl[X_PROC].n_value;
  while (nproc != 0) {
    lseek(kmem, (long)aproc, 0);	/* Read process table           */
    read(kmem, (char *)&p, sizeof p);
    if (p.p_stat && p.p_pgrp)
      { get_user_structure(&p);
        if (u.u_ttyd == sbuf.st_rdev)
          kill(p.p_pid,SIGHUP);
      }
    aproc++;
    nproc--;
  }
}

/*
 *        Warn a user of his job's impending destruction.
*/
warn(t)
time_t t;                               /* Minutes to go                */
{ FILE *tty;                            /* Used to open a tty           */

  tty = fopen(device,"w");              /* Open terminal for writing    */
  if (tty < 0)
    fprintf(stderr,"%s: Can't warn %s\n",argv[0],device);
  else
    fprintf(tty,"\n\7Your job will be killed in %ld minutes.\7\n",min(t));
  fclose(tty);                          /* Close login info file        */
  tosleep = sys(1);                     /* Minimum possible sleep       */
}

/*
 *        See if a sluggard is logged on and has not done anything for
 *      over TOOLONG minutes.  If so, kill all processes on that device
 *      and return.  Otherwise check for WARNING minutes, and give a
 *      warning if needed.
*/
check()
{ static time_t t;                      /* Holds time since tty touched */

  if (ut.ut_name[1] && !IGNORE(ut.ut_name))/* Got user logged in	*/
    { sprintf(device,"/dev/%s",ut.ut_line);/* Format device name        */
      stat(device,&sbuf);               /* Stat user's terminal         */
      t = time((time_t *)0) - sbuf.st_atime;
      if (t > TOOLONG)
        killit();                       /* Kill the job                 */
      else if (t > WARNING)
        warn(TOOLONG-t);                /* Warn the sluggard            */
      else if (tosleep > WARNING-t)     /* Adjust sleep time            */
        tosleep = WARNING - t;
    }
}

main(argc,av)
int argc;
char *av[];
{ int i;

  for (i = 0; i < 16; i++)
    close(i);
  signal(SIGHUP, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  open("/",0);
  dup(0);
  if (open("/dev/console",2) != 2)      /* Reopen error channel         */
    exit(1);
  if (fork())                           /* Fork the daemon              */
    exit(0);                            /* Parent exits                 */
  argv = av;                            /* Give error routines access   */
  mem = open("/dev/mem",0);             /* Open kernel memory           */
  if (mem < 0)
    { fprintf(stderr,"%s: Can't open mem\n",argv[0]);
      exit(1);
    }
  kmem = open("/dev/kmem",0);           /* Open kernel memory           */
  if (kmem < 0)
    { fprintf(stderr,"%s: Can't open kmem\n",argv[0]);
      exit(1);
    }
  swap = open("/dev/swap",0);           /* Open kernel memory           */
  if (swap < 0)
    { fprintf(stderr,"%s: Can't open swap area\n",argv[0]);
      exit(1);
    }
  nlist("/xenix",nl);                  /* Get relevant pointers        */
  if (nl[0].n_type == 0)
    { fprintf(stderr,"%s: No system namelist\n",argv[0]);
      exit(1);
    }
  lseek(kmem, (long)nl[X_SWPLO].n_value, 0);
  read(kmem, (char *)&swplo, sizeof(swplo));
  utemp = open("/etc/utmp",0);          /* Open login info file         */
  while (1)
    { utemp = open("/etc/utmp",0);      /* Open login info file         */
      tosleep = WARNING;                /* Max possible sleep           */
      if (utemp < 0)
        { fprintf(stderr,"%s: Can't open /etc/utmp\n",argv[0]);
          exit(1);
        }
      while (read(utemp, (char *)&ut, sizeof ut) > 0)
        check();
      close(utemp);                     /* Close login info file        */
      sleep(tosleep);                   /* Sleep a while                */
    }
}
