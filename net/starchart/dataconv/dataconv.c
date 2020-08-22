/*
 * dataconv.c/precess -- convert between starchart data formats,
 *                precess to other equator and equinox.
 *	          precession based formulae from
 *                Astronomical Almanac, 1988, p B19
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


static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/dataconv/dataconv.c,v 1.1 1990-03-30 16:37:19 vrs Exp $";



#include <stdio.h>
#include <math.h>
#include <ctype.h>

/* Defines for equinox */
#define EQLOW 1850.0
#define EQHI  2100.0
#define DEFEIN 1950.0
#define DEFEOUT 2000.0


/* other defines */
#define LINELEN 82
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define FALSE 0
#define TRUE 1
#define DEG_TO_RAD 0.01745329251994329600
#define RAD_TO_DEG 57.29577951308232
#define DSIN(x) (sin((x)*DEG_TO_RAD))
#define DCOS(x) (cos((x)*DEG_TO_RAD))
#define DTAN(x) (tan((x)*DEG_TO_RAD))
#define DASIN(x) (asin(x)*RAD_TO_DEG)
#define DACOS(x) (acos(x)*RAD_TO_DEG)
#define DATAN(x) (atan(x)*RAD_TO_DEG)
#define DATAN2(x,y) (atan2(x,y)*RAD_TO_DEG)

/* File types */
#define LINEREAD 1
#define INDEXTYPE 2
#define BINFULL 3
#define BINOBJ 4
#define BINSTAR 5
#define SAOTYPE 6
#define SIFTYPE 7
#define GSCTYPE 8

#define SEPCHAR ';'

/* variables for data, filled by readstar and readsif */
double obj_lat, obj_lon, obj_mag;
char obj_type[] ="SS", obj_color[3], obj_label[3];
char obj_constell[4], obj_name[LINELEN];
char *obj_commnt, fileline[LINELEN];

char *usage =
"%s:\nargs: -i inname intype -o outname outtype\n\t[-sc] [-f from_equinox -t to_equinox]\n";


int tr_type();
int readstar();
int readsif();
void writelr();
void wrbinfull();
void wrbinstar();
void wrbinobj();
void writesif();
int to_consindx();
void initxform(), xform();

double into_range();
void precess_f();

