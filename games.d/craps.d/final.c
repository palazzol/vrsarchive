#include "types.h"
#include "ext.h"
#ifdef __STDC__
#include <stdlib.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>

/*
 *
 * here is the file name for the high roller list.
 * It will need to be modified for your system.
 *
 */
#ifdef	SCORES
#ifdef	__STDC__
#define STR(x)		#x
#define STRING(x)	STR(x)
#define FILNAM(x)	STRING(GAMLIB) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILNAM(x)	STRING(GAMLIB)/x"
#endif
char *sfile = FILNAM(craps.list);
char *reclock = FILNAM(craps.lock);

#define	NAMELEN		40
typedef struct node {
	struct node *next;
	int uid;
	long ngames;
	double amt;
	char name[NAMELEN+1];
} scores;
#endif

final(d)
int d;
{
#ifdef	SCORES
	FILE *list;
	int f,sleepct=300,cuid,did=0,i,n=0,comp(),nchars;
	long l;
	double x;
	char s[NAMELEN+1],c;
	scores *score;

	cuid=getuid();
	clear(); refresh();
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	while(link(sfile, reclock) == -1) {
		perror(reclock);
		if(!sleepct--) {
			puts("I give up. Sorry.");
			puts("Perhaps there is an old record_lock around?");
			exit(-1);
		}
		printf("Waiting for access to record file. (%d)\n",
			sleepct);
		fflush(stdout);
		sleep(1);
	}
	if((list=fopen(sfile,"r"))==NULL) {
		fprintf(stderr,"can't open %s\n",sfile);
		myexit();
		return(0);
	}
	while((c=fgetc(list))!=EOF) if(c=='\n') n++;
	rewind(list);
	score=(scores *)malloc((n+1)*sizeof(scores));
	cuid=getuid();
	i=0;
	while(1) {
		if((fscanf(list,"%d %lf %ld",
			&score[i].uid,
			&score[i].amt,
			&score[i].ngames))
		== EOF) break;
		while((c=fgetc(list))==' ') ;
		nchars=0;
		while(c!='\n' && nchars<NAMELEN) {
			score[i].name[nchars++]=c;
			c=fgetc(list);
		}
		while(c!='\n') c=fgetc(list);
		score[i].name[nchars]=0;
		if(score[i].uid==cuid) {
			score[i].amt = score[i].amt + (wins-loss);
			score[i].ngames = score[i].ngames + 1;
			did=1;
		}
		i++;
	}
	fclose(list);
	if(!did) {
		score[n].uid = cuid;
		score[n].amt = (wins-loss);
		score[n].ngames = 1;
		if(getenv("CRAPSNAME")==NULL)
#ifdef	SYSV
			strncpy(score[n].name,getenv("LOGNAME"),NAMELEN);
#else
#ifdef  XENIX
			strncpy(score[n].name,getenv("LOGNAME"),NAMELEN);
#else
			strncpy(score[n].name,getenv("USER"),NAMELEN);
#endif
#endif
		else strncpy(score[n].name,getenv("CRAPSNAME"),NAMELEN);
		n++;
	}
	qsort(score,n,sizeof(scores),comp);
	list=fopen(sfile,"w");
	for(i=0;i<n;i++)
		fprintf(list,"%d %.2f %ld %s\n",
		score[i].uid,
		score[i].amt,
		score[i].ngames,
		score[i].name);
	fclose(list);
	clear();
	mvaddstr(0,10,"Name                Total to Date              Games");
	mvaddstr(1,10,"----------------------------------------------------");
	refresh();
	putchar('\n');
	for(i=0;i<n;i++)
		printf("          %-28s%10.2f%14ld\n",score[i].name,score[i].amt,score[i].ngames);
	myexit();
#endif
	return(0);
}

#ifdef	SCORES
comp(x,y)
scores *x,*y;
{
	if(x->amt > y->amt) return(-1);
	if(x->amt == y->amt) return(0);
	return(1);
}
#endif

myexit()
{
	unlink(reclock);
}
