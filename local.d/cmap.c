/*
 * %W% %G%
 * Display state of coremap area in kernel.
*/
#ifdef M_XENIX
#include <sys/param.h>
#include <sys/var.h>
#include <sys/map.h>
#include <stdio.h>
#include <a.out.h>

extern char *malloc();

struct	xlist xl[] = {
	{ 0, 0, 0L, "_v" },
#define	X_V		0	/* Config Structure			*/
	{ 0, 0, 0L, "_coremap" },
#define	X_COREMAP	1	/* Core Map				*/
	{ 0, 0, 0L, "_bufbase" },
#define X_BUFBASE	2	/* Address of disk cache		*/
	{ 0, 0, 0L, "_maxmem" },
#define X_MAXMEM	3	/* Total User Pages			*/
	{ 0, 0, 0L, "" }
};

int verbose;			/* Nonzero if full printout desired	*/
int status;			/* Nonzero if error detected		*/

int	kmem;			/* Kernel memory			*/
int	mem;			/* Physical memory			*/
long	lseek();

struct var v;			/* The config structure from the kernel	*/
struct map *coremap;		/* Pointer to copy of kernel coremap	*/
unsigned long bufbase;		/* Address of disk cache		*/
unsigned short maxmem;		/* (Total User Pages)*3/4		*/
unsigned short fpage;		/* First User Page			*/
unsigned short lpage;		/* Last User Page			*/

/*
 *	Print interesting coremap stats
*/
main(argc, argv)
int argc;
char *argv[];
{ struct map *m;
  int cnt, low;

  verbose = 0;
  if ((argc > 1) && (argv[1][0] == '-')) {
    verbose++;
    argc--;
    argv++;
  }
  if ((kmem = open("/dev/kmem", 0)) < 0) {
    (void) perror("kmem");
    (void) exit(1);
  }
  if ((mem = open("/dev/mem", 0)) < 0) {
    (void) perror("mem");
    (void) exit(1);
  }
  (void) xlist("/xenix", xl);
  if (xl[0].xl_type == 0) {
    (void) fprintf(stderr, "No namelist\n");
    (void) exit(1);
  }
  /*
   *	NOTE: The size of the coremap is not configurable, contrary to
   *	popular belief.  The code in sys/malloc.c assumes the coremap
   *	contains 2*v.v_proc entries.  If you try to reduce the coremap
   *	size, kernel malloc may corrupt kernel data, and if you increase
   *	the allocation kernel malloc will not use the additional space.
  */
  (void) lseek(kmem, xl[X_V].xl_value, 0);
  (void) read(kmem, (char *)&v, sizeof(v));
  /*
   *	Compute first page = Address of buffers plus size of buffers.
  */
  (void) lseek(kmem, xl[X_BUFBASE].xl_value, 0);
  (void) read(kmem, (char *)&bufbase, sizeof(bufbase));
  fpage = atoml(bufbase+(long)BSIZE*v.v_buf);
  /*
   *	Kernel maxmem is 3/4 of free space.  Scale and add to fpage to
   *	compute last page.  Note that last = free + avail - 1.
  */
  (void) lseek(kmem, xl[X_MAXMEM].xl_value, 0);
  (void) read(kmem, (char *)&maxmem, sizeof(maxmem));
  lpage = fpage + (maxmem*4)/3 - 1;
  /*
   *	Report results to user if they were asked for.
  */
  if (verbose) {
    (void) printf("The coremap starts at 0x%lx ", xl[X_COREMAP].xl_value);
    (void) printf("for %d slots\n", 2*v.v_proc);
    (void) printf("Low memory  == 0x%04x\n", fpage);
    (void) printf("High memory == 0x%04x\n", lpage);
  }
  /*
   *	Allocate space for the core map.
  */
  coremap = (struct map *) malloc(2*v.v_proc*sizeof(*coremap));
  if (coremap == 0) {
    (void) fprintf(stderr, "malloc(%u) failed\n", 2*v.v_proc*sizeof(*coremap));
    (void) exit(1);
  }
  /*
   *	Since the coremap is very volatile, we must read the whole thing
   *	with one syscall or we will get an inconsistent view.
  */
  (void) lseek(kmem, xl[X_COREMAP].xl_value, 0);
  (void) read(kmem, (char *)coremap, 2*v.v_proc*sizeof(*coremap));
  status = 0;
  /*
   *	Print the core map.
   *	-- guard depth in case list is not terminated properly.
  */
  cnt = 0;				/* Initial Depth		*/
  m = coremap;				/* Start at front of coremap	*/
  low = fpage;				/* Free mem should be user mem	*/
  if (low > m->m_addr) {
    (void) printf("\ncoremap contains kernel memory\n");
    status++;
  }
  if (verbose)
    printf("Memory free list:");
  while (m->m_size && (cnt < 2*v.v_proc)) {
    if (low > m->m_addr) {
      (void) printf("\ncoremap sequence error\n");
      status++;
    }
    low = m->m_addr + m->m_size - 1;	/* Set up new low		*/
    if (low > lpage) {
      (void) printf("\ncoremap frames non-existent memory\n");
      status++;
    }
    if (low < m->m_addr) {
      (void) printf("\ncoremap has negative region size\n");
      status++;
    }
    if (verbose) {
      if (cnt % 4 == 0)
        (void) printf("\n");
      (void) printf("  0x%04x-0x%04x", m->m_addr, low);
    }
    m++;
    cnt++;
  }
  if (verbose)
    (void) printf("\n\n");
  if (cnt >= 2*v.v_proc) {
    printf("Memory free list is not properly terminated.\n");
    status++;
  }
  exit(status);
}
#else /*M_XENIX*/
main()
{ printf("Not ported yet\n");
}
#endif /*M_XENIX*/