main(argc, argv)
     int argc;
     char *argv[];
{
  char *inname = "", *outname = "";
  char *intypestr = "", *outtypestr = "";
  FILE *infile, *outfile;
  int intype, outtype;
  char sepchar = SEPCHAR;
  int i;
  double ein=0.0, eout=0.0, ra_out, de_out;
  int precess = FALSE;

  if ((argc <  7)) {
    fprintf(stderr, usage, argv[0]);
    exit(1);
  }

  i = 0;
  while (i < argc)
    if (argv[i][0] != '-') i++;
  else switch (argv[i][1]) {
  case 'f':
    ein=atof(argv[i+1]);
    precess = TRUE;
    i += 2;
    break;
  case 't':
    eout=atof(argv[i+1]);
    precess = TRUE;
    i += 2;
    break;
  case 'i':
    inname = argv[i+1];
    intypestr = argv[i+2];
    if (!(intype = tr_type(argv[i+2]))) {
      fprintf(stderr, usage, argv[0]);
      fprintf(stderr, "Invalid intype %s\n", argv[i+2]);
      exit(2);
    }
    i += 3;
    break;
  case 'o':
    outname = argv[i+1];
    outtypestr = argv[i+2];
    if (!(outtype = tr_type(argv[i+2]))) {
      fprintf(stderr, usage, argv[0]);
      fprintf(stderr, "Invalid outtype %s\n", argv[i+2]);
      exit(3);
    }
    if (outtype == GSCTYPE) {
      fprintf(stderr, "%s: cannot write gsc format files.\n", argv[0]);
      exit(3);
    }
    i += 3;
    break;
  case 's':
    if (argv[i][2]) sepchar = argv[i][2];
    i++;
    break;
  default:
    fprintf(stderr, usage, argv[0]);
    exit(4);
  }

  if (precess && 
      ((ein < EQLOW) || (ein > EQHI) || (eout  < EQLOW) || (eout > EQHI))) {
    fprintf(stderr, usage, argv[0]);
    if ((ein > 0.0) || (eout > 0.0))
      fprintf(stderr, "equinox not in range [%.1f..%.1f]\n", EQLOW, EQHI);
    exit(5);
  }


#ifdef ATARI_ST
  if ((infile = fopen(inname, (intype == LINEREAD)?"r":"rb")) == NULL) {
    fprintf(stderr, "%s: Can't open input file %s\n", argv[0], inname);
    exit(6);
  }

  if ((outfile = fopen(outname, "wb")) == NULL) {
    fprintf(stderr, "%s: Can't open output file %s\n", argv[0], outname);
    exit(7);
  }
#else
  if ((infile = fopen(inname, "r")) == NULL) {
    fprintf(stderr, "%s: Can't open input file %s\n", argv[0], inname);
    exit(6);
  }

  if ((outfile = fopen(outname, "w")) == NULL) {
    fprintf(stderr, "%s: Can't open output file %s\n", argv[0], outname);
    exit(7);
  }
#endif


  fprintf(stderr,
	  "%s:\n converting %s format file %s\n to %s format file %s\n",
	  argv[0], intypestr, inname, outtypestr, outname);
  if ((intype == SIFTYPE) || (outtype == SIFTYPE))
    fprintf(stderr, "Separation character %c\n", sepchar);

  if (precess)
    fprintf(stderr, "Precessing from equinox %.3f to %.3f\n",
	    ein, eout);


  /* SIF must be read with readsif
     All others should be read with readstar
     */
  if (precess) {
    if (intype == SIFTYPE)
      switch (outtype) {
      case LINEREAD:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  writelr(outfile);
	}
	break;
      case BINFULL:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinfull(outfile);
	}
	break;
      case BINOBJ:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinobj(outfile);
	}
	break;
      case BINSTAR:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinstar(outfile);
	}
	break;
      }
    else /* Not SIF in */ 
      switch (outtype) {
      case LINEREAD:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  writelr(outfile);
	}
	break;
      case BINFULL:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinfull(outfile);
	}
	break;
      case BINOBJ:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinobj(outfile);
	}
	break;
      case BINSTAR:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  wrbinstar(outfile);
	}
	break;
      case SIFTYPE:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  precess_f(ein, eout, obj_lon, obj_lat, &ra_out, &de_out);
	  obj_lon = ra_out;
	  obj_lat = de_out;
	  writesif(outfile, sepchar);
	}
	break;
      }
  } else { /* not precess */
    if (intype == SIFTYPE)
      switch (outtype) {
      case LINEREAD:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  writelr(outfile);
	}
	break;
      case BINFULL:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  wrbinfull(outfile);
	}
	break;
      case BINOBJ:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  wrbinobj(outfile);
	}
	break;
      case BINSTAR:
	for (;;) {
	  if (readsif(infile, sepchar)) break;
	  wrbinstar(outfile);
	}
	break;
      }
    else /* Not SIF in */ 
      switch (outtype) {
      case LINEREAD:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  writelr(outfile);
	}
	break;
      case BINFULL:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  wrbinfull(outfile);
	}
	break;
      case BINOBJ:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  wrbinobj(outfile);
	}
	break;
      case BINSTAR:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  wrbinstar(outfile);
	}
	break;
      case SIFTYPE:
	for (;;) {
	  if (readstar(infile, intype)) break;
	  writesif(outfile, sepchar);
	}
	break;
      }
  }

  exit(0);
}





int tr_type(s)
     char *s;
{
  int i;

  for (i = 0; s[i]; i++)
    if (isupper(s[i]))
      s[i] = tolower(s[i]);

  if(!strcmp(s, "lineread")) return LINEREAD;
/*  else if (!strcmp(s, "indextype")) return INDEXTYPE;*/
  else if (!strcmp(s, "binfull")) return BINFULL;
  else if (!strcmp(s, "binobj")) return BINOBJ;
  else if (!strcmp(s, "binstar")) return BINSTAR;
  else if (!strcmp(s, "sif")) return SIFTYPE;
  else if (!strcmp(s, "gsc")) return GSCTYPE;
/*  else if (!strcmp(s, "saoformat")) return SAOFORMAT;*/
  else return 0;
}


/* constellation abbreviations */
char *con_table[] = {
  "   ",
  "AND",
  "ANT",
  "APS",
  "AQL",
  "AQR",
  "ARA",
  "ARI",
  "AUR",
  "BOO",
  "CAE",
  "CAM",
  "CAP",
  "CAR",
  "CAS",
  "CEN",
  "CEP",
  "CET",
  "CHA",
  "CIR",
  "CMA",
  "CMI",
  "CNC",
  "COL",
  "COM",
  "CRA",
  "CRB",
  "CRT",
  "CRU",
  "CRV",
  "CVN",
  "CYG",
  "DEL",
  "DOR",
  "DRA",
  "EQU",
  "ERI",
  "FOR",
  "GEM",
  "GRU",
  "HER",
  "HOR",
  "HYA",
  "HYI",
  "IND",
  "LAC",
  "LEO",
  "LEP",
  "LIB",
  "LMI",
  "LUP",
  "LYN",
  "LYR",
  "MEN",
  "MIC",
  "MON",
  "MUS",
  "NOR",
  "OCT",
  "OPH",
  "ORI",
  "PAV",
  "PEG",
  "PER",
  "PHE",
  "PIC",
  "PSA",
  "PSC",
  "PUP",
  "PYX",
  "RET",
  "SCL",
  "SCO",
  "SCT",
  "SER",
  "SEX",
  "SGE",
  "SGR",
  "TAU",
  "TEL",
  "TRA",
  "TRI",
  "TUC",
  "UMA",
  "UMI",
  "VEL",
  "VIR",
  "VOL",
  "VUL",
  ""
  };


