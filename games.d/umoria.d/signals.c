#include <curses.h>
#include <signal.h>
#include "constants.h"
#include "types.h"
#include "externs.h"

extern int total_winner;
extern int moria_flag;
extern int search_flag;

#ifdef USG
/* no local special characters */
#else
extern struct ltchars save_special_chars;
#endif
#ifdef ultrix
#define use_crmode
#endif
#ifdef M_XENIX
#define use_crmode
#endif

#ifndef SIG_T
#define SIG_T int
#endif
typedef SIG_T (*sig_t)();
int error_sig, error_code;
sig_t core_dump;
#ifdef USG
/* no suspend signal */
#else
sig_t suspend_handler;
#endif

/* This signal package was brought to you by           -JEW-  */

sig_t signal();
SIG_T signal_save_core();
SIG_T signal_save_no_core();
SIG_T signal_ask_quit();

init_signals()
{
  (void) signal(SIGHUP, signal_save_no_core);
  (void) signal(SIGINT, signal_ask_quit);
  core_dump = signal(SIGQUIT, signal_save_core);
  (void) signal(SIGILL, signal_save_core);
  (void) signal(SIGTRAP, signal_save_core);
#ifndef SIGIOT
#define SIGIOT SIGABRT
#endif
  (void) signal(SIGIOT, signal_save_core);
  (void) signal(SIGEMT, signal_save_core);
  (void) signal(SIGFPE, signal_save_core);
  (void) signal(SIGKILL, signal_save_core);
  (void) signal(SIGBUS, signal_save_core);
  (void) signal(SIGSEGV, signal_save_core);
  (void) signal(SIGSYS, signal_save_core);
  (void) signal(SIGTERM, signal_save_core);
}

/*ARGSUSED*/
#ifdef USG
SIG_T
signal_save_core(sig)
int sig;
{
  error_sig = sig;
  error_code = 0;
  prt("OH NO!!!!!!!!!!  Attempting panic save.", 23, 0);
  save_char(FALSE, FALSE);
  (void) signal(SIGQUIT, SIG_DFL);
  /* restore terminal settings */
#ifndef BUGGY_CURSES
  nl();
#endif
#ifdef use_crmode
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  resetterm();
  /* restore the saved values of the local special chars */
  /* no local special characters */
  /* allow QUIT signal */
  /* nothing needs to be done here */
  (void) kill(getpid(), 3);
  exit_game();
}
#else
SIG_T
signal_save_core(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  error_sig = sig;
  error_code = code;
  prt("OH NO!!!!!!!!!!  Attempting panic save.", 23, 0);
  save_char(FALSE, FALSE);
  (void) signal(SIGQUIT, SIG_DFL);
  /* restore terminal settings */
#ifndef BUGGY_CURSES
  nl();
#endif
#ifdef use_crmode
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  /* restore the saved values of the local special chars */
  (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
  /* allow QUIT signal */
  (void) sigsetmask(0);
  (void) kill(getpid(), 3);
  exit_game();
}
#endif

/*ARGSUSED*/
#ifdef USG
SIG_T
signal_save_no_core(sig)
int sig;
{
  error_sig = sig;
  error_code = 0;
  save_char(FALSE, TRUE);
  exit_game();
}
#else
SIG_T
signal_save_no_core(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  error_sig = sig;
  error_code = code;
  save_char(FALSE, TRUE);
  exit_game();
}
#endif

/*ARGSUSED*/
#ifdef USG
SIG_T
signal_ask_quit(sig)
int sig;
{
  char command;

  /* reset signal handler */
  (void) signal(sig, signal_ask_quit);
  if (get_com("Do you really want to quit?", &command))
    switch(command)
      {
      case 'y': case 'Y':
        if (character_generated)
	  upon_death();
        else
          exit_game();
	break;
      }
  find_flag = FALSE;
  if (search_flag)
    search_off();
  if (py.flags.rest > 0)
    rest_off();
  erase_line(msg_line, msg_line);
}
#else
SIG_T
signal_ask_quit(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  char command;

  /* no need to reset signal handler */
  if (get_com("Do you really want to quit?", &command))
    switch(command)
      {
      case 'y': case 'Y':
	if (character_generated)
	  upon_death();
	else
	  exit_game();
	break;
      }
  find_flag = FALSE;
  if (search_flag)
    search_off();
  if (py.flags.rest > 0)
    rest_off();
  erase_line(msg_line, msg_line);
}
#endif

no_controlz()
{
#ifdef USG
  /* no suspend signal */
#else
  suspend_handler = signal(SIGTSTP, SIG_IGN);
#endif
}

controlz()
{
#ifdef USG
  /* no suspend signal */
#else
  (void) signal(SIGTSTP, suspend_handler);
#endif
}
