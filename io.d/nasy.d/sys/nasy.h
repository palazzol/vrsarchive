/*
 *	Defines for 8250 asynchronous ports.
*/

/*
 *	Definitions for 8250 chips
*/

/* Register offsets from the data register */
#define nasy_DAT		0	/* receive/transmit data */
#define nasy_ICR		1	/* interrupt control register */
#define nasy_ISR		2	/* interrupt status register */
#define nasy_LCR		3	/* line control register */
#define nasy_MCR		4	/* modem control register */
#define nasy_LSR		5	/* line status register */
#define nasy_MSR		6	/* modem status register */
#define nasy_DLL		0	/* divisor latch (lsb) */
#define nasy_DLH		1	/* divisor latch (msb) */

/* ICR */
#define nasy_RIEN	0x01	/* enable receiver interrupt */
#define nasy_TIEN	0x02	/* enable transmitter interrupt */
#define nasy_SIEN	0x04	/* enable receiver line status interrupt */
#define nasy_MIEN	0x08	/* enable modem status interrupt */


/* ISR */
#define nasy_RSTATUS 0x06	/* change in receiver line status */
#define nasy_RxRDY	0x04	/* receiver data available */
#define nasy_TxRDY	0x02	/* transmitter holding register empty */
#define nasy_MSTATUS 0x00	/* change in modem status */

/* LCR */
#define nasy_RXLEN	0x03	/* # of data bits per received/xmitted character */
#define nasy_BITS5	0x00
#define nasy_BITS6	0x01
#define nasy_BITS7	0x02
#define nasy_BITS8	0x03
#define nasy_STOP1	0x00
#define nasy_STOP2	0x04
#define nasy_PAREN	0x08
#define nasy_PAREVN	0x10
#define nasy_PARMRK 0x20
#define nasy_SNDBRK	0x40
#define nasy_DLAB	0x80

/* baud rate definitions */
#define nasy_B9600	12

/* MCR */
#define nasy_DTR	0x01	/* bring up DTR */
#define nasy_RTS	0x02	/* bring up RTS */
#define nasy_OUT1	0x04
#define nasy_OUT2	0x08
#define nasy_LOOP	0x10	/* put chip into loopback state */


/* LSR */
#define nasy_RCA	0x01	/* receive char available */
#define nasy_OVRRUN	0x02	/* receive overrun */
#define nasy_PARERR	0x04	/* parity error */
#define nasy_FRMERR	0x08	/* framing/CRC error */
#define nasy_BRKDET	0x10	/* break detected (null ch + frmerr) */
#define nasy_XHRE	0x20	/* xmit holding register empty */
#define nasy_XSRE	0x40	/* xmit shift register empty */

/* MSR */
#define nasy_DCTS	0x01	/* CTS has changed state */
#define nasy_DDSR	0x02	/* DSR has changed state */
#define nasy_DRI	0x04	/* RI has changed state */
#define nasy_DDCD	0x08	/* DCD has changed state */
#define nasy_CTS	0x10	/* state of CTS */
#define nasy_DSR	0x20	/* state of DSR */
#define nasy_RI		0x40	/* state of RI */
#define nasy_DCD	0x80	/* state of DCD */

struct nasystate {
	unsigned short state;			/* Flags */
};

/* State flags */
#define nasy_ALIVE		0x1			/* Port was found at boot time */
