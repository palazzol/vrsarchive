/*
 *	User mode versions of kernel support routines.
 */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/cmn_err.h>
#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/dir.h>
#include <sys/signal.h>
#include <sys/immu.h>
#include <sys/user.h>
#include <stdio.h>
#include <varargs.h>

extern char *malloc();
struct user u;

void
cmn_err(va_alist)
va_dcl
{	va_list ap;
	int sev;
	char *fmt;

	va_start(ap);
	sev = va_arg(ap, int);
	switch (sev) {
	case CE_NOTE:
		printf("NOTE: ");
		break;
	case CE_WARN:
		printf("WARNING: ");
		break;
	case CE_PANIC:
		printf("PANIC: ");
		break;
	default:
		printf("cmn_err: sev == %d: ", sev);
	}
	fmt = va_arg(ap, char *);
	(void) vprintf(fmt, ap);
	va_end(ap);
	if (sev == CE_PANIC)
		abort();
}

void
panic(msg)
char *msg;
{
	cmn_err(CE_PANIC, msg);
}

caddr_t
getcpages(pages, nosleep)
int pages, nosleep;
{
	if (nosleep)
		panic("getcpages: called with NOSLEEP\n");
	return(malloc(pages*NBPP));
}

paddr_t
svirtophys(vaddr)
caddr_t vaddr;
{
	return((paddr_t)vaddr);
}

void
freepage(pfn)
int pfn;
{	/* NOT IMPLEMENTED -- MEMORY LEAK HERE */
}

void
iodone(bp)
struct buf *bp;
{
	bp->b_flags |= B_DONE;
}

void
iowait(bp)
struct buf *bp;
{
	if (!(bp->b_flags & B_DONE))
		panic("iowait: I/O wasn't done!\n");
}

paddr_t
vtop(vaddr, p)
caddr_t vaddr;
struct proc *p;
{
	return((paddr_t)vaddr-KVBASE);
}

void
bcopy(from, to, count)
char *from, *to;
int count;
{
	memcpy(to, from, count);
}

void
physio(strat, bp, dev, rw)
void (*strat)();
struct buf *bp;
dev_t dev;
{	struct buf buf;

	if (bp == 0)
		bp = &buf;
	bp->b_flags = B_BUSY | B_PHYS | rw;
	bp->b_error = 0;
	bp->b_proc = 0;
	bp->b_dev = dev;
	bp->b_un.b_addr = u.u_base;
	bp->b_blkno = btod(u.u_offset);
	bp->b_bcount = u.u_count;
	(*strat)(bp);
	if (!(bp->b_flags & B_DONE))
		panic("physio: I/O didn't happen!\n");
	bp->b_flags &= ~(B_BUSY|B_PHYS);
	geterror(bp);
	u.u_count = bp->b_resid;
}

void
dri_printf(va_alist)
va_dcl
{	va_list ap;
	char *fmt;

	va_start(ap);
	fmt = va_arg(ap, char *);
	(void) vprintf(fmt, ap);
	va_end(ap);
}
