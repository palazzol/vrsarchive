/*
 * TITLE: scsi.c -- XENIX SCSI Driver for iSBC 286/100
 *
 *	*** INTEL CORPORATION PROPRIETARY INFORMATION ***
 *
 *	This software is supplied under the terms of a license agreement or
 *	nondisclosure agreement with Intel Corporation and may be neither
 *	copied nor disclosed except in acordance with the terms of that
 *	agreement.
 *
 *	Brian J. McArdle	05/24/85
 *
 *	Modification History:
 *
 *	Vincent R. Slyngstad	11/01/85
 *	Modified for XEBEC controller, more internal documentation, a few bug
 *	fixes, many enhancements.
 *
 * ABSTRACT:
 *
 * This driver interfaces between the XENIX Operating System and the Intel
 * 8255 PPI (on the Intel iSBC 286/100 board) configured as a SCSI interface
 * connected to a XEBEC 1420 SCSI controller.
 *	XENIX <-> Driver <-> 8255 <-> XEBEC 1420 <-> Wini/Floppy
 *
 * The driver does not support parallel seek capability.
 * The XEBEC 1420 supports 4 units.  The first two are Winchester disk drives,
 * and the other two are floppy disk drives.
 *
 * Since SCSI makes cylinder 0 invisible, no physical address in cylinder
 * zero has a logical address.  All ioctl() operations in cylinder 0 are
 * secretly remapped onto logical address 0.  Since Cylinder 0 is not
 * available, the partition tables describe one less cylinder.  However,
 * the partition tables use logical addresses, so they make it look like
 * the cylinder was stolen from the far end of the drive.
 *
 * KERNEL INTERFACE ROUTINES:
 *		scsiinit - initializes driver variables, looks for units
 *		scsiopen - opens drives
 *		scsiclose - closes drives
 *		scsistrategy - queues I/O requests
 *		scsiintr - processes results of I/O operation
 *		scsiread - performs raw character I/O input from SCSI 
 *		scsiwrite - performs raw character I/O output to SCSI
 *		scsiioctl - allows XENIX process to format SCSI drive
 *
 * LOCAL DRIVER ROUTINES:
 *		scsi_start - initiates I/O operation
 *		scsi_error - reports error to console/kernel
 *		scsi_setup_dma - initializes the DMA controller for transfer
 *		scsi_dma_start - starts the DMA controller
 *		LIST THE OTHERS HERE
*/
char scsicopyright[] = "Copyright 1985 Intel Corporation";

#include "../h/param.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/a.out.h"
#include "../h/proc.h"
#include "../h/user.h"
#include "../h/systm.h"
#include "../h/mmu.h"
#include "../h/iobuf.h"
#include "../h/scsi.h"
#include "../h/adma.h"

extern struct scsicdrt scsicdrt[];	/* media table */
extern struct scsicfg scsicfg;		/* 218 configuration information */
extern struct scsiminor scsiminor[];	/* Table of minor numbers*/
extern int num_scsi;			/* number of controllers on bus */
extern char adma_ch0_lock;		/* ADMA channel mutual exclusion flag */

struct iobuf scsitab;			/* header to driver buffer's */
struct scsidev scsidev;			/* Device characteristic table */      
struct buf scsirbuf[MAX_UNITS];		/* Raw buffer headers for raw io */
struct user_ftk scsi_ftk[MAX_UNITS];	/* Format parameter buffers */
					/* (Augments scsirbuf) */

int  scsistrategy();			/* Forward references to strat */
int  m_dev;				/* currently busy device (maj/min) */
char scsi_cmd[6];			/* SCSI command block */
char scsi_mode[10];			/* data area for extended commands */
int  scsi_length;			/* data length for extended commands */
char scsi_sense[4];			/* data area for SCSI request sense */

/* Title:  scsiinit
 *
 * Abstract:  The kernel calls this routine at boot time to initialize driver 
 *
 * Calls:
 *
 * Called by: kernel
*/
scsiinit()
{
	register struct scsicfg *cp;
	register struct scsidev *dd;
	register unsigned i;		/* loop index to init units/minors */

#ifdef DEBUG
	printf("SCSI init called\n");
#endif
	/*Set up device table constant characteristics*/
	cp = &scsicfg;
	dd = &scsidev;
	/* make sure adma (82258) chip is initialized */
	admainit();
	/* fill in iopb adma channel stop command block */
	for (i=0; i < 12; i++)
		dd->iopb.adma_stop[i] = 0;
	/* initialize units */
	dd->bufh = &scsitab;
	/* configure 8255 input/output ports */
	outb(cp->ctrl, PORTA_INPUT);
	/* enable SCSI - set port C bit 6 = 0 */
	outb(cp->portc, ALL_OFF);
	/* reset SCSI bus */
	outb(cp->portc, RST_ON);
	delay(1); /* must wait at least 25 usec */
	outb(cp->portc, ALL_OFF);
	/* wait awhile then see if SCSI bus was reset */
	delay(20);
	if ((inb(cp->portb) & BSY) == BSY)
		dd->exists++;
	printf("SCSI interface %s \n",
		dd->exists ? "found" : "NOT found");
}

