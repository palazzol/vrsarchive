/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)addnode.c	7.1 (down!honey) 85/08/07";
#endif lint

#include "def.h"

void	lowercase();
node	*isprivate();

/*
 * these numbers are chosen because:
 *	-> they are prime,
 *	-> they are monotonic increasing,
 *	-> each is a tad smaller than a multiple of 1024.
 * the first point yields good hash functions, the second is used for the
 * standard re-hashing implementation of open addressing, and the third
 * optimizes for quirks in some mallocs i have seen.
 */
STATIC int Primes[]	= {
	1021, 2039, 3067, 4093, 5113, 6133, 7159, 8179, 9209,
	10223, 11261, 12281, 13309, 14327, 15349, 16381, 17401,
	18427, 19447, 20477, 21499, 22511, 23549, 24571, 25589, 0
};
STATIC int	Tabindex = -1;
STATIC int	Collision;	/* mark host name collisions in hash() */

int	Tabsize;	/* used later for the priority queue */
node	**Table;	/* ditto */

node	*
addnode(name)
register char	*name;
{
	register int	i;
	register node	*n;

	if (Iflag)
		lowercase(name);

	/* is it a private host? */
	n = isprivate(name);
	if (n != 0) {
		while (n->n_alias)
			n = n->n_alias;
		return(n);
	}

	i = hash(name, 0);
	if (Table[i] != 0) {
		n = Table[i];
		while (n->n_alias)
			n = n->n_alias;
		return(n);
	}

	n = newnode();
	n->n_name = strsave(name);
	Table[i] = n;
	n->n_tloc = i;	/* essentially a back link to the table */
	if (Collision)
		n->n_flag |= COLLISION;	/* name collision with private host */

	return(n);
}

alias(parent, child) 
register node	*parent;	/* real node */
register node	*child;		/* alias node */
{
	register node	*root;		/* root of this alias tree */

	if (parent == child) {
		char	buf[BUFSIZ];

		sprintf(buf, "warning: alias redeclaration: %s = %s",
			parent->n_name, parent->n_name);
		yyerror(buf);
		return;		/* caused by redeclaration of alias */
	}

	/* avoid alias loops, force many-to-one */
	/* can't happen -- wish it could ... */
	if (parent->n_alias || child->n_alias) {
		yyerror("can't nest aliases");
		return;
	}

	/* merge links from parent(s) to root, point parent at root */
	for (root = parent->n_alias; root; root = root->n_alias) {
		root->n_link = lmerge(root->n_link, parent->n_link);
		parent->n_link = 0;
		parent = root;
	}

	/* merge child links into parent (now root) */
	parent->n_link = lmerge(parent->n_link, child->n_link);
	child->n_link = 0;

	/* set up the alias pointers */
	child->n_alias = parent;
	child->n_aliaslink = parent->n_aliaslink;
	parent->n_aliaslink = child;
}

/* double hashing */
#define HASH1(n)	((n) % Tabsize);
#define HASH2(n)	(((n) % (Tabsize - 2)) + 1)

/*
 * at 75% full, probes per access is about 2.
 */
#define HIGHWATER	75
#define LOWWATER	50
#define isfull(n, size)	((n) > (((size) * HIGHWATER) / 100))
#define isempty(n, size)	((n) < (((size) * LOWWATER) / 100))

STATIC int
hash(name, unique)
char	*name;
{
	register int	probe, hash2;
	register node	*n;

	if (Tabindex < 0) {			/* first time */
		Tabindex = 0;
		Tabsize = Primes[0];
		Table = newtable(Tabsize);
	}

	if (isfull(Ncount, Tabsize))
		rehash();			/* more, more! */
				
	probe = fold(name);
	/* don't change the order of the next two lines */
	hash2 = HASH2(probe);
	probe = HASH1(probe);
	/* thank you! */

	/*
	 * probe the hash table.
	 * if unique is set, we require a fresh slot.
	 * otherwise, use double hashing to find either
	 *  (1) an empty slot, or
	 *  (2) a non-private copy of this host name
	 *
	 * as a side effect, if we notice a collision with a private host
	 * we mark the other so that it is skipped at output time.
	 */
	Collision = 0;
	while ((n = Table[probe]) != 0) {
		if (strcmp(n->n_name, name) == 0) {
			if (unique)
				n->n_flag |= COLLISION;
			else if (n->n_flag & ISPRIVATE)
				Collision++;
			else
				break;	/* this is it! */
		}
		probe -= hash2;
		if (probe < 0)
			probe += Tabsize;
	}
	return(probe);
}

STATIC int
rehash()
{
	register node	**otable, **optr;
	register int	probe;
	int	osize;

#ifdef DEBUG
	hashanalyze();
#endif DEBUG
	optr = Table + Tabsize - 1;	/* ptr to last */
	otable = Table;
	osize = Tabsize;

	do {
		Tabsize = Primes[++Tabindex];
		if (Tabsize == 0) {
			fprintf(stderr, "%s: > %d hosts\n", ProgName,
							Primes[Tabindex-1]);
			badmagic(1);
		}
	} while (!isempty(Ncount, Tabsize));
	vprintf(stderr, "rehash into %d\n", Tabsize);
	Table = newtable(Tabsize);

	do {
		if (*optr == 0)
			continue;
		probe = hash((*optr)->n_name, (*optr)->n_flag & ISPRIVATE);
		if (Table[probe] != 0) {
			fprintf(stderr, "%s: rehash error\n", ProgName);
			badmagic(1);
		}
		Table[probe] = *optr;
		(*optr)->n_tloc = probe;
	} while (optr-- > otable);
	freetable(otable, osize);
}


