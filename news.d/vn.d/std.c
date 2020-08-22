#include <stdio.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/param.h>
#include "server.h"
#include "config_std.h"
#include "std.h"
#ifndef MAXPATHLEN
#define MAXPATHLEN BUFSIZ
#endif

extern NODE *hashfind();
extern FILE *fopen();
extern char *index(), *rindex();
extern char *malloc();
extern char *str_tstore(), *str_tpool(), *str_store();
extern char *strtok(), *strpbrk();
extern char *regex(), *regcmp();

#ifdef MAILCHOOSE
extern int (*Massage)();
#endif

/*
	global flags signifying options set
*/
#define GF_ALL 1	/* -x option - scan everything */
#define GF_SPEC 2	/* -n option(s) - user specified groups */
#define GF_OVER 4	/* command line specification - overide marks */

char *Vns_version = "res1.0";

static char *Onews, *Newsrc;
static int Ntopt, Nntopt, Nwopt, Nnwopt;

static char *Wopt[NUMFILTER];		/* regular expressions for -w options */
static char *Topt[NUMFILTER];		/* for -t options */
static char *Negwopt[NUMFILTER];	/* for negated -w options */
static char *Negtopt[NUMFILTER];	/* for negated -t options */

static char *Options[OPTLINES];
static int Max_name, Optlines;
static unsigned Gflags = 0;
static char **Active;
static int Actnum;
static char *Mailer, *Poster;

static char *RT_head = RTHEAD;
static char *P_head = PHEAD;
static char *M_head = MHEAD;
static char *R_head = RHEAD;
static char *TO_head = TOHEAD;
static char *F_head = FHEAD;
static char *FT_head = FTHEAD;
static char *T_head = THEAD;
static char *DIS_head = DISHEAD;
static char *L_head = LHEAD;
static char *N_head = NHEAD;

static char *Fpfix = FPFIX;

/*
**	environment setup.
*/
vns_envir()
{
 	char dbuf[MAXPATHLEN], *rcname;
	char *vn_env();
	struct passwd *ptr, *getpwuid();
#ifdef MAILCHOOSE
	int mail_prompt();

	Massage = mail_prompt;
#endif

	ptr = getpwuid (getuid());

	rcname = vn_env("MAILER",DEF_MAIL);
#ifdef INLETTER
	sprintf(dbuf,"cat %%s | %s",rcname);
#else
	/* used as a format string TWICE (%%%% -> %% -> %) */
	sprintf(dbuf,"cat %%%%s | %s %%s",rcname);
#endif
	Mailer = str_store(dbuf);
	rcname = vn_env("VNPOSTER",DEF_POST);
	sprintf(dbuf,"%s %%s",rcname);
	Poster = str_store(dbuf);
	rcname = vn_env("NEWSRC",DEF_NEWSRC);
	if (*rcname != '/')
	{
		sprintf (dbuf, "%s/%s",ptr->pw_dir,rcname);
		Newsrc = str_store (dbuf);
	}
	else
		Newsrc = str_store (rcname);
	sprintf (dbuf, "%s/%s%s",ptr->pw_dir,".vn","XXXXXX");
	mktemp(dbuf);
	Onews = str_store (dbuf);
	if (access (Newsrc,0) != 0)
		creat (Newsrc,0666);
}

/*
	change directory to group
*/
vns_gset(grp)
char *grp;
{
	char dbuf [RECLEN];
	g_dir (grp,dbuf);
	if (chdir(dbuf) < 0)
		printex("can't change to newsgroup directory");
}

/*
	g_dir converts newsgroup name to directory string
*/
static
g_dir(s,t)
char *s,*t;
{
	char *ptr;
	sprintf (t,"%s/%s",SPOOLDIR,s);
	for (ptr=t+strlen(SPOOLDIR)+1; (ptr = index(ptr,'.')) != NULL; *ptr = '/')
		;
}

/*
** myfind is used for hashfind() calls which aren't supposed to fail.
*/
static NODE *
myfind(name)
char *name;
{
	NODE *n;

	n = hashfind(name);
	if (n == NULL)
		printex("Unexpected table lookup failure");
	return (n);
}

