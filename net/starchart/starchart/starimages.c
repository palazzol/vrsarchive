/*
** This file defines the star and planet images for vector type devices.
** All vector formed images are here (for tektronix, hp series and laserjet)
**
** Original images by Alan W. Paeth, dynamic 'star compilation' and
** cluster/galaxy images added by (pl) 15-Mar-87
**
** New images added by Steve Kennedy 31-Dec-87 and 1989
**
** Modified for 3.0 by Craig Counterman Jan, 1989
 *
 * Copyright (c) 1990 by Steve Kennedy and Craig Counterman.
 * All rights reserved.
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

static char rcsid[]="$Header: /home/Vince/cvs/net/starchart/starchart/starimages.c,v 1.1 1990-03-30 16:39:31 vrs Exp $";

#include <stdio.h>
#include <math.h>
#ifndef SYSV
#include <strings.h>
#else
#include <string.h>
#endif

#include "star3.h"


#define IMAROWS		20
#define IMACOLS		20		/* both should be more than plenty */
#define ENDCOORD	32767		/* should be out of offset range */
#define MIN_MAG		(-1.5)		/* all magnitude below become this */
#define MAX_MAG		(10.0)		/* all magnitude above become this */

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


/* Defined in lower level routines */

/* Point sizes for font calls */
extern int titlesize;
extern int subtlsize;
extern int namesize;
extern int lblsize;
extern int magsize;

/* Fonts for font calls */
extern int namefnt;
extern int lblfnt;
extern int magfnt;
extern int titlefnt;
extern int subtlfnt;

/* externs for labels */
extern int x_nameoffset, y_nameoffset;
extern int x_lbloffset, y_lbloffset;
extern int x_magoffset, y_magoffset;

/* externs for legend: variables of positioning are here */
extern int l_til;
extern int l_stil;

extern int l_lmar1;
extern int l_lmar2;
extern int l_ltext;
extern int l_rmar1;
extern int l_rmar2;
extern int l_rtext;

extern int l_line1;
extern int l_line2;
extern int l_line3;
extern int l_line4;
extern int l_line5;
extern int l_line6;


/*** begin star definition ***/

/*
 * Star Images
 */

char	*S1[] = {
    "*",
    NULL
    };  /* area = 1 */

/* inner circle, r = 1 */
char	*S2[] = {
    "**",
    "**",
    NULL
    };	/* area = 4 */

/* outer circle, r = 1 */
char	*S3[] = {
    " * ",
    "***",
    " * ",
    NULL
    };	/* area = 5 */

char	*S3a[] = {
    "***",
    "***",
    "***",
    NULL
    };	/* area = 9 */

/* inner circle, r = 2 */
char	*S4[] = {
    " ** ",
    "****",
    "****",
    " ** ",
    NULL
    };	/* area = 12 */

char	*S4a[] = {
    "  *  ",
    " *** ",
    "*****",
    " *** ",
    "  *  ",
    NULL
    };	/* area = 13 */

/* outer circle, r = 2 */
char	*S5[] = {
    " *** ",
    "*****",
    "*****",
    "*****",
    " *** ",
    NULL
    };	/* area = 21 */

/* inner circle, r = 3 */
char	*S6[] = {
    " **** ",
    "******",
    "******",
    "******",
    "******",
    " **** ",
    NULL
    };	/* area = 32 */

/* outer circle, r = 3 */
char	*S7[] = {
    "  ***  ",
    " ***** ",
    "*******",
    "*******",
    "*******",
    " ***** ",
    "  ***  ",
    NULL
    };	/* area = 37 */

/* inner circle, r = 4 */
char	*S8[] = {
    "  ****  ",
    " ****** ",
    "********",
    "********",
    "********",
    "********",
    " ****** ",
    "  ****  ",
    NULL
    };	/* area = 52 */

/* outer circle, r = 4 */
char	*S9[] = {
    "   ***   ",
    " ******* ",
    " ******* ",
    "*********",
    "*********",
    "*********",
    " ******* ",
    " ******* ",
    "   ***   ",
    NULL
    };	/* area = 61 */

/* inner circle, r = 5 */
char	*S10[] = {
    "   ****   ",
    "  ******  ",
    " ******** ",
    "**********",
    "**********",
    "**********",
    "**********",
    " ******** ",
    "  ******  ",
    "   ****   ",
    NULL
    };	/* area = 76 */

/* outer circle, r = 5 */
char	*S11[] = {
    "   *****   ",
    "  *******  ",
    " ********* ",
    "***********",
    "***********",
    "***********",
    "***********",
    "***********",
    " ********* ",
    "  *******  ",
    "   *****   ",
    NULL
    };	/* area = 97 */

/* inner circle, r = 6 */
char	*S12[] = {
    "    ****    ",
    "  ********  ",
    " ********** ",
    " ********** ",
    "************",
    "************",
    "************",
    "************",
    " ********** ",
    " ********** ",
    "  ********  ",
    "    ****    ",
    NULL
    };	/* area = 112 */

