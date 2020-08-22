/*	SC	A Spreadsheet Calculator
 *		Main driver
 *
 *		original by James Gosling, September 1982
 *		modifications by Mark Weiser and Bruce Israel,
 *			University of Maryland
 *
 *              More mods Robert Bond, 12/86
 *
 */


#include <signal.h>
#include <curses.h>

#ifdef BSD42
#include <strings.h>
#else
#ifndef SYSIII
#include <string.h>
#endif
#endif

#include <stdio.h>
#include "sc.h"

char *getenv();

#ifdef SYSV3
void exit();
#endif

/* default column width */

#define DEFWIDTH 10
#define DEFPREC   2

#define MAXCMD 160	/* for ! command below */

/* Globals defined in sc.h */

struct ent *tbl[MAXROWS][MAXCOLS];
int strow, stcol;
int currow, curcol;
int savedrow, savedcol;
int FullUpdate;
int maxrow, maxcol;
int fwidth[MAXCOLS];
int precision[MAXCOLS];
char col_hidden[MAXCOLS];
char row_hidden[MAXROWS];
char line[1000];
int changed;
struct ent *to_fix;
int modflg;
int numeric;
char *mdir;
int showsc, showsr;	/* Starting cell for highlighted range */

char curfile[1024];
char    revmsg[80];

int  linelim = -1;
int  showme = 1;	/* 1 to display the current cell in the top line */
int  showrange;		/* Causes ranges to be highlighted */
int  lastmx, lastmy;	/* Screen address of the cursor */
int  lastcol;		/* Spreadsheet Column the cursor was in last */
char under_cursor[] = " "; /* Data under the < cursor */
char *rev = "$Revision: 1.7 $";

int seenerr;

yyerror (err)
char *err; {
    if (seenerr) return;
    seenerr++;
    (void) move (1,0);
    (void) clrtoeol ();
    (void) printw ("%s: %.*s<=%s",err,linelim,line,line+linelim);
}

struct ent *
lookat(row,col){
    register struct ent **p;
    if (row < 0)
	row = 0;
    else if (row > MAXROWS-1) 
	row = MAXROWS-1;
    if (col < 0) 
	col = 0;
    else if (col > MAXCOLS-1)
	col = MAXCOLS-1;
    p = &tbl[row][col];
    if (*p==0) {
	*p = (struct ent *) xmalloc ((unsigned)sizeof (struct ent));
	if (row>maxrow) maxrow = row;
	if (col>maxcol) maxcol = col;
	(*p)->label = 0;
	(*p)->flags = 0;
	(*p)->row = row;
	(*p)->col = col;
	(*p)->expr = 0;
	(*p)->v = (double) 0.0;
    }
    return *p;
}

/*
 * This structure is used to keep ent structs around before they
 * are deleted to allow the sync_refs routine a chance to fix the
 * variable references.
 * We also use it as a last-deleted buffer for the 'p' command.
 */

free_ent(p)
register struct ent *p;
{
    p->next = to_fix;
    to_fix = p;
    p->flags |= is_deleted;
}

flush_saved()
{
    register struct ent *p;
    register struct ent *q;

    if (!(p = to_fix))
	return;
    while (p) {
	(void) clearent(p);
	q = p->next;
	xfree((char *)p);
	p = q;
    }
    to_fix = 0;
}