/* Title:  scsiopen
 *
 * Abstract:	This routine opens a partition on a drive unit.
 *
 *		Errors are returned to the kernel if the referenced unit was
 *		not found.
 *
 * Parameters:	dev - device number (minor component used)
 *		flag - write flag (not listed because ignored)
 *
 * Calls:
 * 
 * Called by: kernel
*/
scsiopen(dev)
int  dev;		/* device minor number */
{	register unsigned unit;		/* device unit to be opened */
	register struct buf *bp;	/* pointer to raw I/O header */
	register unsigned x;		/* interrupt mask */

#ifdef DEBUG
	printf("\nscsi open called");
#endif
	unit = UNIT(dev);
	if ((unit >= MAX_UNITS) || (scsidev.exists == FALSE)) {
		u.u_error = ENXIO;
		return;
	}
	/*
	 * We queue an 'open' operation to the strategy routine.  This
	 * causes scsistrategy() to issue an 'INIT_FORMAT' operation,
	 * describing to the controller the media format.  Cylinder 0
	 * does not necessarily have the desired info and we have to
	 * supply it.
	 *
	 * Note: Two opens for the same unit with different drtab entries
	 *	 WILL CONFUSE THE CONTROLLER.  The controller will try to
	 *	 use the format implied by the last open.  The only way
	 *	 around this is to initialize per I/O request, and I don't
	 *	 want to pay in performance for functionality rarely used.
	 *
	 *	 We cannot use physio() to handle mutex issues and talk to
	 *	 the strategy routine because when rootdev is opened, there
	 *	 is no user process, and physio requires one.  We fill out
	 *	 the buffer header with dummy buffer addresses and such,
	 *	 even though no input or output is being performed.
	*/
	bp = &scsirbuf[unit];
	x = splbuf();
	while (bp->b_flags&B_BUSY) {	/* wait for buffer */
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO+1);
	}
	splx(x);
	bp->b_flags = B_BUSY | B_OPEN;
	bp->b_dev = dev;
	bp->b_paddr = 0;		/* Dummy buffer address	*/
	bp->b_blkno = 0;		/* Dummy block number	*/
	bp->b_bcount = BSIZE;		/* Dummy transfer size	*/
	bp->b_error = 0;		/* No error		*/
	scsistrategy(bp);		/* Queue the command	*/
	iowait(bp);			/* Wait until done	*/
	bp->b_flags &= ~B_BUSY;		/* for next time	*/
	/*
	 * Mutex guaranteed below because we never sleep and
	 * close guards its clear operation on these flags.
	*/
	scsidev.d_unit[unit].popen |= (1 << PARTITION(dev));
}

/* Title:  scsiclose
 *
 * Abstract:	This routine closes a unit by reseting drive partition
 *		flags to closed.
 *
 * Parameters:	dev - device number(major & minor)
 *
 * Called by: kernel
 */
scsiclose(dev)
register dev_t dev;	/* device number */
{
	register struct scsiunit *dd;	/* unit device table */
	register unsigned x;

#ifdef DEBUG
	printf("\nscsi close called");	   
#endif
	/*
	 * Having to hack root and swap closes is a botch caused by
	 * the fact that when the kernel does I/O to the root and
	 * swap devices, there is no associated file descriptor,
	 * and hence no reference count, which should always have
	 * been non-zero.
	*/
	if (dev == rootdev)     /* never close root device */
		return;
	if (dev == swapdev)     /* never close swap device */
		return;
	dd = &scsidev.d_unit[UNIT(dev)];
	/*
	 *	We handshake with scsistart() to wait for I/O to be complete.
	 *	Unfortunately, we do not know the counts of pending requests
	 *	per partition (or even per drive), and so wait for the SCSI
	 *	controller to be completely idle.  On busy systems this will
	 *	take an obnoxiously long (infinite) time.  Other parts of the
	 *	system (i.e., haltsys) do make the assumption that all I/O
	 *	is complete before the return from close().
	*/
	x = splbuf();
	while ((scsitab.b_active != IO_IDLE) || (scsitab.b_actf != NULL)) {
#ifdef DEBUG
		if (scsitab.b_active != IO_IDLE)
			printf("\nscsiclose(): Device not idle -- sleeping");
		if (scsitab.b_actf != NULL)
			printf("\nscsiclose(): Queue not empty -- sleeping");
#endif DEBUG
		/*
		 * The strat routine will release the ADMA lock and
		 * wakeup on &adma_ch0_lock when no more I/O is queued.
		*/
		sleep((caddr_t) &adma_ch0_lock, PRIBIO+1);
	}
	dd->popen &= ~(1<< PARTITION(dev));
	splx(x);
#ifdef DEBUG
	printf("\nscsiclose(): returning after splx(%x)",x);
#endif DEBUG
}