vns_news(argc,argv,lfirst,nun)
int argc;
char **argv;
int *lfirst, *nun;
{
	FILE *fp;
	static char marks[] =
	{ 
		NEWS_ON, NEWS_OFF, '\0' 
	};
	int line, len, num;
	char buf [RECLEN], trail, optpflag, submark, *fret, *ptr;

	++argv;
	--argc;

	/* fill table with active newsgroups */
	fill_active ();

	if (argc > 0)
	{
		Gflags |= GF_OVER;
		arg_opt(argc,argv,lfirst,nun);
		optpflag = 'y';
	}
	else
		optpflag = 'n';

	if ((fp = fopen (Newsrc,"r")) == NULL)
		printex ("can't open %s for reading",Newsrc);

	Optlines = 0;

	for (line = 1; (fret = fgets(buf,RECLEN-1,fp)) != NULL && emptyline(buf) == 1; ++line)
		;
	if (fret != NULL && strncmp (buf,"options",7) == 0)
	{
		Options[0] = str_store(buf);
		Optlines = 1;
		trail = buf [strlen(buf)-2];
		for ( ; (fret = fgets(buf,RECLEN-1,fp)) != NULL; ++line)
		{
			if (trail != '\\' && buf[0] != ' ' && buf[0] != '\t')
				break;
			if (Optlines >= OPTLINES)
				printex ("%s - too many option lines (%d allowed)",Newsrc,OPTLINES);
			Options[Optlines] = str_store(buf);
			++Optlines;
			if ((len = strlen(buf)) >= 2 && buf[len-2] != '\\')
				trail = buf[len-2];
			else
				trail = '\0';
		}
	}

	/* do the options from the newsrc file if there weren't command line args */
	if (Optlines > 0 && optpflag == 'n')
		newsrc_opt (lfirst,nun);

	for ( ; fret != NULL; ++line, fret = fgets(buf,RECLEN-1,fp))
	{
		if (emptyline(buf) == 1)
			continue;
		if ((ptr = strpbrk(buf,marks)) == NULL)
		{
			fprintf (stderr,"\nwarning: line %d of %s (%s) - bad syntax\n",
			line,Newsrc,buf);
			continue;
		}
		submark = *ptr;
		*ptr = '\0';
		++ptr;
		num = 0;
		for (ptr = strtok(ptr," ,-\n"); ptr != NULL; ptr = strtok(NULL," ,-\n"))
		{
			len = atoi (ptr);
			for ( ; *ptr >= '0' && *ptr <= '9'; ++ptr)
				;
			if (*ptr != '\0' || len < num)
			{
				num = -1;
				fprintf (stderr,"\nwarning: line %d of %s (%s) - bad syntax\n",
				line,Newsrc,buf);
				break;
			}
			num = len;
		}
		if (num < 0)
			continue;
		chkgroup (buf,submark,num,0);
	}
	fclose (fp);

	/* now take care of groups not specified in .newsrc */
	art_active();

	/* free up the option string storage */
	for (num=0; num < Ntopt; ++num)
		regfree (Topt[num]);
	for (num=0; num < Nwopt; ++num)
		regfree (Wopt[num]);
	for (num=0; num < Nntopt; ++num)
		regfree (Negtopt[num]);
	for (num=0; num < Nnwopt; ++num)
		regfree (Negwopt[num]);
	Ntopt = Nwopt = Nntopt = Nnwopt = 0;

	/* free the active list */
	free ((char *) Active);
}

static
emptyline(s)
char *s;
{
	while (isspace(*s))
		++s;
	if (*s == '\0')
		return (1);
	return (0);
}