/* typedefs for exact sizes of int */
typedef char int_8;
typedef short int int_16;
typedef long int int_32;

/* BINFULL structure */
struct bfull_struct {
  int_32 lat;			/* RA in seconds * 1000 */
  int_32 lon;			/* Dec in seconds * 1000 */
  int_16 mag;			/* Mag * 1000 */
  char tycolb[6];		/* Type, color, label fields */
  int_8 consindx;		/* Index number of constellation */
  int_16 strlen;		/* length of name and comment field */
} binfull_in, binfull_out;

char name_comment[LINELEN];

/* BINOBJ structure */
struct bobj_struct {
  int_32 lat;			/* RA in seconds * 1000 */
  int_32 lon;			/* Dec in seconds * 1000 */
  int_16 mag;			/* Mag * 1000 */
  char type[2];			/* e.g. 'SD', 'CO' */
} binobj_in, binobj_out;

/* BINSTAR structure */
struct bstar_struct {
  int_32 lat;			/* RA in seconds * 1000 */
  int_32 lon;			/* Dec in seconds * 1000 */
  int_16 mag;			/* Mag * 1000 */
} binstar_in, binstar_out;


/* readstar reads from the file the information for one object, and
   loads the following variables:
double obj_lat, obj_lon, obj_mag;
char obj_type[] ="SS", obj_color[3], obj_label[3];
char obj_constell[4], obj_name[LINELEN];
char *obj_commnt, fileline[LINELEN];

only lat, lon, and mag are required.  type should default to 'SS',
color, label, constell default to "  ", and the rest default to ""
*/
int readstar(file, ftype)
     FILE *file;
     int ftype;
{
  char *ptr;
  double rah, ram, ras, dld, dlm, dl, inten;
  int i, j;
  int nchars;
  char m1;
  static int GSC_seeked = FALSE;
  static int GSC_skip = FALSE;
  struct {
    double ra_deg, dec_deg, mag;
    int mag_band, class;
  } GSC[10];
  int GSC_nlines;
  static int GSC_ID = 0;
  char id_str[5];

  if ((ftype != LINEREAD) && (ftype != BINFULL)
      && (ftype != BINOBJ) && (ftype != BINSTAR) && (ftype != GSCTYPE))
    return (TRUE);
  /* only LINEREAD, BINFULL, BINOBJ and BINSTAR and GSCTYPE
     supported at this time */

  if (ftype == BINSTAR) {
    if (fread((char *) &binstar_in, sizeof(binstar_in), 1, file) != 1) {
      if (feof(file)) return TRUE;
      perror("Error reading input file");
      exit(2);
    }

    obj_lat = ((double) binstar_in.lat) / 3600000L;
    obj_lon = ((double) binstar_in.lon) / 3600000L;
    obj_mag = ((double) binstar_in.mag) / 1000L;
    obj_type[0] = 'S';
    obj_type[1] = 'S';
    obj_color[0] = ' ';
    obj_color[1] = ' ';
    obj_label[0] = ' ';
    obj_label[1] = ' ';
    obj_constell[0] = ' ';
    obj_constell[1] = ' ';
    obj_constell[2] = ' ';
    obj_name[0] = '\0';
    obj_commnt = "";

    strcpy(fileline, "");
  } else if (ftype == BINOBJ) {
    if (fread((char *) &binobj_in, sizeof(binobj_in), 1, file) != 1) {
      if (feof(file)) return TRUE;
      perror("Error reading input file");
      exit(2);
    }

    obj_lat = ((double) binobj_in.lat) / 3600000L;
    obj_lon = ((double) binobj_in.lon) / 3600000L;
    obj_mag = ((double) binobj_in.mag) / 1000L;
    obj_type[0] = binobj_in.type[0];
    obj_type[1] = binobj_in.type[1];
    obj_color[0] = ' ';
    obj_color[1] = ' ';
    obj_label[0] = ' ';
    obj_label[1] = ' ';
    obj_constell[0] = ' ';
    obj_constell[1] = ' ';
    obj_constell[2] = ' ';
    obj_name[0] = '\0';
    obj_commnt = "";

    strcpy(fileline, "");
  } else if (ftype == BINFULL) {
    if (fread((char *) &binfull_in, sizeof(binfull_in), 1, file) != 1) {
      if (feof(file)) return TRUE;
      perror("Error reading input file");
      exit(2);
    }

    if (binfull_in.strlen == 0)
      strcpy(name_comment, "");
    else {
      if (fread((char *) name_comment, binfull_in.strlen, 1, file) != 1) {
	perror("Error reading input file");
	exit(2);
      }
      name_comment[binfull_in.strlen] = '\0';
    }

    obj_lat = ((double) binfull_in.lat) / 3600000L;
    obj_lon = ((double) binfull_in.lon) / 3600000L;
    obj_mag = ((double) binfull_in.mag) / 1000L;
    obj_type[0] = binfull_in.tycolb[0];
    obj_type[1] = binfull_in.tycolb[1];
    obj_color[0] = binfull_in.tycolb[2];
    obj_color[1] = binfull_in.tycolb[3];
    obj_label[0] = binfull_in.tycolb[4];
    obj_label[1] = binfull_in.tycolb[5];
    strcpy(obj_constell,con_table[binfull_in.consindx]);

    ptr = name_comment;
    i = 0;
    while (*ptr == ' ') ptr++;
    while (*ptr != ',' && *ptr != '\n' && *ptr)
      obj_name[i++] = *ptr++;
    obj_name[i] = '\0';
    if ((*ptr == ',') && (*++ptr) && name_comment[0]) obj_commnt = ptr;
    else obj_commnt = "";

    strcpy(fileline, "");
  } else if (ftype == GSCTYPE) {
#define Val(ch) (ch - '0')
    if (!GSC_seeked) {
      fseek(file, 8640L, 0);
      GSC_seeked = TRUE;
      if (fread((char *) id_str, 5, 1, file) != 1) {
	if (feof(file)) return TRUE;
	perror("Error reading input file");
	exit(2);
      };
      GSC_ID = Val(id_str[0])*10000 +
	Val(id_str[1])*1000 +
	Val(id_str[2])*100 +
	Val(id_str[3])*10 +
	Val(id_str[4]);
    };
    GSC_skip = FALSE;
    do {
      if (id_str[0] == ' ') return TRUE;
      i = 0;
      do {
	if (fread((char *) fileline, 40, 1, file) != 1) {
	  if (feof(file)) return TRUE;
	  perror("Error reading input file");
	  exit(2);
	};
	for (j = 0; j < 40; j++) if (fileline[j] == ' ') fileline[j] = '0';
	/* We care about RA_DEG, DEC_DEG, MAG, MAG_BAND, CLASS, MULTIPLE */
	/* We read the GSC_ID already to see if it is a continuation */
	GSC[i].ra_deg = Val(fileline[0]) * 100.0 +
	  Val(fileline[1]) * 10.0 +
	  Val(fileline[2]) +
	  Val(fileline[4]) / 10.0 +
	  Val(fileline[5]) / 100.0 +
	  Val(fileline[6]) / 1000.0 +
	  Val(fileline[7]) / 10000.0 +
	  Val(fileline[8]) / 100000.0;
	if (fileline[10] == '-')
	  GSC[i].dec_deg = -1 *
	    (Val(fileline[11]) +
	     Val(fileline[13]) / 10.0 +
	     Val(fileline[14]) / 100.0 +
	     Val(fileline[15]) / 1000.0 +
	     Val(fileline[16]) / 10000.0 +
	     Val(fileline[17]) / 100000.0);
	else
	  GSC[i].dec_deg =  ((fileline[9] == '-') ? -1 : 1) *
	    (Val(fileline[10]) * 10.0 +
	     Val(fileline[11]) +
	     Val(fileline[13]) / 10.0 +
	     Val(fileline[14]) / 100.0 +
	     Val(fileline[15]) / 1000.0 +
	     Val(fileline[16]) / 10000.0 +
	     Val(fileline[17]) / 100000.0);
	GSC[i].mag = Val(fileline[23]) * 10.0 +
	  Val(fileline[24]) +
	  Val(fileline[26]) / 10.0 +
	  Val(fileline[27]) / 100.0;
	GSC[i].mag_band = Val(fileline[32])*10 + Val(fileline[33]);
	GSC[i].class = Val(fileline[34]);
	i++;
	if (fread((char *) id_str, 5, 1, file) != 1) {
	  if (!feof(file)) {
	    perror("Error reading input file");
	    exit(2);
	  };
	};
	if (!feof(file)) {
	  j = Val(id_str[0])*10000 +
	    Val(id_str[1])*1000 +
	    Val(id_str[2])*100 +
	    Val(id_str[3])*10 +
	    Val(id_str[4]);
	};
      } while ((j == GSC_ID) && (!feof(file)) && (id_str[0] != ' '));
      GSC_nlines = i;
      GSC_ID = j;
      /* for now just use first */
/* There are many stars with class == 3, so we'll ignore class */
/*      if (GSC[0].class == 0) {*/	/* is a star if class == 0 */
	obj_lon = GSC[0].ra_deg;
	obj_lat = GSC[0].dec_deg;
	obj_mag = GSC[0].mag;
	obj_type[0] = 'S';
	obj_type[1] = 'S';
	obj_color[0] = ' ';
	obj_color[1] = ' ';
	obj_label[0] = ' ';
	obj_label[1] = ' ';
	obj_constell[0] = ' ';
	obj_constell[1] = ' ';
	obj_constell[2] = ' ';
	obj_name[0] = '\0';
/*	obj_commnt = &fileline[28];*/
	obj_commnt = "";
	fileline[0] = '\0';
	GSC_skip = FALSE;
/* Ignoring class seems to be the right thing */
/*      } else {*/			/* not a star, skip */
/*	GSC_skip = TRUE;
      };*/
    } while (GSC_skip);
  } else { /* LINEREAD */

/*
 * file formats:
 * new
064509-1643-14SDA1a CMASirius
051432-0812015SDB8b ORIRigel
 * old
064509-1643-146SSSirius
051432-08120015SSRigel
 */

    fgets(fileline, LINELEN, file);
    if (feof(file)) return(TRUE);	/* IS AN ERROR or eof */
    nchars = 0;
    while (fileline[nchars++]);
    nchars--;
    nchars--;

/*
 * sscanf of floats is TOOO slow:
 *     sscanf(fileline, "%2f%2f%2f%c%2f%2f ... );
 * use alternate:
 */
#define F2(i) (((fileline[i]-'0')*10.0+fileline[i+1]-'0'))
#define F3(i) (((fileline[i]-'0')*100.0+(fileline[i+1]-'0')*10+fileline[i+2]-'0'))
#define F4(i) (((fileline[i]-'0')*1000.0+(fileline[i+1]-'0')*100+(fileline[i+2])-'0')*10+fileline[i+3]-'0')
#define F3M(i) (((fileline[i]-'A'+10.0)*100+(fileline[i+1]-'0')*10+fileline[i+2]-'0'))
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


    obj_lon = (RAHRSSEC*rah + RAMINSEC*ram + RASECSEC*ras)/DLDEGSEC;
    dl = (DLDEGSEC*dld + DLMINSEC*dlm)/DLDEGSEC;
    obj_lat = (fileline[6]  == '-') ? -dl : dl;
    
    /* set unknowns to blanks */
    obj_color[0] = ' ';
    obj_color[1] = ' ';
    obj_color[2] = '\0';
    obj_label[0] = ' ';
    obj_label[1] = ' ';
    obj_label[2] = '\0';
    obj_constell[0] = ' ';
    obj_constell[1] = ' ';
    obj_constell[2] = ' ';
    obj_constell[3] = '\0';
    
    if  (isdigit(fileline[14])) {
    /*
     * old reduced Yale catalog
     */
      inten = F3(12);
      if (fileline[11] == '0' || fileline[11] == '+') obj_mag = inten/100.0;
      else if (fileline[11] == '-') obj_mag = -inten/100.0;
      else obj_mag = F4(11)/1000.0;	/* new feature for stars >= 10.0 mag */
      
      if (nchars > 15) {
	obj_type[0] = fileline[15];
	obj_type[1] = fileline[16];
	ptr = &fileline[MIN(17,nchars)];
	i = 0;
	while (*ptr == ' ') ptr++;
	while (*ptr != ',' && *ptr != '\n' && *ptr)
	  obj_name[i++] = *ptr++;
	obj_name[i] = '\0';
	if (*++ptr) obj_commnt = ptr;
	else obj_commnt = "";
/* Next 2 lines Not in readfile.c readstar, not needed there */
	if (obj_commnt[strlen(obj_commnt) -1] == '\n')
	  obj_commnt[strlen(obj_commnt) -1] = '\0';
      } else {
	obj_type[0] = obj_type[1] = 'S'; /* Default SS single star */
	obj_name[0] = '\0';
	obj_commnt = "";
      }
    } else {
      /*
       * new reduced Yale catalog
       */
      m1 = fileline[11];
      obj_mag = ((m1 == '-') ? -F2(12)/10.0 :
		 (m1 <= '9') ? F3(11)/100.0 : F3M(11)/100.0);
      /* let's get Sirius */
    
      /*
       * extract color, label, constellation, name, and comment
       * Would be faster to just guarentee that the data file is correct
       */
      if (nchars > 22) {
	obj_constell[0] = fileline[20];
	obj_constell[1] = fileline[21];
	obj_constell[2] = fileline[22];
	obj_constell[3] = '\0';
      }
      if (nchars > 19) {
	obj_label[0] = fileline[18];
	obj_label[1] = fileline[19];
	obj_label[2] = '\0';
      }
      if (nchars > 17) {
	obj_color[0] = fileline[16]; 
	obj_color[1] = fileline[17];
	obj_color[2] = '\0';
      }
      if (nchars > 15) {
	obj_type[0] = fileline[14];
	obj_type[1] = fileline[15];
      }

      ptr = &fileline[MIN(23,nchars)];
      i = 0;
      while (*ptr == ' ') ptr++;
      while (*ptr != ',' && *ptr != '\n' && *ptr)
	obj_name[i++] = *ptr++;
      obj_name[i] = '\0';
      if (*++ptr) obj_commnt = ptr;
      else obj_commnt = "";
/* Next 2 lines Not in readfile.c readstar, not needed there */
      if (obj_commnt[strlen(obj_commnt) -1] == '\n')
	obj_commnt[strlen(obj_commnt) -1] = '\0';
    }
  }

  return(FALSE); /* NO error */
}


