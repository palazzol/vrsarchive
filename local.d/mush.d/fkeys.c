/* @(#)fkeys.c		(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"
#define draw(x1,y1,x2,y2,OP) pw_vector(msg_win, x1,y1,x2,y2,OP,1)
#define box(x1,y1,x2,y2,OP)  \
	draw(x1,y1, x1,y2,OP), draw(x1,y2, x2,y2,OP), \
        draw(x2,y2, x2,y1,OP), draw(x2,y1, x1,y1,OP)

struct cmd fkey_cmds[] = {
    { "top", fkey_cmd }, { "close", fkey_cmd }, { "bottom", fkey_cmd },
    { "move", fkey_cmd }, { "stretch", fkey_cmd }, { "repaint", fkey_cmd },
    { "settings", fkey_settings }, { NULL, quit }
};

#define L(n)		KEY_LEFTFIRST+(n)-1
#define R(n)		KEY_RIGHTFIRST+(n)-1
#define F(n)		KEY_TOPFIRST+(n)-1
#define BREAK_KEY	KEY_TOPLAST

char *leftkey_default_settings[] = {
    "Left Function Key Settings",
    "Unused",		/* L1  */    "next",      	/* L2  */
    "undelete",	 	/* L3  */    "delete", 		/* L4  */
    "replyall",  	/* L5  */    "replysender",   	/* L6  */
    "mail",		/* L7  */    "Unset",		/* L8  */
    "lpr",  		/* L9  */    "settings L",      /* L10 */
    0
};

char *topkey_default_settings[] = {
    "Top Function Key Settings",
    "top",		/* T1  */     "close",	    	/* T2  */
    "move",		/* T3  */     "stretch", 	/* T4  */
    "bottom",		/* T5  */     "repaint",   	/* T6  */
    "Unset",		/* T7  */     "Unset",		/* T8  */
    "settings F", 	/* T9  */
    0
};

char *rightkey_default_settings[] = {
    "Right Function Key Settings",
    "Unset",		/* R1  */     "Unset",		 /* R2  */
    "Unset",		/* R3  */     "Unset",		 /* R4  */
    "Unset",		/* R5  */     "Unset",		 /* R6  */
    "Unset",		/* R7  */     "Unset",		 /* R8  */
    "Unset",		/* R9  */     "Unset",		 /* R10 */
    "Unset",		/* R11 */     "Unset",		 /* R12 */
    "Unset",		/* R13 */     "Unset",		 /* R14 */
    "settings R",       /* R15 */
    0
};

/*
 * Execute commands defined by a function key.
 * Left keys:
 * L1 = (null)  can't be set
 * L2 ... L10
 * Top function keys
 * F1 ... F9, BREAK/backspace (key not definable)
 * Right function keys
 * R1 ... R15
 * Usually, the last Function key displays the others' settings.
 */
fkey(key)
register char *key;
{
    register char **argv, *p;
    char buf[256];
    int n;

    /* user defined here...        ... default settings here */
    if (!strncmp((p = key_set_to(key)), "Un", 2)) {
	print("Funciton key: %s  %s", key, p);
	return 0;
    }
    /* make_command will screw up "p", so copy it first */
    (void) strcpy(buf, p);
    Debug("(%s) \"%s\": ", key, p), turnon(glob_flags, CONT_PRNT);
    if (argv = make_command(buf, TRPL_NULL, &n))
	(void) do_command(n, argv, msg_list);
    return -1;
}

fkey_settings(i, argv)
register char i;
register char **argv;
{
    register char key, *p, **fkey_str;
    char buf[256];
    char *help_args[17];

    if (!*++argv) {
	print("Must specify one of L, F or R to identify a function key set");
	return -1;
    }
    key = **argv;
    switch(Upper(key)) {
	when 'L': fkey_str = leftkey_default_settings;
	when 'F': fkey_str = topkey_default_settings;
	when 'R': fkey_str = rightkey_default_settings;
	otherwise: print("Invalid key set: %c (choose L, F or R)", key);
		   return -1;
    }
    help_args[0] = fkey_str[0];
    for (i = 1; fkey_str[i]; i++) {
	p = key_set_to(sprintf(buf, "%c%d", key, i));
	help_args[i] = savestr(sprintf(buf, "%c%-2.d       %s", key, i, p));
    }
    help_args[i] = 0;  /* null terminate list */
    (void) help(print_sw->ts_windowfd, help_args, NULL);

    free_vec(help_args+1);
    return 0;
}

char *
key_set_to(p)
register char *p;
{
    register char *p2, **fkey_str;

    switch(*p) {
	when 'L': fkey_str = leftkey_default_settings;
	when 'F': fkey_str = topkey_default_settings;
	when 'R': fkey_str = rightkey_default_settings;
    }
    p2 = do_set(fkeys, p);
    return (p2)? p2: fkey_str[atoi(p+1)];
}