/*
	fill hash table from active news group list
	This is needed to be able to process options
	before scanning user order.  Constructs an array
	of active newsgroup names for the rest of vns_nws().
*/
static
fill_active ()
{
	FILE *f;
	char *nread, act_rec[RECLEN];
	int num,lownum,rcount;

	Max_name = 0;
	if ((f = fopen (ACTFILE,"r")) == NULL)
		printex ("couldn't open %s\n",ACTFILE);

	/*
	** we do things this way so that we only examine active records
	** once, minimizing the window where changes could screw us up
	** at the cost of possibly alloc'ing a few extra bytes.  We start
	** with a count of one to have a positive rcount for alloc.
	*/
	for(rcount=1; fgets(act_rec, RECLEN-1, f) != NULL; ++rcount)
		;
	if ((Active = (char **) malloc(rcount*sizeof(char *))) == NULL)
		printex("Memory allocation failure");

	rewind(f);

	Actnum = 0;
	while (Actnum < rcount && fgets(act_rec, RECLEN-1, f) != NULL)
	{
		if (strtok (act_rec," \n") == NULL)
			continue;
		nread = strtok (NULL, " \n");
		if (nread != NULL)
			num = atoi(nread);
		else
			num = 0;
		nread = strtok (NULL, " \n");
		if (nread != NULL)
			lownum = atoi(nread);
		else
			lownum = 0;
		if (lownum > 0)
			--lownum;
		if (strlen(act_rec) > Max_name)
			Max_name = strlen(act_rec);

		/* enter newsgroup, point to permanent copy of name */
		hashenter (act_rec, num, lownum);
		Active[Actnum] = (myfind(act_rec))->nd_name;
		++Actnum;
	}

	fclose (f);
}

/*
	check active newsgroups not mentioned in NEWSRC file
	(SFLG_SCAN not set)
*/
static
art_active ()
{
	int i;
	NODE *ptr;

	for( i=0; i < Actnum; ++i)
	{
		ptr = myfind(Active[i]);
		if ((ptr->state & SFLG_SCAN) == 0)
			chkgroup (ptr->nd_name, NEWS_ON, 0, 1);
	}
}

/*
	check group for new articles:
	s - group
	c - subscription indicator from NEWSRC
	n - number read
	new - new newsgroup flag
*/
static
chkgroup (s,c,n,new)
char *s,c;
int n;
int new;
{
	NODE *ptr;
	char sub;
	int nrd;
	int lowart;
	int st;

	if ((ptr = hashfind(s)) != NULL && (ptr->state & SFLG_SCAN) == 0)
	{
		ptr->state |= SFLG_SCAN;

#ifdef SYN_CHECK
		/* if "read" more than exist, reset */
		if (n > ptr->highnum)
		{
			n = ptr->highnum - SYN_SETBACK;
			fgprintf("%s: .newsrc out of synch, resetting\n",s);
		}
#endif
		lowart = ptr->lownum;
		if (n < ptr->lownum)
			n = ptr->lownum;

		nrd = n;
		sub = c;

		/*
		** scan decision is rather complex, since GF_ALL setting
		** overides "n" value, GF_SPEC indicates SFLG_SPEC flag used.
		** if GF_OVER set, SFLG_SPEC overides subscription mark, else
		** SFLG_SPEC AND subscribed is neccesary.
		*/
		if ((Gflags & GF_SPEC) != 0)
		{
			if ((ptr->state & SFLG_SPEC) == 0)
				c = NEWS_OFF;
			else
			{
				if ((Gflags & GF_OVER) != 0)
					c = NEWS_ON;
			}
		}
		if ((Gflags & GF_ALL) != 0)
			n = lowart;
		fw_group(s, new, sub == NEWS_ON, nrd, c == NEWS_ON);
		if (c == NEWS_ON && ptr->highnum > n)
		{
			st = outgroup (s,n,ptr->highnum);
			if (st > nrd)
				fw_chg(new, sub == NEWS_ON, st, c == NEWS_ON);
		}
	}
}

