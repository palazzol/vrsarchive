/*
 * starchart.c -- version 2, September 1987
 *		revision 2.1 December, 1987
 *
 * (c) copyright 1987 by Alan Paeth (awpaeth@watcgl)
 */

/*
 ! Version 2 modification authors:
 !
 !   [a] Petri Launiainen, pl@intrin.FI (with Jyrki Yli-Nokari, jty@intrin.FI)
 !   [b] Bob Tidd, inp@VIOLET.BERKELEY.EDU
 !   [c] Alan Paeth, awpaeth@watcgl
 !
 ! Changes and Additions:
 !
 !a   [1] STARFILE, PLANETFILE, etc. path designations are now in the Makefile
 !ac  [2] Fractional Ra and Decl values are now hh.mm and mm.ss (base 60)
 !a   [3] Command syntax now allows flags as well as old style (arg placement)
 !ab  [4] Extended Yale database (courtesy B. Tidd) also supported on input
 !bc  [5] Greek+Flamsteed numbers now appear as a hardcopy annotation
 !bc  [6] Constellation file finds ra,dl,scale from command line pattern
 !c   [7] New symbols for Nebulae, Galaxies and Clusters added (based on [4])
 !c   [8] Double star indications added (provisions for variable stars) ([4])
 !a   [9] Limiting magnitude flags for stars and labels allowed
 !c  [10] Legend column reformatted to contain additional symbols [4]
 !a  [11] Internal symbol compiler created for raster glyph creation
 !c  [12] add Yale records for "moveto"/"drawto" and text annotations
 !c  [13] added -f file.star to add ephemeride data
 !
 ! Bug Fixes:
 !
 !a   [1] no more core dumps (on SysV) when given bad command line parameters
 !c   [2] Negative RA indications now "wrap" to 24hrs, etc.
 !bc  [3] Mag values *rounded* to nearest unit value (common Atlas convention)
 !c   [4] Removed any device dependent subroutines (eg. getopt).
 !
 ! Final integration by the original author [c].
 ! Questions, suggestions, and fixes should be e-mailed to him.
 */

 /* Version 2.1 patches
 !
 ! patched December, 1987 by Alan Paeth (awpaeth@watcgl),
 ! based on revisions by Craig Counterman (ccount@royal.mit.edu)
 ! et al.
 !
 ! [1] clipping of scaling, ra and decl now takes place
 ! [2] negative magnitude bug fixed; plus format change to yale.star
 ! [3] string arrays ras[2] changed to ras[20] (typo in "chartlegend()")
 ! [4] scanf now uses %f and floats exclusively (for IRIS and sysV)
 ! [5] bigmaster facility added for full-page output
 ! [6] multiple .star files now accepted by -f flag
 ! [7] the -y flag allows overriding of default yale.star dataset
 ! [8] the -n flag allows overriding of default messier.star dataset
 ! [9] the -w flag allows overriding of default planet.star dataset
 ![10] the -a and -o flags allow for separate/sorted object/annotation detail
 ![11] the -g flag sets a mag limit for star proper names, else use codes
 */

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>
#include "starchart.h"

/*
 * default datasets are local, unless defined in Makefile
 */

#ifndef STARFILE
#define STARFILE	"./yale.star"
#endif

#ifndef PLANETFILE
#define PLANETFILE	"./planet.star"
#endif

#ifndef MESSFILE
#define MESSFILE	"./messier.star"
#endif

#ifndef CONSTFILE
#define CONSTFILE	"./con.locs"
#endif


#define DCOS(x) (cos((x)*3.14159265354/180.0))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define FALSE 0
#define TRUE 1

#define MAXUFILES 10	/* Maximum number of user supplied files */
#define THUMBDIFF 2.5	/* thumbnail mag lim is brighter */
#define THUMBMAX 5.0	/* but no lower than this value */
#define THUMBRATIO 1.2	/* and scaled by this size (yet is already wider) */
#define THUMBDEG 10.0	/* but this is the floor on finder scale */
#define FSIZESML 8	/* point size for star designation */
#define FSIZELRG 10	/* point size for other object annotations */

float max(), modup(), moddown();
float ftod(), htod();
/* double atof(); */		/* possible system compatability problem */

/* globals */

char *progname;			/* program name for errors */
float ra, de, sc;		/* global chart attributes */
char *title;
char *starfile = STARFILE;
char *planetfile = PLANETFILE;
char *messfile = MESSFILE;
char *constfile = CONSTFILE;
char *userfile[MAXUFILES];	/* potential .star file of user ephemerides */
int nufiles = 0;		/* number of userfiles */
int bigflag = FALSE;		/* chart page only, no thumbnail or legend */
int annodetail, objdetail;	/* flags to control output */
int annoswitch, objswitch;	/* switches as seen on command line */
float olat, olon;		/* save areas for track clipping */		

/* the code */

