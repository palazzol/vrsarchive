#include <stdio.h>
#define NSTGS 100

/*      call:   ctc opts tc arg1 arg2
		opts:
		    -d - list only differences.
		arg1, arg2 may be +, indicating the TERMCAP environ variable;
			   may be files (the termcap is selected);
			   if arg2 is missing, the right column will contain
			   entry explanations.
		tc - the termcap to be compared;

	The output is a side-by-side listing of the termcaps.
*/

struct entry {
    char *t_name;
    char *t_line;
} interp[] = {
"Bj", "bottom join graphic"                 ,
"Bl", "bottom left corner graphic"          ,
"Br", "bottom right graphic"                ,
"CC", "command character in prototype"      ,
"Cj", "center join (cross) graphic"         ,
"Ge", "enter graphics mode"                 ,
"Gs", "start graphics mode"                 ,
"Hl", "horizontal line graphic"             ,
"Lj", "left join graphic"                   ,
"Rj", "right join graphic"                  ,
"Tj", "top join graphic"                    ,
"Tl", "top left corner graphic"             ,
"Tr", "top right corner graphic"            ,
"Vl", "vertical line graphic"               ,
"Xc", "other special graphic"               ,
"ae", "end alternate character set"         ,
"al", "add (insert) blank line"             ,
"am", "automatic margins"                   ,
"as", "start alternate character set"       ,
"bc", "backspace"                           ,
"bl", "bell"                                ,
"bs", "^h does backspace"                   ,
"bt", "back tab"                            ,
"bw", "backspace wraps backwards"           ,
"cd", "clear to end of display"             ,
"ce", "clear to end of line"                ,
"ch", "like cm but horizontal motion only"  ,
"cl", "clear screen"                        ,
"cm", "cursor motion"                       ,
"co", "number of columns in a line"         ,
"cr", "carriage return"                     ,
"cs", "change scrolling region (vt100)"     ,
"ct", "clear tabs"                          ,
"cv", "like ch but vertical only"           ,
"dB", "number of millisec of bs delay"      ,
"dC", "number of millisec of cr delay"      ,
"dF", "number of millisec of ff delay"      ,
"dN", "number of millisec of nl delay"      ,
"dT", "number of millisec of tab delay"     ,
"da", "display may be retained above"       ,
"db", "display may be retained below"       ,
"dc", "delete character"                    ,
"dl", "delete line"                         ,
"dm", "enter delete mode"                   ,
"do", "down one line"                       ,
"ds", "erase status line"                   ,
"ec", "erase character"                     ,
"ed", "end delete mode"                     ,
"ei", "end insert mode"                     ,
"eo", "can erase overstrikes with a blank"  ,
"es", "escape ok on status line"            ,
"ff", "hardcopy terminal page eject"        ,
"fs", "end status line, restore cursor"     ,
"gn", "generic line type (eg, dialup)"      ,
"hc", "hardcopy terminal"                   ,
"hd", "forward 1/2 linefeed"                ,
"ho", "home cursor"                         ,
"hs", "status line"                         ,
"hu", "reverse 1/2 linefeed"                ,
"hz", "hazeltine; can't print ~'s"          ,
"ic", "insert character"                    ,
"if", "name of file containing is"          ,
"im", "insert mode (enter)"                 ,
"in", "enter insert mode"                   ,
"ip", "insert pad after character inserted" ,
"is", "terminal initialization string"      ,
"it", "initial tabs every n spaces"         ,
"k0", "sent by other function key 0"        ,
"k1", "sent by other function key 1"        ,
"k2", "sent by other function key 2"        ,
"k3", "sent by other function key 3"        ,
"k4", "sent by other function key 4"        ,
"k5", "sent by other function key 5"        ,
"k6", "sent by other function key 6"        ,
"k7", "sent by other function key 7"        ,
"k8", "sent by other function key 8"        ,
"k9", "sent by other function key 9"        ,
"k;", "sent by other function key 10"       ,
"kA", "sent by insert-line key"             ,
"kB", "sent by reverse-tab key"             ,
"kC", "sent by clear key"                   ,
"kD", "sent by delete-character key"        ,
"kE", "sent by clear-to-eol key"            ,
"kF", "sent by scroll-forward/down key"     ,
"kH", "sent by home key"                    ,
"kI", "sent by ins-char/enter-ins-mode key" ,
"kL", "sent by delete-line key"             ,
"kM", "sent by rmir or smir in insert mode" ,
"kN", "sent by next-page key"               ,
"kP", "sent by previous-page key"           ,
"kR", "sent by scroll-backward/up key"      ,
"kS", "sent by clear-to-end-of-screen key"  ,
"kT", "sent by set-tab key"                 ,
"ka", "sent by clear-all-tabs key"          ,
"kb", "sent by backspace key"               ,
"kd", "sent by terminal down arrow key"     ,
"ke", "out of keypad-transmit mode"         ,
"kh", "sent by home key"                    ,
"kl", "sent by terminal left arrow key"     ,
"km", "has meta-shift key"                  ,
"kn", "number of other keys"                ,
"kr", "sent by terminal right arrow key"    ,
"ks", "put terminl in keypad-transmit mode" ,
"kt", "sent by clear-tab key"               ,
"ku", "sent by terminal up arrow key"       ,
"l0", "label on function key 0"             ,
"l1", "label on function key 1"             ,
"l2", "label on function key 2"             ,
"l3", "label on function key 3"             ,
"l4", "label on function key 4"             ,
"l5", "label on function key 5"             ,
"l6", "label on function key 6"             ,
"l7", "label on function key 7"             ,
"l8", "label on function key 8"             ,
"l9", "label on function key 9"             ,
"la", "label on function key 10"            ,
"le", "cursor left"                         ,
"li", "number of lines on screen or page"   ,
"ll", "last line, first column"             ,
"lm", "lines of memory"                     ,
"ma", "arrow key map"                       ,
"mb", "enter blink enhance"                 ,
"md", "enter bold mode"                     ,
"me", "exit attribute mode (normal mode)"   ,
"mh", "enter dim enhance"                   ,
"mi", "safe to move while in insert mode"   ,
"mk", "enter blank enhance"                 ,
"ml", "memory lock on above cursor"         ,
"mm", "metachar mode on"                    ,
"mo", "metachar mode off"                   ,
"mp", "enter protect mode"                  ,
"mr", "enter reverse enhance"               ,
"ms", "safe to move while in so & ul mode"  ,
"mu", "memory unlock (turnoff memory lock)" ,
"nc", "no correctly working carriage retrn" ,
"nd", "nondestructive space (cursor right)" ,
"nl", "newline character"                   ,
"ns", "terminal is CRT but doesn't scroll"  ,
"nw", "new-line carriage-return"            ,
"os", "terminal overstrikes"                ,
"p0", "turn on printer"                     ,
"pb", "lowest baud rate requiring padding"  ,
"pc", "pad character (rather than null)"    ,
"pf", "turn off the printer"                ,
"po", "turn on the printer"                 ,
"ps", "print contents of the screen"        ,
"pt", "has hardware tabs"                   ,
"rP", "like ip but when in replace mode"    ,
"rc", "restore cursor to positn of last sc" ,
"rf", "file containing reset string"        ,
"sc", "save cursor position"                ,
"se", "end stand out mode"                  ,
"sf", "scroll forwards"                     ,
"sg", "number of blank chars left by so"    ,
"so", "begin stand out mode"                ,
"sr", "scroll reverse (backwards)"          ,
"st", "set a tab in current column"         ,
"ta", "tab"                                 ,
"tc", "goto terminal - must be last"        ,
"te", "string to end programs that use cm"  ,
"ti", "string to begin progs that use cm"   ,
"ts", "enter status line entry mode"        ,
"uc", "underscore one char and move past"   ,
"ue", "end underscore mode"                 ,
"ug", "number of blank chars left by us"    ,
"ul", "underlines, doesn't overstrike"      ,
"up", "upline (cursor up)"                  ,
"us", "start underscore mode"               ,
"vb", "visible bell (may not move cursor)"  ,
"ve", "sequence to end open/visual mode"    ,
"vi", "put terminal in visual mode"         ,
"vs", "make cursor standout"                ,
"ws", "length of status line"               ,
"xb", "beehive (f1=escape, f2=ctrl C)"      ,
"xn", "newline ignord after wrap (Concept)" ,
"xo", "terminal uses xon/xoff flow ctrl"    ,
"xr", "return acts like \r\n (DeltaData)"   ,
"xs", "standout not erased by overwrite"    ,
"xt", "tabs destroy, magic so(Teleray1061)" ,
"xv", "vt100 col 80 glitch"                 ,
0,    0,
};