/* outer circle, r = 6 */
char	*S13[] = {
    "    *****    ",
    "  *********  ",
    " *********** ",
    " *********** ",
    "*************",
    "*************",
    "*************",
    "*************",
    "*************",
    " *********** ",
    " *********** ",
    "  *********  ",
    "    *****    ",
    NULL
    };	/* area = 129 */

/* inner circle, r = 7 */
char	*S14[] = {
    "    ******    ",
    "   ********   ",
    "  **********  ",
    " ************ ",
    "**************",
    "**************",
    "**************",
    "**************",
    "**************",
    "**************",
    " ************ ",
    "  **********  ",
    "   ********   ",
    "    ******    ",
    NULL
    };	/* area = 156 */

/* outer circle, r = 7 */
char	*S15[] = {
    "     *****     ",
    "   *********   ",
    "  ***********  ",
    " ************* ",
    " ************* ",
    "***************",
    "***************",
    "***************",
    "***************",
    "***************",
    " ************* ",
    " ************* ",
    "  ***********  ",
    "   *********   ",
    "     *****     ",
    NULL
    };	/* area = 177 */

/* inner circle, r = 8 */
char	*S16[] = {
    "     ******     ",
    "   **********   ",
    "  ************  ",
    " ************** ",
    " ************** ",
    "****************",
    "****************",
    "****************",
    "****************",
    "****************",
    "****************",
    " ************** ",
    " ************** ",
    "  ************  ",
    "   **********   ",
    "     ******     ",
    NULL
    };	/* area = 208 */

/* outer circle, r = 8 */
char	*S17[] = {
    "      *****      ",
    "    *********    ",
    "   ***********   ",
    "  *************  ",
    " *************** ",
    " *************** ",
    "*****************",
    "*****************",
    "*****************",
    "*****************",
    "*****************",
    " *************** ",
    " *************** ",
    "  *************  ",
    "   ***********   ",
    "    *********    ",
    "      *****      ",
    NULL
    };	/* area = 221 */

/* inner circle, r = 9 */
char	*S18[] = {
    "      ******      ",
    "    **********    ",
    "   ************   ",
    "  **************  ",
    " **************** ",
    " **************** ",
    "******************",
    "******************",
    "******************",
    "******************",
    "******************",
    "******************",
    " **************** ",
    " **************** ",
    "  **************  ",
    "   ************   ",
    "    **********    ",
    "      ******      ",
    NULL
    };	/* area = 256 */

/* outer circle, r = 9 */
char	*S19[] = {
    "       *****       ",
    "     *********     ",
    "   *************   ",
    "  ***************  ",
    "  ***************  ",
    " ***************** ",
    " ***************** ",
    "*******************",
    "*******************",
    "*******************",
    "*******************",
    "*******************",
    " ***************** ",
    " ***************** ",
    "  ***************  ",
    "  ***************  ",
    "   *************   ",
    "     *********     ",
    "       *****       ",
    NULL
    };	/* area = 277 */

/* inner circle, r = 10 */
char	*S20[] = {
    "       ******       ",
    "     **********     ",
    "    ************    ",
    "   **************   ",
    "  ****************  ",
    " ****************** ",
    " ****************** ",
    "********************",
    "********************",
    "********************",
    "********************",
    "********************",
    "********************",
    " ****************** ",
    " ****************** ",
    "  ****************  ",
    "   **************   ",
    "    ************    ",
    "     **********     ",
    "       ******       ",
    NULL
    };	/* area = 308 */

/* outer circle, r = 10 */
char	*S21[] = {
    "       *******       ",
    "     ***********     ",
    "    *************    ",
    "   ***************   ",
    "  *****************  ",
    " ******************* ",
    " ******************* ",
    "*********************",
    "*********************",
    "*********************",
    "*********************",
    "*********************",
    "*********************",
    "*********************",
    " ******************* ",
    " ******************* ",
    "  *****************  ",
    "   ***************   ",
    "    *************    ",
    "     ***********     ",
    "       *******       ",
    NULL
    };	/* area = 349 */

/* inner circle, r = 11 */
char	*S22[] = {
    "        ******        ",
    "      **********      ",
    "    **************    ",
    "   ****************   ",
    "  ******************  ",
    "  ******************  ",
    " ******************** ",
    " ******************** ",
    "**********************",
    "**********************",
    "**********************",
    "**********************",
    "**********************",
    "**********************",
    " ******************** ",
    " ******************** ",
    "  ******************  ",
    "  ******************  ",
    "   ****************   ",
    "    **************    ",
    "      **********      ",
    "        ******        ",
    NULL
    };	/* area = 376 */

/* outer circle, r = 11 */
char	*S23[] = {
    "        *******        ",
    "      ***********      ",
    "     *************     ",
    "   *****************   ",
    "   *****************   ",
    "  *******************  ",
    " ********************* ",
    " ********************* ",
    "***********************",
    "***********************",
    "***********************",
    "***********************",
    "***********************",
    "***********************",
    "***********************",
    " ********************* ",
    " ********************* ",
    "  *******************  ",
    "   *****************   ",
    "   *****************   ",
    "     *************     ",
    "      ***********      ",
    "        *******        ",
    NULL
    };	/* area = 413 */