main(argc, argv)
    int argc;
    char *argv[];
    {
    int chartpasses, chartcount;

    commandline(argc, argv);
/*
 * copy command line defaults into all possible charts
 */
    thumbnail.lbllim = bigmaster.lbllim = master.lbllim;
    thumbnail.maglim = bigmaster.maglim = master.maglim;
    thumbnail.gklim = bigmaster.gklim  = master.gklim;
/*
 * drawing order:
 *
 * (1) chart outlines
 * (2) stars a/o
 * (3) annotations
 * (4) legend
 */
    vecopen();
    chartparms(&master, sc);
    chartparms(&bigmaster, sc);
/*
 * thumbnail gets fine-tuned scale and brighter limiting magnitude
 */
    chartparms(&thumbnail, MAX(sc * THUMBRATIO, THUMBDEG));
    thumbnail.maglim = MIN(THUMBMAX, master.maglim - THUMBDIFF);
/*
 * background stuff
 */
    annodetail = TRUE;		/* no omissions on background */
    objdetail = TRUE;
    if (bigflag) chartbackground(&bigmaster);
    else
	{
	chartbackground(&master);
	chartbackground(&thumbnail);
	}
/*
 * main drawing
 */
    chartpasses = (annoswitch && objswitch) ? 2 : 1;
    if (!annoswitch && !objswitch) annoswitch = objswitch = TRUE; /* default */
    for(chartcount=0; chartcount<chartpasses; chartcount++)
	{
/*
 * set detail control flags from command line switches and pass number
 */
	annodetail = annoswitch;
	objdetail =  objswitch;
	if ((chartpasses == 2) && (chartcount == 0)) annodetail = FALSE;
	if ((chartpasses == 2) && (chartcount == 1)) objdetail = FALSE;
/*
 * drawing
 */
	if (bigflag) chartall(&bigmaster);
	else
	    {
	    chartall(&master);
	    chartall(&thumbnail);
	    }
/*
 * (mini)legend
 */
	annodetail = TRUE;	/* legend has no omissions */
	objdetail = TRUE;
	if (bigflag) chartbanner(&bigmaster);
	else chartlegend(&master);
	}
    vecclose();
    exit(0);
    }

commandline(argc, argv)
    char *argv[];
    {
    int j;
    static char *usage =
"\nusage:\tstar* [ Ra Dcl Scale Title Maglim Labellim ]\nor\tstar* [ -r Ra -d Dcl -s Scale -t Title -m Maglim -l Labellim -f x.star ]\nor\tstar*  [ -c con (3 or 4 letters chosen from con.locs) -l ... ]";
    progname = argv[0];
    title = NULL;
    annoswitch = objswitch = FALSE;
    ra = 0.0;
    de = 0.0;
    sc = 0.0;
/*
 * check command line style
 */
    if (argc == 1) die("No arguments - %s", usage);
    if  ( (argv[1][0] == '-') && isalpha(argv[1][1]) )
	{
/*
 * new style command line (using flags)
 */
	for (j=1; j<argc; j++)
	    {
	    if (argv[j][0] != '-') die("unknown argument - %s", argv[j]);
	    switch (argv[j][1])
		{
		case 'c':	/* could be -c con or -c con con.locs */
		    if (((j+2) < argc) && (argv[j+2][0] != '-'))
			{
			constfile = argv[j+2];
			findconst(argv[++j]);
			j++;
			}
		    else findconst(argv[++j]);
		    break;
		case 'r': ra = htod(argv[++j]); break;
		case 'd': de = htod(argv[++j]); break;
		case 's': sc = atof(argv[++j]); break;
		case 'l': master.lbllim = (float)(atof(argv[++j])); break;
		case 'm': master.maglim = (float)(atof(argv[++j])); break;
		case 'g': master.gklim  = (float)(atof(argv[++j])); break;
		case 't': title = argv[++j]; break;
		case 'f': userfile[nufiles++] = argv[++j];
			  if (nufiles >= MAXUFILES) die("too many -f files");
			  break;
		case 'u': die("%s", usage); break;
		case 'y': starfile = argv[++j]; break;
		case 'n': messfile = argv[++j]; break;
		case 'w': planetfile = argv[++j]; break;
/*		case 'p': polarmode = TRUE; break; */	/* someday! */
		case 'b': bigflag = TRUE; break;
		case 'a': annoswitch = TRUE; break;
		case 'o': objswitch = TRUE; break;
		default:  die("unknown switch - %s", argv[j]); break;
		}
	    if (j == argc) die("trailing command line flag - %s", argv[j-1]);
	    }
	}
    else
	{
/*
 * old style command line (position dependent)
 */
	switch (argc)
	    {
	    case 7: master.maglim = atof(argv[6]);
	    case 6: master.lbllim = atof(argv[5]);
	    case 5: title = argv[4];
	    case 4: sc = atof(argv[3]);
	    case 3: de = htod(argv[2]);
	    case 2: ra = htod(argv[1]); break;
	    default: die("too many arguments - %s", usage);
	    }
	}
    }

float ftod(x)
    float x;
    {
    int full;
    full = x;
    return(full + (x-full)*100.0/60.0);
    }

