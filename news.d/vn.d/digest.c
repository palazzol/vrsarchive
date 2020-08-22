/*
** vn news reader.
**
** digest.c - digest unpacking routines
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include "config.h"
#include "head.h"
#include "tune.h"
#include "node.h"
#include "page.h"

extern int Digest;
extern int L_allow;
extern int C_allow;
extern PAGE Page;

extern FILE *vns_aopen();

static char *Ext_pool = NULL;
static char *Show[3];

static char *Dhead = "Date: ";
static char *Fhead = "From: ";
static char *Lhead = "Lines: ";
static char *Thead = "Subject: ";

#define THDLEN 9
#define LHDLEN 7
#define FHDLEN 6
#define DHDLEN 6

digest_page (idx,skip)
int idx;
{
	char *ptr,name[24],*title,*index();
	FILE *fp;
	int i,len,hl;
	char subj[RECLEN],date[RECLEN],from[RECLEN],junk[RECLEN],*str_store();
	long pos;
	ARTHEADER hdr;

	Digest = Page.b[idx].art_id;

	if ((fp = vns_aopen(Digest,&hdr)) == NULL)
		return (-1);

	subj[0] = date[0] = from[0] = junk[0] = '\0';

	/* skip over some articles if requested to */
	for (i=skip; i > 0; --i)
	{
		if (dig_advance(fp,from,subj,date,junk,&pos,&hl) < 0)
			return (-1);
	}

	/* every new call to a digest Page "loses" a small amount of storage */
	title = str_store(Page.b[idx].art_t);
	if ((ptr = index(title,'~')) != 0)
		*ptr = '\0';
	title [C_allow - 20] = '\0';

	for (i=0; i < L_allow &&
			(len = dig_advance(fp,from,subj,date,junk,&pos,&hl)) >= 0; ++i)
	{
		Page.b[i].art_id = i+1+skip;
		Page.b[i].art_mark = ' ';
		subj [C_allow] = '\0';
		from [C_allow] = '\0';
		sprintf (name,"%d",len);
		form_title (date,subj,name,from,100);
		strcpy (Page.b[i].art_t,date);
	}

	vns_aclose (fp);

	if (i == 0)
		return (-1);

	Page.h.name = title;
	Page.h.artnum = i;
	return (i);
}

/*
	returns name of file containing "article", NULL for failure
*/
char * digest_extract (s,art,hdr,start)
char *s;
int art;
ARTHEADER *hdr;
long *start;
{
	FILE *fout,*fin;
	long pos;
	int lines,hl;
	char subj[RECLEN],date[RECLEN],from[RECLEN],bufr[RECLEN];
	char extra[RECLEN];
	char *index();
	long ftell();
	char *str_tpool(), *str_tstore();

	if (Ext_pool != NULL)
		str_tfree(Ext_pool);
	Ext_pool = str_tpool(3);

	if ((fin = vns_aopen(Digest,hdr)) == NULL)
		return (NULL);

	for ( ; art > 0; --art)
	{
		from[0] = subj[0] = date[0] = '\0';
		if ((lines = dig_advance(fin,from,subj,date,extra,&pos,&hl)) < 0)
		{
			vns_aclose(fin);
			return (NULL);
		}
	}

	tmpnam(s);

	if ((fout = fopen(s,"w")) == NULL)
	{
		vns_aclose(fin);
		unlink (s);
		return (NULL);
	}

	fseek(fin,0L,0);

	hdr->show_num = 0;
	hdr->show = Show;
	hdr->lines = lines;
	hdr->hlines = hl;
	if (subj[0] != '\0')
	{
		sprintf (bufr,"%s%s",Thead,subj);
		Show[hdr->show_num] = str_tstore(Ext_pool,bufr);
		++(hdr->show_num);
	}
	if (from[0] != '\0')
	{
		sprintf (bufr,"%s%s",Fhead,from);
		Show[hdr->show_num] = str_tstore(Ext_pool,bufr);
		++(hdr->show_num);
	}
	if (date[0] != '\0')
	{
		sprintf (bufr,"%s%s",Dhead,date);
		Show[hdr->show_num] = str_tstore(Ext_pool,bufr);
		++(hdr->show_num);
	}

	while (fgets(bufr,RECLEN-1,fin) != NULL && bufr[0] != '\n')
	{
		if (strncmp(bufr,Fhead,FHDLEN) == 0)
		{
			fprintf (fout,"%s%s\n",Fhead,from);
			continue;
		}
		if (strncmp(bufr,Thead,THDLEN) == 0)
		{
			fprintf (fout,"%s%s\n",Thead,subj);
			continue;
		}
		if (strncmp(bufr,Dhead,DHDLEN) == 0)
		{
			fprintf (fout,"%s%s\n",Dhead,date);
			continue;
		}
		/* defer line count header - it comes last */
		if (strncmp(bufr,Lhead,LHDLEN) == 0)
			continue;
		fprintf (fout,"%s",bufr);
	}

	/* toss in extra header lines, line count header, extra newline */
	fprintf (fout,"%s%s%d\n\n",extra,Lhead,lines);
	*start = ftell(fout);

	fseek (fin,pos,0);

	while (fgets(bufr,RECLEN-1,fin) != NULL && strncmp(bufr,"--------",8) != 0)
		fprintf(fout,"%s",bufr);

	vns_aclose (fin);
	fclose (fout);
	return (s);
}

