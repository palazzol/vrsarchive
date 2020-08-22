/*
** vn news reader.
**
** hash.c - hash table routines
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "config.h"
#include "tune.h"
#include "node.h"

/*
** hash table manipulation routines:
*/

extern int Ncount;
extern NODE **Newsorder;

static NODE *Tab [HASHSIZE];	/* hash Table */

hashinit ()
{
	int i;
	for (i=0; i < HASHSIZE; ++i)
		Tab[i] = NULL;
	Ncount = 0;
}

/*
	enter new node (name s, articles n, low l) in hash Table, 
	initial flags = 0.  Set order to -1.
*/
NODE *hashenter(s,n,l)
char *s;
int n;
int l;
{
	char *str_store();
	NODE *ptr,*node_store();
	int i;

	i=hash(s);
	ptr = node_store();
	ptr->next = Tab[i];
	Tab[i] = ptr;
	if (l > n)
		l = n;
	++Ncount;
	ptr->lownum = l;
	ptr->state = 0;
	ptr->data = NULL;
	ptr->flags = 0;
	ptr->highnum = n;
	ptr->nd_name = str_store(s);
	ptr->pgshwn = 0;
	ptr->order = -1;
	return (ptr);
}

NODE *hashfind(s)
char *s;
{
	NODE *ptr;

	for (ptr = Tab[hash(s)]; ptr != NULL && strcmp(ptr->nd_name,s) != 0;
					ptr = ptr->next)
		    ;
	return (ptr);
}

make_newsorder()
{
	char *malloc();
	int i;
	NODE *ptr;

	if ((Newsorder = (NODE **) malloc(Ncount * sizeof(NODE))) == NULL)
		printex("Memory allocation failure - newsorder array");
	for (i=0; i < Ncount; ++i)
		Newsorder[i] = NULL;
	for (i=0; i < HASHSIZE; ++i)
	{
		for (ptr = Tab[i]; ptr != NULL; ptr = ptr->next)
		{
			if (ptr->order < 0 || ptr->order >= Ncount)
				printex("News order range error");
			Newsorder[ptr->order] = ptr;
		}
	}
	for (i=0; i < Ncount; ++i)
		if (Newsorder[i] == NULL)
			printex("News order duplication error");
}

static hash (s)
char *s;
{
	unsigned rem;
	for (rem=0; *s != '\0'; ++s)
		rem = (rem*128 + (*s&0x7f)) % HASHSIZE;
	return (rem);
}