/*
	vns_write writes the .newsrc file
*/
vns_write(news,ncount)
NODE **news;
int ncount;
{
	FILE *fp;
	NODE *p;
	char c;
	int i,rc;

	if (link(Newsrc,Onews) < 0)
		printex ("can't backup %s to %s before writing",Newsrc,Onews);

	if (unlink(Newsrc) < 0 || (fp = fopen(Newsrc,"w")) == NULL)
		printex ("can't open %s for writing (backed up in %s)",Newsrc,Onews);
	else
	{
		clearerr(fp);
		for (i=0; (rc = ferror(fp)) == 0 && i < Optlines; ++i)
			fprintf (fp,"%s",Options[i]);
		for (i=0; rc == 0 && i < ncount; ++i)
		{
			p = news[i];
			if ((p->flags & FLG_SUB) == 0)
				c = NEWS_OFF;
			else
				c = NEWS_ON;
#ifdef OLDRC
			fprintf (fp,"%s%c %d\n",p->nd_name,c,p->rdnum);
#else
			if (p->rdnum > 0)
				fprintf(fp,"%s%c 1-%d\n",p->nd_name,c,p->rdnum);
			else
				fprintf(fp,"%s%c 0\n",p->nd_name,c);
#endif
			rc = ferror(fp);
		}
		fclose (fp);
		if (rc != 0)
			printex ("write of %s failed, old copy stored in %s",Newsrc,Onews);
		else
			unlink (Onews);
	}
}

/*
	arg_opt must be called prior to option scanning, since
	it uses the options array.  This is a bit of a kludge,
	but it saves a bunch of work.  NOTE - no command name argument
*/
static
arg_opt (argc,argv,lfirst,nun)
int argc;
char **argv;
int *lfirst, *nun;
{
	if (argc > OPTLINES)
		printex ("too many command line options (%d allowed)\n",OPTLINES);
	for (Optlines=0; Optlines < argc; ++Optlines)
	{
		Options[Optlines] = *argv;
		++argv;
	}
	newsrc_opt(lfirst,nun);
}

/*
	option setting routine:
	sets global flags: GF_ALL for -x option GF_SPEC for -n.
	sets up filter array for article scanning
*/
static
newsrc_opt(lfirst,nun)
int *lfirst, *nun;
{
	int i;
	char curopt,tmp[RECLEN],*tok;

	*nun = *lfirst = 0;
	Ntopt = Nwopt = Nnwopt = Nntopt = 0;
	curopt = '\0';
	for (i=0; i < Optlines; ++i)
	{
		strcpy(tmp,Options[i]);
		for (tok = strtok(tmp,",\\ \t\n"); tok != NULL; tok = strtok(NULL,",\\ \t\n"))
		{
			if (*tok != '-')
				do_opt (curopt,tok);
			else
			{
				for (++tok; index("nwt",*tok) == NULL; ++tok)
				{
					/* options with no strings */
					switch(*tok)
					{
					case 'S':
						Gflags &= ~GF_OVER;
						break;
					case '%':
						*lfirst = 1;
						break;
					case 'U':
						*nun = 1;
						break;
#ifdef OLDRC
					case 'i':
					/* Treat "-i" as synonym for "-x" */
#endif
					case 'x':
						Gflags |= GF_ALL;
					default:
						break;
					}
				}
				curopt = *tok;
				if (*(++tok) != '\0')
					do_opt (curopt,tok);
			}
		}
	}
}

/* do_opt is for options with strings attached */
static
do_opt (opt,str)
char opt, *str;
{
	switch (opt)
	{
	case 'n':
		Gflags |= GF_SPEC;
		specmark(str);
		break;
	case 'w':
		specfilter (FIL_AUTHOR,str);
		break;
	case 't':
		specfilter (FIL_TITLE,str);
		break;
	default:
#ifdef OLDRC
		Gflags |= GF_SPEC;	/* Assume anything else is newsgroup */
		specmark(str);
#endif
		break;
	}
}

static
specfilter (comp,str)
char comp,*str;
{
	int *count;
	char **rex;

	/*
	** we may set rex one past end of array.  we will error before
	** referencing it if that's the case, however.
	*/
	if (*str == '!')
	{
		if (comp == FIL_TITLE)
		{
			count = &Nntopt;
			rex = Negtopt + *count;
		}
		else
		{
			count = &Nnwopt;
			rex = Negwopt + *count;
		}
		++str;
	}
	else
	{
		if (comp == FIL_TITLE)
		{
			count = &Ntopt;
			rex = Topt + *count;
		}
		else
		{
			count = &Nwopt;
			rex = Wopt + *count;
		}
	}
	if (*count >= NUMFILTER)
		printex ("too many %c options, %d allowed",comp,NUMFILTER);
	if ((*rex = regcmp(str,(char *) 0)) == NULL)
		printex ("%c option regular expression syntax: %s",comp,str);
	++(*count);
}