float htod(s)
    char *s;
    {
/*
 * htod(str) reads floating point strings of the form {+-}hh.{mm} thus
 * allowing for fractional values in base 60 (ie, degrees/minutes).
 */
/*
    float x, sign;
    int full, frac;
    char *t;
    t = s-1;
    while(*++t)
	{
	if ((*t == ' ') || (*t = '\t') || (*t == '\n')) break;
	if ( ( (*t<'0') || (*t>'9') ) && (*t!='.') && (*t!='+') && (*t!='-'))
	    die("non-digit in dd.mm style numeric argument: %s", s);
	}
    if (s == NULL) return 0.0;
    full = frac = 0;
    sign = 1.0;
    if (*s == '-')
	{
	sign = -1.0;
	s++;
	}
    else if (*s == '+') s++;
    while (*s && *s != '.') full = 10 * full + *s++ - '0';
    if (*s++ == '.')
	{
	if (*s) frac = 10 * (*s++ - '0');
	if (*s) frac += *s++ - '0';
	if (frac > 59) frac = 59;
	}
    x = (float) full + ((float) frac) / 60.0;
    return sign * x;
*/
    return(ftod(atof(s)));
    }

chartparms(chart, scl)
    float scl;
    map chart;
    {
    float maxdl, adj, xscale;
/*
 * check for bogus ra or decl
 */
    if ((ra >= 24.0) || (ra <    0.0)) die("right ascension out of range");
    if ((de >= 90.0) || (ra <= -90.0)) die("declination out of range");
/*
 * record chart location
 */
    chart->racen = ra * 360.0 / 24.0;
    chart->dlcen = de;
    chart->scale = scl;
    if (chart->scale == 0.0) chart->scale = 15.0;
/*
 * check for pole wrapping and do a potential clip
 */
    maxdl = (90.0 - chart->dlcen)*2.0;
    if (chart->scale > maxdl) chart->scale = maxdl;
/*
 * locate chart limits
 */
    chart->north = (chart->dlcen + chart->scale / 2.0);
    chart->south = (chart->dlcen - chart->scale / 2.0);
/*
 * xscale is other than chart->scale in order to widen the horizontal viewing
 * area, which otherwise shrinks near the poles under Sanson's projection
 * this happens in polar maps which do not span the celestial equator
 */
    adj = 1.0;
    if (chart->north * chart->south > 0.0)
	adj = max(DCOS(chart->north), DCOS(chart->south));
    xscale = chart->scale/adj;

    chart->east  = (chart->racen + xscale*chart->ww/chart->wh / 2.0);
    chart->west  = (chart->racen - xscale*chart->ww/chart->wh / 2.0);
    chart->yscale = chart->wh / chart->scale;
    }

chartlegend(chart)
    map chart;
    {
    char ras[20], dls[20], outstr[40];
    if (!title) title = "LEGEND";
    rastr(ras, chart->racen);
    declstr(dls, chart->dlcen);
    sprintf(outstr, "(%s,%s lim: %2.1f)", ras, dls, chart->maglim);
/*
 * there are reports that large point sizes (eg "16", below) cause characters
 * to overlap on some (pic?) output devices. To fix, set values to "10".
 */
    pvecsize(16); pvecsyms(65, 220, title);
    pvecsize(10); pvecsyms(65, 185, outstr);

    pdrawStar( 65, 150, 0, 'S', NULL);
    pvecsize(12);
    pvecsyms(  95, 150,"<0.5");
    if (chart->maglim >= 0.5)
	{
	pdrawStar(230, 150, 1, 'S', NULL);
	pvecsize(10);
	pvecsyms( 260, 150,"<1.5");
	}
    if (chart->maglim >= 1.5)
	{
	pdrawStar( 65, 125, 2, 'S', NULL);
	pvecsize( 9);
	pvecsyms(  95, 125,"<2.5");
	}
    if (chart->maglim >= 2.5)
	{
	pdrawStar(230, 125, 3, 'S', NULL);
	pvecsize( 8);
	pvecsyms(260, 125,"<3.5");
	}
    if (chart->maglim >= 3.5)
	{
	pdrawStar( 65, 100, 4, 'S', NULL);
	pvecsize( 7);
	pvecsyms(  95, 100,"<4.5");
	}
    if (chart->maglim > 4.5)
	{
	pdrawStar(230, 100, 5, 'S', NULL);
	pvecsize( 6);
	pvecsyms(260, 100,">4.5");
	}

    pvecsize(10); pvecsyms( 95,75,"double");   pdrawStar( 65,75, 2, 'D', NULL);
    pvecsize(10); pvecsyms(260,75,"variable"); pdrawStar(230,75, 2, 'V', NULL);
    pvecsize(10); pvecsyms( 95,50,"planet");   pdrawPlan( 65,50, 1, 'S', NULL);
    pvecsize(10); pvecsyms(260,50,"galaxy");   pdrawGalx(230,50, 1, 'E', NULL);
					       pdrawGalx(205,50, 1, 'S', NULL);
    pvecsize(10); pvecsyms( 95,25,"nebula");   pdrawNebu( 65,25, 1, 'D', NULL);
					       pdrawNebu( 40,25, 1, 'P', NULL);
    pvecsize(10); pvecsyms(260,25,"cluster");  pdrawClus(230,25, 1, 'O', NULL);
					       pdrawClus(205,25, 1, 'G', NULL);
    }

chartbanner(chart)
    map chart;
    {
    char ras[20], dls[20], outstr[100];
    if (!title) title = "LEGEND";
    rastr(ras, chart->racen);
    declstr(dls, chart->dlcen);
    pvecsize(8);
    sprintf(outstr, "%s: %s,%s lim: %2.1f", title, ras, dls, chart->maglim);
    pvecsyms(15, 15, outstr);
    }