/* inner circle, r = 12 */
char	*S24[] = {
    "         ******         ",
    "      ************      ",
    "     **************     ",
    "    ****************    ",
    "   ******************   ",
    "  ********************  ",
    " ********************** ",
    " ********************** ",
    " ********************** ",
    "************************",
    "************************",
    "************************",
    "************************",
    "************************",
    "************************",
    " ********************** ",
    " ********************** ",
    " ********************** ",
    "  ********************  ",
    "   ******************   ",
    "    ****************    ",
    "     **************     ",
    "      ************      ",
    "         ******         ",
    NULL
    };	/* area = 448 */

/* outer circle, r = 12 */
char	*S25[] = {
    "         *******         ",
    "       ***********       ",
    "     ***************     ",
    "    *****************    ",
    "   *******************   ",
    "  *********************  ",
    "  *********************  ",
    " *********************** ",
    " *********************** ",
    "*************************",
    "*************************",
    "*************************",
    "*************************",
    "*************************",
    "*************************",
    "*************************",
    " *********************** ",
    " *********************** ",
    "  *********************  ",
    "  *********************  ",
    "   *******************   ",
    "    *****************    ",
    "     ***************     ",
    "       ***********       ",
    "         *******         ",
    NULL
    };	/* area = 489 */

/* inner circle, r = 13 */
char	*S26[] = {
    "         ********         ",
    "       ************       ",
    "     ****************     ",
    "    ******************    ",
    "   ********************   ",
    "  **********************  ",
    "  **********************  ",
    " ************************ ",
    " ************************ ",
    "**************************",
    "**************************",
    "**************************",
    "**************************",
    "**************************",
    "**************************",
    "**************************",
    "**************************",
    " ************************ ",
    " ************************ ",
    "  **********************  ",
    "  **********************  ",
    "   ********************   ",
    "    ******************    ",
    "     ****************     ",
    "       ************       ",
    "         ********         ",
    NULL
    };	/* area = 540 */

/* outer circle, r = 13 */
char	*S27[] = {
    "          *******          ",
    "       *************       ",
    "      ***************      ",
    "     *****************     ",
    "    *******************    ",
    "   *********************   ",
    "  ***********************  ",
    " ************************* ",
    " ************************* ",
    " ************************* ",
    "***************************",
    "***************************",
    "***************************",
    "***************************",
    "***************************",
    "***************************",
    "***************************",
    " ************************* ",
    " ************************* ",
    " ************************* ",
    "  ***********************  ",
    "   *********************   ",
    "    *******************    ",
    "     *****************     ",
    "      ***************      ",
    "       *************       ",
    "          *******          ",
    NULL
    };	/* area = 569 */

/* inner circle, r = 14 */
char	*S28[] = {
    "          ********          ",
    "        ************        ",
    "      ****************      ",
    "     ******************     ",
    "    ********************    ",
    "   **********************   ",
    "  ************************  ",
    "  ************************  ",
    " ************************** ",
    " ************************** ",
    "****************************",
    "****************************",
    "****************************",
    "****************************",
    "****************************",
    "****************************",
    "****************************",
    "****************************",
    " ************************** ",
    " ************************** ",
    "  ************************  ",
    "  ************************  ",
    "   **********************   ",
    "    ********************    ",
    "     ******************     ",
    "      ****************      ",
    "        ************        ",
    "          ********          ",
    NULL
    };	/* area = 616 */

/* outer circle, r = 14 */
char	*S29[] = {
    "           *******           ",
    "        *************        ",
    "       ***************       ",
    "     *******************     ",
    "    *********************    ",
    "   ***********************   ",
    "   ***********************   ",
    "  *************************  ",
    " *************************** ",
    " *************************** ",
    " *************************** ",
    "*****************************",
    "*****************************",
    "*****************************",
    "*****************************",
    "*****************************",
    "*****************************",
    "*****************************",
    " *************************** ",
    " *************************** ",
    " *************************** ",
    "  *************************  ",
    "   ***********************   ",
    "   ***********************   ",
    "    *********************    ",
    "     *******************     ",
    "       ***************       ",
    "        *************        ",
    "           *******           ",
    NULL
    };	/* area = 657 */

/*
 * list of images in order of decreasing size
 */
char	**star_image_tab[] = {
    S1, S2, S3, S3a, S4, S4a, S5, S6, S7, S8, S9, S10,
    S11, S12, S13, S14, S15, S16, S17, S18, S19, S20,
    S21, S22, S23, S24, S25, S26, S27, S28, S29
    };

/*
 * pointer to "compiled" version of each image
 */
#define N_IMAGES	(sizeof star_image_tab /sizeof (char **))
int	*star_def_tab[N_IMAGES];

/*
 * table which converts from star magnitude to an index into star_def_tab.
 *
 * table entry 0 corresponds to magnitude -1.5 and so on up to
 * magnitude 10.0 increments of 0.1 magnitude.
 */
