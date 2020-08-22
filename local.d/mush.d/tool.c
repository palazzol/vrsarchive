/* @(#)tool.c	(c) copyright	10/15/86 (Dan Heller) */

/* tool.c --make the mailtool windows, panels, etc... */
#include "mush.h"

make_tool(argc, argv)
char **argv;
{
    struct stat rootbuf, tmpbuf;
    struct inputmask im;
    register unsigned i;
    char **args = DUBL_NULL, **choice_args, **button_args, *p;
    char buf1[WIN_NAMESIZE], buf2[WIN_NAMESIZE];

    getfonts();
    mail_icon.ic_font = fonts[DEFAULT];

    if (p = do_set(set_options, "screen_win"))
	screen = atoi(p);
    else
	screen = 6;

    time_out = TIME_OUT;
    /* where to place text on mail icon -- how many messages there are */
    rect_construct(&mail_icon.ic_textrect,
	l_width(DEFAULT), 58-l_height(DEFAULT),
	3*l_width(DEFAULT), l_height(DEFAULT));

    tool_parse_all(&argc, argv, &args, prog_name);

    if (!(tool = tool_make(
	WIN_ICON, 		&mail_icon,
	WIN_HEIGHT, 		700,
	WIN_WIDTH, 		650,
	WIN_BOUNDARY_MGR, 	1,
	WIN_ATTR_LIST, 		args,
	NULL)))
	perror(prog_name), cleanup(0);
    tool_free_attribute_list(args);

    choice_args = panel_make_list(
	PANEL_MENU_TITLE_FONT, fonts[LARGE],
	PANEL_DISPLAY_LEVEL, PANEL_NONE,
	PANEL_SHOW_MENU, TRUE,
	PANEL_SHOW_MENU_MARK, FALSE,
	0);

    button_args = panel_make_list(
	PANEL_FEEDBACK, PANEL_INVERTED,
	PANEL_SHOW_MENU, FALSE,
	0);

    make_hdr_panel(choice_args, button_args);

    if (!(hdr_sw = gfxsw_createtoolsubwindow(tool, "hdr_sw",
	TOOL_SWEXTENDTOEDGE, 10+ screen*l_height(DEFAULT), (char **)0)))
	perror("hdr_sw"), cleanup(0);
    gfxsw_getretained((struct gfxsubwindow *)hdr_sw->ts_data);
    hdr_win = ((struct gfxsubwindow *)(hdr_sw->ts_data))->gfx_pixwin;

    input_imnull(&im);
    win_setinputcodebit(&im, LOC_STILL);
    win_setinputcodebit(&im, LOC_MOVE);
    win_setinputcodebit(&im, LOC_WINENTER);
    for (i = VKEY_FIRSTFUNC; i <= VKEY_LASTFUNC; i++)
	win_setinputcodebit(&im, i);
    win_setinputmask(hdr_sw->ts_windowfd, &im, &im,
				   win_fdtonumber(hdr_panel_sw->ts_windowfd)); 
    hdr_sw->ts_io.tio_selected = hdr_io;
    hdr_sw->ts_io.tio_handlesigwinch = hdrwin_handlesigwinch;

    make_main_panel(choice_args, button_args);
    xfree(choice_args), xfree(button_args);

    if (!(print_sw = gfxsw_createtoolsubwindow(tool, "print_sw",
	TOOL_SWEXTENDTOEDGE, l_height(LARGE) + 10, (char **)0)))
	perror("print_sw"), cleanup(0);
    print_win = ((struct gfxsubwindow *)(print_sw->ts_data))->gfx_pixwin;
    print_sw->ts_io.tio_handlesigwinch = print_sigwinch;

    /* text subwindow */
    if (!(msg_sw = gfxsw_createtoolsubwindow(tool, "msg_sw",
	TOOL_SWEXTENDTOEDGE, TOOL_SWEXTENDTOEDGE, (char **)0)))
	perror("msg_sw"), cleanup(0);
    gfxsw_getretained((struct gfxsubwindow *)msg_sw->ts_data);
    msg_win = ((struct gfxsubwindow *)(msg_sw->ts_data))->gfx_pixwin;

    /* everything we want the text window to pay attention to */
    input_imnull(&im);
    im.im_flags = IM_ASCII;
    im.im_flags &= ~IM_ANSI;
    for (i = VKEY_FIRSTFUNC; i <= VKEY_LASTFUNC; i++)
	win_setinputcodebit(&im, i);
    win_setinputmask(msg_sw->ts_windowfd, &im, &im, WIN_NULLLINK);
    msg_sw->ts_io.tio_selected = msg_io;
    msg_sw->ts_io.tio_handlesigwinch = msgwin_handlesigwinch;

    /* tty subwindow */
    if (!(tty_sw = ttytlsw_createtoolsubwindow(tool, "tty_sw",
	TOOL_SWEXTENDTOEDGE, 0, (char **)0)))
	perror("tty_sw"), cleanup(0);
    ttysw_handlesigwinch(tty_sw);
    win_setcursor(tty_sw->ts_windowfd, &write_cursor);

    (void) sprintf(blank, "%128c", ' ');
    (void) signal(SIGWINCH, sigwinchcatcher);
    (void) signal(SIGTERM, sigtermcatcher);
    (void) signal(SIGCHLD, sigchldcatcher);
    pw_writebackground(hdr_win, 0,0, hdr_rect.r_width, hdr_rect.r_height,
		       PIX_CLR);
    if (ioctl(0, TIOCGETC, &tchars))
	perror("gtty failed"), cleanup(0);
    win_numbertoname (0, buf1);
    if ((rootfd = open(buf1, 0)) == -1)
	error("can't open %s", buf1), cleanup(0);
    if (fstat(rootfd, &rootbuf))
	error("can't stat %s", buf1), cleanup(0);
    for (parentfd = 3; parentfd < rootfd; parentfd++)
	if (fstat(parentfd, &tmpbuf))
	    error("Can't stat fd-%d", parentfd);
	else if (tmpbuf.st_ino == rootbuf.st_ino) {
	    (void) close(rootfd);
	    rootfd = parentfd;
	    break;
	}
    istool = 2;
    (void) do_version();
    lock_cursors();
    tool_install(tool);
    tool_display(tool);
    do_clear();
}
