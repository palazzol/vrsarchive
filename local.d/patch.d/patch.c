/* patch - a program to apply diffs to original files
 *
 * $Header: /home/Vince/cvs/local.d/patch.d/patch.c,v 1.1 1986-10-05 12:02:27 root Exp $
 *
 * Copyright 1984, Larry Wall
 *
 * This program may be copied as long as you don't try to make any
 * money off of it, or pretend that you wrote it.
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  84/11/09  17:03:58  lwall
 * Initial revision
 * 
 */

#define DEBUGGING
#undef SHORTNAMES
#ifdef SHORTNAMES
#define another_patch a_patch
#endif

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

/* constants */

#define TRUE (1)
#define FALSE (0)
#define MAXHUNKSIZE 500
#define MAXLINELEN 1024
#define BUFFERSIZE 1024
#define ORIGEXT ".orig"
#define SCCSPREFIX "s."
#define GET "get -e %s"
#define RCSSUFFIX ",v"
#define CHECKOUT "co -l %s"

/* handy definitions */

#define Null(t) ((t)0)
#define Nullch Null(char *)
#define Nullfp Null(FILE *)

#define Ctl(ch) (ch & 037)

#define strNE(s1,s2) (strcmp(s1,s2))
#define strEQ(s1,s2) (!strcmp(s1,s2))
#define strnNE(s1,s2,l) (strncmp(s1,s2,l))
#define strnEQ(s1,s2,l) (!strncmp(s1,s2,l))

/* typedefs */

typedef char bool;
typedef long LINENUM;			/* must be signed */

/* globals */

int Argc;				/* guess */
char **Argv;

struct stat filestat;			/* file statistics area */

char buf[MAXLINELEN];			/* general purpose buffer */
FILE *pfp = Nullfp;			/* patch file pointer */
FILE *ofp = Nullfp;			/* output file pointer */
FILE *rejfp = Nullfp;			/* reject file pointer */

LINENUM input_lines = 0;		/* how long is input file in lines */
LINENUM last_frozen_line = 0;		/* how many input lines have been */
					/* irretractibly output */

#define MAXFILEC 2
int filec = 0;				/* how many file arguments? */
char *filearg[MAXFILEC];

char *outname = Nullch;
char rejname[128];

char *origext = Nullch;

char TMPOUTNAME[] = "/tmp/patchoXXXXXX";
char TMPINNAME[] = "/tmp/patchiXXXXXX";	/* you might want /usr/tmp here */
char TMPREJNAME[] = "/tmp/patchrXXXXXX";
char TMPPATNAME[] = "/tmp/patchpXXXXXX";

LINENUM last_offset = 0;
#ifdef DEBUGGING
int debug = 0;
#endif
bool verbose = TRUE;

#define CONTEXT_DIFF 1
#define NORMAL_DIFF 2
#define ED_DIFF 3
int diff_type = 0;

char *revision = Nullch;		/* prerequisite revision, if any */

/* procedures */

LINENUM locate_hunk();
bool patch_match();
char *malloc();
char *savestr();
char *strcpy();
int my_exit();
bool rev_in_string();
char *fetchname();

/* patch type */

bool another_patch();
bool another_hunk();
char *pfetch();
int patch_line_len();
LINENUM patch_first();
int patch_lines();
char patch_char();
char *pfetch();
char *pgets();

/* input file type */

char *ifetch();

/* apply a context patch to a named file */

main(argc,argv)
int argc;
char **argv;
{
    LINENUM where;
    int hunk = 0;
    int failed = 0;
    int i;

    setbuf(stderr,malloc(BUFSIZ));
    for (i = 0; i<MAXFILEC; i++)
	filearg[i] = Nullch;

    /* parse switches */
    Argc = argc;
    Argv = argv;
    get_some_switches();
    
    /* make sure we clean up /tmp in case of disaster */
    set_signals();

    for (
	open_patch_file(filearg[1]);
	another_patch();
	reinitialize_almost_everything()
    ) {					/* for each patch in patch file */

	if (outname == Nullch)
	    outname = savestr(filearg[0]);
    
	/* initialize the patched file */
	init_output(TMPOUTNAME);
    
	/* for ed script just up and do it and exit */
	if (diff_type == ED_DIFF) {
	    apply_ed_script();
	    continue;
	}
    
	/* initialize reject file */
	init_reject(TMPREJNAME);
    
	/* find out where all the lines are */
	scan_input(filearg[0]);
    
	/* from here on, open no standard i/o files, because malloc */
	/* might misfire */
    
	/* apply each hunk of patch */
	for (hunk = 1; another_hunk(); hunk++) {
	    where = locate_hunk();
	    if (where == Null(LINENUM)) {
		abort_hunk();
		failed++;
		if (verbose)
		    say("Hunk #%d failed.\n",hunk);
	    }
	    else {
		apply_hunk(where);
		if (verbose)
		    if (last_offset)
			say("Hunk #%d succeeded (offset %d line%s).\n",
			  hunk,last_offset,last_offset==1?"":"s");
		    else
			say("Hunk #%d succeeded.\n", hunk);
	    }
	}
    
	assert(hunk);
    
	/* finish spewing out the new file */
	spew_output();
	
	/* and put the output where desired */
	ignore_signals();
	move_file(TMPOUTNAME,outname);
	fclose(rejfp);
	rejfp = Nullfp;
	if (failed) {
	    if (!*rejname) {
		strcpy(rejname, outname);
		strcat(rejname, ".rej");
	    }
	    say("%d out of %d hunks failed--saving rejects to %s\n",
		failed, hunk, rejname);
	    move_file(TMPREJNAME,rejname);
	}
	set_signals();
    }
    my_exit(0);
}