readstar(file, lat, lon, mag, code, subcode, color, label, name)
    FILE *file;
    float *lat, *lon, *mag;
    char *code, *subcode, *color, *label, *name;
    {
#define LINELEN 80
    char sbuf[LINELEN+1], *ptr;
    float rah, ram, ras, dld, dlm, dl, inten;
    int len, i;
/*
 * file formats:
 * new
064509-1643-14SDA1a CMASirius
051432-0812015SDB8b ORIRigel
 * old
064509-1643-146SSSirius
051432-08120015SSRigel
 */
    fgets(sbuf, LINELEN, file);
    if (feof(file)) return(1);
/*
 * sscanf of floats is TOOO slow:
 *     sscanf(sbuf, "%2f%2f%2f%c%2f%2f ... );
 * use alternate:
 */
#define F2(i) ((float)((sbuf[i]-'0')*10+sbuf[i+1]-'0'))
#define F3(i) ((float)((sbuf[i]-'0')*100+(sbuf[i+1]-'0')*10+sbuf[i+2]-'0'))
#define F4(i) ((float)((sbuf[i]-'0')*1000+(sbuf[i+1]-'0')*100+(sbuf[i+2])-'0')*10+sbuf[i+3]-'0')
    rah = F2(0);
    ram = F2(2);
    ras = F2(4);
    dld = F2(7);
    dlm = F2(9);
/*
 * common code
 */
#define DLDEGSEC 3600.0
#define DLMINSEC 60.0
#define RAHRSSEC 54000.0
#define RAMINSEC 900.0
#define RASECSEC 15.0

    *lon = (RAHRSSEC*rah + RAMINSEC*ram + RASECSEC*ras)/DLDEGSEC;
    dl = (DLDEGSEC*dld + DLMINSEC*dlm)/DLDEGSEC;
    *lat = (sbuf[6]  == '-') ? -dl : dl;
    if  (isdigit(sbuf[14]))
	{
/*
 * old reduced Yale catalog
 */
	inten = F3(12);
	if (sbuf[11] == '0' || sbuf[11] == '+') *mag = inten/100.0;
	else if (sbuf[11] == '-') *mag = -inten/100.0;
	else *mag = F4(11)/1000.0;	/* new feature for stars >= 10.0 mag */
	if (sbuf[11] != 0)
	code[0] = sbuf[15];
	subcode[0] = sbuf[16];
	color [0] = '\0'; strcpy (color, "  "); /* set unknowns to blanks */
	label [0] = '\0';
	name [0] = '\0'; strncat (name, &sbuf[17], strlen (&sbuf [17]) - 1);
	}
    else
	{
/*
 * new reduced Yale catalog
 */
	*mag = ((sbuf[11] == '-') ? -F2(12)/10.0 : F3(11))/100.0;
	code[0] = sbuf[14];	/* let's get Sirius */
	subcode[0] = sbuf[15];
	color [0] = '\0';
	label [0] = '\0';
	name [0] = '\0';
	switch (code[0])
	    {
	    case 'C':
	    case 'N':
	    case 'G':
/*
 * name vs label:
 *
 * A "name" is a common English term for a star or cosmic object (eg Polaris,
 * Pleiades). A label is an official designation for such an object (eg
 * alpha-Ursa Minor, M45). For stars, labels are taken from the two column
 * Bayer/Flamsteed field in the new reduced label format and the proper name
 * from the "name" field (the label field might also include the three letter
 * IAU constellation designation, but at present these are ignored)
 *
 * For non-stellar objects, the Bayer and IAU fields are typically blank (and
 * always ignored), and the "name" field consists of the designation followed
 * by an optional proper name, or perhaps just an annotation, the latter
 * flagged by a leading "," and ignored by the software. For instance, the
 * entry "m45 Pleiades" forms a label "m45" and a name "Pleiades", but the
 * entry "m6  ,butterfly shape" forms merely the label "m6".
 *
 * At the present, the charting software which is supplied this data on a call
 * to "readstar" will print names in favor of labels.
 */
/*
 * extract Messier number and name
 */
		strcpy (color, "  ");
		ptr = &sbuf[23];
		i = 0;
		while (isalnum (*ptr)) label[i++] = *ptr++;

		label[i] = '\0';
		i = 0;
		while (*ptr == ' ') ptr++;
		while (*ptr != ',' && *ptr != '\n' && *ptr)
		    name[i++] = *ptr++;
		name[i] = '\0';
		break;
	default:
/*
 * extract color, label and name
 */
		strncat (color, &sbuf[16], 2);
		strncat (label, &sbuf[18], strlen (&sbuf [18]) - 1);
		if  ((len = strlen (label)) > 5)
			{
			strncat (name, &label[5], len - 5);
			label [5] = '\0';
			}
	    }
	}
    return(0);
    }

