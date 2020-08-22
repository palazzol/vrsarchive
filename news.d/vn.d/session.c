/*
** vn news reader.
**
** session.c - top session loop
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <setjmp.h>
#include "config.h"
#include "tty.h"
#include "brk.h"
#include "head.h"
#include "tune.h"
#include "node.h"
#include "page.h"
#include "vn.h"

extern NODE **Newsorder;
extern char Erasekey, Killkey;
extern int Rot;
extern char *Ps1,*Printer;
extern char *Orgdir,*Savefile;
extern int Ncount, Cur_page, Lrec, L_allow, C_allow;
extern int Headflag;
extern PAGE Page;
extern int Digest;
extern char *No_msg;
extern char *Hdon_msg;
extern char *Hdoff_msg;
extern char *Roton_msg;
extern char *Rotoff_msg;
extern char Cxitop[], Cxptoi[];
extern char *Aformat;
extern char *Contstr;
extern char *Kl,*Kr,*Ku,*Kd;
extern int Nounsub, Listfirst;
extern char *List_sep;
extern char *Version, *Vns_version;

extern int (*Headedit)();

static int C_info;
static int Dskip, Drec;

static char *Unsub_msg = "Unsubscribed";
static char *Egroup_msg = "Entire newsgroup";

static int Crec;
static int Highrec;

/*
	main session handler processing input commands

	NOTE: this is where a setjmp call is made to set the break reentry
		location.  Keep the possible user states in mind.
*/
session ()
{
	char alist [RECLEN], c;
	int newg, i, count;
	jmp_buf brkbuf;

	newg = new_groups();
	tty_set (RAWMODE);
	find_page (0);
	Digest = 0;

	/* reentry point for break from within session interaction */
	setjmp (brkbuf);
	sig_set (BRK_SESS,brkbuf);
	Headflag = FALSE;
	Rot = 0;

	/* done this way so that user gets "really quit?" break treatment */
	if (newg > 0)
	{
		printf ("\n%s",Contstr);
		getnoctl();
		newg = 0;
	}

	/* list preview option - clear after first time for long jumps */
	if (Listfirst)
	{
		/* tot_list settings will be overwritten in this case */
		tot_list();
		Listfirst = 0;
	}

	/* if breaking from a digest, recover original page */
	if (Digest)
	{
		find_page(Cur_page);
		Digest = 0;
	}
	show ();
	Crec = RECBIAS;
	Highrec = Page.h.artnum + RECBIAS;
	term_set (MOVE,0,Crec);

	/*
		handle commands until QUIT, update global/local status
		and display for each.
	 */
	for (count = getkey(&c); c != QUIT; count = getkey(&c))
	{
		if ( srch_help(c,&i) != 0 || (Digest != 0 && i == 0))
		{
			preinfo (UDKFORM,Cxptoi[HELP]);
			term_set (MOVE, 0, Crec);
			continue;
		}

		switch (c)
		{
		case HEADTOG:
			if (Headedit != NULL)
			{
				term_set(ERASE);
				(*Headedit)();
				show();
				term_set (MOVE,0,Crec);
				break;
			}
			if (Headflag)
			{
				Headflag = FALSE;
				prinfo (Hdoff_msg);
			}
			else
			{
				Headflag = TRUE;
				prinfo (Hdon_msg);
			}
			term_set (MOVE,0,Crec);
			break;
		case SETROT:
			if (Rot == 0)
			{
				Rot = 13;
				prinfo (Roton_msg);
			}
			else
			{
				Rot = 0;
				prinfo (Rotoff_msg);
			}
			term_set (MOVE,0,Crec);
			break;
		case SSTAT:
			count_msg ();
			term_set (MOVE,0,Crec);
			break;
		case GRPLIST:
			tot_list ();
			show();
			term_set (MOVE,0,Crec);
			break;
		case REDRAW:
			show();
			term_set (MOVE,0,Crec);
			break;
		case UNSUBSCRIBE:
			new_sub(Page.h.group,0);
			do_update(Unsub_msg);
			term_set (MOVE,0,Crec);
			break;

		case UPDATE:
			new_read(Page.h.group,Page.b[Crec-RECBIAS].art_id);
			wr_show ();
			do_update("Updated to cursor");
			term_set (MOVE,0,Crec);
			break;
		case UPALL:
			new_read(Page.h.group,(Page.h.group)->highnum);
			wr_show();
			do_update(Egroup_msg);
			term_set (MOVE,0,Crec);
			break;
		case ORGGRP:
			new_read(Page.h.group,(Page.h.group)->orgrd);
			wr_show();
			do_update(Egroup_msg);
			term_set (MOVE,0,Crec);
			break;
		case UPSEEN:
			up_seen();
			wr_show();
			do_update("All pages displayed to this point updated");
			term_set (MOVE,0,Crec);
			break;
		case ORGSTAT:
			for (i = 0; i < Ncount; ++i)
				new_read(Newsorder[i],(Newsorder[i])->orgrd);
			wr_show();
			do_update("Original data recovered");
			term_set (MOVE,0,Crec);
			break;
 		case TOPMOVE:
 			Crec = RECBIAS;
 			term_set (MOVE, 0, Crec);
 			break;
 		case BOTMOVE:
 		case ALTBOTTOM:
 			Crec = Highrec - 1;
 			term_set (MOVE, 0, Crec);
 			break;
 		case MIDMOVE:
 			Crec = (RECBIAS + Highrec - 1) / 2;
 			if (Crec < RECBIAS)
 				Crec = RECBIAS;
 			if (Crec >= Highrec)
 				Crec = Highrec - 1;
 			term_set (MOVE, 0, Crec);
 			break;
		case UP:
			if (Crec != RECBIAS)
			{
				Crec -= count;
				if (Crec < RECBIAS)
					Crec = RECBIAS;
				term_set (MOVE, 0, Crec);
			}
			else
				putchar ('\07');
			break;
		case DOWN:
			if (Crec < (Highrec - 1))
			{
				Crec += count;
				if (Crec >= Highrec)
					Crec = Highrec - 1;
				term_set (MOVE, 0, Crec);
			}
			else
				putchar ('\07');
			break;
		case MARK:
		case ART_MARK:
			count += Crec - 1;
			if (count >= Highrec)
				count = Highrec - 1;
			for (i=Crec; i <= count; ++i)
			{
				if (Page.b[i-RECBIAS].art_mark != ART_MARK)
					Page.b[i-RECBIAS].art_mark = ART_MARK;
				else
					Page.b[i-RECBIAS].art_mark = ' ';
				if (i != Crec)
					term_set (MOVE, 0, i);
				printf ("%c\010",Page.b[i-RECBIAS].art_mark);
			}
			if (count != Crec)
				term_set (MOVE, 0, Crec);
			write_page ();
			break;
		case UNMARK:
			for (i=0; i < Page.h.artnum; ++i)
			{
				if (Page.b[i].art_mark == ART_MARK)
				{
					Page.b[i].art_mark = ' ';
					term_set (MOVE, 0, i+RECBIAS);
					putchar (' ');
				}
			}
			term_set (MOVE, 0, Crec);
			write_page ();
			break;
		case BACK:
			count *= -1;	/* fall through */
		case FORWARD:
			if (forward (count) >= 0)
				show();
			else
				preinfo ("No more pages");
			term_set (MOVE,0,Crec);
			break;
		case DIGEST:
			if (Digest)
			{
				Digest = 0;
				find_page (Cur_page);
				show();
				Crec = Drec + RECBIAS + 1;
				Highrec = Page.h.artnum + RECBIAS;
				if (Crec >= Highrec)
					Crec = Highrec - 1;
				term_set (MOVE,0,Crec);
				break;
			}
			(Page.h.group)->flags |= FLG_ACC;
			Dskip = count - 1;
			Drec = Crec - RECBIAS;
			if (digest_page(Drec,Dskip) >= 0)
			{
				show();
				Crec = RECBIAS;
				Highrec = Page.h.artnum + RECBIAS;
				term_set (MOVE,0,Crec);
				break;
			}
			Digest = 0;
			preinfo ("Can't unpack the article");
			term_set (MOVE,0,Crec);
			break;
		case NEWGROUP:
			if ((i = spec_group()) < 0)
			{
				term_set (MOVE,0,Crec);
				break;
			}
			Digest = 0;
			show();
			Crec = RECBIAS;
			Highrec = Page.h.artnum + RECBIAS;
			term_set (MOVE,0,Crec);
			break;

		case SAVE:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,Crec-RECBIAS,count);
			savestr (alist);
			term_set (MOVE,0,Crec);
			break;
		case SAVEALL:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,0,L_allow);
			savestr (alist);
			term_set (MOVE,0,Crec);
			break;
		case SAVESTRING:
		case ALTSAVE:
			(Page.h.group)->flags |= FLG_ACC;
			userlist (alist);
			savestr (alist);
			term_set (MOVE,0,Crec);
			break;
		case READ:
		case ALTREAD:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,Crec-RECBIAS,count);
			readstr (alist,count);
			break;
		case READALL:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,0,L_allow);
			readstr (alist,0);
			break;
		case READSTRING:
			(Page.h.group)->flags |= FLG_ACC;
			userlist (alist);
			readstr (alist,0);
			break;
		case PRINT:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,Crec-RECBIAS,count);
			printstr (alist);
			term_set (MOVE,0,Crec);
			break;
		case PRINTALL:
			(Page.h.group)->flags |= FLG_ACC;
			genlist (alist,0,L_allow);
			printstr (alist);
			term_set (MOVE, 0, Crec);
			break;
		case PRINTSTRING:
			(Page.h.group)->flags |= FLG_ACC;
			userlist (alist);
			printstr (alist);
			term_set (MOVE, 0, Crec);
			break;

		case HELP:
			help ();
			show ();
			term_set (MOVE, 0, Crec);
			break;
		case UNESC:
			user_str (alist,Ps1,1,"");
			term_set (ERASE);
			fflush (stdout);
			tty_set (SAVEMODE);
			if (chdir(Orgdir) < 0)
				printf ("change to original directory, %s, failed",Orgdir);
			else
			{
				system (alist);
				tty_set (RESTORE);
				term_set (RESTART);
			}
			printf (Contstr);
			getnoctl ();
			vns_gset(Page.h.name);
			show ();
			term_set (MOVE, 0, Crec);
			break;
		case PRTVERSION:
			prinfo("%s %s", Version, Vns_version);
			term_set (MOVE, 0, Crec);
			break;
		default:
			preinfo("Unhandled key: %c", c);
			break;
		}
	}

	Digest = 0;
	for (i=0; i < Ncount; ++i)
	{
		if ((Newsorder[i])->rdnum < (Newsorder[i])->pgrd)
			break;
	}
	if (i < Ncount)
	{
		user_str (alist,"Some displayed pages not updated - update ? ",
							1, QUIT_ANSWER);
		if (alist[0] == 'y')
			up_seen();
	}
	sig_set (BRK_OUT);
}

