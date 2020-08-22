#ident "@(#) TREK73 $Header: /home/Vince/cvs/games.d/trek73.d/bpv.c,v 1.2 1987-12-25 20:51:57 vrs Exp $"
/*
 * $Source: /home/Vince/cvs/games.d/trek73.d/bpv.c,v $
 *
 * $Header: /home/Vince/cvs/games.d/trek73.d/bpv.c,v 1.2 1987-12-25 20:51:57 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  87/10/09  15:48:35  15:48:35  okamoto (Jeff Okamoto)
 * Added declaration of round as a function returning a double.
 * 
 * Revision 1.1  87/10/09  11:00:29  11:00:29  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: bpv.c
 *
 * Calculate the Basic Point Value of a ships
 *
 */

void
calculate(regen, pods, p_div, t_div, weapons, crew, bpv, eff, turn, max)
double regen;	/* Regeneration */
float pods;	/* Number of antimatter pods */
float p_div;	/* Shield divisor for phasers */
float t_div;	/* Shield divisor for torps */
int weapons;	/* Number of weapons */
int crew;	/* Number of crew */
double *bpv;	/* Return for BPV */
double *eff;	/* Return for efficiency */
int *turn;	/* Turn per segment */
int *max;	/* Maximum speed */
{
	double floor(), round();

	*bpv = 0.;
	*bpv += regen * 12;
	*bpv += pods / 2;
	*bpv += p_div * 30;
	*bpv += t_div * 40;
	*bpv += weapons * 10;
	*bpv += crew / 15;

	*eff = round(4 * (0.0034 * *bpv - 0.78)) / 4.0;
	if (*eff< 0.25)
		*eff= 0.25;
	*turn = (int) (10 - floor(*bpv / 100.0));
	if (*turn < 1)
		*turn = 1;
	*max= (int) round(-0.004 * *bpv + 11.0);
	if (*max < 1)
		*max = 1;
}

double
round(x)
double x;
{
	double floor();

	return(floor(x + 0.5));
}