/*
	handle the newsgroup specification string.
	("all" convention - braack!!!)
*/
static
specmark (s)
char *s;
{
	unsigned ormask,andmask;
	int i,len;
	char *ptr,*re,pattern[RECLEN];
	NODE *nptr;

	if (*s == '!')
	{
		++s;
		ormask = 0;
		andmask = ~SFLG_SPEC;
		if (*s == '\0')
			return;
	}
	else
	{
		ormask = SFLG_SPEC;
		andmask = 0xffff;
	}

	/* convert "all" not bounded by alphanumerics to ".*". ".all" becomes ".*" */
	for (ptr = s; (len = findall(ptr)) >= 0; ptr += len+1)
	{
		if (len > 0 && isalnum (s[len-1]))
			continue;
		if (isalnum (s[len+3]))
			continue;
		if (len > 0 && s[len-1] == '.')
		{
			--len;
			strcpy (s+len,s+len+1);
		}
		s[len] = '.';
		s[len+1] = '*';
		strcpy (s+len+2,s+len+3);
	}

	/* now use regular expressions */
	sprintf (pattern,"^%s$",s);
	if ((re = regcmp(pattern,(char *) 0)) == NULL)
		printex ("n option regular expression syntax: %s",s);
	for (i=0; i < Actnum; ++i)
	{
		nptr = myfind(Active[i]);
		if (regex(re,nptr->nd_name) != NULL)
		{
			nptr->state |= ormask;
			nptr->state &= andmask;
		}
	}
	regfree (re);
}

static
findall (s)
char *s;
{
	int len;
	for (len=0; *s != '\0'; ++s,++len)
	{
		if (*s == 'a' && strncmp(s,"all",3) == 0)
			return (len);
	}
	return (-1);
}

static
grp_indic (s,ok)
char *s;
int ok;
{
	if (ok)
		fgprintf("    %s\n",s);
	else
		fgprintf("    %s - Can't access spool directory\n",s);
}

/*
	enter newsgroup articles.
	all articles between low and hi are to be included.

	Returns the highest number less than an OPENED (not neccesarily
	accepted) article to allow caller to revise "articles read"
	number beyond non-existent articles.
*/
outgroup (s,low,hi)
char *s;
int low,hi;
{
	int i;
	char subj[RECLEN], lines[RECLEN], auth[RECLEN], gd[RECLEN];
	int ret,op;

	if ((hi-low) > MAXARTRANGE)
		low = hi - MAXARTRANGE;

	ret = low;
	op = 1;

	g_dir(s,gd);
	if (chdir(gd) < 0)
	{
		grp_indic(s,0);
		return (ret);
	}
	grp_indic(s,1);
	for (i=low+1; i <= hi; ++i)
	{
		if (digname(i,subj,lines,auth,&op) >= 0)
		{
			fw_art(i,subj,lines,auth);
		}
		else
		{
			if (op)
				ret = i;
		}
	}

	return(ret);
}

