#include <signal.h>
#include <stdio.h>
#include <curses.h>
#include <sys/types.h>
#include <time.h>

#define rnd(x)          ((rand()>>3) % (x))
#define abs(a)  ((a) < 0 ? -(a) : (a))
#define MIDR  (LINES/2 - 1)
#define MIDC  (COLS/2 - 1)



        /* initial variable values */
#define TANKFULL        200
#define TORPEDOES       20
#define MAXTIME         120

static char *logfile = GAMLIB/sd.logfile"; 
unsigned seed;
int fuel=TANKFULL, torps=TORPEDOES;
int kills=0;
int row, column;
int dr = 0, dc = 0;
char destroyed;
static char *reasons[] = {
	"time ran out",
	"bought the farm",
	"was nuked",
	"blasted out of existence",
	"met his maker",
	"kicked the bucket",
	"blown to pieces",
	"ceased to be",
	"ate hot space death",
	"caught the big bus",
	"bit the big one",
	"turned to space dust",
	"is pushing up the daisies",
	"was vaporized",
	"atomized" };
static char *ranks[] = {
	"alien bait",
	"excess baggage",
	"head stewardess",
	"navigator",
	"co-pilot",
	"space pilot",
	"space ace",
	"master duelist",
	"space-legend",
	"GOD",
	"WIZARD" };
int Clock = MAXTIME;            /* time for all the flights in the game */
char cross = 1;
void (*oldsig)();

void
succumb()
{
        if (oldsig == SIG_DFL) {
                endfly();
	}
        if (oldsig != SIG_IGN) {
                (*oldsig)();
                endfly();
        }
	exit(1);
}

main()
{
        void moveenemy();
	int x;
	extern time_t time();

        seed = (unsigned)time((time_t *)0);
	srand(seed); 
        (void) initscr();
        while (1) {
        destroyed = 0;
        oldsig = signal(SIGINT, succumb);
        crmode();
        noecho();
        screen();
        row = rnd(LINES-3) + 1;
        column = rnd(COLS-2) + 1;
        moveenemy();
        for (;;) {
                switch(getchar()){

                        case 'h':
                        case 'r':
                                dc = -1;
                                fuel--;
                                break;

                        case 'H':
                        case 'R':
                                dc = -5;
                                fuel -= 10;
                                break;

                        case 'l':
                                dc = 1;
                                fuel--;
                                break;

                        case 'L':
                                dc = 5;
                                fuel -= 10;
                                break;

                        case 'j':
                        case 'u':
                                dr = 1;
                                fuel--;
                                break;

                        case 'J':
                        case 'U':
                                dr = 5;
                                fuel -= 10;
                                break;

                        case 'k':
                        case 'd':
                                dr = -1;
                                fuel--;
                                break;

                        case 'K':
                        case 'D':
                                dr = -5;
                                fuel -= 10;
                                break;

                        case '+':
                                if (cross){
                                        cross = 0;
                                        notarget();
                                }
                                else
                                        cross = 1;
                                break;

                        case ' ':
                        case 'f':
                                if (torps){
                                        torps -= 2;
                                        blast();
                                        if (row == MIDR && column - MIDC < 2 
					    && MIDC - column < 2){
                                                destroyed = 1;
                                                alarm(0);
                                        }
					else {
        				if (abs(row - MIDR) <= 1 && 
					    abs(column - MIDC) <= 2) {
					    /* 30% chance of hitting */
					    if ((x = rnd(1000)) > 850 ||
						 x < 150) {
                                                destroyed = 1;
                                                alarm(0);
					    }
					    else {
						move(0,0);
						clrtoeol();
						mvaddstr(0,0,"Missed him!");
						refresh();
					    }
					}
					else {
					    move(0,0);
					    clrtoeol();
					    mvaddstr(0,0,"Missed him!");
					    refresh();
					}
					}
                                }
                                else
                                        mvaddstr(0,0,"*** Out of torpedoes. ***");
                                break;

                        case 'q':
                                endfly();
                                return(0);

                        default:
                                mvaddstr(0,26,"Commands = r,R,l,L,u,U,d,D,f,+,q");
                                continue;

                        case EOF:
                                break;
                }
                if (destroyed){
                        ++kills;
                        if (kills % 5 == 0) {
                           Clock+=60;
                           fuel+=75;
                           torps+=16;
                        }
                        refresh();
                        break;
                        /* return(1);  */
                }
                if (Clock <= 0){
                        endfly();
                }
        }
        }
}

screen()
{
        register int r,c,n;
        int i;

        clear();
        i = rnd(100);
        for (n=0; n < i; n++){
                r = rnd(LINES-3) + 1;
                c = rnd(COLS);
                mvaddch(r, c, '.');
        }
        mvaddstr(LINES-1-1,8,"KILLS        TORPEDOES           FUEL           TIME");
        refresh();
}

target()
{
        register int n;

        move(MIDR,MIDC-10);
        addstr("-------   +   -------");
        for (n = MIDR-4; n < MIDR-1; n++){
                mvaddch(n,MIDC,'|');
                mvaddch(n+6,MIDC,'|');
        }
}

notarget()
{
        register int n;

        move(MIDR,MIDC-10);
        addstr("                     ");
        for (n = MIDR-4; n < MIDR-1; n++){
                mvaddch(n,MIDC,' ');
                mvaddch(n+6,MIDC,' ');
        }
}

