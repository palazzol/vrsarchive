/*
 *	Interface to termcap library.
*/

#ifdef M_V7
#  include <sgtty.h>
#else
#  ifndef TCSETA
#    include <sys/types.h>
#    include <sys/ioctl.h>
#  endif
#  include <termio.h>
#endif

char *		BC;
char *		UP;
char *		eeolseq;
char *		cmseq;
char *		clearseq;
extern int	putchar();
extern char *	tgoto();

#ifdef M_V7
short   ospeed;
#endif

tc_setup() {

    static  char    bp[1024];
    static  char    buffer[1024];
    char    *area = buffer;
    char    *getenv();
    char    *tgetstr();
    char    *name;
    int     retcode;

    name = getenv("TERM");

    retcode = tgetent(bp, name);

    switch(retcode) {

        case -1:
            printf("can't open termcap file.\n");
            exit(1);
            break;

        case 0:
            printf("No termcap entry for %s.\n", name);
            exit(1);
            break;

    }

    eeolseq = tgetstr("ce", &area);
    cmseq   = tgetstr("cm", &area);
    clearseq = tgetstr("cl", &area);
    BC   = tgetstr("bc", &area);
    UP   = tgetstr("up", &area);

}

eeol() {

    tputs(eeolseq, 0, putchar);

}

clear_screen() {

    tputs(clearseq, 0, putchar);

}

movecur(row, col)
int row, col;
{

    tputs(tgoto(cmseq, col, row), 0, putchar);

}

#ifdef M_V7
struct sgttyb old_term;
struct sgttyb new_term;
#else
struct termio old_term;
struct termio new_term;
#endif

/*
    Set terminal to CBREAK and NOECHO.
*/
set_term() {
    static int  first = 1;

    if(first) {
#ifdef M_V7
        gtty(0, &old_term);
        gtty(0, &new_term);
        new_term.sg_flags &= ~(ECHO|XTABS); /* | CRMOD); */
        new_term.sg_flags |= CBREAK;
	ospeed = new_term.sg_ospeed;
#else
	ioctl(0 , TCGETA , &new_term);
	ioctl(0 , TCGETA , &old_term);
        new_term.c_lflag &= ~(ECHO | ECHOE | ICANON);
	new_term.c_lflag |= ISIG;
	new_term.c_cc[4] = 1;
	new_term.c_cc[5] = 0;
#endif
        first = 0;
    }

#ifdef M_V7
    stty(0, &new_term);
#else
    ioctl(0, TCSETA , &new_term);
#endif
}

/*
    Reset the terminal to normal mode.
*/
reset_term() {

#ifdef M_V7
    stty(0, &old_term);
#else
    ioctl(0, TCSETA , &old_term);
#endif
}
