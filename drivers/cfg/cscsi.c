/*
 * The following lines are instl/ICP hooks.
 * Of course, you'll need Multibus II instl/ICP.
*/
#define		CMI10MB		0
#define		QNTM36MB	1

#include "../h/param.h"
#include "../h/scsi.h"
#include "../h/buf.h"
#include "../h/iobuf.h"

/*
 * Set to the number of cylinders to reserve for co-resident MSDOS partition.
 * Of course, you'll need a Multibus II MS-DOS.
*/
#define	MSDOS	0

/*
 *	These are wired down by the XEBEC SCSI controller and the driver
 *	and must not change.  They are only here so the formulas later
 *	will be more clear.
*/
#define SECTORS 17
#define BYTES 512

/*
 *	Multibus II ICP should derive the following from /etc/bootcfg and
 *	put it into a series of ifdefs as is done in Multibus I.
*/
#if CMI10MB
#  define CYLINDERS 306
#  define HEADS 4
#  define SWPSIZE 1188
#  define PRECOMP (CYLINDERS/2)
#endif CMI10MB
#if QNTM36MB
#  define CYLINDERS 512
#  define HEADS 8
#  define SWPSIZE 4104
#  define PRECOMP ((CYLINDERS*15)/32)
#  define USRCGSZ 2720
#endif QNTM36MB

/*
 *	These defines create useful names which help calculate the partition
 *	sizes for the first winchester drive.  Calculating this for the user
 *	at compile time improves accuracy and lets us adapt flexibly when new
 *	drive types are added to /etc/bootcfg.  This last is critical for boot
 *	floppy generation for new disk types.
 *
 *	Note:	You almost certainly do not want to change these.
 *	Note:	We always reserve the last cylinder, and SCSI reserves the
 *		first.  Hence the 2 in the DISKSIZ formula.
*/
#define SCALE	(BSIZE/BYTES)
#define CYLSIZ	(HEADS*SECTORS)
#define DISKSIZ	(CYLINDERS*CYLSIZ-2*CYLSIZ)
#define W_ALTS	(((CYLINDERS*HEADS)/50)/HEADS)
#define DIAGSIZ	(W_ALTS*CYLSIZ+CYLSIZ)
#define UNXENIX (DIAGSIZ+(MSDOS*CYLSIZ))
#define SWAPSIZ	(SWPSIZE*SCALE)

/*
 *	If there is a user partition cylinder group size in /etc/bootcfg, then
 *	there must be a user partition.  Otherwise, make the user partition
 *	zero length and lump the storage into the root partition.  The
 *	convention is that disks with less than 20MB will not have a seperate
 *	user partition.
 *
 *	Note: Changes here can make it very difficult to implement boot
 *	floppies by preventing the use of instl.
*/
#ifndef USRCGSZ
#  define ROOTSIZ (DISKSIZ-UNXENIX-SWAPSIZ-2*SECTORS)
#  define RESTSIZ 0
#else
#  define ROOTSIZ (8208*SCALE)
#  define RESTSIZ (DISKSIZ-UNXENIX-SWAPSIZ-ROOTSIZ-2*SECTORS)
#endif

/*
 * Wini Partition table
 * Note: Partition[0] is the entire disk (except for the diagnostic cylinder
 *	 and the SCSI cylinder).  This includes the alternate-track cylinders
 *	 and the bad-track table.  SCSI logical addresses start with cylinder
 *	 1, making it look as though the SCSI and diagnostic cylinders are
 *	 taken from the far end.  It is really cylinder 0.
*/
struct	scsipartition Piw0[] = {
	0,			DISKSIZ,	/* [0] whole disk	*/
	SECTORS,		ROOTSIZ,	/* [1] "root"		*/
	ROOTSIZ+SECTORS,	SWAPSIZ,	/* [2] "swap"		*/
	SWAPSIZ+ROOTSIZ+SECTORS, RESTSIZ,	/* [3] "rest"		*/
	SECTORS,		RESTSIZ+SWAPSIZ+ROOTSIZ,
						/* [4] mountable device	*/
	DISKSIZ-UNXENIX,	MSDOS*CYLSIZ,	/* [5] "MS-DOS"		*/
	SWAPSIZ+ROOTSIZ+SECTORS+RESTSIZ, SECTORS/* [6] "error"		*/
};

