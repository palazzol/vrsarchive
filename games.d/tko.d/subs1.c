
/**********************************************************
 *							  *
 *                  TKO -- The Boxing Game		  *
 *							  *
 **********************************************************/


/*
Copyright (c) 1979  Mike Gancarz
Permission is hereby granted to freely distribute, copy, modify and otherwise
use this software for nonprofit purposes as long as this copyright appears
in the file and remains intact.
*/


#include "def.h"
#include "extern.h"


savegm()
{
	write(1, "\033[;H\033[2J", 8); /* DEC VT100 only */
	speed = 0;
	freopen("tko.sav","w",stdout);
}


ckname(str)	/* check for boxers' names in name file */
char str[LINE];
{
	register int i;
	int r;
	char fname[NAMSIZ];
	char lname[NAMSIZ];

	sscanf(str,"%s",fname);
	*fname=toupper(*fname);
	for(;;)		/* strip off any white space at end of name */
	{
		if(iswhite(str[strlen(str)-1]))
			str[strlen(str)-1]='\0';
		else break;
	}
	sscanf(rindex(str,' '),"%s",lname);
	*lname=toupper(*lname);
	r = -1;
	for(i=0; i < nboxers; i++)
	{
if (((strcmp(fname,bx[i].fname) == 0) && ((strcmp(lname,bx[i].lname) == 0) || strcmp(lname,bx[i].mname) == 0)) && (bx[i].weight == w))
		{
			r=i;
			break;
		}
	}
	return(r);
}


getrtst()	/* get run-time statistics */
{

	rt[0].bxptr = a;	/* get bx[] structure offsets */
	rt[1].bxptr = b;

	switch(bx[b].style)	/* determine boxer a's control factor */
	{
		case 'b':
			rt[0].ccf = bx[a].cfb;
			break;
		case 's':
			rt[0].ccf = bx[a].cfs;
			break;
		case 'e':
			if(bx[a].cfb == bx[a].cfs)
				rt[0].ccf = bx[a].cfb;
			if(bx[a].cfb > bx[a].cfs)
				rt[0].ccf = bx[a].cfs;
			if(bx[a].cfb < bx[a].cfs)
				rt[0].ccf = bx[a].cfb;
			break;
	}

	switch(bx[a].style)	/* determine boxer b's control factor */
	{
		case 'b':
			rt[1].ccf = bx[b].cfb;
			break;
		case 's':
			rt[1].ccf = bx[b].cfs;
			break;
		case 'e':
			if(bx[b].cfb == bx[b].cfs)
				rt[1].ccf = bx[b].cfb;
			if(bx[b].cfb > bx[b].cfs)
				rt[1].ccf = bx[b].cfs;
			if(bx[b].cfb < bx[b].cfs)
				rt[1].ccf = bx[b].cfb;
			break;
	}

				/* adjust maximum control factor to 10 */
	while((rt[0].ccf > 10) || (rt[1].ccf > 10))
	{
		--rt[0].ccf;
		--rt[1].ccf;
	}

				/* adjust minimum control factor to 10 */
	while((rt[0].ccf < 10) && (rt[1].ccf < 10))
	{
		++rt[0].ccf;
		++rt[1].ccf;
	}

	rt[0].chp = bx[a].hp;	/* get current hit power */
	rt[1].chp = bx[b].hp;

	rt[0].ckdr1 = bx[a].kdr1;	/* get current knockdown rating 1 */
	rt[1].ckdr1 = bx[b].kdr1;

	if(rt[0].chp == 10)	/* adjust kdr1 according to opponent's hp */
		rt[1].ckdr1 = rt[1].ckdr1 + 2;
	else if(rt[0].chp == 9)
		++rt[1].ckdr1;
	else if(rt[0].chp == 6)
		--rt[1].ckdr1;
	else if(rt[0].chp == 5)
		rt[1].ckdr1 = rt[1].ckdr1 - 2;

	if(rt[1].chp == 10)
		rt[0].ckdr1 = rt[0].ckdr1 + 2;
	else if(rt[1].chp == 9)
		++rt[0].ckdr1;
	else if(rt[1].chp == 6)
		--rt[0].ckdr1;
	else if(rt[1].chp == 5)
		rt[0].ckdr1 = rt[0].ckdr1 - 2;

	if(rt[0].ckdr1 <= 0)
		rt[0].ckdr1 = 1;
	if(rt[1].ckdr1 <= 0)
		rt[1].ckdr1 = 1;
	if(rt[0].ckdr1 > 10)
		rt[0].ckdr1 = 10;
	if(rt[1].ckdr1 > 10)
		rt[1].ckdr1 = 10;

	rt[0].cagg = bx[a].agg;		/* get current aggressiveness rating */
	rt[1].cagg = bx[b].agg;
hmr:
	printf("How many rounds? ");	/* find out how many rounds */
	gets(temp);
	sscanf(temp,"%d",&nrounds);
	if((nrounds < 3) || (nrounds > 15))
	{
		printf("?TKO: 3 to 15 rounds only\n");
		goto hmr;
	}

			/* get endurance and adjust it for number of rounds */
	rt[0].cend = (bx[a].end * nrounds)/10;
	rt[1].cend = (bx[b].end * nrounds)/10;

			/* get punches landed and adjust according to defense */
	rt[0].cdef = bx[a].def;
	rt[1].cdef = bx[b].def;
	rt[0].cpl = bx[a].pl + rt[1].cdef;
	rt[1].cpl = bx[b].pl + rt[0].cdef;

			/* set punch values at normal; these may change if a
			   boxer injures himself */
	rt[0].cpv3 = rt[1].cpv3 = 3;
	rt[0].cpv2 = rt[1].cpv2 = 2;
}


