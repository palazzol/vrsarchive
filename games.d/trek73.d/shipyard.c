/*
 * TREK73: shipyard.c
 *
 * Design your own ship
 *
 */

#include <stdio.h>
#ifdef BSD
#include <strings.h>
#endif
#ifdef SYSV
#include <string.h>
#endif
#include <fcntl.h>
#include "externs.h"

char buf[20];
char class[3];
char cloak;
double bpv;
struct {
	char description[30];
	char race[30];
	char empire[30];
} stuff;

struct ship_stat design;

main()
{
	double regen, efficiency, atof(), floor(), round();
	int crew, phasers, torps, pods, max_speed, turn, p_div, t_div;
	int done, atoi();

	printf("Class identifier           :");
	gets(class);
	class[2] = '\0';
	printf("Class description          :");
	gets(stuff.description);
	stuff.description[29] = '\0';
	printf("Race name                  :");
	gets(stuff.race);
	stuff.race[29] = '\0';
	printf("Empire name                :");
	gets(stuff.empire);
	stuff.empire[29] = '\0';
	done = 0;
	while (!done) {
		printf("Regeneration               :");
		gets(buf);
		regen = atof(buf);
		if (regen >= 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Pods                       :");
		gets(buf);
		pods = atof(buf);
		if (pods >= 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Number of phasers          :");
		gets(buf);
		phasers = atoi(buf);
		if ((phasers >= 0) && (phasers < MAXWEAPONS))
			done = 1;
		else
			if (phasers < 0)
				printf(">>> Be reasonable.\n");
			else
				printf(">>> Can't have more than %d.\n",
				    MAXWEAPONS-1);
	}
	done = 0;
	while (!done) {
		printf("Number of tubes            :");
		gets(buf);
		torps = atoi(buf);
		if ((torps >= 0) && (torps < MAXWEAPONS))
			done = 1;
		else
			if (torps < 0)
				printf(">>> Be reasonable.\n");
			else
				printf(">>> Can't have more than %d.\n",
				    MAXWEAPONS-1);
	}
	done = 0;
	while (!done) {
		printf("Shield divisor for phasers :");
		gets(buf);
		p_div = atof(buf);
		if (p_div > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Shield divisor for torps   :");
		gets(buf);
		t_div = atof(buf);
		if (t_div > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	done = 0;
	while (!done) {
		printf("Crew                       :");
		gets(buf);
		crew = atoi(buf);
		if (crew > 0)
			done = 1;
		else
			printf(">>> Be reasonable.\n");
	}
	printf("Can the ship cloak         ?");
	gets(buf);
	if (buf != NULL && (buf[0] == 'y' || buf[0] == 'Y'))
		cloak = 1;
	else
		cloak = 0;

	bpv = 0.;
	bpv += regen * 12;
	bpv += pods / 2;
	bpv += p_div * 30;
	bpv += t_div * 40;
	bpv += (phasers + torps) * 10;
	bpv += crew / 15;
	printf("%s: BPV = %.2f\n", class, bpv);

	efficiency = round(4 * (0.0034 * bpv - 0.78)) / 4;
	if (efficiency < 0.25)
		efficiency = 0.25;
	turn = 10 - floor(bpv / 100);
	if (turn < 1)
		turn = 1;
	max_speed = (int) round(-0.004 * bpv + 11);
	if (max_speed < 1)
		max_speed = 1;

	printf("Efficiency = %.2f\n", efficiency);
	printf("Turn = %d\n", turn);
	printf("Max speed = %d\n", max_speed);

	strcpy(design.abbr, class);
	design.num_phaser = phasers;
	design.num_torp = torps;
	design.o_warpmax = max_speed;
	design.e_warpmax = max_speed + 2;
	design.o_eff = efficiency;
	design.e_eff = efficiency;
	design.regen = regen;
	/* XXXX */
	design.energy = pods * 3 / 4;
	design.pods = pods;
	design.o_crew = crew;
	design.e_crew = crew * 5 / 4;
	design.ph_shield = p_div;
	design.tp_shield = t_div;
	design.turn_rate = turn;
	design.cloaking_energy = 4;
	/* XXXX */
	design.t_blind_left = 135;
	design.t_blind_right = 225;
	design.p_blind_left = 125;
	design.p_blind_right = 235;
	design.p_firing_delay = 4;
	design.t_firing_delay = 4;

	save_design();
}

double round(x)
double x;
{
	return( floor(x + 0.5));
}

save_design()
{
	int fd, bytes;
	char path[BUFSIZ];
	char *home, *getenv();

	if ((home = getenv("HOME")) != NULL)
		strcpy(path, home);
	else
		strcpy(path, ".");
	
	strcat(path, "/.trek");
	strcat(path, design.abbr);
	printf("Saving to file %s\n", path);
	
	if ((fd = open(path, O_WRONLY|O_CREAT, 0644)) < 0) {
		perror("open");
		exit(1);
	}
	bytes = write(fd, (char *)&design, sizeof(struct ship_stat));
	if (bytes != sizeof(struct ship_stat)) {
		fprintf(stderr, "Wrote only %d, not %d bytes\n", bytes,
		    sizeof(struct ship_stat));
		unlink(path);
		exit(1);
	}
	bytes = write(fd, &stuff, sizeof(stuff));
	bytes = write(fd, &cloak, 1);
	bytes = write(fd, (char *)&bpv, sizeof(int));
	close(fd);
}
