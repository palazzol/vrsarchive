/*
 * GDOS Display driver for starchart.c ATARI ST - Version
 */

/*
 * Modified from 2.1 to starchart 3.0.  Holger Zessel, Nov 1989
 */
static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starst.c,v 1.1 1990-03-30 16:39:16 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>    /* isprint(), iscntrl(), etc. */
#include <osbind.h>   /* Operating system bindings */
#include <vdibind.h>  /* The virtual device interface routines */
#include <stdio.h>
#include "star3.h" /* Starchart information */

/* Global line A variables used by vdi; MUST be included */
int contrl[12], intin[128], ptsin[128], intout[128], ptsout[128];
/* Array used by vs_clip() */
int cliparray[] = { 0, 0, 32767, 32767 };

int work_in[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
int work_out[57];
int dummy;
int vdihandle; /* Virtual device's handle */
int vdihandle_save;
int device=1; /* GDOS-Outputdevice */

#define TRUE 1
#ifdef NULL
#undef NULL
#endif
#define NULL 0
	
/*
** Starchart was designed for 1024x768 pixels.  These macros scale the
** image to the 32768*32768 Atari ST GDOS-resolution in normalized mode.
*/
#define xadjust(x) ((x) * 32)
#define yadjust(y) ((y) * 42.667)


/* Externs */
extern int g_argc;
extern char **g_argv;

extern char *title;	/* Title of page */

extern mapwindow *mapwin[MAXWINDOWS];
extern int numwins;

extern int cur_function;
extern int cur_map_type;
extern int cur_map_tag;
extern char *cur_tag_field;


/* Scale multiplier, minimum,
   magnitude change, maximum, for thumbnail,*/
#define THSMUL 1.2
#define THSMIN 12.0
#define THMADJ 2.5
#define THMMAX 5.0


/* Exports */

/* The variables in the first few lines MUST be set by driver */
mapwindow fullpage = {
  880, 700, 20, 65,	/* width, height, x and y offsets */
  5.9, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  FULLPAGEMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */
  
/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* The variables in the first few lines MUST be set by driver */
mapwindow mainmap = {
  880, 500, 20, 265,	/* width, height, x and y offsets */
  5.9, 2.0, 2.05,	/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  MAINMAP,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};


/* The variables in the first few lines MUST be set by driver */
mapwindow thumbmap = {
  480, 195, 420, 35,	/* width, height, x and y offsets */
#ifndef ATARI_ST
  3.0+THMADJ, 1.0+THMADJ, 2.05+THMADJ,
#else
  5.5, 3.5, 4.55,
#endif
			/* default limiting mags for glyph, name, label */

/* The next several variables SHOULD be set by the driver,
   but are only used by the driver */
  THUMBNAIL,		/* Type of map: THUMBNAIL may have
			   some restrictions */
  0,			/* May be used by driver for whatever */
  "String",		/* May be used by driver for whatever */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
  SANSONS,		/* Projection mode */
  FALSE, FALSE,		/* Draw grids */
  0.5, 5.0,		/* grid step size */
  0.0, 0.0,		/* grid origin */

  FALSE,		/* Invert (flip north south) */
};

/* h & v tick text controls */
int htick_lim = 2;
int htext_lim = 80;
int htext_xoff = 2;
int htext_yoff = 17;
int vtick_lim = 2;
int vtext_lim = 20;
int vtext_xoff = 24;
int vtext_yoff = -4;

/* Scale multiplier, minimum,
   magnitude change, maximum, for thumbnail,*/
double th_smul=THSMUL;
double th_smin=THSMIN;
double th_madj=THMADJ;
double th_mmax=THMMAX;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

int CURRX, CURRY;



/* Device control argument */
D_control_arg(s)
char *s;
{
  int i = 0;
  int c;
  char *p;

  switch (*s) {
  case 'd':	/* output-device */
	device = atoi(&s[1]); /* device immediately after 'd' */
  	break;
  default:
	fprintf(stderr,"Wrong device control string!\n");
	exit(1);
	break;
  }
}



static open_virtual_device()
{
	  int i;
          if(appl_init()<0) /* Initialize the application and register it with AES */
	  {
	  	fprintf(stderr,"could not appl_init !!\n");
	  	exit(1);
	  }
          vdihandle = graf_handle(&i, &i, &i, &i); /* Get a VDI handle */
	  work_in[0]=(device==31) ? vdihandle:device;
	  work_in[10]=(device==31) ? 2:0;
          v_opnvwk(work_in, &vdihandle, work_out); /* Open the virtual workscreen */
	  if (vdihandle == 0)
	  {
	  	fprintf(stderr,"could not open virtual workstation!\n");
	  	appl_exit();
	  	exit(1);
	  }
	  work_in[10]=0;
}

/* Open the device */
D_open()
{
     int i;

     if (!vqgdos())
     {
     	fprintf(stderr,"No gdos loaded !!\n");
     	exit(1);
     }

     fprintf(stderr,"Device=%d\n",device);
     if ((device >= 1)&&(device <= 4))
     {
	  Cursconf(0,0);
	  open_virtual_device();
	  v_hide_c(vdihandle);
          v_clrwk(vdihandle);
          (void)vst_font(vdihandle,1);
     }
     else
     {	  int fontindex;
     	  char fontname[33];

	  if (device == 31)
	  {
	  	open_virtual_device(); /* seems to be necessary for metafiles*/
		work_in[10]=2;
		vdihandle_save = vdihandle; /* save handle */
	  }
          work_in[0]=device;
          v_opnwk(work_in, &vdihandle, work_out); /* Open the virtual workscreen */
	  if (vdihandle == 0)
	  {
	  	fprintf(stderr,"could not open physical workstation!\n");
	  	exit(1);
	  }
          (void)vst_load_fonts(vdihandle, 0);
	  fontindex = vqt_name(vdihandle,1,fontname);
	  vst_font(vdihandle, fontindex);
     }

     if (device == 31)
     {
	   intin[0]=0;
	   intin[1]=2100;
	   intin[2]=3050;
	   v_write_meta(vdihandle,3,intin,0,ptsin);
     }
     vs_clip(vdihandle, 1, cliparray); /* Set the clipping rectangle */
     vsf_color(vdihandle, 1);
     vsf_interior(vdihandle, 1);
     vsf_perimeter(vdihandle, 0);
     vsf_style(vdihandle,8);
     vswr_mode(vdihandle, 2);
     vst_rotation(vdihandle, 0);
     vst_color(vdihandle,1);

}

#ifdef ATARI_ST
D_die(a,b)
char *a,*b;
{
	D_close();
}
#endif

/* Close the device */
D_close()
{
     char text[80];
     if ((device >=1) && (device <= 4)) /* Screen */
     {
          gets(text);
          v_clsvwk(vdihandle); /* Close the virtual workscreen */
          appl_exit(); /* Remove the application from AES */
	  v_show_c(vdihandle,1);
	  Cursconf(1,0);
     }
     else
     {
	  if (device == 31)
	    v_meta_extents(vdihandle,0,0,0x7fff,0x7fff);
	    
          v_updwk(vdihandle);
          v_clswk(vdihandle); /* Close the physical workscreen */

	  if (device == 31)
     	  {
	      vdihandle = vdihandle_save;
              v_clsvwk(vdihandle); /* Close the virtual workscreen */
              appl_exit(); /* Remove the application from AES */
          }

     }
}


/* Move to (x, y) */
D_move(x, y)
     int x, y;
{
     CURRX = x;
     CURRY = y;
}


/* Draw a line of style line_style from the current point to (x, y) */
/* Note, this replaces vecdraw vecdrawdot and vecdrawhyph */
D_draw(x, y, line_style)
     int x, y;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
     int line[4];
     switch(line_style) {
     case SOLID:
	vsl_type(vdihandle,1);
	break;
     case DOTTED:
     case VECDOT:
	vsl_type(vdihandle,7);
     	vsl_udsty(vdihandle, 0xaaaa);
     	break;
     case DASHED:
     case VECDASH:
     	vsl_type(vdihandle,5);
     	break;
     default:
        vsl_type(vdihandle,1);
        break;
     }
     line[0] = xadjust(CURRX);
     line[1] = yadjust(CURRY);
     line[2] = xadjust(CURRX = x);
     line[3] = yadjust(CURRY = y);
     v_pline(vdihandle, 2, line);
}

/* This routine is encouraged to look at the extern cur_funtion
   and change the line style drawn as desired */


/* Move to (x1, y1) then draw a line of style line_style to (x2, y2) */
D_movedraw(x1, y1, x2, y2, line_style)
     int x1, y1, x2, y2;
     int line_style;	/* SOLID, DOTTED, DASHED, etc. */
{
  D_move(x1, y1);
  D_draw(x2, y2, line_style);
}


/* Set the color to be used for lines and text */
/* color_str is a 2 char (+ '\0') string containing
   a specification for a color,
   e.g. "G2" for the color of a star of spectral class G2, or "r7" for
   red, level seven.  The interpretation of the color string is left to
   the device driver */
D_color(color_str)
     char *color_str;
{
  switch (color_str[0]) {
  case 'O':
    break;
  case 'B':
    break;
  case 'A':
    break;
  case 'F':
    break;
  case 'G':
    break;
  case 'K':
    break;
  case 'M':
    break;
  case 'R':
  case 'N':
  case 'S':
    break;
  case 'r':
  case 'g':
  case 'b':
  case 'y':
  case 'c':
  case 'p':
  case 'o':
  case 'w':
    break;
  case ' ':
  default:
    break;
  }
}


/* Set the font and font size to be used for text. */
/* Note order of args */
D_fontsize(fsize, font)
     int fsize;	/* Size of font */
     int font;	/* e.g. TIMES, HELV, TIMES+ITALIC */
{
            (void)vst_point(vdihandle,fsize,&dummy,&dummy,&dummy,&dummy);
/*          (void)vst_height(vdihandle,fsize,&dummy,&dummy,&dummy,&dummy);*/
}
/* This routine is encouraged to look at the extern cur_funtion
   and change the font used as desired */


/* Display text string str at x,y, in current font and font size.
   if star_lbl is TRUE, string is a star label, use
     greek characters (if possible) */
D_text(x, y, str, star_lbl)
     int x, y;
     char *str;
     int star_lbl;
{
     int x1,y1;

     if (*str == '\0') /* Don't bother if the string is empty */
          return;

     if (star_lbl) {
    	/* remove leading spaces */
        while (*str == ' ') str++;
     /* can't display greek characters */
     }

     x1 = xadjust(x);
     y1 = yadjust(y);
     if (y1 > 32750)
          y1 = 32750; /* Make sure the bottom line shows up */

     vswr_mode(vdihandle, 1);
     v_gtext(vdihandle,x1,y1,str);
     vswr_mode(vdihandle, 2);


}



/* Return input coordinate in device coords where there are pointing devices */
D_inxy(x, y)
     int *x, *y;
{
}


/* Put non-displayed comment in output.  Allowed in postscript, but
   few other drivers will be able to support this. */ 
D_comment(str)
     char *str;
{
#if 0
  fprintf(stderr, "%s\n", str);
#endif
}


/**
Higher level functions
**/

/* Point sizes for font calls */
#define TITLESIZE 12
#define SUBTLSIZE 12
#ifdef NAMESIZE
#undef NAMESIZE
#endif
#define NAMESIZE 7
#define LBLSIZE 7
#define MAGSIZE 7

#ifdef STARIMAGES
int namesize = NAMESIZE;
int lblsize  = LBLSIZE;
int magsize  = MAGSIZE;
#endif

/* Fonts for font calls */
#define NAMEFNT TIMESROMAN
#define LBLFNT HELV
#define MAGFNT COURIER
#define TITLEFNT TIMESBOLD
#define SUBTLFNT TIMESROMAN

#ifdef STARIMAGES
int namefnt = NAMEFNT;
int lblfnt  = LBLFNT;
int magfnt  = MAGFNT;
#endif

int x_nameoffset = 10, y_nameoffset = 0;
int x_lbloffset = 0, y_lbloffset = 10;
int x_magoffset = 15, y_magoffset = -15;


#ifndef STARIMAGES

/* Interface Function */
/* Draw object at x, y.  properties set by other parameters */
drawobj(x, y, mag, type, color_str, label_field, con_str, obj_name,
	comment_str, file_line,
	draw_glyph, draw_text, use_lbl, use_name, use_mag)

     int x, y;
     double mag;	/* Magnitude of object */
     char *type;	/* 2 chars, object code and subcode */
     char *color_str;	/* 2 chars, spectral type for stars, 
			   color code otherwise */
     char *label_field;	/* 2 chars, Bayer or flamsteed for stars,
			   size in seconds for nebulae and
			   planets */
     char *con_str;	/* 3 chars, the constellation the object is in */
     char *obj_name;	/* Name of object */
     char *comment_str;	/* Comment field */
     char *file_line;	/* The full line from the file,
			   containing the above if it is in
			   standard format */
     int draw_glyph;	/* Draw object symbol */
     int draw_text;	/* Draw text */
     int use_lbl;	/* Label object with the label_field string */
     int use_name;	/* Label object with the obj_name string */
     int use_mag;	/* Label object with a 2 or 3 character string
			   containing the magnitude * 10 without
			   decimal point */
{
  char magstr[10];

/*fprintf(stderr, "%d %d %f <%s> <%s> <%s> <%s> <%s> <%s> <%s>\n", x, y, mag,
	type, color_str, label_field, con_str, obj_name, comment_str,
	file_line);*/

  if (draw_glyph)
    switch(type[0]) {
    case 'S':	drawStar(x,y,mag,type[1],color_str);
      break;
    case 'P':	drawPlan(x,y,mag,type[1],color_str, size_obj(label_field),
			 comment_str);
      break;
    case 'N':	drawNebu(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'G':	drawGalx(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'C':	drawClus(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'U':
      drawUnknown(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'O':
      drawOther(x,y,mag,type[1],color_str, size_obj(label_field));
      break;
    case 'V':
    case 'A':
    case 'I':
      break;
    case '#':
    default:
      break;
    };


/*
 * use name or label
 */
  if (draw_text) {
    if (type[0] == 'I')
      D_color(color_str);
    else
      D_color("  ");

    if (use_name && obj_name[0]) {
      D_fontsize(NAMESIZE, NAMEFNT);
      D_text(x+x_nameoffset, y+y_nameoffset, obj_name, FALSE);
    } else if (use_lbl &&
	       ((label_field[0] != ' ') || (label_field[1] != ' '))) {
      D_fontsize(LBLSIZE, LBLFNT);
      D_text(x+x_lbloffset, y+y_lbloffset, label_field, TRUE);
    }
/* If you want to mag label other objects, change this */
    if (use_mag && (type[0] == 'S')) {
      sprintf(magstr, "%02d", (int)(mag*10.0+0.5));
      D_fontsize(MAGSIZE, MAGFNT);
      D_text(x+x_magoffset, y+y_magoffset, magstr, FALSE);
    }
  }
}

drawStar(x, y, mag, type, color)
	int x, y;
	double mag;
	char type, *color;
{
	double mag2,mag3;
	mag2 = (250 - (mag+1)*30);
	mag2= (mag2<=0)?1:mag2;
	vsf_perimeter(vdihandle, 1);
	v_circle(vdihandle, (int)xadjust(x), (int)yadjust(y), (int)mag2);
	vsf_perimeter(vdihandle, 0);

	if ((type == 'V') && (mag<7))
	{
		mag3 = mag2 * 2.0;
		vsf_interior(vdihandle, 0);
		vsf_perimeter(vdihandle, 1);
		v_circle(vdihandle, (int)xadjust(x), (int)yadjust(y), (int)mag3);
		vsf_perimeter(vdihandle, 0);
		vsf_interior(vdihandle, 1);
	}
	else
	if (((type == 'B') || (type == 'D')) && (mag<7))
	{
		int line[4];
		line[1]=line[3]=(int)yadjust(y);
		line[0]= (int)(xadjust(x)-mag2-(mag2/4.0));
		line[2]= (int)(xadjust(x)+mag2+(mag2/4.0));
		v_pline(vdihandle, 2, line);
	}
}

drawPlan(x, y, mag, pcode, color, plansize)
     int x,y;
     double mag;
     char pcode, *color;
     long plansize;

{
     vsf_interior(vdihandle, 0);
     drawStar(x, y, mag, 'S', color);
     vsf_interior(vdihandle, 1);
}

drawGalx(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
     vsf_interior(vdihandle, 0);
     vsf_perimeter(vdihandle, 1);
     v_ellipse(vdihandle, (int)xadjust(x), (int)yadjust(y),(int)xadjust(12),(int)yadjust(8));
     vsf_perimeter(vdihandle, 0);
     vsf_interior(vdihandle, 1);
}

drawClus(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
     switch(type) {
	case 'G': {
     		int pxyarray[4];
     		int x1=(int)xadjust(x);
		int y1=(int)yadjust(y);
		int dx=(int)xadjust(6);
		int dy=(int)yadjust(6);

		vsf_interior(vdihandle, 0);
		vsf_perimeter(vdihandle, 1);
/*		v_circle(vdihandle, x1, y1, 300); */
		v_ellipse(vdihandle, x1, y1, (int)xadjust(10),(int)yadjust(10));
		vsf_perimeter(vdihandle, 0);
		vsf_interior(vdihandle, 1);
		pxyarray[0]=pxyarray[2]=x1;
		pxyarray[1]=y1+dy;
		pxyarray[3]=y1-dy;
		v_pline(vdihandle, 2, pxyarray);
		pxyarray[0]=x1-dx;
		pxyarray[2]=x1+dx;
		pxyarray[1]=pxyarray[3]=y1;
		v_pline(vdihandle, 2, pxyarray);
		break;
	}
     	case 'O':
     	default:
		vsf_interior(vdihandle, 0);
		vsl_type(vdihandle,3);
		/* vsl_type(vdihandle,7);*/
		/* vsl_udsty(vdihandle, 0xcccc);*/
		vsf_perimeter(vdihandle, 1);
		v_arc(vdihandle, (int)xadjust(x), (int)yadjust(y), 300,0,3600);
		vsf_perimeter(vdihandle, 0);
		vsl_type(vdihandle,1);
		vsf_interior(vdihandle, 1);
		break;
     }
}

drawNebu(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
     int pxyarray[4];
     int x1=(int)xadjust(x);
     int y1=(int)yadjust(y);
     int dx=(int)xadjust(5);
     int dy=(int)yadjust(5);
     /* box of 5 edgelength */
     pxyarray[0]= x1 - dx;
     pxyarray[1]= y1 - dy;
     pxyarray[2]= x1 + dx;
     pxyarray[3]= y1 + dy;
     vsf_interior(vdihandle, 0);
     vsl_type(vdihandle,1);
     vsf_perimeter(vdihandle, 1);
     v_bar(vdihandle,pxyarray);
     vsf_perimeter(vdihandle, 0);
     vsl_type(vdihandle,1);
     vsf_interior(vdihandle, 1);
}

drawUnknown(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
}

drawOther(x, y, mag, type, color, nebsize)
     int x, y;
     double mag;
     char type, *color;
     long nebsize;		/* -1 should give default size */
{
}

#else STARIMAGES

/*
** Draw a horizontal line.
*/

drawlen (x, y, dx, dy, len)
int x, y, dx, dy, len;
{
/*  D_movedraw(x+dx, y+dy,
	     x+dx+len,
	     y+dy, SOLID); another solution ! */
#undef DRAWTEST
#ifdef DRAWTEST
     int line[4],i;
     double cx,cy;

     cx=xadjust(x);
     cy=yadjust(y)+dy*5;

     line[0] = cx + dx*5;
     line[2] = cx + dx*5 + len*10 - 10;

     for (i=-5; i<=5 ; i++)
     {
     	line[1] = line[3] = cy + i;
     	v_pline(vdihandle, 2, line);
     }
#else
     int line[4];

     line[0] = xadjust(x) + dx;
     line[2] = xadjust(x) + dx + len - 1;
     line[1] = line[3] = yadjust(y) + dy;
     v_pline(vdihandle, 2, line);
#endif
}

#endif STARIMAGES

/* Functions for areas, drawn as lines for now */
/* Move to (x, y) to begin an area */
D_areamove(x, y)
     int x, y;
{
  D_move(x, y);
}

/* Add a segment to the area border */
D_areaadd(x, y)
     int x, y;
{
  D_draw(x, y, SOLID);
}

/* Fill the area, after adding the last segment */
D_areafill(x, y)
     int x, y;
{
  D_draw(x, y, SOLID);
}


/* Variables of positioning are here */
#define L_TIL 220
#define L_STIL 185

#define L_LMAR1 40
#define L_LMAR2 65
#define L_LTEXT 95
#define L_RMAR1 205
#define L_RMAR2 230
#define L_RTEXT 260

#define L_LINE1 150
#define L_LINE2 125
#define L_LINE3 100
#define L_LINE4 75
#define L_LINE5 50
#define L_LINE6 25


chartlegend(win)
     mapwindow *win;
{
  char ras[20], dls[20], outstr[40];
  if (!title[0]) title = "LEGEND";
  rastr(ras, win->racen);
  declstr(dls, win->dlcen);

  if (win->map_type != FULLPAGEMAP) {
    sprintf(outstr, "(%s,%s lim: %2.1f)", ras, dls, win->maglim);
    D_fontsize(TITLESIZE, TITLEFNT); D_text(L_LMAR1, L_TIL, title, FALSE);
    D_fontsize(SUBTLSIZE, SUBTLFNT); D_text(L_LMAR1, L_STIL, outstr, FALSE);
    
    drawStar(L_LMAR2, L_LINE1, 0.0, 'S', "  ");
    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_LTEXT, L_LINE1,"<0.5", FALSE);
    if (win->maglim >= 0.5)
      {
	drawStar(L_RMAR2, L_LINE1, 1.0, 'S', "  ");
	D_fontsize(NAMESIZE, NAMEFNT);
	D_text( L_RTEXT, L_LINE1,"<1.5", FALSE);
      }
    if (win->maglim >= 1.5)
      {
	drawStar(L_LMAR2, L_LINE2, 2.0, 'S', "  ");
	D_fontsize( NAMESIZE, NAMEFNT);
	D_text(L_LTEXT, L_LINE2,"<2.5", FALSE);
      }
    if (win->maglim >= 2.5)
      {
	drawStar(L_RMAR2, L_LINE2, 3.0, 'S', "  ");
	D_fontsize( NAMESIZE, NAMEFNT);
	D_text(L_RTEXT, L_LINE2,"<3.5", FALSE);
      }
    if (win->maglim >= 3.5)
      {
	drawStar(L_LMAR2, L_LINE3, 4.0, 'S', "  ");
	D_fontsize( NAMESIZE, NAMEFNT);
	D_text(L_LTEXT, L_LINE3,"<4.5", FALSE);
      }
    if (win->maglim > 4.5)
      {
	drawStar(L_RMAR2, L_LINE3, 5.0, 'S', "  ");
	D_fontsize( NAMESIZE, NAMEFNT);
	D_text(L_RTEXT, L_LINE3,">4.5", FALSE);
      }
    
    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_LTEXT,L_LINE4,"double", FALSE);
    drawStar(L_LMAR2,L_LINE4,2.0, 'D', "  ");
    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_RTEXT,L_LINE4,"variable",FALSE);
    drawStar(L_RMAR2,L_LINE4,2.0, 'V', "  ");

    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_LTEXT,L_LINE5,"planet", FALSE);
    drawPlan(L_LMAR2,L_LINE5,1.0, ' ', "  ", (long) -1);

    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_RTEXT,L_LINE5,"galaxy", FALSE);
    drawGalx(L_RMAR2,L_LINE5,1.0, 'E', "  ", (long) -1);
    drawGalx(L_RMAR1,L_LINE5,1.0, 'S', "  ", (long) -1);

    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_LTEXT,L_LINE6,"nebula", FALSE);
    drawNebu(L_LMAR2,L_LINE6,1.0, 'D', "  ", (long) -1);
    drawNebu( L_LMAR1,L_LINE6,1.0, 'P', "  ", (long) -1);

    D_fontsize(NAMESIZE, NAMEFNT);
    D_text(L_RTEXT,L_LINE6,"cluster", FALSE);
    drawClus(L_RMAR2,L_LINE6,1.0, 'O', "  ", (long) -1);
    drawClus(L_RMAR1,L_LINE6,1.0, 'G', "  ", (long) -1);
  } else {
    D_fontsize(NAMESIZE, NAMEFNT);

    sprintf(outstr, "%s: %s,%s lim: %2.1f", title, ras, dls, win->maglim);
    D_text(15, 15, outstr, FALSE);
  }
}