#ifdef BIG_STARS
short star_mag_to_image[] = {
	30, /* -1.5 */
	30, /* -1.4 */
	30, /* -1.3 */
	30, /* -1.2 */
	30, /* -1.1 */
	30, /* -1.0 */
	30, /* -0.9 */
	30, /* -0.8 */
	29, /* -0.7 */
	28, /* -0.6 */
	27, /* -0.5 */
	26, /* -0.4 */
	26, /* -0.3 */
	25, /* -0.2 */
	24, /* -0.1 */
	23, /* 0.0 */
	22, /* 0.1 */
	22, /* 0.2 */
	21, /* 0.3 */
	20, /* 0.4 */
	20, /* 0.5 */
	19, /* 0.6 */
	18, /* 0.7 */
	18, /* 0.8 */
	17, /* 0.9 */
	16, /* 1.0 */
	16, /* 1.1 */
	16, /* 1.2 */
	15, /* 1.3 */
	15, /* 1.4 */
	14, /* 1.5 */
	14, /* 1.6 */
	13, /* 1.7 */
	13, /* 1.8 */
	12, /* 1.9 */
	12, /* 2.0 */
	12, /* 2.1 */
	11, /* 2.2 */
	11, /* 2.3 */
	11, /* 2.4 */
	10, /* 2.5 */
	10, /* 2.6 */
	10, /* 2.7 */
	9, /* 2.8 */
	9, /* 2.9 */
	9, /* 3.0 */
	9, /* 3.1 */
	8, /* 3.2 */
	8, /* 3.3 */
	8, /* 3.4 */
	7, /* 3.5 */
	7, /* 3.6 */
	7, /* 3.7 */
	7, /* 3.8 */
	6, /* 3.9 */
	6, /* 4.0 */
	6, /* 4.1 */
	6, /* 4.2 */
	6, /* 4.3 */
	6, /* 4.4 */
	6, /* 4.5 */
	5, /* 4.6 */
	5, /* 4.7 */
	5, /* 4.8 */
	5, /* 4.9 */
	4, /* 5.0 */
	4, /* 5.1 */
	3, /* 5.2 */
	3, /* 5.3 */
	3, /* 5.4 */
	3, /* 5.5 */
	3, /* 5.6 */
	3, /* 5.7 */
	3, /* 5.8 */
	2, /* 5.9 */
	2, /* 6.0 */
	2, /* 6.1 */
	2, /* 6.2 */
	2, /* 6.3 */
	2, /* 6.4 */
	1, /* 6.5 */
	1, /* 6.6 */
	1, /* 6.7 */
	1, /* 6.8 */
	1, /* 6.9 */
	1, /* 7.0 */
	1, /* 7.1 */
	1, /* 7.2 */
	0, /* 7.3 */
	0, /* 7.4 */
	0, /* 7.5 */
	0, /* 7.6 */
	0, /* 7.7 */
	0, /* 7.8 */
	0, /* 7.9 */
	0, /* 8.0 */
	0, /* 8.1 */
	0, /* 8.2 */
	0, /* 8.3 */
	0, /* 8.4 */
	0, /* 8.5 */
	0, /* 8.6 */
	0, /* 8.7 */
	0, /* 8.8 */
	0, /* 8.9 */
	0, /* 9.0 */
	0, /* 9.1 */
	0, /* 9.2 */
	0, /* 9.3 */
	0, /* 9.4 */
	0, /* 9.5 */
	0, /* 9.6 */
	0, /* 9.7 */
	0, /* 9.8 */
	0, /* 9.9 */
	0, /* 10.0 */
};
#else
short star_mag_to_image[] = {
	8, /* -1.5 */
	8, /* -1.4 */
	8, /* -1.3 */
	8, /* -1.2 */
	8, /* -1.1 */
	8, /* -1.0 */
	8, /* -0.9 */
	8, /* -0.8 */
	8, /* -0.7 */
	8, /* -0.6 */
	8, /* -0.5 */
	8, /* -0.4 */
	8, /* -0.3 */
	8, /* -0.2 */
	8, /* -0.1 */
	8, /* 0.0 */
	8, /* 0.1 */
	8, /* 0.2 */
	8, /* 0.3 */
	8, /* 0.4 */
	7, /* 0.5 */
	7, /* 0.6 */
	7, /* 0.7 */
	7, /* 0.8 */
	7, /* 0.9 */
	7, /* 1.0 */
	7, /* 1.1 */
	7, /* 1.2 */
	7, /* 1.3 */
	7, /* 1.4 */
	6, /* 1.5 */
	6, /* 1.6 */
	6, /* 1.7 */
	6, /* 1.8 */
	6, /* 1.9 */
	6, /* 2.0 */
	6, /* 2.1 */
	6, /* 2.2 */
	6, /* 2.3 */
	6, /* 2.4 */
	5, /* 2.5 */
	5, /* 2.6 */
	5, /* 2.7 */
	5, /* 2.8 */
	5, /* 2.9 */
	5, /* 3.0 */
	5, /* 3.1 */
	5, /* 3.2 */
	5, /* 3.3 */
	5, /* 3.4 */
	4, /* 3.5 */
	4, /* 3.6 */
	4, /* 3.7 */
	4, /* 3.8 */
	4, /* 3.9 */
	4, /* 4.0 */
	4, /* 4.1 */
	4, /* 4.2 */
	4, /* 4.3 */
	4, /* 4.4 */
	3, /* 4.5 */
	3, /* 4.6 */
	3, /* 4.7 */
	3, /* 4.8 */
	3, /* 4.9 */
	3, /* 5.0 */
	3, /* 5.1 */
	3, /* 5.2 */
	3, /* 5.3 */
	3, /* 5.4 */
	2, /* 5.5 */
	2, /* 5.6 */
	2, /* 5.7 */
	2, /* 5.8 */
	2, /* 5.9 */
	2, /* 6.0 */
	2, /* 6.1 */
	2, /* 6.2 */
	2, /* 6.3 */
	2, /* 6.4 */
	1, /* 6.5 */
	1, /* 6.6 */
	1, /* 6.7 */
	1, /* 6.8 */
	1, /* 6.9 */
	1, /* 7.0 */
	1, /* 7.1 */
	1, /* 7.2 */
	1, /* 7.3 */
	1, /* 7.4 */
	0, /* 7.5 */
	0, /* 7.6 */
	0, /* 7.7 */
	0, /* 7.8 */
	0, /* 7.9 */
	0, /* 8.0 */
	0, /* 8.1 */
	0, /* 8.2 */
	0, /* 8.3 */
	0, /* 8.4 */
	0, /* 8.5 */
	0, /* 8.6 */
	0, /* 8.7 */
	0, /* 8.8 */
	0, /* 8.9 */
	0, /* 9.0 */
	0, /* 9.1 */
	0, /* 9.2 */
	0, /* 9.3 */
	0, /* 9.4 */
	0, /* 9.5 */
	0, /* 9.6 */
	0, /* 9.7 */
	0, /* 9.8 */
	0, /* 9.9 */
	0, /* 10.0 */
};
#endif