/* readsif reads standard starchart interchange format files,
   extracting the same data as readstar, if possible, and loading
   the same variables */
int readsif(file, sepchar)
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
  i = 0; 
  while (i < strlen(parsed_line[1])) {
    if ((parsed_line[1][i] == '+') || (parsed_line[1][i] == '-')) {
      i++;
      while ((i < strlen(parsed_line[1])) && (parsed_line[1][i] == ' '))
	parsed_line[1][i++] = '0';
    } else i++;
  };
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

  if (!to_consindx(obj_constell)) strcpy(obj_constell,"   ");

  /* Commas should not appear in name field */
  i = 0;
  while (obj_name[i])
    if (obj_name[i++] == ',')
      fprintf(stderr, "Warning: comma in name field:\"%s\"\n", obj_name);

  return FALSE;
}


/* write lineread format */
void writelr(outfile)
     FILE *outfile;
{
  int ra_h, ra_m, ra_s;
  int de_d, de_m;
  char outline[LINELEN];
  char dsign;
  char mstr[4];
  int imag;

  if ((obj_lon > 360.0) || (obj_lon < 0.0)) {
    fprintf(stderr, "Error: R.A. out of range:\"%f\"\n", obj_lon/15.0);
    obj_lon = 0.0;
    obj_mag = 35.0;
  };
  if ((obj_lat > 90.0) || (obj_lat < -90.0)) {
    fprintf(stderr, "Error: declination out of range:\"%f\"\n", obj_lat);
    obj_lat = 0.0;
    obj_mag = 35.0;
  };
  if (obj_mag > 35.0)
    fprintf(stderr, "Warning: magnitude out of range:\"%f\"\n", obj_mag);
  

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
  de_m = (obj_lat - de_d) * 60 + 0.5;

  if (de_m >= 60) {de_m -= 60; de_d++;};

  imag = fabs(obj_mag);
  if (obj_mag <= -10.0)
    sprintf(mstr, "-99");
  else if (obj_mag < 0.0)
    sprintf(mstr, "-%02d", (int) (-obj_mag * 10 + 0.5));
  else if (imag >= 10) {
    /* Hex+ format for stars dimmer than 9th mag */
    sprintf(mstr, "%03d", (int) ((obj_mag - imag) * 100 + 0.5));
    mstr[0] = 'A' - 10 + imag;
    if (imag > 35)
      mstr[0] = 'Z';
  } else {
    sprintf(mstr, "%03d", (int) (obj_mag * 100 + 0.5));
  }

  /* Try to trim off excess spaces */
  if (!obj_commnt[0] && !obj_name[0] && !strcmp(obj_constell, "   "))
    /* can trim constellation */
    obj_constell[0] = '\0';

  if (!obj_constell[0] && !strcmp(obj_label, "  "))
    /* can trim label */
    obj_label[0] = '\0';

  if (!obj_label[0] && !strcmp(obj_color, "  "))
    /* can trim color */
    obj_color[0] = '\0';





  if (obj_commnt[0])
    sprintf(outline, "%02d%02d%02d%c%02d%02d%s%s%s%s%s%s,%s",
	    ra_h, ra_m, ra_s, dsign, de_d, de_m,
	    mstr,
	    obj_type, obj_color, obj_label, obj_constell,
	    obj_name, obj_commnt);
  else
    sprintf(outline, "%02d%02d%02d%c%02d%02d%s%s%s%s%s%s",
	    ra_h, ra_m, ra_s, dsign, de_d, de_m,
	    mstr,
	    obj_type, obj_color, obj_label, obj_constell, obj_name);

  fprintf(outfile, "%s\n", outline);
}