/* Title:  scsistrategy
 *
 * Abstract:	This routine queues I/O requests from the kernel and
 *		starts I/O if controller idle.    
 *
 * Calls: scsi_start - starts I/O
 *	  iodone - kernel routine, returns buffer to free list
 *	  disksort - kernel routine, queues buffer headers by cylinder #
 *	  outb - kernel routine, outputs a byte to I/O port
 *
 * Called by:	kernel
 *		scsiread - indirectly thru physio for raw read
 *		scsiwrite - indirectly thru physio for raw write
 *		scsiioctl - to queue format track op
*/
scsistrategy(bp)
register struct buf *bp;    /* ptr to buf header containing request info */
{
	struct scsipartition *p;	/* ptr to media partition */
	struct scsicdrt *media;		/* ptr to media type in drive unit */
	daddr_t secno;			/* sector number where access begins */
	unsigned x;			/* saves old interrupt state */

#ifdef DEBUG
	printf("\nscsi Strat called bp: %x",bp);
#endif
	media = scsicdrt + DRTAB(bp->b_dev);
	p = &media->part[PARTITION(bp->b_dev)];
	if (bp->b_flags & (B_FORMAT|B_OPEN))
		secno = 0;		/* Not used */
	else
		secno = bp->b_blkno * (BSIZE/media->secsiz);
					/* blkno * sectors per block */
	/*
	 * Return an error if attempting to read/write beyond end of partition
	 * or attemting to write off the end of the partition.  Reading off the
	 * end is OK, and returns a short read (up to the end of the partition).
	*/
	if ((secno > p->p_nsec)
	|| ((secno == p->p_nsec) && !(bp->b_flags & B_READ))) {
		bp->b_flags |= B_ERROR;
		bp->b_error = ENXIO;
		iodone(bp);
		return;
	}
	/*
	 * Calculate the amount to the transfer that is beyond the end of the
	 * partition.  This causes an EOF if read extends beyond last sector.
	 * The number temporarily computed into resid is the secctor number
	 * of the last sector requested.
	*/
	bp->b_resid = secno + (bp->b_bcount+media->secsiz-1)/media->secsiz;
	if (bp->b_resid > p->p_nsec) {
		bp->b_resid = (bp->b_resid-p->p_nsec)*media->secsiz;
						/* Bytes beyond end	*/
		if (bp->b_resid == bp->b_bcount) {
			iodone(bp);		/* No I/O of zero bytes	*/
			return;
		}
	} else
		bp->b_resid = 0;		/* Can do all		*/
	/*
	 * Start the requested I/O.  First, we calculate the absolute
	 * sector number from the partition relative offset.
	*/
	secno += p->p_fsec;
	/*
	 * Still use cylinder to sort by, because disksort can't sort
	 * by block number.
	*/
	bp->b_cylin = secno / (media->spt * media->nhead);
	/*
	 * Mutex through disksort and test of active flag.  These are seperate
	 * mutex issues, but we don't bother to release mutex in between.  I
	 * don't believe the sum of times is significantly worse than either
	 * (taken individually) from the viewpoint of interrupt latency.
	*/
	x = splbuf();			/* disable interrupts */
	disksort(&scsitab, bp);		/* queue the request */
	if (!(scsitab.b_active & IO_BUSY))
		scsi_start();		/* start I/O if driver idle */
	splx(x);			/* restore interrupts */        
	return;
}

/* Title:  scsi_start 
 *
 * Abstract:	Called from scsiintr or scsistrategy, scsi_start starts
 *		a drive access operation.
 *
 * Calls:	scsi_setup_dma   - sets up DMA for access operation
 * 
 * Called by:	scsi_endop - after wrapup of prior request
 *		scsiintr - to resume I/O on next block
 *		scsiopen - to start I/O on any accumulated blocks
 *		scsistrategy - to start I/O on queued block
*/
scsi_start()
{
	struct scsicdrt *media;		/* drive media description */
	struct iobuf *bufh;		/* buffer header containing request */
	struct buf *bp;			/* next buf header on request list */
	unsigned x;			/* interrupt mask */

#ifdef DEBUG
	printf("\nscsi start called, ");
#endif
	bufh = &scsitab;
	/*
	 * Mutex the test for IO_IDLE with I/O pending. 
	*/
	x = splbuf();
	if ((bufh->b_active == IO_IDLE) && ((bp = bufh->b_actf) != NULL)) {
		/* start I/O if controller idle */
		bufh->b_active = IO_BUSY;		/* lock buffer */
		/* 
		 * Done with mutex for IO_IDLE check.  State is indeed IO_IDLE,
		 * with I/O pending.
		*/
		splx(x);
		m_dev = bp->b_dev;
#ifdef DEBUG
		printf("for unit %x; bp %x", UNIT(m_dev), bp);
#endif
		media = scsicdrt + DRTAB(m_dev);
		/*
		 * It is safe to muck with these variables since we have
		 * marked ourselves IO_BUSY and possible interrupt time calls
		 * to scsistart() will not issue commands.  NO OTHER routine
		 * should build commands while IO_BUSY is set OR interrupts
		 * are enabled.
		*/
		scsidev.state = 0;
		scsi_cmd[1] = UNIT(m_dev) << 5;
		if (bp->b_flags & B_OPEN) {
			scsi_init_format(media);
		} else if (bp->b_flags & B_FORMAT) {
			scsi_format(media);
		} else if (bp->b_flags & B_READ) {
			scsi_cmd[0] = READ;
			scsi_read_write(bp, media);
			scsi_setup_dma(bp->b_paddr, READ, bp->b_bcount);
		} else {	/* write operation */
			scsi_cmd[0] = WRITE;
			scsi_read_write(bp, media);
			scsi_setup_dma(bp->b_paddr, WRITE, bp->b_bcount);
		}
		scsi_select();
		return;
	}
	/*
	 * State is not IO_IDLE, or possibly IO_IDLE with
	 * empty request queue.
	*/
	if (bufh->b_active == IO_IDLE) {
		splx(x);
		/*
		 * Done with mutex from IO_IDLE test.  State is
		 * definitely IO_IDLE with empty request queue.
		 * We do not need to mutex the clearing of
		 * the adma lock due to the single thread nature of
		 * the kernel.  We want to release the mutex before
		 * calling wakeup() as part of our contribution to
		 * reducing interrupt latency.
		*/
		adma_ch0_lock= 0; /* release ADMA channel */
		wakeup((caddr_t) &adma_ch0_lock);
#ifdef DEBUG
		printf("\nno requests, releasing ADMA channel lock");
#endif
	} else {
		splx(x);
		/*
		 * Done with mutex from IO_IDLE test.  The state is
		 * not IO_IDLE.  Since the driver is written so that
		 * calls to scsistart() will not happen while the
		 * controller is not idle, this indicates a driver
		 * malfunction of some sort.  We ignore the extra
		 * call but warn that the driver messed up.
		*/
		printf("\nSCSI device %x busy", m_dev);
	}
}

