/*******************************************************\
| FILE: scsi.h -- Macros/Constants used by SCSI driver	|
\*******************************************************/


struct scsiminor	/* defines scsi minor device numbers */
{
	unsigned partition:4;		/* partition number */
	unsigned drtab: 4;		/* drtab number */
	unsigned unit: 4;		/* unit */
	unsigned target: 4;		/* SCSI target (board) id */
};


/**** MACROS *****/

#define UNIT(dev)\
		(scsiminor[minor(dev)].unit)	/* returns unit */
#define DRTAB(dev)\
		(scsiminor[minor(dev)].drtab)	/* returns media */
#define PARTITION(dev)\
		(scsiminor[minor(dev)].partition)/* returns partition */
#define TARGET(dev)\
		(scsiminor[minor(dev)].target)	/* returns scsi target id */
#define SCSIMINOR(tnum,unum,media,panum)\
		((tnum<<12)|(unum<<8)|(media<<4)|panum)	/* puts fields in structure */

#define LOW(x)		((x) &0x0FF)		/* low byte */
#define HIGH(x)		((x)>>8 &0x0FF)		/* high byte */
#define MIN(x,y)	((x) > (y) ? (y) : (x))	/* return minimum of args */


/***** CONSTANTS *****/

/* booleans */
#define YES		0x1		/* true boolean */
#define NO		0x0		/* false boolean */
#define TRUE		0x1		/* another true boolean */
#define FALSE		0x0		/* another false boolean */
#define SUCCESS		0x0		/* normal return from function */
#define FAILURE		0x1		/* error status returned by function */

/* SCSI commands */
#define TEST_UNIT	0x00		/* test unit ready command */
#define REQ_SENSE	0x03		/* request sense (error status) */
#define FORMATALL	0x04		/* format unit command (not used) */
#define FORMAT		0x06		/* format track command */
#define READ		0x08		/* read command */
#define WRITE		0x0A		/* write command */
#define SEEK		0x0B		/* seek command (not used) */
#define FORMATALT	0x0E		/* assign alternate track */
#define INIT_FORMAT	0x11		/* specify device parameters */

/* bit masks used for SCSI bus status with 8255 port B */
#define BSY		0x01		/* SCSI bus busy line */
#define PARITY		0x02		/* SCSI bus parity line */
#define RST		0x04		/* SCSI bus reset line */
#define MSG		0x08		/* SCSI bus msg line */
#define SEL		0x10		/* SCSI bus sel line */
#define CD		0x20		/* SCSI bus c/d line */
#define REQ		0x40		/* SCSI bus request line */
#define IO		0x80		/* SCSI bus i/o line */

/* bit masks used for SCSI control with 8255 port C */
#define RST_ON		0x01		/* assert SCSI bus reset line */
#define SEL_ON		0x02		/* assert SCSI bus select line */
#define ATN_ON		0x04		/* assert SCSI bus attention line */
#define ALL_OFF		0x00		/* all lines not asserted */
#define ARB_START	0xC0		/* start SCSI bus arbitration */

/* 8255 commands */
#define PORTA_INPUT	0x92		/* 8255 port A is an input port */
#define PORTA_OUTPUT	0x82		/* 8255 port A is an output port */

/* buffer header activity flags */
#define IO_IDLE		0		/* device idle */
#define IO_BUSY		1		/* device busy */
#define IO_OPEN_WAIT	2		/* scsiopen needs ctrl to open dev */

/* unit flags */
#define U_FORMAT	0x20		/* FORMAT command queued */

/* scsidev state bits */
#define SENSE		0x01		/* REQ_SENSE command has been issued */
#define CHECK		0x02		/* SCSI error  */
#define BUSY		0x08		/* SCSI device busy */
#define EXTND_CMD	0x10		/* Extended command */

#define CLASS_MASK	0xE0		/* mask for SCSI command class */
#define FORMAT_BITS	0x00		/* format command control bits */

/* ioctl format command */
#define IOC_FMT		(('W' << 8) | 0)

/* format flag - taken from i215g.h */
#define B_FORMAT 040000

/* limits on array dimensions */
#define MAX_UNITS		4	/* max number supported drive units */