/*
** open article and interpret options, if any.  The op parameter is set
** to ZERO if and only if an article is opened.  Used above as a flag to
** indicate no articles opened yet.
*/
static digname (n, subj, lines, auth, op)
int n;
char *subj, *lines, *auth;
int *op;
{
	int i,j;
	FILE *fp;
	char t[RECLEN];
	char *nfgets();

	/* open article */
	sprintf (t,"%d", n);
	if ((fp = fopen(t,"r")) == NULL)
		return (-1);
	*op = 0;

	/* get subject, from and lines by reading article */
	subj[0] = lines[0] = auth[0] = '?';
	subj[1] = lines[1] = auth[1] = '\0';
	for (i = 0; i < HDR_LINES && nfgets(t,RECLEN-1,fp) != NULL; ++i)
	{
		if (index(CHFIRST,t[0]) == NULL)
			continue;
		t[strlen(t) - 1] = '\0';
		if (strncmp(T_head,t,THDLEN) == 0)
		{
			for (j=0; j < Nntopt; ++j)
			{
				if (regex(Negtopt[j],t+THDLEN) != NULL)
				{
					fclose(fp);
					return(-1);
				}
			}
			if (Ntopt > 0)
			{
				for (j=0; j < Ntopt; ++j)
				{
					if (regex(Topt[j],t+THDLEN) != NULL)
						break;
				}
				if (j >= Ntopt)
				{
					fclose(fp);
					return(-1);
				}
			}
			strcpy(subj,t+THDLEN);
			continue;
		}
		if (strncmp(F_head,t,FHDLEN) == 0)
		{
			for (j=0; j < Nnwopt; ++j)
			{
				if (regex(Negwopt[j],t+FHDLEN) != NULL)
				{
					fclose(fp);
					return(-1);
				}
			}
			if (Nwopt > 0)
			{
				for (j=0; j < Nwopt; ++j)
				{
					if (regex(Wopt[j],t+FHDLEN) != NULL)
						break;
				}
				if (j >= Nwopt)
				{
					fclose(fp);
					return(-1);
				}
			}
			strcpy(auth,t+FHDLEN);
			continue;
		}
		if (strncmp(L_head,t,LHDLEN) == 0)
		{
			strcpy(lines,t+LHDLEN);
			break;
		}
	}

	fclose (fp);

	/* reject empty or 1 line files */
	if (i < 2)
		return (-1);

	return (0);
}

/*
** special fgets for reading header lines, which unfolds continued lines
** and throws away trailing stuff on buffer overflow.
*/
static char *
nfgets(buf, size, fp)
char	*buf;
int	size;
FILE	*fp;
{
	register int c;

	while (!feof(fp))
	{
		if ((c = getc(fp)) == '\n')
		{
			if ((c = getc(fp)) == '\t' || c == ' ')
				continue;
			ungetc(c, fp);
			*buf = '\n';
			++buf;
			*buf = '\0';
			++buf;
			return (buf);
		}

		/* prevent "terminal bombs" */
		if (c < ' ' || c == '\177')
		{
			switch(c)
			{
			case '\r':
			case '\010':
			case '\07':
				break;
			case '\177':
				c = '~';
				break;
			case '\t':
				c = ' ';
				break;
			default:
				if (size > 1)
				{
					*buf = '^';
					++buf;
					--size;
				}
				c += 'A' - 1;
				break;
			}
		}

		if (size > 0)
		{
			*buf = c;
			++buf;
			--size;
		}
		if (c == '\r')
		{
			if ((c = getc(fp)) != '\n')
			{
				ungetc(c, fp);
				continue;
			}
			if ((c = getc(fp)) != ' ' && c != '\t')
			{
				*buf = '\0';
				++buf;
				ungetc(c, fp);
				return (buf);
			}
			--buf;
			++size;
			continue;
		}
	}

	*buf = '\0';
	++buf;
	return (NULL);
}

static char *Mail[2], *Show[6], *Post[4];
static char *Priv[8];
static char *Pool;