/*
** update status of Newsgroups to all seen pages
*/
up_seen()
{
	int i;

	for (i = 0; i < Ncount; ++i)
	{
		if (Nounsub && ((Newsorder[i])->flags & FLG_SUB) == 0)
		{
			new_read(Newsorder[i],(Newsorder[i])->highnum);
			continue;
		}
		if ((Newsorder[i])->rdnum < (Newsorder[i])->pgrd)
			new_read(Newsorder[i],(Newsorder[i])->pgrd);
	}
}

/*
	count_msg displays count information
*/
count_msg ()
{
	int i, gpnum, gscan, gpage;
	unsigned long mask;
	gpnum = 1;
	for (gscan = gpage = i = 0; i<Ncount; ++i)
	{
		if (((Newsorder[i])->flags & FLG_PAGE) != 0)
		{
			if (((Newsorder[i])->pnum + (Newsorder[i])->pages - 1) < Cur_page)
				++gpnum;
			++gpage;
			for (mask=1; mask != 0L; mask <<= 1)
				if (((Newsorder[i])->pgshwn & mask) != 0L)
					++gscan;
		}
	}
	prinfo (CFORMAT,Cur_page+1,Lrec+1,gscan,gpnum,gpage);
}

/*
	forward utility handles paging
*/
static
forward (count)
int count;
{
	if (!Digest)
	{
		if ((count < 0 && Cur_page <= 0) || (count > 0 && Cur_page >= Lrec))
			return (-1);
		Cur_page += count;
		if (Cur_page < 0)
			Cur_page = 0;
		if (Cur_page > Lrec)
			Cur_page = Lrec;
		find_page (Cur_page);
		Crec = RECBIAS;
		Highrec = Page.h.artnum + RECBIAS;
		return (0);
	}
	/*
	** in digests, paging past the end of the digest returns to
	** page extracted from.
	*/
	if (Dskip > 0 && (Dskip + count*L_allow) < 0)
		Dskip = 0;
	else
		Dskip += count * L_allow;
	find_page (Cur_page);
	if (Dskip >= 0)
	{
		if (digest_page(Drec,Dskip) >= 0)
		{
			Crec = RECBIAS;
			Highrec = Page.h.artnum + RECBIAS;
			return (0);
		}
	}
	Digest = 0;
	Crec = Drec + RECBIAS + 1;
	Highrec = Page.h.artnum + RECBIAS;
	if (Crec >= Highrec)
		Crec = Highrec - 1;
	return (0);
}

