#include "adv.h"

/*
 * Initialize game variables.  Move to init room; carrying nothing;
 * light has full time; move items to initial rooms; set vars to 0.
 * Clear the base window and print the startup message.
 */
init()
{
	register int i;
	register struct item *ip;

	rm = i_rm;
	littim = litmax;
	for (ip = items; ip < iteme; ip++)
		ip->rm = ip->i_rm;
	for (i = 0; i < 256; ++i)
		var[i] = 0;
	isend = FALSE;

	CurWin = BaseWin;
	Wclear(BaseWin, 2);
	WAcursor(BaseWin, ROWS - 1, 0);
	prt(msg[0]);
	prt("\n");
}
