/* plot.c -- plot routines for visual star trek */

#include "vtrek.h"

/* replot screen */
replot()
{
	cls();
	plt_stat(ALL);
	plt_srs(ALL);
	plt_dam(ALL);
	plt_gal(ALL);
	moveyx(12, 18);
	printf("READOUT");
	moveyx(13, 18);
	printf("-------");
	plt_num(ALL);
}

/* plot status (upper left) */
plt_stat(op, item)
int op, item;
{
	static char *text[9] = {
	    "      Status", "      ------", "Stardate     :",
	    "Condition    :", "Quadrant     :", "Sector       :",
	    "Energy       :", "Photon torps :", "Shields      :"
	};
	static char *ctext[4] = {
	    "Green", "Yellow", "Red", "Docked"
	};
	int i, high, low;

	if (op & TEXT)
	    for (i = 0; i < 9; i++) {
		moveyx(i + 2, 1);
		printf("%s", text[i]);
	    }

	if (op & (INFO | ELEMENT)) {
	    if (op & INFO) {
		low = STARDATE;
		high = SHIELDS;
	    }
	    else {
		low = item;
		high = item;
	    }

	    for (i = low; i <= high; i++) {
		switch (i) {
		case STARDATE :
		    moveyx(4, 16);
		    printf("%-.1f", stardate);
		    break;
		case CONDITION :
		    moveyx(5, 16);
		    printf("%-6s", ctext[condition]);
		    break;
		case QUADRANT :
		    moveyx(6, 16);
		    printf("[%d,%d]", xquad + 1, yquad + 1);
		    break;
		case SECTOR :
		    moveyx(7, 16);
		    printf("[%d,%d]", xsect + 1, ysect + 1);
		    break;
		case ENERGY :
		    moveyx(8, 16);
		    printf("%-4d", energy);
		    break;
		case TORPS :
		    moveyx(9, 16);
		    printf("%-2d", torps);
		    break;
		case SHIELDS :
		    moveyx(10, 16);
		    printf("%-4d", shields);
		    break;
		}
	    }
	}
}

/* plot short range scan */
plt_srs(op, xs, ys)
int op, xs, ys;
{
	static char *htext = "-1--2--3--4--5--6--7--8-";
	static char *stext[6] = {
	    "   ", "<K>", "<S>", " * ", "???", " + "
	};
	int i, j;

	if (op & TEXT) {
	    moveyx(1, 28);
	    printf("%s", htext);
	    for (i = 1; i < 9; i++) {
		moveyx(i + 1, 27);
		printf("%d", i);
		moveyx(i + 1, 52);
		printf("%d", i);
	    }
	    moveyx(10, 28);
	    printf("%s", htext);
	}

	strcpy(stext[PLAYER], playership);

	if (op & INFO) {
	    for (i = 0; i < 8; i++) {
		moveyx(i + 2, 28);
		for (j = 0; j < 8; j++)
		    printf("%s", stext[(damage[SRS] <= 0) ? EMPTY : quadrant[j][i]]);
	    }
	}
	else if (op & ELEMENT) {
	    moveyx(ys + 2, 28 + 3 * xs);
	    printf("%s", stext[(damage[SRS] <= 0) ? EMPTY : quadrant[xs][ys]]);
	}
}

/* plot damage info */
plt_dam(op, item)
int op, item;
{
	static char *text[10] = {
	    "    Damage Report", "    -------------",
	    "Warp engines    :", "S.R. sensors    :", "L.R. sensors    :",
	    "Phaser control  :", "Damage control  :", "Defense control :",
	    "Computer        :", "Photon tubes    :"
	};
	int i;

	if (op & TEXT)
	    for (i = 0; i < 10; i++) {
		moveyx(i + 1, 56);
		printf("%s", text[i]);
	    }

	if (op & INFO)
	    for (i = 0; i < 8; i++) {
		moveyx(i + 3, 74);
		if (damage[DAMAGE] <= 0)
		    printf("    ");
		else
		    printf("%4d", damage[i]);
	    }
	else if (op & ELEMENT) {
	    moveyx(item + 3, 74);
	    if (damage[DAMAGE] <= 0)
		printf("    ");
	    else
		printf("%4d", damage[item]);
	}
}

/* plot galaxy map */
plt_gal(op, xq, yq)
int op, xq, yq;
{
	static char *htext = "-1- -2- -3- -4- -5- -6- -7- -8-";
	int i, j, fedquad;

	if (op & TEXT) {
	    moveyx(13, 47);
	    printf("%s", htext);
	    for (i = 1; i < 9; i++) {
		moveyx(i + 13, 45);
		printf("%d:", i);
		for (j = 0; j < 7; j++) {
		    moveyx(i + 13, 50 + (j << 2));
		    putch(':');
		}
		moveyx(i + 13, 78);
		printf(":%d", i);
	    }
	    moveyx(22, 47);
	    printf("%s", htext);
	}

	if (op & INFO) {
	    for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) {
		    moveyx(i + 14, 47 + (j << 2));
		    if (damage[COMPUTER] <= 0 || !galaxy[j][i].known)
			printf("   ");
		    else
			printf("%01d%01d%01d", galaxy[j][i].nkling, galaxy[j][i].nbase,
			    galaxy[j][i].nstar);
		}
	    moveyx(yquad + 14, 46 + (xquad << 2));
	    putch('[');
	    moveyx(yquad + 14, 50 + (xquad << 2));
	    putch(']');
	}
	else if (op & ELEMENT) {
	    moveyx(yq + 14, 46 + (xq << 2));
	    fedquad = (xq == xquad && yq == yquad);
	    putch(fedquad ? '[' : ':');
	    if (damage[COMPUTER] <= 0)
		printf("   ");
	    else
		printf("%01d%01d%01d", galaxy[xq][yq].nkling, galaxy[xq][yq].nbase, galaxy[xq][yq].nstar);
	    putch(fedquad ? ']' : ':');
	}
}

/* plot number of star bases & klingons */
plt_num(op)
int op;
{
	float kf;

	if (op & TEXT) {
	    moveyx(23, 47);
	    printf("Base stars = ");
	    moveyx(23, 63);
	    printf("Klingons = ");
	    moveyx(24, 54);
	    printf("Kill Factor = ");
	}

	if (op & INFO) {
	    moveyx(23, 59);
	    printf(" %d", numbases);
	    moveyx(23, 73);
	    printf(" %d/%d  ", numkling, begkling);
	    moveyx(24, 68);
	    if (begdate != stardate)
		kf = (begkling - numkling) / (stardate - begdate);
	    else
		kf = 0.0;
	    printf("%5.3f  ", kf);
	}
}

/* change readout */
readout(op, str)
int op;
char *str;
{
	int i, j;

	switch (op) {

	case CLEAR :		/* clear readout */
	    for (i = 14; i <= 13 + rolines; i++) {
		moveyx(i, 1);
		for (j = 0; j < 44; j++)
		    putch(' ');
	    }
	    rolines = 0;
	    break;

	case ADDLINE :		/* add line to readout */
	    if (rolines >= 10)
		readout(CLEAR, NULL);
	    moveyx(14 + rolines, 1);
	    printf("%-.44s", str);
	    rolines++;
	    break;
	}
}
