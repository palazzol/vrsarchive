/* information about dam */
#include	<curses.h>
prinfo(){
	int f;
	char c;
	int y, x;

        f = open("dam.help",0);
        if(f < 0) f = open(HELPFIL,0);
        if(f < 0){
                printw("Sorry, cannot find it\n");
                return;
        }
	erase();
        while(read(f,&c,1) == 1) {
		addch(c);
		getyx(stdscr,y,x);
		if (y >= 23) {
			printw("-- More? --");
			if (!answer())
				break;
			erase();
		}
	}
        (void) close(f);
}
