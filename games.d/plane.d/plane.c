/*
 *	%W%	%G%
 *	plane - a slightly frivolous curses demo.
*/
#include <curses.h>

main()
{ int line, column;
  WINDOW *bomb;
  WINDOW *plane;
  WINDOW *cloud;

  initscr();
  cloud = newwin(8,30,2,COLS*3/5);
  plane = newwin(8,COLS,9,0);
  bomb = newwin(LINES-15,COLS,15,0);
  waddstr(cloud,"          .\n        .  . .\n  .  .   .. .   ..\n .      .  .  .   .\n  .  .   .. .   . .\n        .  .     .  .");
  waddstr(plane,"  ^\n | \\	   --------\n |  \\________/ /___|\n |--   S U  / /	   0\n <---------/ /-----|\n        --------");
  waddstr(bomb,"          rm *");
  wrefresh(cloud);
  wrefresh(plane);
  wrefresh(bomb);
  for (column = 0;column < COLS; column++) {
    wmove(cloud,column+12/23,0);
    winsch(cloud,' ');
    wrefresh(cloud);
    for (line = 0; line < ((LINES-15 < 9) ? 9 : LINES-15); line++) {
      wmove(plane,line,0);
      winsch(plane,' ');
      wmove(bomb,line,0);
      winsch(bomb,' ');
    }
    wrefresh(plane);
    if (((COLS/2) < column+10) && (column%2 != 0)) {
      wmove(bomb,0,0);
      winsertln(bomb);
    }
    wrefresh(bomb);
  }
  mvcur(0,COLS-1,LINES-1,0);
  endwin();
  return(0);
}
