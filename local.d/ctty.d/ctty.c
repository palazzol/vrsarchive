/*
 *	Ctty - Set terminal type and speed
 *
 *	usage:	ttype type [tty...]
 *		tspeed speed [tty...]
*/
#include <stdio.h>
#ifndef M_XENIX
#include <sys/types.h>
#include <nlist.h>
#endif
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/user.h>

#ifdef M_XENIX
#define KERNEL	"/xenix"
#else
#define KERNEL	"/unix"
#endif

char buf[1024];

/*
 *	Routine to return the name of the controlling tty.  This is a lot
 *	of bother because there is no easy way to get this information
 *	from the kernel.  We must nlist KERNEL, seek into /dev/kmem for
 *	_u, Read the U structure, extract u.u_ttyd, and then search for
 *	a node with that major/minor number.  Thank goodness we know _u
 *	is our own u structure and is guaranteed to be incore because
 *	we are running.  This gets my vote for most needed system call.
*/
char *
mytty()
{ static struct nlist nl[] = {
#ifdef M_XENIX
    { "_u" },
#else
    { "u" },
#endif
    { NULL }
  };
  int fd;
  static struct direct dbuf;
  static struct stat   sbuf;
  static struct user u;

  if (nlist(KERNEL, nl) == -1) {
    fprintf(stderr,"no namelist for %s\n", KERNEL);
    exit(1);
  }
  if (nl[0].n_type == 0) {
    fprintf(stderr,"no _u in namelist\n");
    exit(1);
  }
  fd = open("/dev/kmem",0);
  if (fd < 0) {
    perror("Cannot open /dev/kmem");
    exit(1);
  }
  if (lseek(fd,(long)nl[0].n_value,0) != nl[0].n_value) {
    perror("Cannot seek to _u in /dev/kmem");
    exit(1);
  }
  if (read(fd,(char *)&u,sizeof u) != sizeof u) {
    perror("Cannot read _u from /dev/kmem");
    exit(1);
  }
  (void) close(fd);
  if (chdir("/dev") == -1) {
    perror("Cannot chdir to /dev\n");
    exit(1);
  }
  fd = open(".",0);
  while (read(fd,(char *)&dbuf,sizeof dbuf) == sizeof dbuf) {
    if(dbuf.d_ino == 0)
      continue;
    if(stat(dbuf.d_name, &sbuf) < 0)
      continue;
    if ((sbuf.st_mode & S_IFMT) != S_IFCHR)
      continue;
    if (sbuf.st_rdev == u.u_ttyd) {
      close(fd);
      return(dbuf.d_name);
    }
  }
  close(fd);
  return((char *)NULL);
}

/*
 *	Set up a tty speed in /etc/ttys.  Knowledge of speed codes used
 *	by getty is wired in here.
*/
void
dottys(tty,spd)
char *tty, *spd;
{ static struct {
    char *code;
    int  speed;
  } sp[] = {
    { "-",  110 },
    { "0",  150 },
    { "1",  300 },
    { "3", 1200 },
    { "4", 2400 },
    { "5", 4800 },
    { "6", 9600 },
    { "@",    0 },
  }, *p;
  int   speed;

  speed = atoi(spd);
  for (p = sp; p->speed; p++)
    if (p->speed == speed)
      break;
  if (!p->speed || (p->speed != speed)) {
    fprintf(stderr,"Don't know how to set for %d baud\n",speed);
    return;
  }
  strcpy(buf,"sed '/");
  strcat(buf,tty);
  strcat(buf,"$/s/../1");
  strcat(buf,p->code);
  strcat(buf,"/' /etc/ttys > /tmp/");
  strcat(buf,tty);
  system(buf);			/* Fudge speed of the line		*/
  strcpy(buf,"cp /tmp/");
  strcat(buf,tty);
  strcat(buf," /etc/ttys");
  system(buf);			/* Copy back to /etc/ttys		*/
  strcpy(buf,"disable ");
  strcat(buf,tty);
  system(buf);			/* Disable the line			*/
  printf("Enabling %s...",tty);
  fflush(stdout);
  sleep(5);
  strcpy(buf,"enable ");
  strcat(buf,tty);
  system(buf);			/* Enable the line at new speed		*/
  printf("\n");
  strcpy(buf,"/tmp/");
  strcat(buf,tty);
  unlink(buf);			/* Remove temp file			*/
}

void
dottytype(tty,type)
char *tty;
char *type;
{
  strcpy(buf,"sed '/");
  strcat(buf,tty);
  strcat(buf,"$/s/.* /");
  strcat(buf,type);
  strcat(buf," /' /etc/ttytype > /tmp/");
  strcat(buf,tty);
  system(buf);			/* Fudge type of the line		*/
  strcpy(buf,"cp /tmp/");
  strcat(buf,tty);
  strcat(buf," /etc/ttytype");
  system(buf);			/* Copy back to /etc/ttytype		*/
  strcpy(buf,"/tmp/");
  strcat(buf,tty);
  unlink(buf);			/* Remove temp file			*/
}

main(argc,argv)
int   argc;
char *argv[];
{ char *type;
  char *tty;
  void (*func)();

  if (argc < 2) {
    if (argv[0][1] == 't')
      fprintf(stderr,"usage: %s type [tty...]\n",argv[0]);
    else
      fprintf(stderr,"usage: %s speed [tty...]\n",argv[0]);
    exit(1);
  }
  type = argv[1];
  func = dottys;
  if (argv[0][1] == 't') {
    if ((strcmp(type,"modem") != 0) && (tgetent(buf,type) != 1)) {
      fprintf(stderr,"Sorry, no termcap for %s\n",type);
      exit(1);
    }
    func = dottytype;
  }
  if (argc > 2) {
    if (getuid() != 0) {
      fprintf(stderr,"Permission denied\n");
      exit(1);
    }
    setuid(0);			/* Need real uid of root for cp	*/
    while (argc > 2) {
      (*func)(argv[2],type);
      argv++;
      argc--;
    }
  } else {
    tty = mytty();		/* Do login tty			*/
    setuid(0);			/* Need real uid of root for cp	*/
    (*func)(tty,type);
  }
}
