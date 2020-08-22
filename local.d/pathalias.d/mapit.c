/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)mapit.c	7.1 (down!honey) 85/08/06";
#endif /*lint*/

#include "def.h"

STATIC void reheap(), insert(), setpath(), swap();
STATIC node *min_node();
STATIC Cost setcost();

STATIC int	Nheap;

mapit()
{
	node *n, *next;
	link *l;
	Cost	cost;
	char	*sbrk();

	vprintf(stderr, "%d vertices, %d edges\n", Ncount, Lcount);
	vprintf(stderr, "break is %ld after parsing\n", (long) sbrk(0));
	
	/* use the hash Table for the heap */
	/* TODO: coalesce the following functions into a single one */
	pack();		/* remove holes in the Table */
	amerge();	/* merge all alias links once and for all */
	if (Linkout && *Linkout)	/* dump cheapest links */
		showlinks();
	if (Graphout && *Graphout)	/* dump the edge list */
		dumpgraph();

	while (Home->n_alias)
		Home = Home->n_alias;
	dehash(Home);
	Home->n_path = strsave("%s");
	insert(Home);

	/*
	 * main mapping loop.
	 * assertion: no alias can ever appear in the heap.  'struth.
	 */
	while ((n = min_node()) != 0) {

		printit(n);

		/*
		 * if reached by a gatewayed net, discourage further links.
		 * this has some relevance to common carriers and the FCC ...
		 */
		if (n->n_flag & GATEWAYIN)
			n->n_cost += 2* DEFCOST;

		/* add children to heap */
		for (l = n->n_link; l != 0; l = l->l_next) {
			next = l->l_to;
			while (next->n_alias)
				next = next->n_alias;
			if (next->n_flag & MAPPED)
				continue;
			dehash(next);

			cost = setcost(n, l, next);

			if (next->n_cost == 0) {		/* first time */
				next->n_cost = cost;
				insert(next);
			} else if (cost < next->n_cost) {	/* cheaper route */
				next->n_cost = cost;
				reheap(next);
			} else					/* lose lose */
				continue;

			/* note whether we got here via a gatewayed net */
			if (DEADNET(n))
				next->n_flag |= GATEWAYIN;
			else
				next->n_flag &= ~GATEWAYIN;

			setpath(n, next, l);

			free((char *) l);	
		}

		/* done with this node forever -- free as much as possible */
		free((char *) n->n_path);	/* absolutely free ... */
		free((char *) n->n_name);

		/* expunge aliases */
		for (next = n->n_aliaslink; next; next = next->n_aliaslink) {
			dehash(next);
			Table[next->n_tloc] = 0;
			next->n_tloc = 0;
			free((char *) next->n_name);
		}
	}
	vprintf(stderr, "break is %ld at after mapping\n", (long) sbrk(0));

	if (Nheap != 0) {
		fprintf(stderr, "%s: null entry found in heap\n", ProgName);
		badmagic(1);
	}

	if (Hashpart < Tabsize) {
		fprintf(stderr, "You can't get there from here:\n");
		while (Hashpart < Tabsize) {
			n = Table[Hashpart++];
			if (n->n_alias)
				continue;	/* primary hosts only */
			fprintf(stderr, "\t%s", n->n_name);
			if (n->n_aliaslink) {
				fprintf(stderr, " (alias %s", n->n_aliaslink->n_name);
				n = n->n_aliaslink;
				while (n = n->n_aliaslink)
					fprintf(stderr,  ", %s", n->n_name);
				putc(')', stderr);
			}
			putc('\n', stderr);
		}
	}
}

