#include "types.h"
#include "ext.h"
#include <sys/types.h>
#include <sys/file.h>
#include <pwd.h>
extern struct passwd *getpwuid();

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
char *fil = FILNAM(craps.list);
char *reclock = FILNAM(craps.lock);

typedef struct node {
	struct node *next;
	int uid;
	long ngames;
	double amt;
	char name[15];
} scores;
#endif

final(d)
int d;
{
#ifdef	SCORES
	FILE *list;
	int f,sleepct=300,cuid,did=0,i,n=0,comp();
	long l;
	double x;
	char s[15];
	scores *score;

	cuid=getuid();
	clear(); refresh();
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	while(link(fil, reclock) == -1) {
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
	if((list=fopen(fil,"r"))==NULL) {
		fprintf(stderr,"can't open %s\n",fil);
		myexit();
		return(0);
	}
	while((fscanf(list,"%d %f %ld %s",&i,&x,&l,s))!=EOF) n++;
	rewind(list);
	score=(scores *)malloc((n+1)*sizeof(scores));
	cuid=getuid();
	i=0;
	while(1) {
		if((fscanf(list,"%d %f %ld %s",
			&score[i].uid,
			&score[i].amt,
			&score[i].ngames,
			score[i].name))
		== EOF) break;
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
			if(getenv("USER")==NULL)
				strcpy(score[n].name,getpwuid(cuid)->pw_name);
			else
				strcpy(score[n].name,getenv("USER"));
		else
			strcpy(score[n].name,getenv("CRAPSNAME"));
		n++;
	}
	qsort(score,n,sizeof(scores),comp);
	list=fopen(fil,"w");
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
		printf("          %-15s %17.2f        %11ld\n",score[i].name,score[i].amt,score[i].ngames);
	myexit();
#endif
	return(0);
}

comp(x,y)
scores *x,*y;
{
	if(x->amt > y->amt) return(-1);
	if(x->amt == y->amt) return(0);
	return(1);
}

myexit()
{
	unlink(reclock);
}