int getagg()	/* determine the aggressor of the fight */
{
	if(rt[0].cagg > rt[1].cagg)
		return(0);
	if(rt[1].cagg > rt[0].cagg)
		return(1);
	if(bx[a].rating > bx[b].rating)
		return(0);
	if(bx[b].rating > bx[a].rating)
		return(1);
	if(rnd80 > rnd80)
		return(0);
	else return(1);
}


ptable()	/* print "tale of the tables" message */
{
	printf("\n\n\t\t\tTALE OF THE TABLES\n\n");
	printf("\t\t    %-12s%s\n",bx[a].lname,bx[b].lname);
	printf("%-20s%-12d%-6dhigh numbers mean better control\n","Control factor:",rt[0].ccf,rt[1].ccf);
	printf("%-20s%-12d%-6dthe higher the number, the harder he hits\n","Hit power:",rt[0].chp,rt[1].chp);
	printf("%-20s%-12d%-6dhigh number: he cleans up if you look hurt","Killer instinct:",bx[a].ki,bx[b].ki);
	printf("%-20s%-12d%-6dlower number: means he has a tougher chin\n","Knockdown rating:",rt[0].ckdr1+bx[a].kdr2,rt[1].ckdr1+bx[b].kdr2);
	printf("%-20s%-12d%-6dhigh number: he has staying power\n","Endurance:",rt[0].cend,rt[1].cend);
	printf("%-20s%-12d%-6dlow number: difficult to knock him out\n","Knockout rating:",bx[a].kor,bx[b].kor);
	printf("%-20s%-12.0f%-6.0f\n","Punch accuracy(%):",(float)100*rt[0].cpl/bx[a].pm,(float)100*rt[1].cpl/bx[b].pm);
	printf("%-20s%-12d%-6dhigh number: he bleeds easily\n","Cut susceptibility:",bx[a].ch+bx[b].co,bx[b].ch+bx[a].co);
	printf("%-20s%-12d%-6dlow number: he can take a lot of punches\n","Tko susceptibility:",bx[a].tko,bx[b].tko);
	printf("\n\t%s will be the aggressor.\n",bx[rt[fagg].bxptr].lname);
}


pround()	/* print the round number */
{
	register int cc,size1,size2;

	sprintf(temp,"*  %s vs. %s  *\n",bx[a].lname,bx[b].lname);
	size1 = strlen(temp);
	if(round < 10)
		size2 = 7;
	 else	size2 = 8;

	for(cc=1; cc < size1; cc++)
		putchar('*');
	putchar('\n');
	putchar('*');
	for(cc=2; cc < size1-1; cc++)
		putchar(' ');
	printf("*\n%s*",temp);
	for(cc=1; cc < (size1 - size2)/2; cc++)
		putchar(' ');
	printf("ROUND %d",round);
	for(cc=((size1-size2)/2)+size2; cc < size1 - 2; cc++)
		putchar(' ');
	putchar('*');
	putchar('\n');
	putchar('*');
	for(cc=2; cc < size1-1; cc++)
		putchar(' ');
	putchar('*');
	putchar('\n');
	for(cc=1; cc < size1; cc++)
		putchar('*');
	putchar('\n');
}



getcard()	/* get an unused boxing action card */
{
	register int i,tmp;

	--actcnt;
	if(killer > 0)	/* if killer instinct in effect, reduce killer count */
		--killer;

	if(cused[79] != 0)	/* if all cards used, return a random card */
		return(rnd80);

	for(;;)
	{
		tmp = rnd80;
		for(i=0; i < 80; i++)
		{
			if(cused[i] == tmp)
				break;
			if(cused[i] == 0)
			{
				cused[i] = tmp;
				return(tmp);
			}
		}
	}
}



score()		/* compute the winner of the round */
{
	register byte hiscore;

	if(rt[0].cnpts == rt[1].cnpts)	/* even round */
	{
		if((rt[0].ckdflg > 0) && (rt[1].ckdflg > 0))
			return;
		else if(rt[0].ckdflg > 0)
		{
			++rt[1].cj1scr;
			++rt[1].cj2scr;
			++rt[1].crscr;
		}
		else if(rt[1].ckdflg > 0)
		{
			++rt[0].cj1scr;
			++rt[0].cj2scr;
			++rt[0].crscr;
		}
		return;
	}

	if(rt[0].cnpts > rt[1].cnpts)
		hiscore=0;
	else hiscore=1;

	if((rt[hiscore].cnpts - rt[flip(hiscore)].cnpts) >= 6)
	{
		++rt[hiscore].cj1scr;
		++rt[hiscore].cj2scr;
		++rt[hiscore].crscr;
	}
	else {
		ccard=getcard();

		switch(CARD.acj1)
		{
		case 'H':
			++rt[hiscore].cj1scr;
			break;
		case 'L':
			++rt[flip(hiscore)].cj1scr;
		case 'E':
			break;
		}

		switch(CARD.acj2)
		{
		case 'H':
			++rt[hiscore].cj2scr;
			break;
		case 'L':
			++rt[flip(hiscore)].cj2scr;
		case 'E':
			break;
		}

		switch(CARD.acref)
		{
		case 'H':
			++rt[hiscore].crscr;
			break;
		case 'L':
			++rt[flip(hiscore)].crscr;
			break;
		case 'E':
			break;
		}
	}
	return;
}
