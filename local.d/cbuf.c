/*
 * cbuf - Display state of cbuf area in kernel.
 *
 * COMPILE USING SAME MODEL AS THE KERNEL.
 *
 * This program displays the state of the cbuf allocations in the
 * kernel.  Specifically, it dispalys the cbuf free list and then
 * attempts to determine the status of each cbuf in the system.
*/

#include <sys/param.h>
#include <sys/mmu.h>
#include <sys/tty.h>
#include <stdio.h>
#include <a.out.h>

struct	xlist xl[] = {
	{ 0, 0, 0L, "_gdt" },
#define	X_GDT		0	/* Global Descriptor Table		*/
	{ 0, 0, 0L, "_cbuffree" },
#define	X_CBUFFREE	1	/* Cbuf free list head			*/
	{ 0, 0, 0L, "_i74tty" },
#define	X_74TTY		2	/* 8274 driver's tty structures		*/
	{ 0, 0, 0L, "_lptty" },
#define	X_LPTTY		3	/* lp driver's tty structures		*/
	{ 0, 0, 0L, "_spt_tty" },
#define	X_SPTTTY	4	/* lp driver's tty structures		*/
	{ 0, 0, 0L, "_i188tty" },
#define	X_188TTY	5	/* i188 driver's tty structures		*/
	{ 0, 0, 0L, "_N188" },
#define	X_N188		6	/* i188 board count			*/
	{ 0, 0, 0L, "_i534tty" },
#define	X_534TTY	7	/* i534 driver's tty structures		*/
	{ 0, 0, 0L, "_N534" },
#define	X_N534		8	/* i534 board count			*/
	{ 0, 0, 0L, "_i544tty" },
#define	X_544TTY	9	/* i544 driver's tty structures		*/
	{ 0, 0, 0L, "_N544" },
#define	X_N544		10	/* i544 board count			*/
	{ 0, 0, 0L, "" }
};

int verbose;			/* Nonzero if full printout desired	*/
int status;			/* Nonzero if error detected		*/

int	kmem;			/* Kernel memory			*/
int	mem;			/* Physical memory			*/
long	lseek();

struct desctab descr;		/* A descriptor as read from the GDT	*/
long buf_base;			/* The CBUF segment base		*/

struct tty tty;			/* A tty structure as read from kmem	*/

/*
 *	There is at most one segment (65536 bytes) worth of CBUFs.  Here
 *	we declare an array large enough to track references to each of
 *	the possible CBUFs.  [65536/(CBUFSZ*4) == 16384/CBUFSZ].
*/
int portused[16384/CBUFSZ];	/* Count of uses for each cbuf		*/

/*
 *	Print allocations for <num> tty structures beginning at <addr> in kmem.
*/
void
doports(str, addr, num)
char *str;
long addr;
int num;
{ int i;
  unsigned off;
  struct nclist *p;

  if (addr == 0) {
    if (verbose)
      (void) printf("No %s in this system...\n", str);
    return;
  }
  (void) lseek(kmem, addr, 0);
  for (i = 0; i < num; i++) {
    (void) read(kmem, (char *)&tty, sizeof(tty));
    if (tty.t_state & ISOPEN) {
      p = (struct nclist *)&tty.t_rawq;
      off = p->c_addr;
      if (off % CBUFSZ) {
        (void) printf("%s port %d has bad cbuf ptr 0x%0x\n",
                       str,     i,                 off);
        status++;
      } else {
        if (verbose)
          (void) printf("%s port %d has allocated cbuf %d\n",
                         str,     i,                   off/(CBUFSZ*4));
        portused[off/(CBUFSZ*4)]++;
      }
    }
#ifdef DEBUG
    else
      if (verbose)
        (void) printf("%s port %d is not open\n", str, i);
#endif DEBUG
  }
}

/*
 *	Print interesting cbuf stats
*/
main(argc, argv)
int argc;
char *argv[];
{ unsigned off;
  int cnt;

  verbose = 0;
  if ((argc > 1) && (argv[1][0] == '-')) {
    verbose++;
    argc--;
    argv++;
  }
  if ((kmem = open("/dev/kmem", 0)) < 0) {
    (void) perror("kmem:");
    (void) exit(1);
  }
  if ((mem = open("/dev/mem", 0)) < 0) {
    (void) perror("mem:");
    (void) exit(1);
  }
  (void) xlist("/xenix", xl);
  if (xl[0].xl_type == 0) {
    (void) fprintf(stderr, "No namelist\n");
    (void) exit(1);
  }
#ifdef DEBUG
  printf("The gdt starts at 0x%lx\n", xl[X_GDT].xl_value);
#endif DEBUG
  (void) lseek(kmem, xl[X_GDT].xl_value+TTY0_SEL, 0);
  (void) read(kmem, (char *)&descr, sizeof(descr));
  buf_base = (((long)descr.d_hiaddr)<<16) + descr.d_loaddr;
  if (verbose)
    (void) printf("The cbuf area starts at 0x%lx ", buf_base);
  status = 0;
  if (++descr.d_limit % (CBUFSZ*4)) {
    (void) printf("\nThe cbuf area has size %d bytes (OOPS!)\n", descr.d_limit);
    status++;
  } else
    if (verbose)
      (void) printf("for %d cbufs\n", descr.d_limit/(CBUFSZ*4));
  /*
   *	Print the cbuf free list.
   *	-- guard depth in case circular list is encountered.
  */
  (void) lseek(kmem, xl[X_CBUFFREE].xl_value, 0);
  (void) read(kmem, (char *)&off, sizeof(off));
  cnt = 0;
  if (verbose)
    printf("Cbuf free list:");
  while ((off != 0xFFFF) && (off <= descr.d_limit)) {
    if (cnt % 8 == 0)
      if (verbose)
        (void) printf("\n");
    if (verbose)
      (void) printf("  %2d", off/(CBUFSZ*4));
    if (off % (CBUFSZ*4)) {
      printf("\nThe cbuf free list is garbage: %04x", off);
      status++;
      break;
    } else
      portused[off/(CBUFSZ*4)]++;
    (void) lseek(mem, buf_base+off, 0);
    (void) read(mem, (char *)&off, sizeof(off));
    if (++cnt > descr.d_limit/(CBUFSZ*4)) {
      printf("\nThe cbuf free list is garbage");
      status++;
      break;
    }
  }
  if (verbose)
    (void) printf("\n\n");
  doports("8274", xl[X_74TTY].xl_value, 2);
  doports("  lp", xl[X_LPTTY].xl_value, 1);
  doports("pseudo-tty", xl[X_SPTTTY].xl_value, 16);
  (void) lseek(kmem, xl[X_N188].xl_value, 0);
  (void) read(kmem, (char *)&cnt, sizeof(cnt));
  doports(" 188", xl[X_188TTY].xl_value, cnt*12);
  (void) lseek(kmem, xl[X_N534].xl_value, 0);
  (void) read(kmem, (char *)&cnt, sizeof(cnt));
  doports(" 534", xl[X_534TTY].xl_value, cnt*4);
  (void) lseek(kmem, xl[X_N544].xl_value, 0);
  (void) read(kmem, (char *)&cnt, sizeof(cnt));
  doports(" 544", xl[X_544TTY].xl_value, cnt*4);
  for (cnt = 0; cnt < descr.d_limit/(CBUFSZ*4); cnt++)
    switch (portused[cnt]) {
    case 0:  printf("Cbuf %d has been lost\n", cnt);
             status++;
    case 1:  break;
    default: printf("Cbuf %d has been used %d times\n", cnt, portused[cnt]);
             status++;
    }
  exit(status);
}
