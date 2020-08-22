/*
 * sif_to_text -- Produce plain text discriptions
 *                from `sif' format starchart data.
 *
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 * DISCLAIMER OF WARRANTY
 * ----------------------
 * The author  disclaims all warranties  with regard to  this software to
 * the   extent  permitted  by applicable   law,  including all   implied
 * warranties  of merchantability  and  fitness. In  no event shall   the
 * author be liable for any special, indirect or consequential damages or
 * any  damages whatsoever resulting from  loss of use, data or  profits,
 * whether in an action of contract, negligence or other tortious action,
 * arising  out of  or in connection with the  use or performance of this
 * software.
 *
 */

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/dataconv/sif_to_text.c,v 1.1 1990-03-30 16:37:16 vrs Exp $";



#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define SEPCHAR ';'

#define LINELEN 82
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define FALSE 0
#define TRUE 1


/* variables for data, filled by readstar and readsif */
double obj_lat, obj_lon, obj_mag;
char obj_type[] ="SS", obj_color[3], obj_label[3];
char obj_constell[4], obj_name[LINELEN];
char *obj_commnt;

/* Special for readsif in this program */
int line_no=0;
   
char *usage =
"%s: [-i infile] [-sc]\n";

int to_consindx();

main(argc, argv)
     int argc;
     char *argv[];
{
  char *inname = "";
  FILE *infile = stdin;
  char sepchar = SEPCHAR;
  int i;

  i = 0;
  while (i < argc)
    if (argv[i][0] != '-') i++;
  else switch (argv[i][1]) {
  case 'i':
    inname = argv[i+1];
    i += 2;
    break;
  case 's':
    if (argv[i][2]) sepchar = argv[i][2];
    i++;
    break;
  default:
    fprintf(stderr, usage, argv[0]);
    exit(4);
  }

  if (inname[0])
    if ((infile = fopen(inname, "r")) == NULL) {
      fprintf(stderr, "%s: Can't open input file %s\n", argv[0], inname);
      exit(6);
    }

  for (;;) {
    if (readsif(infile, sepchar)) break;
    check_data();
    write_text();
  }

  exit(0);
}


/* readsif reads standard starchart interchange format files,
   extracting the same data as readstar, if possible, and loading
   the same variables */
