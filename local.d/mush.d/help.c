/* @(#)help.c	(c) copyright 10/15/86 (Dan Heller) */

/*
 * This file conatins two main routines:
 *    display_help() and find_help()
 * Both are virtually equivalent in functionality; they are passed
 * a char * or a char **. If a char * is passed, then open "file"
 * argument (containing help strings) and search for %str% and read
 * the following text until %% or EOF into a local buffer.
 * If str is a char **, then that array of strings is used directly.
 *
 * If display_help is used, then the final array of strings used is
 * displayed in the center of the bitmapped console in a box with
 * shading and the user must hit the left mouse button to remove the
 * message.  the fd passed is the fd of the window locking the screen.
 *
 * In text mode, the routine find_help is used (or, if the graphics
 * mode doesn't want to lock the screen to display a message). The
 * same actions occur, but instead of "ifdef"ing up one function, I
 * just made two for readability.
 */
#include <stdio.h>
#include "strings.h"

#define NULL_FILE (FILE *)0

#ifdef SUNTOOL
#include <suntool/tool_hs.h>
#include <signal.h>
#include <suntool/fullscreen.h>

#define l_width()	  font->pf_defaultsize.x /* width of letter */
#define l_height()	  font->pf_defaultsize.y /* height of letter */

#define draw(win,x1,y1,x2,y2,OP) 	pw_vector(win, x1,y1,x2,y2,OP,1)
#define box(win, x1,y1,x2,y2,OP) \
	draw(win,x1,y1, x1,y2, OP), \
	draw(win,x1,y2, x2,y2, OP), \
	draw(win,x2,y2, x2,y1, OP), \
	draw(win,x2,y1, x1,y1, OP)

