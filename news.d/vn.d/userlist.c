/*
** vn news reader.
**
** userlist.c - generate user's list of articles
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "tune.h"
#include "node.h"
#include "page.h"
#include "vn.h"

extern PAGE Page;
extern char *List_sep;

static char Pattern[MAX_C] = "";

/*
	generate user list of articles - either article numbers
	are input directly (numeric list), or input is a search
	string - invoke regular expression library and examine titles
	search string "*" reserved for marked articles.  Strings may
	be prefixed with '!' for negation.
*/
userlist (list)
char *list;
{
	int i,j,anum[RECLEN/2],acount;
	char neg, *s, sbuf[MAX_C+1], *reg, *regex(), *regcmp(), *index(), *strtok();

	user_str (sbuf,"Articles or title search string : ",1,Pattern);
	if (sbuf[0] == '!')
	{
		neg = '!';
		s = sbuf+1;
	}
	else
	{
		neg = '\0';
		s = sbuf;
	}
	for (i=0; s[i] != '\0'; ++i)
	{
		if (index(List_sep,s[i]) == NULL)
		{
			if (s[i] < '0' || s[i] > '9')
				break;
		}
	}
	acount = 0;

	if (s[i] == '\0')
	{
		for (s = strtok(s,List_sep); s != NULL; s = strtok(NULL,List_sep))
		{
			anum[acount] = atoi(s);
			++acount;
		}
	}
	else
	{
		/* we save old input only if NOT a list of article numbers */
		strcpy(Pattern,sbuf);
		if (s[0] == ART_MARK)
		{
			for (i=0; i < Page.h.artnum; ++i)
			{
				if (Page.b[i].art_mark == ART_MARK)
				{
					anum[acount] = Page.b[i].art_id;
					++acount;
				}
			}
		}
		else
		{
			reg = regcmp(s,(char *) 0);
			if (reg != NULL)
			{
				for (i=0; i < Page.h.artnum; ++i)
				{
					if (regex(reg,Page.b[i].art_t) != NULL)
					{
						anum[acount] = Page.b[i].art_id;
						++acount;
					}
				}
				regfree (reg);
			}
			else
				preinfo ("bad regular expression syntax");
		}
	}

	/* algorithm is inefficient, but we're only handling a few numbers */
	*list = '\0';
	for (i=0; i < Page.h.artnum; ++i)
	{
		for (j=0; j < acount && anum[j] != Page.b[i].art_id; ++j)
			;
		if (neg == '!')
		{
			if (j < acount)
				continue;
		}
		else
		{
			if (j >= acount)
				continue;
		}
		sprintf (list,"%d ",Page.b[i].art_id);
		list += strlen(list);
	}
}
