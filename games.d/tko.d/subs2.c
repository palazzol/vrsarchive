
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


punch()		/* a punch was landed */
{
	register int c;
	int kdres,strong;

	kdres=NOKD;
	ccard = lcard = getcard();
#ifdef DEBUG
printf("3 card count = %d\tcard = %d\n",actcnt,ccard);
#endif
	if(CARD.accutk == 100)	/* check for a chance of a knockdown */
		kdres=kdcheck();

	if(kdres == NOKD)
	{
		strong=gpunch();	/* get the punch */

		c = rand() % 8;
		switch(c)
		{
		case 0:
			printf("%s connects with a ",OFFNAME);
			break;
		case 1:
			printf("%s lands a ",OFFNAME);
			break;
		case 2:
			printf("%s catches %s with a ",OFFNAME,DEFNAME);
			break;
		case 3:
			printf("%s tags %s with a ",OFFNAME,DEFNAME);
			break;
		case 4:
			printf("%s pops %s with a ",OFFNAME,DEFNAME);
			break;
		case 5:
			printf("%s is on target with a ",OFFNAME);
			break;
		case 6:
			printf("%s delivers a ",OFFNAME);
			break;
		case 7:
			printf("%s stings him with a ",OFFNAME);
		}

		if(strong == YES)
		{
					/* score points for punch landed */
			RTOFF.cnpts=RTOFF.cnpts+RTOFF.cpv3;

					/* describe the punch verbally */
			c = rand() % 5;
			switch(c)
			{
			case 0:
				fputs("tough",stdout);
				break;
			case 1:
				fputs("solid",stdout);
				break;
			case 2:
				fputs("firm",stdout);
				break;
			case 3:
				fputs("powerful",stdout);
				break;
			case 4:
				fputs("heavy",stdout);
			}
		} else {
					/* score points for punch landed */
			RTOFF.cnpts=RTOFF.cnpts+RTOFF.cpv2;

			c = rand() % 5;
			switch(c)
			{
			case 0:
				fputs("light",stdout);
				break;
			case 1:
				fputs("soft",stdout);
				break;
			case 2:
				fputs("short",stdout);
				break;
			case 3:
				fputs("weak",stdout);
				break;
			case 4:
				fputs("glancing",stdout);
			}
		}
		puts(ptype);
		if(BXOFF.co >= LCARD.accutk)	/* check for chance of a cut */
			cutcheck();
	}
}



miss()
{
	register int c;

	--RTOFF.cend;	/* deduct an endurance point for a miss */
	c = rand() % 6;

	switch(c)
	{
	case 0:
		printf("%s swings and misses\n",OFFNAME);
		break;
	case 1:
       printf("%s ducks as %s tries to fire one to his head\n",DEFNAME,OFFNAME);
		break;
	case 2:
		printf("%s avoids a stiff body punch\n",DEFNAME);
		break;
	case 3:
		printf("%s's swing is off target\n",OFFNAME);
		break;
	case 4:
		printf("%s tries to dig one in, but %s steps back out of reach\n",OFFNAME,DEFNAME);
		break;
	case 5:
		printf("%s\'s punch misses the mark as %s moves from side to side\n",OFFNAME,DEFNAME);
	}
}



clinch()
{
	register int c;
	c = rand() % 4;

	switch(c)
	{
	case 0:
		printf("%s is clinching\n",OFFNAME);
		break;
	case 1:
		printf("%s ties him up\n",OFFNAME);
		break;
	case 2:
		printf("%s ties up %s in the clinch\n",OFFNAME,DEFNAME);
		break;
	case 3:
		printf("%s holds on\n",OFFNAME);
	}

	sleep(speed);
	c = rand() % 6;

	switch(c)
	{
	case 0:
		printf("The referee separates them\n");
		break;
	case 1:
		printf("%s pushes away\n",DEFNAME);
		break;
	case 2:
	       printf("%s tries to force %s into the corner\n",OFFNAME,DEFNAME);
		break;
	case 3:
		printf("The referee steps in to separate them\n");
		break;
	case 4:
		printf("The referee has to pull them apart\n");
		break;
	case 5:
		break;
	}
}



movement()
{
	register int c;

	c = rnd80 % 4;
	switch(c)
	{
	case 0:
		printf("%s is moving around the ring\n",OFFNAME);
		break;
	case 1:
		printf("%s steps back away from %s\n",OFFNAME,DEFNAME);
		break;
	case 2:
		printf("%s moves to his left\n",OFFNAME);
		break;
	case 3:
		printf("%s moves to his right\n",OFFNAME);
	}
}



foul()		/* chance of a foul */
{
	printf("There is a chance of a foul\n");
}


