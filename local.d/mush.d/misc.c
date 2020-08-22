/* @(#)misc.c	(c) copyright 10/18/86 (Dan Heller) */

#include "mush.h"

/* check to see if a string describes a message that is within the range of
 * all messages; if invalid, return 0 and print error. else return msg number
 */
chk_msg(s)
register char *s;
{
    register int n;

    if ((n = atoi(s)) > 0 && n <= msg_cnt)
	return n;
    else if (*s == '^' && msg_cnt)
	return 1;
    else if (*s == '$' && msg_cnt)
	return msg_cnt;
    else if (*s == '.' && msg_cnt)
	return current_msg+1;
    print("Invalid message number: %s\n", s);
    return 0;
}

/*
 * loop thru all msgs starting with current_msg and find next message whose
 * m_flags has the "flags" bit being on
 */
next_msg(on, flags)
{
    register int n = current_msg;
    if (!msg_cnt)
	return current_msg = 0;
    for (n++; n != current_msg; n++)
	if (n == msg_cnt)     /* hit the end, start back at the beginning */
	    n = -1; /* increments to 0 in  loop  */
	else if (isoff(msg[n].m_flags, DELETE) &&
		((on && ison(msg[n].m_flags, flags)) ||
		(!on && isoff(msg[n].m_flags, flags))))
	    return current_msg = n;
    return current_msg = 0;
}

/* takes string 'p' and address of int (isdir).  If p uses the ~ to reference
 * a home directory of somesort, then expand it.  find out what sort of
 * file final path is. set isidr to 1 if a directory, 0 if not, -1 on error
 * return final path. If an error occurrs, return string indicating error.
 * if isdir has a value of 1 when passed, it ignores "No such file or directory"
 */
#include <pwd.h>
char *
getpath(p, isdir)
register char *p;
int *isdir;
{
    static char buf[256];
    struct stat stat_buf;

    if (!p || !*p || !strcmp(p, "~")) {
	char *home = do_set(set_options, "home");
	if (!home || !*home)
	    home = ALTERNATE_HOME;
	(void) strcpy(buf, home);  /* no arg means home */
    } else if (*p == '~') {
	if (p[1] != '/') {
	    /* not our home, but someone else's
	     * look for ~user or ~user/subpath
	     * if '/' exists, separate into tmp="user" p="subpath"
	     */
	    struct passwd *ent, *getpwnam();
	    char *p2 = p+1;
	    if (p = index(p2, '/'))
		*p++ = 0;
	    if (!(ent = getpwnam(p2))) {
		*isdir = -1;
		return sprintf(buf, "no such user: %s", p2);
	    }
	    /* append subpath to pathname */
	    if (p && *p)
		(void) sprintf(buf, "%s/%s", ent->pw_dir, p);
	    /* if *p == NULL, pathname is done (buf), set isdir = 1 */
	    else {
		*isdir = 1;
		return strcpy(buf, ent->pw_dir);
	    }
	} else {
	    char *home = do_set(set_options, "home");
	    if (!home || !*home)
		home = ALTERNATE_HOME;
	    (void) sprintf(buf, "%s/%s", home, p+2);
	}
    } else if (*p == '%') {
	/* if %user, append user name... else, it's just us */
	(void) sprintf(buf, "%s/", MAILDIR);
	if (!*++p || *p == ' ' || *p == '\t')
	    (void) strcat(buf, login);
	else
	    (void) strcat(buf, p);
    } else if (*p == '+') {
	register char *p2 = do_set(set_options, "folder");
	if (p2 && *p2)
	    (void) sprintf(buf, "%s/%s", p2, ++p);
	else
	    (void) sprintf(buf, "~/%s", p);
	if (*buf == '~') {
	    int t_isdir = *isdir;
	    char *t, tmp[256];
	    (void) strcpy(tmp, buf);
	    t = getpath(tmp, &t_isdir);
	    if (t_isdir == -1) {
		*isdir = -1;
		return t;
	    }
	    /* strcpy(buf, t); --buf already has info because it's static */
	}
    } else {  /* allow \ to escape the special chars, +, %, ~ */
	if (*p == '\\')
	    p++;
	(void) strcpy(buf, p);
    }
    if (stat(buf, &stat_buf)) {
	(void) access(buf, 0); /* set errno to the "real" reason */
	if (errno == ENOENT && *isdir == 1) {
	    *isdir = 0; /* say it's a regular file even tho it doesn't exist */
	    return buf; /* it may be wanted for creating */
	}
	*isdir = -1;
	return sys_errlist[errno];
    }
    *isdir = ((stat_buf.st_mode & S_IFDIR) != 0);
    return buf;
}