reinitialize_almost_everything()
{
    re_patch();
    re_input();

    input_lines = 0;
    last_frozen_line = 0;

    filec = 0;
    if (filearg[0] != Nullch) {
	free(filearg[0]);
	filearg[0] = Nullch;
    }

    if (outname != Nullch) {
	free(outname);
	outname = Nullch;
    }

    last_offset = 0;

    diff_type = 0;

    if (revision != Nullch) {
	free(revision);
	revision = Nullch;
    }
    get_some_switches();
    if (filec >= 2)
	fatal("You may not change to a different patch file.\n");
}

get_some_switches()
{
    register char *s;
    register int i;

    rejname[0] = '\0';
    if (!Argc)
	return;
    for (Argc--,Argv++; Argc; Argc--,Argv++) {
	s = Argv[0];
	if (strEQ(s,"+")) {
	    return;			/* + will be skipped by for loop */
	}
	if (*s != '-' || !s[1]) {
	    if (filec == MAXFILEC)
		fatal("Too many file arguments.\n");
	    filearg[filec++] = savestr(s);
	}
	else {
	    switch (*++s) {
	    case 'b':
		origext = savestr(Argv[1]);
		Argc--,Argv++;
		break;
	    case 'c':
		diff_type = CONTEXT_DIFF;
		break;
	    case 'd':
		if (chdir(Argv[1]) < 0)
		    fatal("Can't cd to %s.\n",Argv[1]);
		Argc--,Argv++;
		break;
	    case 'e':
		diff_type = ED_DIFF;
		break;
	    case 'n':
		diff_type = NORMAL_DIFF;
		break;
	    case 'o':
		outname = savestr(Argv[1]);
		Argc--,Argv++;
		break;
	    case 'r':
		strcpy(rejname,Argv[1]);
		Argc--,Argv++;
		break;
	    case 's':
		verbose = FALSE;
		break;
#ifdef DEBUGGING
	    case 'x':
		debug = atoi(s+1);
		break;
#endif
	    default:
		fatal("Unrecognized switch: %s\n",Argv[0]);
	    }
	}
    }
}

LINENUM
locate_hunk()
{
    register LINENUM first_guess = patch_first() + last_offset;
    register LINENUM offset;
    LINENUM pat_lines = patch_lines();
    register max_pos_offset = input_lines - first_guess - pat_lines + 1; 
    register max_neg_offset = first_guess - last_frozen_line - 1
				 - patch_context();

    if (!pat_lines)			/* null range matches always */
	return first_guess;
    if (max_neg_offset >= first_guess)	/* do not try lines < 0 */
	max_neg_offset = first_guess - 1;
    if (first_guess <= input_lines && patch_match(first_guess,(LINENUM)0))
	return first_guess;
    for (offset = 1; ; offset++) {
	bool check_after = (offset <= max_pos_offset);
	bool check_before = (offset <= max_pos_offset);

	if (check_after && patch_match(first_guess,offset)) {
#ifdef DEBUGGING
	    if (debug & 1)
		printf("Offset changing from %d to %d\n",last_offset,offset);
#endif
	    last_offset = offset;
	    return first_guess+offset;
	}
	else if (check_before && patch_match(first_guess,-offset)) {
#ifdef DEBUGGING
	    if (debug & 1)
		printf("Offset changing from %d to %d\n",last_offset,-offset);
#endif
	    last_offset = -offset;
	    return first_guess-offset;
	}
	else if (!check_before && !check_after)
	    return Null(LINENUM);
    }
}

/* we did not find the pattern, dump out the hunk so they can handle it */

abort_hunk()
{
    register int i;
    register int pat_last = patch_last();

    fprintf(rejfp,"***************\n");
    for (i=0; i<=pat_last; i++) {
	switch (patch_char(i)) {
	case '*': case '=': case '\n':
	    fprintf(rejfp,"%s", pfetch(i));
	    break;
	case ' ': case '-': case '+': case '!':
	    fprintf(rejfp,"%c %s", patch_char(i), pfetch(i));
	    break;
	default:
	    say("Fatal internal error in abort_hunk().\n"); 
	    abort();
	}
    }
}

/* we found where to apply it (we hope), so do it */

