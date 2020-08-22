/*
 * Help for starXaw
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starXawHelp.c,v 1.1 1990-03-30 16:39:06 vrs Exp $";

#include <stdio.h>
#include <math.h>

#include "star3.h"
#include "starXaw.h"

#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef X11R4
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#define ASCII_STRING
#define XAW_BC
#include <X11/Xaw/AsciiText.h>
#else
#include <X11/Command.h>
#include <X11/Form.h>
#include <X11/Box.h>
#include <X11/AsciiText.h>
#endif


Widget help_wid;

/*
123456789 123456789 123456789 1234567.
2
3
4
5
6
7
8
9
0
1
2
3
4
5
6
7
8
9
0
1
2
3
4
5
6
7
823456789 123456789 123456789 1234567.
*/

static char *help_pages[] = {
"          Welcome to StarXaw\n\
\n\
This is the X11 Athena Widgets\n\
interface to Starchart, version 3.1.\n\
\n\
This is the first help page.  For more\n\
information, press the Help button\n\
below.\n\
\n\
The Quit button exits the program.\n\
\n\
The Controls buttons open a new window\n\
for changing the variables which\n\
control the appearance of the chart.\n\
\n\
Draw Chart draws the chart as\n\
specified by the options set above.\n\
\n\
Exceptions: Save / Load File and\n\
Additional Controls have options which\n\
only take effect when Draw Chart is\n\
selected.\n\
\n\
\n\
Press the Help button for the next\n\
help page.  When you reach the last\n\
page, you will return to this page.",


"Page 2\n\
\n\
The Output region is used to display\n\
the status of the program, and some\n\
other output.  Note that some errors\n\
are written to the standard error\n\
output, and do not appear in this\n\
space. \n\
\n\
This Help space will also be used to\n\
provide details about the selected\n\
control.\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Press the Help button for the next\n\
help page.",


"Page 3      The Controls\n\
\n\
The buttons and what they control: \n\
\n\
Chart - Center coordinates and scale\n\
        of chart, and title.\n\
Type  - Chart type: currently either\n\
        one large map or one large and\n\
        one smaller map of a larger\n\
        region (The thumbnail map).\n\
Magnitudes - Limiting magnitudes.\n\
Layers - Layer drawing order.  Each\n\
        map is drawn as several\n\
	layers, which may be drawn in\n\
	any order.\n\
Data Files - The data files and their\n\
        file formats.\n\
Projection Mode - The projection\n\
        transformation used for the\n\
        main map.\n\
Magnitude Labels - Labelling of stars\n\
        with their magnitudes.\n\
Grid -  Right Ascension and\n\
        Declination grid lines.\n\
\n\
Press the Help button for the next\n\
help page.",

"Page 4     More Controls\n\
\n\
The following controls allow further\n\
control, and some are specific to the\n\
X11 version of Starchart.\n\
\n\
Additional Control - Allows for\n\
        detailed editing of the\n\
        structure which specifies\n\
        each map in the chart\n\
        (the ``Mapwin'' structure).\n\
X Parameters - Controls specific to\n\
        the X11 version.\n\
Save / Load File - input and output of\n\
        Mapwin format files, and\n\
        output of the current settings\n\
        into a ``Starrc'' format file.\n\
\n\
Additional information about each\n\
button will appear here when the\n\
button is selected.\n\
\n\
\n\
\n\
\n\
Press the Help button for the next\n\
help page.",

"Page 5\n\
\n\
Each control window has a Dismiss\n\
button and a Cancel button.  Some\n\
additionally have an Apply button.\n\
\n\
If there is an Apply button, it must\n\
be pressed for the settings to take\n\
effect.\n\
\n\
The Dismiss button closes the window.\n\
If there is an Apply button, some\n\
settings will not take effect.\n\
\n\
The Cancel button cancels any changes\n\
made, and closes the window.\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Press the Help button to return to the\n\
first help page."
};
#define Nhelp_pages 5

#define HELPSIZE 30*40
/* Can display 27*(38+\n) characters in 6x13 font */
char help_buffer[HELPSIZE];

static int hpg_no = 0;


void show_help()
{
  Arg args[2];


  XtSetArg(args[0], XtNlabel, help_buffer);
  XtSetValues(help_wid, args, (Cardinal)1);
  XtTextSetLastPos(help_wid, strlen(help_buffer));
}

void XHelp(widget,closure,callData)
     Widget widget;
     caddr_t closure;		/* Widget */
     caddr_t callData;
{
  hpg_no++;
  if (hpg_no == Nhelp_pages)
    hpg_no = 0;

  strcpy(help_buffer, help_pages[hpg_no]);
  show_help();
}


