/*
** vn news reader.
**
** sig_set.c - signal handler
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include <sys/signal.h>
#include <sgtty.h>
#include <setjmp.h>
#include "tty.h"
#include "config.h"
#include "brk.h"
#include "tune.h"
#include "node.h"
#include "page.h"

extern int L_allow;
extern char *Version;
extern char *Brk_fmt;
extern char *Vns_version;

static int Sigflag=BRK_INIT;	/* phase of interaction */
static FILE **Fpseek;		/* article reading file pointer pointer */
static int Foreground;
static jmp_buf Jumploc;		/* for BRK_SESS phase */

/*
	interrupt handler - unusual termination (longjmp and printex aborts)
	if not abort, remember to reset signal trap
	CAUTION - the passing of a jump buffer is a little dicey - assumes
	type jump_buf is an array.

	sigcatch and sig_set control a lot of i/o on stderr also, since
	it is so intimately related to signal interaction.  Note that the
	SIGTSTP action causes a "stopped on tty output" if raw terminal
	mode is restored by tty_set(RESTORE).  We don't get it if we were
	already cooked since tty_set avoids calling ioctl if it doesn't
	have to.
*/
static sigcatch (sig)
int sig;
{
	char buf [MAX_C+1];
	int pgrp;

	/* disable signal while processing it */
	signal (sig,SIG_IGN);

	switch (sig)
	{
	case SIGINT:
	case SIGQUIT:
		break;

#ifdef JOBCONTROL
	case SIGTSTP:
		/* ignore SIGTTOU so we don't get stopped if [kc]sh grabs the tty */
		signal(SIGTTOU, SIG_IGN);
		tty_set (SAVEMODE);
		term_set (MOVE,0,L_allow+RECBIAS-1);
		printf ("\n");
		Foreground = 0;
		fflush (stdout);
		fflush (stderr);
		signal(SIGTTOU, SIG_DFL);

		/* Send the TSTP signal to suspend our process group */
		signal(SIGTSTP, SIG_DFL);
		sigsetmask(0);
		kill (0, SIGTSTP);

		/* WE ARE NOW STOPPED */

		/*
				WELCOME BACK!
				if terminals process group is ours, we are foregrounded again
				and can turn newsgroup name printing back on
			*/
		tty_set (RESTORE);
		switch (Sigflag)
		{
		case BRK_SESS:
			signal (SIGTSTP,sigcatch);
			longjmp (Jumploc,1);
		case BRK_IN:
			ioctl (1,TIOCGPGRP,&pgrp);
			if (pgrp == getpgrp(0))
				Foreground = 1;
			break;
		default:
			break;
		}
		signal (SIGTSTP,sigcatch);
		return;
#endif
	default:
		printex (Brk_fmt,sig);
	}

	/* QUIT and INTERRUPT signals */
	switch (Sigflag)
	{
	case BRK_SESS:
		/* if in session, ask if really a quit, do longjump if not */
		term_set (ERASE);
		tty_set (RAWMODE);
		user_str (buf, BRK_PR, 1, "");
		if (buf[0] == 'y')
			printex (Brk_fmt,sig);
		signal (sig,sigcatch);
		longjmp (Jumploc,1);
	case BRK_READ:
		/* if reading seek file to end to abort page printing */
		printf ("\n");
		if (*Fpseek == NULL || fseek(*Fpseek,0L,2) < 0)
			putchar ('\07');
		break;
	default:
		printex (Brk_fmt,sig);
	}
	signal (sig,sigcatch);
}

/*
	sig_set controls what will be done with a signal when picked up by
	sigcatch.  fgprintf is included here to keep knowledge
	of TSTP state localized.
*/
/* VARARGS */
sig_set (flag,dat)
int flag, *dat;
{
	int i, *xfer, pgrp;
	if (Sigflag == BRK_INIT)
	{
		signal (SIGINT,sigcatch);
		signal (SIGQUIT,sigcatch);
		signal (SIGTERM,sigcatch);
#ifdef JOBCONTROL
		signal (SIGTSTP,sigcatch);
		ioctl (1,TIOCGPGRP,&pgrp);
		if (pgrp == getpgrp(0))
		{
			Foreground = 1;
			fgprintf ("Visual News, %s(%s), reading:\n",
					Version, Vns_version);
		}
		else
			Foreground = 0;
#else
		Foreground = NOJOB_FG;
#endif
	}
	switch (flag)
	{
	case BRK_IN:
	case BRK_OUT:
		Sigflag = flag;
		break;
	case BRK_READ:
		if (Sigflag != BRK_SESS)
			printex ("unexpected read state, sig_set\n");
		Fpseek = (FILE **) dat;
		Sigflag = BRK_READ;
		break;
	case BRK_SESS:
		xfer = (int *) Jumploc;
		for (i=0; i < sizeof(Jumploc) / sizeof(int); ++i)
			xfer[i] = dat[i];
		Sigflag = BRK_SESS;
		break;
	case BRK_RFIN:
		if (Sigflag != BRK_READ)
			printex ("unexpected finish state, sig_set\n");
		Sigflag = BRK_SESS;
		break;
	default:
		printex ("bad state %d, sig_set\n",flag);
	}
}

fgprintf (fs,a,b,c,d,e)
char *fs;
int a,b,c,d,e;
{
	if (Foreground)
		fprintf (stderr,fs,a,b,c,d,e);
	fflush (stderr);
}