apply_hunk(where)
LINENUM where;
{
    register int old = 1;
    register int lastline = patch_lines();
    register int new = lastline+1;

    where--;
    while (patch_char(new) == '=' || patch_char(new) == '\n')
	new++;
    
    while (old <= lastline) {
	if (patch_char(old) == '-') {
	    copy_till(where + old - 1);
	    last_frozen_line++;
	    old++;
	}
	else if (patch_char(new) == '+') {
	    copy_till(where + old - 1);
	    fputs(pfetch(new),ofp);
	    new++;
	}
	else {
	    if (patch_char(new) != patch_char(old)) {
		say("Out-of-sync patch, lines %d,%d\n",
		    patch_hunk_beg() + old - 1,
		    patch_hunk_beg() + new - 1);
#ifdef DEBUGGING
		printf("oldchar = '%c', newchar = '%c'\n",
		    patch_char(old), patch_char(new));
#endif
		my_exit(1);
	    }
	    if (patch_char(new) == '!') {
		copy_till(where + old - 1);
		while (patch_char(old) == '!') {
		    last_frozen_line++;
		    old++;
		}
		while (patch_char(new) == '!') {
		    fputs(pfetch(new),ofp);
		    new++;
		}
	    }
	    else {
		assert(patch_char(new) == ' ');
		old++;
		new++;
	    }
	}
    }
    while (new <= patch_last() && patch_char(new) == '+') {
	copy_till(where + old - 1);
	fputs(pfetch(new),ofp);
	new++;
    }
}

apply_ed_script()
{
    FILE *pipefp, *popen();
    bool this_line_is_command = FALSE;
    register char *s, *t;
    long beginning_of_this_line;

    unlink(TMPOUTNAME);
    copy_file(filearg[0],TMPOUTNAME);
    sprintf(buf,"/bin/ed %s",TMPOUTNAME);
    pipefp = popen(buf,"w");
    for (;;) {
	beginning_of_this_line = ftell(pfp);
	if (pgets(buf,sizeof buf,pfp) == Nullch) {
	    intuit_at(beginning_of_this_line);
	    break;
	}
	for (t=buf; isdigit(*t); t++) ;
	this_line_is_command = (isdigit(*buf) &&
	  (*t == 'd' || *t == 'c' || *t == 'a' || *t == 'i') );
	if (this_line_is_command) {
	    fputs(buf,pipefp);
	    if (*t != 'd') {
		while (pgets(buf,sizeof buf,pfp) != Nullch) {
		    fputs(buf,pipefp);
		    if (strEQ(buf,".\n"))
			break;
		}
	    }
	}
	else {
	    intuit_at(beginning_of_this_line);
	    break;
	}
    }
    fprintf(pipefp,"w\n");
    fprintf(pipefp,"q\n");
    fflush(pipefp);
    pclose(pipefp);
    ignore_signals();
    move_file(TMPOUTNAME,outname);
    set_signals();
}

init_output(name)
char *name;
{
    mktemp(name);
    ofp = fopen(name,"w");
    if (ofp == Nullfp)
	fatal("patch: can't create %s.\n",name);
}

init_reject(name)
char *name;
{
    mktemp(name);
    rejfp = fopen(name,"w");
    if (rejfp == Nullfp)
	fatal("patch: can't create %s.\n",name);
}

move_file(from,to)
char *to;
{
    char bakname[512];
    register char *s;
    int fromfd;
    register int i;

    /* to stdout? */

    if (strEQ(to,"-")) {
#ifdef DEBUGGING
	if (debug & 4)
	    say("Moving %s to stdout.\n",from);
#endif
	fromfd = open(from,0);
	if (fromfd < 0)
	    fatal("patch: internal error, can't reopen %s\n",from);
	while ((i=read(fromfd,buf,sizeof buf)) > 0)
	    write(1,buf,i);
	close(fromfd);
	return;
    }

    strcpy(bakname,to);
    strcat(bakname,origext?origext:ORIGEXT);
    if (stat(to,&filestat) >= 0) {	/* output file exists */
	dev_t to_device = filestat.st_dev;
	ino_t to_inode  = filestat.st_ino;
	char *simplename = bakname;
	
	for (s=bakname; *s; s++) {
	    if (*s == '/')
		simplename = s+1;
	}
	/* find a backup name that is not the same file */
	while (stat(bakname,&filestat) >= 0 &&
		to_device == filestat.st_dev && to_inode == filestat.st_ino) {
	    for (s=simplename; *s && !islower(*s); s++) ;
	    if (*s)
		*s = toupper(*s);
	    else
		strcpy(simplename, simplename+1);
	}
	while (unlink(bakname) >= 0) ;	/* while() is for benefit of Eunice */
#ifdef DEBUGGING
	if (debug & 4)
	    say("Moving %s to %s.\n",to,bakname);
#endif
	if (link(to,bakname) < 0) {
	    say("patch: can't backup %s, output is in %s\n",
		to,from);
	    return;
	}
	while (unlink(to) >= 0) ;
    }
#ifdef DEBUGGING
    if (debug & 4)
	say("Moving %s to %s.\n",from,to);
#endif
    if (link(from,to) < 0) {		/* different file system? */
	int tofd;
	
	tofd = creat(to,0666);
	if (tofd < 0) {
	    say("patch: can't create %s, output is in %s.\n",
	      to, from);
	    return;
	}
	fromfd = open(from,0);
	if (fromfd < 0)
	    fatal("patch: internal error, can't reopen %s\n",from);
	while ((i=read(fromfd,buf,sizeof buf)) > 0)
	    write(tofd,buf,i);
	close(fromfd);
	close(tofd);
    }
    unlink(from);
}

