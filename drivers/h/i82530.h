/*
 *	%Z% %M% %I% %D% %Q%
 *
 *	This Module contains Proprietary Information of Microsoft
 *	Corporation and AT&T, and should be treated as Confidential.
 */

/* i82530.h */


#define LOW(x)		((x) &0x0FF)		/* low byte */
#define HIGH(x)		((x)>>8 &0x0FF)		/* high byte */
#define MIN(x,y)	((x) > (y) ? (y) : (x))	/* return minimum of args */

#define	M_REG0	0x00		/* select reg 0 */
#define	M_REG1	0x01		/* select reg 1 */
#define	M_REG2	0x02		/* select reg 2 */
#define	M_REG3	0x03		/* select reg 3 */
#define	M_REG4	0x04		/* select reg 4 */
#define	M_REG5	0x05		/* select reg 5 */
#define	M_REG6	0x06		/* select reg 6 */
#define	M_REG7	0x07		/* select reg 7 */
#define	M_REG8	0x08		/* select reg 8 */
#define	M_REG9	0x09		/* select reg 9 */
#define	M_REG10	0x0A		/* select reg 10 */
#define	M_REG11	0x0B		/* select reg 11 */
#define	M_REG12	0x0C		/* select reg 12 */
#define	M_REG13	0x0D		/* select reg 13 */
#define	M_REG14	0x0E		/* select reg 14 */
#define	M_REG15	0x0F		/* select reg 15 */

#define	M_RS_EX_INT	0x10		/* reset external ints		wr0 */
#define	M_RS_TX_INT	0x28		/* reset TxINT pending		wr0 */
#define	M_ERR_RES	0x30		/* error reset			wr0 */
#define	M_EOI		0x38		/* reset internal ints 		wr0 */

#define	M_EXT_EN	0x01		/* enable external/status ints	wr1 */
#define	M_TX_INT_EN	0x02		/* Tx int enable		wr1 */
#define	M_RX_INT	0x10		/* interrupt on all chars	wr1 */
#define	M_P_SPEC	0x04		/* parity is special condition	wr1 */

#define	M_RX_EN		0x01		/* enable Rx			wr3 */
#define	M_RX_8BPC	0xC0		/* Rx 8 bits/char		wr3 */
#define	M_RX_7BPC	0x40		/* Rx 7 bits/char		wr3 */
#define	M_RX_6BPC	0x80		/* Rx 6 bits/char		wr3 */

#define	M_16X		0x40		/* 16x clock rate		wr4 */
#define	M_1STOP		0x04		/* one stop bit			wr4 */
#define	M_2STOP		0xC0		/* two stop bits		wr4 */
#define	M_PAR_EN	0x01		/* parity enable		wr4 */
#define	M_PAR_EVEN	0x02		/* even parity, else odd	wr4 */

#define	M_RTS		0x02		/* request to send		wr5 */
#define	M_TX_EN		0x08		/* Tx enable			wr5 */
#define	M_BREAK		0x10		/* send break 			wr5 */
#define	M_TX_8BPC	0x60		/* Tx 8 bits/char		wr5 */
#define	M_TX_7BPC	0x20		/* Tx 7 bits/char		wr5 */
#define	M_TX_6BPC	0x40		/* Tx 6 bits/char		wr5 */
#define	M_DTR		0x80		/* data terminal ready		wr5 */

#define M_VIS		0x01		/* vector includes status	wr9 */
#define	M_NV		0x02		/* non-vectored mode (slave)	wr9 */
#define	M_MIE		0x08		/* master interrupt enable	wr9 */
#define M_RESET		0xC0		/* force hardware reset		wr9 */

#define	M_RX_TX_CLKS	0x56		/* RX, TX clock sources		wr11 */

#define	M_BRG_EN	0x01		/* enable baud rate generator	wr14 */
#define	M_BRG_SRC	0x02		/* use SCC CLK signal as source	wr14 */

#define	M_CD_EN		0x04		/* enable INT on change in CD	wr15 */
#define	M_CTS_EN	0x20		/* enable INT on change in CTS	wr15 */
#define	M_BRK_EN	0x80		/* enable INT on BREAK detect	wr15 */

#define	M_CHAR_AV	0x01		/* recieve char avail 		rr0 */
#define	M_TX_EMPTY	0x04		/* Tx buffer empty		rr0 */

#define	M_PERROR	0x10		/* parity error			rr1 */
#define	M_FRERROR	0x20		/* framing error		rr1 */
#define	M_OVERRUN	0x40		/* overrun error		rr1 */

#define	M_CHA		0x08		/* channel A bit (unshifted)	rr2 */

#define CH_A		0		/* index into i530cfg table for chan A */
#define CH_B		1		/* index into i530cfg table for chan B */

/*
 * data structures for 82530
 * The SCC (managed by the i82530 driver) is described in a table
 * of i530cfg structures. 
 */
struct i530cfg {
	unsigned	m_ctrl;	/* control port for this channel */
	unsigned	m_data;	/* data port for this channel */
	unsigned	m_intlev;/* interrupt level */
	};

/*
 * baud rate definitions for 286/100
 */
#define	i530_B19200	19200
#define	i530_B9600	9600
#define	i530_B4800	4800
#define	i530_B2400	2400
#define i530_B1800	1800
#define	i530_B1200	1200
#define	i530_B600	600
#define	i530_B300	300
#define i530_B200	200
#define	i530_B150	150
#define i530_B134	134
#define	i530_B110	110
#define	i530_B75	75
#define i530_B50	50
#define i530_B0		0

/*
 * BRG_CONSTANT is the magic number which converts the baud rates above
 * into time constants for the 82530. This number is correct for a 16x
 * clock multiplier (wr4) and a 9.8304 MHz input clock to the 82530.
 * see the 82530 technical manual.
 */
#define	BRG_CONSTANT	153600L		/* double word constant */

#define	MINORMSK	0x1F
#define	MODEMMSK	0x20

#define	RATEMD0		0x36

#define	TEST_VECT	0xA5
#define	VECT_MASK	0xE0
#define	INT_PENDING	0x02