FILE *
vns_aopen(art,hdr)
int art;
ARTHEADER *hdr;
{
	char buf[RECLEN];
	char *dist, *reply, *from, *ngrp, *flto, *path, *resubj;
	FILE *fp;
	int n;
	char *mail_trim();

	dist = resubj = path = reply = from = ngrp = flto = NULL;

	sprintf(buf,"%d",art);
	if ((fp = fopen(buf,"r")) == NULL)
		return(NULL);

	/*
	** we only really need a lot extra if MAILCHOOSE, but allocating
	** a temporary array of pointers isn't that much.  Similarly, a
	** few assignments, and the "Priv" declaration are only needed
	** with some define settings.  Not worth ifdef'ing.
	*/
	Pool = str_tpool(100);

	hdr->artid = "<some article>";
	hdr->from = "<somebody>";
	hdr->priv = Priv;
	hdr->postcmd = Poster;
	hdr->mail = Mail;
	hdr->show = Show;
	hdr->post = Post;
	hdr->priv_num = hdr->show_num = hdr->post_num = hdr->mail_num = 0;

	/* for conditional is abnormal - expected exit is break */
	for (n=0; n < HDR_LINES && fgets(buf,RECLEN-1,fp) != NULL; ++n)
	{
		/* bail out at first non-header line */
		if (buf[0] == '\n')
			break;
		if (strncmp(buf,RT_head,RTHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			reply = str_tstore(Pool,buf+RTHDLEN);
			continue;
		}
		if (strncmp(buf,P_head,PHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			path = str_tstore(Pool,buf+PHDLEN);
			continue;
		}
		if (strncmp(buf,DIS_head,DISHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			dist = str_tstore(Pool,buf);
			continue;
		}
		if (strncmp(buf,M_head,MHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			hdr->artid = str_tstore(Pool,buf+MHDLEN);
			continue;
		}
		if (strncmp(buf,F_head,FHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			(hdr->show)[hdr->show_num] = str_tstore(Pool,buf);
			from = hdr->from = (hdr->show)[hdr->show_num]+FHDLEN;
			++(hdr->show_num);
			continue;
		}
		if (strncmp(buf,T_head,THDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			(hdr->show)[hdr->show_num] = str_tstore(Pool,buf);
			if (strncmp(buf+THDLEN,Fpfix,FPFLEN) != 0)
			{
				sprintf(buf,"%s%s%s",T_head,Fpfix,
					((hdr->show)[hdr->show_num])+THDLEN);
				resubj = str_tstore(Pool,buf);
			}
			else
				resubj = (hdr->show)[hdr->show_num];
			++(hdr->show_num);
			continue;
		}
		if (strncmp(buf,N_head,NHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';

			/* if multiple newsgroups, include in "show" */
			if (index(buf,',') != NULL)
			{
				(hdr->show)[hdr->show_num] = str_tstore(Pool,buf);
				ngrp = (hdr->show)[hdr->show_num] + NHDLEN;
				++(hdr->show_num);
			}
			else
				ngrp = str_tstore(Pool,buf+NHDLEN);
			continue;
		}
		if (strncmp(buf,FT_head,FTHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			(hdr->show)[hdr->show_num] = str_tstore(Pool,buf);
			flto = (hdr->show)[hdr->show_num] + FTHDLEN;
			++(hdr->show_num);
			continue;
		}
		if (strncmp(buf,L_head,LHDLEN) == 0)
		{
			buf [strlen(buf)-1] = '\0';
			hdr->lines = atoi(buf+LHDLEN);
			(hdr->show)[hdr->show_num] = str_tstore(Pool,buf);
			++(hdr->show_num);
			continue;
		}
	}

	hdr->hlines = n;

#ifdef MAILCHOOSE
	(hdr->priv)[hdr->priv_num] = resubj;
	++(hdr->priv_num);
	if (reply != NULL)
	{
		(hdr->priv)[hdr->priv_num] = mail_trim(reply);
		++(hdr->priv_num);
	}
	if (from != NULL)
	{
		(hdr->priv)[hdr->priv_num] = mail_trim(from);
		++(hdr->priv_num);
	}
	if (path != NULL)
	{
		(hdr->priv)[hdr->priv_num] = mail_trim(path);
		++(hdr->priv_num);
	}
#else
#ifdef MAILSMART
	if (reply == NULL)
		if (from != NULL)
			reply = from;
		else
		{
			if (path != NULL)
				reply = path;
		}
#else
	if (path != NULL)
		reply = path;
#endif
	reply =  mail_trim(reply);
	if (reply != NULL)
		mail_cmd(hdr,reply,resubj);
#endif /* MAILCHOOSE */

	if (flto == NULL)
	{
		if ((flto = ngrp) == NULL)
			flto = "group.unknown";
	}
	ngrp = rindex(flto,'.');

	if (strncmp("mod.",flto,4) == 0 ||
			(ngrp != NULL && strcmp(".announce",ngrp) == 0))
	{
		sprintf(buf,"Cannot post a follow-up to \"%s\", reply with mail to moderator",flto);
		hdr->post_err = str_tstore(Pool,buf);
		return (fp);
	}

	if (ngrp != NULL && strcmp(ngrp,".general") == 0)
	{
		*ngrp = '\0';
		sprintf(buf,"%s%s.followup",N_head,flto);
	}
	else
		sprintf(buf,"%s%s",N_head,flto);
	flto = str_tstore(Pool,buf);

	hdr->post_err = NULL;

	if (resubj != NULL)
	{
		(hdr->post)[hdr->post_num] = resubj;
		++(hdr->post_num);
	}

	(hdr->post)[hdr->post_num] = flto;
	++(hdr->post_num);

	sprintf(buf,"%s%s",R_head,hdr->artid);
	(hdr->post)[hdr->post_num] = str_tstore(Pool,buf);
	++(hdr->post_num);

	if (dist != NULL)
	{
		(hdr->post)[hdr->post_num] = dist;
		++(hdr->post_num);
	}

	return (fp);
}

#ifdef MAILCHOOSE
/*
** routine to prompt user for mail path approval
*/
static
mail_prompt(hdr)
ARTHEADER *hdr;
{
	int i;
	char buf[RECLEN],*ptr;

	tty_set(SAVEMODE);
	for (i=1; i < hdr->priv_num; ++i)
	{
		printf("%d - %s\n",i,(hdr->priv)[i]);
	}
	printf("\nType number to choose one of the above, or input address: ");
	fgets(buf,RECLEN-1,stdin);
	tty_set(RESTORE);

	ptr = strtok(buf," \t\n");
	if (ptr == NULL)
		ptr = "";

	i = strlen(ptr);
	if (i == 1)
	{
		i = atoi(ptr);
		if (i > 0 && i <= hdr->priv_num)
			ptr = (hdr->priv)[i];
		i = 1;
	}

	/*
	** If the user keeps cycling through here on the same article,
	** we will eventually run out of strings.  We made Pool large
	** enough to make it unlikely (user will have to retry about 80
	** times without switching articles).  Hardly elegant, but should
	** be sufficient.
	*/
	if (i > 1 && hdr->priv_num < 8)
	{
		(hdr->priv)[hdr->priv_num] = str_tstore(Pool,ptr);
		++(hdr->priv_num);
	}
	mail_cmd(hdr,ptr,(hdr->priv)[0]);
}
#endif

/*
** trim () off potential mail address, and make copy if needed.
** addr must be allocated string.
*/
static char *
mail_trim(addr)
char *addr;
{
	char buf[RECLEN];
	char *ptr;

	if (index(addr,'(') == NULL)
		return(addr);

	strcpy(buf,addr);
	ptr = index(buf,'(');
	for (--ptr; *ptr == ' ' || *ptr == '\t'; --ptr)
		;
	++ptr;
	*ptr = '\0';
	return (str_tstore(Pool,buf));
}

/*
** format mail command.  Subj must point to allocated string.
*/
static
mail_cmd(hdr,addr,subj)
ARTHEADER *hdr;
char *addr, *subj;
{
	char buf[RECLEN];

	if (addr == NULL || *addr == '\0')
	{
		hdr->mail_err = "No address";
		return;
	}

	hdr->mail_err = NULL;
			;

#ifdef INLETTER
	hdr->mailcmd = Mailer;
	sprintf(buf,"%s%s",TO_head,addr);
	(hdr->mail)[0] = str_tstore(Pool,buf);
	hdr->mail_num = 1;
#else
	sprintf(buf,Mailer,addr);
	hdr->mailcmd = str_tstore(Pool,buf);
	hdr->mail_num = 0;
#endif
	if (subj != NULL)
	{
		(hdr->mail)[hdr->mail_num] = subj;
		++(hdr->mail_num);
	}
}

vns_aclose(fp)
FILE *fp;
{
	str_tfree(Pool);
	fclose(fp);
}

/*
** we don't use the count / name / mode arguments because this doesn't
** implement any fancy article massaging
*/
vns_asave(art,fp)
int art;
FILE *fp;
{
	char buf[RECLEN];
	FILE *fin;

	sprintf(buf,"%d",art);
	if ((fin = fopen(buf,"r")) == NULL)
		return;

	while (fgets(buf,RECLEN-1,fin) != NULL)
		fputs(buf,fp);
	fclose(fin);
}

vns_exit()
{
}