#define DEF_CONT_MSG	"Click LEFT mouse Button to continue."
DEFINE_CURSOR(oldcursor, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

/* shading */
static short dat_shade_25[] = {
#include <images/square_25.pr>
};

static short dat_shade_50[] = {
#include <images/square_50.pr>
};

static short dat_shade_75[] = {
#include <images/square_75.pr>
};

static short dat_mouse_left[] = {
#include <images/confirm_left.pr> 
};

mpr_static(shade_25,        16, 16, 1, dat_shade_25);
mpr_static(shade_50,        16, 16, 1, dat_shade_50);
mpr_static(shade_75,        16, 16, 1, dat_shade_75);
mpr_static(confirm_pr,      16, 16, 1, dat_mouse_left);

static struct cursor confirm_cursor = { 3, 3, PIX_SRC, &confirm_pr };
static struct pixrect *shading;

#else

#include <sys/types.h>
#define wprint printf
#define print  printf

#endif /* SUNTOOL */

/* what to print if nothing was found */
static char *def_msg[] = {
    "There is no help found for this item.",
    "Perhaps getting help from another item",
    "would be of some use.", 0
};

#define MAXLINES	40
#define MAXLENGTH	128
static FILE *help_file;
static char file_name[128];

#ifdef SUNTOOL
display_help(fd, str, file, font)
register caddr_t *str;   /* could be a single or double pointer */
register char *file;
register struct pixfont *font;
{
    struct fullscreen *fs;
    struct inputmask im, old_im;
    struct inputevent event;
    struct rect rect;
    register char *p;
    register int x, y, z, height;
    struct pixrect *saved, *save_bits();
    int width = 0, old_link;
    char *cont_msg = DEF_CONT_MSG, *getenv();
    char args[MAXLINES][MAXLENGTH], help_str[40];

    if (!shading)
	if (p = getenv("SHADE")) {
	    register int x = atoi(p);
	    if (x <= 25)
		shading = &shade_25;
	    else if (x <= 50)
		shading = &shade_50;
	    else
		shading = &shade_75;
	} else
	    shading = &shade_25;

    /* If no file given, take "str" arg as message to print */
    if (!file || !*file) {
	file = NULL;
	for(height = 0; *str && height < MAXLINES-1; height++) {
	    if (!compose_str(*str, &width))
		break;
	    (void) strcpy(args[height], *str++);
	}
    } else {
	if (!*file_name || strcmp(file_name, file)) {
	    (void) strcpy(file_name, file);
	    if (help_file)
		fclose(help_file), help_file = NULL_FILE;
	}
	/* file remains open */
	if (!help_file && !(help_file = fopen(file_name, "r")))
	    return -1;
	/* look for %str% in helpfile */
	(void) sprintf(help_str, "%%%s%%\n", str);

	rewind(help_file);

	while(p = fgets(args[0], MAXLENGTH, help_file))
	    if (*p == '%' && !strcmp(p, help_str))
		break;

	if (!p || !fgets(args[0], MAXLENGTH, help_file))
	    for(height = 0; def_msg[height] && height < MAXLINES-1; height++) {
		compose_str(def_msg[height], &width);
		(void) strcpy(args[height], def_msg[height]);
	    }
	else for (height = 0; p && *p && strcmp(p, "%%\n"); height++) {
		compose_str(p, &width);
		p = fgets(args[height+1], MAXLENGTH, help_file);
	    }
    }
    if (height == MAXLINES - 1)
	print("Help message is too long!\n");

    fs = fullscreen_init(fd);
    /* Figure out the height and width in pixels (rect.r_height, rect.r_width)
     * Remember to provide for the "confirm" line (cont_msg).
     * extend the new box by 15 pixels on the sides (30 total), top, and bottom.
     * finally, add 16 pixels for "shadow" -- remove before clearing area
     * to preserve background and give a shadow-like appearance.
     */
    if ((x = strlen(cont_msg)) > width)
	width = x; /* this x value must be saved! */
    rect.r_width = (width*l_width()) + 30 + 16;
    rect.r_height = ((height+1) * l_height()) + 30 + 16;
    rect.r_left = fs->fs_screenrect.r_left +
	(fs->fs_screenrect.r_width / 2) - (rect.r_width / 2);
    rect.r_top = fs->fs_screenrect.r_top +
	(fs->fs_screenrect.r_height / 2) - (rect.r_height / 2);

    /* save old area */
    saved = save_bits(fs->fs_pixwin, &rect);

    /* prepare surface, clear the background, and reset rect for shadow */
    pw_preparesurface(fs->fs_pixwin, &rect);
    pw_lock(fs->fs_pixwin, &rect);
    rect.r_width -= 16;
    rect.r_height -= 16;

    pw_writebackground(fs->fs_pixwin,
	rect.r_left, rect.r_top, rect.r_width, rect.r_height, PIX_CLR);

    /* make a box that's 5 pixels thick. Then add a thin box inside it */
    for (z = 0; z < 5; z++)
	box(fs->fs_pixwin,
	    rect.r_left+z, rect.r_top+z,
	    rect.r_left+rect.r_width-z-1, rect.r_top+rect.r_height-z-1,
	    PIX_SRC);
    box(fs->fs_pixwin,
	rect.r_left+z+2, rect.r_top+z+2,
	rect.r_left+rect.r_width-z-3, rect.r_top+rect.r_height-z-3,
	PIX_SRC);

    /* shading -- pw_replrop() doesn't work (bug)
     * NOTE: fs->fs_screenrect.r_top and r_left are negative values
     */
    pr_replrop(fs->fs_pixwin->pw_pixrect,
	   rect.r_left + rect.r_width - fs->fs_screenrect.r_left,
	   rect.r_top + 16 - fs->fs_screenrect.r_top,
	   16, rect.r_height-16, PIX_SRC|PIX_DST, shading, 0, 0);
    pr_replrop(fs->fs_pixwin->pw_pixrect,
	   rect.r_left + 16 - fs->fs_screenrect.r_left,
	   rect.r_top+rect.r_height - fs->fs_screenrect.r_top,
	   rect.r_width, 16, PIX_SRC|PIX_DST, shading, 0, 0);

    if (x > width)
	x = 15;
    else
	x = rect.r_width/2 - (x*l_width())/2;
    y = rect.r_height - 15;

    /* Print everything in reverse order now; start with the "confirm" string
     * (which is centered and highlighted)
     */
    pw_text(fs->fs_pixwin, rect.r_left+x, rect.r_top+y, PIX_SRC,font, cont_msg);
    pw_text(fs->fs_pixwin, rect.r_left+x+1, rect.r_top+y, PIX_SRC|PIX_DST, font,
	    cont_msg);

    y -= (5 + l_height());
    x = 15;

    /* now print each string in reverse order (start at bottom of box) */
    for (height--; height >= 0; height--) {
	pw_text(fs->fs_pixwin, rect.r_left+x, rect.r_top+y,
	    PIX_SRC, font, args[height]);
	y -= l_height();
    }

    pw_unlock(fs->fs_pixwin);

    /* wait for user to read and confirm */
    win_getinputmask(fd, &old_im, &old_link);
    input_imnull(&im);
    im.im_flags |= IM_ASCII;
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_setinputmask(fd, &im, &im, WIN_NULLLINK);
    win_getcursor(fd, &oldcursor);
    win_setcursor(fd, &confirm_cursor);
    while (input_readevent(fd, &event) != -1 && event.ie_code != MS_LEFT);

    /* restore old cursor */
    win_setcursor(fd, &oldcursor);

    /* restore old pixrect (size already restored) */
    rect.r_height += 16; /* to take care of the shadow */
    rect.r_width += 16;
    restore_bits(fs->fs_pixwin, &rect, saved);
    /* release screen */
    fullscreen_destroy(fs);
    win_setinputmask(fd, &old_im, &old_im, old_link);
    return 0;
}

static
compose_str(p, width)
register char *p;
int *width;
{
    register int x;
    if (!p || !*p)
	return 0;
    x = strlen(p);
    if (p[x-1] == '\n')
	p[--x] = 0; /* get rid of newline */
    if (x > *width)
	*width = x;
    return 1;
}
#endif SUNTOOL

find_help(str, file)
register caddr_t *str;
register char *file;
{
    register char	*p;
    char		args[MAXLINES][MAXLENGTH], help_str[40];
    register int	n, height;
    extern char		*no_newln();

    /* If no file given, take "str" arg as message to print */
    if (!file || !*file) {
	file = NULL;
	for(height = 0; *str && height < MAXLINES-1; height++)
	    (void) strcpy(args[height], *str++);
    } else {
	if (!*file_name || strcmp(file_name, file)) {
	    (void) strcpy(file_name, file);
	    if (help_file)
		fclose(help_file), help_file = NULL_FILE;
	}
	/* file remains open */
	if (!help_file && !(help_file = fopen(file_name, "r")))
	    return -1;
	/* look for %str% in helpfile */
	(void) sprintf(help_str, "%%%s%%\n", str);
	rewind(help_file);
	while(p = fgets(args[0], MAXLENGTH, help_file))
	    if (*p == '%' && !strcmp(p, help_str))
		break;
	if (!p || !fgets(args[0], MAXLENGTH, help_file))
	    for(height = 0; def_msg[height] && height < MAXLINES-1; height++)
		(void) strcpy(args[height], def_msg[height]);
	else for (height = 0; p && *p && strcmp(p, "%%\n"); height++)
	    p = fgets(args[height+1], MAXLENGTH, help_file);
    }
    if (height == MAXLINES - 1)
	print("Help message is too long!\n");

    for (n = 0; n < height; n++) {
	(void) no_newln(args[n]);
	wprint("%s\n", args[n]);
    }

    return 0;
}
