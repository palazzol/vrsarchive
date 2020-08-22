/* init.c	(c) copyright 1986 (Dan Heller) */

/* init.c -- functions and whatnot that initialize everything */
#include "mush.h"
#include <pwd.h>

#ifdef SUNTOOL
/* mouse symbols */
short dat_mouse_left[] = {
#include <images/confirm_left.pr> 
};

short dat_mouse_middle[] = {
#include <images/confirm_middle.pr> 
};

short dat_mouse_right[] = {
#include <images/confirm_right.pr> 
};

short dat_mail_icon_1[] = {
#include "mail.icon.1"
};

short dat_mail_icon_2[] = {
#include "mail.icon.2"
};

short dat_coffee_cup[] = {
#include "coffee.cup.pr"
};

short dat_read_cursor[] = {
#include "glasses.pr"
};

short dat_write_cursor[] = {
#include "write.pr"
};

short dat_up_arrow[] = {
#include "up.arrow.pr"
};

short dat_dn_arrow[] = {
#include "dn.arrow.pr"
};

short dat_envelope[] = {
#include "envelope.pr"
};

short dat_cycle_cursor[] = {
#include "cycle.pr"
};

short dat_check_cursor[] = {
#include "check.pr"
};

mpr_static(mail_icon_image1, 64, 64, 1, dat_mail_icon_1);
mpr_static(mail_icon_image2, 64, 64, 1, dat_mail_icon_2);

mpr_static(mouse_left,      16, 16, 1, dat_mouse_left);
mpr_static(mouse_middle,    16, 16, 1, dat_mouse_middle);
mpr_static(mouse_right,     16, 16, 1, dat_mouse_right);
mpr_static(coffee_cup,      16, 16, 1, dat_coffee_cup);
mpr_static(glasses_cursor,  16, 16, 1, dat_read_cursor);
mpr_static(pencil_cursor,   16, 16, 1, dat_write_cursor);
mpr_static(up_arrow,        16, 16, 1, dat_up_arrow);
mpr_static(dn_arrow,        16, 16, 1, dat_dn_arrow);
mpr_static(envelope_cursor, 16, 16, 1, dat_envelope);
mpr_static(cycle,           16, 16, 1, dat_cycle_cursor);
mpr_static(check_cursor,    16, 16, 1, dat_check_cursor);

struct cursor l_cursor     = { 3, 3, PIX_SRC,         &mouse_left      };
struct cursor m_cursor     = { 3, 3, PIX_SRC,         &mouse_middle    };
struct cursor r_cursor     = { 3, 3, PIX_SRC,         &mouse_right     };
struct cursor coffee       = { 8, 8, PIX_SRC,         &coffee_cup      };
struct cursor read_cursor  = { 8, 8, PIX_SRC|PIX_DST, &glasses_cursor  };
struct cursor write_cursor = { 8, 8, PIX_SRC|PIX_DST, &pencil_cursor   };
struct cursor main_cursor  = { 8, 8, PIX_SRC,         &envelope_cursor };
struct cursor checkmark = { 8, 8, PIX_SRC|PIX_DST, &check_cursor };

/* text and font will be set in mail_status() */
struct icon mail_icon = { 64,  64, (struct pixrect *)NULL,
            { 0, 0, 64, 64 }, &mail_icon_image1,
            { 5, 5, 26, 12 }, NULL, (struct pixfont *)NULL, 0 };

char *font_files[] = {
    "serif.r.14", "sail.r.6", "serif.r.16",
};

char *alt_fonts[] = {
    "gacha.r.8", "sail.r.6", "screen.r.14",
};

#endif SUNTOOL

#ifdef BSD
#include <netdb.h>
#endif BSD