update () {
    register    row,
                col;
    register struct ent **p;
    int     mxcol;
    int     mxrow;
    int     rows;
    int     cols;
    int     minsr, minsc, maxsr, maxsc;
    register r;
    register i;

    while (row_hidden[currow])   /* You can't hide the last row or col */
	currow++;
    while (col_hidden[curcol])
	curcol++;
    /* First see if the last display still covers curcol */
    if (stcol <= curcol) { 
	for (i = stcol, cols = 0, col = RESCOL;
			(col + fwidth[i]) < COLS-1 && i < MAXCOLS; i++) {
	    cols++;
	    if (col_hidden[i])
		continue;
	    col += fwidth[i];
	}
    }
    while (stcol + cols - 1 < curcol || curcol < stcol) {
	FullUpdate++;
	if (stcol - 1 == curcol) {    /* How about back one? */
	    stcol--;
	} else if (stcol + cols == curcol) {   /* Forward one? */
	    stcol++;
	} else {
	    /* Try to put the cursor in the center of the screen */
	    col = (COLS - RESCOL - fwidth[curcol]) / 2 + RESCOL; 
	    stcol = curcol;
	    for (i=curcol-1; i >= 0 && col-fwidth[i] > RESCOL; i--) {
		stcol--;
		if (col_hidden[i])
		    continue;
		col -= fwidth[i];
	    }
	}
	/* Now pick up the counts again */
	for (i = stcol, cols = 0, col = RESCOL;
			(col + fwidth[i]) < COLS-1 && i < MAXCOLS; i++) {
	    cols++;
	    if (col_hidden[i])
		continue;
	    col += fwidth[i];
	}
    }
    /* Now - same process on the rows */
    if (strow <= currow) { 
	for (i = strow, rows = 0, row=RESROW; row<LINES && i<MAXROWS; i++) {
	    rows++;
	    if (row_hidden[i])
		continue;
	    row++;
	}
    }
    while (strow + rows - 1 < currow || currow < strow) {
	FullUpdate++;
	if (strow - 1 == currow) {    /* How about up one? */
	    strow--;
	} else if (strow + rows == currow) {   /* Down one? */
	    strow++;
	} else {
	    /* Try to put the cursor in the center of the screen */
	    row = (LINES - RESROW) / 2 + RESROW; 
	    strow = currow;
	    for (i=currow-1; i >= 0 && row-1 > RESROW; i--) {
		strow--;
		if (row_hidden[i])
		    continue;
		row--;
	    }
	}
	/* Now pick up the counts again */
	for (i = strow, rows = 0, row=RESROW; row<LINES && i<MAXROWS; i++) {
	    rows++;
	    if (row_hidden[i])
		continue;
	    row++;
	}
    }
    mxcol = stcol + cols - 1;
    mxrow = strow + rows - 1;
    if (FullUpdate) {
	(void) move (2, 0);
	(void) clrtobot ();
	(void) standout();
	for (row=RESROW, i=strow; i <= mxrow; i++) {
	    if (row_hidden[i]) 
		continue;
	    (void) move(row,0);
#if MAXROW < 1000
	    (void) printw("%-*d", RESCOL-1, i);
#else
	    (void) printw("%-*d", RESCOL, i);
#endif
	    row++;
	}
	(void) move (2,0);
	(void) printw("%*s", RESCOL, " ");
	for (col=RESCOL, i = stcol; i <= mxcol; i++) {
	    register int k;
	    if (col_hidden[i])
		continue;
	    (void) move(2, col);
	    k = fwidth[i]/2;
	    if (k == 0)
		(void) printw("%1s", coltoa(i));
	    else
	        (void) printw("%*s%-*s", k, " ", fwidth[i]-k, coltoa(i));
	    col += fwidth[i];
	}
	(void) standend();
    }

    /* Get rid of the cursor standout */
    (void) move(lastmx, lastmy);
    if (showme)
        repaint(lastmx, lastmy, fwidth[lastcol]);

    if (showrange && showme) {
	minsr = showsr < currow ? showsr : currow;
	minsc = showsc < curcol ? showsc : curcol;
	maxsr = showsr > currow ? showsr : currow;
	maxsc = showsc > curcol ? showsc : curcol;
	(void) move(1,0);
	(void) clrtoeol();
        (void) printw("Default range - %s", r_name(minsr, minsc, maxsr, maxsc));
    }

    /* Repaint the visible screen */
    for (row = strow, r = RESROW; row <= mxrow; row++) {
	register c = RESCOL;
	int do_stand = 0;
	int fieldlen;
	int nextcol;

	if (row_hidden[row])
	    continue;
	for (p = &tbl[row][col = stcol]; col <= mxcol;
	         p += nextcol - col,  col = nextcol, c += fieldlen) {

	    nextcol = col+1;
	    if (col_hidden[col]) {
		fieldlen = 0;
		continue;
	    }

	    fieldlen = fwidth[col];
	    if (showrange && showme && (row >= minsr) && (row <= maxsr) &&
			       (col >= minsc) && (col <= maxsc)) {
		do_stand = 1;
	    }
	    if (*p && ((*p) -> flags & is_changed || FullUpdate) || do_stand) {
		char   *s;
		(void) move (r, c);
		if (!*p)
		    *p = lookat(row, col);
		if (do_stand) {
		    (void) standout();
		    (*p) -> flags |= is_changed; 
		} else {
		    (*p) -> flags &= ~is_changed;
		}
		if ((*p) -> flags & is_valid) {
		    char field[1024];
		    (void)sprintf(field,"%*.*f", fwidth[col],
					         precision[col], (*p)->v);
		    if(strlen(field) > (unsigned)fwidth[col]) {
			for(i = 0; i<fwidth[col]; i++)
			    (void)addch('*');
		    } else {
			(void)addstr(field);
		    }
		}
		if (s = (*p) -> label) {
		    char field[1024];
		    int  slen;
		    char *start, *last;
		    register char *fp;
		    struct ent *nc;

		    /* This figures out if the label is allowed to
		       slop over into the next blank field */
		    slen = strlen(s);
		    while(slen>fieldlen && nextcol<=mxcol &&
			   !((nc = lookat(row,nextcol))->flags & is_valid) &&
			   !(nc->label)) {

			if (!col_hidden[nextcol])
			    fieldlen += fwidth[nextcol];

		        nextcol++;
		    }
		    if (slen > fieldlen)
			slen = fieldlen;

		    /* Now justify and print */
		    start = (*p)->flags & is_leftflush ? field 
		    			: field + fieldlen - slen;
		    last = field+fieldlen;
		    fp = field;
		    while (fp < start)
			*fp++ = ' ';
		    while (slen--)
			*fp++ = *s++;
		    if (!((*p)->flags & is_valid) || fieldlen != fwidth[col]) 
		        while (fp < last)
			    *fp++ = ' ';
		    *fp = 0;
		    (void) mvaddstr(r, c, field);
		}
		if (!((*p)->flags & is_valid) && !(*p)->label) {
		    /* Need to repaint a blank cell */
		    (void) printw ("%*s", fwidth[col], " ");
		}
		if (do_stand) {
		    (void) standend();
		    do_stand = 0;
		}
	    }
	}
	r++;
    }
	    
    (void) move(lastmy, lastmx+fwidth[lastcol]);
    if((inch() & 0x7f) == '<')
        (void) addstr(under_cursor);
    lastmy =  RESROW;
    for (row = strow; row < currow; row++)
	if (!row_hidden[row])
		lastmy += 1;
    lastmx = RESCOL;
    for (col = stcol; col < curcol; col++)
	if (!col_hidden[col])
		lastmx += fwidth[col];
    lastcol = curcol;
    (void) move(lastmx, lastmy);
    if (showme) {
        (void) standout();
        repaint(lastmx, lastmy, fwidth[lastcol]);
        (void) standend();
    }
    (void) move(lastmy, lastmx+fwidth[lastcol]);
    *under_cursor = (inch() & 0x7f);
    (void) addstr("<");

    (void) move (0, 0);
    (void) clrtoeol ();
    if (linelim >= 0) {
	(void) addstr (">> ");
	(void) addstr (line);
    } else {
	if (showme) {
	    register struct ent *p1;

            (void) printw("%s%d", coltoa(curcol), currow);
	    p1 = tbl[currow][curcol];
	    if (p1 && ((p1->flags & is_valid) || p1->label)) {
		if (p1->expr) {
		    linelim = 0;
		    editexp(currow, curcol);
		} else if (p1->label) {
		    (void) sprintf(line, "%s", p1->label);
		} else {
		    (void) sprintf(line, "%.15g", p1->v);
		}
		(void) addstr("[");
		(void) addstr (line);
		(void) addstr("]");
		linelim = -1;
	    } else {
		(void) addstr("[]");
	    }
	}
	(void) move (lastmy, lastmx + fwidth[lastcol]);
    }
    if (revmsg[0]) {
	(void) move(0, 0);
	(void) printw(revmsg);
	revmsg[0] = 0;
	(void) move (lastmy, lastmx + fwidth[lastcol]);
    }
    FullUpdate = 0;
}