copy_file(from,to)
{
    int tofd;
    int fromfd;
    register int i;
    
    tofd = creat(to,0666);
    if (tofd < 0)
	fatal("patch: can't create %s.\n", to);
    fromfd = open(from,0);
    if (fromfd < 0)
	fatal("patch: internal error, can't reopen %s\n",from);
    while ((i=read(fromfd,buf,sizeof buf)) > 0)
	write(tofd,buf,i);
    close(fromfd);
    close(tofd);
}

copy_till(lastline)
register LINENUM lastline;
{
    if (last_frozen_line > lastline)
	say("patch: misordered hunks! output will be garbled.\n");
    while (last_frozen_line < lastline) {
	dump_line(++last_frozen_line);
    }
}

spew_output()
{
    copy_till(input_lines);		/* dump remainder of file */
    fclose(ofp);
    ofp = Nullfp;
}

dump_line(line)
LINENUM line;
{
    register char *s;

    for (s=ifetch(line,0); putc(*s,ofp) != '\n'; s++) ;
}

/* does the patch pattern match at line base+offset? */

bool
patch_match(base,offset)
LINENUM base;
LINENUM offset;
{
    register int pline;
    register LINENUM iline;
    register int pat_lines = patch_lines();

    for (pline = 1, iline=base+offset; pline <= pat_lines; pline++,iline++) {
	if (strnNE(ifetch(iline,(offset >= 0)),
		   pfetch(pline),
		   patch_line_len(pline) ))
	    return FALSE;
    }
    return TRUE;
}

/* input file with indexable lines abstract type */

bool using_plan_a = TRUE;
static long i_size;			/* size of the input file */
static char *i_womp;			/* plan a buffer for entire file */
static char **i_ptr;			/* pointers to lines in i_womp */

static int tifd = -1;			/* plan b virtual string array */
static char *tibuf[2];			/* plan b buffers */
static LINENUM tiline[2] = {-1,-1};	/* 1st line in each buffer */
static int lines_per_buf;		/* how many lines per buffer */
static int tireclen;			/* length of records in tmp file */

re_input()
{
    if (using_plan_a) {
	i_size = 0;
	free(i_ptr);
	free(i_womp);
	i_womp = Nullch;
	i_ptr = Null(char **);
    }
    else {
	using_plan_a = TRUE;		/* maybe the next one is smaller */
	close(tifd);
	tifd = -1;
	free(tibuf[0]);
	free(tibuf[1]);
	tibuf[0] = tibuf[1] = Nullch;
	tiline[0] = tiline[1] = -1;
	tireclen = 0;
    }
}

scan_input(filename)
char *filename;
{
    bool plan_a();

    if (!plan_a(filename))
	plan_b(filename);
}

/* try keeping everything in memory */

bool
plan_a(filename)
char *filename;
{
    int ifd;
    register char *s;
    register LINENUM iline;

    if (stat(filename,&filestat) < 0) {
	sprintf(buf,"RCS/%s%s",filename,RCSSUFFIX);
	if (stat(buf,&filestat) >= 0 || stat(buf+4,&filestat) >= 0) {
	    sprintf(buf,CHECKOUT,filename);
	    if (verbose)
		say("Can't find %s--attempting to check it out from RCS.\n",
		    filename);
	    if (system(buf) || stat(filename,&filestat))
		fatal("Can't check out %s.\n",filename);
	}
	else {
	    sprintf(buf,"%s%s",SCCSPREFIX,filename);
	    if (stat(buf,&filestat) >= 0) {
		sprintf(buf,GET,filename);
		if (verbose)
		    say("Can't find %s--attempting to get it from SCCS.\n",
			filename);
		if (system(buf) || stat(filename,&filestat))
		    fatal("Can't get %s.\n",filename);
	    }
	    else
		fatal("Can't find %s.\n",filename);
	}
    }
    i_size = filestat.st_size;
    i_womp = malloc(i_size+2);
    if (i_womp == Nullch)
	return FALSE;
    if ((ifd = open(filename,0)) < 0)
	fatal("Can't open file %s\n",filename);
    if (read(ifd,i_womp,i_size) < i_size)
	fatal("Can't read file %s\n",filename);
    close(ifd);
    if (i_womp[i_size-1] != '\n')
	i_womp[i_size++] = '\n';
    i_womp[i_size] = '\0';

    /* count the lines in the buffer so we know how many pointers we need */

    iline = 0;
    for (s=i_womp; *s; s++) {
	if (*s == '\n')
	    iline++;
    }
    i_ptr = (char **)malloc((iline + 1) * sizeof(char *));
    if (i_ptr == Null(char **)) {	/* shucks, it was a near thing */
	free(i_womp);
	return FALSE;
    }
    
    /* now scan the buffer and build pointer array */

    iline = 1;
    i_ptr[iline] = i_womp;
    for (s=i_womp; *s; s++) {
	if (*s == '\n')
	    i_ptr[++iline] = s+1;	/* these are NOT null terminated */
    }
    input_lines = iline - 1;

    /* now check for revision, if any */

    if (revision != Nullch) { 
	if (!rev_in_string(i_womp)) {
	    ask("This file doesn't appear to be the %s version--patch anyway? [n] ",
		revision);
	    if (*buf != 'y')
		fatal("Aborted.\n");
	}
	else if (verbose)
	    say("Good.  This file appears to be the %s version.\n",
		revision);
    }
    return TRUE;			/* plan a will work */
}

