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
    signal(SIGTSTP,SIG_IGN);
    inter = 1;
}

catchstop()
{
    signal(SIGTSTP,SIG_IGN);
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
    signal(SIGTSTP,SIG_DFL);
    kill(getpid(),SIGTSTP);
    signal(SIGTSTP,SIG_IGN);
    stopped = 0;
    ioctl(0,TIOCSETP,&curseterm);
    redrawscr();
    ioctl(0,TIOCSETP,&curseterm);       /* Just to make sure... */
    waitboard();
    signal(SIGTSTP,catchstop);
    signal(SIGINT,catchint);
}

quit()
{
    char ch;

    signal(SIGQUIT,SIG_IGN);
    mvaddstr(12,60,"Really quit?");
    refresh();
    ch = getchar();
    move(12,60);
    clrtoeol();
    refresh();
    if (ch == 'y' || ch == 'Y')
	endgame();
    inter = 0;
    signal(SIGINT,catchint);
    signal(SIGTSTP,catchstop);
    signal(SIGQUIT,quit);
}

waitboard()
{
    char ch;

    signal(SIGTSTP,SIG_IGN);
    signal(SIGINT,SIG_IGN);
    mvaddstr(12,60,"Press return");
    mvaddstr(13,60,"when ready");
    refresh();
    while ((ch = getchar()) != '\r')
    {
#ifdef WIZARD
	if (ch == '\020')
	    setname();
	else if (ch == '\014')
#else
	 if (ch == '\014')
#endif
	    redrawscr();
	else if (ch == 's' || ch == 'S')
	    savegame();
    }
    move(12,60);
    clrtoeol();
    move(13,60);
    clrtoeol();
    move(15,60);
    clrtoeol();
    signal(SIGINT,catchint);
    signal(SIGTSTP,catchstop);
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