int start_help()
{
  strcpy(help_buffer, help_pages[hpg_no]);
  return sizeof(help_buffer);
}

un_help()
{
  strcpy(help_buffer, help_pages[hpg_no]);
  show_help();
}



/* Chart */
static char chart_page[] =
  "                Chart\n\
\n\
Here, the coordinates of the center of\n\
the chart, the scale of the main map,\n\
and the title of the chart are set.\n\
\n\
Either specify these values directly,\n\
or read them for a given constellation\n\
from the constellation data file.\n\
\n\
The values for R.A. and Dec. are given\n\
in hh.mm and dd.mm format respectively,\n\
e.g. an R.A. of 18.30 means 18 hrs.,\n\
30 min.\n\
\n\
In addition, the main map may be\n\
drawin flipped top to bottom (for use\n\
with inverting telescopes), if so\n\
specified by the ``Normal/Inverted''\n\
button.";

chart_help()
{
  strcpy(help_buffer, chart_page);
  show_help();
}

/* Type */
static char type_page[] =
  "                 Type\n\
\n\
Currently, two standard chart types\n\
are defined.\n\
\n\
In the first, there is a large main\n\
map, a smaller thumbnail map showing\n\
a larger area, and a legend.\n\
\n\
In the second, there is only one large\n\
map.";

type_help()
{
  strcpy(help_buffer, type_page);
  show_help();
}

/* Magnitudes */
static char magnitudes_page[] =
  "              Magnitudes\n\
\n\
There are three limiting magnitudes.\n\
\n\
The first controls the display of the\n\
object name, e.g. Betelgeuse.\n\
\n\
The second controls the display of the\n\
Bayer or Flamsteed label for a star,\n\
e.g. 28, or RY, or <alpha>.\n\
\n\
The third controls the display of the\n\
symbol for the object.";

magnitudes_help()
{
  strcpy(help_buffer, magnitudes_page);
  show_help();
}

/* Layers */
static char layers_page[] =
  "                Layers\n\
\n\
Each map is drawn as several layers.\n\
This controls which layers are drawn,\n\
and the order in which they are drawn.\n\
\n\
The outline (including tick marks when\n\
possible), R.A. and Declination grids\n\
(if specified), ecliptic,\n\
constellation boundaries,\n\
constellation patterns, and\n\
constellation names, are all separate\n\
layers.  The legend is also considered\n\
as a separate layer.\n\
\n\
In addition, the data from the object\n\
files may be drawn either as the\n\
object is read (``All Files''), or as\n\
separate layers for the: symbol, name,\n\
magnitude label, vectors, and areas\n\
specified in the files.\n\
\n\
Pressing a button causes the layer for\n\
that position to cycle through the\n\
possiblilities.";

layers_help()
{
  strcpy(help_buffer, layers_page);
  show_help();
}

/* Map_Files */
static char dfiles_page[] =
  "              Data Files\n\
\n\
The data files, and the order in which\n\
they are read, are set in this dialog.\n\
\n\
Each file has a name and a type.  The\n\
type specifies the format of the file.\n\
Pressing the type button cycles\n\
through the possiblilties.";

dfiles_help()
{
  strcpy(help_buffer, dfiles_page);
  show_help();
}

/* Pmode */
static char pmode_page[] =
  "           Projection Mode\n\
\n\
There are currently five projection\n\
transformations available.  Press the\n\
button to cycle through the\n\
possiblilties.";

pmode_help()
{
  strcpy(help_buffer, pmode_page);
  show_help();
}

/* Maglabels */
static char maglabels_page[] =
  "           Magnitude Labels\n\
\n\
It is often useful to have stars in a\n\
given range of magnitudes labeled with\n\
their magnitudes, e.g. in an asteroid\n\
finding chart, or a variable star\n\
chart.\n\
\n\
Set bright and faint limits to the\n\
range, and specify ``Label stars with\n\
their magnitudes''.";

maglabels_help()
{
  strcpy(help_buffer, maglabels_page);
  show_help();
}

/* Grid */
static char grid_page[] =
  "                 Grid\n\
\n\
A coordinate lines in right\n\
ascension and declination may be\n\
drawn, with a specified spacing and\n\
starting coordinates.\n\
\n\
The values for R.A. and Dec. are given\n\
in hh.mm and dd.mm format\n\
respectively, e.g. an R.A. Step of\n\
0.30 means draw the R.A. lines with a\n\
separation of 30 min.\n\
\n\
The ``Show R.A. grid'' and ``Show Dec.\n\
grid'' buttons control the display of\n\
the lines for R.A. and Dec., respectively.";