xform(chart, lat, lon, xloc, yloc)
    map chart;
    float lat, lon;
    int *xloc, *yloc;
    {
 /*
  * This is Sanson's Sinusoidal projection. Its properties:
  *   (1) area preserving
  *   (2) preserves linearity along y axis (declination/azimuth)
  */
    *xloc = chart->wx+chart->ww/2 + (chart->racen-lon)*chart->yscale*DCOS(lat);
    *yloc = chart->wy + (int)((lat - chart->south) * chart->yscale);
    }

chartall(chart)
    map chart;
    {
/*
 * we might "die" after drawing the borders, but we want stars to overlay
 * the border for imaging software which may handle the bottom-up 2-1/2D
 * order of printing correctly (eg PostScript).
 */
    int i;
    if (!chartfile(chart, starfile)) die("open fail on %s", starfile);
    chartfile(chart, planetfile);
    chartfile(chart, messfile);
    for (i = 0; i < nufiles; i++)
	{
	if (!chartfile(chart,userfile[i])) die("open fail on %s", userfile[i]);
	}
    }

chartfile(chart, filename)
    map chart;
    char *filename;
    {
#define READMODE "r"
#define OPENFAIL 0
    FILE *sfile;
    if ((sfile = fopen(filename, READMODE)) == OPENFAIL) return(0);
    chartobjects(chart, sfile);
    fclose(sfile);
    return(1);
    }

chartobjects(chart, file)
    map chart;
    FILE *file;
    {
    float lat, lon, mag;
    char code[1], subcode[1], label[100], name [100], color [3], *ptr;
    int xloc, yloc, staronly, smallflag, vecmode, gkflag;

    for(;;)
	{
	if (readstar(file,&lat,&lon,&mag,code,subcode,color,label,name)) break;
	if ((mag > chart->maglim) && (code[0] == 'S')) break;
	if ((chart->west < 0.0) && (lon>180.0)) lon -= 360.0;
	if ((chart->east > 360.0) && (lon<180.0)) lon += 360.0;
	if ( ( (lon >= chart->west) && (lon <= chart->east) &&
	       (lat >= chart->south) && (lat <= chart->north) &&
	       (mag <= chart->maglim) )
	    || (code[0] == 'V') )	/* no spatial preclip on vector data */
	    {
	    xform(chart, lat, lon, &xloc, &yloc);
	    smallflag = vecmode = staronly = gkflag = 0;
	    switch(code[0])
		{
    case 'S':	pdrawStar(xloc,yloc,(int)(mag+0.5),subcode[0],color);
		staronly = 1;
		break;
    case 'P':	pdrawPlan(xloc, yloc,(int)(mag+0.5),subcode[0],color);
		break;
    case 'N':	pdrawNebu(xloc, yloc,(int)(mag+0.5),subcode[0],color);
		break;
    case 'G':	pdrawGalx(xloc, yloc,(int)(mag+0.5),subcode[0],color);
		break;
    case 'C':	pdrawClus(xloc, yloc,(int)(mag+0.5),subcode[0],color);
		break;
    case 'I':	break;		/* invisible */
    case 'V':	vecmode = 1;	/* vector: check subcodes */
		if (subcode[0] == 'M') /* move */
		    {
		    olat = lat;
		    olon = lon;
		    }
		else			/* draw */
		    {
		    float lat1, lon1, lat2, lon2;
		    if (clip(chart->north, chart->south, chart->west,
				chart->east, olon, olat, lon, lat,
				&lon1, &lat1, &lon2, &lat2))
			{
			xform(chart, lat1, lon1, &xloc, &yloc);
			pvecmove(xloc, yloc);
			xform(chart, lat2, lon2, &xloc, &yloc);
			switch (subcode[0])
			    {
		case 'D':   pvecdrawdot(xloc, yloc); break;
		case 'H':   pvecdrawhyph(xloc,yloc); break;
		case 'S':   /* solid */
		default:    pvecdraw(xloc, yloc); break;
			    }
			}
		    olat = lat;
		    olon = lon;
		    }
		break;
		}
/*
 * pick up object name or label if (star<limit), (nonstar), and (not vector)
 */
	    if (((mag < chart->lbllim) || !staronly) && !vecmode)
		{
		ptr = NULL;
/*
 * if star is too "dark", mask off any proper name, so only label can appear.
 */
		if (staronly && (mag > chart->gklim)) *name = '\0';
/*
 * choose any proper name in favor of label
 */
		if (*name != '\0') ptr = name;
		else if (*label != '\0')
		    {
		    ptr = label;
			{
			if (staronly)	/* star: Gk char(s) or Roman digits */
			    {
			    smallflag = 1;
			    ptr[2] = '\0'; /* snuff the IAU designation */
			    if(!isdigit(ptr[0]))
			        {		/* is Bayer (Greek) */
			        gkflag = 1;
			        if (ptr[1] == ' ') ptr[1] = '\0';
			        }
			    }
			}
		    }
		if  (ptr != NULL)
		    {
		    pvecsize( smallflag ? FSIZESML : FSIZELRG);
/*
 * vecsyms* --
 * some x offset present to avoid overstriking corresponding object. Note
 * that some bias is already precent (obj is center-align, text is left align)
 */
		    if (gkflag) pvecsymsgk(xloc+10, yloc, ptr);
		    else pvecsyms(xloc+10, yloc, ptr);
		    }
		}
	    }
	}
    }

/*
 * Chart Construction
 */

