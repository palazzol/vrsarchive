/*
 * TITLE: scsi.c -- XENIX SCSI Driver for iSBC 286/100
 *
 *		Intel Corporation
 *		ISO
 *		2402 West Beardsley Road
 *		Phoenix, Arizona  85027
 *
 *	Date: 05/24/1985
 *
 *	Programmer:	Brian J. McArdle
 *
 *	Modification History:
 *
 *	Vincent R. Slyngstad	11/1/85
 *	Modified for XEBEC controller, more internal documentation, a few bug
 *	fixes.
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
 * BUGS:
 *	Formatting needs enhancement to make the format utility work as
 *	expected.  Currently the whole drive will be formatted as many
 *	times as there are cylinders, which is VERY unacceptable.
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
*/

	/*******************************************************\
	|							|
	|       INTEL CORPORATION PROPRIETARY INFORMATION       |
	|							|
	|       This software is supplied under the terms of a  |
	|       license agreement or nondisclosure agreement    |
	|       with Intel Corporation and may neither be       |
	|       copied nor disclosed except in acordance with   |
	|       the terms of that agreement.                    |
	|							|
	\*******************************************************/

char scsicopyright[] = "Copyright 1985 Intel Corporation";

#include "../h/param.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/a.out.h"
#include "../h/user.h"
#include "../h/systm.h"
#include "../h/mmu.h"
#include "../h/iobuf.h"
#include "../h/scsi.h"
#include "../h/ADMA.h"

extern struct scsicdrt scsicdrt[];	/* media table */
extern struct scsicfg scsicfg;		/* 218 configuration information */
extern struct scsiminor scsiminor[];	/* Table of minor numbers*/
extern int num_scsi;			/* number of configured units on bus */
extern char adma_ch0_lock;		/* ADMA channel mutual exclusion flag */

struct iobuf scsitab;			/* header to driver buffer's */
struct scsidev scsidev;			/* Device characteristic table */      
struct buf scsirbuf[MAX_UNITS];		/* Raw buffer headers for raw io */

int m_dev;				/* currently busy device */
char scsi_cmd[10];			/* SCSI command block */
char scsi_mode[22];			/* data area for SCSI mode select */
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
	printf("scsi init called\n");
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
 *		Errors are returned to the kernel if the reference unit was
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
{
	register unsigned unit;		/* device unit to be opened */

#ifdef DEBUG
	printf("\nscsi open called");
#endif
	unit = UNIT(dev);
	if ((unit >= num_scsi) || (scsidev.exists == FALSE))
		u.u_error = ENXIO;
	else {
		/*
		 * Mutex guaranteed because we never sleep and
		 * close guards its clear operation).
		*/
		scsidev.d_unit[unit].flags |= OPEN;
		scsidev.d_unit[unit].popen |= (1 << PARTITION(dev));
					/* mark partition opened */
	}
}

/* Title:  scsiclose
 *
 * Abstract:  This routine closes a unit by reseting drive partition flags to
 *		closed.  In addtion, if this is the last open partition on the
 *		drive, the unit open flag is set to NO.
 *
 * Parameters: dev - device number(major & minor)
 *
 * Called by: kernel
 */
