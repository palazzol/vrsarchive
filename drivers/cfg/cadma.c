/*
 * 82258 configuration
 *
 * values for the GENERAL MODE REGISTER,
 *                GENREAL BURST REGISTER,
 *        and the GENERAL DELAY REGISTER must be specified.
 */

int adma_gmr = 0x0601;	/* 8 bit I/O, 16 bit MEM bus width, Local mode,	*/
			/* ch. 3 normal, 2 cycle, fixed priority	*/
			/* interrupts disabled, common int. disabled	*/

char adma_gbr = 1;	/* maximum number of contiguous bus cycles	*/

char adma_gdr = 1;	/* minimum number of clocks between bursts	*/

