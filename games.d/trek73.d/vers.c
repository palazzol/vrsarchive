#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/vers.c,v 1.2 1987-12-25 20:52:12 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/vers.c,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/vers.c,v 1.2 1987-12-25 20:52:12 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  87/10/09  11:13:48  11:13:48  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: vers.c
 *
 * version
 */

vers()
{
	extern char version[];

	printf("%s\n", version);
	printf("Jeff Okamoto, Peter Yee, and Roger Noe\n");
	return 1;
}