/*
 * Given a filename[pointer] (p), a file pointer, and a mode, file_to_fp
 * opens the file with the mode.
 * If the mode is "r" then we read the file into the file pointer at the
 * end (fseek(fp, 2, 0)).  If the file is opened for writing, then read
 * from the beginning of fp and write it into the file.
 * This is usually called to read .signatures into messages (thus,
 * opening .signture with "r" and writing to the end of fp which is probably
 * the sendmail process or the message file pointer) or to write fortunes into
 * the message buffer: reading fp (the popened fortune) and writing into file.
 */
void
file_to_fp(p, fp, mode)
register char *p;
register FILE *fp;
char *mode;
{
    int 	x = 1;
    char 	*file, buf[BUFSIZ];
    FILE 	*tmp_fp;

    if (!p || !*p) {
	print("specify filename");
	return;
    }
    file = getpath(p, &x);
    if (x == -1) { /* on error, file contains error message */
	wprint(file);
	return;
    }
    wprint("%s: ", file), fflush(stdout);
    if (x)   /* if x == 1, then path is a directory */
	wprint("directory.\n");
    else if (!(tmp_fp = fopen(file, mode))) {
	wprint("%s\n", sys_errlist[errno]);
	return;
    } else if (*mode != 'r') {
	rewind(fp);
	for(x = 0; fgets(buf, BUFSIZ, fp); x++)
	    fputs(buf, tmp_fp);
    } else
	for(x = 0; fgets(buf, BUFSIZ, tmp_fp); x++)
	    fputs(buf, fp);
    wprint("%s%d line%s\n", (*mode == 'a')? "added ": "",
				  x, (x == 1)? "": "s");
    fflush(fp);
    fclose(tmp_fp);
}

/*
 * lose the newline character, trailing whitespace, and return the end of p
 * test for '\n' separately since some _ctype_[] arrays may not have the
 * _S bit set for the newline character.  see <ctype.h> for more info.
 */
char *
no_newln(p)
register char *p;
{
    register char *p2 = p + strlen(p);	/* point it to the null terminator */

    while (p2 > p && *--p2 == '\n' || isspace(*p2))
	*p2 = 0;  /* get rid of newline and trailing spaces */
    return p2;
}

/* find any character in string2 that's in string1 */
char *
any(s1, s2)
register char *s1, *s2;
{
    register char *p;
    if (!s1 || !*s1 || !s2 || !*s2)
	return NULL;
    for( ; *s1; s1++) {
	for(p = s2; *p; p++)
	    if (*p == *s1)
		return s1;
    }
    return NULL;
}

/* since print_help just prints help, always return help() */
print_help(argc, argv)
register char **argv;
{
#ifdef SUNTOOL
    if (istool)
	return help(tool->tl_windowfd, "general", tool_help);
#endif SUNTOOL
    if (!argc || !*++argv)
	return help(0, "general", cmd_help);
    return help(0, *argv, cmd_help);
}

/* since this function does not affect messages, return -1 */
help(fd, str, file)
#ifdef SUNTOOL
caddr_t *str;
#else
char *str;
#endif /* SUNTOOL */
char *file;
{
#ifdef SUNTOOL
    if (istool > 1) {
	int oldmask;
	if (!fd)
	    fd = print_sw->ts_windowfd;
	oldmask = sigblock(1 << ((SIGALRM) - 1));
	lock_cursors();
	if (display_help(fd, str, file, fonts[LARGE]) && file)
	    error("can't read %s", file);
	unlock_cursors();
	(void) sigsetmask(oldmask);
    } else
#endif SUNTOOL
    if (find_help(str, file) && file)
	error("can't read %s", file);
    return -1; /* doesn't affect any messages */
}