void
init()
{
    char 		buf[BUFSIZ], cwd[128], *home;
    extern char		*getwd(), *getlogin();
#ifdef SYSV
    char *getcwd();
    extern struct passwd *getpwuid();  /* sys-v forgot this in pwd.h! */
#else
    char ourhost[128];
#endif SYSV
    register char 	*p, *p2 = buf, **argv;
    struct passwd 	*entry;
    int			cnt = 0;
#ifdef BSD
    struct hostent 	*hp;
#endif BSD

    home = getenv ("HOME");

    if (!(entry = getpwuid(getuid())))
	if (p = getlogin())
	    strdup(login, p);
	else {
	    login = "unknown";
	    print("I don't know you, but that's ok.\n");
	}
    else {
	strdup(login, entry->pw_name);
	if (!home || !*home)
	    home = entry->pw_dir;
	endpwent();
    }
    if (!home || !*home || access(home, 2)) {
	error(home);
	home = ALTERNATE_HOME;
	print_more("Using \"%s\" as home.\n", home);
    }

#ifndef SYSV
    (void) gethostname(ourhost, sizeof ourhost);
    if (!(hp = gethostbyname(ourhost)))
	error("gethostbyname: %s", ourhost);
    else for (p = hp->h_name; p && *p && cnt < MAX_HOST_NAMES;
	      p = hp->h_aliases[cnt++])
	ourname[cnt] = savestr(p);
    endhostent();
#endif SYSV

#ifndef SYSV
    if (getwd(cwd) == NULL)
#else
    if (getcwd(cwd, 128) == NULL)
#endif SYSV
	error("getcwd: %s", cwd), *cwd = 0;

    p2 += strlen(strcpy(p2, "set "));
    p2 += strlen(sprintf(p2, "cwd=\"%s\" ", cwd));
    p2 += strlen(sprintf(p2, "home=\"%s\" ", home));
    p2 += strlen(sprintf(p2, "prompt=\"%s\" ", DEF_PROMPT));
    p2 += strlen(sprintf(p2, "mbox=\"%s\" ",   DEF_MBOX));
    p2 += strlen(sprintf(p2, "folder=\"%s\" ", DEF_FOLDER));
    p2 += strlen(sprintf(p2, "escape=\"%s\" ", DEF_ESCAPE));

    p = getenv("SHELL");
    p2 += strlen(sprintf(p2, "shell=\"%s\" ", (p)? p: DEF_SHELL));

    p = getenv("EDITOR");
    p2 += strlen(sprintf(p2, "editor=\"%s\" ", (p)? p: DEF_EDITOR));

    p = getenv("VISUAL");
    p2 += strlen(sprintf(p2, "visual=\"%s\" ", (p)? p: DEF_EDITOR));

    p = getenv("PAGER");
    p2 += strlen(sprintf(p2, "pager=\"%s\" ", (p)? p: DEF_PAGER));

    p = getenv("PRINTER");
    p2 += strlen(sprintf(p2, "printer=\"%s\" ", (p)? p: DEF_PRINTER));

    crt = 25;
    /* p2 += strlen(strcat(p2, "crt=\"25\" ")); */
    screen = 18;
    /* p2 += strlen(strcat(p2, "screen=\"18\" ")); */

    if (!(argv = make_command(buf, TRPL_NULL, &cnt)))
	print("error initializing variables.\n");
    else {
	(void) set(cnt, argv);
	free_vec(argv);
    }
#ifdef CURSES
    init_bindings();
#endif CURSES
}

/*
 * Source a file, or just the default file.  Since sourcing files
 * means reading possible aliases, don't expand the ! as history
 * by setting the IGN_BANG flag.  Since a command in the sourced file
 * may call source on another file, this routine may be called from
 * within itself.  Continue to ignore ! chars by setting save_bang (local).
 *
 * Try opening the file passed to us.  If not given, check for the correct
 * .rc file which is found in the user's home dir.
 */