fkey_cmd(x, p)
register char **p;
{
    if (!strcmp(*p, "close"))
	toolquit(NO_ITEM, 0, NO_EVENT);
    else if (!strcmp(*p, "top"))
	wmgr_top(tool->tl_windowfd, rootfd);
    else if (!strcmp(*p, "move"))
	wmgr_move(tool->tl_windowfd, rootfd);
    else if (!strcmp(*p, "stretch"))
	wmgr_stretch(tool->tl_windowfd, rootfd);
    else if (!strcmp(*p, "bottom"))
	wmgr_bottom(tool->tl_windowfd, rootfd);
    else if (!strcmp(*p, "repaint"))
	wmgr_refreshwindow(tool->tl_windowfd, rootfd);
    return -1;
}

/* execute a command given a function key, if the key is user defined,
 * call fkey() at top of file. Parameter is the key number in "ascii"
 */
func_key(key)
register int key;
{
    register char *p;
    char buf[4];
    int nkey;

    if (key >= KEY_LEFTFIRST && key <= KEY_LEFTLAST)
	buf[0] = 'L', nkey = key - KEY_LEFTFIRST;
    else if (key >= KEY_TOPFIRST && key <= KEY_TOPLAST)
	buf[0] = 'F', nkey = key - KEY_TOPFIRST;
    else if (key >= KEY_RIGHTFIRST && key <= KEY_RIGHTLAST)
	buf[0] = 'R', nkey = key - KEY_RIGHTFIRST;
    (void) sprintf(buf+1, "%d", nkey+1);

    return fkey(buf);
}

void
set_fkeys()
{
    ready(display_keys() + 10);
    print_valid_functions(txt.y+20);
    getting_opts = 2;
    win_setcursor(msg_sw->ts_windowfd, &checkmark);
}

char *MSG = "Click the appropriate mouse button over a function key";
ready(Y)
{
    static int y;
    int x = (msg_rect.r_width - strlen(MSG)*l_width(LARGE))/2;
    if (Y)
	y = Y;
    Clrtoeol(msg_win, (txt.x = 0), (txt.y = y), LARGE);
    highlight(msg_win, x, y, LARGE, MSG);
}

/* number of pixels in x and y directions describing the size of a graphical
 * function key.  they represent the little keys and big keys respectively.
 */
/* static struct pr_pos fkey_sizes[2] = { { 23, 23 }, { 50, 23 } }; */
static struct pr_pos fkey_sizes[2] = { { 24, 23 }, { 52, 23 } };

#define BORDER		4   /* border (distance) between keys */
#define KEYTOP		15  /* distance from top to start drawing */
#define LEFT_START	15  /* pixels from left to start drawing boxes */
#define TOP_START	(LEFT_START+2*fkey_sizes[0].x + fkey_sizes[1].x+BORDER)
#define RIGHT_START	(TOP_START + 5*(fkey_sizes[0].x+BORDER) + \
				     5*(fkey_sizes[1].x+BORDER))

/*
 * if !p, we're setting key at location x,y.
 * else Set that key to this string (p).
 */
void
set_key(p, x,y)
register char *p;
register int x,y;
{
    char 	buf[256], **argv;
    static char *key;
    int 	argc;

    static int key_x, key_y;
    if (!p) {
	if (key = find_key(x,y)) {
	    print("Type new setting for key: %s", key);
	    (void) sprintf(buf, "Function key \"%s\": ", key);
	    highlight(msg_win, 20, txt.y, LARGE, buf);
	    txt.x = 20 + strlen(buf)*l_width(LARGE);
	    Clrtoeol(msg_win, txt.x, txt.y, LARGE);
	    pw_char(msg_win, txt.x, txt.y, PIX_SRC, fonts[curfont], '_');
	} else
	    ready(0);
	key_x = x, key_y = y;
    } else {
	int save_bang = ison(glob_flags, IGN_BANG);
	if (!*p)
	    (void) sprintf(buf, "unfkey %s", key);
	else
	    (void) sprintf(buf, "fkey %s \"%s\"", key, p);
	turnon(glob_flags, IGN_BANG);
	if (argv = make_command(buf, TRPL_NULL, &argc)) {
	    (void) do_command(argc, argv, msg_list);
	    print("Function key %s:  %s", key, key_set_to(key));
	}
	if (!save_bang)
	    turnoff(glob_flags, IGN_BANG);
	ready(0);
    }
}

/* passed the x and y coords of a mouse click, return the function key
 * that exists in that position. NULL if no key there.  string will be
 * something like "L6" or "F9" or "R12"
 */