injury()	/* chance that a fighter has been injured */
{
	ccard = getcard();
	if(ccard > 73)
	{
		switch(ccard)
		{
			case 74:
    printf("%s pops %s in the nose with a solid right cross\n",DEFNAME,OFFNAME);
				RTDEF.cnpts = RTDEF.cnpts+2;
				sleep(speed);
		printf("%s seems to be having trouble breathing\n",OFFNAME);
				if(RTDEF.ccf <= 9)
					RTDEF.ccf = RTDEF.ccf+2;
				else if(RTOFF.ccf > 2)
					RTOFF.ccf = RTOFF.ccf-2;
				sleep(speed);
			printf("It looks like %s's nose is broken!\n",OFFNAME);
				break;
			case 75:
	    printf("%s plants a crunching hook on %s's jaw\n",DEFNAME,OFFNAME);
				RTDEF.cnpts = RTDEF.cnpts+3;
				sleep(speed);
				printf("%s is grimacing in pain\n",OFFNAME);
				if(RTDEF.ccf <= 9)
					RTDEF.ccf = RTDEF.ccf+2;
				else if(RTOFF.ccf > 2)
					RTOFF.ccf = RTOFF.ccf-2;
				sleep(speed);
			    printf("%s is trying desperately to cover up\n",
					OFFNAME);
				sleep(speed);
				printf("%s broke his jaw!\n",DEFNAME);
				break;
			case 76:
				break;
			case 77:
				break;
			case 78:
				break;
			case 79:
				break;
		}
	}
}



gpunch()	/* get the actual punch */
{
		if(LCARD.achand == 'L') /* determine which hand */
			strcpy(ptype," left ");
		else strcpy(ptype," right ");

		if(LCARD.acpunch <= BXOFF.jab2)
		{
			strcat(ptype,"jab");
			if(LCARD.acpunch <= BXOFF.jab3)
				return(HARD);
			else return(SOFT);
		}

		if(LCARD.acpunch <= BXOFF.hook2)
		{
			strcat(ptype,"hook");
			if(LCARD.acpunch <= BXOFF.hook3)
				return(HARD);
			else return(SOFT);
		}

		if(LCARD.acpunch <= BXOFF.cross2)
		{
			strcat(ptype,"cross");
			if(LCARD.acpunch <= BXOFF.cross3)
				return(HARD);
			else return(SOFT);
		}

		if(LCARD.acpunch <= BXOFF.combo2)
		{
			strcat(ptype,"combination");
			if(LCARD.acpunch <= BXOFF.combo3)
				return(HARD);
			else return(SOFT);
		}

		strcat(ptype,"uppercut");
		if(LCARD.acpunch <= BXOFF.upper3)
				return(HARD);
			else return(SOFT);
}



kdcheck()	/* check for a knockdown */
{
	register int c,i,tmp;

	ccard=getcard();
#ifdef DEBUG
printf("4 card count = %d\tcard = %d\n",actcnt,ccard);
#endif
	if(CARD.ackd <= RTOFF.chp)	/* enought hit power? */
	{
		tmp = gpunch();	/* get the type of punch */

		c = rand() % 6;
		switch(c)
		{
		case 0:
			printf("%s unloads on %s with a ",OFFNAME,DEFNAME);
			break;
		case 1:
			printf("%s blasts %s with a ",OFFNAME,DEFNAME);
			break;
		case 2:
			printf("%s drives home a ",OFFNAME);
			break;
		case 3:
			printf("%s lets loose a ",OFFNAME);
			break;
		case 4:
			printf("%s smashes %s with a ",OFFNAME,DEFNAME);
			break;
		case 5:
			printf("%s pounds %s with a ",OFFNAME,DEFNAME);
		}

		c = rand() % 5;
		switch(c)
		{
		case 0:
			printf("tremendous%s!\n",ptype);
			break;
		case 1:
			printf("fantastic%s!\n",ptype);
			break;
		case 2:
			printf("devastating%s!\n",ptype);
			break;
		case 3:
			printf("crusher of a%s!\n",ptype);
			break;
		case 4:
			printf("pulverizing%s!\n",ptype);
		}

#ifdef DEBUG
printf("There is a chance of a knockdown\n");
#endif
		sleep(speed/2);
		ccard=getcard();
#ifdef DEBUG
printf("5 card count = %d\tcard = %d\n",actcnt,ccard);
#endif
		if(RTDEF.ckdflg > 0)
		    tmp = kdtab[(20*(BXDEF.kdr2+RTDEF.ckdr1-1))+(CARD.ackdr-1)];
		else tmp = kdtab[(20*(RTDEF.ckdr1-1))+(CARD.ackdr-1)];
#ifdef DEBUG
printf("Score a %d for tmp\n",tmp);
#endif
		switch(tmp)
		{
			case 0:
				RTOFF.cnpts=RTOFF.cnpts+4;
			       printf("%s looks a little wobbly\n",DEFNAME);
				return(NOKILL);
			case 1:
				RTOFF.cnpts=RTOFF.cnpts+5;
		printf("%s falls back against the ropes\n",DEFNAME);
				if(killer <= 0)
					killer = BXOFF.ki;
				return(killer);
			case 2:
				printf("%s hits the deck!\n",DEFNAME);
				sleep(speed/2);
				if(killer <= 0)
					killer = BXOFF.ki;
				kocheck();
				++RTDEF.ckdflg; /* set the knockdown flag */

				ccard = getcard(); /* determine the kd count */
				if(ccard <= 5)
					kdcount=1;
				else kdcount=((byte)ccard/10)+1;

				if(kdcount <= 3)	/* score the points */
					RTOFF.cnpts=RTOFF.cnpts+6;
				else if(kdcount <= 6)
					RTOFF.cnpts=RTOFF.cnpts+7;
				else RTOFF.cnpts=RTOFF.cnpts+kdcount+1;


				printf("The referee starts counting...\n");
				for(i=1; i <= kdcount; i++)
				{
					printf("\t%d\n",i);
					if(speed != 0)
						sleep(2);
					if(kdcount > 5) stumble();
				}
				printf("He's up at %d\n",kdcount);
				if(speed != 0)
					sleep(1);
				if(kdcount < 8)
	       printf("The referee gives him the mandatory standing 8 count\n");
				sleep(speed);
				return(killer);
		}
	}
	return(NOKD);
}



