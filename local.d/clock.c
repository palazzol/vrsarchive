/*   Program:         clock.
 *   Author:          Jim Earenluindil Trethewey.
 *   Version:         1.03.
 *   Index:           CSD255.
 *   Date:            21-Oct-1983.
 *   Last-edit:       07-Feb-1984.
 *   Language:        C.
 *   Ppn:             /user/serial/flamer.
 *   For:             Intel Corporation, Hillsboro, Oregon.
 *
 *   This program will print a pretty clock on the terminal until the user 
 *   hits a key on the keyboard.
 *
 *   Usage:  clock [<refresh_time>]
 *
 *       <refresh_time> is the period (in seconds) at which the screen
 *                      display is refreshed.  Note that the first refresh
 *                      may come sooner, the program attempts to synchronize
 *                      itself on even minutes.  If not specified, defaults
 *                      to 60 (every minute).
 *
 *   Compile:  cc -o clock clock.c -lcurses -ltermcap -ljobs
 *
 *   If not running under BSD, change all sigset's to signal's.
 *
 *   You may want to substitute the characters used to make the numbers
 *   from #'s to something else depending on your terminal.  If your
 *   terminal has 'blotch' characters, they are most pleasing.  (OK, so
 *   we ought to have the termcap 'blotch=<char>' capability, and be able
 *   to look it up.)
 *
 */

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <curses.h>
#ifndef BERKELEY
#  define sigset signal
#endif

char myname[10] = "XENIX";
WINDOW     *scr;
int        done;
int        in_char;
unsigned   refresh_time;
char       *ctime();
struct tm  *localtime();
struct tm  *loc_time;

pr_colon (col)
int col;
{ wmove (scr,  6, col);        wprintw (scr, "   ");
  wmove (scr,  7, col);        wprintw (scr, "   ");
  wmove (scr,  8, col);        wprintw (scr, "   ");
  wmove (scr,  9, col);        wprintw (scr, " # ");
  wmove (scr, 10, col);        wprintw (scr, "###");
  wmove (scr, 11, col);        wprintw (scr, " # ");
  wmove (scr, 12, col);        wprintw (scr, "   ");
  wmove (scr, 13, col);        wprintw (scr, "   ");
  wmove (scr, 14, col);        wprintw (scr, " # ");
  wmove (scr, 15, col);        wprintw (scr, "###");
  wmove (scr, 16, col);        wprintw (scr, " # ");
  wmove (scr, 17, col);        wprintw (scr, "   ");
  wmove (scr, 18, col);        wprintw (scr, "   ");
  wmove (scr, 19, col);        wprintw (scr, "   ");
}

pr_space ()
{ int i;

  for (i = 6; i < 20; i++) {
    wmove (scr, i, 9);
    wprintw (scr, "                                   ");
    wmove (scr, i, 42);
    wprintw (scr, "                                   ");
  }
}