/* keep (virtually) nothing in memory */

plan_b(filename)
char *filename;
{
    FILE *ifp;
    register int i = 0;
    register int maxlen = 1;
    bool found_revision = (revision == Nullch);

    using_plan_a = FALSE;
    if ((ifp = fopen(filename,"r")) == Nullfp)
	fatal("Can't open file %s\n",filename);
    mktemp(TMPINNAME);
    if ((tifd = creat(TMPINNAME,0666)) < 0)
	fatal("Can't open file %s\n",TMPINNAME);
    while (fgets(buf,sizeof buf, ifp) != Nullch) {
	if (revision != Nullch && !found_revision && rev_in_string(buf))
	    found_revision = TRUE;
	if ((i = strlen(buf)) > maxlen)
	    maxlen = i;			/* find longest line */
    }
    if (revision != Nullch) {
	if (!found_revision) {
	    ask("This file doesn't appear to be the %s version--patch anyway? [n] ",
		revision);
	    if (*buf != 'y')
		fatal("Aborted.\n");
	}
	else if (verbose)
	    say("Good.  This file appears to be the %s version.\n",
		revision);
    }
    fseek(ifp,0L,0);			/* rewind file */
    lines_per_buf = BUFFERSIZE / maxlen;
    tireclen = maxlen;
    tibuf[0] = malloc(BUFFERSIZE + 1);
    tibuf[1] = malloc(BUFFERSIZE + 1);
    if (tibuf[1] == Nullch)
	fatal("Can't seem to get enough memory.\n");
    for (i=1; ; i++) {
	if (! (i % lines_per_buf))	/* new block */
	    write(tifd,tibuf[0],BUFFERSIZE);
	if (fgets(tibuf[0] + maxlen * (i%lines_per_buf), maxlen + 1, ifp)
	  == Nullch) {
	    input_lines = i - 1;
	    if (i % lines_per_buf)
		write(tifd,tibuf[0],BUFFERSIZE);
	    break;
	}
    }
    fclose(ifp);
    close(tifd);
    if ((tifd = open(TMPINNAME,0)) < 0) {
	fatal("Can't reopen file %s\n",TMPINNAME);
    }
}

/* fetch a line from the input file, \n terminated, not necessarily \0 */
char *
ifetch(line,whichbuf)
register LINENUM line;
int whichbuf;				/* ignored when file in memory */
{
    if (line < 1 || line > input_lines)
	return "";
    if (using_plan_a)
	return i_ptr[line];
    else {
	int offline = line % lines_per_buf;
	LINENUM baseline = line - offline;

	if (tiline[0] == baseline)
	    whichbuf = 0;
	else if (tiline[1] == baseline)
	    whichbuf = 1;
	else {
	    tiline[whichbuf] = baseline;
	    lseek(tifd,(long)baseline / lines_per_buf * BUFFERSIZE,0);
	    if (read(tifd,tibuf[whichbuf],BUFFERSIZE) < 0)
		fatal("Error reading tmp file %s.\n",TMPINNAME);
	}
	return tibuf[whichbuf] + (tireclen*offline);
    }
}

/* patch abstract type */

static long p_filesize;			/* size of the patch file */
static LINENUM p_first;			/* 1st line number */
static LINENUM p_lines;			/* # lines in pattern */
static LINENUM p_last = -1;		/* last line in hunk */
static LINENUM p_max;			/* max allowed value of p_last */
static LINENUM p_context = 3;		/* # of context lines */
static LINENUM p_input_line = 0;	/* current line # from patch file */
static char *p_line[MAXHUNKSIZE];	/* the text of the hunk */
static char p_char[MAXHUNKSIZE];	/* +, -, and ! */
static int p_len[MAXHUNKSIZE];		/* length of each line */
static int p_indent;			/* indent to patch */
static long p_base;			/* where to intuit this time */
static long p_start;			/* where intuit found a patch */

re_patch()
{
    p_first = (LINENUM)0;
    p_lines = (LINENUM)0;
    p_last = (LINENUM)-1;
    p_max = (LINENUM)0;
    p_indent = 0;
}

