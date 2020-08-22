/*
 * include.h - includes all important files for Phantasia
 */

#include <ctype.h>
#include <curses.h>
#ifndef A_REVERSE	/* No terminfo */
#define flushinp()	(raw(), noraw(), crmode())
#endif
#include <math.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

#include "macros.h"
#include "phantdefs.h"
#include "phantstruct.h"
#include "phantglobs.h"
