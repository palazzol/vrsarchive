/*
 $	3/10/85	@(#)mdemo.c	1.2
 *	Display a constantly updated map of main memory.
 *
*/
#ifdef lint
#  define far				/* Lint chokes on far ptrs	*/
#else
static char *sccsid = "3/10/85 @(#)mdemo.c	1.2";
					/* Suppress unref message	*/
#endif

#include <curses.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/proc.h>
#include <sys/relsym86.h>
#include <sys/user.h>
#include <sys/var.h>
#undef nl

extern char *ctime();
extern long lseek();
extern char *malloc();
extern char *strcat();
extern long time();

int         done;			/* Set by SIGINT handler	*/
int         kmem;			/* Kernel memory		*/
int         mem;			/* Physical memory		*/
float	    packing;			/* Scaling factor for display	*/
struct proc *p, *pp;
int         fupage;
int         maxmem;
struct var  v;

struct desctab ldt[NUSEGS];		/* The ldt for a process	*/
struct user u;				/* The u structure for a proc	*/

struct nlist nl[] = {
  { "_v" },
#define NL_V		0
  { "_maxmem" },
#define NL_MAXMEM	1
  { "_fupage" },
#define NL_FUPAGE	2
  { "_proc" },
#define NL_PROC		3
  { 0 },
};

/*
 *	Note that we are done when SIGINT received.
*/
setdone()
{ done = 1;
}

/*
 *	Draw a cute representation of a segment.  The name goes vertically
 *	above a line segment that has arrows on the end ("<---->").  Addr
 *	specifies the location of the picture, size the length of the line
 *	segment.  Name is the name to place at right angles to the line.
*/
position(name,addr,size)
char  *name;
int addr, size;
{ register r, c;
  float t;

  if (size == 0)
    return;
  if (*name == '\0')
    return;				/* The swapper makes messy displ*/
  r = LINES / 2 - 2;
  c = addr/packing + .5;
  t = size;				/* Size is length of the line	*/
  while (t > packing/2) {		/* For each character of line	*/
    if (c >= COLS) {			/* If time to wrap		*/
      r += 6;				/*   wrap to lower display area	*/
      c -= COLS;
    }
    move(r,c);				/* Move to spot			*/
    addch('-');				/* Draw part of line		*/
    c++;				/* Next character, next column	*/
    t -= packing;			/* One less to go		*/
  }
  r = LINES / 2 - 10;
  c = (addr+size/2)/packing + .5;	/* Find halfway point		*/
  if (c >= COLS) {			/* If need to wrap		*/
    r += 15;				/*   wrap to lower display area	*/
    c -= COLS;
  }
  name[7] = '\0';			/* Only show 8 chars of name	*/
  while (strlen(name) < 7)
    (void) strcat(name," ");		/* Pad the name to 8 characters	*/
  while (*name) {			/* Add name to the display	*/
    move(r,c);
    addch(*name);
    r++;				/* Next char under last		*/
    name++;
  }
  r = LINES / 2 - 2;
  c = addr/packing + .5;
  if (c >= COLS) {			/* If need to wrap		*/
    r += 6;				/*   wrap to lower display area	*/
    c -= COLS;
  }
  move(r,c);				/* Move to spot			*/
  addch('<');				/* Draw left endpoint		*/
  c += size/packing + .5;
  if (c >= COLS) {			/* If need to wrap		*/
    r += 6;				/*   wrap to lower display area	*/
    c -= COLS;
  }
  move(r,c);				/* Move to spot			*/
  addch('>');				/* Draw right endpoint		*/
}

