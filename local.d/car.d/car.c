/*
 *	car - auto maintenance reminder service.
 *	Col. Sicherman - 9 Nov 1982.
*/

/*
 *	car.log consists of line images in the following form:
 *
 *	V 811231
 *	B 820101
 *	? 831225 112900
 *
 *	Mileages are always in miles, not kilometers.
 *	Fields are separated by white space.
*/

/*
 *	form of .carrc:
 *
 *	B=replace brakes
 *	V=check veeblefetzer
 *	B 20000 365d
 *	O 6m
 *	V 4O
 *
 *	I.e., replace brakes every 20,000 miles or 365 days, whichever
 *	comes first; change oil every 6 months; check veeblefetzer every
 *	4 oil changes.
*/

#define MAXDEPEND 10
#define PASSWORD "/etc/passwd"
#define MAIL "mail"
#define NNSIZE 512
#define TOMILES 0.62137
#define TOGALLONS 0.2641721

#include <stdio.h>
#include <time.h>

typedef long int datetype;

FILE *Specs, *Log;
FILE *Temp, *Password;
FILE *Mail;
char *strcat(), *strcpy();
char tempname[] = "/usr/tmp/carXXXXXX";
char *servicename[26] = {"change air filter","<B>","<C>","<D>",
	"<E>","change oil filter","<G>","<H>","<I>",
	"<J>","check oil","<L>","<M>","<N>",
	"change oil","<P>","<Q>","<R>","<S>",
	"<T>","<U>","<V>","<W>","<X>",
	"<Y>","<Z>"};
