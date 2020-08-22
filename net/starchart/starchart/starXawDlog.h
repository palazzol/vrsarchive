/* Header for Athena widgets dialogs */
/*
 *
 * $Header: /home/Vince/cvs/net/starchart/starchart/starXawDlog.h,v 1.1 1990-03-30 16:39:05 vrs Exp $
 * 
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
 */


#ifndef MAXPATHLEN
#define MAXPATHLEN 1025
#endif


typedef enum {
  NoDialog,
  ChartDialog,
  TypeDialog,
  MagnitudesDialog,
  LayersDialog,
  Map_filesDialog,
  Projection_modeDialog,
  Magnitude_labelsDialog,
  GridDialog,
  Edit_mapwinsDialog,
  X_parmsDialog
} DlogType;


#define ChartStringLen 12
typedef struct {
  double ra, de, sc;
  char ra_str[ChartStringLen], de_str[ChartStringLen], sc_str[ChartStringLen];
  Widget ra_wid, de_wid, sc_wid;
  char *title;
  Widget title_wid;
  char *constfile;
  int all_invert;
  int precess;
  double to_precess;
  char to_p_str[ChartStringLen];
  Widget to_p_wid;
} ChartD_S;

typedef struct {
  int chart_type;
} TypeD_S;

#define MagStringLen 12
typedef struct {
  double all_lbllim, all_maglim, all_gklim;
  char all_lbllim_str[MagStringLen], all_maglim_str[MagStringLen],
                  all_gklim_str[MagStringLen];
  Widget all_lbllim_wid, all_maglim_wid, all_gklim_wid;
} MagnitudesD_S;

typedef struct {
  int all_layer[MAXLAYRS];
  int numlayers;

  Widget wid[MAXLAYRS];
  int laynum[MAXLAYRS];
} LayersD_S;

typedef struct {
  char mapfiles[MAXMAPFILES][MAXPATHLEN];
  int mapftypes[MAXMAPFILES];
  int nummapfiles;

  Widget file_wid[MAXMAPFILES];
  Widget ftype_wid[MAXMAPFILES];
  int mapnum[MAXMAPFILES];
} Map_filesD_S;

typedef struct {
  int all_proj_mode;
} Projection_modeD_S;

typedef struct {
  double all_vmin, all_vmax;
  char all_vmin_str[MagStringLen], all_vmax_str[MagStringLen];
  Widget all_vmin_wid, all_vmax_wid;
  int use_vmin;
  int nomaglbls;
} Magnitude_labelsD_S;

#define GridStringLen 12
typedef struct {
  double all_rstep, all_dstep;
  char all_rstep_str[GridStringLen], all_dstep_str[GridStringLen];
  Widget all_rstep_wid, all_dstep_wid;
  int use_rstep;
  double all_rstrt, all_dstrt;
  char all_rstrt_str[GridStringLen], all_dstrt_str[GridStringLen];
  Widget all_rstrt_wid, all_dstrt_wid;
  int no_ra_grid;
  int no_dec_grid;
} GridD_S;

typedef struct {
  char *nothing;
} Edit_mapwinsD_S;

#define XParmStringLen 10
typedef struct {
  int post_preview;
  int post_landscape;
  int use_backup;
  int is_color;
  int fixed_size;
  int fixed_width, fixed_height;
  char fixed_width_str[XParmStringLen], fixed_height_str[XParmStringLen];
  Widget fixed_width_wid, fixed_height_wid;
} X_parmsD_S;


char *proj_mode_strings[] = {
  "Error        ",
  "Sansons      ",
  "Stereographic",
  "Gnomonic     ",
  "Orthographic ",
  "Rectangular  ",
};
#define MAXPMODE 5

char *lay_strings[] = {
  "None                ",
  "Outline             ",
  "R.A. Grid           ",
  "Dec. Grid           ",
  "Ecliptic            ",
  "Boundaries          ",
  "Patterns            ",
  "Constellation Names ",
  "All Files           ",
  "All Object Symbols  ",
  "All Object Names    ",
  "All Star Mag. labels",
  "All Vectors         ",
  "All Areas           ",
  "Legends             "
};
#define MAXLAYNUM 14

char *filetype_strings[] = {
  "",
  "LineRead ",
  "IndexType",
  "BinFull  ",
  "BinObj   ",
  "BinStar  ",
  "SAOFormat"
  };
#define MAXFTYPES 6