char *
find_key(x,y)
int x, y;
{
    static char buf[6];
    int row, col;
    static int old_left, old_top, old_right, old_bot;

    if (!(row = find_y(&y)))
	return NULL;
    if (x < LEFT_START || x > RIGHT_START + 3*(fkey_sizes[0].x+BORDER))
	return NULL;   /* out of range */
    if (x > LEFT_START && x < TOP_START-fkey_sizes[0].x - BORDER) {
	if ((col = (x > LEFT_START + fkey_sizes[0].x + BORDER)+1) == 1)
	    x = LEFT_START+1;
	else x = LEFT_START + fkey_sizes[0].x + BORDER + 1;
	if (col == 1 && row == 1)
	    return NULL;
	/* unhighlight the old function key image */
	if (old_left)
	    box(old_left, old_top, old_right, old_bot, PIX_CLR);
	old_left = x, old_top = y;
	old_right = x+fkey_sizes[(col != 1)].x-2, old_bot = y+fkey_sizes[0].y-2;
	/* highlight most recently selected function key image */
	box(x,y, old_right, old_bot, PIX_SRC);

	return sprintf(buf, "L%d", col + 2*(row-1));
    }
    if (x > TOP_START && x < RIGHT_START - fkey_sizes[0].x - BORDER) {
	int which;
	if (row > 1)
	    return NULL;
	which = (x - TOP_START) / (fkey_sizes[0].x + BORDER) + 1;
	if (which == 15)
	    return NULL;   /* Can't set break key (backspace on a sun3) */
	if (which == 14)
	    x = TOP_START + ((which = 9)-2) * (fkey_sizes[1].x+BORDER) -
		    fkey_sizes[0].x - BORDER + 1;
	else if (which <= 2)
	    x = TOP_START + (which-1) * (fkey_sizes[0].x+BORDER) + 1;
	else {
	    which = (which+3)/2;
	    x = TOP_START + (which-2) * (fkey_sizes[1].x+BORDER) + 1;
	}

	/* unhighlight the old function key image */
	if (old_left)
	    box(old_left, old_top, old_right, old_bot, PIX_CLR);
	old_left = x, old_top = y;
	old_right = x+fkey_sizes[(which > 2 && which < 8)].x-2;
	old_bot = y+fkey_sizes[0].y-2;
	/* highlight most recently selected function key image */
	box(x,y, old_right, old_bot, PIX_SRC);

	return sprintf(buf, "F%d", which);
    }
    if (x > RIGHT_START) {
	if (x < RIGHT_START + fkey_sizes[0].x)
	    x = RIGHT_START+1, col = 1;
	else if (x < RIGHT_START + fkey_sizes[0].x + BORDER)
	    return NULL;  /* cursor was clicked between keys */
	else if (x < RIGHT_START + 2*fkey_sizes[0].x + BORDER)
	    x = RIGHT_START+fkey_sizes[0].x+BORDER+1, col = 2;
	else if (x < RIGHT_START + 2 * (fkey_sizes[0].x+BORDER))
	    return NULL;  /* click between keys again */
	else x = RIGHT_START + 2*(fkey_sizes[0].x+BORDER)+1, col = 3;

	/* unhighlight the old function key image */
	if (old_left)
	    box(old_left, old_top, old_right, old_bot, PIX_CLR);
	old_left = x, old_top = y;
	old_right = x+fkey_sizes[0].x-2, old_bot = y+fkey_sizes[0].y-2;
	/* highlight most recently selected function key image */
	box(x,y, old_right, old_bot, PIX_SRC);

	return sprintf(buf, "R%d", col + 3*(row-1));
    }
    return NULL;
}

/* find_y will find which row in a function key pad a y coordinate
 * represents. return 1,2,3,4, or 5   0 if inbetween rows
 */
find_y(y)
register int *y;
{
    int Y, y_incr = fkey_sizes[0].y, ret_value = 0;
    for (Y = KEYTOP; Y <= KEYTOP + 6*y_incr + 4 * BORDER; Y += y_incr + BORDER)
	if (*y < Y) {
	    *y = (Y - y_incr - BORDER) + 1;
	    return ret_value;
	} else ret_value++;
    return 0;
}

char *l_msg = "Specifies which function key for setting value";
char *m_msg = "Display current value for function key";
char *r_msg = "Help setting and viewing function key values";