#ifdef SUNTOOL
void
unlock_cursors()
{
    if (istool < 2)
	return;
    win_setcursor(print_sw->ts_windowfd, &main_cursor);
    win_setcursor(panel_sw->ts_windowfd, &main_cursor);
    if (getting_opts)
	win_setcursor(msg_sw->ts_windowfd, &checkmark);
    else if (ison(glob_flags, IS_GETTING))
	win_setcursor(msg_sw->ts_windowfd, &write_cursor);
    else
	win_setcursor(msg_sw->ts_windowfd, &read_cursor);
    win_setcursor(hdr_panel_sw->ts_windowfd, &main_cursor);
    win_setcursor(hdr_sw->ts_windowfd, &l_cursor);
}

void
lock_cursors()
{
    if (istool < 2)
	return;
    win_setcursor(hdr_sw->ts_windowfd, &coffee);
    win_setcursor(print_sw->ts_windowfd, &coffee);
    win_setcursor(panel_sw->ts_windowfd, &coffee);
    win_setcursor(msg_sw->ts_windowfd, &coffee);
    win_setcursor(hdr_panel_sw->ts_windowfd, &coffee);
}

#include <suntool/fullscreen.h>
/* return the event-id that confirmed */
confirm(fd)
{
    struct fullscreen *fs;

    struct inputmask im;
    struct inputevent event;

    fs = fullscreen_init(fd);
    input_imnull(&im);
    im.im_flags |= IM_ASCII;
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_setinputmask(fd, &im, &im, WIN_NULLLINK);
    win_setcursor(fd, &l_cursor);
    if (input_readevent(fd, &event) == -1)
	error("confim failed");
    fullscreen_destroy(fs);
    return ID;
}
#endif SUNTOOL

/* check two lists of strings each of which contain substrings.
 * Each substring is delimited by any char in "delimeters"
 * return true if any elements in list1 are on list2.
 * thus:
 * string1 = "foo, bar, baz"
 * string2 = "foobar, baz, etc"
 * delimeters = ", \t"
 * example returns 1 because "baz" exists in both lists
 * NOTE: case is ignored.
 */
chk_two_lists(list1, list2, delimeters)
register char *list1, *list2, *delimeters;
{
    register char *p, c;
    register int found = 0;

    if (p = any(list1, delimeters)) {
	for (p++; *p && index(delimeters, *p); p++)
	    ;
	if (chk_two_lists(p, list2, delimeters))
	    return 1;
    }
    if (p = any(list2, delimeters)) {
	for (p++; *p && index(delimeters, *p); p++)
	    ;
	if (chk_two_lists(list1, p, delimeters))
	    return 1;
    }
    if (p) {
	while (index(delimeters, *(p-1)))
	    --p;
	c = *p, *p = 0;
    }
    found = !lcase_strcmp(list1, list2);
    if (p)
	*p = c;
    return found;
}

bzero(addr, size)
register char *addr;
register int size;
{
    while (size-- > 0)
	addr[size] = 0;
}

/* see if there are at least Max lines in this file. return lines less than Max
 * that actually *are* in the file
 */
lines_in(fp, Max)
register FILE *fp;
register int Max;
{
    char tmp[BUFSIZ];
    long place_in_fp = ftell(fp);
    rewind(fp);
    while (Max > 0 && fgets(tmp, BUFSIZ, fp))
	Max--;
    (void) fseek(fp, place_in_fp, 0);
    return Max;
}

/* clear all contents of the file.  Careful that the file is opened for
 * _writing_ --tempfile is opened for reading, so don't try to empty it
 * if you're using ftruncate.   Return -1 on error, 0 on success.
 */
emptyfile(fp, fname)
register FILE **fp;
register char *fname;
{
    Debug("Emptying \"%s\"\n", fname);
#ifndef SYSV
    return ftruncate(fileno(*fp), 0);
#else
    {
	int omask = umask(077), ret;
	fclose(*fp);
	if (!(*fp = fopen(fname, "w")))
	    ret = -1;
	ret = 0;
	(void) umask(omask);
	return ret;
    }
#endif SYSV
}

/* do an atoi() on the string passed and return in "val" the decimal value.
 * the function returns a pointer to the location in the string that is not
 * a digit.
 */
char *
my_atoi(p, val)
register char *p;
register int *val;
{
    if (!p)
	return NULL;
    *val = 0;
    while (isdigit(*p))
	*val = (*val) * 10 + *p++ - '0';
    return p;
}

/* strcmp ignoring case */
lcase_strcmp(str1, str2)
register char *str1, *str2;
{
    while (*str1 && *str2)
	if (lower(*str1) != lower(*str2))
	    break;
	else
	    str1++, str2++;
    return *str1 - *str2;
}

