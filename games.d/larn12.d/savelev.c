/* savelev.c		 Larn is copyrighted 1986 by Noah Morgan. */
#include "header.h"

/*
 *	routine to save the present level into storage
 */
savelevel()
	{
	register struct cel *pcel;
	register char *pitem,*pknow,*pmitem;
	register short *phitp,*piarg;
	register struct cel *pecel;
#ifdef OLD
	pcel = &cell[level][0];	/* pointer to this level's cells */
#else
	pcel = cell;		/* pointer to this level's cells */
#endif
	pecel = pcel+MAXX*MAXY;	/* pointer to past end of this level's cells */
	pitem=item[0]; piarg=iarg[0]; pknow=know[0]; pmitem=mitem[0]; phitp=hitp[0];
	while (pcel < pecel)
		{
		pcel->mitem  = *pmitem++;
		pcel->hitp   = *phitp++;
		pcel->item   = *pitem++;
		pcel->know   = *pknow++;
		pcel++->iarg = *piarg++;
		}
#ifndef OLD
	lseek(dung, (long)level*sizeof(cell), 0);
	write(dung, (char *)cell, sizeof(cell));
#endif
	}

/*
 *	routine to restore a level from storage
 */
getlevel()
	{
	register struct cel *pcel;
	register char *pitem,*pknow,*pmitem;
	register short *phitp,*piarg;
	register struct cel *pecel;
#ifdef OLD
	pcel = &cell[level][0];	/* pointer to this level's cells */
#else
	lseek(dung, (long)level*sizeof(cell), 0);
	read(dung, (char *)cell, sizeof(cell));
	pcel = cell;	/* pointer to this level's cells */
#endif
	pecel = pcel+MAXX*MAXY;	/* pointer to past end of this level's cells */
	pitem=item[0]; piarg=iarg[0]; pknow=know[0]; pmitem=mitem[0]; phitp=hitp[0];
	while (pcel < pecel)
		{
		*pmitem++ = pcel->mitem;
		*phitp++ = pcel->hitp;
		*pitem++ = pcel->item;
		*pknow++ = pcel->know;
		*piarg++ = pcel++->iarg;
		}
	}