extern char *getenv();
char lbuf[1024], rbuf[1024];
int dflag = 0;
int expflag = 0;

main (argc, argv)

    int argc;
    char **argv;
{
    char *arg;
    static char *tcp = (char *) 0;
    static char *larg = (char *) 0;
    static char *rarg = (char *) 0;
    char c;
    char *lstgs[NSTGS], *rstgs[NSTGS];
    int ls, rs, i, j;
    int argcnt = 1;

    while (--argc) {
	arg = *(++argv);
	if (*arg == '-') {
	    arg++;
	    while ((c = *arg++) != 0) {
		switch (c) {
		case 'd':
		    dflag = 1;
		    break;
		default:
		    goto uerr;
		}
	    }
	} else {
	    switch (argcnt++) {
	    case 1:
		tcp = arg;
		break;
	    case 2:
		larg = arg;
		break;
	    case 3:
		rarg = arg;
		break;
	    default:
		goto uerr;
	    }
	}
    }
    if (!larg) larg = "+";
    if (!tcp) tcp = getenv ("TERM");
    i = gettc (tcp, larg, lbuf, &ls, lstgs);
    j = gettc (tcp, rarg, rbuf, &rs, rstgs);
    printf ("Termcap entry for %s terminal:\n", tcp);
    prtln (lbuf, rbuf);
    printf ("%-4d bytes in %-4d strings          %-4d bytes in %-4d strings\n",
	ls, i, rs, j);
    printf ("----------------------------------- -----------------------------------\n");
    present (lstgs, i, rstgs, j);
    exit (0);

uerr:
    fprintf (stderr, "Usage: ctc [-opts] term tc1 [tc2]\n");
    exit (1);
}