dig_list (s)
char *s;
{
	char *ptr,*out,*new,ns[L_tmpnam],tmp[RECLEN],*strtok();
	ARTHEADER hdr;
	long pos;
	int i;

	prinfo ("Extracting articles .....");
	strcpy (tmp,s);
	out = s;

	for (ptr = strtok(tmp," "); ptr != NULL; ptr = strtok(NULL," "))
	{
		i = atoi(ptr);
		if ((new = digest_extract(ns,i,&hdr,&pos)) != NULL)
		{
			sprintf (out,"%s ",new);
			out += strlen(new) + 1;
		}
	}

	*out = '\0';

	if (*s == '\0')
		strcpy (s,"NULLDIGEST");
}

dig_ulist (s)
char *s;
{
	char *strtok();
	for (s = strtok(s," "); s != NULL; s = strtok(NULL," "))
		unlink (s);
}

/*
	returns # lines in article, -1 for failure
	scans past article, returns position of start.
	also returns "extra" header lines encountered, WITH newlines.
	and counts total header lines.
*/
static dig_advance (fp,from,subj,date,extra,pos,hcount)
FILE *fp;
char *from,*subj,*date,*extra;
long *pos;
int *hcount;
{
	char buf[RECLEN];
	char *ptr, *index();
	int len,state,lcount;

	*hcount = lcount = state = 0;
	*extra = '\0';

	while (fgets(buf,RECLEN-1,fp) != NULL)
	{
		buf[(len = strlen(buf) - 1)] = '\0';
		for (--len ; len >= 0 && buf[len] == ' ' || buf[len] == '\t'; --len)
			buf[len] = '\0';
		++len;

		switch(state)
		{
		case 0:
			/* skip blank lines before header */
			if (len == 0)
				break;
			state = 1;	/* fall through */
		case 1:
			++(*hcount);
			if (strncmp(buf,Fhead,FHDLEN) == 0)
			{
				strcpy (from,buf+FHDLEN);
				break;
			}
			if (strncmp(buf,Thead,THDLEN) == 0)
			{
				strcpy (subj,buf+THDLEN);
				break;
			}
			if (strncmp(buf,Dhead,DHDLEN) == 0)
			{
				strcpy (date,buf+DHDLEN);
				break;
			}
			/* put wierd header lines in extra */
			if ((ptr = index(buf,':')) != NULL)
			{
				*ptr = '\0';
				if (index(buf, ' ') == NULL)
				{
					*ptr = ':';
					sprintf(extra,"%s\n",buf);
					extra += strlen(extra);
					break;
				}
				*ptr = ':';
			}
			state = 2;
			--(*hcount);

			/* remember the newline we lopped off */
			*pos = ftell(fp)-strlen(buf)-1;	/* fall through */
		case 2:
			++lcount;
			if (strncmp("--------",buf,8) == 0)
			{
				--lcount;
				return (lcount);
			}
			break;
		}
	}

	return (-1);
}
