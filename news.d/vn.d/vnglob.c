/*
** vn news reader.
**
** vnglob.c - global variables - see string.c also
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "config.h"
#include "head.h"
#include "tune.h"
#include "node.h"
#include "page.h"

/*
	global data structure
*/
NODE **Newsorder = NULL;	/* in order of fw_group calls */

char *Editor, *Ps1, *Printer;

int (*Massage)() = NULL;
int (*Headedit)() = NULL;
int (*Postfunc)() = NULL;
int (*Mailfunc)() = NULL;

char Erasekey, Killkey;		/* user keys from stty */
char *Orgdir;			/* .newsrc file, and original pwd */
char *Savefile = DEF_SAVE;	/* file in which to save articles */
char *Savedir;			/* default directory for saved articles */
char *Ccfile;			/* author_copy file, stored /bin/mail fmt */
char *Home;			/* user's home */

int Rot;	/* rotation */
int Headflag;	/* header printing flag */
int Digest;	/* if non-zero, digest article */

char *Ku, *Kd, *Kl, *Kr;	/* Cursor movement capabilities */

/* character translation arrays for commands */
char Cxitop[128], Cxitor[128], Cxrtoi[128], Cxptoi[128];

/*
	cur_page - current page displayed;
	lrec - last record
	l_allow - lines allowed for article display
	c_allow - columns allowed
	ncount = newsorder index
*/
int Cur_page, Lrec, L_allow, C_allow, Ncount;

int Nounsub, Listfirst;
/*
	current page
*/
PAGE Page;