repaint(x, y, len)
{
    char buf[] = " ";

    while(len-- > 0) {
	(void) move(y,x);
	*buf = inch() & 0x7f;
	(void) addstr(buf);
	x++;
    }
}

main (argc, argv)
char  **argv; {
    int     inloop = 1;
    register int   c;
    int     edistate = -1;
    int     arg = 1;
    int     narg;
    int     nedistate;
    int	    running;
    char    *revi;
    char    *pname;

    pname = argv[0];
    while (argc > 1 && argv[1][0] == '-') {
	argv++;
	argc--;
    	switch (argv[0][1]) {
	    case 'x':
		    Crypt = 1;
		    break;
	    case 'n':
		    numeric = 1;
		    break;
	    default:
		    (void) fprintf(stderr,"%s: unrecognized flag: %c\n",
			pname,argv[0][1]);
		    exit(1);
	}
    }

    {
	register    i;
	for (i = 0; i < MAXCOLS; i++) {
	    fwidth[i] = DEFWIDTH;
	    precision[i] = DEFPREC;
	}
    }
    curfile[0]=0;

    signals();
    (void) initscr();
    (void) clear();
    nonl();
    noecho ();
    cbreak();
    initkbd();
    (void) strcpy(revmsg, pname);
    for (revi=rev; *revi++ != ':';);
    (void) strcat(revmsg, revi);
    revi = revmsg+strlen(revmsg);
    *--revi = 0;
    (void) strcat(revmsg,"Type '?' for help.");
    if (argc > 1) {
	(void) strcpy(curfile,argv[1]);
	readfile (argv[1], 0);
    }
    modflg = 0;
#ifdef VENIX
    setbuf (stdin, NULL);
#endif
    FullUpdate++;
    while (inloop) { running = 1;
    while (running) {
	nedistate = -1;
	narg = 1;
	if (edistate < 0 && linelim < 0 && (changed || FullUpdate))
	    EvalAll (), changed = 0;
	update();
	(void) refresh(); /* 5.3 does a refresh in getch */ 
	c = nmgetch();
	(void) move (1, 0);
	(void) clrtoeol ();
	(void) fflush (stdout);
	seenerr = 0;
	if ((c < ' ') || ( c == 0177 ))
	    switch (c) {
#if defined(BSD42) || defined (BSD43)
		case ctl('z'):
		    deraw();
#ifndef V7
		    (void) kill(getpid(),SIGTSTP);
#endif

		    /* the pc stops here */

		    goraw();
		    break;
#endif
		case ctl('r'):
		case ctl('l'):
		    FullUpdate++;
		    (void) clearok(stdscr,1);
		    break;
		default:
		    error ("No such command  (^%c)", c + 0100);
		    break;
		case ctl('b'):
		    while (--arg>=0) {
			if (curcol)
			    curcol--;
			else
			    error ("At column A");
			while(col_hidden[curcol] && curcol)
			    curcol--;
		    }
		    break;
		case ctl('c'):
		    running = 0;
		    break;
		case ctl('e'):
		    switch (nmgetch()) {
		    case 'j':
		    case ctl('n'):
			doend(1,0);
			break;
		    case 'k':
		    case ctl('p'):
			doend(-1,0);
			break;
		    case 'h':
		    case ctl('b'):
			doend(0,-1);
			break;
		    case 'l':
		    case ctl('f'):
			doend(0,1);
			break;
		    default:
			error("Invalid end command");
			break;
		    }
		    break;
		case ctl('f'):
		    while (--arg>=0) {
			if (curcol < MAXCOLS - 1)
			    curcol++;
			else
			    error ("The table can't be any wider");
			while(col_hidden[curcol]&&(curcol<MAXCOLS-1))
			    curcol++;
		    }
		    break;
		case ctl('g'):
		case ctl('['):
		    showrange = 0;
		    linelim = -1;
		    (void) move (1, 0);
		    (void) clrtoeol ();
		    break;
		case 0177:
		case ctl('h'):
		    while (--arg>=0) if (linelim > 0)
			line[--linelim] = 0;
		    break;
		case ctl('i'): 		/* tab */
		    if (linelim > 0) {
			if (!showrange) {
			    startshow();
			} else {
			    showdr();
			    linelim = strlen(line);
			    line[linelim++] = ' ';
			    line[linelim] = 0;
			    showrange = 0;
			}
			linelim = strlen (line);
		    }
		    break;
		case ctl('m'):
		case ctl('j'):
		    showrange = 0;
		    if (linelim < 0)
			line[linelim = 0] = 0;
		    else {
			linelim = 0;
			(void) yyparse ();
			linelim = -1;
		    }
		    break;
		case ctl('n'):
		    while (--arg>=0) {
			if (currow < MAXROWS - 1)
			    currow++;
			else
			    error ("The table can't be any longer");
			while (row_hidden[currow] && (currow < MAXROWS - 1))
			    currow++;
		    }
		    break;
		case ctl('p'):
		    while (--arg>=0) {
			if (currow)
			    currow--;
			else
			    error ("At row zero");
			while (row_hidden[currow] && currow)
			    currow--;
		    }
		    break;
		case ctl('q'):
		    break;	/* ignore flow control */
		case ctl('s'):
		    break;	/* ignore flow control */
		case ctl('t'):
		    error("Toggle - n:numeric  t:top row  x:encryption");
		    (void) refresh();
		    switch (nmgetch()) {
			case 't': case 'T':
			    showme ^= 1;
			    repaint(lastmx, lastmy, fwidth[lastcol]);
			    break;
			case 'n': case 'N':
			    numeric ^= 1;
			    error("Numeric input %sabled.",numeric? "en":"dis");
			    break;
			case 'x': case 'X':
			    Crypt ^= 1;
			    error("Encryption %sabled.",Crypt? "en":"dis");
			    break;
			default:
			    error("Bad toggle switch");
		    }
		    break;
		case ctl('u'):
		    narg = arg * 4;
		    nedistate = 1;
		    break;
		case ctl('v'):	/* insert variable name */
		    if (linelim > 0) {
		    (void) sprintf (line+linelim,"%s", v_name(currow, curcol));
			linelim = strlen (line);
		    }
		    break;
		case ctl('w'):	/* insert variable expression */
		    if (linelim > 0) editexp(currow,curcol);
		    break;
		case ctl('a'):	/* insert variable value */
		    if (linelim > 0) {
			struct ent *p = tbl[currow][curcol];

			if (p && p -> flags & is_valid) {
			    (void) sprintf (line + linelim, "%.*f",
					precision[curcol],p -> v);
			    linelim = strlen (line);
			}
		    }
		    break;
		}
	else
	    if ('0' <= c && c <= '9' && ( (numeric && edistate >= 0) ||
			(!numeric && (linelim < 0 || edistate >= 0))))
	    {
		if (edistate != 0) {
		    if (c == '0')      /* just a '0' goes to left col */
			curcol = 0;
		    else {
		        nedistate = 0;
		        narg = c - '0';
		    }
		} else {
		    nedistate = 0;
		    narg = arg * 10 + (c - '0');
		}
	    }
	    else
		if (linelim >= 0) {     /* Editing line */
		    switch(c) {
			case ')':
			    if (showrange) {
				showdr();
			        showrange = 0;
			        linelim = strlen (line);
			    }
			    break;
		       default:
			    break;
		    }
		    line[linelim++] = c;
		    line[linelim] = 0;
		}
		else
		    switch (c) {
			case ':':
			    break;	/* Be nice to vi users */

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case '-': case '.': case '+':
			    (void) sprintf(line,"let %s = %c",
					v_name(currow, curcol), c);
			    linelim = strlen (line);
			    break;

			case '=':
			    (void) sprintf(line,"let %s = ",
						v_name(currow, curcol));
			    linelim = strlen (line);
			    break;

			case '!':
			    {
			    /*
			     *  "! command"  executes command
			     *  "!"	forks a shell
			     *  "!!" repeats last command
			     */
			    char *shl;
			    int pid, temp;
			    char cmd[MAXCMD];
			    static char lastcmd[MAXCMD];

			    if (!(shl = getenv("SHELL")))
				shl = "/bin/sh";

			    deraw();
			    (void) fputs("! ", stdout);
			    (void) fflush(stdout);
			    (void) fgets(cmd, MAXCMD, stdin);
			    cmd[strlen(cmd) - 1] = '\0';	/* clobber \n */
			    if(strcmp(cmd,"!") == 0)		/* repeat? */
				    (void) strcpy(cmd, lastcmd);
			    else
				    (void) strcpy(lastcmd, cmd);

			    if (!(pid = fork()))
			    {
			        if(strlen(cmd))
					(void)execl(shl,shl,"-c",cmd,(char *)0);
				else
					(void) execl(shl, shl, (char *)0);
				exit(-127);
			    }

			    while (pid != wait(&temp));

			    (void) printf("Press <return> to continue\n");
			    (void)nmgetch();
			    goraw();
			    break;
			    }

			case '/':
			    error("c:copy x:erase v:value f:fill d:define u:undefine s:show");
			    (void) refresh();
			    switch (nmgetch()) {
			    case 'c':
				(void) sprintf(line,"copy [dest_range src_range] ");
				linelim = strlen(line);
				startshow();
				break;
			    case 'x':
				(void) sprintf(line,"erase [range] ");
				linelim = strlen(line);
				startshow();
				break;
			    case 'v':
				(void) sprintf(line, "value [range] ");
				linelim = strlen(line);
				startshow();
				break;
			    case 'f':
				(void) sprintf(line,"fill [range start inc] ");
				linelim = strlen(line);
				startshow();
				break;
			    case 'd':
				(void) sprintf(line,"define [string range] \"");
				linelim = strlen(line);
				startshow();
				modflg++;
				break;
			    case 'u':
				(void) sprintf(line,"undefine [range] ");
				linelim = strlen(line);
				modflg++;
				break;
			    case 's':
				{
				FILE *f;
				int pid;
				f = openout("| sort | less", &pid);
				if (!f) {
				    error("Cant open pipe to sort");
				    break;
				}
				list_range(f);
				closeout(f, pid);
				break;
				}
			   default:
				error("Invalid region operation");
			    }
			    break;
			case '$':
			    {
			    register struct ent *p;

			    curcol = MAXCOLS - 1;
			    while (!VALID_CELL(p, currow, curcol) && curcol > 0)
				curcol--;
			    break;
			    }
			case '#':
			    {
			    register struct ent *p;

			    currow = MAXROWS - 1;
			    while (!VALID_CELL(p, currow, curcol) && currow > 0)
				currow--;
			    break;
			    }
			case 'w':
			    {
			    register struct ent *p;

			    while (--arg>=0) {
				do {
				    if (curcol < MAXCOLS - 1)
					curcol++;
				    else {
					if (currow < MAXROWS - 1) {
					    while(++currow < MAXROWS - 1 &&
					            row_hidden[currow]) /* */;
					    curcol = 0;
					} else {
					    error("End of the table");
					    break;
					}
				    }
				} while(col_hidden[curcol] ||
					!VALID_CELL(p, currow, curcol));
			    }
			    break;
			    }
			case 'b':
			    {
			    register struct ent *p;

			    while (--arg>=0) {
				do {
				    if (curcol) 
					curcol--;
				    else {
					if (currow) {
					    while(--currow &&
						row_hidden[currow]) /* */;
					    curcol = MAXCOLS - 1;
					} else {
					    error ("At start of table");
					    break;
					}
				    }
				} while(col_hidden[curcol] ||
					!VALID_CELL(p, currow, curcol));
			    }
			    break;
			    }
			case '^':
			    currow = 0;
			    break;
			case '?':
			    help ();
			    break;
			case '"':
			    (void) sprintf (line, "label %s = \"",
						v_name(currow, curcol));
			    linelim = strlen (line);
			    break;
			case '<':
			    (void) sprintf (line, "leftstring %s = \"",
				    v_name(currow, curcol));
			    linelim = strlen (line);
			    break;
			case '>':
			    (void) sprintf (line, "rightstring %s = \"",
				   v_name(currow, curcol));
			    linelim = strlen (line);
			    break;
			case 'e':
			    editv (currow, curcol);
			    break;
			case 'E':
			    edits (currow, curcol);
			    break;
			case 'f':
			    if (arg == 1)
			        (void) sprintf (line, "format [for column] %s ",
					coltoa(curcol));
			    else {
				(void) sprintf(line, "format [for columns] %s:",
					coltoa(curcol));
				(void) sprintf(line+strlen(line), "%s ",
					coltoa(curcol+arg-1));
			    }
			    error("Current format is %d %d",
					fwidth[curcol],precision[curcol]);
			    linelim = strlen (line);
			    break;
			case 'g':
			    (void) sprintf (line, "goto [v] ");
			    linelim = strlen (line);
			    break;
			case 'P':
			    (void) sprintf (line, "put [\"dest\" range] \"");
			    if (*curfile)
			        error("Default path is '%s'",curfile);
			    linelim = strlen (line);
			    break;
			case 'M':
			    (void) sprintf (line, "merge [\"source\"] \"");
			    linelim = strlen (line);
			    break;
			case 'R':
			    (void) sprintf (line,"merge [\"macro_file\"] \"%s/", mdir);
			    linelim = strlen (line);
			    break;
			case 'D':
			    (void) sprintf (line, "mdir [\"macro_directory\"] \"");
			    linelim = strlen (line);
			    break;
			case 'G':
			    (void) sprintf (line, "get [\"source\"] \"");
			    if (*curfile)
			        error("Default file is '%s'",curfile);
			    linelim = strlen (line);
			    break;
			case 'W':
			    (void) sprintf (line, "write [\"dest\" range] \"");
			    linelim = strlen (line);
			    break;
			case 'T':	/* tbl output */
			    (void) sprintf (line, "tbl [\"dest\" range] \"");
			    linelim = strlen (line);
			    break;
			case 'i':
			    switch (get_qual()) {
			    case 'r':
				insertrow(arg);
				break;
			    case 'c':
				insertcol(arg);
				break;
			    default:
				error("Invalid insert command");
				break;
			    }
			    break;
			case 'd':
			    switch (get_qual()) {
			    case 'r':
				deleterow(arg);
				break;
			    case 'c':
				deletecol(arg);
				break;
			    default:
				error("Invalid delete command");
				break;
			    }
			    break;
			case 'v':
			    switch (get_qual()) {
			    case 'r':
				rowvalueize(arg);
				modflg++;
				break;
			    case 'c':
				colvalueize(arg);
				modflg++;
				break;
			    default:
				error("Invalid value command");
				break;
			    }
			    break;
			case 'p':
			    {
			    register qual;
			    qual = get_qual();
			    while (arg--)
			        pullcells(qual);
			    break;
			    }
			case 'x':
			    {
			    register struct ent **p;
			    register int c1;

			    flush_saved();
			    for (c1 = curcol; arg-- && c1 < MAXCOLS; c1++) {
				p = &tbl[currow][c1];
				if (*p) {
			            free_ent(*p);
			            *p = 0;
				}
			    }
			    sync_refs();
			    FullUpdate++;
			    }
			    break;
			case 'Q':
			case 'q':
			    running = 0;
			    break;
			case 'h':
			    while (--arg>=0) {
				if (curcol)
				    curcol--;
				else
				    error ("At column A");
				while(col_hidden[curcol] && curcol)
				    curcol--;
			    }
			    break;
			case 'j':
			    while (--arg>=0) {
				if (currow < MAXROWS - 1)
				    currow++;
				else
				    error ("The table can't be any longer");
				while (row_hidden[currow]&&(currow<MAXROWS-1))
				    currow++;
			    }
			    break;
			case 'k':
			    while (--arg>=0) {
				if (currow)
				    currow--;
				else
				    error ("At row zero");
				while (row_hidden[currow] && currow)
				    currow--;
			    }
			    break;
			case ' ':
			case 'l':
			    while (--arg>=0) {
				if (curcol < MAXCOLS - 1)
				    curcol++;
				else
				    error ("The table can't be any wider");
				while(col_hidden[curcol]&&(curcol<MAXCOLS-1))
				    curcol++;
			    }
			    break;
			case 'm':
			    savedrow = currow;
			    savedcol = curcol;
			    break;
			case 'c': {
			    register struct ent *p = tbl[savedrow][savedcol];
			    register c1;
			    register struct ent *n;
			    if (!p)
				break;
			    FullUpdate++;
			    modflg++;
			    for (c1 = curcol; arg-- && c1 < MAXCOLS; c1++) {
				n = lookat (currow, c1);
				(void) clearent(n);
				n -> flags = p -> flags;
				n -> v = p -> v;
				n -> expr = copye(p->expr,
					    currow - savedrow,
					    c1 - savedcol);
				n -> label = 0;
				if (p -> label) {
				    n -> label = (char *)
						 xmalloc((unsigned)strlen(p->label)+1);
				(void) strcpy (n -> label, p -> label);
				}
			    }
			    break;
			}
			case 'z':
			    switch (get_qual()) {
			    case 'r':
				hiderow(arg);
				break;
			    case 'c':
				hidecol(arg);
				break;
			    default:
				error("Invalid zap command");
				break;
			    }
			    break;
			case 's':
			    switch (get_qual()) {
			    case 'r':
				rowshow_op();
				break;
			    case 'c':
				colshow_op();
				break;
			    default:
				error("Invalid show command");
				break;
			    }
			    break;
			case 'a':
			    switch (get_qual()) {
			    case 'r':
				while (arg--)
				    duprow();
				break;
			    case 'c':
				while (arg--)
				    dupcol();
				break;
			    default:
				error("Invalid add row/col command");
				break;
			    }
			    break;
			default:
			    if ((c & 0177) != c)
				error("Weird character, decimal '%d'.\n",
					(int) c);
			    else
				    error ("No such command  (%c)", c);
			    break;
		    }
	edistate = nedistate;
	arg = narg;
    }				/* while (running) */
    inloop = modcheck(" before exiting");
    }				/*  while (inloop) */
    deraw();
    endwin();
    exit(0);
    /*NOTREACHED*/
}