/*
 * Floppy Partitions.
 *	Note: It is possible to use scsiminor to specify combinations
 *	of scsicdrt and scsipartition entries that do not make sense.
 *	The standard configurations should use the corresponding index
 *	for each and keep the two tables in the same order.
*/
struct	scsipartition Pf0[] = {
	8,	632,	/* [0] (minor 07) dnf0	RMX         format	*/
	0,	1280,	/* [1] (minor 09) syf0	bootstrap   format	*/
	0,	640,	/* [2] (minor 10) dzf0	MSDOS R1    format	*/
	16,	368,	/* [3] (minor 12) boot	boot floppy format	*/
	384,	224	/* [4] (minor 13) ram	boot floppy format	*/
};

/*
 * Device-Table Definitions (drtabs)
 *	Note: It is possible to use scsiminor to specify combinations
 *	of scsicdrt and scsipartition entries that do not make sense.  The
 *	standard configurations should use the corresponding index for
 *	each and keep the two tables in the same order.  If we plan this
 *	out for the next release, maybe we can bring minor and function
 *	and name into agreement, so one /dev/makefile entry would suffice
 *	for 5 and 8 inch configs.
 *
 *	The field that now holds the number of alternates is not currently
 *	used.  It will eventually be taken over to give the driver driver
 *	information about how to format the drive.  For wini drives this
 *	will mean imbedded servo or not, and for floppies, this will mean
 *	single density, double density, etc.
*/
struct	scsicdrt scsicdrt[] = {
  CYLINDERS, HEADS, SECTORS, BYTES, SRV_DRV+WBS15, PRECOMP, Piw0,
  40,	2,	8,	512,  MFM,	0, Pf0,  /* [1] dnf0 [07]	*/
  40,	1,	16,	128,  FM,	0, Pf0,  /* [2] syf0 [09]	*/
  40,	2,	8,	512,  MIXED,	0, Pf0,  /* [1] dnf0 [07]	*/
};

/*
 * This table maps the minor number to the board number, unit,
 * drtab subscript, and partition subscript. 
 *
 * Note: Units 0 and 1 are the first and second wini drive.
 *       Units 2 and 3 are the first and second floppy drive.
 *	 We reserve the minor numbers that correspond to tape in
 *	 Multibus I systems.
*/
unsigned scsiminor[] = {
/*	SCSIMINOR(target#,unit#,drtab#,partition#)			*/
						/* [minor] device */
	SCSIMINOR(0,0,0,0),			/* [0]  Wini format disk */
	SCSIMINOR(0,0,0,1),			/* [1]  Wini root  part	 */
	SCSIMINOR(0,0,0,2),			/* [2]  Wini swap  part	 */
	SCSIMINOR(0,0,0,3),			/* [3]  Wini usr   part	 */
	SCSIMINOR(0,0,0,6),			/* [4]  Wini error part	 */
	SCSIMINOR(0,0,0,4),			/* [5]  Wini whole disk	 */
	SCSIMINOR(0,0,0,5),			/* [6]  Wini MS-DOS part */
	SCSIMINOR(0,2,1,0),			/* [7]	dnf0/dvf0 floppy */
	SCSIMINOR(0,2,1,2),			/* [8]  df0 -> dzf0      */
	SCSIMINOR(0,2,2,1),			/* [9]  syf0	  floppy */
	SCSIMINOR(0,2,1,2),			/* [10] dzf0/dxf0 floppy */
	SCSIMINOR(0,2,1,2),			/* [11] dvf0 -> dzf0     */
	SCSIMINOR(0,2,3,3),			/* [12] boot	  floppy */
	SCSIMINOR(0,2,3,4),			/* [13] ram	  floppy */
	SCSIMINOR(4,0,0,0),			/* [14] Reserved	 */
	SCSIMINOR(4,0,0,0),			/* [15] Reserved	 */
	SCSIMINOR(4,0,0,0),			/* [16] Reserved	 */
	SCSIMINOR(4,0,0,0),			/* [17] Reserved	 */
	SCSIMINOR(4,0,0,0),			/* [18] Reserved	 */
	SCSIMINOR(4,0,0,0),			/* [19] Reserved	 */
};

struct	scsicfg  scsicfg[] = {
/*   ctrl addr,  porta addr,  portb addr,  portc addr,  intr,  drtab  */
     0x0CE,      0x0C8,       0x0CA,       0x0CC,       1,     scsicdrt
};

/*
 *	The following are static initialization variables which are based
 *	on the configuration.  These values are self-configuring based on
 *	the above, and should not be changed.
*/
int	num_scsi = sizeof(scsicfg)/sizeof(scsicfg[0]);
				/* Number of boards configured */
