/*
** vn news reader.
**
** storage.c - storage allocation routines
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "tune.h"
#include "node.h"
#include "page.h"

extern char *malloc();

extern int L_allow;

extern PAGE Page;
/*
	Storage allocaters.
*/

char *str_store (s)
char *s;
{
	static unsigned av_len = 0;	/* current storage available */
	static char *avail;
	int len;

	if (s == NULL)
		s = "";

	if ((len = strlen(s)+1) > av_len)
	{
		if (len > STRBLKSIZE)
			av_len = len;
		else
			av_len = STRBLKSIZE;
		if ((avail = malloc(av_len)) == NULL)
			printex ("can't allocate memory for string storage");
	}
	strcpy (avail,s);
	s = avail;
	avail += len;
	av_len -= len;
	return (s);
}

/*
** called after number of terminal lines (L_allow) is known, to set
** up storage for Page.
*/
page_alloc ()
{
	char *body;

	if ((body = malloc(L_allow*sizeof(BODY))) == NULL)
		printex ("can't allocate memory for display storage");

	Page.b = (BODY *) body;
}

NODE
*node_store()
{
	static int nd_avail = 0;
	static NODE *nd;
	NODE *ret;

	if (nd_avail <= 0)
	{
		if ((nd = (NODE *) malloc(sizeof(NODE)*NDBLKSIZE)) == NULL)
			printex ("can't allocate memory for newsgroup table");
		nd_avail = NDBLKSIZE;
	}
	--nd_avail;
	ret = nd;
	++nd;
	return(ret);
}

/*
** temp string storage
*/

typedef struct
{
	int len;
	int idx;
	char **ptr;
} STRINGPOOL;

char *
str_tpool(n)
int n;
{
	int size;
	STRINGPOOL *p;

	size = sizeof(STRINGPOOL) + n * sizeof(char **);

	if ((p = (STRINGPOOL *) malloc(size)) == NULL)
		printex("Cannot allocate temporary string storage");

	p->ptr = (char **)(p+1);
	p->len = n;
	p->idx = 0;

	return((char *) p);
}

char *
str_tstore(cp,s)
char *cp;
char *s;
{
	STRINGPOOL *p;
	int len;

	p = (STRINGPOOL *) cp;
	if (p->idx >= p->len)
		printex("Temporary string storage overflow");
	len = strlen(s)+1;
	if ((cp = malloc(len)) == NULL)
		printex("Cannot allocate copy of string");
	strcpy(cp,s);
	(p->ptr)[p->idx] = cp;
	++(p->idx);

	return(cp);
}

char **
str_taptr(cp)
char *cp;
{
	STRINGPOOL *p;

	p = (STRINGPOOL *) cp;

	return (p->ptr + p->idx);
}

str_tfree(cp)
char *cp;
{
	STRINGPOOL *p;
	int i;

	p = (STRINGPOOL *) cp;
	for (i=0; i < p->idx; ++i)
		free((p->ptr)[i]);
	free (cp);
}