STATIC Cost
setcost(n, l, next)
register node	*n;
register link	*l;
node	*next;
{
	register Cost	cost;

	cost = n->n_cost + l->l_cost;	/* fundamental cost */

	/*
	 * heuristics:
	 *    charge for getting out of a dead host.
	 *    charge for getting in to a dead net
	 *         unless the link cost is non-zero (gateway).
	 *    charge for a dead link.
	 *    discourage mixing of left and right syntax.
	 */
	if ((n->n_flag & (NNET | NDEAD)) == NDEAD)
		cost += INF/2;	/* dead host */
	if (DEADNET(next) && l->l_cost == 0)
		cost += INF/2;	/* dead net, not gateway */
	if (l->l_flag & LDEAD)
		cost += INF/2;	/* dead link */
	if ((n->n_flag & HASLEFT) && (NETDIR(l) == LRIGHT))
		cost += DEFCOST;	/* mix */
	if ((n->n_flag & HASRIGHT) && (NETDIR(l) == LLEFT))
		cost += DEFCOST;	/* mix */

	return(cost);
}

/*
 * heap implementation of priority queue.
 */

STATIC void
insert(n)
node	*n;
{
	int	i, parent;

	Table[n->n_tloc] = 0;
	if (Table[Nheap+1] != 0) {
		fprintf(stderr, "%s: heap error in insert\n", ProgName);
		badmagic(1);
	}
	if (Nheap++ == 0) {
		Table[1] = n;
		n->n_tloc = 1;
		return;
	}

	/* insert at the end and percolate up */
	Table[Nheap] = n;
	n->n_tloc = Nheap;
	for (i = Nheap; i > 1; i = parent) {
		if (Table[i] == 0) {
			fprintf(stderr, "%s: heap error in insert\n", ProgName);
			badmagic(1);
		}
		parent = i / 2;
		if (Table[i]->n_cost < Table[parent]->n_cost)
			swap(i, parent);
	}
	return;
}

STATIC node	*
min_node()
{
	node	*rval;
	int	i;

	if (Nheap == 0)
		return(0);

	rval = Table[1];	/* return this one */
			
	/* move last entry into root, percolate down */
	Table[1] = Table[Nheap];
	Table[1]->n_tloc = 1;
	Table[Nheap] = 0;
	if (--Nheap == 0)
		return(rval);

	i = 1;
	for (;;) {
		/* swap with smaller child  (if larger than same) */
		int	child;

		child = i * 2;
		if (child > Nheap)
			break;
		if (child < Nheap) 	/* right child exists? */
			if (Table[child]->n_cost > Table[child+1]->n_cost)
				child++;
			
		if (Table[i]->n_cost > Table[child]->n_cost)
			swap(i, child);
		i = child;
	}

	return(rval);
}

STATIC void
swap(i, j)
{
	node	*temp;

	temp = Table[i];
	Table[i] = Table[j];
	Table[j] = temp;
	Table[j]->n_tloc = j;
	Table[i]->n_tloc = i;
}

/* "percolate" node n up the heap by exchanging with the parent */
STATIC void
reheap(n)
node	*n;
{
	int	loc, parent;
	Cost	cost;

	cost = n->n_cost;
	for (loc = n->n_tloc; loc != 1; loc = parent) {
		parent = loc / 2;
		if (cost >= Table[parent]->n_cost)
			return;
		swap(loc, parent);
	}
}