/* Title:  scsiintr
 *
 * Abstract: This routine is called from kernel upon receipt of an interrupt
 *	     signal from SCSI device.  It reads the SCSI bus phase bits to
 *	     determine what action must be taken.
 *
 *  Calls: scsi_dma_start - start DMA operation
 *	   scsi_error - prints out error message and updates buffer header
 *	   iodone - kernel routine, puts buffer on free list    
 *
 * Called by: kernel
 */
scsiintr()
{
	unsigned char phase;			/* SCSI bus phase lines */
	unsigned x;			/* SHOULDN'T NEED THIS */

#ifdef DEBUG
	printf("\nscsi intr called");
#endif
	x = splbuf();			/* SHOULDN'T NEED THIS */
	phase= inb(scsicfg.portb); /* read SCSI bus state */
	if ((phase & IO) == 0) {  /* +I/O signal asserted */
		/* DATA IN, STATUS, or MSG IN phase */
		/* configure 8255 for data input */
		outb(scsicfg.ctrl, PORTA_INPUT);
		if ((phase & CD) != 0) { /* +C/D is NOT asserted */
#ifdef DEBUG
			printf(" READ");
#endif
			scsi_dma_start();	/* DATA IN */
		} else if ((phase & MSG) != 0) {
#ifdef DEBUG
			printf(" STATUS  ");
			printf(" 82258 GSR = %x", in(ADMA_GSR));
#endif
			scsi_r_status();	/* STATUS */
		} else {
#ifdef DEBUG
			printf(" MSG IN");
#endif
			scsi_r_msg();		/* MSG IN */
			scsi_endop();
		}
	} else {/* +I/O is NOT asserted */
		/* DATA OUT, COMMAND, or MSG OUT phase */
		/* configure 8255 for data output */
		outb(scsicfg.ctrl, PORTA_OUTPUT);
		if ((phase & CD) != 0) {
#ifdef DEBUG
			printf(" WRITE");
#endif
			scsi_write_data();	/* DATA OUT */
		} else if ((phase & MSG) != 0) {
#ifdef DEBUG
			printf(" COMMAND");
#endif
			scsi_s_cmd();	/* COMMAND */
		} else
			panic("SCSI bus error (MSG OUT)");
	}
	splx();			/* SHOULDN'T NEED THIS */
}

/* Title:  scsiread()
 *
 * Abstract:  This routine allows direct physical input from a floppy device.
 *
 * Calls: Physio - kernel routine, performs serial output
 *	  scsistrategy (indirectly thru physio)
 *
 * Called by: kernel
 */
scsiread(dev)
dev_t dev;		/* device number */
{
#ifdef DEBUG
	printf("\nscsi Entered Read in Driver");
#endif
	physio(scsistrategy, &scsirbuf[UNIT(dev)], dev, B_READ);
}


/* Title:  scsiwrite()
 *
 * Abstract:  This routine allows direct physical output to a floppy.
 *
 * Calls: Physio - kernel routine, performs serial output
 *	  scsistrategy (indirectly thru physio)
 *
 * Called by: kernel
 */
scsiwrite(dev)
dev_t dev;	/* device number */
{
#ifdef DEBUG
	printf("\nscsi Entered Write in Driver");
#endif
	physio(scsistrategy, &scsirbuf[UNIT(dev)], dev, B_WRITE);
}

