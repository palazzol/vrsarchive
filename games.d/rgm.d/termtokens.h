/*
 * termtokens.h: Rog-O-Matic XIV (CMU) Fri Dec 28 22:16:05 1984 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Various tokens used by the screen reading package.
 */

/*
 *	If XX_CHR is defined, the control sequence <ESC> <CHR> will be replaced
 *	by XX_TOK.  Otherwise, the control token is assumed to oocur literally
 *	in the input.  ER_TOK is the token for an invalid escape sequence.
*/
# define ROGUETERM "vc404"
# define ROGUECAP "M8|vc404:am:bs:ce=^V:cl=^X:cm=^P%+ %+ :co#80:li#24:up=^Z:do=^J:nd=^U:"
# define ER_TOK -2		/* -1 is reserved for EOF */
# define BS_TOK ctrl('H')
# define CE_TOK ctrl('V')
# define CL_TOK ctrl('X')
# define CM_TOK ctrl('P')
# define CR_TOK ctrl('M')
# define LF_TOK ctrl('J')
# define ND_TOK ctrl('U')
# define TA_TOK ctrl('I')
# define UP_TOK ctrl('Z')
