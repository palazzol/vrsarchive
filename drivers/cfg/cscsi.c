#define		CMI10MB		1

#include "../h/param.h"
#include "../h/scsi.h"
#include "../h/buf.h"
#include "../h/iobuf.h"

#define CYLS	306
				/* Total Cylinders			*/
#define HEADS	4
				/* Fixed Heads				*/
#define SECTORS	17
				/* Sectors per track			*/
#define SECSIZ	512
				/* Big as XEBEC can get			*/
#define SWAPSIZ	sectors(1188)
				/* Wired down in xenixconf at 1188K	*/

/*******************************************************************************
 *
 * CMI 5.25 wini
 * Note: Partition[0] is the entire disk (except for the diagnostic cylinder).
 *	 This includes the alternate-track cylinders and the bad-track data.
 *	 Partitions[1-3] only address cylinders 0-293.  294-303 are alternate
 *	 track cylinders.  Cylinder 304 contains the bad-track data.
 *	 Cylinder 305 is for diagnostics.  
 *
 ******************************************************************************/

#define CYLSIZ	(HEADS*SECTORS)
				/* Sectors per cylinder			*/
#define sectors(k) (k*(1024/SECSIZ))
				/* Convert Kbytes to sectors		*/
#define UNXENIX	10
				/* 10 cyls (8 alts, 1 BT table, 1 diag) */
#define XENIX	((CYLS-UNXENIX)*SECTORS*HEADS)
				/* Size to account for with XENIX	*/
#define ROOTSIZ	(XENIX-2*SECTORS-SWAPSIZ)
				/* Root partition size			*/

struct	scsipartition Piw0[] = {
	0,	(CYLS-1)*HEADS*SECTORS,	/* [0] whole disk (+alts & bad-track)*/
	SECTORS,	       ROOTSIZ,	/* [1] "root" */
	ROOTSIZ+SECTORS,       SWAPSIZ,	/* [2] "swap" */
	SWAPSIZ+ROOTSIZ+SECTORS,SECTORS,/* [3] "error" */
	    0,		   0		/* extra */
};


/*
 * 215 Board 0, Unit 0 (Wini) Device-Table Definitions (drtab's)
 * Note: the drive description sets nalt == 0; this is to allow
 *	 access to the bad-track data which is beyond the alternate
 *	 tracks.  The user must be careful not to overwrite this
 *	 data unintentionally (ie, via format). 
 */
struct	scsicdrt scsicdrt[] = {
/* Cyls, Fixed, #Sec,  SecSiz,  Nalt, Partitions  Unit: Drive-Type   */
   306,	 HEADS,	SECTORS, SECSIZ,	8,	Piw0	/* [0]: CMI 5.25"   */
};


/*
 *
 * This table maps the minor number to the board number, unit,
 * drtab subscript, and partition subscript. 
 *
 */
unsigned scsiminor[] = {

/*	SCSIMINOR(target#,unit#,drtab#,partition#)		*/
						/* [minor] device */
	SCSIMINOR(0,0,0,0),			/* [0] cmi track 0 - bad trk. */
	SCSIMINOR(0,0,0,1),			/* [1] cmi root part */
	SCSIMINOR(0,0,0,2)			/* [2] cmi swap part */
};


struct	scsicfg  scsicfg[] = {
/*   ctrl addr,  porta addr,  portb addr,  portc addr,  intr,  drtab  */
     0x0CE,      0x0C8,       0x0CA,       0x0CC,       1,     scsicdrt
};

/*******************************************************************************
 *
 * The following are static initialization variables
 * which are based on the configuration.
 * These values are self-configuring based on the above, and should
 * not be changed.
 *
 ******************************************************************************/

/* Number of boards configured */

int	num_scsi = 1;