/* Title:  scsiioctl
 *
 * Abstract:  This is a special function routine used to format a track or
 *		cylinder on disk.  Availability of the the raw buffer header
 *		assigned the unit causes mutual exclusion among processes
 *		performing raw I/O or also formatting.
 *
 * Calls:  physio - Checks request and queues it
 *	   copyin - kernel routine that copies data from user area
 *	   splx - kernel routine which enables interrupt previously disabled
 *	   splbuf - kernel routine disables all interrupts except timer
 *
 *  Called by: kernel
*/
scsiioctl(dev, cmd, cmdarg)
dev_t dev;		/* device number */
int cmd;		/* command */
faddr_t cmdarg;		/* user structure with parameters */
{	
	register struct scsiunit *dd;	/* unit device table */
	struct scsicdrt *media;		/* ptr to media descr */
	unsigned x;			/* saves old interrupt state */
	unsigned unit;			/* unit being formatted */
	long track, alt;			/* track, alternate */

#ifdef DEBUG
	printf("\nscsi entered ioctl");
#endif
	if (cmd != IOC_FMT) {
		u.u_error = ENXIO;	/* only FORMAT command supported */
		return;
	}
	unit = UNIT(dev);
	dd = &scsidev.d_unit[unit];
	media = &scsicdrt[DRTAB(dev)];
	/*
	 * Get the format parameters.  Note that we must mutex scsi_ftk[unit].
	*/
	x = splbuf();
	while (dd->flags & U_FORMAT)
		sleep((caddr_t) dd, PRIBIO+1);
	dd->flags |= U_FORMAT;
	splx(x);
	copyin(cmdarg, (caddr_t)&scsi_ftk[unit], sizeof scsi_ftk);
	if (((scsi_ftk[unit].f_type&0xFF) != FORMAT_DATA)
	&&  ((scsi_ftk[unit].f_type&0xFF) != FORMAT_BAD)) {
		dd->flags &= ~U_FORMAT;	/* Release Mutex		*/
		return;
	}
	if (scsi_ftk[unit].f_trck >= media->nhead*media->ncyl) {
		u.u_error = ENXIO;	/* FORMAT beyond end of drive	*/
		dd->flags &= ~U_FORMAT;	/* Release Mutex		*/
		return;
	}
	/*
	 * We use physio() to handle the mutex issues and talk to the
	 * strategy routine.  We set up u.u_offset, u.u_count, etc. so
	 * physio() can pretend input or output is involved.
	*/
	u.u_offset = 0;
	u.u_count = BSIZE;
	u.u_base = sotofar(u.u_procp->p_fdsel, 0);
	if ((scsi_ftk[unit].f_type&0xFF) == FORMAT_BAD) {
		/*
		 * Get the track number of the bad track and the alternate.
		*/
		track = scsi_ftk[unit].f_trck;
		alt  = (scsi_ftk[unit].f_pat[1]&0xFF) << 8;
		alt += (scsi_ftk[unit].f_pat[0]&0xFF);
		alt *= media->nhead;
		alt += (scsi_ftk[unit].f_pat[2]&0xFF);
		/*
		 * Format alternate as data track.  If it is already formatted
		 * as an alternate, XEBEC will refuse to re-use it.
		*/
		scsi_ftk[unit].f_trck = alt;
		scsi_ftk[unit].f_type = FORMAT_DATA;
		physio(scsistrategy, &scsirbuf[unit], dev, B_FORMAT);
		/*
		 * Now put everything back and fall through to assign the
		 * alternate and mark the bad track.
		*/
		scsi_ftk[unit].f_trck = track;
		scsi_ftk[unit].f_type = FORMAT_BAD;
	}
	physio(scsistrategy, &scsirbuf[unit], dev, B_FORMAT);
	/*
	 * Now we release the mutex on scsi_ftk[unit].
	*/
	dd->flags &= ~U_FORMAT;
	wakeup((caddr_t)dd);
}

/* Title:  scsi_endop
 *
 * Abstract:	This routine processes the status returned by the SCSI
 *		device. If an error is returned, the device is queried
 *		for extended status.
 *
 * Calls:	scsi_stop_dma - make sure 82258 is in good state	
 *		scsi_setup_dma - setup dma for multiple command operation
 *		scsi_select - start command sequence
 *		iodone - return completed I/O buffer to kernel
 *
 * Called by:	scsiintr
 *
 */
scsi_endop()
{
	struct buf *bp;
	struct iobuf *bufh;		/* driver I/O param block */

	bufh = &scsitab;
	bp = bufh->b_actf;
	/*
	 * An operation has completed.  We don't overlap commands while
	 * previous I/O is in progress, so this means that not only is
	 * the controller idle, but scsistart() is not in the process of
	 * building a command.  Since we leave interrupts masked (and
	 * the queue marked IO_BUSY, for that matter), we cannot get
	 * blown away by scsistart() while we are in progress.  All of
	 * which is to say that it is OK here (and only here) to break
	 * the usual rule that only scsistart() should issue commands.
	 * This allows us to send 'get status' commands on errors.
	*/
#ifdef DEBUG
	printf("\n	entered scsi_endop state= %x\n", scsidev.state);
#endif
	if ((scsidev.state & CHECK) != 0) {
		/*
		 * CHECK bit was set in the status byte, implying an error.
		*/
		if ((scsidev.state & SENSE) == 0) {
			/*
			 * Send 'get sense' command to the controller, since
			 * we haven't done so yet.
			*/
			scsidev.state |= SENSE;
			scsi_stop_dma(); /* stop 82558 */
			scsi_setup_dma(((long)dscraddr(KDS_SEL)+(long)scsi_sense),
					 READ, sizeof scsi_sense);
			scsi_cmd[0] = REQ_SENSE;
			scsi_cmd[1] = UNIT(m_dev) << 5;
			scsi_cmd[2] = scsi_cmd[3] = scsi_cmd[5] = 0;
			scsi_cmd[4] = sizeof scsi_sense;
			scsi_select();
			return;
		} else {
			/*
			 * We got sense data, now notify user.
			*/
			scsi_error(bp, &scsitab);
		}
	} else {
		/*
		 * I/O completed, dequeue the finished request and mark
		 * it as completed.
		*/
		scsidev.state = 0;
		bufh->b_actf = bp->av_forw;
#ifdef DEBUG
		printf("\nscsi_endop calling iodone bp= %x, bp->av_forw= %x\n",
			bp, bp->av_forw);
#endif
		iodone(bp);
		bufh->b_active &= ~IO_BUSY;
	}
	/*
	 * Start next I/O (if any).
	*/
	scsi_start();
}

/* Title:  scsi_error
 *
 * Abstract:	This routine calls a kernel routine to print out an appropriate
 *		error message consisting of block number, command attempted
 *		and status register.  The I/O block is returned to the kernel
 *		with error status flagged.
 *
 * Calls:	deverr - kernel routine, outputs error message
 *		iodone - kernel routine which returns buffer to free list
 *
 * Called by:  scsi_endop
 */
