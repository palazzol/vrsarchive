#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/union.h,v 1.1 1987-12-25 20:55:17 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/union.h,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/union.h,v 1.1 1987-12-25 20:55:17 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  87/10/09  11:14:09  11:14:09  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: union.h
 *
 * Union for yacc
 */
typedef union {
	double dval;
	char sval[20];
} YYSTYPE;
