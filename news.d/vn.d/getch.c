/*
** vn news reader.
**
** getch.c - character i/o routines
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <setjmp.h>
#include "config.h"
#include "vn.h"

extern char Cxitop[];
extern char *Ku, *Kd, *Kr, *Kl;

/*
	getkey obtains user keystroke with count from leading
	numerics, if any.  Picks up arrow key sequences and maps
	them to other keys.  Also translates character through
	Cxitop array since this routine is only used in session
	loop.  Saves untranslating arrow keys.
*/
getkey (c)
char *c;
{
	int i, j;
	static char	ckseq[32];

	/* Check for leading count */
	for (i = 0; (*c = getchar() & 0x7f) >= '0' && *c <= '9'; i = i * 10 + *c - '0')
		;

	/* @#$!!! flakey front ends that won't map newlines in raw mode */
	if (*c == '\012' || *c == '\015')
		*c = '\n';

	/* @#$!!! flakey terminals which send control sequences for cursors! */
	if( *c == '\033' )
	{
		/*
		** Check if part of cursor key input sequence
		** (pitch unknown escape sequences)
		*/
		j = 0;
		ckseq[j] = *c; ckseq[j+1] = '\0';
		while(*c == Ku[j] || *c == Kd[j] || *c == Kl[j] || *c == Kr[j])
		{
			if( strcmp(ckseq, Ku) == 0 ) { *c = UP; break; }
			if( strcmp(ckseq, Kd) == 0 ) { *c = DOWN; break; }
#ifdef PAGEARROW
			if( strcmp(ckseq, Kl) == 0 ) { *c = BACK; break; }
			if( strcmp(ckseq, Kr) == 0 ) { *c = FORWARD; break; }
#else
			if( strcmp(ckseq, Kl) == 0 ) { *c = UP; break; }
			if( strcmp(ckseq, Kr) == 0 ) { *c = DOWN; break; }
#endif
			*c = (getchar() & 0x7f);
			ckseq[++j] = *c; ckseq[j+1] = '\0';
		}
	}
	else
		*c = Cxitop[*c];

	if (i <= 0)
		i = 1;
	return (i);
}

/*
	get user key ignoring most controls.  Used from reader and other
	non-screen interactions.
*/
getnoctl ()
{
	char c;
	while ((c = getchar() & 0x7f) < ' ' || c == '\177')
	{
		if (c == '\015' || c == '\012')
			c = '\n';
		if (c == '\n' || c == '\b' || c == '\t')
			return (c);
	}
	return ((int) c);
}
