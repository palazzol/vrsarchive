#include "cent.h"

endgame()
{
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

catchint()
{
    signal(SIGINT,SIG_IGN);
#ifndef SYSV
    signal(SIGTSTP,SIG_IGN);
#endif
    inter = 1;
}

catchstop()
{
#ifndef SYSV
    signal(SIGTSTP,SIG_IGN);
#endif
    signal(SIGINT,SIG_IGN);
    stopped = 1;
}

stopawhile()
{
    struct sgttyb curseterm;

    move(23,0);
    refresh();
    putchar('\n');
    fflush(stdout);
    ioctl(0,TIOCGETP,&curseterm);
    ioctl(0,TIOCSETP,&origterm);
#ifndef SYSV
    signal(SIGTSTP,SIG_DFL);
    kill(getpid(),SIGTSTP);
    signal(SIGTSTP,SIG_IGN);
#endif
    stopped = 0;
    ioctl(0,TIOCSETP,&curseterm);
    redrawscr();
    ioctl(0,TIOCSETP,&curseterm);       /* Just to make sure... */
    waitboard();
#ifndef SYSV
    signal(SIGTSTP,catchstop);
#endif
    signal(SIGINT,catchint);
}

quit()
{
    char ch;

    signal(SIGQUIT,SIG_IGN);
    mvaddstr(12,60,"Really quit?");
    refresh();
#ifndef SYSV
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
#ifndef SYSV
    signal(SIGTSTP,catchstop);
#endif
    signal(SIGQUIT,catchint);
}

waitboard()
{
    char ch;

#ifndef SYSV
    signal(SIGTSTP,SIG_IGN);
#endif
    signal(SIGINT,SIG_IGN);
    mvaddstr(12,60,"Press return");
    mvaddstr(13,60,"when ready");
    refresh();
    setblock(0, TRUE);
    while ((ch = getchar()) != '\r')
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
#ifndef SYSV
    signal(SIGTSTP,catchstop);
#endif
}

catchalarm()
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