/* strcpy coverting everything to lower case (arbitrary) to ignore cases */
char *
lcase_strcpy(dst, src)
register char *dst, *src;
{
    register char *s = dst;

    /* "lower" is a macro, don't incrment its argument! */
    while (*dst++ = lower(*src))
	src++;
    return s;
}

/* this strcpy returns number of bytes copied */
Strcpy(dst, src)
register char *dst, *src;
{
    register int n = 0;
    if (!dst || !src)
	return 0;
    while (*dst++ = *src++)
	n++;
    return n;
}

void
xfree(cp)
char *cp;
{
    extern char end[];

    if (cp >= end && cp < (char *) &cp && debug < 2)
	free(cp);
}

char *
savestr(s)
register char *s;
{
    register char *p;
    char *malloc();
    if (!s)
	s = "";
    if (!(p = malloc((unsigned) (strlen(s) + 1)))) {
	error("out of memory saving %s", s);
	return NULL;
    }
    return strcpy(p, s);
}

void
free_vec(argv)
char **argv;
{
    register int n;
    if (!argv)
	return;
    for (n = 0; argv[n]; n++)
	xfree(argv[n]);
    xfree((char *)argv);
}

/* copy a vector of strings into one string -- return the end of the string */
char *
argv_to_string(p, argv)
register char *p, **argv;
{
    register int i;
    register char *ptr = p;

    *p = 0;
    if (!argv[0])
	return "";
    for (i = 0; argv[i]; i++)
	ptr += strlen(sprintf(ptr, "%s ", argv[i]));
    *--ptr = 0;   /* get rid of the last space */
    return ptr;
}

/* echo the command line. return -1 cuz no messages are affected */
do_echo(argc, argv)
register char **argv;
{
    char buf[BUFSIZ];
    int no_return;

    if (argc > 1 && !strcmp(argv[1], "-?")) {
	print("usage: %s [-n] ...\n", *argv);
	return -1;
    }
    no_return = *++argv && !strcmp(*argv, "-n");
    (void) argv_to_string(buf, argv+no_return);
    print("%s%s", buf, (no_return)? "" : "\n");
    return -1;
}

/* return -1 on error or number of arguments in argv that were parsed */
get_msg_list(argv, list)
register char **argv;
char list[];
{
    register char *p2, *p;
    char buf[256];
    register int n;

    if (!msg_cnt) {
	print("No messages.\n");
	return -1;
    }
    if (!argv || !*argv) {
	if (isoff(glob_flags, IS_PIPE))
	    set_msg_bit(list, current_msg);
	return 0;
    }
    /* first, stuff argv's args into a single char array buffer */
    (void) argv_to_string(buf, argv);
    p = buf;
    Debug("get_msg_list: parsing: (%s): ", p);
    /*
     * if do_range returns NULL, an invalid message was specified
     */
    if (!(p2 = do_range(p, list)))
	return -1;
    /*
     * if p2 == p (and p isn't $ or ^ or .), then no message list was
     * specified.  set the current message in such cases if we're not piping
     */
    if (p2 == p) {
	if (*p == '$')
	    set_msg_bit(list, msg_cnt-1);
	else if (*p == '^')
	    set_msg_bit(list, 0);
	else if (*p == '.' || isoff(glob_flags, IS_PIPE))
	    set_msg_bit(list, current_msg);
    } else if (ison(glob_flags, IS_PIPE)) {
	print("You can't pipe to a command *and* specifiy a msg_list\n");
	return -1;
    }
    for (n = 0; p2 > p && *argv; n++)
	p2 -= (strlen(*argv++)+1);
    Debug("parsed %d args\n", n);
    return n;
}

char *
itoa(n)
{
    static char buf[10];
    return sprintf(buf, "%d", n);
}

#ifdef NOT_NEEDED_NOW
/* return whether or not this process is in the foreground or background */
foreground()
{
#ifdef TIOCGPGRP
    int tpgrp;	/* short in 4.1, int in 4.2 */

    if (ioctl(0, TIOCGPGRP, (char *)&tpgrp))
	return 0;
    return tpgrp == getpgrp(0);
#else
    return 1;
#endif TIOCGPGRP
}
#endif NOT_NEEDED_NOW

#ifdef SYSV
char *
Sprintf(buf, fmt, args)
register char *buf, *fmt;
{
    vsprintf(buf, fmt, &args);
    return buf;
}
#endif /* SYSV */

