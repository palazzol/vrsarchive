/*
 * Copyright (C) Crispin Goswell 1987, All Rights Reserved.
 */

#define ROP_FALSE	0	/* F		*/
#define ROP_AND		1	/* S & D	*/
#define ROP_ANDNOT	2	/* S & ~D	*/
#define ROP_SOURCE	3	/* S		*/
#define ROP_NOTAND	4	/* ~S & D	*/
#define ROP_DEST	5	/* D		*/
#define ROP_XOR		6	/* S ^ D	*/
#define ROP_OR		7	/* S | D	*/
#define ROP_NOR		8	/* ~(S | D)	*/
#define ROP_NXOR	9	/* ~(S ^ D)	*/
#define ROP_NOTDEST	10	/* ~D		*/
#define ROP_ORNOT	11	/* S | ~D	*/
#define ROP_NOTSOURCE	12	/* ~S		*/
#define ROP_NOTOR	13	/* ~S | D	*/
#define ROP_NAND	14	/* ~(S & D)	*/
#define ROP_TRUE	15	/* T		*/