readsif(file, sepchar)
FILE *file;
char sepchar;
{
  static char inp_line[10*LINELEN];
  int i;
  char *cp;
  char *parsed_line[9];
  int num_parsed;
  double ra_h, ra_m, ra_s, de_d, de_m, de_s;

  /* Get line */
  if (fgets(inp_line, 10*LINELEN, file) == NULL) return TRUE;
  line_no++;

  /* Remove newline */
  inp_line[strlen(inp_line)-1] = '\0';

  /* split line into tokens */
  for (i = 0; i < 9; i++) parsed_line[i] = "";

  i = 0;
  cp = inp_line;
  parsed_line[i++] = cp;
  while (*cp)
    if (*cp != sepchar) cp++;
    else if (i < 9) {
      *cp++ = '\0';
      parsed_line[i++] = cp;
    };
  num_parsed = i;

  /* parse ra and dec */
  ra_h = ra_m = ra_s = 0.0;
  de_d = de_m = de_s = 0.0;
  sscanf(parsed_line[0], "%lf %lf %lf", &ra_h, &ra_m, &ra_s);
  sscanf(parsed_line[1], "%lf %lf %lf", &de_d, &de_m, &de_s);

  /* set obj_ values */
  obj_lon = ra_h * 15.0 + ra_m / 4.0 + ra_s / (4.0 * 60.0);
  obj_lat = fabs(de_d) + de_m / 60.0 + de_s / 3600.0;

  /* In order to assign the sign properly if de_d == 0,
     we must see if there is a negative sign before the first digit */
  if (de_d < 0.0) obj_lat = -obj_lat;
  else if (de_d == 0.0) {
    i = 0;
    while ((parsed_line[1][i] != '-') && (!isdigit(parsed_line[1][i]))) i++;
    if (parsed_line[1][i] == '-') obj_lat = -obj_lat;
  };

  sscanf(parsed_line[2], "%lf", &obj_mag);

  if (sscanf(parsed_line[3], "%2s", obj_type) == EOF) strcpy(obj_type, "SS");
  if (sscanf(parsed_line[4], "%2s", obj_color) == EOF) strcpy(obj_color, "  ");
  if (sscanf(parsed_line[5], "%2s", obj_label) == EOF) strcpy(obj_label, "  ");
  if (sscanf(parsed_line[6], "%3s", obj_constell) == EOF)
    strcpy(obj_constell, "   ");

  if (!obj_type[1]) obj_type[1] = ' ';
  if (!obj_color[1]) obj_color[1] = ' ';
  if (!obj_label[1]) obj_label[1] = ' ';
  if (!obj_constell[1]) obj_constell[1] = ' ';
  if (!obj_constell[2]) obj_constell[2] = ' ';

  obj_type[2] = '\0';
  obj_color[2] = '\0';
  obj_label[2] = '\0';
  obj_constell[3] = '\0';

/* Magic for label:
   type and color should be left justified, constellation is 3 chars if valid,
   but label could be " X" or "X " with equal validity.
   If the label field is exactly two characters long including whitespace,
   and both characters are printable, use it verbatum. */
  if ((strlen(parsed_line[5]) == 2)  && isprint(parsed_line[5][0]) &&
       isprint(parsed_line[5][1])) strcpy(obj_label, parsed_line[5]);

  /* Trim whitespace before and after name */
  while ((*parsed_line[7] == ' ') || (*parsed_line[7] == '\t'))
    parsed_line[7]++;
  i = strlen(parsed_line[7]) -1 ;
  while ((parsed_line[7][i]  == ' ') || (parsed_line[7][i] == '\t'))
    parsed_line[7][i] = '\0';
  if (!parsed_line[7][0]) strcpy(obj_name,"");
  else strcpy(obj_name,parsed_line[7]);

  obj_commnt = parsed_line[8];

  if (to_consindx(obj_constell) == -1) strcpy(obj_constell,"   ");

  /* Commas should not appear in name field */
  i = 0;
  while (obj_name[i])
    if (obj_name[i++] == ',')
      fprintf(stderr, "Warning: comma in name field:\"%s\"\n", obj_name);

  return FALSE;
}


/* write text discription of object */
write_text()
{
  int ra_h, ra_m, ra_s;
  int de_d, de_m, de_s;
  char dsign;

  char *starid(), *objid();

  ra_h = obj_lon/15.0;
  ra_m = ((obj_lon/15.0) - ra_h) * 60 + (0.5 / 60);
  ra_s = ((((obj_lon/15.0) - ra_h) * 60) - ra_m) * 60 + 0.5;

  if (ra_s >= 60) {ra_s -= 60; ra_m++;};
  if (ra_m >= 60) {ra_m -= 60; ra_h++;};


  if (obj_lat < 0.0) {
    obj_lat = -obj_lat;
    dsign = '-';
  } else dsign = '+';

  de_d = obj_lat;
  de_m = (obj_lat - de_d) * 60 + (0.5 / 60);
  de_s = (((obj_lat - de_d) * 60) - de_m) * 60 + 0.5;

  if (de_s >= 60) {de_s -= 60; de_m++;};
  if (de_m >= 60) {de_m -= 60; de_d++;};


  if ((obj_type[0] == 'S') || (obj_type[0] == 'I'))
    printf("%70s ", starid());
		/* starid supplies label, constellation, subtype, and name */
  else
    printf("%70s ", objid());
		/* objid supplies constellation, type, size, and name */

  printf("%2dh%2dm%2ds %c%02dd%2dm%2ds %5.2f %s",
	 ra_h, ra_m, ra_s, dsign, de_d, de_m, de_s, obj_mag, obj_color);


  if (obj_commnt[0])
    printf(" %s", obj_commnt);

  printf("\n");
}