/*
	generate list of articles on current page,
	count articles, starting with first.
*/
static
genlist (list,first,count)
char *list;
int first,count;
{
	int i;
	for (i=first; i < Page.h.artnum && count > 0; ++i)
	{
		sprintf (list,"%d ",Page.b[i].art_id);
		list += strlen(list);
		--count;
	}
}

/*
	send list of articles to printer
*/
static
printstr (s)
char *s;
{
	char cmd [RECLEN];
	char fn[L_tmpnam];

	prinfo ("preparing print command ....");

	if (Digest)
		dig_list (s);

	if (*s != '\0')
	{
		tmpnam(fn);
		if (art_xfer(fn,s,"w") != 0)
		{
			preinfo("Couldn't open temporary file");
			return;
		}
		sprintf (cmd,"%s %s 2>/dev/null",Printer,fn);
		if (system (cmd) == 0)
			prinfo ("Sent to printer");
		else
			preinfo ("Print failed");
		unlink(fn);
	}
	else
		preinfo (No_msg);

	if (Digest)
		dig_ulist (s);
}

/*
	read a list of articles.
*/
readstr (s,count)
char *s;
int count;
{
	char *strtok();
	char *fn[MAXARTLIST+1];
	int pc, num, i;

	/* we pre-process tokens to release strtok() for further use */
	fn[0] = strtok(s,List_sep);
	for (num=0; fn[num] != NULL; fn[(++num)] = strtok(NULL,List_sep))
		if (num >= MAXARTLIST)
			break;
	fn[num] = NULL;

	if (fn[0] != NULL)
	{
		term_set (ERASE);
		for (i=0; i < num && readfile(fn[i], fn[i+1] ,&pc) >= 0; ++i)
		{
			if (Digest)
				unlink (fn[i]);
		}
		if (Digest && fn[i] != NULL)
			unlink (fn[i]);
		if (pc != 0)
			forward (pc);
		else
		{
			Crec += count;
			if (Crec >= Highrec)
				Crec = Highrec - 1;
		}
		show ();
		term_set (MOVE, 0, Crec);
	}
	else
	{
		preinfo ("%s",No_msg);
		term_set (MOVE, 0, Crec);
	}
}