kocheck()	/* check for a knockout */
{
	int i;

	ccard = getcard();
	if(kdtab[(20*(RTDEF.ckdr1-1))+(CARD.ackdr-1)] == 2)
	{
		printf("The referee starts counting...\n");
		for(i=1; i < 11; i++)
		{
			printf("\t%d\n",i);
			if (speed != 0)
				sleep(2);
			stumble();
		}
	printf("%s cannot get up...%s has knocked him out!!\n",DEFNAME,OFFNAME);
		sleep(speed);
		if(round == 1)
	printf("%s wins by k.o. in the 1st round\n",OFFNAME);
		else if(round == 2)
	printf("%s wins by k.o. in the 2nd round\n",OFFNAME);
		else if(round == 3)
	printf("%s wins by k.o. in the 3rd round\n",OFFNAME);
	       else printf("%s wins by k.o. in the %dth round\n",OFFNAME,round);

		exit(0);
	}
}



stumble()	/* show a fighter stumbling when knocked down */
{
	int c;

	c = rand() % 30;
	if(c < 5)
	{
	switch(c)
	{
		case 0:
		 printf("%s tries to pull himself up with the ropes\n",DEFNAME);
			break;
		case 1:
    printf("The referee motions to %s to stay in the neutral corner\n",OFFNAME);
			break;
		case 2:
			printf("%s is struggling to get up\n",DEFNAME);
			break;
		case 3:
			printf("%s is up\n",DEFNAME);
			printf("He's back down\n");
			break;
		case 4:
			break;
	}
	}
}


cutcheck()	/* check for a cut */
{
	register byte c,tmp,z;

	ccard = getcard();
	sleep(speed);

	tmp = (BXDEF.ch-1)*9;
	z=0;
	for(c=0; c < 9; c++)
	{
		if(ccard <= cuttab[tmp++])
		{
			z=c;
			break;
		}
	}
	if(c == 9)
		z=c;

	switch(z)
	{
	case 0:
		break;
	case 1:
		printf("%s's nose is bleeding\n",DEFNAME);
		break;
	case 2:
      printf("There appears to be a slight cut under %s's right eye\n",DEFNAME);
		break;
	case 3:
      printf("There appears to be a slight cut under %s's left eye\n",DEFNAME);
		break;
	case 4:
     printf("%s has opened a slight cut over %s's right eye\n",OFFNAME,DEFNAME);
		break;
	case 5:
       printf("There is a small trickle of blood over %s's left eye\n",DEFNAME);
		break;
	case 6:
		printf("%s is cut on the bridge of his nose\n",DEFNAME);
		break;
	case 7:
		printf("%s's mouth is badly torn\n",DEFNAME);
		break;
	case 8:
  printf("%s has opened a terrible gash over %s's right eye\n",OFFNAME,DEFNAME);
		sleep(speed);
		printf("The referee asks the ring doctor take a look at it\n");
		sleep(speed);
   printf("The doctor says the eye is all right and allows them to continue\n");
		break;
	case 9:
  		printf("%s has a very bad gash over his left eye\n",DEFNAME);
		sleep(speed);
		printf("The referee asks the ring doctor take a look at it\n");
		sleep(speed);
   printf("The doctor says that %s can continue, although it doesn't look good\n",DEFNAME);
	}

	if((z > 0) && (z < 7))	/* score the cut point */
		++RTDEF.cncpts;
	else if(z >= 7)
	{
		RTDEF.cncpts=RTDEF.cncpts+2;
		if(RTOFF.ccf <= 9)   /* reduce control factor on nasty cuts */
			++RTOFF.ccf;
		else if(RTDEF.ccf > 1)
			--RTDEF.ccf;
	}
}