struct {
  char *abbrev;
  char *name;
  char *genetive;
} constellations[] = {
  {"AND",	"Andromeda",	"Andromedae"},
  {"ANT",	"Antlia",	"Antliae"},
  {"APS",	"Apus",		"Apodis"},
  {"AQL",	"Aquila",	"Aquilae"},
  {"AQR",	"Aquarius",	"Aquarii"},
  {"ARA",	"Ara",		"Arae"},
  {"ARI",	"Aries",	"Arietis"},
  {"AUR",	"Auriga",	"Aurigae"},
  {"BOO",	"Bootes",	"Bootis"},
  {"CAE",	"Caelum",	"Caeli"},
  {"CAM",	"Camelopardalis",	"Camelopardalis"},
  {"CAP",	"Capricornus",	"Capricorni"},
  {"CAR",	"Carina",	"Carinae"},
  {"CAS",	"Cassiopeia",	"Cassiopeiae"},
  {"CEN",	"Centaurus",	"Centauri"},
  {"CEP",	"Cepheus",	"Cephei"},
  {"CET",	"Cetus",	"Ceti"},
  {"CHA",	"Chamaeleon",	"Chamaeleonis"},
  {"CIR",	"Circinus",	"Circini"},
  {"CMA",	"Canis Major",	"Canis Majoris"},
  {"CMI",	"Canis Minor",	"Canis Minoris"},
  {"CNC",	"Cancer",	"Cancri"},
  {"COL",	"Columba",	"Columbae"},
  {"COM",	"Coma Berenices",	"Comae Berenices"},
  {"CRA",	"Corona Australis",	"Coronae Australis"},
  {"CRB",	"Corona Borealis",	"Corona Borealis"},
  {"CRT",	"Crater",	"Crateris"},
  {"CRU",	"Crux",	"Cruxis"},
  {"CRV",	"Corvus",	"Corvi"},
  {"CVN",	"Canes Venatici",	"Canum Venaticorum"},
  {"CYG",	"Cygnus",	"Cygni"},
  {"DEL",	"Delphinus",	"Delphini"},
  {"DOR",	"Dorado",	"Doradus"},
  {"DRA",	"Draco",	"Draconis"},
  {"EQU",	"Equuleus",	"Equulei"},
  {"ERI",	"Eridanus",	"Eridani"},
  {"FOR",	"Fornax",	"Fornacis"},
  {"GEM",	"Gemini",	"Geminorum"},
  {"GRU",	"Grus",	"Gruis"},
  {"HER",	"Hercules",	"Herculis"},
  {"HOR",	"Horologium",	"Horologii"},
  {"HYA",	"Hydra",	"Hydrae"},
  {"HYI",	"Hydrus",	"Hydri"},
  {"IND",	"Indus",	"Indi"},
  {"LAC",	"Lacerta",	"Lacertae"},
  {"LEO",	"Leo",	"Leonis"},
  {"LEP",	"Lepus",	"Leporis"},
  {"LIB",	"Libra",	"Librae"},
  {"LMI",	"Leo Minor",	"Leonis Minoris"},
  {"LUP",	"Lupus",	"Lupi"},
  {"LYN",	"Lynx",		"Lyncis"},
  {"LYR",	"Lyra",	"Lyrae"},
  {"MEN",	"Mensa",	"Mensae"},
  {"MIC",	"Microscopium",	"Microscopii"},
  {"MON",	"Monoceros",	"Monocerotis"},
  {"MUS",	"Musca",	"Muscae"},
  {"NOR",	"Norma",	"Normae"},
  {"OCT",	"Octans",	"Octantis"},
  {"OPH",	"Ophiuchus",	"Ophiuchi"},
  {"ORI",	"Orion",	"Orionis"},
  {"PAV",	"Pavo",	"Pavonis"},
  {"PEG",	"Pegasus",	"Pegasi"},
  {"PER",	"Perseus",	"Persei"},
  {"PHE",	"Phoenix",	"Phoenicis"},
  {"PIC",	"Pictor",	"Pictoris"},
  {"PSA",	"Piscis Astrinus",	"Piscis Austrini"},
  {"PSC",	"Pisces",	"Piscium"},
  {"PUP",	"Puppis",	"Puppis"},
  {"PYX",	"Pyxis",	"Pyxidis"},
  {"RET",	"Reticulum",	"Reticuli"},
  {"SCL",	"Sculptor",	"Sculptoris"},
  {"SCO",	"Scorpius",	"Scorpii"},
  {"SCT",	"Scutum",	"Scuti"},
  {"SER",	"Serpens",	"Serpentis"},
  {"SEX",	"Sextans",	"Sextantis"},
  {"SGE",	"Sagitta",	"Sagittae"},
  {"SGR",	"Sagittarius",	"Sagittarii"},
  {"TAU",	"Taurus",	"Tauri"},
  {"TEL",	"Telescopium",	"Telescopii"},
  {"TRA",	"Triangulum Astrale",	"Trianguli Astralis"},
  {"TRI",	"Triangulum",	"Trianguli"},
  {"TUC",	"Tucana",	"Tucanae"},
  {"UMA",	"Ursa Major",	"Ursae Majoris"},
  {"UMI",	"Ursa Minor",	"Ursae Minoris"},
  {"VEL",	"Vela",	"Velorum"},
  {"VIR",	"Virgo",	"Virginis"},
  {"VOL",	"Volans",	"Volantis"},
  {"VUL",	"Vulpecula",	"Vulpeculae"},
  {"   ",	"",	""},
  {"",	"",	""}
};