chartbackground(chart)
    map chart;
    {
    chartoutline(chart);
    chartgrid(chart);
    }

chartgrid(chart)
    map chart;
    {
    charthgrid(chart, 15.0, 18);
    charthgrid(chart, 5.0, 12);
    charthgrid(chart, 1.0, 6);
    chartvgrid(chart, 10.0, 18);
    chartvgrid(chart, 5.0 , 12);
    chartvgrid(chart, 1.0, 6);
    }

chartoutline(chart)
    map chart;
    {
    float start, inc;
    int xloc, xloc2, yloc, yloc2, div, i;

    xform(chart, chart->south, chart->west, &xloc,  &yloc);
    xform(chart, chart->south, chart->east, &xloc2, &yloc2);
    pvecmovedraw(xloc, yloc, xloc2, yloc2);
    xform(chart, chart->north, chart->west, &xloc,  &yloc);
    xform(chart, chart->north, chart->east, &xloc2, &yloc2);
    pvecmovedraw(xloc, yloc, xloc2, yloc2);

    inc = (chart->north - chart->south);
    div = (int)(inc);
    if (div < 1) div = 1;
    inc /= div;
    start = chart->south;
    xform(chart, start, chart->west, &xloc, &yloc);
    pvecmove(xloc, yloc);
    for (i=0; i < div; i++)
	{
	start += inc;
	xform(chart, start, chart->west, &xloc, &yloc);
	pvecdraw(xloc, yloc);
	}
    start = chart->south;
    xform(chart, start, chart->east, &xloc, &yloc);
    pvecmove(xloc, yloc);
    for (i=0; i < div; i++)
	{
	start += inc;
	xform(chart, start, chart->east, &xloc, &yloc);
	pvecdraw(xloc, yloc);
	}
    }

rastr(str, ras)
    char *str;
    float ras;
    {
    int hrs, min;
    if (ras <   0.0) ras += 360.0;
    if (ras > 360.0) ras -= 360.0;
    hrs = (int)(ras/15.0);
    min = (int)((ras - hrs * 15.0) * 4.0);
    sprintf(str, "%2dh", hrs);
    if (min) sprintf(str, "%s%02dm", str, min);
    }

declstr(str, dl)
    char *str;
    float dl;
    {
    int deg, min;
    if (dl == 0.0) sprintf(str, "%s", " ");
    else if (dl > 0.0) sprintf(str, "%s", "+");
	else
	{
	sprintf(str, "%s", "-");
	dl = -dl;
	}
    deg = (int)(dl);
    min = (int)((dl - deg) * 60.0);
    sprintf(str, "%s%02dd", str, deg);
    if (min) sprintf(str, "%s%02dm", str, min);
    }

charthgrid(chart, inc, hgt)
    map chart;
    float inc;
    {
#define HTICKLIM 2
#define HTEXTLIM 80
    float start, stop, ras;
    int xloc, xloc2, yloc, xloc3, yloc3;
    start = modup(chart->west, inc);
    stop = moddown(chart->east, inc);
    xform(chart, chart->south, start, &xloc, &yloc);
    xform(chart, chart->south, start+inc, &xloc2, &yloc);
    if (xloc - xloc2 > HTICKLIM)
	for (ras = start; ras <= stop; ras += inc)
	    {
	    xform(chart, chart->south, ras, &xloc3, &yloc3);
	    pvecmovedraw(xloc3, yloc3-hgt, xloc3, yloc3);
	    if (xloc - xloc2 > HTEXTLIM)
		{
		char tstr[20];
		rastr(tstr, ras);
		pvecsize(10);
		pvecsyms(xloc3+2, yloc-17, tstr);
		}
	    }
    }

chartvgrid(chart, inc, wid)
    map chart;
    float inc;
    {
#define VTICKLIM 2
#define VTEXTLIM 20
    float start, stop, dl;
    int xloc, yloc, yloc2, xloc3, yloc3;
    start = modup(chart->south, inc);
    stop = moddown(chart->north, inc);
    xform(chart, start, chart->west, &xloc, &yloc);
    xform(chart, start+inc, chart->west, &xloc, &yloc2);
    if (yloc2 - yloc > VTICKLIM)
	{
	for (dl = start; dl <= stop; dl += inc)
	    {
	    xform(chart, dl, chart->west, &xloc3, &yloc3);
	    pvecmovedraw(xloc3, yloc3, xloc3+wid, yloc3);
	    if (yloc2 - yloc > VTEXTLIM)
		{
		char tstr[20];
		declstr(tstr, dl);
		pvecsize(10);
		pvecsyms(xloc3+24, yloc3, tstr);
		}
	    }
	}
    }

/*
 * General Utilities
 */

float max(a, b)
    float a, b;
    {
    if (a>b) return(a);
    return(b);
    }

float modup(a, b)
    float a, b;
    {
    float new;
    new = ((float)((int)(a/b))*b);
    if (new >= a) return(new);
    return(new += b);
    }

float moddown(a, b)
    float a, b;
    {
    float new;
    new = ((float)((int)(a/b))*b);
    if (new <= a) return(new);
    return (new -= b);
    }

