/* Header for parse input */
/*
 *
 * $Header: /home/Vince/cvs/net/starchart/starchart/parse_input.h,v 1.1 1990-03-30 16:38:57 vrs Exp $
 * 
*/
/* Variable setting */

/* RETURN CODES */
#define TINT 1000
#define TFLO 1001
#define TBOOL 1002
#define TNONE 1003
#define TCHAR 1004
#define TREPORT 1005

#define EXIT 101
#define END_INPUT 103
#define HELP_ME 104
#define SHOW_ME 105
#define WRITE_RC_FILE 110

#define R_NONE 0
#define R_RA 1
#define R_DEC 2
#define R_SCALE 3
#define R_NAMELIM 4
#define R_MAGLIM 5
#define R_GKLIM 6
#define R_TITLE 7
#define R_STARFILE 8
#define R_INDEXFILE 9
#define R_NEBFILE 10
#define R_PLANETFILE 11
#define R_USERFILE 12
#define R_MAPFILE 13
#define R_CONSTFILE 14
#define R_FINDCONST 15
#define R_BOUNDFILE 16
#define R_PATTERNFILE 17
#define R_CONSTLNAMEFILE 18
#define R_LAYERS 19
#define R_POLARMODE 20
#define R_BIGFLAG 21
#define R_PROJECTION_MODE 22
#define R_INVERT 23
#define R_CHART_TYPE 24
#define R_VRANGE 25
#define R_VMAGS 26
#define R_GRID 27
#define R_NOGRID 28
#define R_NOGRID_RA 29
#define R_NOGRID_DEC 30
#define R_NOMAGLBLS 31
#define R_DRIVER_CONTROL 32
#define R_WRITE_MAPWIN_FILE 33
#define R_READ_MAPWIN_FILE 34
#define R_PRECESS_TO 35

/* VARIABLE TYPES */
#define V_NONE 0
#define V_CHAR 1
#define V_FLOAT 2
#define V_INT 3
#define V_BOOL 4

typedef struct {
  char *word;
  int return_code;
  int variable_type;
  char *synonym;
  char *doc;
} keyword;

#ifdef PARSE_INPUT
keyword keyword_map[] =
{
/*
  {"int", TINT, V_INT, "", ""},
  {"bool", TBOOL, V_BOOL, "", "Test doc string"},
  {"none", TNONE, V_NONE, "", ""},
  {"flo", TFLO, V_FLOAT, "", ""},
  {"char", TCHAR, V_CHAR, "", ""},
*/
  {"exit", EXIT, V_NONE, "", "Exit the program"},
  {"quit", EXIT, V_NONE, "exit", ""},
  {"end_input", END_INPUT, V_NONE, "", "End input, display chart"},
  {"done_input", END_INPUT, V_NONE, "end_input", ""},
  {"done", END_INPUT, V_NONE, "end_input", ""},
  {"draw", END_INPUT, V_NONE, "end_input", ""},
  {"help", HELP_ME, V_NONE, "", ""},
  {"?", HELP_ME, V_NONE, "help", ""},
  {"show", SHOW_ME, V_NONE, "", ""},
  {"ra", R_RA, V_FLOAT, "", "Right Ascension of center of map"},
  {"dec", R_DEC, V_FLOAT, "", "Declination of center of map"},
  {"scale", R_SCALE, V_FLOAT, "", "Scale of center of map"},
  {"title", R_TITLE, V_CHAR, "", "Title of chart"},
  {"findconst", R_FINDCONST, V_CHAR, "",
     "Constellation, specification in constfile"},
  {"constfile", R_CONSTFILE, V_CHAR, "",
     "Filename for constellation location"},
  {"namelim", R_NAMELIM, V_FLOAT, "", "Limiting magnitude for object names"},
  {"lbllim", R_NAMELIM, V_FLOAT, "namelim", ""},
  {"gklim", R_GKLIM, V_FLOAT, "", "Limiting magnitude for object label"},
  {"maglim", R_MAGLIM, V_FLOAT, "", "Limiting magnitude for object"},
  {"starfile", R_STARFILE, V_CHAR, "", "Filename for stars"},
  {"indexfile", R_INDEXFILE, V_CHAR, "", "Filename for SAO stars"},
  {"nebfile", R_NEBFILE, V_CHAR, "", "Filename for nebulae"},
  {"planetfile", R_PLANETFILE, V_CHAR, "", "Filename for planets"},
  {"userfile", R_USERFILE, V_CHAR, "", "Filename for user file"},
  {"mapfile", R_MAPFILE, V_CHAR, "",
     "Filenames for files to replaces starfile etc."},
  {"boundfile", R_BOUNDFILE, V_CHAR, "",
     "Filename for constellation boundaries"},
  {"patternfile", R_PATTERNFILE, V_CHAR, "",
     "Filename for constellation patterns"},
  {"constlnamefile", R_CONSTLNAMEFILE, V_CHAR, "",
     "Filename for constellation names"},
  {"layers", R_LAYERS, V_CHAR, "",
     "Order for layers drawn in each window of the chart"},
  {"all_layer", R_LAYERS, V_CHAR, "layers", ""},
  {"polarmode", R_POLARMODE, V_BOOL, "", "Use stereographic projection"},
  {"projection_mode", R_PROJECTION_MODE, V_CHAR, "",
     "Projection mode (sansons, stereographic, gnomonic, etc."},
  {"invert", R_INVERT, V_BOOL, "", "Flip north and south"},
  {"precess_to", R_PRECESS_TO, V_FLOAT, "",
     "Precess to equinox and ecliptic of year."},
  {"bigflag", R_BIGFLAG, V_BOOL, "", "Chart contains one large window"},
  {"chart_type", R_CHART_TYPE, V_CHAR, "",
     "fullpage for one large window, threepart for thumbscale map\n\
\t\t\t\tand legend"},
  {"vrange", R_VRANGE, V_FLOAT, "",
     "label stars with magnitudes if in range"},
  {"vmags", R_VMAGS, V_FLOAT, "",
     "label stars with magnitudes if within (arg2) of (arg1)"},
  {"grid", R_GRID, V_FLOAT, "",
     "draw grid: grid ra_space dec_space [ra_start] [dec_start]"},
  {"nogrid", R_NOGRID, V_BOOL, "", "Don't draw grids"},
  {"nogrid_ra", R_NOGRID_RA, V_BOOL, "", "Don't draw grid in R.A."},
  {"nogrid_dec", R_NOGRID_DEC, V_BOOL, "", "Don't draw grid in dec."},
  {"nomaglbls", R_NOMAGLBLS, V_BOOL, "", "Don't label stars with magnitudes"},
  {"driver_control", R_DRIVER_CONTROL, V_CHAR, "",
     "special control string for device driver"},
  {"drivercontrol", R_DRIVER_CONTROL, V_CHAR, "driver_control", ""},
  {"additional", R_DRIVER_CONTROL, V_CHAR, "driver_control", ""},
  {"write_rc_file", WRITE_RC_FILE, V_CHAR, "",
     "Write current state to .starrc file"},
  {"write_mapwin_file", R_WRITE_MAPWIN_FILE, V_CHAR, "",
     "write detailed specification of chart to given filename"},
  {"read_mapwin_file", R_READ_MAPWIN_FILE, V_CHAR, "",
     "read detailed specification of chart from given filename"},
/*  {"report", R_REPORT, V_NONE, "", ""},*/
  {"", R_NONE, V_NONE, "", ""}
};

/* longest word  17 characters */
#endif