startshow()
{
    showrange = 1;
    showsr = currow;
    showsc = curcol;
}

showdr()
{
    int     minsr, minsc, maxsr, maxsc;

    minsr = showsr < currow ? showsr : currow;
    minsc = showsc < curcol ? showsc : curcol;
    maxsr = showsr > currow ? showsr : currow;
    maxsc = showsc > curcol ? showsc : curcol;
    (void) sprintf (line+linelim,"%s", r_name(minsr, minsc, maxsr, maxsc));
}


goraw()
{
#if SYSV2 || SYSV3
    fixterm();
#else
    cbreak();
    nonl();
    noecho ();
#endif
    kbd_again();
    (void) clear();
    FullUpdate++;
}

deraw()
{
    (void) move (LINES - 1, 0);
    (void) clrtoeol();
    (void) refresh();
#if SYSV2 || SYSV3
    resetterm();
#else
    nocbreak();
    nl();
    echo();
#endif
    resetkbd();
}

SIG_T
quit()
{
    deraw();
    resetkbd();
    endwin();
    exit(1);
}

signals()
{
    SIG_T quit();
    SIG_T atimeout();

    (void) signal(SIGINT, SIG_IGN);
    (void) signal(SIGQUIT, quit);
    (void) signal(SIGPIPE, quit);
    (void) signal(SIGTERM, quit);
    (void) signal(SIGALRM, atimeout);
    (void) signal(SIGFPE, quit);
    (void) signal(SIGBUS, quit);
}

