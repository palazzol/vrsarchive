#define LPUNIT(dev)	(minor(dev)>>1)	/* Which printer is this?	*/
#define	LPPRI		(PZERO+1)	/* allow sleepers to wakeup early */
#define LPTIMEO		(10*HZ)		/* Time between not-ready checks */

/*
 *	Markers
*/
#define	LPLWAT	50	/* line printer low water mark */
#define	LPHWAT	75	/* line printer high water mark */

/*
 *  This is for the printer which is attached to the 8255 on the
 *	286/10 board...
 *
 *     8255 is programmed for mode 0 (mode word: 82H)
 *
 *     Port A and C: OUTPUT
 *     Port B : INPUT
 *
 *  Port B definition (bit 0 is LSB;  bit 7 is MSB):
 *
 *         Bit
 *       0 - 3   -   not used
 *           4   -   Line Printer ACK/
 *           5   -   not used by the printer
 *           6   -   Line Printer Error
 *           7   -   Line Printer Busy
 *
 *  Port C definition (bit 0 is LSB;  bit 7 is MSB):
 *
 *         Bit
 *           0   -   Character strobe to the printer
 *       1 - 6   -   not used by the printer
 *      (note, bit 1 must be high for proper system operation)
 *           7   -   Clear Line Printer Ack Flop/
*/

/*
 *	Hardware constants
*/
#define PT_INIT 0x82	/* 8255 command word for port init */
#define SET_PRINTER_ACK 0xf  /* set printer ACK bit */
#define CL_PR_ACK 0xe  /* clear printer ACK bit */
#define TEST	0xaa	/* test pattern read back by probe */
#define PR_BUSY 0x80  /* printer busy bit */
#define PR_ERROR 0x40  /* printer error bit */
#define PR_ACK_BAR 0x10  /* printer ACK line */
#define ONSTROBE 1
#define OFFSTROBE 0
#define PT_OVERRIDE 0x2  /* written to port C to set bit 1 */

/*
 *	Device Structures
*/
struct lpcfg {
		int p_level;		/* intr level */
		int p_porta;		/* data out */
		int p_portb;		/* status in */
		int p_portc;		/* strobe out */
		int control;
};
