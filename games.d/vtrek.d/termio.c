/* terminal I/O */
#include "vtrek.h"

#ifdef AZTEC
/* These constants are at the beginning so they will be easy to find with */
/* DDT.  They will be a 0103 and 0107 if this file is loaded first. */
char cm[4] = "\033=";		/* cursor movement string */
char cl[4] = "\032";		/* clear screen string */
#else /* UNIX */
#include <sgtty.h>
static int saveflags;
static char bp[1024], area[1024];
static char *cl, *cm;

extern char *tgoto();
extern char PC;
extern char *BC;
extern char *UP;
extern short ospeed;

#endif

/* initialize the termimal mode */
terminit()
{
#ifdef UNIX
	struct sgttyb tty;
	char *p, *getenv(), *tgetstr();
	if ((p = getenv("TERM")) == NULL) {
	    fprintf(stderr, "TERM not set\n");
	    exit(1);
	}
	if (tgetent(bp, p) != 1) {
	    fprintf(stderr, "Can't get termcap entry\n");
	    exit(1);
	}
	p = area;
	cl = tgetstr("cl", &p);
	cm = tgetstr("cm", &p);
	UP = tgetstr("up", &p);
	BC = tgetstr("bc", &p);
	gtty(0, &tty);
	ospeed = tty.sg_ospeed;	/* THIS IS REQUIRED TO GET PADDING */
	saveflags = tty.sg_flags;
	tty.sg_flags |= RAW;
	tty.sg_flags &= ~(ECHO | XTABS);
	stty(0, &tty);
#endif
}

/* reset the terminal mode */
termreset()
{
#ifdef UNIX
	struct sgttyb tty;
	gtty(0, &tty);
	tty.sg_flags = saveflags;
	stty(0, &tty);
#endif
}

/* get a character from the terminal */
getch()
{
#ifdef AZTEC
	int ch;
	while ((ch = CPM(6, 0xff)) == 0)
	    ;
	return ch;
#else /* UNIX */
	return getchar() & 0177;
#endif
}

/* write a character */
putch(ch)
int ch;
{
#ifdef AZTEC
	CPM(6, ch);
#else /* UNIX */
	putchar(ch);
#endif
}

#ifdef AZTEC
/* see if a character is ready to be read */
chready()
{
	return CPM(11, 0);
}
#endif

/* move cursor */
moveyx(ypos,xpos)
int ypos,xpos;
{
#ifdef AZTEC
	printf("%s%c%c", cm, ypos+31, xpos+31);
#else /* UNIX */
	tputs(tgoto(cm, xpos - 1, ypos - 1),1,putch);
#endif
}

/* clear screen */
cls()
{
#ifdef AZTEC
	fputs(cl, stdout);
#else /* UNIX */
	tputs(cl,24,putch);
#endif
}