int to_consindx(cons)
char *cons;
{
  int i;

  if (!cons[0]) return -1;

  i = -1;
  while (constellations[++i].abbrev[0])
    if (!strcmp(cons, constellations[i].abbrev)) break;

  return (constellations[i].abbrev[0] ? i : 0);
}


char ret_star[100];

/* return pointer to ret_star, which contains the label and constellation,
   name, and subtype */
char *starid()
{
  char *grk_str;
  char *to_greek();

  grk_str = to_greek(obj_label);

  if (grk_str[0])
    if ((obj_type[1] != 'S') && (obj_type[1] != ' '))
      sprintf(ret_star, "%8s %-18s (%c) %-35.35s",
	      grk_str,
	      constellations[to_consindx(obj_constell)].genetive,
	      obj_type[1],
	      obj_name);
    else
      sprintf(ret_star, "%8s %-18s     %-35.-35s", grk_str,
	      constellations[to_consindx(obj_constell)].genetive,
	      obj_name);
  else
    if ((obj_type[1] != 'S') && (obj_type[1] != ' '))
      sprintf(ret_star, "         %-18s (%c) %35.35s", 
	      constellations[to_consindx(obj_constell)].name,
	      obj_type[1],
	      obj_name);
    else
      sprintf(ret_star, "         %-18s     %35.35s",
	      constellations[to_consindx(obj_constell)].name,
	      obj_name);
  if (obj_type[0] == 'I') strcat(ret_star, "I ");
  else strcat(ret_star, "  ");

  return ret_star;
}

struct {
  char roman;
  char *greek_name;
} grk_tr_tab[] = {
  {'a',	"Alpha"},
  {'b', "Beta"},
  {'g', "Gamma"},
  {'d', "Delta"},
  {'e', "Epsilon"},
  {'z', "Zeta"},
  {'h', "Eta"},
  {'q', "Theta"},
  {'i', "Iota"},
  {'k', "Kappa"},
  {'l', "Lambda"},
  {'m', "Mu"},
  {'n', "Nu"},
  {'x', "Xi"},
  {'o', "Omicron"},
  {'p', "Pi"},
  {'r', "Rho"},
  {'s', "Sigma"},
  {'t', "Tau"},
  {'u', "Upsilon"},
  {'f', "Phi"},
  {'j', "Phi"},
  {'c', "Chi"},
  {'y', "Psi"},
  {'w', "Omega"},
  {' ', ""}
};

char grk_ret[12];

char *to_greek(label)
char *label;
{
  int i;

  if (isgreek(label[0]) && (isdigit(label[1]) || (label[1] == ' '))) {
    /* Greek if first character is greek encoded,
       and the second is space or a digit */
    i = 0;
    while ((grk_tr_tab[i].greek_name[0])
	   && (grk_tr_tab[i].roman != label[0])) i++;
    if (grk_tr_tab[i].greek_name[0])
      sprintf(grk_ret, "%s%c", grk_tr_tab[i].greek_name, label[1]);
    else { /* Error */
      fprintf(stderr, "Report bug in greek coding\n");
      exit(1);
    }
  } else {			/* Not greek */
    if ((label[0] != ' ') || (label[1] != ' '))
      sprintf(grk_ret, "%s", label);
    else
      grk_ret[0] = '\0';
  }

  return grk_ret;
}



isgreek(c)
char c;
{
  char *cp;

  cp = "abgdezhqiklmnxoprstufcywj";

  while (*cp && (*cp != c)) cp++;
  return (*cp != '\0'); /* True if letter was in greek string */
}






