#define BSD
#include "cent.h"

endgame()
{
    setblock(0, TRUE);
    signal(SIGINT,SIG_IGN);
    mvaddstr(22,60,"[Press return");
    mvaddstr(23,60," to continue]");
    refresh();
    nocrmode();
    nl();
    while (getchar() != '\n');
    echo();
    endwin();
    printf("\n\n");
    doscores();
    exit(0);
}

SIG_T
catchint(dummy)
{
    signal(SIGINT,SIG_IGN);
#ifdef SIGTSTP
    signal(SIGTSTP,SIG_IGN);
#endif
    inter = 1;
}

SIG_T
catchstop(dummy)
{
#ifdef SIGTSTP
    signal(SIGTSTP,SIG_IGN);
#endif
    signal(SIGINT,SIG_IGN);
    stopped = 1;
}

SIG_T
stopawhile(dummy)
{
#ifdef __STDC__
    struct termios curseterm;
#else
    struct sgttyb curseterm;
#endif

    move(23,0);
    refresh();
    putchar('\n');
    fflush(stdout);
#ifdef __STDC__
    tcgetattr(0,&curseterm);
    tcsetattr(0,TCSADRAIN,&origterm);
#else
    ioctl(0,TIOCGETP,&curseterm);
    ioctl(0,TIOCSETP,&origterm);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP,SIG_DFL);
    kill(getpid(),SIGTSTP);
    signal(SIGTSTP,SIG_IGN);
#endif
    stopped = 0;
#ifdef __STDC__
    tcsetattr(0,TCSADRAIN,&curseterm);
#else
    ioctl(0,TIOCSETP,&curseterm);
#endif
    redrawscr();
#ifdef __STDC__
    tcsetattr(0,TCSADRAIN,&curseterm);
#else
    ioctl(0,TIOCSETP,&curseterm);       /* Just to make sure... */
#endif
    waitboard();
#ifdef SIGTSTP
    signal(SIGTSTP,catchstop);
#endif
    signal(SIGINT,catchint);
}

SIG_T
quit(dummy)
{
    char ch;

    signal(SIGQUIT,SIG_IGN);
    mvaddstr(12,60,"Really quit?");
    refresh();
#ifndef SYS5
    ch = getchar();
#else
    setblock(0, TRUE);
    ch = getchar();
    setblock(0, FALSE);
#endif
    move(12,60);
    clrtoeol();
    refresh();
    if (ch == 'y' || ch == 'Y')
	endgame();
    inter = 0;
    signal(SIGINT,catchint);
#ifdef SIGTSTP
    signal(SIGTSTP,catchstop);
#endif
    signal(SIGQUIT,catchint);
}

waitboard()
{
    char ch;

#ifdef SIGTSTP
    signal(SIGTSTP,SIG_IGN);
#endif
    signal(SIGINT,SIG_IGN);
    mvaddstr(12,60,"Press return");
    mvaddstr(13,60,"when ready");
    refresh();
    setblock(0, TRUE);
    while ((ch = getchar()) != '\n')
    {
#ifdef WIZARD
	if (ch == '\020')
	    setname();
	else if (ch == '\014' || ch == 'r')
#else
	 if (ch == '\014' || ch == 'r')
#endif
	    redrawscr();
	else if (ch == 's' || ch == 'S')
	    savegame();
    }
    setblock(0, FALSE);
    move(12,60);
    clrtoeol();
    move(13,60);
    clrtoeol();
    move(15,60);
    clrtoeol();
    signal(SIGINT,catchint);
#ifdef SIGTSTP
    signal(SIGTSTP,catchstop);
#endif
}

SIG_T
catchalarm(dummy)
{
   gameover = 1;
}

ladie()
{
    mvaddstr(10,60,"Your game is over");
    mvaddstr(11,60,"due to the load av.");
    endgame();
}

redrawscr()
{
    curscr->_clear = 1;
    touchwin(stdscr);
    refresh();
}