scsi_error(bp, bufh)
register struct buf *bp;	/* buffer header pointer */
register struct iobuf *bufh;	/* static buffer header */
{
	
#ifdef DEBUGX
	/*
	 * I broke the printf calls down to be as simple as possible because
	 * DEBUGX caused the driver to malfunction.  I think the issue is space
	 * on the kernel stack.
	*/
	printf("\nentered scsi_error\n");
	printf("scsi_sense = %x", scsi_sense[0]&0xFF);
	printf(" %x", scsi_sense[1]&0xFF);
	printf(" %x", scsi_sense[2]&0xFF);
	printf(" %x\n", scsi_sense[3]&0xFF);
#endif
	/*
	 *	Output error message.  The cmd= field really prints the
	 *	drive number.
	*/
	deverr(bufh, scsi_sense[1] >> 5, scsi_sense[0] & 0x3F, "SCSI");
	/* flag block with error status */
	bp->b_flags |= B_ERROR;
	bp->b_error = EIO;
	bufh->b_actf = bp->av_forw;
#ifdef DEBUG
	printf("calling iodone bp= %x, bp->av_forw= %x\n",
		bp, bp->av_forw);
#endif
	iodone(bp);
	scsitab.b_active &= ~IO_BUSY;
}

/* Title:  scsi_r_msg
 *
 * Abstract: This routine programs the ADMA controller and gains
 *		access to the DMA control register.
 *
 * Calls:	inb, outb - kernel physical I/O routines
 *
 * Called by:	scsiintr
*/
scsi_r_msg()
{
	char msg;	/* message byte from controller */

#ifdef DEBUG
	printf("      entered SCSI rcv_msg\n");
#endif
	msg= inb(scsicfg.porta);
	outb(scsicfg.portc, ALL_OFF);	/* idle bus */
	/* only support the command complete message */
	if (msg != 0)
		printf("SCSI unsupported message = %x\n", msg);
}

/* Title:  scsi_r_status
 *
 * Abstract: This routine receives the status byte from the SCSI controller
 *
 * Calls:	inb- kernel physical I/O routines
 *
 * Called by:	scsiintr
 */
scsi_r_status()
{
	unsigned status;	/* command completion status byte */

	status= inb(scsicfg.porta);
#ifdef DEBUG
	printf("      entered SCSI rcv_status status= %x\n", status);
#endif
	if ((scsidev.state & SENSE) == 0)
		scsidev.state |= (status & (CHECK | BUSY));
}

/* Title:  scsi_select
 *
 * Abstract:	This routine selects the SCSI target designated by 
 *		the minor device number in the driver variable "m_dev".
 *
 * Calls:	inb, outb - kernel physical I/O routines
 *
 *	This driver assumes that it is the only SCSI initiator on the bus.
 *	It does not put its host id on the data lines during target selection.
 *	There is an implicit assumption that a call to inb will take at least
 *	100 nanoseconds.  This will probably be true for a while :-), and the
 *	granularity for delays is huge relative to this, so none were coded.
 *
 * Called by:	scsi_start 
 */
scsi_select()
{
#ifdef DEBUG
	printf("      entered SCSI select\n");
#endif
	/* reset 8255 */
	outb (scsicfg.ctrl, PORTA_OUTPUT);
	/* start arbitration if its there */
	outb(scsicfg.portc, ARB_START);
	/* wait for SCSI bus free */
	while ((inb(scsicfg.portb) & (BSY|SEL)) != (BSY|SEL))
		;
	/* put target id on data bus */
	outb(scsicfg.porta, (1 << TARGET(m_dev)));
	/* assert SEL bus line */
	outb(scsicfg.portc, SEL_ON);
	/* wait for acknowledge from target */
	while ((inb(scsicfg.portb) & BSY) != 0)
		;
	/* deassert SEL */
	outb(scsicfg.portc, ALL_OFF);
}

/* Title:  scsi_s_cmd
 *
 * Abstract:	This routine sends a SCSI command to the selected target.
 *
 * Calls	inb, outb - kernel physical I/O routines
 *
 * Called by:	scsiintr
 */
scsi_s_cmd()
{
	register int i;

#ifdef DEBUGX
	printf("\n      entered SCSI send_cmd");
#endif
	for (i=0; i < sizeof scsi_cmd; i++) {
		while ((inb(scsicfg.portb) & REQ) != 0)
			;
#ifdef DEBUGX
	printf(" %x",scsi_cmd[i] & 0xFF);
#endif
		outb(scsicfg.porta, scsi_cmd[i]);
		/*
		 * How does ACK- get toggled here?  According to my manual,
		 * I'm supposed to assert ACK-, wait until the controller
		 * removes REQ-, and remove ACK-.  On the other hand, this
		 * mess seems to be working.
		*/
	}
	/*
	 * The next line is black magic required when using interrupts
	 * with the SCSI interface.  If you leave this out, the state
	 * machine does not enable interrupts.  See INTERRUPT DRIVEN
	 * OPERATION and the SCSI State Machine Diagram in Appendix B
	 * of the iSBC 286/100 Single Board Computer User's Guide.
	*/
	outb(scsicfg.portb, 0x20);
#ifdef DEBUGX
	printf(" \n");
#endif
}