/*** end of star definition ***/

/* Planetary images */

char	*Sun[] = {
    "         ",
    "         ",
    "   ***   ",
    "  *   *  ",
    "  * * *  ",
    "  *   *  ",
    "   ***   ",
    "         ",
    "         ",
    NULL
    };

char	*Luna[] = {
    "  **     ",
    "   **    ",
    "    **   ",
    "    * *  ",
    "    * *  ",
    "    * *  ",
    "    **   ",
    "   **    ",
    "  **     ",
    NULL
    };

char	*Mercury[] = {
    "  *   *  ",
    "   ***   ",
    "  *   *  ",
    "  *   *  ",
    "  *   *  ",
    "   ***   ",
    "    *    ",
    "   ***   ",
    "    *    ",
    NULL
    };

char	*Venus[] = {
    "         ",
    "   ***   ",
    "  *   *  ",
    "  *   *  ",
    "  *   *  ",
    "   ***   ",
    "    *    ",
    "   ***   ",
    "    *    ",
    NULL
    };

char	*Mars[] = {
    "         ",
    "    ***  ",
    "     **  ",
    "    * *  ",
    "    *    ",
    "   ***   ",
    "  *   *  ",
    "  *   *  ",
    "   ***   ",
    NULL
    };

char	*Jupiter[] = {
    "   *     ",
    "  * * *  ",
    "    * *  ",
    "    * *  ",
    "   *  *  ",
    "   *  *  ",
    "   ****  ",
    "      *  ",
    "         ",
    NULL
    };

char	*Saturn[] = {
    "         ",
    "  ***    ",
    "   *     ",
    "   *     ",
    "   ***   ",
    "   *  *  ",
    "   * *   ",
    "   * *   ",
    "      *  ",
    NULL
    };

char	*Uranus[] = {
    "    *    ",
    "   ***   ",
    "  * * *  ",
    "    *    ",
    "   ***   ",
    "  *   *  ",
    "  * * *  ",
    "  *   *  ",
    "   ***   ",
    NULL
    };

char	*Neptune[] = {
    "  * * *  ",
    "  * * *  ",
    "  * * *  ",
    "  * * *  ",
    "   ***   ",
    "    *    ",
    "   ***   ",
    "    *    ",
    "         ",
    NULL
    };

char	*Pluto[] = {
    "         ",
    "  ****   ",
    "  *   *  ",
    "  *   *  ",
    "  ****   ",
    "  *      ",
    "  *****  ",
    "         ",
    NULL
    };

char	*Comet[] = {
    "      * *",
    "     * * ",
    "  ***    ",
    " **** * *",
    " ***** * ",
    "  ***    ",
    "     * * ",
    "      * *",
    NULL
    };

char	*Aster[] = {
    "*       *",
    " *     * ",
    "  * * *  ",
    "   ***   ",
    "   ***   ",
    "  * * *  ",
    " *     * ",
    "*       *",
    NULL
    };

/* Galaxy images */

char	*Gal_E[] = {
    "***   ",
    "****  ",
    "***** ",
    " *****",
    "  ****",
    "   ***",
    NULL
    };