/*
	concatenate articles to save file with appropriate infoline messages.
	prompt for save file, giving default.  If save file begins with "|"
	handle as a filter to pipe to.  NOTE - every user specification of
	a new Savefile "loses" some storage, but it shouldn't be a very great
	amount.
*/
static
savestr (s)
char *s;
{
	char *ptr, newfile [MAX_C+1], msg[RECLEN];
	char *str_store();

	if (Digest)
		dig_list (s);

	if (*s != '\0')
	{
		user_str (newfile,"save file ? ",1,Savefile);
		ptr = newfile;
		if (*ptr == '|')
		{
			term_set (ERASE);
			fflush (stdout);
			save_art(s,ptr,msg);
			printf ("%s\n%s",msg,Contstr);
			getnoctl ();
			show ();
		}
		else
		{
			prinfo("saving ....");
			if (*ptr == '\0')
				ptr = Savefile;
			else
				Savefile = str_store(ptr);
			if (save_art(s,Savefile,msg) != 0)
				preinfo(msg);
			else
				prinfo(msg);
		}
	}
	else
		preinfo (No_msg);

	if (Digest)
		dig_ulist (s);
}

/*
	basic page display routine.  erase screen and format current page
*/
static
show ()
{
	int i;
	unsigned long mask;
	char helpstr[40]; 

	term_set (ERASE);
	C_info = 0;
	i = Cur_page - (Page.h.group)->pnum + 1;
	if (Digest)
		printf (DHFORMAT,Page.h.name);
	else
		printf (HFORMAT,Page.h.name,i,(Page.h.group)->pages);

	mask = 1L << (i-1);
	(Page.h.group)->pgshwn |= mask;
	mask = 1;
	for (--i; i > 0 && (mask & (Page.h.group)->pgshwn) != 0 ; --i)
		mask <<= 1;
	if (i <= 0)
		(Page.h.group)->pgrd = Page.b[(Page.h.artnum)-1].art_id;

	for (i=0; i < Page.h.artnum; ++i)
	{
		if (Digest)
		{
			printf(Aformat,Page.b[i].art_mark,ART_UNWRITTEN,Page.b[i].art_id);
			printf("%s",Page.b[i].art_t);
			continue;
		}

		if ((Page.h.group)->rdnum >= Page.b[i].art_id)
			printf(Aformat,Page.b[i].art_mark,ART_WRITTEN,Page.b[i].art_id);
		else
			printf(Aformat,Page.b[i].art_mark,ART_UNWRITTEN,Page.b[i].art_id);
		printf("%s",Page.b[i].art_t);
	}

	sprintf(helpstr,HELPFORM,Cxptoi[HELP]);
	if (!Digest && ((Page.h.group)->flags & FLG_SUB) == 0)
		prinfo ("%s, %s",Unsub_msg,helpstr);
	else
		prinfo (helpstr);
}