grid_help()
{
  strcpy(help_buffer, grid_page);
  show_help();
}

/* Emapwins */
static char emapwins_page[] =
  "          Additional Control\n\
\n\
Each map in the chart is controlled by\n\
a structure.\n\
\n\
The other buttons control default\n\
settings for values in these\n\
structures.\n\
\n\
By choosing ``Edit'', you will be\n\
given the chance to control the\n\
detailed appearance of each map, and\n\
add additional maps to the chart.";

emapwins_help()
{
  strcpy(help_buffer, emapwins_page);
  show_help();
}

/* Xparms */
static char xparms_page[] =
  "             X Parameters\n\
\n\
There are several controls specific to\n\
X11, and starXaw.\n\
\n\
You may use flexible window dimensions\n\
to set the window size when the chart\n\
window is opened, or you may use fixed\n\
window dimensions specified in the\n\
width and height fields, or you may\n\
use dimensions which will duplicate\n\
the appearance of the page produced by\n\
the starpost program.\n\
\n\
You may use an X backing store if your\n\
X display has sufficient memory.  This\n\
allows the window to be repainted if\n\
it is overwritten.\n\
\n\
You may specify that color should not\n\
be used, even if it is available.";

xparms_help()
{
  strcpy(help_buffer, xparms_page);
  show_help();
}

/* Svld */
static char svld_page[] =
  "           Save / Load File\n\
\n\
A ``mapwin'' file contains the map\n\
description structures for the chart\n\
(see ``Additional Control'' for more\n\
info).  These files may be read,\n\
or written for future use, including\n\
use by other starchart programs.\n\
\n\
A ``.starrc'' file contains the global\n\
specifications for the chart set by\n\
the other buttons.";

svld_help()
{
  strcpy(help_buffer, svld_page);
  show_help();
}


/* D_mapwininput */
static char mwinin_page[] =
  "       Edit mapwindow structure\n\
\n\
Note that if you change the number of\n\
structures, the first structure is\n\
used for the defaults in the new\n\
structure.\n\
\n\
Note also that since the Legend is\n\
drawn as a layer for a window, you\n\
must edit the Layers for the\n\
structures to ensure that the correct\n\
legend, and only the correct legend,\n\
is drawn.";

mwinin_help()
{
  strcpy(help_buffer, mwinin_page);
  show_help();
}

/* edit_structs */
static char estru_page[] =
  "            Edit Structure\n\
\n\
Note that the magnitude limits are set\n\
individually for each data file.\n\
\n\
Press the Next button (if any) to move\n\
to the next structure,  press the Prev\n\
button (if any) to move to the\n\
previous structure.";

estru_help()
{
  strcpy(help_buffer, estru_page);
  show_help();
}

/* edit_layers */
static char elay_page[] =
  "             Edit Layers\n\
\n\
Each map is drawn as several layers.\n\
This controls which layers are drawn,\n\
and the order in which they are drawn.\n\
\n\
The outline (including tick marks when\n\
possible), R.A. and Declination grids\n\
(if specified), ecliptic,\n\
constellation boundaries,\n\
constellation patterns, and\n\
constellation names, are all separate\n\
layers.  The legend is also considered\n\
as a separate layer.\n\
\n\
In addition, the data from the object\n\
files may be drawn either as the\n\
object is read (``All Files''), or as\n\
separate layers for the: symbol, name,\n\
magnitude label, vectors, and areas\n\
specified in the files.\n\
\n\
Pressing a button causes the layer for\n\
that position to cycle through the\n\
possiblilities.";

elay_help()
{
  strcpy(help_buffer, elay_page);
  show_help();
}

/* edit_files */
static char efiles_page[] =
  "              Edit Files\n\
\n\
Press the Next button (if any) to move\n\
to the next file,  press the Prev\n\
button (if any) to move to the\n\
previous file.";

efiles_help()
{
  strcpy(help_buffer, efiles_page);
  show_help();
}






/* Pointer input */
static char pointer_page[] =
  "Press the left mouse button\n\
or equivalent to\n\
display the coordinates at the\n\
pointer location.\n\
\n\
Press the right mouse button\n\
or equivalent to\n\
identify objects near the\n\
pointer location.\n\
\n\
Results appear in the\n\
Output window and on\n\
the standard error output.\n\
\n";


pointer_help()
{
  strcpy(help_buffer, pointer_page);
  show_help();
}

