/*
 * TREK73: bpv.c
 *
 * Calculate Basic Point Values for all ships.
 *
 */

#include <stdio.h>
#include "externs.h"

main()
{
	double crew, pods, regen, num_weapons, phaser, torp;
	double bpv;
	double atof();
	int i;
	char buf[20];

	printf("Regeneration multiplier :");
	gets(buf);
	regen = atof(buf);
	printf("Pods divisor            :");
	gets(buf);
	pods = atof(buf);
	printf("Phaser multiplier       :");
	gets(buf);
	phaser = atof(buf);
	printf("Torpedo multiplier      :");
	gets(buf);
	torp = atof(buf);
	printf("Weapons multiplier      :");
	gets(buf);
	num_weapons = atof(buf);
	printf("Crew divisor            :");
	gets(buf);
	crew = atof(buf);

	for(i=0; i<MAXSHIPCLASS; i++) {
		bpv = 0.;
		bpv += stats[i].regen * regen;
		bpv += stats[i].pods / pods;
		bpv += stats[i].ph_shield * phaser;
		bpv += stats[i].tp_shield * torp;
		bpv += (stats[i].num_phaser + stats[i].num_torp) * num_weapons;
		bpv += stats[i].o_crew / crew;
		printf("%s: BPV = %.2f\n", stats[i].abbr, bpv);
	}
}