die(a,b)
    char *a, *b;
    {
    fprintf(stderr,"%s: ", progname);
    fprintf(stderr,a,b);
    fprintf(stderr,"\n");
    exit(1);
    }

#define LINELEN 80
static char legend[LINELEN];

findconst(tag)
char *tag;
     {
/*
 * lookup "con.locs" for a matching tag, and then substitute initial values
 * for ra, decl, scale, and label. File layout follows:
 *
 * com  13    25   10   Coma Bereneces
 * cor  15.45 28   10   Corona Borealis
 * 0....+....1....+....2....+....3
 */
    char *newline;
    FILE *cfile;
    int taglen;
    char cbuf[LINELEN+1];

    if ((cfile = fopen(constfile, "r")) == NULL)
	die("open fail on %s", constfile);
    taglen = strlen(tag);
    if ((taglen < 3) || (taglen > 4))
	die("constellation name must be three or four characters");
    for (;;)
	{
	fgets(cbuf, LINELEN, cfile);
	if (ferror(cfile)) die("read error in %s", constfile);
	if (feof(cfile)) break;
	if (strncmp(tag, cbuf, taglen) == 0)	/* FOUND */
	    {	
	    if (4!=sscanf(cbuf,"%*5s%f%f%f %[^\n]", &ra, &de, &sc, legend))
		die("bogus line in constellation file: %s", cbuf);
	    ra    = ftod(ra);
	    de    = ftod(de);
	    if ((newline=index(legend, '\n')) != 0) *newline = '\0';
	    title = legend;
	    return;
	    }
	}
    die("Constellation '%s' not found", tag);
    }

/*
 * intercept vector and text functions
 * (used for omission of detail, but generally useful for clipping, etc.)
 */

pvecsize(points)
    int points;
    {
    vecsize(points);
    }

pvecmove(x, y)
    {
    vecmove(x, y);
    }

pvecdrawdot(x, y)
    {
    if (objdetail) vecdrawdot(x, y);
    }

pvecdrawhyph(x, y)
    {
    if (objdetail) vecdrawhyph(x, y);
    }

pvecdraw(x, y)
    {
    if (objdetail) vecdraw(x, y);
    }

pvecsyms(x, y, s)
    char *s;
    {
    if (annodetail) vecsyms(x, y, s);
    }

pvecmovedraw(x, y, x2, y2)
    {
    if (objdetail) vecmovedraw(x, y, x2, y2);
    }

pdrawPlan(x, y, mag, type, color)
    char type, *color;
    {
    if (objdetail) drawPlan(x, y, mag, type, color);
    }

pdrawStar(x, y, mag, type, color)
    char type, *color;
    {
    if (objdetail) drawStar(x, y, mag, type, color);
    }

pdrawGalx(x, y, mag, type, color)
    char type, *color;
    {
    if (objdetail) drawGalx(x, y, mag, type, color);
    }

pdrawNebu(x, y, mag, type, color)
    char type, *color;
    {
    if (objdetail) drawNebu(x, y, mag, type, color);
    }

pdrawClus(x, y, mag, type, color)
    char type, *color;
    {
    if (objdetail) drawClus(x, y, mag, type, color);
    }

pvecsymsgk(str, x, y)
    char *str;
    {
    if (annodetail) vecsymsgk(str, x, y);
    }

/*
 * clipping extentions (awpaeth@watcgl)
 */

#define TOPFLAG 8
#define BOTTOMFLAG 4
#define LEFTFLAG 2
#define RIGHTFLAG 1

int clip(top, bottom, left, right, ix1, iy1, ix2, iy2, ox1, oy1, ox2, oy2)
    float top, bottom, left, right, ix1, iy1, ix2, iy2, *ox1, *oy1, *ox2,*oy2;
    {
    int c, c1, c2;
    float x, y;

    c1 = c2 = 0;
    if (ix1 < left) c1 |= LEFTFLAG; else if (ix1 > right) c1 |= RIGHTFLAG;
    if (iy1 < bottom) c1 |= BOTTOMFLAG; else if (iy1 > top) c1 |= TOPFLAG;
    if (ix2 < left) c2 |= LEFTFLAG; else if (ix2 > right) c2 |= RIGHTFLAG;
    if (iy2 < bottom) c2 |= BOTTOMFLAG; else if (iy2 > top) c2 |= TOPFLAG;

    while (c1 || c2)
	{
	if (c1 & c2) return(0);	/* bitwise AND, not statement AND */
	c = c1 ? c1 : c2;
	
	if (c & LEFTFLAG)
	    y = iy1 + (iy2 - iy1) * ( (x = left) - ix1) / (ix2 - ix1);
	else if (c & RIGHTFLAG)
	    y = iy1 + (iy2 - iy1) * ( (x = right) - ix1) / (ix2 - ix1);
	else if (c & TOPFLAG)	
	    x = ix1 + (ix2 - ix1) * ( (y = top) - iy1) / (iy2 - iy1);
	else if (c & BOTTOMFLAG)	
	    x = ix1 + (ix2 - ix1) * ( (y = bottom) - iy1) / (iy2 - iy1);
	
	if (c == c1)
	    {
	    ix1 = x;
	    iy1 = y;
	    c1 = 0;
	    if (x < left) c1 |= LEFTFLAG; else if (x > right) c1 |= RIGHTFLAG;
	    if (y < bottom) c1 |= BOTTOMFLAG; else if (y > top) c1 |= TOPFLAG;
	    }
	else
	    {
	    ix2 = x;
	    iy2 = y;
	    c2 = 0;
	    if (x < left) c2 |= LEFTFLAG; else if (x > right) c2 |= RIGHTFLAG;
	    if (y < bottom) c2 |= BOTTOMFLAG; else if (y > top) c2 |= TOPFLAG;
	    }
	}
/*  if ((ix1 == ix2) && (iy1 == iy2)) return(0); */	/* no motion */
    *ox1 = ix1;
    *oy1 = iy1;
    *ox2 = ix2;
    *oy2 = iy2;
    return(1);
    }

