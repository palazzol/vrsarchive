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


main()
{
	int i,z;

	printf("TKO\tVersion 1.1\n\n");

	srand(getpid());
	for(nboxers=0;;)	/* count the number of available boxers */
		if(bx[++nboxers].rating == 0) break;

	for(w=1; w < 12; w++)
		printf("%d= %s\n",w,wc[w]);
	for(;;)		/* query the operator for weight class */
	{
		printf("Enter the weight class number: ");
		gets(temp);
		sscanf(temp,"%d",&w);
		if((w < 1) || (w > 11)) {
			printf("?TKO: no such weight class number\n");
		} else
			break;
	}

	printf("Do you wish to see a list of the boxers' names? ");
	gets(temp);
	if((toupper(temp[0])) == 'Y')
	{
		printf("\nTKO's star %ss are:\n",wc[w]);
		z=0;
		for(i=0; i < nboxers; i++)
		{
			if(bx[i].weight == w)
			{
		       	if(bx[i].mname[0] == '\0')
 	      			printf("%s %s\n",bx[i].fname,bx[i].lname);
			else
		       printf("%s %s %s\n",bx[i].fname,bx[i].mname,bx[i].lname);
			z++;
			}
			if(z > 20)
			{
				printf("\tPress <return> to see more ");
				gets(temp);
				z=0;
			}
		}
	}

	for(;;)		/* verify that this boxer is in the name file */
	{
		printf("Please print the boxer's full name: ");
		gets(temp);
		if((a=ckname(temp)) != -1)
			break;
		else printf("?TKO: invalid boxer name\n");
	}

loop1:
	for(;;)		/* verify that his opponent is in the name file */
	{
		printf("Please print his opponent's full name: ");
		gets(temp);
		if((b=ckname(temp)) != -1)
			break;
		else printf("?TKO: invalid boxer name\n");
	}

	if(bx[a].weight != bx[b].weight) /* are boxers in same weight class? */
	{
printf("?TKO: You can't match a %s with a %s!\n",wc[bx[b].weight],wc[bx[a].weight]);
		printf("Try another opponent.\n");
		goto loop1;
	}

	getrtst();	/* get run-time boxer statistics */

	for(;;)		/* query the operator for the speed of the fight */
	{
		printf("Specify fight speed [0 (fast) to 10 (slow)]: ");
		gets(temp);
		sscanf(temp,"%d",&speed);
		if(speed > 10) {
			printf("?TKO: valid speeds are from 0 to 10\n");
		} else
			break;
	}

	fagg = getagg();	/* get the aggressor of the fight */
	ptable();	/* print the "tale of the tables" message */
	rt[0].cncpts=rt[1].cncpts=rt[0].pncpts=rt[1].pncpts=0;

	printf("\nPress <return> to begin the fight.");
	gets(temp);
	signal(SIGINT,savegm);
	printf("\n\n");

	RTOFF.catko1=RTDEF.catko1=0; /* reset auto tko points counter */
	RTOFF.catko2=RTDEF.catko2=0; /* reset auto tko points counter */
	deck = 0;	/* reset the action card array */
	for(i=0; i < 80; i++)
		cused[i] = 0;

	/****************************************************
	 *	The following loop contains the code for the*
	 *	live action of the fight		    *
	 ****************************************************/

	for(round=1; round <= nrounds; round++)
	{
		pround();	/* print the round number */
		RTOFF.cnpts=RTDEF.cnpts=0; /* reset punch points counter */
		RTOFF.ckdflg=RTDEF.ckdflg=0; /* clear knockdown flag */
		kdcount=0;	/* clear knockdown counter */
		offense = fagg; /* aggressor starts action on offense */
		actcnt = 40;	/* reset boxing action card counter */
		killer = NOKILL;/* reset killer instinct flag */

	ccard = getcard();	/* see who's in control */
#ifdef DEBUG
printf("1 card count = %d\tcard = %d\n",actcnt,ccard);
#endif

	while(actcnt > 0)
	{
		if((killer <= 0) && (CARD.accf > RTOFF.ccf))
		{
			offense = flip(offense);
#ifdef DEBUG
		       printf("%s is in control\n",OFFNAME);
#endif
		}

		if(killer > 0)
			sleep(speed/2);
		else sleep(speed);
		ccard = getcard();	/* determine the action */
#ifdef DEBUG
printf("2 card count = %d\tcard = %d\n",actcnt,ccard);
#endif
		if(ccard <= RTOFF.cpl)
			punch();
		else if(ccard <= BXOFF.pm)
			miss();
		else if(ccard <= BXOFF.cl)
			clinch();
		else if(ccard < 79)
			movement();
		else if(ccard == 79)
			foul();
		else injury();

		if(killer <= 0)		/* if not in killer instinct, */
		{
			ccard = getcard();	/* see who's in control       */
#ifdef DEBUG
printf("1 card count = %d\tcard = %d\n",actcnt,ccard);
#endif
		}
	}

	if(deck == 0)	/* clear portions of used card array */
		i=40;
	else i=0;
	for(; i < 80; i++)
		cused[i] = 0;
#ifdef DEBUG
printf("deck used was %d\n",deck);
#endif
	deck = flip(deck);
	printf("End of round %d\n",round);
	sleep(speed);
	if(round < nrounds)
		tkocut();	/* check for a tko because of cuts */

		/* decrease hit power if boxer has used up his endurance */
	if(rt[0].cend <= 0)
	{
		--rt[0].cpl;	/* reduce punches landed, too */
		if(rt[0].chp > 1)
			--rt[0].chp;
	}
	if(rt[1].cend <= 0)
	{
		--rt[1].cpl;	/* reduce punches landed, too */
		if(rt[1].chp > 1)
			--rt[1].chp;
	}

printf("Points this round: %s(%d)\t%s(%d)\n",bx[rt[0].bxptr].lname,rt[0].cnpts,bx[rt[1].bxptr].lname,rt[1].cnpts);

			/* deduct endurance points for punches in this round */
	rt[0].cend=rt[0].cend-rt[1].cnpts;
	rt[1].cend=rt[1].cend-rt[0].cnpts;
printf("Endurance left: %s(%d)\t%s(%d)\n\n",BX0NAM,rt[0].cend,BX1NAM,rt[1].cend);

	score();	/* compute the winner of round */
	if(round < nrounds)
		autotko();	/* check for an automatic tko */

	sleep(speed*2);
	}

	printf("We are awaiting the scoring of the judges...\n");
	sleep(speed*2);
	printf("Judge #1 scores the fight:\t");
	printf("%d-%d-%d ",J1WSCR,J1LSCR,nrounds-(J1WSCR+J1LSCR));
	if(J1WSCR== J1LSCR)
		puts("even");
	else puts(bx[rt[J1WIN].bxptr].lname);
	sleep(speed);
	printf("Judge #2 scores the fight:\t");
	printf("%d-%d-%d ",J2WSCR,J2LSCR,nrounds-(J2WSCR+J2LSCR));
	if(J2WSCR== J2LSCR)
		puts("even");
	else puts(bx[rt[J2WIN].bxptr].lname);
	sleep(speed);
	printf("The referee scores the fight:\t");
	printf("%d-%d-%d ",REFWSCR,REFLSCR,nrounds-(REFWSCR+REFLSCR));
	if(REFWSCR== REFLSCR)
		puts("even");
	else puts(bx[rt[REFWIN].bxptr].lname);
}