blast()
{
        register int n;

        alarm(0);
        move(LINES-1, 24);
        printw("%3d", torps);
        for(n = LINES-1-2; n >= MIDR + 1; n--){
                mvaddch(n, MIDC+MIDR-n, '/');
                mvaddch(n, MIDC-MIDR+n, '\\');
                refresh();
        }
        mvaddch(MIDR,MIDC,'*');
        for(n = LINES-1-2; n >= MIDR + 1; n--){
                mvaddch(n, MIDC+MIDR-n, ' ');
                mvaddch(n, MIDC-MIDR+n, ' ');
                refresh();
        }
        alarm(1);
}

void
moveenemy()
{
        int d;
        int oldr, oldc;

        oldr = row;
        oldc = column;
        if (fuel > 0){
                if (row + dr <= LINES-3 && row + dr > 0)
                        row += dr;
                if (column + dc < COLS-1 && column + dc > 0)
                        column += dc;
        } else if (fuel < 0){
                fuel = 0;
                mvaddstr(0,60,"*** Out of fuel ***");
        }
        d = ((row - MIDR)*(row - MIDR) + (column - MIDC)*(column - MIDC));
        if (d < 16){
                row += (rnd(9) - 4) % (4 - abs(row - MIDR));
                column += (rnd(9) - 4) % (4 - abs(column - MIDC));
        }
        Clock--;
        mvaddstr(oldr, oldc - 1, "   ");
        if (cross)
                target();
        mvaddstr(row, column - 1, "/-\\");
        move(LINES-1, 9);
        printw("%3d", kills);
        move(LINES-1, 24);
        printw("%3d", torps);
        move(LINES-1, 42);
        printw("%3d", fuel);
        move(LINES-1, 57);
        printw("%3d", Clock);
        refresh();
        if (oldr == MIDR && oldc - MIDC < 2 && MIDC - oldc < 2) {
	    move(0,0);
	    clrtoeol();
	    mvaddstr(0,0,"The alien fires at you...");
	    refresh();
	    sleep(1);
#ifdef WIZARD
	    addstr(" You're hit!  Magically, you are unharmed");
	    refresh();
#else
  	    space_death();
#endif
	}
	else {
            if (abs(oldr - MIDR) <= 1 && abs(oldc - MIDC) <= 2) {
		move(0,0);
		clrtoeol();
	        mvaddstr(0,0,"The alien fires at you...");
		refresh();
		sleep(1);
	        if (rnd(1000) % 20 == 0) { 	/* 5% chance of being hit */
#ifdef WIZARD
	        addstr("You're hit! Magically, you are unharmed");
	        refresh();
#else
  	        space_death();
#endif
		}
	        else {
	            addstr("but misses!!");
		    refresh();
	        }
	    }
	}
        signal(SIGALRM, moveenemy);
        alarm(1);
}

endfly()
{
        alarm(0);
        signal(SIGALRM, SIG_DFL);
        mvcur(0,COLS-1,LINES-1,0);
#ifdef SIGTSTP
        signal(SIGTSTP, SIG_DFL);
#endif
        signal(SIGINT, oldsig);
	die();
}

die() 
{
        FILE *fp;
	int rank();
        char bf[12];
        long tv;
        char *date, *ctime();
        void (*s)() = signal(SIGINT,SIG_IGN);

	raw();	/* Flush pending input...GROT */
	noraw();
	clear();
	refresh();
	endwin();
        time(&tv);
        date = ctime(&tv);
        date[10] = '\0';
        if ((fp = fopen(logfile,"r")) == NULL) {
            fp = fopen(logfile,"a");
            fprintf(fp,"\t\t\t\tSPACE DUEL SCORE FILE\n\n");
            fprintf(fp," Who           What                Kills           When            Rank\n\n");
        }
        else
            fp = fopen(logfile,"a");
	if (fp == NULL)
		exit(0);
#ifdef WIZARD
        fprintf(fp, "%-6s %-23s with %-2d kills on  %-10s %16s\n",cuserid(bf),
	Clock == 0 ? reasons[0] : reasons[rnd(sizeof(reasons)/sizeof(char *)-1)+1] , kills, date, ranks[9]);
#else
        fprintf(fp, "%-6s %-23s with %-2d kills on  %-10s %16s\n",cuserid(bf),
	Clock == 0 ? reasons[0] : reasons[rnd(sizeof(reasons)/sizeof(char *)-1)+1] , kills, date, ranks[rank(kills)]);
#endif
        fclose(fp);
	chmod(logfile,0644);
        signal(SIGINT,s);
        execlp(PAGER, PAGER, logfile, (char *)0);
	exit(1);
}
space_death() {
    clear();
    mvaddstr(MIDR, 18,"\"EAT HOT SPACE DEATH!!!\" the alien screams,");
    mvaddstr(MIDR+1, 18,"as he breaks off in a victory roll.");
    mvaddstr(MIDR+3, 18,"You are consumed by the nuclear blast and die....");
    refresh();
    sleep(4);
    mvcur(0,COLS-1,LINES-1,0);
    die();
}
rank(x)
int x;
{
	if (x < 5)		
	    return(x);
	else
	   if (x < 10)	
	     return(5);
	   else
	      if (x < 15)
		 return(6);
	      else
		 if (x < 20)
		    return(7);
		 else		
		    if (x < 25)
			return(8);
		    else
		        return(9);
}