/* write binfull format */
void wrbinfull(file)
     FILE *file;
{
  int to_consindx();

  if ((obj_lon > 360.0) || (obj_lon < 0.0)) {
    fprintf(stderr, "Error: R.A. out of range:\"%f\"\n", obj_lon/15.0);
    obj_lon = 0.0;
    obj_mag = 35.0;
  };
  if ((obj_lat > 90.0) || (obj_lat < -90.0)) {
    fprintf(stderr, "Error: declination out of range:\"%f\"\n", obj_lat);
    obj_lat = 0.0;
    obj_mag = 35.0;
  };

  strcpy(name_comment, obj_name);
  if (obj_commnt[0]) {
    strcat(name_comment, ",");
    strcat(name_comment, obj_commnt);
    if (name_comment[strlen(name_comment)-1] == '\n')
      name_comment[strlen(name_comment)-1] = '\0';
  }

  binfull_out.lat = obj_lat * 3600 * 1000L;
  binfull_out.lon = obj_lon * 3600 * 1000L;
  binfull_out.mag = obj_mag * 1000L;
  binfull_out.tycolb[0] = obj_type[0];
  binfull_out.tycolb[1] = obj_type[1];
  binfull_out.tycolb[2] = obj_color[0];
  binfull_out.tycolb[3] = obj_color[1];
  binfull_out.tycolb[4] = obj_label[0];
  binfull_out.tycolb[5] = obj_label[1];
  binfull_out.consindx = to_consindx(obj_constell);
  binfull_out.strlen = strlen(name_comment);

  if (fwrite((char *) &binfull_out, sizeof(binfull_out), 1, file) != 1) {
    perror("Error writing output file");
    exit(2);
  }

  if (name_comment[0] &&
      (fwrite((char *) name_comment, binfull_out.strlen, 1, file) != 1)) {
    perror("Error writing output file");
    exit(2);
  }
}

