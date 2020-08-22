/*
 *	i8751.h -- Definitions for the new line printer driver
*/
#define i8751_data_in(port)		(i8751_state[port].reg+2)
#define i8751_data_out(port)	(i8751_state[port].reg+3)
#define i8751_status(port)		(i8751_state[port].reg+4)
#define i8751_int_enable(port)	(i8751_state[port].reg+5)
#define i8751_options(port)		(i8751_state[port].reg+6)

/* status/interrupt enable register bits */
#define i8751_RCV_OFLOW		0x04
#define i8751_RCV_FULL		0x02
#define i8751_XMT_EMPTY		0x01

/* option register imperatives */
#define i8751_RCV_DISABLE	0x80
#define i8751_B300			0x00
#define i8751_B600			0x01
#define i8751_B1200			0x02
#define i8751_B2400			0x03
#define i8751_B4800			0x04
#define i8751_B9600			0x05
#define i8751_B19200		0x06	/* Not implemented */
#define i8751_B38400		0x07	/* Not implemented */

struct i8751state {
	unsigned short reg;				/* Interconnect record offset */
	unsigned short state;			/* Flags */
};

/* State flags */
#define i8751_ALIVE		0x1			/* Port was found at boot time */
#define i8751_OUTPUT	0x2			/* i8751_XMT_EMPTY is on now */