/*
 * Finds out how many file descriptors are opened.  Useful for making sure
 * no files got opened in subprocedures which were not subsequently closed.
 */
nopenfiles(argc)
{
    register int nfiles = 0;
#ifdef MAXFILES
    register int size = MAXFILES;
#else
    register int size = getdtablesize();
#endif MAXFILES

    if (argc < 2)
	print("open file descriptors:");
    while (--size >= 0)
        if (fcntl(size, F_GETFL, 0) != -1) {
	    if (argc < 2)
		print_more(" %d", size);
            ++nfiles;
        }
    if (argc < 2)
	print("\n");
    return nfiles;
}

/*
 * Open a path for writing or appending -- return a FILE pointer.
 * If program is TRUE, then use popen, not fopen and don't check 
 * to see if the file is writable.
 */
FILE *
open_file(p, program)
register char *p;
{
    register FILE *newfile = NULL_FILE;
    register char *tmp;
    int x = 1;

    tmp = getpath(p, &x);
    if (x == 1)
	print("%s is a directory.\n", tmp);
    else if (x == -1)
	print("%s: %s\n", p, tmp);
    else {
	register char *mode = NULL;
	/* if it doesn't exist open for "w" */
	if (program || access(tmp, 0))
	    mode = "w";
	/* if we can't write to it, forget it */
	else if (access(tmp, 2))
	    error(tmp);
	else
	    mode = "a";
	if (mode)
	    if (program) {
		if (!(newfile = popen(tmp, mode))) {
		    error("Can't execute %s\n", tmp);
		    return newfile;
		}
	    } else
		if (!(newfile = fopen(tmp, mode)))
		    error("Can't write to %s", tmp);
	    else
		Debug("Successfully opened %s\n", tmp);
    }
    return newfile;
}

/*
 * find_files gets a string of space/comma delimeted words and an array of
 * file pointers and the maximum size that array can be.
 * The object is to find the files or programs listed in "p", attempt
 * to fopen/popen them and save their filepointers in the array. If the
 * size is 0, then just extract the file names and give error messages
 * for each one since they will not be opened. Return the number of
 * files opened and delete all files (opened or not) from the list in
 * "p".  Tokens beginning with a "/, ~, or + are files; tokens beginning
 * with a | are programs.
 */
find_files(p, files, size)
register char *p;
FILE *files[];
{
    register int     total = 0;
    char 	     file[BUFSIZ], *start = p;
    register char    *p2, *s;

    for (s = p; p = any(s, "~+/|"); s = p) {
	int prog = 0;

	/* If there is no space or comma before this address, then
	 * it's not the beginning of an address, but part of another.
	 * Ignore this addres and continue to the next in the list.
	 */
	if (p > start && *(p-1) != ',' && !isspace(*(p-1))) {
	    p++; /* prevent inifinite loop -- resume starting at next char */
	    continue;
	}

	/* get the whole filename, stick it in "file" */
	if (p2 = any(p, ", \t"))
	    *p2++ = '\0';
	else
	    p2 = p + strlen(p);
	(void) strcpy(file, p);
	/* overwrite the filename in the string with whatever proceeds it */
	while (isspace(*p2)) /* move to the next address (or end of string) */
	    p2++;
	if (*p2)
	    (void) strcpy(p, p2);
	else
	    do
		*p = '\0';
	    while (--p >= start && (isspace(*p) || *p == ','));

	if (*file == '|')
	    prog = 1; /* it's a program name */
	if (size && total < size) {
	    /* either open "file" or &file[1] */
	    if (files[total] = open_file(&file[prog], prog))
		total++;
	} else
	    print("No open space for %s\n", file);
	skipspaces(0);
    }
    return total;
}

/*
 * execute a command from a string.  f'rinstance: "pick -f foobar"
 * The string is made into an argv and then run.  Errors are printed
 * if the command failed to make.
 *   NOTES:
 *     NEVER pass stright text: e.g. "pick -f foobar", ALWAYS strcpy(buf, "...")
 *     no history is expanded (ignore_bang).
 */