open_patch_file(filename)
char *filename;
{
    if (filename == Nullch || !*filename || strEQ(filename,"-")) {
	mktemp(TMPPATNAME);
	pfp = fopen(TMPPATNAME,"w");
	if (pfp == Nullfp)
	    fatal("patch: can't create %s.\n",TMPPATNAME);
	while (fgets(buf,sizeof buf,stdin) != NULL)
	    fputs(buf,pfp);
	fclose(pfp);
	filename = TMPPATNAME;
    }
    pfp = fopen(filename,"r");
    if (pfp == Nullfp)
	fatal("patch file %s not found\n",filename);
    fstat(pfp->_file,&filestat);
    p_filesize = filestat.st_size;
    intuit_at(0L);			/* start at the beginning */
}

bool
another_patch()
{
    bool no_input_file = (filearg[0] == Nullch);
    
    if (p_base != 0L && p_base >= p_filesize) {
	if (verbose)
	    say("done\n");
	return FALSE;
    }
    if (verbose)
	say("Hmm...");
    diff_type = intuit_diff_type();
    if (!diff_type) {
	if (p_base != 0L) {
	    if (verbose)
		say("  Ignoring the trailing garbage.\ndone\n");
	}
	else
	    say("  I can't seem to find a patch in there anywhere.\n");
	return FALSE;
    }
    if (verbose)
	say("  %sooks like %s to me...\n",
	    (p_base == 0L ? "L" : "The next patch l"),
	    diff_type == CONTEXT_DIFF ? "a context diff" :
	    diff_type == NORMAL_DIFF ? "a normal diff" :
	    "an ed script" );
    if (p_indent && verbose)
	say("(Patch is indented %d space%s.)\n",p_indent,p_indent==1?"":"s");
    skip_to(p_start);
    if (verbose)
	say("--------------------------\n");
    if (no_input_file) {
	if (filearg[0] == Nullch) {
	    ask("File to patch: ");
	    filearg[0] = fetchname(buf);
	}
	else if (verbose) {
	    say("Patching file %s...\n",filearg[0]);
	}
    }
    return TRUE;
}

intuit_diff_type()
{
    long this_line = 0;
    long previous_line;
    long first_command_line = -1;
    bool last_line_was_command = FALSE;
    bool this_line_is_command = FALSE;
    register int indent;
    register char *s, *t;
    char *ret;
    char *oldname = Nullch;
    char *newname = Nullch;
    bool no_filearg = (filearg[0] == Nullch);

    fseek(pfp,p_base,0);
    for (;;) {
	previous_line = this_line;
	last_line_was_command = this_line_is_command;
	this_line = ftell(pfp);
	indent = 0;
	if (fgets(buf,sizeof buf,pfp) == Nullch) {
	    if (first_command_line >= 0L) {
					/* nothing but deletes!? */
		p_start = first_command_line;
		return ED_DIFF;
	    }
	    else {
		p_start = this_line;
		return 0;
	    }
	}
	for (s = buf; *s == ' ' || *s == '\t'; s++) {
	    if (*s == '\t')
		indent += 8 - (indent % 8);
	    else
		indent++;
	}
	for (t=s; isdigit(*t); t++) ; 
	this_line_is_command = (isdigit(*s) &&
	  (*t == 'd' || *t == 'c' || *t == 'a' || *t == 'i') );
	if (first_command_line < 0L && this_line_is_command) { 
	    first_command_line = this_line;
	    p_indent = indent;		/* assume this for now */
	}
	if (strnEQ(s,"*** ",4))
	    oldname = fetchname(s+4);
	else if (strnEQ(s,"--- ",4)) {
	    newname = fetchname(s+4);
	    if (no_filearg) {
		if (oldname && newname) {
		    if (strlen(oldname) < strlen(newname))
			filearg[0] = oldname;
		    else
			filearg[0] = newname;
		}
		else if (oldname)
		    filearg[0] = oldname;
		else if (newname)
		    filearg[0] = newname;
	    }
	}
	else if (strnEQ(s,"Index:",6)) {
	    if (no_filearg) 
		filearg[0] = fetchname(s+6);
					/* this filearg might get limboed */
	}
	else if (strnEQ(s,"Prereq:",7)) {
	    for (t=s+7; isspace(*t); t++) ;
	    revision = savestr(t);
	    for (t=revision; *t && !isspace(*t); t++) ;
	    *t = '\0';
	    if (!*revision) {
		free(revision);
		revision = Nullch;
	    }
	}
	if ((!diff_type || diff_type == ED_DIFF) &&
	  first_command_line >= 0L &&
	  strEQ(s,".\n") ) {
	    p_indent = indent;
	    p_start = first_command_line;
	    return ED_DIFF;
	}
	if ((!diff_type || diff_type == CONTEXT_DIFF) &&
		 strnEQ(s,"********",8)) {
	    p_indent = indent;
	    p_start = this_line;
	    return CONTEXT_DIFF;
	}
	if ((!diff_type || diff_type == NORMAL_DIFF) && 
	  last_line_was_command &&
	  (strnEQ(s,"< ",2) || strnEQ(s,"> ",2)) ) {
	    p_start = previous_line;
	    p_indent = indent;
	    return NORMAL_DIFF;
	}
    }
}

