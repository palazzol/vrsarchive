#include "cent.h"

struct score {
    char name[10];
    long score;
    int board;
    };

err()
{
    perror("bad score file");
    exit(-1);
}

doscores()
{
    register int fd,wr,numscores,i,putin = 0,rank = 0;
    struct stat buf;
    struct score *scores, myscore;
    FILE *morefp;

    if (stat(scorefile,&buf) == -1)
	err();
    lockscore();
    if (buf.st_size % sizeof(struct score) != 0)
    {
	printf("Bad score file format\n");
	exit(-1);
    }
    numscores = buf.st_size / sizeof(struct score);
    scores = (struct score *)malloc(buf.st_size);
    strcpy(myscore.name,name);
    myscore.score = score;
    myscore.board = board;
    if ((fd = open(scorefile,0)) == -1)
	err();
    if (read(fd,scores,buf.st_size) == -1)
	err();
    close(fd);
    if (wr = needtowrite(scores,numscores,&myscore))
	if ((fd = creat(scorefile,0600)) == -1)
	    err();
    if ((morefp = popen("/usr/ucb/more","w")) == NULL)
    {
	perror("Error popen'ing more");
	exit(1);
    }
    printf("Centipede Hall of Fame\n");
    signal(SIGPIPE,SIG_IGN);
    fprintf(morefp,"Rank Score      Board Name\n");
    for (i = 0; i < numscores; i++)
    {
	if (!putin && score > scores[i].score)
	{
	    putin = 1;
	    prscore(&myscore,++rank,wr,fd,morefp);
	}
	if (!putin || strcmp(scores[i].name,myscore.name))
	{
	    prscore(&scores[i],++rank,wr,fd,morefp);
	    if (!strcmp(scores[i].name,myscore.name))
		putin = 1;
	}
    }
    if (!putin)
	prscore(&myscore,++rank,wr,fd,morefp);
    close(fd);
    pclose(morefp);
    signal(SIGPIPE,SIG_DFL);
    unlockscore();
}

prscore(sc,rank,wr,fd,morefp)
struct score *sc;
int rank,wr,fd;
FILE *morefp;
{
    if (wr)
	write(fd,sc,sizeof(struct score));
    fprintf(morefp,"%-4d %-10s %-5d %-s\n",rank,addcommas(sc->score),sc->board,
      sc->name);
}

needtowrite(sc,num,myscore)
struct score *sc, *myscore;
int num;
{
    for (; num--; sc++)
	if (!strcmp(sc->name,myscore->name))
	    return(score > sc->score);
    return(1);
}

showscores()
{
    struct score sc;
    register int fd,rank = 0;
    FILE *morefp;

    lockscore();
    if ((fd = open(scorefile,0)) == -1)
	err();
    if ((morefp = popen("/usr/ucb/more","w")) == NULL)
    {
	perror("Error popen'ing more");
	exit(1);
    }
    printf("Centipede Hall of Fame\n");
    fprintf(morefp,"Rank Score      Board Name\n");
    while (read(fd,&sc,sizeof(struct score)))
	prscore(&sc,++rank,0,0,morefp);
    close(fd);
    pclose(morefp);
    unlockscore();
    exit(0);
}

/* lockscore and unlockscore: routines for controlling access to score file */
int lockfd;

lockscore()
{
    struct stat buf;

    while (stat(lockfile,&buf) != -1)
    {
	char ch;

	printf("Score file busy. Wait? ");
	scanf("%c",&ch);
	if (ch == 'n' || ch == 'N')
	    exit(-1);
	sleep(1);
    }
    if ((lockfd = creat(lockfile,0444)) == -1)
    {
	perror("Error opening lockfile");
	exit(-1);
    }
}

unlockscore()
{
    close(lockfd);
    if (unlink(lockfile) == -1)
    {
	perror("Error removing lockfile");
	exit(-1);
    }
}