/* ADMA channel lock ID */
#define SCSI_ADMA_ID	0x02

/****** STRUCTURES ******/

/* partition table - one for every media type */
struct scsipartition
{
	daddr_t p_fsec;		/* first sector */
	daddr_t p_nsec;		/* number sectors */
};

/* media descriptor table element */
struct scsicdrt
{
	unsigned	ncyl;	/* number cylinders */
	char		nhead;	/* number heads(sides) */
	unsigned	spt;	/* sectors per track */
	unsigned	secsiz;	/* sector size */
	unsigned	fmt3;	/* format magic (byte 3) */
	unsigned	pcomp;	/* format magic (byte 9) */
	struct scsipartition
			*part;	/* ptr to partition table */
};

/* SCSI configuration table */
struct scsicfg
{
	unsigned	ctrl;	/* 8255 control port address */
	unsigned	porta;	/* 8255 porta address */
	unsigned	portb;	/* 8255 portb address */
	unsigned	portc;	/* 8255 portc address */
	char		c_level;/* interrupt level */
	struct scsicdrt	*drtab;	/* pointer to drive characteristics */
};


/* state table - one in system */
/* parameter block - one in system */
struct scsiiopb
{
	unsigned	adma_command;	/* 82258 command */
	long		adma_src_ptr;	/* 82258 source pointer */
	long		adma_dst_ptr;	/* 82258 destination pointer */
	long		adma_count;	/* transfer byte count */
	unsigned	adma_status;	/* status byte filled in by 82258 */
	char		adma_stop[12];	/* unconditional stop */
};

/* device unit table - one per drive */
struct scsiunit
{
	char			flags;		/* unit status flags */
	char			popen;		/* partition open flags - 1 bit each */
};

/* device table - one per SCSI controller (currently limited to 1) */
struct scsidev
{
	char			exists;		/* existence flag */
	unsigned		state;		/* device state flags */
	struct iobuf		*bufh;		/* buffer header pointer */
	struct scsiiopb		iopb;		/* parameter block */
	struct scsiunit		d_unit[MAX_UNITS];
};

/* user format structure */
struct user_ftk
{
	int	f_trck;		/* track # */
	int	f_intl;		/* interleave factor */
	int	f_skew;		/* track skew */
	char	f_type;		/* format type-code */
	char	f_pat[4];	/* pattern data */
};

/*
 *	Conversion macro - PHYSICAL track to LOGICAL sector number.  Use by
 *	ioctl() processing, which is in terms of physical cylinder/surface
 *	addresses.  SCSI wants logical sector numbers for everything.
 *	Cylinder 0 is reserved to the SCSI controller, and any reference to
 *	it redirected to logical address 0 (Cylinder 1, sector 0).
 *	Be sure to cast utrk to a long, if it isn't already.
*/
#define address(media,utrk) (utrk<media->nhead?0:(utrk-media->nhead)*media->spt)

#define FORMAT_DATA	0x00	/* Format as a data track	*/
#define FORMAT_CYL	0x20	/* Format cylinder (internal only) */
#define FORMAT_ALT	0x40	/* Format as an alternate track (ignored) */
#define FORMAT_BAD	0x80	/* Format as a bad track	*/

/*
 *	These values are used for byte 3 in the configuration block
 *	of an INIT_FORMAT command.  They must be chosen to match the
 *	characteristics of the drive and format desired.
*/
#define WMS3		0x00	/* 3ms unbuffered seek		*/
#define WBS15		0x10	/* 15us buffered seek		*/
#define WBS30		0x20	/* 30us buffered seek		*/
#define WBS70		0x30	/* 70us buffered seek		*/
#define WBS200		0x40	/* 200us buffered seek		*/
#define STD_DRV		0x00	/* Standard Drive		*/
#define SRV_DRV		0x01	/* Inbedded Servo Drive		*/
#define FM		0x21	/* 2Ms step, FM all tracks	*/
#define MIXED		0x22	/* 2Ms step, FM trk 0, MFM rest	*/
#define MFM		0x23	/* 2Ms step, MFM all tracks	*/