pr_big (num, col)
int num;
int col;
{ if (num == 0) {
    wmove (scr,  6, col);	wprintw (scr, "   ######   ");
    wmove (scr,  7, col);	wprintw (scr, " ########## ");
    wmove (scr,  8, col);	wprintw (scr, "####    ####");
    wmove (scr,  9, col);	wprintw (scr, "###      ###");
    wmove (scr, 10, col);	wprintw (scr, "###      ###");
    wmove (scr, 11, col);	wprintw (scr, "###      ###");
    wmove (scr, 12, col);	wprintw (scr, "###      ###");
    wmove (scr, 13, col);	wprintw (scr, "###      ###");
    wmove (scr, 14, col);	wprintw (scr, "###      ###");
    wmove (scr, 15, col);	wprintw (scr, "###      ###");
    wmove (scr, 16, col);	wprintw (scr, "###      ###");
    wmove (scr, 17, col);	wprintw (scr, "####    ####");
    wmove (scr, 18, col);	wprintw (scr, " ########## ");
    wmove (scr, 19, col);	wprintw (scr, "   ######   ");
  } else if (num == 1) {
    wmove (scr,  6, col);	wprintw (scr, "      ##    ");
    wmove (scr,  7, col);	wprintw (scr, "     ###    ");
    wmove (scr,  8, col);	wprintw (scr, "    ####    ");
    wmove (scr,  9, col);	wprintw (scr, "   #####    ");
    wmove (scr, 10, col);	wprintw (scr, "     ###    ");
    wmove (scr, 11, col);	wprintw (scr, "     ###    ");
    wmove (scr, 12, col);	wprintw (scr, "     ###    ");
    wmove (scr, 13, col);	wprintw (scr, "     ###    ");
    wmove (scr, 14, col);	wprintw (scr, "     ###    ");
    wmove (scr, 15, col);	wprintw (scr, "     ###    ");
    wmove (scr, 16, col);	wprintw (scr, "     ###    ");
    wmove (scr, 17, col);	wprintw (scr, "     ###    ");
    wmove (scr, 18, col);	wprintw (scr, "   #######  ");
    wmove (scr, 19, col);	wprintw (scr, "   #######  ");
  } else if (num == 2) {
    wmove (scr,  6, col);	wprintw (scr, "  ########  ");
    wmove (scr,  7, col);	wprintw (scr, " ########## ");
    wmove (scr,  8, col);	wprintw (scr, "###     ####");
    wmove (scr,  9, col);	wprintw (scr, "###      ###");
    wmove (scr, 10, col);	wprintw (scr, "         ###");
    wmove (scr, 11, col);	wprintw (scr, "         ###");
    wmove (scr, 12, col);	wprintw (scr, "         ###");
    wmove (scr, 13, col);	wprintw (scr, "        ### ");
    wmove (scr, 14, col);	wprintw (scr, "       ###  ");
    wmove (scr, 15, col);	wprintw (scr, "     ####   ");
    wmove (scr, 16, col);	wprintw (scr, "   ####     ");
    wmove (scr, 17, col);	wprintw (scr, " ####       ");
    wmove (scr, 18, col);	wprintw (scr, "############");
    wmove (scr, 19, col);	wprintw (scr, "############");
  } else if (num == 3) {
    wmove (scr,  6, col);	wprintw (scr, "  ########  ");
    wmove (scr,  7, col);	wprintw (scr, " ########## ");
    wmove (scr,  8, col);	wprintw (scr, "###     ####");
    wmove (scr,  9, col);	wprintw (scr, "###      ###");
    wmove (scr, 10, col);	wprintw (scr, "         ###");
    wmove (scr, 11, col);	wprintw (scr, "         ###");
    wmove (scr, 12, col);	wprintw (scr, "    ####### ");
    wmove (scr, 13, col);	wprintw (scr, "    ####### ");
    wmove (scr, 14, col);	wprintw (scr, "         ###");
    wmove (scr, 15, col);	wprintw (scr, "         ###");
    wmove (scr, 16, col);	wprintw (scr, "###      ###");
    wmove (scr, 17, col);	wprintw (scr, "###     ####");
    wmove (scr, 18, col);	wprintw (scr, " ########## ");
    wmove (scr, 19, col);	wprintw (scr, "  ########  ");
  } else if (num == 4) {
    wmove (scr,  6, col);	wprintw (scr, "       ###  ");
    wmove (scr,  7, col);	wprintw (scr, "      ####  ");
    wmove (scr,  8, col);	wprintw (scr, "     #####  ");
    wmove (scr,  9, col);	wprintw (scr, "    ######  ");
    wmove (scr, 10, col);	wprintw (scr, "   ### ###  ");
    wmove (scr, 11, col);	wprintw (scr, "  ###  ###  ");
    wmove (scr, 12, col);	wprintw (scr, " ###   ###  ");
    wmove (scr, 13, col);	wprintw (scr, "############");
    wmove (scr, 14, col);	wprintw (scr, "############");
    wmove (scr, 15, col);	wprintw (scr, "       ###  ");
    wmove (scr, 16, col);	wprintw (scr, "       ###  ");
    wmove (scr, 17, col);	wprintw (scr, "       ###  ");
    wmove (scr, 18, col);	wprintw (scr, "       ###  ");
    wmove (scr, 19, col);	wprintw (scr, "       ###  ");
  } else if (num == 5) {
    wmove (scr,  6, col);	wprintw (scr, "############");
    wmove (scr,  7, col);	wprintw (scr, "############");
    wmove (scr,  8, col);	wprintw (scr, "###         ");
    wmove (scr,  9, col);	wprintw (scr, "###         ");
    wmove (scr, 10, col);	wprintw (scr, "### ######  ");
    wmove (scr, 11, col);	wprintw (scr, "########### ");
    wmove (scr, 12, col);	wprintw (scr, "###     ####");
    wmove (scr, 13, col);	wprintw (scr, "         ###");
    wmove (scr, 14, col);	wprintw (scr, "         ###");
    wmove (scr, 15, col);	wprintw (scr, "         ###");
    wmove (scr, 16, col);	wprintw (scr, "###      ###");
    wmove (scr, 17, col);	wprintw (scr, "###     ####");
    wmove (scr, 18, col);	wprintw (scr, " ########## ");
    wmove (scr, 19, col);	wprintw (scr, "  ########  ");
  } else if (num == 6) {
    wmove (scr,  6, col);	wprintw (scr, "       ###  ");
    wmove (scr,  7, col);	wprintw (scr, "      ###   ");
    wmove (scr,  8, col);	wprintw (scr, "     ###    ");
    wmove (scr,  9, col);	wprintw (scr, "    ###     ");
    wmove (scr, 10, col);	wprintw (scr, "   ###      ");
    wmove (scr, 11, col);	wprintw (scr, "  ###       ");
    wmove (scr, 12, col);	wprintw (scr, " #########  ");
    wmove (scr, 13, col);	wprintw (scr, "########### ");
    wmove (scr, 14, col);	wprintw (scr, "###      ###");
    wmove (scr, 15, col);	wprintw (scr, "###      ###");
    wmove (scr, 16, col);	wprintw (scr, "###      ###");
    wmove (scr, 17, col);	wprintw (scr, "###      ###");
    wmove (scr, 18, col);	wprintw (scr, " ########## ");
    wmove (scr, 19, col);	wprintw (scr, "   ######   ");
  } else if (num == 7) {
    wmove (scr,  6, col);	wprintw (scr, "############");
    wmove (scr,  7, col);	wprintw (scr, "############");
    wmove (scr,  8, col);	wprintw (scr, "###      ###");
    wmove (scr,  9, col);	wprintw (scr, "         ###");
    wmove (scr, 10, col);	wprintw (scr, "         ###");
    wmove (scr, 11, col);	wprintw (scr, "        ### ");
    wmove (scr, 12, col);	wprintw (scr, "       ###  ");
    wmove (scr, 13, col);	wprintw (scr, "      ###   ");
    wmove (scr, 14, col);	wprintw (scr, "     ###    ");
    wmove (scr, 15, col);	wprintw (scr, "    ###     ");
    wmove (scr, 16, col);	wprintw (scr, "   ###      ");
    wmove (scr, 17, col);	wprintw (scr, "  ###       ");
    wmove (scr, 18, col);	wprintw (scr, " ###        ");
    wmove (scr, 19, col);	wprintw (scr, "###         ");
  } else if (num == 8) {
    wmove (scr,  6, col);	wprintw (scr, "   ######   ");
    wmove (scr,  7, col);	wprintw (scr, " ########## ");
    wmove (scr,  8, col);	wprintw (scr, "###      ###");
    wmove (scr,  9, col);	wprintw (scr, "###      ###");
    wmove (scr, 10, col);	wprintw (scr, "###      ###");
    wmove (scr, 11, col);	wprintw (scr, "###      ###");
    wmove (scr, 12, col);	wprintw (scr, " ########## ");
    wmove (scr, 13, col);	wprintw (scr, " ########## ");
    wmove (scr, 14, col);	wprintw (scr, "###      ###");
    wmove (scr, 15, col);	wprintw (scr, "###      ###");
    wmove (scr, 16, col);	wprintw (scr, "###      ###");
    wmove (scr, 17, col);	wprintw (scr, "###      ###");
    wmove (scr, 18, col);	wprintw (scr, " ########## ");
    wmove (scr, 19, col);	wprintw (scr, "   ######   ");
  } else if (num == 9) {
    wmove (scr,  6, col);	wprintw (scr, "   ######   ");
    wmove (scr,  7, col);	wprintw (scr, " ########## ");
    wmove (scr,  8, col);	wprintw (scr, "###      ###");
    wmove (scr,  9, col);	wprintw (scr, "###      ###");
    wmove (scr, 10, col);	wprintw (scr, "###      ###");
    wmove (scr, 11, col);	wprintw (scr, "###      ###");
    wmove (scr, 12, col);	wprintw (scr, " ###########");
    wmove (scr, 13, col);	wprintw (scr, "  ######### ");
    wmove (scr, 14, col);	wprintw (scr, "       ###  ");
    wmove (scr, 15, col);	wprintw (scr, "      ###   ");
    wmove (scr, 16, col);	wprintw (scr, "     ###    ");
    wmove (scr, 17, col);	wprintw (scr, "    ###     ");
    wmove (scr, 18, col);	wprintw (scr, "   ###      ");
    wmove (scr, 19, col);	wprintw (scr, "  ###       ");
  }
}