/*
 *	Main program.
*/
main()
{ struct proc *addr;
  struct desctab *l;
  int i, r, c;
  long t;
  char *ct;

  /*
   *	Initialize for this kernel.
  */
  if (nlist("/xenix",nl) < 0) {		/* Find the kernel structures	*/
    perror("Cannot nlist /xenix");
    exit(1);
  }
  kmem = open("/dev/kmem",0);		/* Open kernel memory		*/
  if (kmem < 0) {
    perror("Cannot open /dev/kmem");
    exit(1);
  }
  mem = open("/dev/mem",0);		/* Open physical memory		*/
  if (mem < 0) {
    perror("Cannot open /dev/mem");
    exit(1);
  }
  (void) lseek(kmem,(long)nl[NL_V].n_value,0);
  (void) read(kmem,(char *)&v,sizeof v);
  p = (struct proc *)malloc((unsigned)(sizeof(*p)*v.v_proc));
  (void) lseek(kmem,(long)nl[NL_MAXMEM].n_value,0);
  (void) read(kmem,(char *)&maxmem,sizeof maxmem);
  (void) lseek(kmem,(long)nl[NL_FUPAGE].n_value,0);
  (void) read(kmem,(char *)&fupage,sizeof fupage);
  /*
   *	Basic screen initialization.
  */
  savetty();
  signal(SIGINT,setdone);
  initscr();
  move(0,0);				/* Top Left of display		*/
  printw("Xenix Release 3");
  move(0,COLS/2-7);			/* Top Center of display	*/
  printw("Memory Display");
  move(LINES/2-1,0);			/* Go to upper center of screen	*/
  for (i = 0; i < COLS; i++)
    addch('-');				/* Draw a line			*/
  move(LINES/2+1,0);			/* Go to center of screen	*/
  for (i = 0; i < COLS; i++)
    addch('=');				/* Draw a line			*/
  move(LINES/2+3,0);			/* Go to lower center of screen	*/
  for (i = 0; i < COLS; i++)
    addch('-');				/* Draw a line			*/
  packing = maxmem / 16.0;		/* 16 slices total		*/
  r = LINES / 2;			/* Start above center line	*/
  c = COLS/8 - 4;			/* 8 slices per line		*/
  for (i = 1; i <= 16; i++) {		/* Put in slice markers		*/
     move(r, c);
     printw("%4d", (int)(i*packing+.5));/* Label a spot			*/
     c += COLS/8;
     if (i == 8) {			/* Detect line transition	*/
       r += 2;				/* Start new row		*/
       c = COLS/8 - 4;			/* At the beginning		*/
    }
  }
  refresh();
  packing = maxmem;
  packing = packing / (COLS*2.0);	/* Compute the scaling		*/
  done = 0;
  while (!done) {
    /*
     *	Put the date/time on the screen.
    */
    (void) time(&t);
    ct = ctime(&t);
    ct[20] = '\0';
    move(0,COLS-19);			/* Top Right of display		*/
    printw("%s",ct);
    /*
     *	Get copies of the kernel process structures.
    */
    (void) lseek(kmem,(long)nl[NL_PROC].n_value,0);
    (void) read(kmem,(char *)p,(unsigned)(sizeof(*p)*v.v_proc));
    /*
     *	Have copies of the state of the kernel, now make a pretty picture.
    */
    addr = (struct proc *)nl[NL_PROC].n_value;
    for (pp = p; pp < p+v.v_proc; pp++,addr++) {
      if (!(pp->p_flag & SLOAD))
        continue;			/* Skip procs not in memory	*/
      (void) lseek(mem,(long)mltoa(pp->p_addr.p_caddr),0);
      (void) read(mem,(char *)&u,sizeof u);/* Get the u structure	*/
      r = pp->p_ldsel >> 3;		/* How much ldt?		*/
      if (pp->p_ldsel >= NUSEGS)
        continue;			/* Ldt was weird		*/
      (void) read(mem,(char *)ldt,sizeof(ldt[0])*(pp->p_ldsel));
      if (u.u_procp != addr)		/* U structure point at p?	*/
        continue;			/* No, u structure vanished!	*/
      for (l = ldt; l <= ldt+r; l++)
        position(u.u_comm,
                 (int)((l->d_hiaddr<<6)+(l->d_loaddr>>10)),
                 (int)(l->d_limit/1024));
    }
    position("Kernel   ",0,fupage/2-1);
    position("Buffers  ",fupage/2,v.v_buf);
    refresh();
    sleep(1);
    for (r = LINES/2-10; r <= LINES/2-2; r++) {
       move(r,0);
       clrtoeol();
    }
    for (r = LINES/2+4; r <= LINES/2+12; r++) {
       move(r,0);
       clrtoeol();
    }
  }
  clear();				/* Clear the screen		*/
  refresh();
  endwin();				/* Restore terminal modes	*/
  exit(0);				/* and exit			*/
}