/* Title:  scsi_setup_dma
 *
 * Abstract: This routine programs the ADMA controller and gains
 *		access to the DMA control register.
 *
 * Calls:	sleep - kernel routine
 *		out, outb - kernel physical I/O routines
 *		splx, splbuf - kernel interrupt masking routines
 *
 * Called by:	scsi_start
*/
scsi_setup_dma(b_addr, mode, count)
paddr_t b_addr;			/* physical address of buffer */
char mode;			/* read/write flag */
unsigned count;			/* transfer count */
{
	unsigned x;
	register struct scsiiopb *iopb;

#ifdef DEBUG
	printf("\nEntered SCSI setup_dma");
#endif
	iopb = &scsidev.iopb;
	iopb->adma_count = (long)count;
	if (mode == WRITE)
	{
		iopb->adma_src_ptr = b_addr;
		iopb->adma_dst_ptr = (long)(scsicfg.porta);
		iopb->adma_command = ADMA_WRITE;
	}
	else
	{
		iopb->adma_src_ptr = (long)(scsicfg.porta);
		iopb->adma_dst_ptr = b_addr;
		iopb->adma_command = ADMA_READ;
	}
	/* gain ownership of the DMA multiplexor register	*/
	/* we need ADMA channel 0				*/
	/*
	 * WE SHOULD DO A MORE RIGOROUS CHECK that there is no
	 * race with close, which also sleeps on &adma_ch0_lock.
	*/
	x = splbuf();
	if (adma_ch0_lock != SCSI_ADMA_ID) {
		while (adma_ch0_lock != 0)
			sleep(&adma_ch0_lock, PRIBIO+1);
	}
	outb(DMA_CONTROL, (DMA_SCSI | (inb(DMA_CONTROL)&CH0_MASK)));
	adma_ch0_lock= SCSI_ADMA_ID;
	splx(x);
}

/* Title:  scsi_init_format
 *
 * Abstract:	This routine fills in the data structures for the XEBEC 1420
 *		INIT_FORMAT command.
 *
 * Calls:
 *
 * Called by:	scsi_start
*/
scsi_init_format(media)
register struct scsicdrt *media;	/* ptr to media type in drive unit */
{
	/*
	 * Format specification is heavily dependent on the the specific
	 * SCSI controller.  This routine is implemented for the XEBEC
	 * 1420 controller and standard drives.  IT WILL NOT WORK on other
	 * drives or other SCSI controller boards.  This is most unfortunate.
	*/
	scsi_cmd[0] = INIT_FORMAT;
	scsi_cmd[2] = 0;
	scsi_cmd[3] = 0;
	scsi_cmd[4] = 0;
	scsi_cmd[5] = 0;
	/*
	 * Set up init format parameter list.  Multiple drtab
	 * entries refer to the same drive.  WE FALL ON OUR
	 * FACE IF YOU OPEN USING TWO DRTABS on a single unit.
	*/
	scsidev.state |= EXTND_CMD;
	scsi_length = 10;		/* 10 bytes of extended data	*/
	scsi_mode[0] = media->ncyl >> 8;
	scsi_mode[1] = media->ncyl & 0xFF;	/* # Cylinders		*/
	scsi_mode[2] = media->nhead;		/* # Heads		*/
	scsi_mode[3] = media->fmt3;		/* Format parameters	*/
	if (media->pcomp == 0) {		/* Floppy?		*/
		if (media->fmt3 == FM)		/* Yes, what format?	*/
			scsi_mode[4] = 0x01;	/* Sector size (128)	*/
		else
			scsi_mode[4] = 0x03;	/* Sector size (512)	*/
		scsi_mode[5] = 15;	/* 240Ms until head unloads	*/
		scsi_mode[6] = 80;	/* 800Ms motor start time	*/
		scsi_mode[7] = 15;	/* 30Ms head load time		*/
		scsi_mode[8] = 20;	/* 2 seconds until motor off	*/
		scsi_mode[9] = 0;	/* Must be 0			*/
	} else {
		scsi_mode[4] = 0x2;	/* Sector size (512)		*/
		scsi_mode[5] = media->ncyl >> 8;
		scsi_mode[6] = media->ncyl & 0xFF;
					/* Reduce Write Current		*/
		scsi_mode[7] = media->pcomp >> 8;
		scsi_mode[8] = media->pcomp & 0xFF;
					/* Write Precompensation	*/
		scsi_mode[9] = 11;	/* Bits to correct (max 11)	*/
	}
}