STATIC void
setpath(prev, next, l) 
node	*prev, *next;
link	*l;
{
	char	pathbuf[BUFSIZ], hostbuf[BUFSIZ], *hptr;
	char	netchar, netdir;

	netchar = NETCHAR(l);
	netdir = NETDIR(l);
	/* undo settings from earlier calls */
	if (next->n_path)
		free((char *) next->n_path);	/* absolutely free ... */

	if (prev->n_flag & ATSIGN)
		next->n_flag |= ATSIGN;
	else
		next->n_flag &= ~ATSIGN;

	if (prev->n_flag & HASLEFT)
		next->n_flag |= HASLEFT;
	else
		next->n_flag &= ~HASLEFT;

	if (prev->n_flag & HASRIGHT)
		next->n_flag |= HASRIGHT;
	else
		next->n_flag &= ~HASRIGHT;

	if (next->n_flag & NNET) {
		/*
		 * grumble. when climbing onto a "domain" style network,
		 * append .netname.  but we can't do it 'til later ...
		 *
		 * unless, of course, we are in transit from another
		 * domain style network, in which case we tack the
		 * predecessor's name onto the next domain.
		 *
		 * e.g., prev = arpa, next = csnet.  change next->n_name
		 * to csnet.arpa.  but first wipe out any previous
		 * domain on next.  this is too gross for words.
		 */
		if (l->l_flag & LDOMAIN) {
			next->n_flag |= NDOMAIN;
			if (prev->n_flag & NDOMAIN) {
				/*
				 * clean out dots in next->n_name -- they're
				 * no longer valid.  N.B.: we are guaranteed
				 * that next is not an alias
				 */
				hptr = index(next->n_name, '.');
				if (hptr)
					*hptr = 0;
				sprintf(hostbuf, "%s.%s", next->n_name, prev->n_name);
				free(next->n_name);
				next->n_name = strsave(hostbuf);
			}
		} else
			next->n_flag &= ~NDOMAIN;
		next->n_path = strsave(prev->n_path);
		return;
	}

	/* do it by hand instead of sprintf-ing -- foo on '%' */
	if (netdir == LLEFT) {
		/* e.g., host!%s */
		next->n_flag |= HASLEFT;
		strcpy(hostbuf, next->n_name);
		hptr = hostbuf + strlen(hostbuf);
		if (prev->n_flag & NDOMAIN) {
			*hptr++ = '.';
			strcpy(hptr, prev->n_name);
			hptr += strlen(hptr);
		}
		*hptr++ = netchar;
		if (netchar == '%')
			*hptr++ = netchar;
		*hptr++ = '%';
		*hptr++ = 's';
		*hptr = 0;
	} else {
		/* e.g., %s@host, but watch out for the magic @-% conversion */
		next->n_flag |= HASRIGHT;
		if (netchar == '@') {
			next->n_flag |= ATSIGN;
			if (prev->n_flag & ATSIGN)
				netchar = '%';	/* shazam?  shaman? */
		}
		hptr = hostbuf;
		*hptr++ = '%';
		*hptr++ = 's';
		*hptr++ = netchar;
		if (netchar == '%')
			*hptr++ = '%';
		strcpy(hptr, next->n_name);
		if (prev->n_flag & NDOMAIN) {
			hptr += strlen(hptr);
			*hptr++ = '.';
			strcpy(hptr, prev->n_name);
		}
	}
	/* this would be an sprintf were it not for the %'s.  feh. */
	pathprintf(pathbuf, prev->n_path, hostbuf);
	next->n_path = strsave(pathbuf);
}

dehash(n)
node	*n;
{
	if (n->n_flag & DEHASH)
		return;
	Table[Hashpart]->n_tloc = n->n_tloc;
	Table[n->n_tloc] = Table[Hashpart];
	Table[Hashpart] = n;
	n->n_flag |= DEHASH;
	n->n_tloc = Hashpart++;
}

pathprintf(buf, path, host)
char	*buf, *path, *host;
{
	for ( ; *buf = *path; path++) {
		if (*path == '%') {
			switch(path[1]) {
			case 's':
				strcpy(buf, host);
				buf += strlen(buf);
				path++;
				break;
			case '%':
				*++buf = *++path;
				buf++;
				break;
			}
		} else
			buf++;
	}
}

printit(n)
node	*n;
{
	char	*path = n->n_path;
	Cost	cost = n->n_cost;

	for ( ; n; n = n->n_aliaslink) {
		n->n_flag |= MAPPED;
		if (n->n_flag & (NNET | ISPRIVATE | COLLISION))
			continue;
		if (Cflag)
			printf("%ld\t", (long) cost);
		printf("%s\t%s\n", n->n_name, path);
	}
}