STATIC
fold(s)
register char	*s;
{
	register int	sum = 0;

	while (*s) {
		sum <<= 2;
		sum += *s++;
	}
	if (sum < 0) 
		sum = -sum;
	return(sum);
}

/* merge the l2 list into the l1 list */
link	*
lmerge(l1, l2)
register link	*l1, *l2;
{
	register link	*ltmp;
	link	*rval;

	if (l1 == 0)
		return(l2);

	if (l2 == 0)
		return(l1);

	if (l1->l_to > l2->l_to) {
		ltmp = rval = l1;
		l1 = l1->l_next;
	} else if (l1->l_to < l2->l_to) {
		ltmp = rval = l2;
		l2 = l2->l_next;
	} else if (l1->l_cost <= l2->l_cost) {
		ltmp = rval = l1;
		l1 = l1->l_next;
		free((char *) l2);
		l2 = l2->l_next;
	} else {
		ltmp = rval = l2;
		l2 = l2->l_next;
		free((char *) l1);
		l1 = l1->l_next;
	}

	while (l1 && l2) {
		if (l1->l_to > l2->l_to) {
			ltmp->l_next = l1;
			ltmp = l1;
			l1 = l1->l_next;
		} else if (l1->l_to < l2->l_to) {
			ltmp->l_next = l2;
			ltmp = l2;
			l2 = l2->l_next;
		} else if (l1->l_cost <= l2->l_cost) {	/* use cheaper link */
			ltmp->l_next = l1;
			ltmp = l1;
			l1 = l1->l_next;
			free((char *) l2);
			l2 = l2->l_next;
		} else {
			ltmp->l_next = l2;
			ltmp = l2;
			l2 = l2->l_next;
			free((char *) l1);
			l1 = l1->l_next;
		}
	}
	if (l1)
		ltmp->l_next = l1;
	else
		ltmp->l_next = l2;

	return(rval);
}

hashanalyze()
{
	int	probe, hash2, count, i, collision[5];
	int	longest = 0, total = 0, slots = 0;
	int	nslots = sizeof(collision)/sizeof(collision[0]);

	if (!Vflag)
		return;

	strclear((char *) collision, sizeof(collision));
	for (i = 0; i < Tabsize; i++) {
		if (Table[i] == 0)
			continue;
		/* private hosts too hard to account for ... */
		if (Table[i]->n_flag & ISPRIVATE)
			continue;
		count = 1;
		probe = fold(Table[i]->n_name);
		/* don't change the order of the next two lines */
		hash2 = HASH2(probe);
		probe = HASH1(probe);
		/* thank you! */
		while (Table[probe] != 0
		    && strcmp(Table[probe]->n_name, Table[i]->n_name) != 0) {
			count++;
			probe -= hash2;
			if (probe < 0)
				probe += Tabsize;
		}
		if (Table[probe] == 0) {
			fprintf(stderr, "%s: impossible hash error for %s\n",
					ProgName, Table[i]->n_name);
			continue;
		}
		
		total += count;
		slots++;
		if (count > longest)
			longest = count;
		if (count >= nslots)
			count = 0;
		collision[count]++;
	}
	for (i = 1; i < nslots; i++)
		if (collision[i])
			fprintf(stderr, "%d chains: %d (%d%%)\n",
				i, collision[i], (collision[i] * 100)/ slots);
		if (collision[0])
			fprintf(stderr, "> %d chains: %d (%d%%)\n",
				nslots - 1, collision[0],
				(collision[0] * 100)/ slots);
	fprintf(stderr, "%2.2f probes per access, longest chain: %d\n",
		(double) total / slots, longest);
}

STATIC void
lowercase(s)
register char	*s;
{
	do {
		*s = isupper(*s) ? tolower(*s) : *s;
	} while (*s++);
}

STATIC node	*
isprivate(name)
register char	*name;
{
	register node	*n;

	for (n = Private; n != 0; n = n->n_private)
		if (strcmp(name, n->n_name) == 0)
			return(n);
	return(0);
}

fixprivate()
{
	register node	*n, *next;
	register int	i;

	for (n = Private; n != 0; n = next) {
		n->n_flag |= ISPRIVATE;		/* overkill, but safe */
		i = hash(n->n_name, 1);
		if (Table[i] != 0) {
			fprintf(stderr, "%s: impossible private node error on %s\n",
				ProgName, n->n_name);
			badmagic(1);
		}
	
		Table[i] = n;
		n->n_tloc = i;	/* essentially a back link to the table */
		next = n->n_private;
		n->n_private = 0;	/* clear for later use */
	}
	Private = 0;
}

node	*
addprivate(name)
register char	*name;
{
	register node	*n;

	if (Iflag)
		lowercase(name);
	n = isprivate(name);
	if (n)
		return(n);	/* this isn't even worth a warning */

	n = newnode();
	n->n_name = strsave(name);
	n->n_private = Private;
	Private = n;
	return(n);
}