char *
fetchname(at)
char *at;
{
    char *s = savestr(at);
    char *name;
    register char *t;
    char *test;
    char tmpbuf[64];

    for (t=s; isspace(*t); t++) ;
    name = t;
    for (; *t && !isspace(*t); t++)
	if (*t == '/')
	    name = t+1;
    *t = '\0';
    name = savestr(name);
    sprintf(tmpbuf,"RCS/%s",name);
    free(s);
    if (stat(name,&filestat) < 0) {
	strcat(tmpbuf,RCSSUFFIX);
	if (stat(tmpbuf,&filestat) < 0 && stat(tmpbuf+4,&filestat) < 0) {
	    sprintf(tmpbuf,"%s%s",SCCSPREFIX,name);
	    if (stat(tmpbuf,&filestat) < 0) {
		free(name);
		name = Nullch;
	    }
	}
    }
    return name;
}

intuit_at(file_pos)
long file_pos;
{
    p_base = file_pos;
}

skip_to(file_pos)
long file_pos;
{
    char *ret;

    assert(p_base <= file_pos);
    if (verbose && p_base < file_pos) {
	fseek(pfp,p_base,0);
	say("The text leading up to this was:\n--------------------------\n");
	while (ftell(pfp) < file_pos) {
	    ret = fgets(buf,sizeof buf,pfp);
	    assert(ret != Nullch);
	    say(buf);
	}
    }
    else
	fseek(pfp,file_pos,0);
}

bool
another_hunk()
{
    register char *s;
    char *ret;
    int context = 0;

    while (p_last >= 0) {
	free(p_line[p_last--]);
    }
    assert(p_last == -1);

    p_max = MAXHUNKSIZE;		/* gets reduced when --- found */
    if (diff_type == CONTEXT_DIFF) {
	long line_beginning = ftell(pfp);

	ret = pgets(buf,sizeof buf, pfp);
	if (ret == Nullch || strnNE(buf,"********",8)) {
	    intuit_at(line_beginning);
	    return FALSE;
	}
	p_context = 100;
	while (p_last < p_max) {
	    ret = pgets(buf,sizeof buf, pfp);
	    if (ret == Nullch) {
		if (p_max - p_last < 4)
		    strcpy(buf,"  \n");	/* assume blank lines got chopped */
		else
		    fatal("Unexpected end of file in patch.\n");
	    }
	    p_input_line++;
	    if (strnEQ(buf,"********",8))
		fatal("Unexpected end of hunk at line %d.\n",
		    p_input_line);
	    p_char[++p_last] = *buf;
	    switch (*buf) {
	    case '*':
		if (p_last != 0)
		    fatal("Unexpected *** at line %d: %s", p_input_line, buf);
		context = 0;
		p_line[p_last] = savestr(buf);
		for (s=buf; *s && !isdigit(*s); s++) ;
		p_first = (LINENUM) atol(s);
		while (isdigit(*s)) s++;
		for (; *s && !isdigit(*s); s++) ;
		p_lines = ((LINENUM)atol(s)) - p_first + 1;
		break;
	    case '-':
		if (buf[1] == '-') {
		    LINENUM tmp_first;

		    if (p_last != p_lines + 1 && p_last != p_lines + 2)
			fatal("Unexpected --- at line %d: %s",
			    p_input_line,buf);
		    context = 0;
		    p_line[p_last] = savestr(buf);
		    p_char[p_last] = '=';
		    for (s=buf; *s && !isdigit(*s); s++) ;
		    tmp_first = (LINENUM) atol(s);
		    while (isdigit(*s)) s++;
		    for (; *s && !isdigit(*s); s++) ;
		    p_max = ((LINENUM)atol(s)) - tmp_first + 1 + p_last;
		    break;
		}
		/* FALL THROUGH */
	    case '+': case '!':
		if (context > 0) {
		    if (context < p_context)
			p_context = context;
		    context = -100;
		}
		p_line[p_last] = savestr(buf+2);
		break;
	    case '\t': case '\n':	/* assume the 2 spaces got eaten */
		p_line[p_last] = savestr(buf);
		if (p_last != p_lines + 1) {
		    context++;
		    p_char[p_last] = ' ';
		}
		break;
	    case ' ':
		context++;
		p_line[p_last] = savestr(buf+2);
		break;
	    default:
		fatal("Malformed patch at line %d: %s",p_input_line,buf);
	    }
	    p_len[p_last] = strlen(p_line[p_last]);
					/* for strncmp() so we do not have */
					/* to assume null termination */
	}
	if (p_last >=0 && !p_lines)
	    fatal("No --- found in patch at line %d\n", patch_hunk_beg());
    }
    else {				/* normal diff--fake it up */
	char hunk_type;
	register int i;
	LINENUM min, max;
	long line_beginning = ftell(pfp);

	p_context = 0;
	ret = pgets(buf,sizeof buf, pfp);
	p_input_line++;
	if (ret == Nullch || !isdigit(*buf)) {
	    intuit_at(line_beginning);
	    return FALSE;
	}
	p_first = (LINENUM)atol(buf);
	for (s=buf; isdigit(*s); s++) ;
	if (*s == ',') {
	    p_lines = (LINENUM)atol(++s) - p_first + 1;
	    while (isdigit(*s)) s++;
	}
	else
	    p_lines = (*s != 'a');
	hunk_type = *s;
	if (hunk_type == 'a')
	    p_first++;			/* do append rather than insert */
	if (hunk_type == 'd') {
	    p_last = p_lines + 1;
	    min = 1;
	    max = 0;
	}
	else {
	    min = (LINENUM)atol(++s);
	    for (; isdigit(*s); s++) ;
	    if (*s == ',')
		max = (LINENUM)atol(++s);
	    else
		max = min;
	    p_last = p_lines + 1 + max - min + 1;
	}
	sprintf(buf,"*** %d,%d\n", p_first, p_first + p_lines - 1);
	p_line[0] = savestr(buf);
	p_char[0] = '*';
	for (i=1; i<=p_lines; i++) {
	    ret = pgets(buf,sizeof buf, pfp);
	    p_input_line++;
	    if (ret == Nullch)
		fatal("Unexpected end of file in patch at line %d.\n",
		  p_input_line);
	    if (*buf != '<')
		fatal("< expected at line %d of patch.\n", p_input_line);
	    p_line[i] = savestr(buf+2);
	    p_len[i] = strlen(p_line[i]);
	    p_char[i] = '-';
	}
	if (hunk_type == 'c') {
	    ret = pgets(buf,sizeof buf, pfp);
	    p_input_line++;
	    if (ret == Nullch)
		fatal("Unexpected end of file in patch at line %d.\n",
		    p_input_line);
	    if (*buf != '-')
		fatal("--- expected at line %d of patch.\n", p_input_line);
	}
	sprintf(buf,"--- %d,%d\n",min,max);
	p_line[i] = savestr(buf);
	p_char[i] = '=';
	for (i++; i<=p_last; i++) {
	    ret = pgets(buf,sizeof buf, pfp);
	    p_input_line++;
	    if (ret == Nullch)
		fatal("Unexpected end of file in patch at line %d.\n",
		    p_input_line);
	    if (*buf != '>')
		fatal("> expected at line %d of patch.\n", p_input_line);
	    p_line[i] = savestr(buf+2);
	    p_len[i] = strlen(p_line[i]);
	    p_char[i] = '+';
	}
    }
#ifdef DEBUGGING
    if (debug & 2) {
	int i;
	char special;

	for (i=0; i <= p_last; i++) {
	    if (i == p_lines)
		special = '^';
	    else
		special = ' ';
	    printf("%3d %c %c %s",i,p_char[i],special,p_line[i]);
	}
    }
#endif
    return TRUE;
}

