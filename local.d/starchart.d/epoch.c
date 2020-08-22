/*
 * epoch.c -- convert reduced Yale databases between epochs.
 *		(initial RA and DL fields rewritten, rest of input copied
 *		verbatim, so tool may be used on either reduced Yale dataset)
 *
 * copyright (c) 1987 by Alan Paeth
 *
 * transformations are based on equations appearing in
 * "Celestial BASIC" by Eric Burgess (SYBEX 1982)
 */

#include <stdio.h>
#include <math.h>

#define EPLOW 1850.0
#define EPHI  2100.0
#define DEFEIN 1950.0
#define DEFEOUT 2000.0

#define LINELEN 80

#define DLDEGSEC 3600.0
#define DLMINSEC 60.0
#define RAHRSSEC 54000.0
#define RAMINSEC 900.0
#define RASECSEC 15.0

#define DEGTORAD (3.14159265359/180.0)
#define DSIN(x) (sin((x)*DEGTORAD))
#define DCOS(x) (cos((x)*DEGTORAD))
#define DTAN(x) (tan((x)*DEGTORAD))

#define USAGE "{inputepoch=1950.0} {outputepoc=2000.0} <stdin >stdout"

char buf[LINELEN];	/* global to carry remainer of input line easily */

main(argc, argv)
    char **argv;
    {
    float r, d, r2, d2, ein, eout;
    ein  = DEFEIN;
    eout = DEFEOUT;
    switch (argc)
	{
	default: fprintf(stderr, "usage: [%s] %s\n", argv[0], USAGE); exit(1);
	case 3: eout = atof(argv[2]);
	case 2: ein =  atof(argv[1]);
	case 1: break;
	}
    if ((ein < EPLOW) || (ein > EPHI) || (eout < EPLOW) || (eout > EPHI))
	{
	fprintf(stderr, "epoch not in range [%.1f..%.1f]\n", EPLOW, EPHI);
	exit(1);
	}
    while(readline(&r, &d))
	{
	xform(r, d, &r2, &d2, ein, eout);		/* r2d2 ?! */
	writeline(r2, d2);
	}
    exit(0);
    }

readline(ra, de)
    float *ra, *de;
    {
    float rah, ram, ras, dld, dlm;
    char sign;

    fgets(buf, LINELEN, stdin);
    if (feof(stdin)) return(0);
    sscanf(buf, "%2f%2f%2f%c%2f%2f", &rah, &ram, &ras, &sign, &dld, &dlm);
    *ra = (RAHRSSEC*rah + RAMINSEC*ram + RASECSEC*ras)/DLDEGSEC;
    *de = (DLDEGSEC*dld + DLMINSEC*dlm)/DLDEGSEC;
    if (sign == '-') *de = -(*de);
    return(1);
    }

writeline(ra, de, rst)
    float ra, de;
    char *rst;
    {
    int rah, ram, ras, dld, dlm, sign;
    ra *= DLDEGSEC;
    rah = ra/RAHRSSEC;
    ra -= rah*RAHRSSEC;
    ram = ra/RAMINSEC;
    ra -= ram*RAMINSEC;
    ras = ra/RASECSEC;
    sign = (de < 0.0);
    if (sign) de = -de;
    dld = de;
    de -= dld;
    dlm = de * DLMINSEC + 0.5;
    if (dlm >= 60) dlm = 59;
    printf("%02d%02d%02d%s%02d%02d%s",
	rah, ram, ras, sign ? "-":"+", dld, dlm, &buf[11]);
    }

xform(rin, din, rout, dout, ein, eout)
    float rin, din, *rout, *dout, ein, eout;
    {
    float t, t2, x, y, z, w, d;
    t2 = ( (ein+eout)/2.0 - 1900.0 ) / 100.0;
    x = 3.07234 + (.00186 * t2);
    y = 20.0468 - (.0085 * t2);
    z = y/15;
    t = eout-ein;
    w = .0042 * t * (x + (z * DSIN(rin) * DTAN(din)) );
    d = .00028 * t * y * DCOS(rin);
    *rout = rin + w;
    if (*rout >= 360.0) *rout -= 360.0;
    if (*rout < 0.0) *rout += 360.0;
    *dout = din + d;
    }