tkocut()	/* check for tko because of cuts */
{
	register int i;
	byte tko[2];

	for(i=0; i < 2; i++)
	{
		tko[i] = NO;
		if(rt[i].cncpts > rt[i].pncpts)
		{
			rt[i].pncpts=rt[i].cncpts;
     printf("%s's cornermen work to stop the bleeding\n",bx[rt[i].bxptr].lname);
			sleep(speed);
			if(rt[i].cncpts > 6)
			{
				ccard = getcard();
				switch(rt[i].cncpts)
				{
				case 7:
					if(ccard < 21)
						tko[i] = YES;
					else {
	      printf("The referee asks the ring doctor to examine %s\n",CUTNAM);
					sleep(speed);
			     printf("The doctor says %s can continue\n",CUTNAM);
					sleep(speed);
					}
					break;
				case 8:
					if(ccard < 41)
						tko[i] = YES;
					else {
				printf("%s's cornermen look worried\n",CUTNAM);
					sleep(speed);
				printf("Their man can't go on like this much longer\n");
					sleep(speed);
					}
					break;
				case 9:
					if(ccard < 61)
						tko[i] = YES;
					else {
			    printf("%s's cornermen summon the doctor\n",CUTNAM);
					sleep(speed);
			 printf("The doctor takes a close look at %s\n",CUTNAM);
					sleep(speed);
			printf("He says that %s looks pretty bad\n",CUTNAM);
					sleep(speed);
			printf("If %s gets any worse, he'll have to stop the fight\n",CUTNAM);
					sleep(speed);
					}
					break;
				case 10:
				default:
					tko[i] = YES;
				}
			}
		}
	}

	if((tko[0] == YES) && (tko[1] == YES))
	{
printf("Both fighters are cut so badly that neither can answer the bell for round %d\n",round+1);
		sleep(speed);
		printf("The officials declare the bout a draw\n");
		exit(0);
	}
	else if((tko[0] || tko[1]) == YES)
	{
		for(i=0; i < 2; i++)
		{
			if(tko[i] == YES)
			{
		printf("The referee asks the ring doctor to take a look at %s again\n",bx[rt[i].bxptr].lname);
		sleep(speed);
		printf("The doctor looks at %s and shakes his head\n",bx[rt[i].bxptr].lname);
		sleep(speed);
		printf("He advises the referee to stop the fight\n");
		sleep(speed);
		printf("%s scores a technical knockout over %s in %d rounds\n",bx[rt[flip(i)].bxptr].lname,bx[rt[i].bxptr].lname,round);
			exit(0);
			}
		}
	}
}



autotko()	/* check for an automatic tko */
{
	register int i;
	byte tko[2];
	register byte c;

	for(i=0; i < 2; i++)
	{
		tko[i] = NO;
		if(atko[BXI.tko-1] <= rt[i].cnpts)
			tko[i] = YES;
		else if(atko[BXI.tko+4] <= rt[i].cnpts + rt[i].catko1)
			tko[i] = YES;
	       else if(atko[BXI.tko+9] <= rt[i].cnpts+rt[i].catko1+rt[i].catko2)
			tko[i] = YES;
	}

	if((tko[0] == YES) && (tko[1] == YES))
	{
		sleep(speed);
printf("Neither fighter can answer the bell for round %d\n",round+1);
		sleep(speed);
		printf("The officials declare the bout a draw\n");
		exit(0);
	}
	else if((tko[0] || tko[1]) == YES)
	{
		sleep(speed);
		for(i=0; i < 2; i++)
		{
			if(tko[i] != YES)
			{
				c = rand() % 3;
				switch(c)
				{
				case 0:
		printf("%s's corner throws in the towel\n",BXI.lname);
					break;
				case 1:
		printf("The referee takes a good look at %s\n",BXI.lname);
					sleep(speed);
				       printf("He decides to stop the fight\n");
					break;
				case 2:
		printf("%s has sustained so much punishment that he cannot\n",BXI.lname);
			printf("answer the bell for round %d\n",round+1);
					break;
				}
		sleep(speed);
		printf("%s scores a technical knockout over %s in %d rounds\n",bx[rt[flip(i)].bxptr].lname,bx[rt[i].bxptr].lname,round);
			exit(0);
			}
		}
	}
	else for(i=0; i < 2; i++)
	{
		rt[i].catko2=rt[i].catko1;
		rt[i].catko1=rt[i].cnpts;
	}
}
