/*
 * Example program.  Split the screen into three windows, input using the
 * top window until an escape is typed, and show it in the bottom window.
 * The middle window is just a border.  Continue until a ^E is typed.
 */

#include <signal.h>

#define	BOARDER	10		/* row for boarder window */
#define	BUFSIZE	1000		/* maximum chars which can be input */
#define	ESC	'\033'		/* escape character */
#define	QUIT	'\005'		/* quit character (^E) */

int	grabchar();		/* routine to read tty chars */
int	quit();			/* interrupt routine */

main()
{
	register int	i;		/* character count */
	char	buf[BUFSIZE];		/* input buffer */

	signal(SIGINT, quit);
	if (dpyinit((char *)0, "-ec"))
		exit(1);
	dpywindow(BOARDER, BOARDER, 0, -1);
	while (dpywrite("-----", 5) == 0) ;
	while (1) {
		dpywindow(0, BOARDER - 1, 0, -1);
		i = dpyread("Input: ", grabchar, buf, sizeof(buf));
		if ((i > 0) && (buf[i-1] == QUIT)) break;
		dpywindow(BOARDER + 1, -1, 0, -1);
		dpyprintf("Read %d chars:\n", i);
		dpywrite(buf, i);
		dpyclrwindow();
		dpyupdate();
	}
	dpyclose();
}


/*
 * Read next char from tty, quitting on an end of file or escape character.
 * The escape character is removed from the buffer, but the end of file
 * character is kept.
 */
grabchar(oldch)
{
	unsigned char	newch;

	if ((oldch == QUIT) || (read(0, &newch, 1) != 1) || (newch == ESC))
		return(-1);
	return(newch);
}


/*
 * Here on an interrupt.
 */
quit()
{
	dpyclose();
	exit(0);
}