char ret_obj[100];

/* return pointer to ret_obj, which contains the constellation,
   type, size and name */
char *objid()
{
  long int size_obj();
  long int sze;
  char *obj_ty();
  char sze_str[10];

  sze = size_obj(obj_label);
  if (sze > 4800)
    sprintf(sze_str, "%.2fd", sze/3600.0);
  else if (sze > 120)
    sprintf(sze_str, "%.2fm", sze/60.0);
  else
    sprintf(sze_str, "%ds", sze);

  if (sze != -1)
    sprintf(ret_obj, "%-18s %-19.19s %-24s %6s",
	    constellations[to_consindx(obj_constell)].name,
	    obj_name,
	    obj_ty(obj_type),
	    sze_str);
  else
    sprintf(ret_obj, "%-18s %-19.19s %-24s       ",
	    constellations[to_consindx(obj_constell)].name,
	    obj_name,
	    obj_ty(obj_type));

  return ret_obj;
}



/* Translate two digit encoded size string */
/* Maximum parsed: 89000 secs of arc = 24.666 degrees */
/* Warning, should use 32 bit integers to allow this value */
long size_obj(size_str)
     char *size_str;
{
	char chr1, chr2;

	chr1 = islower(size_str[0]) ? toupper(size_str[0]) : size_str[0];
	chr2 = islower(size_str[1]) ? toupper(size_str[1]) : size_str[1];

	if ((chr1 == ' ') && (chr2 == ' ')) return -1;
	if (chr1 == ' ') return chr2 - '0';
	if (isdigit(chr1)) return ((chr1-'0')*10L + (chr2-'0'));
	if (chr1 < 'J') return ((chr1-'A'+1)*100L + (chr2-'0')*10L);
	if (chr1 < 'S') return ((chr1-'I')*1000L + (chr2-'0')*100L);
	if (chr1 <= 'Z') return ((chr1-'R')*10000L + (chr2-'0')*1000L);
	return -1;
}

/*
Examples:

"  "		-1
" 6"		6
"09"		9
"73"		73
"A0"		100
"C3"		330
"D5"		450
"I6"		960
"J2"		1200
"R3"		9300
"S6"		16000
"Z0"		80000
"Z9"		89000
*/



struct subtytab {
  char ch;
  char *str;
};

struct tytab {
  char ch;
  char *str;
  struct subtytab *subtab;
};

struct subtytab st_subty[] = {
  {'S',	"Single"},
  {'D',	"Double"},
  {'V',	"Variable"},
  {'?',	""}
};

struct subtytab pl_subty[] = {
  {'M',	"Mercury"},
  {'V',	"Venus"},
  {'m',	"Mars"},
  {'J',	"Jupiter"},
  {'s',	"Saturn"},
  {'U',	"Uranus"},
  {'N',	"Neptune"},
  {'P',	"Pluto"},
  {'A',	"Asteroid"},
  {'C',	"Comet"},
  {'S',	"Sun"},
  {'L',	"Moon"},
  {'?',	""}
};

struct subtytab cl_subty[] = {
  {'O',	"Open Cluster"},
  {'G',	"Globular"},
  {'?',	""}
};

struct subtytab nb_subty[] = {
  {'D',	"Diffuse Nebula"},
  {'P',	"Planetary Nebula"},
  {'?',	""}
};

struct subtytab gx_subty[] = {
  {'a',	"Spiral Galaxy Sa"},
  {'b',	"Spiral Galaxy Sb"},
  {'c',	"Spiral Galaxy Sc"},
  {'d',	"Spiral Galaxy Sd"},
  {'p',	"Spiral Galaxy Sp"},
  {'Z',	"Spiral Galaxy S0"},
  {'s',	"Spiral Galaxy"},
  {'A',	"Barred Spiral Galaxy SBa"},
  {'B',	"Barred Spiral Galaxy SBb"},
  {'C',	"Barred Spiral Galaxy SBc"},
  {'D',	"Barred Spiral Galaxy SBd"},
  {'P',	"Barred Spiral Galaxy SBp"},
  {'S',	"Barred Spiral Galaxy"},
  {'0',	"Elliptical Galaxy E0"},
  {'1',	"Elliptical Galaxy E1"},
  {'2',	"Elliptical Galaxy E2"},
  {'3',	"Elliptical Galaxy E3"},
  {'4',	"Elliptical Galaxy E4"},
  {'5',	"Elliptical Galaxy E5"},
  {'6',	"Elliptical Galaxy E6"},
  {'7',	"Elliptical Galaxy E7"},
  {'E',	"Elliptical Galaxy"},
  {'I',	"Irregular Galaxy"},
  {'Q',	"Quasar"},
  {'U',	"Unknown Galaxy"},
  {'!',	"Pecular Galaxy"},
  {'?',	""}
};

