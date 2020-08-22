/*
 *
 *		Intel Corporation
 *		2402 W. Beardsley Rd.
 *		Phoenix, Az. 85027
 *
 *	Date: 06/18/85
 *
 *	Programmer: Brian J. McArdle
 * 
 * ABSTRACT:
 * 
 * This routine is called by all drivers which use the 82258 (ADMA) chip
 * on the 286/100. Only the first call performs any initialization, however
 * all drivers should call this routine to ensure that the 82258 is in fact
 * initialized.
 *
 * Calls:	outw - Kernel I/O port output routine.
 *
 */

#include "../h/adma.h"

#define LOW(x)		((x) &0x0FF)		/* low byte */
#define HIGH(x)		((x)>>8 &0x0FF)		/* high byte */

char adma_ch0_lock= 0;
char adma_ch1_lock= 0;
char admainitialized= 0;

extern int adma_gmr;
extern char adma_gbr, adma_gdr;

admainit()
{
#ifdef DEBUG
	printf(" Entered admainit - ");

#endif
	if (!admainitialized++) {
#ifdef DEBUG
		printf("initializing\n");
#endif
		outb(ADMA_GMR, LOW(adma_gmr));
		outb(ADMA_GMR+1, HIGH(adma_gmr));
		outb(ADMA_GBR, adma_gbr);
		outb(ADMA_GDR, adma_gdr);
	}
#ifdef DEBUG
	else
		printf("already initialized\n");
#endif
}