char newnamespace[NNSIZE], *nnptr = newnamespace;
int line;		/* INPUT LINE COUNTER */
double odometer;	/* MILEAGE COUNTER FOR PHASE 2 */
double fuel;		/* FUEL CONSUMED */
int metric = 0;		/* FLAG FOR PRINT FUEL CONSUMPTION IN METRIC */
datetype calendar;	/* DATE COUNTER FOR PHASE 3 */
datetype today;		/* TODAY'S DATE */
int changes;		/* FLAG FOR CHANGES IN PHASE 4 */
int moff[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

struct dencytype {
	char key;
	long int cycle, count;
};

struct servicetype {
	long int dents;
	int ndencies;	/* NUMBER OF DEPENDENCIES */
	int isdue;	/* 0 IF NOT DUE */
	struct dencytype dency[MAXDEPEND];
	datetype lastdate;
	long int lastmile;
} service[27], *sp;

main(argc,argv)
int argc;
char **argv;
{
	if (argc>2) bomb();
	if (argc>1) switch (**++argv) {
	case '-':
		switch(*++*argv) {
		case '\0':
			talk();
			break;
		case 'm':
			alltalk();
			break;
		case 'f':
			switch (*++*argv) {
			case 'm':
				metric++;
			case '\0':
				fueltalk();
				break;
			default:
				bomb();
			}
			break;
		default:
			bomb();
		}
		break;
	case '%':	/* Not suitable for crontab! */
		alltalk();
		break;
	default:
		bomb();
	}
	else listen();
}

talk()
{
	if (NULL==(Specs=fopen(".carrc","r"))) {
		fprintf(stderr,"car: cannot read .carrc\n");
		exit(1);
	}
	if (NULL==(Log=fopen("car.log","r"))) {
		fprintf(stderr,"car: cannot read car.log\n");
		exit(1);
	}
	talk1(Specs,Log,stdout);
}

alltalk()
{
	char c;
	char user[9];
	char home[40];
	char assem[50];
	char *mktemp();
	FILE *popen();
	mktemp(tempname);
	if (NULL==(Temp=fopen(tempname,"w+"))) {
		fprintf(stderr,"car: cannot write temp file\n");
		exit(1);
	}
	unlink(tempname);	/* SO IT'S SURE TO DISAPPEAR */
	if (NULL==(Password=fopen(PASSWORD,"r"))) {
		fprintf(stderr,"car: cannot read %s\n",PASSWORD);
		exit(1);
	}
	while (2==fscanf(Password,
	"%[^:]:%*[^:]:%*[^:]:%*[^:]:%*[^:]:%[^:]", user, home)) {
		while ('\n'!=getc(Password));
		strcpy(assem,home);
		strcat(assem,"/.carrc");
		if (NULL==(Specs=fopen(assem,"r"))) {
			continue;
		}
		strcpy(assem,home);
		strcat(assem,"/car.log");
		if (NULL==(Log=fopen(assem,"r"))) {
			fprintf(Temp,"car: cannot read %s\n",assem);
			goto mailit;
		}
		talk1(Specs,Log,Temp);
		fclose(Log);
mailit:
		fclose(Specs);
		rewind(Temp);
		strcpy(assem,MAIL);
		strcat(assem," ");
		strcat(assem,user);
		if (NULL==(Mail=popen(assem,"w"))) {
			fprintf(stderr,"car: cannot mail to %s\n",user);
			continue;
		}
		while (EOF!=(c=getc(Temp))) putc(c,Mail);
		pclose(Mail);
		rewind(Temp);
	}
}

listen()
{
	long int clock, time();
	char hold[20];
	char k;
	char *ctime();
	int r;
	double m;
	Specs=fopen(".carrc","r");
	if (NULL==(Log=fopen("car.log","a"))) {
		fprintf(stderr,"car: cannot open car.log\n");
		exit(1);
	}
	time(&clock);
	if (Specs) if (phase1(Specs,stderr)) exit(1);
	whatday();
	printf("\ncar \t%s\n\n",ctime(&clock));
	for (;;) {
		printf("Report (type h for help): ");
		if (EOF==scanf(" %s",hold)) break;
		while ('A'<=hold[0] && 'Z'>=hold[0]) {
			fprintf(Log,"%c %ld\n",hold[0],today);
			printf("\tReported: %s\n",servicename[indx(hold[0])]);
			strcpy(hold,hold+1);
		}
		if ('0'<=hold[0] && '9'>=hold[0]) {
			r=sscanf(hold,"%lf%c",&m,&k);
			if (r<2) {
				printf("\tReported: %lf miles\n",m);
				fprintf(Log,"? %ld %lf\n",today,m);
			}
			else if ('k'==k) {
				printf("\tReported: %lf kilometers\n",m);
				m*=TOMILES;
				fprintf(Log,"? %ld %lf\n",today,m);
			}
			else if ('g'==k) {
				printf("\tReported: %lf gallons\n",m);
				fprintf(Log,"f %ld %lf\n",today,m);
			}
			else if ('l'==k) {
				printf("\tReported: %lf liters\n",m);
				m*=TOGALLONS;
				fprintf(Log,"f %ld %lf\n",today,m);
			}
			else printf("\tError in suffix\n");
		}
		else if (hold[0]=='h') help();
		else if (hold[0]=='q') break;
		else if (hold[0]) printf("\tBad command - %c\n",hold[0]);
	}
}

#define SVCNAMELENGTH 80

talk1(S,L,O)
FILE *S, *L, *O;
{
	char s;
	char *malloc();
	int i;

	if (phase1(S,O)) return;
/*
 *	PHASE 2 - FIRST PASS THROUGH LOG FILE.
 */
	line=0;
	while (EOF!=(s=getc(L))) {
		line++;
		if ((s<'A'||s>'Z')&&s!='?'&&s!='f') {
			badlog(O);
			return;
		}
		if (1!=fscanf(L," %ld",&calendar)) {
			badlog(O);
			return;
		}
		if ('?'==s) if (1!=fscanf(L," %lf",&odometer)) {
			badlog(O);
			return;
		}
		if ('f'==s) if (1!=fscanf(L," %lf",&fuel)) {
			badlog(O);
			return;
		}
		/* FLUSH THE LINE */
		while ('\n'!=getc(L));
		sp = &service[indx(s)];
		sp->lastdate=calendar;
		sp->lastmile=odometer;
	}
/*
 *	PHASE 3 - SECOND PASS THROUGH LOG FILE.
 */
	rewind(L);
	while (EOF!=(s=getc(L))) {
		fscanf(L," %ld",&calendar);
		while ('\n'!=getc(L));
		if ('f'!=s && '?'!=s) xref(s);
	}
/*
 *	PHASE 4 - COMPUTATION.
 */
	whatday();
	do phase4();
	while (changes);
	/* PRINT MESSAGES */
	for (i=0; i<26; i++) if (service[i].isdue)
	fprintf(O,"car - %s\n",servicename[i]);
	if (service[26].isdue) fprintf(O,"car - please report mileage\n");
}

int phase1(S,O)
FILE *S, *O;
{
	int i;
	char c, s, k;
	long int n;
	for (i=0; i<27; i++) {
		sp = &service[i];
		sp->lastmile = 0;
		sp->lastdate = 100;
		sp->isdue = sp->ndencies = sp->dents = 0;
	}
	while (EOF!=(s=getc(S))) {
		c=getc(S);
		switch (c) {
		case '=':
			i=0;
			if (s<'A'||s>'Z') {
				fprintf(O,"car: bad definition '%c'\n",s);
				return 1;
			}
			servicename[s-'A']=nnptr;
			while ('\n'!=(c=getc(S))) *nnptr++ = c;
			*nnptr++ ='\0';
			break;
		case ' ':
		case '\t':
			if ((s<'A'||s>'Z')&&s!='?') {
				fprintf(O,"car: bad rule '%c'\n",s);
				return 1;
			}
			for (;;) {
				if (1!=fscanf(S," %ld",&n)) {
					fprintf(O,
					"car: bad rule '%c'\n",s);
					return 1;
				}
				c=getc(S);
				if (c==' '||c=='\t'||c=='\n') k='?';
				else if (c==s) {
					fprintf(O,
					"car: '%c' depends on itself\n",
					s);
					return 1;
				}
				else if ((c<'A'||c>'Z')&&c!='d'&&
				c!='w'&&c!='m'&&c!='y'&&c!='k') {
					fprintf(O,"car: bad rule '%c'\n",
					s);
					return 1;
				}
				else if ('k'==c) {/* KILOMETERS */
					k='?';
					n*=TOMILES;
				}
				else k=c;
				sp = &service[indx(s)];
				sp->dency[sp->ndencies].cycle=n;
				sp->dency[sp->ndencies].key=k;
				sp->dency[sp->ndencies].count=0;
				sp->ndencies++;
				/* CROSS-REFERENCE */
				if ('A'<=k&&'Z'>=k)
				service[indx(k)].dents|=1L<<indx(s);
				/* ADVANCE TO NEXT FIELD */
				if ('\n'!=c) do c=getc(S);
				while (' '==c||'\t'==c);
				if ('\n'==c) break;
				else ungetc(c,S);
			}
			break;
		default:
			fprintf(O,
			"car: syntax error in specifications file\n");
			return 1;
		}
	}
	/* DEFAULT DEPENDENCY FOR MILEAGE CHECK */
	sp = &service[26];
	if (!sp->ndencies) {
		sp->ndencies = 1;
		sp->dency[0].key = 'w';
		sp->dency[0].cycle = 1;
		sp->dency[0].count = 0;
	}
	return 0;
}

phase4()
{
	int i, j;
	struct servicetype *sp;
	struct dencytype *u;
	changes = 0;
	for (i=0; i<27; i++) {
		sp = &service[i];
		if (sp->isdue) continue;
		if (!sp->ndencies) continue;
		for (j=0; j<sp->ndencies; j++) {
			u = &sp->dency[j];
			if ('?'==u->key) {
				/* MILEAGE DEPENDENCY */
				if (odometer>=sp->lastmile+u->cycle)
				goto markdue;
			}
			else if ('d'==u->key || 'w'==u->key
			|| 'm'==u->key || 'y'==u->key) {
				/* TIME DEPENDENCY */
				if (expired(u,sp->lastdate))
				goto markdue;
			}
			/* SERVICE DEPENDENCY */
			else if (u->count >= u->cycle) goto markdue;
		}
		/* NOT DUE - GO TO NEXT SERVICE */
		continue;
markdue:
		sp->isdue++;
		if (i<26) xref((char)('A'+i));
		changes++;
	}
}


int expired(u,d)
struct dencytype *u;
datetype d;
{
	long int now, then;
	switch (u->key) {
	case 'y':
		return (today>=d+10000L*u->cycle);
	case 'm':
		now = today/10000L * 12 + (today/100)%100;
		then = d/10000L * 12 + (d/100)%100;
		if (today%100 < d%100) now--;
		return (now >= then+u->cycle);
	case 'w':
		u->key='d';
		u->cycle*=7;
		/* SO MUCH FOR THAT! */
	case 'd':
		now = today/10000L * 365 + moff[(today/100)%100-1] + today%100;
		then = d/10000L * 365 + moff[(d/100)%100-1] + d%100;
		return (now >= then+u->cycle);
	}
}

update(i,s)
int i;
char s;
{
	struct servicetype *rp;
	int j;
	rp = &service[i];
	for (j=0; j<rp->ndencies; j++)
	if (rp->dency[j].key == s)
	if (calendar>rp->lastdate) rp->dency[j].count++;
}

whatday()
{
	long int clock, time();
	struct tm *l, *localtime();
	time(&clock);
	l=localtime(&clock);
	today=10000L*l->tm_year+100*l->tm_mon+l->tm_mday+100;
}

int indx(c)
char c;
{
	return ('?'==c ? 26 : c-'A');
}

xref(s)
char s;
{
	int i;
	sp = &service[indx(s)];
	for (i=0; i<26; i++) if ((1L<<i) & sp->dents) update(i,s);
}

fueltalk()
{
	double fuel0, mile0, fueltot;
	int isfuel;
	char c;
	if (NULL==(Log=fopen("car.log","r"))) {
		fprintf(stderr,"car: cannot read car.log\n");
		exit(1);
	}
	mile0 = fuel0 = -1.0;
	fueltot = 0.0;
	line = 0;
	while (EOF!=(c=getc(Log))) {
		line++;
		switch (c) {
		case 'f':
			isfuel=1;
			if (1!=fscanf(Log," %*ld %lf",&fuel)) {
				badlog(stderr);
				exit(1);
			}
			if (fuel0<0) fuel0=fuel;
			fueltot += fuel;
			break;
		case '?':
			isfuel=0;
			if (1!=fscanf(Log," %*ld %lf",&odometer)) {
				badlog();
				exit(1);
			}
			if (fuel0<0) break;
			if (mile0<0) mile0=odometer;
		}
		while ('\n'!=getc(Log));
	}
	if (isfuel) fueltot -= fuel;
	if (fueltot==0.0 || odometer==mile0) {
		fprintf(stdout,"insufficient data\n");
		exit(0);
	}
	odometer -= mile0;
	if (metric) {
		odometer /= TOMILES;
		fueltot /= TOGALLONS;
	}
	printf("\t%.1lf %s / %.1lf %s = %.2lf\n",
		odometer, metric?"km":"mi", fueltot, metric?"l":"gal",
		odometer/fueltot);
}

help()
{
	int i;
	putchar('\n');
	for (i=0; i<26; i++) if ('<'!=*servicename[i])
	printf("\t%c\t%s\n",(char)(i+'A'),servicename[i]);
	printf("\t1234\tcurrent mileage\n");
	printf("\t1234k\tcurrent mileage in kilometers\n");
	printf("\t12.3g\tfuel consumed in US gallons\n");
	printf("\t12.3l\tfuel consumed in liters\n");
	printf("\th\thelp\n");
	printf("\tq\tquit\n\n");
}

bomb()
{
	fprintf(stderr,"usage: car [-] [-m] [-f[m]]\n");
	exit(1);
}

badlog(stm)
FILE *stm;
{
	fprintf(stm,"car: bad log entry, line %d\n",line);
	return;
}