scsiclose(dev)
register dev_t dev;	/* device number */
{
	register struct scsiunit *dd;	/* unit device table */
	extern dev_t rootdev;		/* root device id */
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
	x = spl6();
	while ((scsitab.b_active != IO_IDLE) || (scsitab.b_actf != NULL)) {
		/*
		 * The strat routine will release the ADMA lock and
		 * wakeup on &adma_ch0_lock when no more I/O is queued.
		 * We sleep on this address and wake when the device
		 * queue is empty.
		*/
		sleep((caddr_t) &adma_ch0_lock, PRIBIO+1);
	}
	dd->popen &= ~(1<< PARTITION(dev));
	if (dd->popen == FALSE)
		dd->flags &= ~OPEN;
	splx(x);
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
  register struct scsiunit *dd;	/* ptr to unit device info */
  struct scsipartition *p;	/* ptr to media partition */
  struct scsicdrt	*media;		/* ptr to media type in drive unit */
  daddr_t secno;			/* sector number where access begins */
  unsigned x;			/* saves old interrupt state */

#ifdef DEBUG
  printf("\nscsi Strat called bp: %x",bp);
#endif
  dd = &scsidev.d_unit[UNIT(bp->b_dev)];
  media = scsicdrt + DRTAB(bp->b_dev);
  p = &media->part[PARTITION(bp->b_dev)];
  secno = bp->b_blkno;
  if ((bp->b_flags & B_FORMAT) == FALSE)
    secno *= (BSIZE/media->secsiz);	/* blkno * sectors per block */
  /* Return an ERROR if 
   * 	1) unit not open,
   *	2) attempting to read/write beyond end of partition.
  */
  if (((dd->flags & OPEN) == FALSE) || (secno > p->p_nsec)
  || ((secno==p->p_nsec) && ((bp->b_flags&B_WRITE)||(bp->b_flags&B_FORMAT)))) {
    /*
     * This is probably not the right error return for all of the
     * above conditions.  Buyer beware for now.
    */
    bp->b_flags |= B_ERROR;
    bp->b_error = ENXIO;
    iodone(bp);
    return;
  }
  /* return EOF if read or write extends beyond last sector */
  if ((secno == p->p_nsec)
  ||  ((secno+(bp->b_bcount+media->secsiz-1)/media->secsiz) > p->p_nsec)) {
    /* set number of transfer bytes to zero */
    bp->b_resid = bp->b_bcount;
    iodone(bp);
    return;
  }
  /*
   * No error -- start I/O
  */
  secno += p->p_fsec;
  /*
   * still use cylinder to sort by, disksort can't sort
   * by block number.
   */
  bp->b_cylin = secno / (media->spt * media->nhead);
  /*
   * Mutex through disksort and test of active flag.  These are seperate
   * mutex issues, but we don't bother to release mutex in between.  I
   * don't believe the sum of times is significantly worse than either
   * (taken individually) from the viewpoint of interrupt latency.
  */
  x = spl6();			/* disable interrupts */
  disksort(&scsitab, bp);	/* queue request */
  if ((scsitab.b_active & IO_BUSY) == FALSE)
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
 * Called by:	scsi_error - after reporting error on last request
 *		scsiintr - to resume I/O on next block
 *		scsiopen - to start I/O on any accumulated blocks
 *		scsistrategy - to start I/O on queued block
 */
scsi_start()
{
	struct scsicdrt *media;		/* drive media description */
	struct iobuf *bufh;		/* buffer header containing request */
	struct buf *bp;			/* next buf header on request list */
	char mode;			/* access mode (READ or WRITE) */
	daddr_t block;			/* scsi device block number */
	unsigned x;			/* interrupt mask */

#ifdef DEBUG
	printf("\nscsi start called, ");
#endif
	bufh = &scsitab;
	x = spl6();
	/*
	 * Mutex the test for IO_IDLE with I/O pending. 
	*/
	if ((bufh->b_active==IO_IDLE) && ((bp = bufh->b_actf)!=NULL)) {
		/* start I/O if controller idle */
		bufh->b_active = IO_BUSY;		/* lock buffer */
		/* 
		 * Done with mutex for IO_IDLE check.  State is indeed IO_IDLE,
		 * with I/O pending.
		*/
		splx(x);
		m_dev = UNIT(bp->b_dev);
#ifdef DEBUG
		printf("for unit %x; bp %x", m_dev, bp);
#endif
		media = scsicdrt + DRTAB(bp->b_dev);
		/*
		 * It is safe to muck with these variables since we have
		 * marked ourselves IO_BUSY and interupt time calls to
		 * scsistart() will not issue commands.  NO OTHER routine
		 * should build commands while IO_BUSY is set OR interrupts
		 * are enabled.  I suspect format operations violate this
		 * restriction.  Never format while the SCSI controller is
		 * active doing anything else.
		*/
		scsidev.state = 0;
		scsi_cmd[1] = UNIT(m_dev) << 5;
		if ((bp->b_flags & B_FORMAT) == B_FORMAT)
			scsi_format(bp);
		else    /* read or write operation */
		{
			/* fill in SCSI command block */
			if ((bp->b_flags & B_READ) == B_READ) {
				scsi_cmd[0] = READ;
				mode= READ;
			}
			else {	/* write operation */
				scsi_cmd[0] = WRITE;
				mode= WRITE;
			}
			block = (bp->b_blkno*(BSIZE/media->secsiz))
			      + media->part[PARTITION(bp->b_dev)].p_fsec;
			/*
			 * Probably should insure block <= 2^20...
			 * LARGER BLOCK NUMBERS WILL CORRUPT THE PARAMETER
			 * BLOCK, specifically the drive number.  DriveS
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
			scsi_setup_dma(bp->b_paddr, mode, bp->b_bcount);
		}
#ifdef DEBUG
		printf ("\nscsi  addr: %lx", bp->b_paddr);
#endif
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
 * Abstract:  This routine is called from kernel upon receipt of an interrupt
 *		signal from SCSI device.  It reads the SCSI bus phase bits to
 *		determine what action must be taken.
 *
 *  Calls: scsi_start - start I/O
 *	   scsi_dma_start - start DMA operation
 *	   scsi_error - prints out error message and updates buffer header
 *	   iodone - kernel routine, puts buffer on free list    
 *
 * Called by: kernel
 */
scsiintr()
{
	unsigned char phase;			/* SCSI bus phase lines */

#ifdef DEBUG
	printf("\nscsi intr called");
#endif
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
		}
		else if ((phase & MSG) != 0) {
#ifdef DEBUG
				printf(" STATUS  ");
				printf(" 82258 GSR = %x", in(ADMA_GSR));
#endif
				scsi_r_status();	/* STATUS */
			}
		else {
#ifdef DEBUG
			printf(" MSG IN");
#endif
			scsi_r_msg();		/* MSG IN */
			scsi_endop();
		}
	}
	else {  /* +I/O is NOT asserted */
		/* DATA OUT, COMMAND, or MSG OUT phase */
		/* configure 8255 for data output */
		outb(scsicfg.ctrl, PORTA_OUTPUT);
		if ((phase & CD) != 0) {
#ifdef DEBUG
			printf(" WRITE");
#endif
			scsi_write_data();	/* DATA OUT */
		}
		else if ((phase & MSG) != 0) {
#ifdef DEBUG
			printf(" COMMAND");
#endif
			scsi_s_cmd();	/* COMMAND */
		}
		else {
#ifdef DEBUG
			printf(" MSG OUT");
#endif
			scsi_s_msg();	/* MSG OUT */
		}
	}
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
 * Calls:  scsistrategy - Checks request and queues it
 *	   copyin - kernel routine that copies data from user area
 *	   iowait - kernel routine, sleep on a buffer
 *	   sleep - kernel routine used to delay until I/O block ready
 *	   splx - kernel routine which enables interrupt previously disabled
 *	   spl6 - kernel routine disables all interrupts except timer
 *
 *  Called by: kernel
 */
/* ARGSUSED */
scsiioctl(dev, cmd, cmdarg)
dev_t dev;		/* device number */
int cmd;		/* command */
faddr_t cmdarg;		/* user structure with parameters */
{
	register struct buf *bp;	/* I/O block used for format op */
	unsigned x;			/* saves old interrupt state */
	unsigned unit;			/* floppy unit being formatted */

#ifdef DEBUG
	printf("\nscsi entered ioctl");
#endif
	/* initialize variables */
	unit = UNIT(dev);
	bp = &scsirbuf[unit];
	if (cmd != IOC_FMT) {
		u.u_error = ENXIO;	/* only FORMAT command supported */
		return;
	}
	/* get the raw I/O buffer header to set up format request */
	x = spl6();
	/*
	 * Mutex while waiting for raw I/O buffer header to be free.
	*/
	while (bp->b_flags & B_BUSY)
	{
		bp->b_flags |= B_WANTED;
#ifdef DEBUG
		printf("\nscsi Just before sleep");
#endif
		sleep((caddr_t) bp, PRIBIO+1);
	}
	bp->b_flags = B_BUSY | B_FORMAT;
	bp->b_dev = dev;
	splx(x);
	/*
	 * Have the buffer header, done with the mutex.
	*/
#ifdef DEBUG
	printf("\nscsi queueing FORMAT request");
#endif
	scsistrategy(bp);	/* queue request ... */
	iowait(bp);		/* wait for track to be formatted ... */
	bp->b_flags &= ~(B_BUSY | B_FORMAT);	/* and release buffer */
#ifdef DEBUG
	printf("\nscsi bp->b_flags = %x\n", bp->b_flags);
#endif
}

/* Title:  scsi_endop
 *
 * Abstract:	This routine processes the status returned by the SCSI
 *		device. If an error is returned, the device is queried
 *		for extended status. Xenix operations which require
 *		multiple SCSI commands are handled here.
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
	 * the queue marked IO_BUSY, for that matter, we cannot get
	 * blown away by scsistart() while we are in progress.  All of
	 * which is to say that it is OK here (and only here) to break
	 * the usual rule that only scsistart() should issue commands.
	 * This allows us to send 'get status' commands on errors and
	 * send 'format' commands after the 'init format' command
	 * completes.
	*/
#ifdef DEBUGX
	printf("\n	entered scsi_endop state= %x\n", scsidev.state);
#endif
	if ((scsidev.state & CHECK) != 0) {
		/*
		 * CHECK bit was set in the status byte, implying an error.
		*/
		if ((scsidev.state & SENSE) == 0) {
			/*
			 * send 'get sense' command to the controller, since
			 * we haven't done so yet.
			 */
			scsidev.state |= SENSE;
			scsi_stop_dma(); /* stop 82558 */
			scsi_setup_dma(((long)dscraddr(KDS_SEL)+(long)scsi_sense),
					 READ, REQ_SENSE_LEN);
			scsi_cmd[0] = REQ_SENSE;
			scsi_cmd[1] = UNIT(m_dev) << 5;
			scsi_cmd[2] = scsi_cmd[3] = scsi_cmd[5] = 0;
			scsi_cmd[4] = REQ_SENSE_LEN;
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
		 * No error, check for format request.
		*/
		if (((bp->b_flags & B_FORMAT) == B_FORMAT)
		&&  ((scsidev.state & FORMAT2) == 0)) {
			/*
			 * Formatting is a 2 part SCSI operation.  We have
			 * completed the 'init format' part correctly, and
			 * now must do the actual format.
			*/
			scsi_format(bp);
			scsi_select();
			return;
		} else {
			/*
			 * I/O completed, deque the finished request and mark
			 * it as completed.
			*/
			scsidev.state = 0;
			bufh->b_actf = bp->av_forw;
#ifdef DEBUG
			printf("\n\tcalling iodone bp= %x, bp->av_forw= %x\n",
				bp, bp->av_forw);
#endif
			iodone(bp);
			bufh->b_active &= ~IO_BUSY;
		}
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
	printf("\nentered scsi_error\n");
	printf("scsi_sense= %x %x %x %x\n",scsi_sense[0],scsi_sense[1],
		scsi_sense[2],scsi_sense[3]);
#endif
	/* output error message */
	deverr(bufh, scsi_sense[1] & 0xFF, scsi_sense[0] & 0xFF, "SCSI");
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
	printf("      entered scsi rcv_msg\n");
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
	printf("      entered scsi rcv_status status= %x\n", status);
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
 *
 * Called by:	scsi_start 
 */
scsi_select()
{
#ifdef DEBUG
	printf("      entered scsi select\n");
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
	register int i, ncmd;	/* number of command bytes */

#ifdef DEBUGX
	printf("\n      entered scsi send_cmd");
#endif
	if ((scsi_cmd[0] & CLASS_MASK) == 0)
		ncmd= 6;
	else
		ncmd= 10;
	for (i=0; i < ncmd; i++) {
		while ((inb(scsicfg.portb) & REQ) != 0);
#ifdef DEBUGX
	printf(" %x",scsi_cmd[i]);
#endif
/*		delay(1);	*/
		outb(scsicfg.porta, scsi_cmd[i]);
	}
	outb(scsicfg.portb, 0x20); /* tweak scsi state machine hardware */
#ifdef DEBUGX
	printf(" \n");
#endif
}

/* Title:  scsi_s_msg
 *
 * Abstract:	This routine sends a message on the SCSI bus.
 *
 * Calls:
 *
 *	*************************************************
 *	*                                               *
 *	*                   NOTE                        *
 *	*                                               *
 *	* This routine should never be entered in this  *
 *	* implementation of the driver.                 *
 *	*                                               *
 *	*************************************************
 * Called by:	scsiintr
 */
scsi_s_msg()
{
	panic("SCSI bus error (MSG OUT)");
}

/* Title:  scsi_setup_dma
 *
 * Abstract: This routine programs the ADMA controller and gains
 *		access to the DMA control register.
 *
 * Calls:	sleep - kernel routine
 *		out, outb - kernel physical I/O routines
 *		splx, spl6 - kernel interrupt masking routines
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
	printf("Entered scsi setup_dma\n");
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
	x = spl6();
	if (adma_ch0_lock != SCSI_ADMA_ID) {
		while (adma_ch0_lock != 0)
			sleep(&adma_ch0_lock, PRIBIO+1);
	}
	outb(DMA_CONTROL, (DMA_SCSI | (inb(DMA_CONTROL)&CH0_MASK)));
	adma_ch0_lock= SCSI_ADMA_ID;
	splx(x);
}

/* Title:  scsi_format
 *
 * Abstract:	This routine fills in the data structures for the format
 *		command. Both phases of the format operation are handled.
 *
 * Calls:	sleep - kernel routine
 *		out, outb - kernel physical I/O routines
 *		splx, spl6 - kernel interrupt masking routines
 *
 * Called by:	scsi_start
*/
scsi_format(bp)
struct buf *bp;
{
	struct scsicdrt	*media;		/* ptr to media type in drive unit */

	/*
	 * Formatting is heaviliy dependent on the the specific SCSI
	 * controller.  This routine is implemented for the XEBEC 1420
	 * controller and ST506 drives.  IT WILL NOT WORK on other drives
	 * or other SCSI controller boards.  This is most unfortunate.
	*/
	media = scsicdrt + DRTAB(m_dev);
	if ((scsidev.state & FORMAT1) == 0) {
		/*
		 * Set up for a format operation.
		*/
		scsidev.state |= FORMAT1;
#ifdef DEBUG
		printf("	scsi FORMAT1\n");
#endif
		/*
		 * setup SCSI init format command
		 */
		scsi_cmd[0] = INIT_FORMAT;
		scsi_cmd[1] = UNIT(m_dev) << 5;
		scsi_cmd[2] = scsi_cmd[3] = scsi_cmd[4] = scsi_cmd[5] = 0;
		/*
		 * Set up init format parameter list
		*/
		scsi_mode[0] = media->ncyl >> 8;/* # Cylinders		*/
		scsi_mode[1] = media->ncyl & 0xFF;
		scsi_mode[2] = media->nhead;	/* # Heads		*/
		scsi_mode[3] = 0; /* ST-506 Non-Buffered seek - 3.0 mS rate */
		scsi_mode[4] = 0x2;		/* Sector size (512)	*/
		scsi_mode[5] = media->ncyl >> 8;
		scsi_mode[6] = media->ncyl & 0xFF;/* Reduce Write Current */
		scsi_mode[7] = (media->ncyl/2) >> 8;
		scsi_mode[8] = (media->ncyl/2) & 0xFF;
						/* Write Precompensation */
		scsi_mode[9] = 11;		/* Bits to correct	*/
#ifdef DEBUGX
		printf("\n%x %x %x %x %x ",scsi_mode[0],scsi_mode[1]&0xFF,
		 scsi_mode[2],scsi_mode[3],scsi_mode[4]);
		printf(" %x %x %x %x %x\n",scsi_mode[5],scsi_mode[6]&0xFF,
		 scsi_mode[7],scsi_mode[8]&0xFF,scsi_mode[9]);
#endif
	} else {
		/*
		 * First half of format done.  Now do second half.
		*/
		scsidev.state |= FORMAT2;
#ifdef DEBUG
		printf("	scsi FORMAT2\n");
#endif
		/*
		 * Issue SCSI format command as second step in
		 * formatting drive.
		 */
		scsi_cmd[0] = FORMAT;
		scsi_cmd[1] = UNIT(m_dev)<<5;
		scsi_cmd[2] = 0; /* From sector 0 */
		scsi_cmd[3] = 0; /* From sector 0 */
		scsi_cmd[4] = 4; /* use interleave of 4 */
		scsi_cmd[5] = 0; /* reserved */
	}
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
	printf("Entered scsi start_dma\n");
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
 *		handles the transfer for a mode select command.
 *
 * Calls:	out, outb - kernel physical I/O routines
 *		scsi_dma_start
 *
 * Called by:	scsiintr  
 */
scsi_write_data()
{
	int i;

	if (((scsidev.state & FORMAT1)!= 0) &&
	     ((scsidev.state & FORMAT2)== 0)) {
#ifdef DEBUG
		printf("	sending MODE SELECT data\n");
#endif
		for (i=0; i<INIT_FORMAT_LEN; i++) {
			while ((inb(scsicfg.portb) & REQ) != 0);
			outb(scsicfg.porta, scsi_mode[i]);
		}
		return;
	}
	else 
		scsi_dma_start();
}
