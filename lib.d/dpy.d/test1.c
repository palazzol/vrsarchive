/*
 * Example program.  Randomly fill up the screen with numbers until
 * it all turns to asterisks.
 */

#ifdef USG
#define	random	rand
#endif USG

main()
{
	register int row, col, ch;
	register int rows, cols;

	if (dpyinit((char *)0, "e")) exit(1);
	dpymove(-1, -1);
	rows = dpygetrow() + 1;
	cols = dpygetcol() + 1;
	dpyhome();
	while (1) {
		dpyupdate();
		row = random() % rows;
		col = random() % cols;
		ch = dpyget(row, col);
		if (ch == ' ') ch = '1';
		else if (ch == '9') ch = '*';
		else if (ch != '*') ch++;
		dpyplace(row, col, ch);
	}
}