int to_consindx(cons)
     char *cons;
{
  int i;

  if (!cons[0]) return 0;

  i = -1;
  while (con_table[++i][0])
    if (!strcmp(cons, con_table[i])) break;

  return (con_table[i][0] ? i : 0);
}

/* write binstar format */
void wrbinstar(file)
     FILE *file;
{
  int to_consindx();

  if ((obj_lon > 360.0) || (obj_lon < 0.0)) {
    fprintf(stderr, "Error: R.A. out of range:\"%f\"\n", obj_lon/15.0);
    obj_lon = 0.0;
    obj_mag = 35.0;
  };
  if ((obj_lat > 90.0) || (obj_lat < -90.0)) {
    fprintf(stderr, "Error: declination out of range:\"%f\"\n", obj_lat);
    obj_lat = 0.0;
    obj_mag = 35.0;
  };

  binstar_out.lat = obj_lat * 3600 * 1000L;
  binstar_out.lon = obj_lon * 3600 * 1000L;
  binstar_out.mag = obj_mag * 1000L;

  if (fwrite((char *) &binstar_out, sizeof(binstar_out), 1, file) != 1) {
    perror("Error writing output file");
    exit(2);
  }
}

void wrbinobj(file)
     FILE *file;
{
  int to_consindx();

  if ((obj_lon > 360.0) || (obj_lon < 0.0)) {
    fprintf(stderr, "Error: R.A. out of range:\"%f\"\n", obj_lon/15.0);
    obj_lon = 0.0;
    obj_mag = 35.0;
  };
  if ((obj_lat > 90.0) || (obj_lat < -90.0)) {
    fprintf(stderr, "Error: declination out of range:\"%f\"\n", obj_lat);
    obj_lat = 0.0;
    obj_mag = 35.0;
  };

  binobj_out.lat = obj_lat * 3600 * 1000L;
  binobj_out.lon = obj_lon * 3600 * 1000L;
  binobj_out.mag = obj_mag * 1000L;
  binobj_out.type[0] = obj_type[0];
  binobj_out.type[1] = obj_type[1];

  if (fwrite((char *) &binobj_out, sizeof(binobj_out), 1, file) != 1) {
    perror("Error writing output file");
    exit(2);
  }
}


