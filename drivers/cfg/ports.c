#include "../h/i8254.h"
/*
 * definition of ports for 286/100
 *
 * P Barrett 10/21/82
 */
/* 
 * 8254 timer ports
 *
 * Do not change for the 286/100
 */
int	pitctl = PIT_CTRL_PORT;
int	pit_t0 = PIT_CTR0_PORT;
int	pit_t1 = PIT_CTR1_PORT;
int	pit_t2 = PIT_CTR2_PORT;

/*
 * 8255 Programmable Peripheral Interface ports
 * to disable use of this device, set all entries to 0
 *
 * Do not change for the 286/100
 */
int	ppi[] = {
		0xC8,	/* port A */
		0xCA,	/* port B */
		0xCC,	/* port C */
		0xCE	/* control*/
		};
/*
 * 8274 ports
 *
 * Do not change for the 286/100
 */
int	mpsc_data[2] = {
		0xDE,	/* channel A */
		0xDA};	/* channel B */

int	mpsc_ctrl[2] = {
		0xDC,	/* channel A */
		0xD8 };	/* channel B */
