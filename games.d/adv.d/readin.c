/*
 * readin()
 * Reads the source file & allocates initial values & such.
 */

#include "adv.h"
#include <ctype.h>

char *b2, *bs, *savb2();

readin()
{
	register struct room *rp;
	register struct word *p;
	register struct item *ip;
	char	*s;			/* String ptr */
	char	c;
	int	i, j;			/* Temporaries */
	int	dflag = 0,		/* Directions flag */
		hflag = 0;		/* Hellroom flag */

	wrdsiz = 3;
	s = rdr();

	/*  Allow #! construct as first line - Chris Miller, August 1985 */
	if (*s == '#' && s[1] == '!') {
	    s = rdr();
	} /*  End of #! modification */

	/*
	 * Init specials
	 */
	while (*s == '!') {
		++s;
		switch (toupper(*s)) {
		case 'D':
			for (dflag = 0; dflag < 6; dflag++) {
				if (*s)
					++s;
				dirs[dflag] = nxtwrd(&s, 255);
			}
		when 'W':
			wrdsiz = atoi(++s);
		when 'L':
			lititm = atoi(++s);
		when 'H':
			hellrm = atoi(++s);
			++hflag;
		}
		s = rdr();
	}

	/*
	 * Init initials
	 */
	i_rm = atoi(s);
	trsrm = atoi(rdr());
	tottrs = atoi(rdr());
	maxinv = atoi(rdr());
	litmax = atoi(rdr());
	warnpt = atoi(rdr());

	/*
	 * Init rooms
	 */
	nrooms = atoi(rdr());
	rooms = (struct room *)alcz(nrooms * sizeof (struct room));
	roome = &rooms[nrooms];
	for (rp = rooms; rp < roome; rp++) {
		s = rdr();
		while (*s) {
			c = islower (*s) ? toupper (*s) : *s;
			s++;
			switch (c) {
			case 'N':
				rp->dirs[0] = atoi(s);
			when 'S':
				rp->dirs[1] = atoi(s);
			when 'E':
				rp->dirs[2] = atoi(s);
			when 'W':
				rp->dirs[3] = atoi(s);
			when 'U':
				rp->dirs[4] = atoi(s);
			when 'D':
				rp->dirs[5] = atoi(s);
			when '%':
				rp->dark = TRUE;
			when ':':
				skpwht(s);
				rp->desc = strsav(s);
				goto r_nxt;
			}
		}
		Wcleanup();
		printf("Bad room %d -- no description!\n", rp - rooms);
		exit(100);
r_nxt:	;
	}

	/*
	 * Init verbs, objects
	 */
	nverbs = atoi(rdr());
	verbs = (struct word *)alcnz(nverbs * sizeof (struct word));
	verbe = &verbs[nverbs];
	s = "";
	j = -1;
	for (p = verbs; p < verbe; p++) {
		if (*s == 0)
			s = rdr();
		if (*s == '*') {
			s++;
			p->val = j;
		}
		else
			p->val = ++j;
		p->word = nxtwrd(&s, wrdsiz);
		if (*s)
			s++;
		skpwht(s);
	}
	nobjs = atoi(rdr());
	objs = (struct word *)alcnz(nobjs * sizeof (struct word));
	obje = &objs[nobjs];
	s = "";
	j = -1;
	for (p = objs; p < obje; p++) {
		if (*s == 0)
			s = rdr();
		if (*s == '*') {
			s++;
			p->val = j;
		}
		else
			p->val = ++j;
		p->word = nxtwrd(&s, wrdsiz);
		if (*s)
			s++;
		skpwht(s);
	}

	/*
	 * Init items
	 */
	nitems = atoi(rdr());
	items = (struct item *)alcnz(nitems * sizeof (struct item));
	iteme = &items[nitems];
	for (ip = items; ip < iteme; ip++) {
		s = rdr();
		ip->i_rm = atoi(s);
		while (*s && *s != '=' && *s != ':')
			s++;
		if (*s == 0)
			goto baditm;
		if (*s == '=') {
			s++;
			ip->name = nxtwrd(&s, wrdsiz);
			skpwht(s);
		}
		else
			ip->name = "";
		if (*s != ':')
			goto baditm;
		s++;
		skpwht(s);
		ip->desc = strsav(s);
		continue;
baditm:
		Wcleanup();
		printf("Bad item %d -- no description!\n", ip - items);
		exit(100);
	}

	/*
	 * Init msgs
	 */
	nmsgs = atoi(rdr());
	msg = (char **)alcnz(nmsgs * sizeof (char *));
	for (i = 0; i < nmsgs; ++i)
		msg[i] = strsav(rdr());

	/*
	 * Init %s, actions
	 */
	npcts = atoi(rdr());
	pct = (char **)alcz(npcts * sizeof (char *));
	bs = "";
	for (i = 0; i < npcts; ++i) {
		b2 = buf2;
		while (rdbyte());
		while (rdbyte());
		pct[i] = savb2();
	}
	nactns = atoi(rdr());
	actn = (char **)alcz(nactns * sizeof (char *));
	bs = "";
	for (i = 0; i < nactns; i++) {
		b2 = buf2;
		while (rdbyte());
		while (rdbyte());
		actn[i] = savb2();
	}
	fclose(file);
	if (!hflag)
		hellrm = nrooms-1;
	if (!dflag) {
		dirs[0] = "North";
		dirs[1] = "South";
		dirs[2] = "East";
		dirs[3] = "West";
		dirs[4] = "Up";
		dirs[5] = "Down";
	}
	toomch = "I've too much to carry.  Try -TAKE INVENTORY-\n";
	s = "QUIT SAVE LOAD GAME";
	qcom = nxtwrd(&s, wrdsiz);
	s++;
	scom = nxtwrd(&s, wrdsiz);
	s++;
	lcom = nxtwrd(&s, wrdsiz);
	s++;
	gobj = nxtwrd(&s, wrdsiz);
}

rdbyte()
{

	if (*bs == 0) {
		bs = rdr();
		skpwht(bs);
	}
	*b2 = atoi(bs);
	while (*bs >= '0' && *bs <= '9')
		++bs;
	if (*bs == ',')
		++bs;
	skpwht(bs);
	return (*b2++ & 255);
}

#define bcopy(fr, to, ln) {char *f=fr, *t=to;int l=ln;while (l--) *t++ = *f++;}

char *
savb2()
{
	register int c = b2 - buf2;
	char *p;

	p = alcnz(c);
	bcopy(buf2, p, c);	/* copy from buf2 to p */
#ifdef DEBUG
	printf("Allocated %d bytes for action\n", c);
#endif
	return (p);
}