/* write sif format */
void writesif(file, sepchar)
     FILE *file;
     char sepchar;
{
  if ((obj_lon > 360.0) || (obj_lon < 0.0)) {
    fprintf(stderr, "Error: R.A. out of range:\"%f\"\n", obj_lon/15.0);
    obj_lon = 0.0;
    obj_mag = 35.0;
  };
  if ((obj_lat > 90.0) || (obj_lat < -90.0)) {
    fprintf(stderr, "Error: declination out of range:\"%f\"\n", obj_lat);
    obj_lat = 0.0;
    obj_mag = 35.0;
  };

  fprintf(file, "%.10f%c%.10f%c%f%c%s%c%s%c%s%c%3s%c%s%c%s\n",
	  obj_lon/15.0, sepchar,
	  obj_lat, sepchar,
	  obj_mag, sepchar,
	  obj_type, sepchar,
	  obj_color, sepchar,
	  obj_label, sepchar,
	  obj_constell, sepchar,
	  obj_name, sepchar,
	  obj_commnt);
}


/*  Old precession formula */
double M, N;
void initxform(ein, eout)
     double ein, eout;
{
  double T;

  T = (eout - ein) / 100.0;
  M = (1.2812323 + (0.0003879 + 0.0000101 * T) * T) * T;
  N = (0.5567530 - (0.0001185 - 0.0000116 * T) * T) * T;
}