cmd_line(buf, list)
char buf[], list[];
{
    register char **argv;
    int argc, ret_val = -1;
    long save_bang = ison(glob_flags, IGN_BANG);
    long save_do_pipe = ison(glob_flags, DO_PIPE);
    long save_is_pipe = ison(glob_flags, IS_PIPE);

    turnon(glob_flags, IGN_BANG);
    turnoff(glob_flags, DO_PIPE);
    turnoff(glob_flags, IS_PIPE);
    if (argv = make_command(buf, TRPL_NULL, &argc))
	ret_val = do_command(argc, argv, list);
    if (!save_bang)
	turnoff(glob_flags, IGN_BANG);
    if (save_do_pipe)
	turnon(glob_flags, DO_PIPE);
    if (save_is_pipe)
	turnon(glob_flags, IS_PIPE);
    return ret_val;
}

glob_test(s)
char *s;
{
    print("%s: glob_flags =", s);
    if (ison(glob_flags, DO_UPDATE))
	print_more(" DO_UPDATE");
    if (ison(glob_flags, REV_VIDEO))
	print_more(" REV_VIDEO");
    if (ison(glob_flags, CONT_PRNT  ))
	print_more(" CONT_PRNT");
    if (ison(glob_flags, DO_SHELL    ))
	print_more(" DO_SHELL");
    if (ison(glob_flags, DO_PIPE))
	print_more(" DO_PIPE");
    if (ison(glob_flags, IS_PIPE))
	print_more(" IS_PIPE");
    if (ison(glob_flags, IGN_SIGS))
	print_more(" IGN_SIGS");
    if (ison(glob_flags, IGN_BANG))
	print_more(" IGN_BANG");
    if (ison(glob_flags, ECHO_FLAG))
	print_more(" ECHO_FLAG");
    if (ison(glob_flags, IS_GETTING))
	print_more(" IS_GETTING");
    if (ison(glob_flags, PRE_CURSES))
	print_more(" PRE_CURSES");
    if (ison(glob_flags, READ_ONLY  ))
	print_more(" READ_ONLY");
    if (ison(glob_flags, REDIRECT))
	print_more(" REDIRECT");
    if (ison(glob_flags, WAS_INTR ))
	print_more(" WAS_INTR");
    if (ison(glob_flags, WARNING   ))
	print_more(" WARNING");
    print_more("\n");
}

is_spool(f)
register char *f;
{
    return !strncmp(f, MAILDIR, strlen(MAILDIR));
}

print_argv(argv)
char **argv;
{
    while (*argv)
	if (debug)
	    printf("(%s) ", *argv++);
	else
	    wprint("%s ", *argv++);
    if (debug) {
	putchar('\n');
	fflush(stdout);
    } else
	wprint("\n");
}

msg_flags(c, v, list)
register char **v, *list;
{
    register int	i;
    register long	newflag = 0;

    if (c && *++v && !strcmp(*v, "-?"))
	return help(0, "msg_flags", cmd_help);
    if (c && (c = get_msg_list(v, list)) == -1)
	return -1;
    if (v && *(v += (c-1))) {
	turnon(glob_flags, DO_UPDATE);
	while (*++v)
	    for (c = 0; v[0][c]; c++)
		switch (lower(v[0][c])) {
		    case 'n' : turnon(newflag, UNREAD), turnoff(newflag, OLD);
		    when 'o' : turnon(newflag, OLD);
		    when 'r' : turnoff(newflag, UNREAD);
		    when 'd' : turnon(newflag, DELETE);
		    when 'p' : turnon(newflag, PRESERVE);
		    when 'u' : turnon(newflag, UNREAD);
		    otherwise: return help(0, "msg_flags", cmd_help);
		}
    }

    for (i = 0; i < msg_cnt; i++) {
	if (!msg_bit(list, i))
	    continue;
	else if (!newflag) {
	    wprint("msg %d: offset: %d, lines: %d, bytes: %d, flags:", i+1,
		msg[i].m_offset, msg[i].m_lines, msg[i].m_size);
	    if (ison(msg[i].m_flags, UNREAD))
		wprint(" UNREAD");
	    if (ison(msg[i].m_flags, OLD))
		wprint(" OLD");
	    if (ison(msg[i].m_flags, DELETE))
		wprint(" DELETE");
	    if (ison(msg[i].m_flags, PRESERVE))
		wprint(" PRESERVE");
	    if (ison(msg[i].m_flags, UPDATE_STATUS))
		wprint(" UPDATE_STATUS");
	    wprint("\n");
	} else
	    msg[i].m_flags = newflag;
    }
    return 0;
}
