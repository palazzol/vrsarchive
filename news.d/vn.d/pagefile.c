/*
** vn news reader.
**
** pagefile.c - routines to deal with page display tempfile
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>

#ifdef SYSV
#include <sys/types.h>
#include <fcntl.h>
#endif

#include <sys/file.h>

#include "tune.h"
#include "node.h"
#include "page.h"

extern int Ncount,Lrec,L_allow,Cur_page,C_allow;
extern NODE **Newsorder;
extern PAGE Page;
extern int Digest;

extern char *Aformat;

extern char *T_head, *F_head, *L_head;

static int Tdes;	/* temp file descriptor */
static int Pgsize;	/* block size for seeking file */

static NODE *Curgp = NULL;	/* current newsgroup being written */
static int Order = 0;		/* order counter */

/*
	routines which deal with the temp file containing
	display pages.  Note the "invisible" file feature -
	tempfile is unlinked from /usr/tmp immediately.  when
	Tdes is closed by UNIX the disk space will be given back.
*/

temp_open ()
{
	char tmpart [L_tmpnam];
	Lrec = -1;
	tmpnam (tmpart);
	Pgsize = sizeof (HEAD) + L_allow * sizeof(BODY);
	if ((Tdes = open(tmpart,O_RDWR|O_CREAT)) < 0)
		printex ("can't open %s",tmpart);
	unlink (tmpart);
}

/*
** set newsgroup for tempfile write
*/
fw_group(ng,new,sub,rd,look)
char *ng;
int new;
int sub;
int rd;
int look;
{
	NODE *hashfind();

	if (Curgp != NULL && Page.h.artnum > 0)
		fw_flush();
	
	if ((Curgp = hashfind(ng)) == NULL)
		printex("fw_group - non-existent newsgroup, \"%s\"",ng);
	if (Curgp->order >= 0)
		printex("fw_group - repeat call on newsgroup, \"%s\"",ng);
	Curgp->order = Order;
	++Order;
	fw_chg(new,sub,rd,look);
	Curgp->pages = 0;
	Curgp->pnum = Lrec+1;
	Page.h.name = Curgp->nd_name;
	Page.h.group = Curgp;
	Page.h.artnum = 0;
}

fw_chg(new,sub,rd,look)
int new;
int sub;
int rd;
int look;
{
	Curgp->flags &= ~(FLG_NEW|FLG_SUB|FLG_SEARCH);
	if (new)
		Curgp->flags |= FLG_NEW;
	if (sub)
		Curgp->flags |= FLG_SUB;
	if (look)
		Curgp->flags |= FLG_SEARCH;
	Curgp->rdnum = Curgp->orgrd = Curgp->pgrd = rd;
}

/*
** write article to temp file.
*/
fw_art(anum,subj,lines,author)
int anum;
char *subj;
char *lines;
char *author;
{
	char tbuf[RECLEN];
	int idx;

	form_title(tbuf,subj,lines,author,anum);
	idx = Page.h.artnum;
	strcpy((Page.b)[idx].art_t, tbuf);
	(Page.b)[idx].art_id = anum;
	(Page.b)[idx].art_mark = ' ';

	++(Page.h.artnum);
	if (Page.h.artnum >= L_allow)
		fw_flush();
}

fw_done()
{
	if (Curgp != NULL && Page.h.artnum > 0)
	{
		/* correct if server was lying at fw_group() */
		Curgp->flags |= FLG_SEARCH;
		fw_flush();
	}
}

static
fw_flush()
{
	++(Curgp->pages);
	++Lrec;
	Curgp->flags |= FLG_PAGE;
	do_write();
	Page.h.artnum = 0;
}

find_page (n)
int n;
{
	long off,lseek();
	int i,last;
	Cur_page = n;
	off = Pgsize;
	off *= (long) n;
	lseek (Tdes, off, 0);
	if (read(Tdes, (char *) &(Page.h), sizeof(HEAD)) < sizeof(HEAD))
		printex("bad temp file read");
	i = Pgsize - sizeof(HEAD);
	if (read(Tdes, (char *) Page.b, i) < i)
		printex("bad temp file read");
	last = -1;
	for (i=0; i < Ncount; ++i)
	{
		if ((Newsorder[i])->pages > 0)
		{
			if ((Newsorder[i])->pnum > n)
				break;
			last = i;
		}
	}
	if (last < 0)
		printex ("can't find page %d",n);
	Page.h.group = Newsorder[last];
	Page.h.name = (Page.h.group)->nd_name;
	vns_gset(Page.h.name);
}

write_page ()
{
	long off,lseek();
	if (!Digest)
	{
		off = Pgsize;
		off *= (long) Cur_page;
		lseek (Tdes, off, 0);
		do_write();
	}
}

static do_write()
{
	int num;

	if (write(Tdes, (char *) &(Page.h), sizeof(HEAD)) < sizeof(HEAD))
		printex ("Bad temp file write");
	num = L_allow * sizeof(BODY);
	if (write(Tdes, (char *) Page.b, num) < num)
		printex ("Bad temp file write");
}

form_title (t,fn,fl,ff,n)
char *t,*fn,*fl,*ff;
int n;
{
	char *ptr,*index();
	int i;

	if ((ptr = index(ff,'(')) != NULL && strlen(ptr) > 3)
		ff = ptr;
	sprintf (t,TFORMAT,fn,fl,ff);
	sprintf(ff,Aformat,' ',' ',n);
	i = C_allow - strlen(ff) + 1;	/* remember newline in Aformat */
	t[i] = '\0';
	ctl_xlt(t);
	return (0);
}

/* replace control characters in titles */
static ctl_xlt(s)
char *s;
{
	while (*s != '\0')
	{
		if (*s < ' ')
			*s += 'A' - 1;
		++s;
	}
}
