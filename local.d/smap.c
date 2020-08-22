/*
 * %W% %G%
 * Display state of swapmap area in kernel.
*/
#include <sys/param.h>
#include <sys/var.h>
#include <sys/map.h>
#include <stdio.h>
#include <a.out.h>

extern char *malloc();

struct	xlist xl[] = {
	{ 0, 0, 0L, "_v" },
#define	X_V		0	/* Config Structure			*/
	{ 0, 0, 0L, "_swapmap" },
#define	X_SWAPMAP	1	/* Swap Map				*/
	{ 0, 0, 0L, "_nswap" },
#define X_NSWAP		2	/* Total User Pages			*/
	{ 0, 0, 0L, "" }
};

int verbose;			/* Nonzero if full printout desired	*/
int status;			/* Nonzero if error detected		*/

int	kmem;			/* Kernel memory			*/
int	mem;			/* Physical memory			*/
long	lseek();

struct var v;			/* The config structure from the kernel	*/
struct map *swpmap;		/* Pointer to copy of kernel swpmap	*/
unsigned short nswap;		/* Size of swap (AUGH!)			*/
unsigned short swplo;		/* First Used Block on Swap		*/

/*
 *	Print interesting swpmap stats
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
   *	NOTE: The size of the swpmap is not configurable, contrary to
   *	popular belief.  The code in sys/malloc.c assumes the swpmap
   *	contains 2*v.v_proc entries.  If you try to reduce the swpmap
   *	size, kernel malloc may corrupt kernel data, and if you increase
   *	the allocation kernel malloc will not use the additional space.
  */
  (void) lseek(kmem, xl[X_V].xl_value, 0);
  (void) read(kmem, (char *)&v, sizeof(v));
  /*
   *	NOTE: The start of the swap area (swplo) is not configurable,
   *	contrary to popular belief.  The swap area is assumed to start
   *	at block 1, and the user's configuration parameter is not used.
  */
  swplo = 1;
  /*
   *	Kernel nswap is The total size of swap.  The kernel will free nswap
   *	blocks starting at swplo.  This means all existing configurations
   *	are wrong.
  */
  (void) lseek(kmem, xl[X_NSWAP].xl_value, 0);
  (void) read(kmem, (char *)&nswap, sizeof(nswap));
#ifdef RIGHTWAY
  nswap += swplo - 1;			/* Compute last block of swap	*/
#else
  nswap += swplo;			/* THIS IS A BUG - SEE ABOVE	*/
#endif RIGHTWAY
  /*
   *	Report results to user if they were asked for.
  */
  if (verbose) {
    (void) printf("The swpmap starts at 0x%lx ", xl[X_SWAPMAP].xl_value);
    (void) printf("for %d slots\n", 2*v.v_proc);
    (void) printf("Low swap  == 0x%04x\n", swplo);
    (void) printf("High swap == 0x%04x\n", nswap);
  }
  /*
   *	Allocate space for the swap map.
  */
  swpmap = (struct map *) malloc(2*v.v_proc*sizeof(*swpmap));
  if (swpmap == 0) {
    (void) fprintf(stderr, "malloc(%u) failed\n", 2*v.v_proc*sizeof(*swpmap));
    (void) exit(1);
  }
  /*
   *	Since the swpmap is very volatile, we must read the whole thing
   *	with one syscall or we will get an inconsistent view.
  */
  (void) lseek(kmem, xl[X_SWAPMAP].xl_value, 0);
  (void) read(kmem, (char *)swpmap, 2*v.v_proc*sizeof(*swpmap));
  status = 0;
  /*
   *	Print the swap map.
   *	-- guard depth in case list is not terminated properly.
  */
  cnt = 0;				/* Initial Depth		*/
  m = swpmap;				/* Start at front of swpmap	*/
  low = swplo;				/* Free swap should >= swplo	*/
  if (low > m->m_addr) {
    (void) printf("\nswpmap contains blocks before swap\n");
    status++;
  }
  if (verbose)
    printf("Swap free list:");
  while (m->m_size && (cnt < 2*v.v_proc)) {
    if (low > m->m_addr) {
      (void) printf("\nswpmap sequence error\n");
      status++;
    }
    low = m->m_addr + m->m_size;	/* Set up new low		*/
    if (low > nswap) {
      (void) printf("\nswpmap frames non-existent swap\n");
      status++;
    }
    if (low < m->m_addr) {
      (void) printf("\nswpmap has negative region size\n");
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
    printf("Swap free list is not properly terminated.\n");
    status++;
  }
  exit(status);
}