source(argc, argv)
char **argv;
{
    register char *p, *p2, **newargv;
    int 	  line_no = 0, if_else = 0, parsing = 1, cont_line = 0;
    FILE 	 *fp;
    char 	  file[128], line[BUFSIZ];
    long	  save_bang = ison(glob_flags, IGN_BANG);

    if (argc && *++argv && !strcmp(*argv, "-?"))
	return help(0, "source_help", cmd_help);
    if (argc && *argv)
	(void) strcpy(file, *argv);
    else if (p = getenv("MAILRC"))
	(void) strcpy(file, p);
    else {
	char *home = do_set(set_options, "home");
	if (!home || !*home)
	    home = ALTERNATE_HOME;
	if (access(sprintf(file, "%s/%s", home, MAILRC), R_OK)
          && access(sprintf(file, "%s/%s", home, ALTERNATE_RC), R_OK))
	(void) strcpy(file, DEFAULT_RC);
    }

    argc = 0; /* don't ignore ENOENT */
    p = getpath(file, &argc);
    if (argc) {
	if (strcmp(file, DEFAULT_RC))
	    if (argc == -1)
		print("%s: %s\n", file, p);
	    else
		print("%s is a directory.\n", file);
	return -1;
    }
    if (!(fp = fopen(p, "r"))) {
	if (errno != ENOENT)
	    error("Can't open %s", p);
	return -1;
    }
    (void) strcpy(file, p);
    turnon(glob_flags, IGN_BANG); /* ignore ! when reading record files */
    while (p = fgets(&line[cont_line], BUFSIZ - cont_line, fp)) {
	line_no++;
	if (*(p2 = no_newln(line + cont_line)) == '\\') {
	    cont_line = p2 - line;
	    continue;
	} else
	    cont_line = 0;
	/* don't consider comments (#) in lines. check if # is within quotes */
        if (p = any(p, "\"'#")) {
	    register int balanced = 1;
	    while (p && (*p == '\'' || *p == '"') && balanced) {
		/* first find matching quote */
		register char *quote = index(p+1, *p);
		if (!quote) {
		    print("%s: line %d: unbalanced %c.\n", file, line_no, *p);
		    balanced = 0;
		} else
		    p = any(quote+1, "'\"#");
	    }
	    if (!balanced)
		continue;
	    if (p && *p == '#')
		*p = 0; /* found a Comment: null terminate line at comment */
	}
	if (!*line || !(newargv = make_command(line, TRPL_NULL, &argc))) {
	    if (!strncmp(line, "if", 2))
		if_else++, parsing = FALSE;
	    continue;
	}
	if (!strcmp(newargv[0], "endif")) {
	    if (!if_else)
		print("%s: line %d: endif with no \"if\".\n", file, line_no);
	    else
		if_else = 0, parsing = 1;
	    goto bad;
	} else if (!strcmp(newargv[0], "else")) {
	    if (!if_else)
		print("%s: line %d: if-less \"else\".\n", file, line_no);
	    else
		parsing = !parsing;
	    goto bad;
	} else if (parsing && !strcmp(newargv[0], "if")) {
	    /* if statements are of the form:
	     *     if expr
	     *     if !expr  or  if ! expr
	     *     if expr == expr   or   if expr != expr
	     */
	    int equals = TRUE;
	    register char *lhs = newargv[1], *rhs = NULL;

	    if (if_else)
		print("%s: line %d: no nested if statements!\n", file, line_no);
	    else
		if_else = 1;
	    parsing = 0;
	    if (!lhs || !*lhs) {
		print("%s: line %d: if what?\n", file, line_no);
		goto bad;
	    }
	    /* "lhs" is the left hand side of the equation
	     * In this instance, we're doing case 2 above.
	     */
	    if (*lhs == '!') {
		int tmp = argc;
		equals = FALSE;
		if (!*++lhs)
		    if (!(lhs = newargv[2])) {
			print("%s: %d: syntax error: \"if ! <what?>\"\n",
			    file, line_no);
			goto bad;
		    } else
			tmp--;
		if (tmp > 2) {
		    print("%s: %d: syntax error: \"if !<expr> <more junk>\"\n",
			file, line_no);
		    goto bad;
		}
	    } else if (argc > 2) {
		if (argc != 4) {
		    print("%s: %d: argument count error: line has %d args.\n",
			file, line_no, argc);
		    goto bad;
		}
		/* now check newargv[1] for == or != */
		if (!strcmp(newargv[2], "!="))
		    equals = FALSE;
		else if (strcmp(newargv[2], "==")) {
		    print("%s: %d: use `==' or `!=' only.\n", file, line_no);
		    goto bad;
		}
		rhs = newargv[3];
	    }
	    if (!strcmp(lhs, "redirect") &&
		      (ison(glob_flags, REDIRECT) && equals ||
		       isoff(glob_flags, REDIRECT) && !equals)
		|| !strcmp(lhs, "istool") &&
		      (istool && equals || !istool && !equals)
		|| !strcmp(lhs, "hdrs_only") &&
		      (hdrs_only && equals || !hdrs_only && !equals)
		|| !strcmp(lhs, "iscurses") &&
		      ((iscurses || ison(glob_flags, PRE_CURSES)) && equals ||
		      (isoff(glob_flags, PRE_CURSES) && !iscurses && !equals)))
		    parsing = 1;
	    else if (rhs)
		if (strcmp(lhs, rhs) && !equals || !strcmp(lhs, rhs) && equals)
		    parsing = 1;
bad:
	    free_vec(newargv);
	    continue;
	}
	if (parsing && argc > 0)
	    if (!strcmp(newargv[0], "exit")) {
		if_else = 0;
		break;
	    } else
		(void) do_command(argc, newargv, msg_list);
	else
	    free_vec(newargv);
    }
    if (if_else)
	print("%s: missing endif\n", file);
    fclose(fp);
    /* if we entered the routine ignoring !, leave it that way. */
    if (!save_bang)
	turnoff(glob_flags, IGN_BANG);
    return -1;
}

#ifdef SUNTOOL
/* open all fonts and place in fonts array. */
getfonts()
{
    char tmp[80];
    register int offset = strlen(FONTDIR) + 1;
    struct pixfont *pf_open();

    (void) sprintf(tmp, "%s/", FONTDIR);
    for (total_fonts = 0; total_fonts < MAX_FONTS; total_fonts++) {
	(void) strcpy(&tmp[offset], font_files[total_fonts]);
	if (!(fonts[total_fonts] = pf_open(tmp))) {
	    (void) strcpy(&tmp[offset], alt_fonts[total_fonts]);
	    if (!(fonts[total_fonts] = pf_open(tmp))) {
		print("couldn't open font \"%s\"\n", tmp);
		fonts[total_fonts] = pf_default();
	    }
	}
    }
}
#endif SUNTOOL