int
gettc (tc, arg, buf, s, stgs)
    char *tc, *arg, *buf;
    char *stgs[NSTGS];
    int *s;
{
    char *i;
    int n;

    if (arg == (char *) 0) {
	expflag++;
	s = 0;
	return 0;
    }
    if (*arg == '+')
	*arg = 0;
    if (tgetent (buf, tc, arg) != 1) {
	fprintf (stderr, "ctc: tgetent can't find term %s in %s\n", tc, arg);
	exit (1);
    }
    *s = strlen (buf);
    for (i = buf; *i != ':'; i++)
	;
    *i = 0;
    for (n = 0, i++; n < NSTGS; n++) {
	stgs[n] = i;
	    for (; *i != ':' && *i != 0; i++)
		;
	if (*i == 0)
	    break;
	*i++ = 0;
	if (strlen (stgs[n]) == 0 ||
		*stgs[n] == '	' ||
		*stgs[n] == ' '
		    ) n--;
    }
    sort (stgs, n);
    return n;
}

sort (s, n)      /* right from the Book */
    char **s;
    int n;
{
    int gap, i, j;
    char *temp;

    for (gap = n/2; gap > 0; gap /= 2)
	for (i = gap; i < n; i++)
	    for (j = i - gap; j >= 0; j -= gap) {
		if (strcmp (s[j], s[j+gap]) <= 0)
		    break;
		temp = s[j];
		s[j] = s[j+gap];
		s[j+gap] = temp;
	    }
}

present (lstgs, i0, rstgs, j0)
    char **lstgs, **rstgs;
    int i0, j0;
{
    int i, j, n;
    char lbuf[40];
    i = j = 0;
    do {
	if (expflag) {
	    explan (lstgs[i], lbuf);
	    prtln (lstgs[i++], lbuf);
	} else {
	    if ((n = comp (lstgs[i], i == i0, rstgs[j], j == j0)) == 0) {
		if (!dflag || strcmp (lstgs[i], rstgs[j]) != 0)
			prtln (lstgs[i], rstgs[j]);
		i++; j++;
	    } else if (n < 0)
		prtln (lstgs[i++], "");
	    else
		prtln ("", rstgs[j++]);
	}
    } while (i < i0 || (j < j0 && ! expflag));
}

int comp (ls, ie, rs, je)
    char *ls, *rs;
    int ie, je;
{
    if (ie) return (1);
    if (je) return (-1);
    do {
	if (*ls == *rs) continue;
	if (*ls < *rs) return (-1);
	else return (1);
    } while (*ls && *ls != '=' && *ls++ != '#' &&
	     *rs && *rs != '=' && *rs++ != '#'    );
    return (0);
}

explan (instg, outstg)
    char *instg, *outstg;
{
    struct entry *i;
    char *k, *j;

    for (i = interp; i->t_name; i++) {
	k = i->t_name;
	for (j = instg; *j != 0 && *j != '=' && *j != '#'; j++)
	    if (*j != *k++) goto nextstg;
	strcpy (outstg, i->t_line);
	return;
nextstg:;
    }
    strcpy (outstg, "Unknown capability");
}

prtln (s1, s2)
char *s1, *s2;
{
    if (strlen (s2) == 0)
	printf ("%-.35s\n", s1);
    else
	printf ("%-35.35s %-.35s\n", s1, s2);
    prtpln (strlen (s1) > 35 ? &s1[35] : "",
	    strlen (s2) > 35 ? &s2[35] : "");
}

prtpln (s1, s2)
char *s1, *s2;
{
    if (strlen (s1) + strlen (s2) == 0) return;
    if (strlen (s2) == 0)
	printf ("     %-.30s\n", s1);
    else
	printf ("     %-30.30s      %-.30s\n", s1, s2);
    prtpln (strlen (s1) > 30 ? &s1[30] : "",
	    strlen (s2) > 30 ? &s2[30] : "");
}