char	*Gal_S[] = {
    " ***  ",
    "*   * ",
    "  **  ",
    " **** ",
    "  **  ",
    " *   *",
    "  *** ",
    NULL
    };

/* Cluster images */

char	*Clu_G[] = {
    "   *   ",
    " *   * ",
    "   *   ",
    "* *** *",
    "   *   ",
    " *   * ",
    "   *   ",
    NULL
    };

char	*Clu_O[] = {
    "   *   ",
    " *   * ",
    "       ",
    "*     *",
    "       ",
    " *   * ",
    "   *   ",
    NULL
    };

/* Nebula images */

char	*Neb_D[] = {
    "  *  ",
    " * * ",
    "*   *",
    " * * ",
    "  *  ",
    NULL
    };

char	*Neb_P[] = {
    "   ***",
    "  *  *",
    " *   *",
    "*   * ",
    "*  *  ",
    "***   ",
    NULL
    };

/* Other images */

char	*Unk_U[] = {
    " *** ",
    "*   *",
    "   **",
    "  *  ",
    "  *  ",
    "     ",
    "  *  ",
    NULL
    };

char	*Other_O[] = {
    "* * * ",
    " * * *",
    "* * * ",
    " * * *",
    "* * * ",
    " * * *",
    NULL
    };

char *calloc();

/**********************************************************************
**
** cimage:
**
** Parameters:
**	image		char ** pointer to ascii reprentation of images
**
** Function:
**	'compiles' star image data for drawlen from char pointers given
**	this method simplifies the creation and updating of images.
**	The lengths of rows of images MUST be equal !!
**
** Returns:
**	NULL when ptr != NULL, pointer to allocated vector defining
**	drawlen information and ending to ENDCOORD value when ptr == NULL
**
** History:
**
**	Modified from cstar by Steve Kennedy 3/1/89
** 
*/
int *
cimage(image)
char **image;
{
	int n;				/* number of lines gathered */
	int len;			/* longest line found */
	int cx, cy;			/* center points */
	char *ptr;
	char **rptr;
	int *retarea;
	int i,j, x, s, l;
	

	n = 0;
	len = 0;
	for (rptr = image; *rptr != NULL; ++rptr, ++n)
	{
	    cx = strlen (*rptr);		/* get max len */
	    if  (cx > len) len = cx;
	}

	/*
	** start 'compilation'
	*/
	cy = n / 2;				/* align */
	cx = len / 2;

	/*
	** allocate offset area
	*/
	retarea = (int *) calloc 
		((unsigned)(len * n * (n / 2 + 1) * 3 + 1), sizeof (int));
	if  (retarea == NULL) {
		perror ("Memory allocation failed");
		exit (2);
	}
	/*
	** fill return area
	*/
	j = 0;
	for (i = 0; i < n; i++) {
		l = 0;  s = 0;  x = 0;
		ptr = image [i];
		while (*ptr) {
			if  (*ptr == ' ') {
				if  (l) { /* flush */
					retarea [j++] = s - cx;
/*					retarea [j++] = i - cy;*/
					retarea [j++] = cy - i;
					retarea [j++] = l;
							
					l = 0;
				}
			}
			else {
				if  (l)
					l++;
				else {
					s = x;
					l = 1;
				}
			}
			ptr++;
			x++;
		} /* while */
		if  (l) { /* flush */
			retarea [j++] = s - cx;
/*			retarea [j++] = i - cy;*/
			retarea [j++] = cy - i;
			retarea [j++] = l;
		}
	} /* for */
	n = 0;
	retarea [j] = ENDCOORD;
	return retarea;
} /* cimage */


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
      D_fontsize(namesize, namefnt);
      D_text(x+x_nameoffset, y+y_nameoffset, obj_name, FALSE);
    } else if (use_lbl &&
	       ((label_field[0] != ' ') || (label_field[1] != ' '))) {
      D_fontsize(lblsize, lblfnt);
      D_text(x+x_lbloffset, y+y_lbloffset, label_field, TRUE);
    }
/* If you want to mag label other objects, change this */
    if (use_mag && (type[0] == 'S')) {
      sprintf(magstr, "%02d", (int)(mag*10.0+0.5));
      D_fontsize(magsize, magfnt);
      D_text(x+x_magoffset, y+y_magoffset, magstr, FALSE);
    }
  }
}

drawStar (x, y, mag, type, color)
     int x, y;
     double mag;
     char type, *color;
{
    int		*coord;
    int		i;
    int		i_mag;
    int		maxlen;


    if (mag < MIN_MAG)
    {
	mag = MIN_MAG;
    }
    else if (mag > MAX_MAG)
    {
	mag = MAX_MAG;
    }
    mag += 1.5;

    i_mag = star_mag_to_image[(int) (mag*10.0 + 0.5)];

    D_color(color);
    switch (type)
	{
    case 'S':
    default:
	if (star_def_tab[i_mag] == NULL)
	{
	    star_def_tab[i_mag] = cimage(star_image_tab[i_mag]);
	}
	coord = star_def_tab[i_mag];
	break;
	}

    i = 0;
    maxlen = 0;
    while (coord [i] != ENDCOORD)
    {
	drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
#ifdef NOTYET
	if (type == 'D' && coord [i+2] > maxlen)
	{
	    maxlen = coord [i + 2];
	    dx = coord [i];
	}
#endif
	i += 3;
    }
#ifdef NOTYET
    if (type == 'D' && maxlen >= 5)
    {
	fprintf(stderr, "(%d,%d) dx=%d, dy=%d, len=%d\n",
	    x, y, dx, 0, maxlen);
	drawlen(x, y, dx - 2, 0, maxlen + 4);
    }
#endif

    D_color(" ");
}