/*
 !--------------------------------------------------------------------------
 ! Future Development
 !
 ! Here is my "wish" list of items not added in this version (#2). I have
 ! intentionally included it here with the source code in an attempt to direct
 ! the course of further software development.
 !
 ! It is strongly suggested that all software extentions which will be re-
 ! posted to the general public be first forwarded to the current developer
 ! (presently the author) in an effort to avoid a multiplicity of dissimilar,
 ! but "cannonical" versions.
 !
 ! It is hoped that this will simplify the task of coordinating the eventual
 ! reintegration of new features created by such a large software community.
 ! Wishes are listed in rough order of simplicity and evolution.
 !
 !
 ! Software:
 !
 ! [0] add glyphs for double stars/variable stars + Greek on bitmap devices.
 ! [1] write better PostScript macros.
 !**partially done
 ! [2] integrate a "boundary.star" to draw constellation boundaries.
 ! [3] rewrite "moonphase" to add ra/decl information, merge into "planet.c".
 ! [4] break yale.star into constellation files (and remove "cons" field).
 ! [5] write "orbit.c" for asteroid overlays.
 ! [6] add a polar plot facility (round window?) for high declinations
 !**two previous attempts - success will be met when the output includes
 !**both proper frame boundaries and tick marks, and when the underlying
 !**code uses (and sensibly extends) the functions "xform" and chartparms, etc.
 ! [7] rework planet.star to give planet tracks for a range of dates.
 !**rumored to exist
 !
 ! Database additions:
 !
 ! [1'] convert a larger database (9th-10th mag) into Yale format.
 ! [2'] Milky Way isophots
 ! [3'] Ecliptic track (dotted line vectors)
 ! [4'] IAU Constellation boundaries (see [2])
 !
 ! (comments)
 !
 ! [0] Is best facilitated by revising the image compiler in "starimages.c".
 ! [1] Would allow nice glyphs for non-stellar objects, scaled continuously
 !     to the right size. The code already provides a "halo" feature when
 !     generating stars in bright to dim (large to small) order, following
 !     a common aesthetic convention in many atlases. Variable stars are
 !     given rings, bars pass through double stars, etc., but a more
 !     comprehensive set of glyphs would be nice.
 !
 !     Starchart now allows user specified generation of text and vectors,
 !     which are useful for giving planet tracks across a range of dates, plus
 !     constellation boundaries, and possibly shapes of moon phases ([2], [3]).
 ! [2] I have access to such a database from UC Santa Cruz, and will request
 !     it once proven software is available. It would then be reworked into
 !     the "yale.star" format, using the move/draw commands of.
 ! [3] Burgess' book "Celestial Basic" gives routines for this, but lacks the
 !     precision of the work by Meesus (see planet.man). Done correctly, one
 !     should be able to check for eclipses, transits and occultations (this
 !     will require that "moonphase" know the user's geocentric latitude and
 !     longitude, due to the libration of the moon owing to parallax). This
 !     advanced version would print the profile of the moon's limb, with
 !     terminator. The "basic" version would just give a planet symbol.
 ! [4] Break-down by constellation would be a simple conceptual organization.
 !     It is also helpful to users requiring detailed finder charts of select
 !     regions. At the very least, the thirteen files for ecliptic-based
 !     constellation (~the Zodiac) would be quite helpful. The three character
 !     constellation field in the "label" field of the newer (but still
 !     reduced) version of yale.star would then go.
 ! [5] I have already produced tables of Keplerian elements for asteroids
 !     to the 10th mag. I also have code to read the data and solve for various
 !     related values (e.g. eccentric anomoly from mean anomoly). Left are
 !     the routines to find earth's position, plus the spherical trig needed
 !     to convert this into Ra and Decl. This would best be integrated into
 !     "planet.c", which already does the latter. Interested parties should
 !     e-mail me for the files "orbit.c" and "asteroids.el".
 ! [6] xcen and ycen would be ignored, and scale would give the limit of
 !     declination (90.0-scale). Provisions to choose which pole needed, too.
 !     I believe stereographic projection would be the best, being both
 !     conformal and preserving of circles (as circles of new radii).
 ! [7] A complete rework would probably also place moon and asteroid calcs
 !     here, in addition to generating "track" output records.
 !
 ! [1'] Electronic databases to the 10th mag rumored to exist, containing on
 !	the order of 40000 stars. Don't ask me where to find them.
 ! [2'] No idea where to find
 ! [3'] Should not be too hard
 */