void xform(rin, din, rout, dout)
     double rin, din, *rout, *dout;
{
  double am, dm, a2, d2;

/* am = /alpha_m, dm = /delta_m */

  am = rin + (M + N * DSIN(rin) * DTAN(din))/2.0;
  dm = din + N*DCOS(am)/2.0;
  a2 = rin + M + N*DSIN(am)*DTAN(dm);
  d2 = din + N * DCOS(am);
  
  if (a2 >= 360.0) a2 -= 360.0;
  if (a2 < 0.0) a2 += 360.0;

  *rout = a2;
  *dout = d2;
}

/* Rigorous precession */
/* From Astronomical Ephemeris 1989, p. B18 */
/*
from t_0 to t:

A = 
sin(alpha - z_A) cos(delta) = sin(alpha_0 + zeta_A) cos(delta_0);
B =
cos(alpha - z_A) cos(delta) = cos(alpha_0 + zeta_A) cos(theta_A) cos(delta_0)
				- sin(theta_A) sin(delta_0);
C =
                 sin(delta) = cos(alpha_0 + zeta_A) sin(theta_A) cos(delta_0)
				+ cos(theta_A) sin(delta_0);

delta = asin(C);
alpha = atan2(A/B) + z_A;



from t to t_0:

A =
sin(alpha_0 + zeta_A) cos(delta_0) = sin(alpha - z_A) cos(delta);

B =
cos(alpha_0 + zeta_A) cos(delta_0) = cos(alpha - z_A) cos(theta_A) cos(delta)
				+ sin(theta_A) sin(delta);
C =
                      sin(delta_0) = -cos(alpha - z_A) sin(theta_A) cos(delta)
				+ cos(theta_A) sin(delta)

delta_0 = asin(C);
alpha_0 = atan2(A,B) - zeta_A;
*/



/* For reduction with respect to the standard epoch t_0 = J2000.0
zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T
   Z_A  = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T
theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T

in degrees.

T = (jd - 2451545.0)/36525.0;

alpha2000 = alpha_0;
delta2000 = delta_0;
*/


void precess_f(from_equinox, to_equinox,
	     alpha_in, delta_in, alpha_out, delta_out)
     double from_equinox, to_equinox,
       alpha_in, delta_in, *alpha_out, *delta_out;
{
  double zeta_A, z_A, theta_A;
  double T;
  double A, B, C;
  double alpha, delta;
  double alpha2000, delta2000;
  double into_range();


  /* From from_equinox to 2000.0 */
  if (from_equinox != 2000.0) {
    T = (from_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T;

    A = DSIN(alpha_in - z_A) * DCOS(delta_in);
    B = DCOS(alpha_in - z_A) * DCOS(theta_A) * DCOS(delta_in)
      + DSIN(theta_A) * DSIN(delta_in);
    C = -DCOS(alpha_in - z_A) * DSIN(theta_A) * DCOS(delta_in)
      + DCOS(theta_A) * DSIN(delta_in);

    alpha2000 = into_range(DATAN2(A,B) - zeta_A);
    delta2000 = DASIN(C);
  } else {
    /* should get the same answer, but this could improve accruacy */
    alpha2000 = alpha_in;
    delta2000 = delta_in;
  };


  /* From 2000.0 to to_equinox */
  if (to_equinox != 2000.0) {
    T = (to_equinox - 2000.0)/100.0;
    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
    theta_A = 0.5567530* T - 0.0001185* T*T + 0.0000116* T*T*T;

    A = DSIN(alpha2000 + zeta_A) * DCOS(delta2000);
    B = DCOS(alpha2000 + zeta_A) * DCOS(theta_A) * DCOS(delta2000)
      - DSIN(theta_A) * DSIN(delta2000);
    C = DCOS(alpha2000 + zeta_A) * DSIN(theta_A) * DCOS(delta2000)
      + DCOS(theta_A) * DSIN(delta2000);

    alpha = into_range(DATAN2(A,B) + z_A);
    delta = DASIN(C);
  } else {
    /* should get the same answer, but this could improve accruacy */
    alpha = alpha2000;
    delta = delta2000;
  };

  *alpha_out = alpha;
  *delta_out = delta;
}


double into_range(ang)
     double ang;
{
  int i;

  while (ang < 0.0) ang += 360.0;
  /* Shouldn't be more than once */

  i = ang/360.0;

  ang = ang - i * 360;

  return(ang);
}