big_nums (hrs, mins)
int hrs, mins;
{ int h1, h2, m1, m2;

  h1 = hrs / 10;
  h2 = hrs % 10;
  m1 = mins / 10;
  m2 = mins % 10;
      
  if (h1 != 0) {
    pr_space ();
    pr_big (h1, 9);
    pr_big (h2, 24);
    pr_colon (39);
    pr_big (m1, 45);
    pr_big (m2, 60);
  } else {
    pr_space ();
    pr_big (h2, 16);
    pr_colon (31);
    pr_big (m1, 37);
    pr_big (m2, 52);
  }
}

time_update ()
{ long clock;
  char time_str [27];
  int t_hour, t_min, t_sec;
  unsigned delta;

  sigset (SIGALRM, time_update);
  wmove (scr, 3, 0);
  wclrtoeol (scr);
  wmove (scr, 3, (COLS / 2 - strlen (myname) / 2));
  wprintw (scr, myname);

  time (&clock);
  wmove (scr, LINES - 2, 0);
  wclrtoeol (scr);
  strcpy (time_str, ctime (&clock));
  wmove (scr, LINES - 2, (COLS / 2 - strlen (time_str) / 2));
  wprintw (scr, time_str); 

  loc_time = localtime (&clock);
  t_hour = loc_time->tm_hour;
  if (t_hour > 12)
    t_hour = t_hour - 12;
  if (t_hour == 0)
    t_hour = 12;
  t_min = loc_time->tm_min;
  big_nums (t_hour, t_min);
  t_sec = loc_time->tm_sec;

  wmove (scr, LINES - 1, 0);
  wrefresh (scr);
  delta = refresh_time - (t_sec % refresh_time);
  if (delta != 0)
    alarm (delta);
  else
    alarm (refresh_time);
}