/*
	update written status marks on screen
*/
static
wr_show ()
{
	int i,row;
	char c;

	row = RECBIAS;
	for (i=0; i < Page.h.artnum; ++i)
	{
		term_set (MOVE,WRCOL,row);
		if ((Page.h.group)->rdnum >= Page.b[i].art_id)
			c = ART_WRITTEN;
		else
			c = ART_UNWRITTEN;
		printf("%c",c);
		++row;
	}
}

/*
	obtain user input of group name, becomes current page if valid.
	returns -1 or page number.  calling routine does the show, if needed
*/
static
spec_group ()
{
	char nbuf [MAX_C + 1];
	NODE *p, *hashfind();

	user_str(nbuf,"Newsgroup ? ",1,"");

	if (*nbuf == '\0' || (p = hashfind(nbuf)) == NULL)
	{
		preinfo ("Not a newsgroup");
		return (-1);
	}
	if ((p->flags & FLG_PAGE) == 0)
	{
		if ((p->flags & FLG_SUB) == 0)
		{
			new_sub(p,FLG_SUB);
			do_update("Not subscribed: resubscribed for next reading session");
		}
		else
			prinfo ("No news for that group");
		return (-1);
	}
	if ((p->flags & FLG_SUB) == 0)
	{
		new_sub(p,FLG_SUB);
		do_update("Resubscribed");
	}
	find_page (p->pnum);
	return (p->pnum);
}

/*
	obtain user input with prompt p.  Optionally on info line.
	handle erase and kill characters, suppresses leading
	white space.  Use defstr as the editable default user input.
	If on info line, cursor is not moved anywhere whe done, otherwise
	a <CR><LF> is done after input.  Should be in raw mode to use
	this routine.  Used from outside this source file so that we
	only have to do erase / kill key stuff one place.
*/
user_str (s,p,iline,defstr)
char *s;
char *p;
int iline;
char *defstr;
{
	int i,idx,len;

	if (iline)
	{
 		prinfo ("%s%s",p,defstr);
		idx = C_info;
	}
	else
	{
 		printf ("%s%s",p,defstr);
		idx = strlen(p);
	}

	len = strlen(defstr);
 	for (i=0; i < len; i++)
 		s[i] = defstr[i];
 
	for (i=len; idx < C_allow && (s[i] = getchar() & 0x7f) != '\012' && s[i] != '\015'; ++i)
	{
		if (s[i] == Erasekey)
		{
			if (i > 0)
			{
				term_set (RUBSEQ);
				i -= 2;
				--idx;
			}
			else
				i = -1;
			continue;
		}
		if (s[i] == Killkey)
		{
 			if (iline)
 			{
 				prinfo ("%s",p);
 				idx = C_info;
 			}
 			else
 			{
 				printf ("\r%s",p);
 				term_set(ZAP,strlen(p),idx);
 				fflush(stdout);
 				idx = strlen(p);
 			}
			i = -1;
			continue;
		}
		/* no leading spaces */
		if (s[i] == ' ' && i == 0)
		{
			i = -1;
			putchar('\07');
			continue;
		}
		/* no controls */
		if (s[i] < ' ' || s[i] == '\177')
		{
			--i;
			putchar('\07');
			continue;
		}
		++idx;
		putchar (s[i]);
	}

	if (iline)
		C_info = idx;
	else
		printf("\r\n");

	s[i] = '\0';
}

