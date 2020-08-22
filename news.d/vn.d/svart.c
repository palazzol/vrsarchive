/*
** vn news reader.
**
** svart.c - article save routine
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "tty.h"
#include "tune.h"
#include "node.h"
#include "page.h"

extern PAGE Page;
extern int Digest;
extern char *List_sep;
extern char *Home;
extern char *Savedir;

/*
** save article in file.  Called from reader and session both.
** handles "|" pipe convention.  Caller passes in return message buffer.
*/
save_art(art,idest,msg)
char *art;
char *idest;
char *msg;
{
	char fn[L_tmpnam+1];
	char cmd[RECLEN];
	char *mode;
	char *dest, dstore[RECLEN];
	struct stat sbuf;
	int rstat;
	char *colon;
	char *pcnt;
	char *index();

	/* temporary copy so we don't overwrite saved string */
	strcpy((dest = dstore),idest);

	if (*dest == '|')
	{
		tmpnam(fn);
		if (art_xfer(fn,art,"w") != 0)
		{
			strcpy(msg,"Can't open temporary file");
			return (-1);
		}
		sprintf(cmd,"cat %s %s",fn,dest);
		tty_set (SAVEMODE);
		rstat = system (cmd);
		tty_set (RESTORE);
		sprintf(msg,"Command returns %d",rstat);
		return (rstat);
	}

	if ((colon = index(dest,':')) != NULL)
	{
		mode = dest;
		*colon = '\0';
		dest = colon+1;
	}
	else
		mode = "a";

	if (*dest == '~')
	{
		if (twiddle(dest,msg) < 0)
			return (-1);
	}

	if (*dest == '\0')
		strcpy(dest,"%d");

	if (*dest != '/')
	{
		if (noslash(dest,msg) < 0)
			return (-1);
	}

	if ((pcnt = index(dest,'%')) != NULL && pcnt[1] == 'd')
	{
		if (Digest)
			sprintf(cmd,dest,Digest);
		else
			sprintf(cmd,dest,atoi(art));
		dest = cmd;
	}

	rstat = stat(dest,&sbuf);

	if (art_xfer(dest,art,mode) != 0)
	{
		sprintf(msg,"Can't open %s with mode %s",dest,mode);
		return(-1);
	}

	if (rstat != 0)
	{
		sprintf(msg,"Created %s",dest);
		return(0);
	}

	if (strcmp(mode,"a") == 0)
	{
		sprintf(msg,"Appended %s",dest);
		return(0);
	}

	sprintf(msg,"Wrote (mode %s) %s",mode,dest);
	return(0);
}

static
noslash(dest,msg)
char *dest;
char *msg;
{
	char *pcnt;
	char buf[RECLEN];
	char dir[RECLEN];
	struct stat sbuf;

	strcpy(buf,Page.h.name);
#ifdef SYSV
	buf[14] = '\0';
#endif
	if ((pcnt = index(Savedir,'%')) != NULL && pcnt[1] == 's')
		sprintf(dir,Savedir,buf);
	else
		strcpy(dir,Savedir);
	if (dir[0] == '~')
	{
		if (twiddle(dir,msg) < 0)
			return (-1);
	}
	if (stat(dir,&sbuf) != 0)
	{
		if (mkdir(dir,0755) != 0)
		{
			sprintf(msg,"Cannot make directory %s",dir);
			return (-1);
		}
	}
	sprintf(buf,"%s/%s",dir,dest);
	strcpy(dest,buf);
	return (0);
}

static
twiddle(dest,msg)
char *dest, *msg;
{
	char *tail;
	char *name;
	char tmp;
	char buf[RECLEN];
	struct passwd *ptr, *getpwnam();

	for (tail=name=dest+1; *tail != '/' && *tail != '\0'; ++tail)
		;

	if (*name == '\0' || *name == '/')
		sprintf(buf,"%s%s",Home,tail);
	else
	{
		tmp = *tail;
		*tail = '\0';
		ptr = getpwnam(name);
		*tail = tmp;
		if (ptr == NULL)
		{
			sprintf(msg,"Can't interpret ~%s",name);
			return(-1);
		}
		sprintf(buf,"%s%s",ptr->pw_dir,tail);
	}

	strcpy(dest,buf);
	return (0);
}

/*
** transfer contents of a list of articles to a file.  If Digest, this
** is simply a list of files.  If not, it is a list of articles to be
** saved with vns_asave.  Parses list destructively with
** strtok().  Return 0 for success, -1 for failure to open file.
**
** Called directly to copy a list of articles to a temp. file to
** direct to printer.
*/
art_xfer(fn,list,mode)
char *fn, *list, *mode;
{
	char *p;
	FILE *fout, *fin;
	int count;
	char buf[RECLEN];
	char *strtok();

	if ((fout = fopen(fn,mode)) == NULL)
		return (-1);

	count = 0;
	for (p = strtok(list,List_sep); p != NULL; p = strtok(NULL,List_sep))
	{
		if (Digest)
		{
			fin = fopen(p,"r");
			if (fin == NULL)
				continue;
			while (fgets(buf,RECLEN-1,fin) != NULL)
				fputs(buf,fout);
			fclose(fin);
			continue;
		}
		vns_asave(atoi(p),fout,count,fn,mode);
		++count;
	}
	fclose(fout);
	return(0);
}