struct subtytab ve_subty[] = {
  {'M',	"Vector Move to"},
  {'S',	"Solid Line"},
  {'D',	"Dotted Line"},
  {'H',	"Dashed Line"},
  {'?',	""}
};

struct subtytab ar_subty[] = {
  {'M',	"Area move to"},
  {'A',	"Area border"},
  {'F',	"Area fill"},
  {'?',	""}
};


struct subtytab nul_subty[] = {
  {'?',	""}
};



struct tytab tytop[] = {
  {'S',	"? Star", st_subty},
  {'P',	"? Planet", pl_subty},
  {'C',	"? Cluster", cl_subty},
  {'N',	"? Nebula", nb_subty},
  {'G',	"? Galaxy", gx_subty},
  {'V',	"ERROR Vector", ve_subty},
  {'O',	"Other", nul_subty},
  {'U',	"Unknown", nul_subty},
  {'I',	"Invisible", nul_subty},
  {'#',	"Comment", nul_subty},
  {'A',	"ERROR Area", ar_subty},
  {'U',	"Unknown", nul_subty},
  {'?',	"", nul_subty}
};



char ty_ret[100];
char *obj_ty(typ)
char *typ;
{
  int i, j;


  i = 0;
  while ((tytop[i].str[0]) && (tytop[i].ch != typ[0])) i++;
  if (tytop[i].str[0]) {
    j = 0;
    while ((tytop[i].subtab[j].str[0])
	    && (tytop[i].subtab[j].ch != typ[1])) j++;
    if (tytop[i].subtab[j].str[0])
      sprintf(ty_ret, "%s", tytop[i].subtab[j].str);
    else
      sprintf(ty_ret, "%s", tytop[i].str);
  } else
      sprintf(ty_ret, "%s", "Unknown Type");

  return ty_ret;
}


check_data()
{
  int i, j;

  if ((obj_lon < 0.0) || (obj_lon > 360.0))
    fprintf(stderr, "Warning: RA out of range: line %d: %f\n",
	    line_no, obj_lon);

  if ((obj_lat < -90.0) || (obj_lat > 90.0))
    fprintf(stderr, "Warning: declination out of range: line %d: %f\n",
	    line_no, obj_lat);

  if ((obj_mag < -30.0) || (obj_mag > 30.0))
    fprintf(stderr, "Warning: abnormal magnitude: line %d: %f\n",
	    line_no, obj_mag);

  i = 0;
  while ((tytop[i].str[0]) && (tytop[i].ch != obj_type[0])) i++;
  if (tytop[i].str[0]) {
    j = 0;
    while ((tytop[i].subtab[j].str[0])
	    && (tytop[i].subtab[j].ch != obj_type[1])) j++;
    if ((!tytop[i].subtab[j].str[0])
	&& (obj_type[1] != ' '))
      fprintf(stderr,
	      "Warning: unknown subtype: line %d: type %c subtype %c\n",
	    line_no, obj_type[0], obj_type[1]);
  } else
    fprintf(stderr, "Warning: unknown type: line %d: %c\n",
	    line_no, obj_type[0]);

  if (obj_type[0] == 'S') {
    if ((!isgreek(obj_label[0]))       /* letter is not greek, */
	&& islower(obj_label[0])       /* but is lowercase */
	&& (isdigit(obj_label[1]) || (obj_label[1] == ' ')))
				       /* and is followed by space or digit */
      fprintf(stderr, "Warning: not greek encoding: line %d: %s\n",
	      line_no, obj_label);
  }

  if (to_consindx(obj_constell) == -1)
    fprintf(stderr, "Warning: unknown constellation: line %d: %s\n",
	    line_no, obj_constell);
    

}