display_keys()
{
    register int i, x,y;
    register char *p;

    do_clear();

    x = LEFT_START, y = KEYTOP;
    /* print left keys */
    for (i = 0; i < 10; i++) {
	box(x, y, x + fkey_sizes[i%2].x, y + fkey_sizes[i%2].y, PIX_SRC);
	box(x+2, y+2, x+fkey_sizes[i%2].x-2, y+fkey_sizes[i%2].y-2, PIX_SRC);
	if (i && (p = find_key(x+4,y+4)))
	    pw_text(msg_win, x+3, y+3+l_height(SMALL), PIX_SRC,fonts[SMALL], p);
	else pw_replrop(msg_win, x+3, y+3, fkey_sizes[0].x-5, fkey_sizes[0].y-5,
		 PIX_SRC | PIX_DST, &shade_50, 0,0);
	if (i % 2)
	    y += fkey_sizes[0].y + BORDER, x = LEFT_START;
	else
	    x += fkey_sizes[0].x + BORDER;
    }

    x = TOP_START, y = KEYTOP;
    /* print top keys */
    for (i = 1; i <= 10; i++) {
	register int n = (i >= 3 && i <= 7);
	box(x, y, x + fkey_sizes[n].x, y + fkey_sizes[n].y, PIX_SRC);
	box(x+2, y+2, x + fkey_sizes[n].x-2, y + fkey_sizes[n].y-2, PIX_SRC);
	if (i != 10 && (p = find_key(x+4,y+4)))
	    pw_text(msg_win, x+3, y+3+l_height(SMALL), PIX_SRC,fonts[SMALL], p);
	/* shade the break key (backspace on sun3's) -- can't set */
	else if (i == 10)
	    pw_replrop(msg_win, x+3, y+3, fkey_sizes[n].x-5, fkey_sizes[n].y-5,
		 PIX_SRC | PIX_DST, &shade_50, 0,0);
	x += fkey_sizes[n].x + BORDER;
    }

    x = RIGHT_START;
    /* print right keys */
    for (i = 0; i < 15; i++) {
	box(x, y, x + fkey_sizes[0].x, y + fkey_sizes[0].y, PIX_SRC);
	box(x+2, y+2, x + fkey_sizes[0].x-2, y + fkey_sizes[0].y-2, PIX_SRC);
	if (p = find_key(x+4,y+4))
	    pw_text(msg_win, x+3, y+3+l_height(SMALL),PIX_SRC, fonts[SMALL], p);
	if (!((i+1) % 3))
	    y += fkey_sizes[0].y + BORDER, x -= 2*(fkey_sizes[0].x + BORDER);
	else
	    x += fkey_sizes[0].x + BORDER;
    }
    x = TOP_START;
    y = KEYTOP + BORDER + fkey_sizes[0].y + l_height(DEFAULT);
    pw_rop(msg_win, x, y-11, 16,16, PIX_SRC, &mouse_left, 0,0);
    pw_text(msg_win, x+30, y, PIX_SRC, fonts[DEFAULT], l_msg);

    y += BORDER + fkey_sizes[0].y;
    pw_rop(msg_win, x,y-11, 16,16, PIX_SRC, &mouse_middle, 0,0);
    pw_text(msg_win, x+30, y, PIX_SRC, fonts[DEFAULT], m_msg);

    y += BORDER + fkey_sizes[0].y;
    pw_rop(msg_win, x,y-11, 16,16, PIX_SRC, &mouse_right, 0,0);
    pw_text(msg_win, x+30, y, PIX_SRC, fonts[DEFAULT], r_msg);

    x = (msg_rect.r_width - 26*l_width(DEFAULT))/2;
    y += BORDER + fkey_sizes[0].y;
    highlight(msg_win, x, y, DEFAULT, "You may not set shaded keys");

    y += BORDER + fkey_sizes[0].y + 15;
    for (i = 0; i < BORDER; i++)
	draw(0, y+i, msg_rect.r_width, y+i, PIX_SRC);
    y += 10;
    for (i = 0; i < BORDER; i++)
	draw(0, y+l_height(LARGE)+i, msg_rect.r_width, y+l_height(LARGE)+i,
	     PIX_SRC);
    return y;
}

print_valid_functions(y)
register int y;
{
    register int x, n, cmd_len = 12 * l_width(DEFAULT);
    register char *p;

    y += 20, x = (msg_rect.r_width - 25*l_width(LARGE))/2;
    highlight (msg_win, x, y, LARGE, "Available Command Names");
    y += 20, x = 30;
    for (n = 0; p = cmds[n].command; n++) {
	if (x + cmd_len > msg_rect.r_width - 5)
	    y += l_height(DEFAULT), x = 30;
	pw_text(msg_win, x, y, PIX_SRC, fonts[DEFAULT], p);
	x += cmd_len;
    }
    for (n = 0; p = fkey_cmds[n].command; n++) {
	if (x + cmd_len > msg_rect.r_width - 5)
	    y += l_height(DEFAULT), x = 30;
	pw_text(msg_win, x, y, PIX_SRC, fonts[DEFAULT], p);
	x += cmd_len;
    }
}
