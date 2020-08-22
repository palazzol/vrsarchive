/*
** vn news reader.
**
** stat.c - stat and log file collection
**
** see copyright disclaimer / history in vn.c source file
*/
#include <stdio.h>
#include <sys/types.h>
#ifdef SYSV
#include <fcntl.h>
#endif
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#include "config.h"
#include "node.h"

extern NODE *hashfind();
extern char *strtok();
extern int Ncount;
extern NODE **Newsorder;

#ifdef VNLOGFILE
static char Start[80];
#endif

stat_start()
{
#ifdef VNLOGFILE
	char *ctime();
	long now;

	time(&now);
	strcpy(Start,ctime(&now));
#endif
}

/*
** flag = 0, "NO NEWS" type session.
**      = 1, regular session.
**	= -1, aborted session
**
** CAUTION: routine CALLED from within printex() - do NOT
** call printex().  Simply do message to stderr on fail.
*/
stat_end(flag)
int flag;
{
	NODE *nd;
	char *nl,*index();
	char *how;
	struct passwd *ptr, *getpwuid();
	struct stat buf;
	long now;
	char bufr[80];
	char name[60];
	FILE *fp;
	int fd;
	long chk, rd, pg;
	int i;

#ifdef VNLOGFILE
	if (stat(VNLOGFILE,&buf) == 0 && (fp = fopen(VNLOGFILE,"a")) != NULL)
	{
		time(&now);
		strcpy(bufr,ctime(&now));
		if ((nl = index(bufr,'\n')) != NULL)
			*nl = '\0';
		if ((nl = index(Start,'\n')) != NULL)
			*nl = '\0';
		if (flag == 0)
			how = "NO NEWS";
		else
		{
			if (flag > 0)
				how = "OK";
			else
				how = "ABORTED";
		}
		ptr = getpwuid (getuid());
		fprintf(fp, "%s\t%s - %s %s\n", ptr->pw_name, Start, bufr, how);
		fclose (fp);
	}
#endif

#ifdef VNSTATFILE
	/*
	** Stat file is done with a fixed record size, and maintaining the
	** existing record order exactly so that concurrent users will do
	** the least damage.  If two users actually read & update a single
	** record simultaneously, we should just lose one user's counts.
	** Short of implementing a locking scheme, we probably won't do
	** much better.  Disadvantages are that deleted newsgroups never
	** get cleaned out, order is set by the first user whose
	** statistics are collected, it will break if anyone modifies it,
	** and the file is a bit larger than it needs to be.
	**
	** record format:
	**
	** CCCCCC PPPPPP RRRRRR newsgroup name ....  \n
	** ^      ^      ^      ^                    ^
	** 0      7      14     21              char 79
	**
	** CCCCCC - count of sessions searching group
	** PPPPPP - count of sessions actually finding pages for group
	** RRRRRR - count of sessions actually accessing articles in group
	*/
	if ((fd = open(VNSTATFILE,O_RDWR)) > 0)
	{
		bufr[80] = '\0';

		/*
		** read a record, find the newsgroup, update counts.
		** If changed, seek back & overwrite.  By using fixed
		** length records, we should only lose something on
		** concurrent writes of the same record, and by writing
		** the ENTIRE record, we keep it consistent
		*/
		while ((i = read(fd,bufr,80)) == 80 && bufr[79] == '\n')
		{
			chk = atoi(bufr);
			pg = atoi(bufr+7);
			rd = atoi(bufr+14);
			strcpy(name,bufr+21);
			nl = strtok(name," \n");
			if (nl == NULL || (nd = hashfind(nl)) == NULL)
				continue;
			nd->flags |= FLG_STAT;
			if ((nd->flags & (FLG_SEARCH|FLG_ACC|FLG_PAGE)) == 0)
				continue;
			if ((nd->flags & FLG_SEARCH) != 0)
				++chk;
			if ((nd->flags & FLG_PAGE) != 0)
				++pg;
			if ((nd->flags & FLG_ACC) != 0)
				++rd;
			if (chk > 999999L)
				chk = 999999L;
			if (pg > 999999L)
				pg = 999999L;
			if (rd > 999999L)
				rd = 999999L;
			sprintf(bufr,"%6ld",chk);
			bufr[6] = ' ';
			sprintf(bufr+7,"%6ld",pg);
			bufr[13] = ' ';
			sprintf(bufr+14,"%6ld",rd);
			bufr[20] = ' ';
			lseek(fd,-80L,1);
			write(fd,bufr,80);
		}

		/* format screwed up ? */
		if (i != 0)
		{
			lseek(fd,(long) -i,1);
			fprintf(stderr,"bad data in %s\n",VNSTATFILE);
		}

		/* may have aborted during vns_news() */
		if (Newsorder == NULL)
			Ncount = 0;

		/* now append any groups not in file yet */
		for (i = 0; i < Ncount; ++i)
		{
			nd = Newsorder[i];
			if ((nd->flags & FLG_STAT) != 0)
				continue;
			chk = rd = pg = 0;
			if ((nd->flags & FLG_SEARCH) != 0)
				chk = 1;
			if ((nd->flags & FLG_PAGE) != 0)
				pg = 1;
			if ((nd->flags & FLG_ACC) != 0)
				rd = 1;
			sprintf(bufr,"%6ld %6ld %6ld %-58s\n",
					chk, pg, rd, nd->nd_name);
			write(fd,bufr,80);
		}
		close(fd);
	}
#endif
}