modcheck(endstr)
char *endstr;
{
    if (modflg && curfile[0]) {
	char ch, lin[100];

	(void) move (0, 0);
	(void) clrtoeol ();
	(void) sprintf (lin,"File '%s' is modified, save%s? ",curfile,endstr);
	(void) addstr (lin);
	(void) refresh();
	ch = nmgetch();
 	if (ch != 'n' && ch != 'N')
 	    if (writefile(curfile, 0, 0, maxrow, maxcol) < 0)
 		return (1);
	else if (ch == ctl('g') || ch == ctl('[')) return(1);
    } else if (modflg) {
	char ch, lin[100];

	(void) move (0, 0);
	(void) clrtoeol ();
	(void) sprintf (lin,"Do you want a chance to save the data? ");
	(void) addstr (lin);
	(void) refresh();
	ch = nmgetch();
	if (ch == 'n' || ch == 'N') return(0);
	else return(1);
      }
    return(0);
}

    
writefile (fname, r0, c0, rn, cn)
char *fname;
{
    register FILE *f;
    register struct ent **p;
    register r, c;
    char save[1024];
    int pid;

    if (Crypt) {
	return (cwritefile(fname, r0, c0, rn, cn));
    }

    if (*fname == 0) fname = &curfile[0];

    (void) strcpy(save,fname);

    f = openout(fname, &pid);
    if (f == 0) {
	error ("Can't create %s", fname);
	return (-1);
    }

    (void) fprintf (f, "# This data file was generated by the Spreadsheet ");
    (void) fprintf (f, "Calculator.\n");
    (void) fprintf (f, "# You almost certainly shouldn't edit it.\n\n");
    for (c=0; c<MAXCOLS; c++)
	if (fwidth[c] != DEFWIDTH || precision[c] != DEFPREC)
	    (void) fprintf (f, "format %s %d %d\n",coltoa(c),fwidth[c],precision[c]);
    write_range(f);
    if (mdir) 
	    (void) fprintf(f, "mdir \"%s\"\n", mdir);
    for (r=r0; r<=rn; r++) {
	p = &tbl[r][0];
	for (c=c0; c<=cn; c++, p++)
	    if (*p) {
		if ((*p)->label) {
		    edits(r,c);
		    (void) fprintf(f, "%s\n",line);
		}
		if ((*p)->flags&is_valid) {
		    editv (r, c);
		    (void) fprintf (f, "%s\n",line);
		}
	    }
    }

    closeout(f, pid);

    if (!pid) {
        (void) strcpy(curfile, save);
        modflg = 0;
        error("File '%s' written.",curfile);
    }

    return (0);
}