/* Title:  scsi_format
 *
 * Abstract:	This routine fills in the data structures for the format
 *		command.
 *
 * Called by:	scsi_start
*/
scsi_format(media)
register struct scsicdrt *media;	/* ptr to media type in drive unit */
{
	register unsigned unit;
	long track;

	/*
	 * Formatting is heavily dependent on the the specific SCSI
	 * controller.  This routine is implemented for the XEBEC 1420
	 * controller and ST506 drives.  IT WILL PROBABLY NOT WORK on
	 * other drives or other SCSI controller boards.  This is most
	 * unfortunate.
	*/
	unit = UNIT(m_dev);
	/*
	 * Build the SCSI FORMAT command.
	 *
	 * There is a botch in the XEBEC manual, and the correct parameter
	 * block for a 'format tracks' command is not shown.  The correct
	 * parameter block looks like this:
	 *
	 *	Byte 0	0 0 0 0 0 1 1 0
	 *	Byte 1	0 d d High Addr
	 *	Byte 2	Middle Address
	 *	Byte 3	 Low   Address
	 *	Byte 4	0 0 0 Interleave
	 *	Byte 5	r 0 b 0 0 0 0 0
	 *	Followed by track count high, then track count low.
	 *
	 * The other piece of logic needed is to determine if format wants
	 * to format an alternate.  Right now bad tracks are lethal.  There
	 * will probably need to be software to remap tracks when 'access to
	 * bad track' errors are seen (GROT).
	*/
	scsidev.state |= EXTND_CMD;	/* scsi_mode[] is releavent	*/
	scsi_cmd[1] = unit << 5;
	/*
	 *	Note that here, as in other places, drives > 1Gb give
	 *	XEBEC fits, since only 21 bits are allowed in a sector
	 *	number.
	*/
	track = address(media, (long)scsi_ftk[unit].f_trck);
	scsi_cmd[1] += track >> 16;
	scsi_cmd[2]  = track >> 8;
	scsi_cmd[3]  = track;
	/*
	 *	Format it as a data track or assign the alternate.
	*/
	if ((scsi_ftk[unit].f_type&0xFF) == FORMAT_DATA) {
		scsi_cmd[0] = FORMAT;	/* Format a data track		*/
		scsi_length = 2;	/* 2 bytes of extended data	*/
		scsi_mode[0] = 0;	/* High track count is zero	*/
		scsi_mode[1] = 1;	/* Format one data track	*/
	} else {
		scsi_cmd[0] = FORMATALT;/* Assign an alternate track	*/
		/*
		 *	The following fields encode the address of the
		 *	alternate track to be assigned:
		 *
		 *	scsi_ftk[unit].f_pat[0] contains low  cylinder number
		 *	scsi_ftk[unit].f_pat[1] contains high cylinder number
		 *	scsi_ftk[unit].f_pat[2] contains head number
		*/
		track  = (scsi_ftk[unit].f_pat[1]&0xFF) << 8;
		track += (scsi_ftk[unit].f_pat[0]&0xFF);
		track *= media->nhead;
		track += (scsi_ftk[unit].f_pat[2]&0xFF);
		track  = address(media, track);
		/*
		 *	Now fill in scsi_mode to point to that track.
		*/
		scsi_length = 3;	/* 2 bytes of extended data	*/
		scsi_mode[0] = track >> 16;
		scsi_mode[1] = track >> 8;
		scsi_mode[2] = track;
	}
	/*
	 * Note that in general, the choices of interleave used with the
	 * 215 will be AWFUL due to the fact that we cannot provide 1K
	 * sectors.  Try around 3/2X or a little less, because those 512
	 * byte sectors are smaller and take less time to spin by.
	*/
	scsi_cmd[4] = scsi_ftk[unit].f_intl;
				/* Use interleave requested by user	*/
	scsi_cmd[5] = 0; /* No Retries */
}

/* Title:  scsi_read_write
 *
 * Abstract:	Fill in the SCSI command block for a read/write request.
 *
 * Called by:	scsistart
*/
scsi_read_write(bp, media)
register struct scsicdrt *media;	/* ptr to media type in drive unit */
register struct buf *bp;    /* ptr to buf header containing request info */
{
	daddr_t block;			/* SCSI device block number */

	block = (bp->b_blkno*(BSIZE/media->secsiz))
	      + media->part[PARTITION(m_dev)].p_fsec;
	/*
	 * Probably should insure block <= 2^20...
	 * LARGER BLOCK NUMBERS WILL CORRUPT THE PARAMETER
	 * BLOCK, specifically the drive number.  Drives
	 * > 500Mb c
	*/
	scsi_cmd[1] += block >> 16;	/* hope block >= 0 */
	scsi_cmd[2]  = block >> 8;
	scsi_cmd[3]  = block;
	/*
	 * Probably should insure < 255 sectors requested...
	 * REQUESTS > 255 SECTORS (>= 128K BYTES) WILL silently
	 * TAKE THE REQUEST SIZE MODULO 256 sectors.
	*/
	scsi_cmd[4]  = (unsigned)(bp->b_bcount/ media->secsiz);
	scsi_cmd[5]  = 0; /* Non-portable fields */
}

/* Title:  scsi_dma_start
 *
 * Abstract:	This routine starts ADMA channel 0 for a SCSI data
 *		transfer phase.
 *
 * Calls:	out, outb - kernel physical I/O routines
 *
 * Called by:	scsiintr
*/
scsi_dma_start()
{
	long p_addr;		/* 24 bit address of iopb struct */

#ifdef DEBUG
	printf("Entered SCSI start_dma\n");
#endif
	p_addr = (long)dscraddr(KDS_SEL) + (long)(&scsidev.iopb);
	/* load ADMA Command Pointer Register with address of iopb */
	out(ADMA_CPRL0, LOWWORD(p_addr));
	out(ADMA_CPRH0, HIGHWORD(p_addr));
	/* start ADMA Channel 0 */
	outb(ADMA_GCR, START_CH0);
}

/* Title:  scsi_stop_dma
 *
 * Abstract:	This routine stops ADMA channel 0 for a SCSI data
 *		transfer phase.
 *
 * Calls:	out, outb - kernel physical I/O routines
 *
 * Called by:	scsi_endop
 */
scsi_stop_dma()
{
	out(ADMA_GCR, STOP_CH0);
}

/* Title:  scsi_write_data
 *
 * Abstract:	This routine starts the dma for a data write or directly
 *		handles the transfer for a init format command.
 *
 * Calls:	out, outb - kernel physical I/O routines
 *		scsi_dma_start
 *
 * Called by:	scsiintr  
*/
scsi_write_data()
{
	int i;

	if (scsidev.state & EXTND_CMD) {
		/*
		 * Can this hack be done away with?  That is, is it possible
		 * to DMA the scsi_mode array just as we would regular buffer
		 * data?
		*/
#ifdef DEBUG
		printf("	sending INIT/FORMAT data\n");
#endif
		for (i=0; i < scsi_length; i++) {
#ifdef DEBUGX
			printf(" %x", scsi_mode[i]&0xFF);
#endif
			while ((inb(scsicfg.portb) & REQ) != 0)
				;
			outb(scsicfg.porta, scsi_mode[i]);
		}
		scsidev.state &= ~EXTND_CMD;
		return;
	} else 
		scsi_dma_start();
}
