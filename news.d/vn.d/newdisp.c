/*
** vn news reader.
**
** newgroups.c - display list of new groups since user's last ession.
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "config.h"
#include "tty.h"
#include "node.h"

extern NODE **Newsorder;
extern int Ncount, Lrec, C_allow;

new_groups ()
{
	int i,wrem,w;
	int max;
	char fs[24],c_end;

	max = 0;
	for (i=0; i < Ncount; ++i)
		if (((Newsorder[i])->flags & FLG_NEW) != 0 &&
				(w = strlen((Newsorder[i])->nd_name)) > max)
			max = w;
	sprintf (fs,"%%-%ds%%c",max);

	if (max <= 0)
		return (0);

	term_set (ERASE);
	printf ("New newsgroups:\n");

	wrem = C_allow;
	for (i=0; i < Ncount; ++i)
	{
		if (((Newsorder[i])->flags & FLG_NEW) == 0)
			continue;
		if ((wrem -= max) < max)
		{
			wrem = C_allow;
			c_end = '\n';
		}
		else
			c_end = ' ';
		printf (fs,(Newsorder[i])->nd_name,c_end);
	}
	if (c_end != '\n')
		putchar ('\n');

	return (1);
}