readfile (fname,eraseflg)
char *fname;
int eraseflg;
{
    register FILE *f;
    char save[1024];

    if (*fname == '*' && mdir) { 
       (void) strcpy(save, mdir);
       *fname = '/';
       (void) strcat(save, fname);
    } else {
        if (*fname == 0)
	    fname = &curfile[0];
        (void) strcpy(save,fname);
    }

    if (Crypt)  {
	creadfile(save, eraseflg);
	return;
    }

    if (eraseflg && strcmp(fname,curfile) && modcheck(" first")) return;

    f = fopen (save, "r");
    if (f==0) {
	error ("Can't read %s", save);
	return;
    }

    if (eraseflg) erasedb ();

    while (fgets(line,sizeof line,f)) {
	linelim = 0;
	if (line[0] != '#') (void) yyparse ();
    }
    (void) fclose (f);
    linelim = -1;
    modflg++;
    if (eraseflg) {
	(void) strcpy(curfile,save);
	modflg = 0;
    }
    EvalAll();
}

erasedb () {
    register r, c;
    for (c = 0; c<=maxcol; c++) {
	fwidth[c] = DEFWIDTH;
	precision[c] = DEFPREC;
    }

    for (r = 0; r<=maxrow; r++) {
	register struct ent **p = &tbl[r][0];
	for (c=0; c++<=maxcol; p++)
	    if (*p) {
		if ((*p)->expr) efree ((*p) -> expr);
		if ((*p)->label) xfree ((char *)((*p) -> label));
		xfree ((char *)(*p));
		*p = 0;
	    }
    }
    maxrow = 0;
    maxcol = 0;
    clean_range();
    FullUpdate++;
}

#if DEBUG
debugout(g,fmt,args) FILE *g; char *fmt; {
    int op;

    if (g == 0) g = fopen("debug","a"),op = 1;
    if (g == 0) return;

    _doprnt(fmt, &args, g);

    (void) fflush(g);
    if (op) (void) fclose(g);
}
#endif