drawPlan(x, y, mag, pcode, color, plansize, comment_str)
     int x,y;
     double mag;
     char pcode, *color;
     long plansize;
     char *comment_str;
{
  static int def[128], *coord[128];
#define MOONR	5
#define DTR(x)	((x)*.0174532925199)
  static char	moon[2*MOONR+1][2*MOONR+2];
  int	*tcoord;
  int i;

    D_color(color);
  if (!def[pcode]) {
    def[pcode] = 1;
    switch (pcode) {
    case 'S':
      tcoord = cimage(Sun);
      break;
    case 'L':
      tcoord = cimage(Luna);
      break;
    case 'M':
      tcoord = cimage(Mercury);
      break;
    case 'V':
      tcoord = cimage(Venus);
      break;
    case 'm':
      tcoord = cimage(Mars);
      break;
    case 'J':
      tcoord = cimage(Jupiter);
      break;
    case 's':
      tcoord = cimage(Saturn);
      break;
    case 'U':
      tcoord = cimage(Uranus);
      break;
    case 'N':
      tcoord = cimage(Neptune);
      break;
    case 'P':
      tcoord = cimage(Pluto);
      break;
    case 'C':
      tcoord = cimage(Comet);
      break;
    case 'A':
      tcoord = cimage(Aster);
      break;
    default:
      tcoord = cimage(Unk_U);
      break;
    }
    coord[pcode] = tcoord;
  }

  i = 0;
  while (coord[pcode][i] != ENDCOORD) {
    drawlen (x, y, coord[pcode][i], coord[pcode][i+1],
	     coord[pcode][i+2]);
    i += 3;
  }
    D_color(" ");
}


drawGalx(x, y, mag, type, color, nebsize)
int x, y;
double mag;
char type, *color;
long nebsize;		/* -1 should give default size */
{
	static int defSpiral, *coordSpiral;
	static int defEll, *coordEll;
	int	*coord;
	int i;

    D_color(color);
	switch (type)
	    {
	case 'S':
	default:
	    if  (!defSpiral) {
		defSpiral = 1;
		coordSpiral = cimage(Gal_S);
		}
	    coord = coordSpiral;
	    break;
	case 'E':
	    if (!defEll) {
		defEll = 1;
		coordEll = cimage(Gal_E);
		}
	    coord = coordEll;
	    break;
	    }

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
    D_color(" ");
}

drawClus(x, y, mag, type, color, nebsize)
int x, y;
double mag;
char type, *color;
long nebsize;		/* -1 should give default size */
{
	static int defGlob, *coordGlob;
	static int defOpen, *coordOpen;
	int	*coord;
	int i;

    D_color(color);
	switch (type)
	    {
	case 'G':
	    if (!defGlob) {
		defGlob = 1;
		coordGlob = cimage(Clu_G);
		}
	    coord = coordGlob;
	    break;
	case 'O':
	default:
	    if  (! defOpen) {
		defOpen = 1;
		coordOpen = cimage(Clu_O);
		}
	    coord = coordOpen;
	    break;
	    }

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
    D_color(" ");
}

drawNebu(x, y, mag, type, color, nebsize)
int x, y;
double mag;
char type, *color;
long nebsize;		/* -1 should give default size */
{
	static int defDiff, *coordDiff;
	static int defPlan, *coordPlan;
	int	*coord;
	int i;

    D_color(color);
	switch (type)
	    {
	case 'P':
	    if  (!defPlan) {
		defPlan = 1;
		coordPlan = cimage(Neb_P);
		}
	    coord = coordPlan;
	    break;
	default:
	case 'D':
	    if (!defDiff) {
		defDiff = 1;
		coordDiff = cimage(Neb_D);
		}
	    coord = coordDiff;
	    break;
	    }

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
    D_color(" ");
}

drawUnknown(x, y, mag, type, color, nebsize)
int x, y;
double mag;
char type, *color;
long nebsize;		/* -1 should give default size */
{
	static int defUnk, *coordUnk;
	int	*coord;
	int i;

    D_color(color);
	switch (type)
	    {
	    case ' ':
	default:
	    if (!defUnk) {
		defUnk = 1;
		coordUnk = cimage(Unk_U);
		}
	    coord = coordUnk;
	    break;
	    }

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
    D_color(" ");
}