/*
	print something on the information line,
	clearing any characters not overprinted.
	preinfo includes reverse video and a bell for error messages.
*/
preinfo (s,a,b,c,d,e,f)
{
	int l;
	char buf[RECLEN];

	term_set (MOVE,0,INFOLINE);
	putchar ('\07');
	term_set (ONREVERSE);
	sprintf (buf,s,a,b,c,d,e,f);
	printf (" %s ",buf);
	term_set (OFFREVERSE);
	l = strlen(buf) + 2;
	if (l < C_info)
		term_set (ZAP,l,C_info);
	C_info = l;
	fflush(stdout);
}

prinfo (s,a,b,c,d,e,f)
char *s;
long a,b,c,d,e,f;
{
	int l;
	char buf[RECLEN];
	term_set (MOVE,0,INFOLINE);
	sprintf (buf,s,a,b,c,d,e,f);
	printf ("%s",buf);
	l = strlen(buf);
	if (l < C_info)
		term_set (ZAP,l,C_info);
	C_info = l;
	fflush(stdout);
}

static
tot_list ()
{
	int i,max,len;
	char c;
	char ff[MAX_C+1];

	term_set (ERASE);

	for (max=i=0; i < Ncount; ++i)
	{
		if ((Newsorder[i])->pages == 0)
			continue;
		if ((len = strlen((Newsorder[i])->nd_name)) > max)
			max = len;
	}

	sprintf (ff,"%%4d %%%ds: %%3d new %%3d updated\n",max);

	for (len=i=0; i < Ncount; ++i)
	{
		if ((Newsorder[i])->pages == 0)
			continue;
		printf (ff, i, (Newsorder[i])->nd_name,
				(Newsorder[i])->highnum - (Newsorder[i])->orgrd,
				(Newsorder[i])->rdnum - (Newsorder[i])->orgrd);
		++len;
		if (len == L_allow && i < (Ncount-1))
		{
			printf("\nr - return, n - new group, other to continue ... ");
			if ((c = getnoctl()) == 'r' || c == 'n')
				break;
			printf ("\n\n");
			len = 0;
		}
	}
	if (i >= Ncount)
	{
		printf("n - new group, other to return ... ");
		c = getnoctl();
	}

	/* c will remain 'n' while user chooses bad newsgroups */
	while (c == 'n')
	{
		printf("\n");
		user_str(ff,"Newsgroup number ? ",0,"");
		i = atoi(ff);
 		if (i < 0 || i >= Ncount || (Newsorder[i])->pages == 0)
		{
 			printf("\nBad newsgroup number\n");
 			printf("n - new group, other to return ... ");
			c = getnoctl();
			continue;
 		}
 		find_page((Newsorder[i])->pnum);
		Crec = RECBIAS;
		Highrec = Page.h.artnum + RECBIAS;
		c = '\0';
	}
}

/*
** call vns_write if anything has changed, then wipe FLG_ECHG bits
** also produce message(s)
*/
static
do_update(msg)
char *msg;
{
	int i;

	for (i=0; i < Ncount; ++i)
		if(((Newsorder[i])->flags & FLG_ECHG) != 0)
			break;
	if (i < Ncount)
	{
		prinfo("Writing news status");
		vns_write(Newsorder,Ncount);
		for (i=0; i < Ncount; ++i)
			(Newsorder[i])->flags &= ~FLG_ECHG;
	}
	prinfo(msg);
}

/*
** set a new rdnum value.  If a change, set FLG_ECHG
*/
static
new_read(n,rd)
NODE *n;
int rd;
{
	if (n->rdnum != rd)
	{
		n->rdnum = rd;
		n->flags |= FLG_ECHG;
	}
}

/*
** set a new subscription bit.  bit argument is either 0 or FLG_SUB.
*/
static
new_sub(n,bit)
NODE *n;
unsigned bit;
{
	/*
	** since bit is 0 or FLG_SUB, we could get tricky with ^
	** but this is clearer
	*/
	if (bit != 0 && (n->flags & FLG_SUB) == 0)
		n->flags |= FLG_SUB|FLG_ECHG;
	else
	{
		if (bit == 0 && (n->flags & FLG_SUB) != 0)
		{
			n->flags &= ~FLG_SUB;
			n->flags |= FLG_ECHG;
		}
	}
}