on_intr ()
{ done = TRUE;
}

main(argc, argv)
int argc;
char *argv [];
{ FILE *fd;

  if (argc == 1)
    refresh_time = 60;
  else if (argc == 2)
    refresh_time = atoi(argv [1]);
  else {
    printf ("Usage: %s [<refresh_time>]\n", argv [0]);
    exit (0);
  }
  fd = fopen("/etc/systemid","r");
  if (fd != NULL) {
    fgets(myname,sizeof myname,fd);
    myname[strlen(myname)] = '\0';
  }
  savetty ();
  initscr ();
  crmode ();
  noecho ();
  scr = newwin (LINES, COLS, 0, 0);
  scrollok (scr, FALSE);
  leaveok (scr, FALSE);
  sigset (SIGPIPE, SIG_IGN);
  sigset (SIGALRM, time_update);
  sigset (SIGINT, on_intr);
  wclear (scr);
  wstandout (scr);
  done = FALSE;
  time_update ();
  while (!done) {
    in_char = getchar ();
    if ((in_char != EOF) && (in_char != '\0'))
      done = TRUE;
  }
  wclear (scr);
  wmove (scr, LINES - 1, 0);
  wclrtoeol (scr);
  wrefresh (scr);
  wstandend (scr);
  echo ();
  nocrmode ();
  endwin ();
  resetty ();
}