char *
pgets(bf,sz,fp)
char *bf;
int sz;
FILE *fp;
{
    char *ret = fgets(bf,sz,fp);
    register char *s;
    register int indent = 0;

    if (p_indent && ret != Nullch) {
	for (s=buf; indent < p_indent && (*s == ' ' || *s == '\t'); s++) {
	    if (*s == '\t')
		indent += 8 - (indent % 7);
	    else
		indent++;
	}
	if (buf != s)
	    strcpy(buf,s);
    }
    return ret;
}

LINENUM
patch_first()
{
    return p_first;
}

patch_lines()
{
    return p_lines;
}

patch_last()
{
    return p_last;
}

patch_context()
{
    return p_context;
}

patch_line_len(line)
LINENUM line;
{
    return p_len[line];
}

char
patch_char(line)
LINENUM line;
{
    return p_char[line];
}

char *
pfetch(line)
LINENUM line;
{
    return p_line[line];
}

patch_hunk_beg()
{
    return p_input_line - p_last - 1;
}

char *
savestr(string)
char *string;
{
    return strcpy(malloc(strlen(string)+1),string);
}

my_exit(status)
int status;
{
    unlink(TMPINNAME);
    unlink(TMPOUTNAME);
    unlink(TMPREJNAME);
    unlink(TMPPATNAME);
    exit(status);
}

say(pat,arg1,arg2,arg3)
char *pat;
int arg1,arg2,arg3;
{
    fprintf(stderr,pat,arg1,arg2,arg3);
    fflush(stderr);
}

fatal(pat,arg1,arg2,arg3)
char *pat;
int arg1,arg2,arg3;
{
    say(pat,arg1,arg2,arg3);
    my_exit(1);
}

ask(pat,arg1,arg2,arg3)
char *pat;
int arg1,arg2,arg3;
{
    int ttyfd = open("/dev/tty",2);

    say(pat,arg1,arg2,arg3);
    read((ttyfd < 0 ? 2 : ttyfd), buf, sizeof buf);
    close(ttyfd);
}

bool
rev_in_string(string)
char *string;
{
    register char *s;
    register int patlen;

    if (revision == Nullch)
	return TRUE;
    patlen = strlen(revision);
    for (s = string; *s; s++) {
	if (isspace(*s) && strnEQ(s+1,revision,patlen) && 
		isspace(s[patlen+1] )) {
	    return TRUE;
	}
    }
    return FALSE;
}

set_signals()
{
    signal(SIGHUP,my_exit);
    signal(SIGINT,my_exit);
}

ignore_signals()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
}
