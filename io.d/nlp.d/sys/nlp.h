/*
 *	nlp.h -- Definitions for the new line printer driver
*/
#define nlp_data(port)	(nlp_base[port])
#define nlp_stat(port)	(nlp_base[port]+1)
#define nlp_cmnd(port)	(nlp_base[port]+2)

/* status register predicates */
#define NLP_NOPAPER(sr)	((sr)&0x20)
#define NLP_OFFLINE(sr)	(!((sr)&0x10))
#define NLP_ERROR(sr)	(!((sr)&0x08))
#define NLP_IDLE(sr)	((sr)&0x80)
#define NLP_READY(sr)	(!((sr)&0x40))

/* command register imperatives */
#define STROBE		0x01	/* Character strobe (1 active) */
#define NO_FEED		0x02	/* Paper feed after line print (1 active) */
#define NO_RESET	0x04	/* Reset (0 active) */
#define SELECT		0x08	/* Enable data transfer (1 active) */
#define INTERRUPT 	0x10	/* Enable interrupts on NACKLDG transition */

struct nlpstate {
	int state;
};

/* State flags */
#define ALIVE	0x1