drawOther(x, y, mag, type, color, nebsize)
int x, y;
double mag;
char type, *color;
long nebsize;		/* -1 should give default size */
{
	static int defOthr_O, *coordOthr_O;
	int	*coord;
	int i;

    D_color(color);
	switch (type)
	    {
	    case ' ':
	default:
	    if (!defOthr_O) {
		defOthr_O = 1;
		coordOthr_O = cimage(Other_O);
		}
	    coord = coordOthr_O;
	    break;
	    }

	i = 0;
	while (coord [i] != ENDCOORD) {
		drawlen (x, y, coord [i], coord [i+1], coord [i+2]);
		i += 3;
	}
    D_color(" ");
}


#ifdef AREAS
/* Functions for areas, drawn as lines for now */
static struct {int x,y;} areapts[1000];
static int nareapts;
/* Move to (x, y) to begin an area */
D_areamove(x, y)
     int x, y;
{
  nareapts = 0;
  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;
}

/* Add a segment to the area border */
D_areaadd(x, y)
     int x, y;
{
  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;
}

/* Fill the area, after adding the last segment */
D_areafill(x, y)
     int x, y;
{
  int i;

  areapts[nareapts].x = x;
  areapts[nareapts].y = y;
  nareapts++;


  D_move(areapts[0].x, areapts[0].y);

  for (i = 1; i < nareapts; i++)
    D_draw(areapts[i].x, areapts[i].y);
}
#endif




/* Note externs which are used */

chartlegend(win)
     mapwindow *win;
{
  char ras[20], dls[20], outstr[40];
  if (!title[0]) title = "LEGEND";
  rastr(ras, win->racen);
  declstr(dls, win->dlcen);

  if (win->map_type != FULLPAGEMAP) {
    sprintf(outstr, "(%s,%s lim: %2.1f)", ras, dls, win->maglim);
    D_fontsize(titlesize, titlefnt); D_text(l_lmar1, l_til, title, FALSE);
    D_fontsize(subtlsize, subtlfnt); D_text(l_lmar1, l_stil, outstr, FALSE);
    
    drawStar(l_lmar2, l_line1, 0.0, 'S', "  ");
    D_fontsize(namesize, namefnt);
    D_text(l_ltext, l_line1,"<0.5", FALSE);
    if (win->maglim >= 0.5)
      {
	drawStar(l_rmar2, l_line1, 1.0, 'S', "  ");
	D_fontsize(namesize, namefnt);
	D_text( l_rtext, l_line1,"<1.5", FALSE);
      }
    if (win->maglim >= 1.5)
      {
	drawStar(l_lmar2, l_line2, 2.0, 'S', "  ");
	D_fontsize(namesize, namefnt);
	D_text(l_ltext, l_line2,"<2.5", FALSE);
      }
    if (win->maglim >= 2.5)
      {
	drawStar(l_rmar2, l_line2, 3.0, 'S', "  ");
	D_fontsize(namesize, namefnt);
	D_text(l_rtext, l_line2,"<3.5", FALSE);
      }
    if (win->maglim >= 3.5)
      {
	drawStar(l_lmar2, l_line3, 4.0, 'S', "  ");
	D_fontsize(namesize, namefnt);
	D_text(l_ltext, l_line3,"<4.5", FALSE);
      }
    if (win->maglim > 4.5)
      {
	drawStar(l_rmar2, l_line3, 5.0, 'S', "  ");
	D_fontsize(namesize, namefnt);
	D_text(l_rtext, l_line3,">4.5", FALSE);
      }
    
    D_fontsize(namesize, namefnt);
    D_text(l_ltext,l_line4,"double", FALSE);
    drawStar(l_lmar2,l_line4,2.0, 'D', "  ");
    D_fontsize(namesize, namefnt);
    D_text(l_rtext,l_line4,"variable",FALSE);
    drawStar(l_rmar2,l_line4,2.0, 'V', "  ");

    D_fontsize(namesize, namefnt);
    D_text(l_ltext,l_line5,"planet", FALSE);
    drawPlan(l_lmar2,l_line5,1.0, ' ', "  ", (long) -1, "");

    D_fontsize(namesize, namefnt);
    D_text(l_rtext,l_line5,"galaxy", FALSE);
    drawGalx(l_rmar2,l_line5,1.0, 'E', "  ", (long) -1);
    drawGalx(l_rmar1,l_line5,1.0, 'S', "  ", (long) -1);

    D_fontsize(namesize, namefnt);
    D_text(l_ltext,l_line6,"nebula", FALSE);
    drawNebu(l_lmar2,l_line6,1.0, 'D', "  ", (long) -1);
    drawNebu( l_lmar1,l_line6,1.0, 'P', "  ", (long) -1);

    D_fontsize(namesize, namefnt);
    D_text(l_rtext,l_line6,"cluster", FALSE);
    drawClus(l_rmar2,l_line6,1.0, 'O', "  ", (long) -1);
    drawClus(l_rmar1,l_line6,1.0, 'G', "  ", (long) -1);
  } else {
    D_fontsize(namesize, namefnt);

    sprintf(outstr, "%s: %s,%s lim: %2.1f", title, ras, dls, win->maglim);
    D_text(15, 15, outstr, FALSE);
  }
}



