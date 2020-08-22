/*
** vn news reader.
**
** help.c - print help
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <setjmp.h>
#include "config.h"
#include "tty.h"
#include "tune.h"
#include "node.h"
#include "page.h"
#include "reader.h"
#include "vn.h"

extern int L_allow;
extern int C_allow;
extern int Digest;
extern char *Contstr;
extern char Cxptoi[], Cxrtoi[];

/*
	Help message table.  Character for command, plus its help
	message.  Table order is order of presentation to user.
*/
static struct HELPTAB
{
	char cmd, *msg;
	int dig;
	char *amsg;
} 
Helptab [] =
{
	{ QUIT, "quit", 1, NULL},
	{ UP, "(or up arrow) move up [number of lines]", 1, NULL},
	{ DOWN, "(or down arrow) move down [number of lines]", 1, NULL},
#ifdef PAGEARROW
	{ BACK, "(or left arrow) previous page [number of pages]", 1, NULL},
	{ FORWARD, "(or right arrow) next page [number of pages]", 1, NULL},
#else
	{ BACK, "previous page [number of pages]", 1, NULL},
	{ FORWARD, "next page [number of pages]", 1, NULL},
#endif
	{ TOPMOVE, "move to top of page", 1, NULL},
	{ BOTMOVE, "move to bottom of page", 1, NULL},
	{ ALTBOTTOM, "move to bottom of page (alternate L)", 1, NULL},
	{ MIDMOVE, "move to middle of page", 1, NULL},
	{ DIGEST, "unpack digest", 1, "exit digest"},
	{ READ, "read article [number of articles]", 1, NULL},
	{ ALTREAD, "read article (alternate 'r')", 1, NULL},
	{ READALL, "read all articles on page", 1, NULL},
	{ READSTRING, "specify articles to read", 1, NULL},
	{ SAVE, "save or pipe article [number of articles]", 1, NULL},
	{ SAVEALL, "save or pipe all articles on page", 1, NULL},
	{ SAVESTRING, "specify articles to save", 1, NULL},
	{ ALTSAVE, "specify articles to save (alternate ctl-s)", 1, NULL},
	{ PRINT, "print article [number of articles]", 1, NULL},
	{ PRINTALL, "print all article on page", 1, NULL},
	{ PRINTSTRING, "specify articles to print", 1, NULL},
	{ UPDATE, "update .newsrc status to cursor", 0, NULL},
	{ UPALL, "update .newsrc status for whole newsgroup", 0, NULL},
	{ UPSEEN, "update .newsrc status for all pages displayed", 0, NULL},
	{ ORGGRP, "recover original .newsrc status for newsgroup", 0, NULL},
	{ ORGSTAT, "recover all original .newsrc status", 0, NULL},
	{ SSTAT, "display count of groups and pages - shown and total", 0, NULL},
	{ GRPLIST, "list newsgroups with new article, updated counts", 0, NULL},
	{ NEWGROUP, "specify newsgroup to display and/or resubscribe to", 1, NULL},
	{ UNSUBSCRIBE, "unsubscribe from group", 0, NULL},
	{ MARK, "mark/unmark article [number of articles]", 1, NULL},
	{ ART_MARK, "mark/unmark article [number of articles]", 1, NULL},
	{ UNMARK, "erase marks on articles", 1, NULL},
	{ HEADTOG, "toggle flag for display of headers when reading", 1, NULL},
	{ SETROT, "toggle rotation for reading", 1, NULL},
	{ REDRAW, "redraw screen", 1, NULL},
	{ UNESC, "escape to UNIX to execute a command", 1, NULL},
	{ PRTVERSION, "show vn version", 1, NULL},
	{ HELP, "show this help menu", 1, NULL}
};

#define HTSIZE (sizeof(Helptab)/sizeof(struct HELPTAB))

/*
	help from main screen
*/
help ()
{
	int i,lcount,lim; 
	term_set (ERASE);
	lim = L_allow + RECBIAS - 2;
	printf("%s\n",HELP_HEAD);
	lcount = HHLINES;
	for (i=0; i < (sizeof(Helptab))/(sizeof(struct HELPTAB)); ++i)
	{
		if (Digest && !(Helptab[i].dig))
			continue;
		++lcount;
		if (Digest && Helptab[i].amsg != NULL)
			h_print (Cxptoi[Helptab[i].cmd],Helptab[i].amsg);
		else
			h_print (Cxptoi[Helptab[i].cmd],Helptab[i].msg);
		if (lcount >= lim)
		{
			printf ("\n%s",Contstr);
			getnoctl ();
			term_set (MOVE,0,lim+1);
			term_set (ZAP,0,strlen(Contstr));
			term_set (MOVE,0,lim-1);
			putchar ('\n');
			lcount = 0;
		}
	}
	if (lcount > 0)
	{
		printf ("\n%s",Contstr);
		getnoctl ();
	}
}

/*
	help from reader
*/
help_rd()
{
	h_print (Cxrtoi[PG_NEXT],HPG_NEXT);
	h_print (Cxrtoi[PG_QUIT],HPG_QUIT);
	h_print (Cxrtoi[PG_FLIP],HPG_FLIP);
	h_print (Cxrtoi[PG_REWIND],HPG_REWIND);
	h_print (Cxrtoi[PG_WIND],HPG_WIND);
	h_print (Cxrtoi[PG_SEARCH],HPG_SEARCH);
	h_print (Cxrtoi[PG_STEP],HPG_STEP);
	h_print (Cxrtoi[PG_REPLY],HPG_REPLY);
	h_print (Cxrtoi[PG_FOLLOW],HPG_FOLLOW);
	h_print (Cxrtoi[SAVE],HPG_SAVE);
	h_print (Cxrtoi[PRINT],HPG_PRINT);
	h_print (Cxrtoi[SETROT],HPG_ROT);
	h_print (Cxrtoi[HEADTOG],HPG_HEAD);
	h_print (Cxrtoi[PG_HELP],HPG_HELP);
	printf ("%s\n",HPG_DEF);
}

srch_help(c,dig)
char c;
int *dig;
{
	int i;

	for (i=0; i < HTSIZE; ++i)
		if (Helptab[i].cmd == c)
			break;
	if (i < HTSIZE)
	{
		*dig = Helptab[i].dig;
		return (0);
	}
	return(-1);
}

/*
	h_print prints a character and a legend for a help menu.
*/
static
h_print(c,s)
char c,*s;
{
	if (strlen(s) > (C_allow - 14))
		s [C_allow - 14] = '\0';
	if (c > ' ' && c != '\177')
		printf ("	 %c - %s\n",c,s);
	else
	{
		switch (c)
		{
		case '\177':
			printf ("  <delete> - %s\n",s);  
			break;
		case '\040':
			printf ("   <space> - %s\n",s);  
			break;
		case '\033':
			printf ("  <escape> - %s\n",s);  
			break;
		case '\n':
			printf ("  <return> - %s\n",s);  
			break;
		case '\t':
			printf ("     <tab> - %s\n",s);  
			break;
		case '\b':
			printf (" <back sp> - %s\n",s);  
			break;
		case '\f':
			printf ("<formfeed> - %s\n",s);  
			break;
		case '\07':
			printf ("    <bell> - %s\n",s);  
			break;
		case '\0':
			printf ("    <null> - %s\n",s);  
			break;
		default:
			if (c < '\033')
			{
				c += 'a' - 1;
				printf(" control-%c - %s\n",c,s);
			}
			else
				printf("       %c0%o - %s\n",'\\',(int) c,s);
			break;
		}
	}
}
