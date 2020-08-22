/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)mapaux.c	7.1 (down!honey) 85/08/06";
#endif /*lint*/

#include "def.h"

void	pack();

void
pack()
{
	int	hole, next;

	/* find first "hole " */
	for (hole = Tabsize - 1; hole >= 0 && Table[hole] != 0; --hole)
		;

	/* repeatedly move next filled entry into last hole */
	for (next = hole - 1; next >= 0; --next) {
		if (Table[next] != 0) {
			Table[hole] = Table[next];
			Table[hole]->n_tloc = hole;
			Table[next] = 0;
			while (Table[--hole] != 0)	/* find next hole */
				;
		}
	}
	Hashpart = hole + 1;
}

amerge()
{
	node	*n, *a;
	int	i;

	for (i = Hashpart; i < Tabsize; i++) {
		n = Table[i];
		if (n == 0)	/* impossible, but ... */
			continue;
		for (a = n->n_alias; a; a = a->n_alias) {
			a->n_link = lmerge(a->n_link, n->n_link);
			n->n_link = 0;
			n = a;
		}
	}
}

STATIC FILE	*Gstream;

dumpgraph()
{
	int	i;
	node	*n;

	if ((Gstream = fopen(Graphout, "w")) == NULL) {
		fprintf(stderr, "%s: ", ProgName);
		perror(Graphout);
	}

	untangle();	/* untangle net cycles for proper output */

	for (i = Hashpart; i < Tabsize; i++) {
		n = Table[i];
		if (n == 0)
			continue;	/* impossible, but ... */
		if (n->n_alias)
			continue;	/* primaries only (wrong?) */
		/* a minor optimization ... */
		if (n->n_link == 0)
			continue;
		/* pathparse doesn't need these */
		if (n->n_flag & NNET)
			continue;
		dumpnode(n);
	}
}

dumpnode(from)
node	*from;
{
	node	*to;
	link	*l;
	char	netbuf[BUFSIZ], *nptr = netbuf;

	for (l = from->n_link ; l; l = l->l_next) {
		to = l->l_to;
		while (to->n_alias)
			to = to->n_alias;	/* get to primary */
		if (from == to)
			continue;	/* oops -- it's me! */

		if ((to->n_flag & NNET) == 0) {
			/* host -> host -- print host>host */
			if (l->l_cost == INF)
				continue;	/* phoney link */
			fputs(from->n_name, Gstream);
			putc('>', Gstream);
			fputs(to->n_name, Gstream);
			putc('\n', Gstream);
		} else {
			/* host -> net -- just cache it for now */
			while (to->n_root && to != to->n_root)
				to = to->n_root;
			*nptr++ = ',';
			strcpy(nptr, to->n_name);
			nptr += strlen(nptr);
		}
	}

	/* dump nets */
	if (nptr != netbuf) {
		/* nets -- print host@\tnet,net, ... */
		*nptr = 0;
		fputs(from->n_name, Gstream);
		putc('@', Gstream);
		*netbuf = '\t';	/* insert tab by killing initial ',' */
		fputs(netbuf, Gstream);	/* skip initial ',' */
		putc('\n', Gstream);
	}
}

/*
 * remove cycles in net definitions. 
 *
 * depth-first search
 *
 * for each net, run dfs on its neighbors (nets only).  if we return to
 * a visited node, that's a net cycle.  mark the cycle with a pointer
 * in the n_root field (which gets us closer to the root of this
 * portion of the dfs tree).
 */
untangle()
{
	int	i;
	node	*n;

	for (i = Hashpart; i < Tabsize; i++) {
		n = Table[i];
		if (n == 0 || (n->n_flag & NNET) == 0 || n->n_root)
			continue;
		dfs(n);
	}
}

dfs(n)
node	*n;
{
	link	*l;
	node	*next;

	n->n_flag |= INDFS;
	n->n_root = n;
	for (l = n->n_link; l; l = l->l_next) {
		next = l->l_to;
		if ((next->n_flag & NNET) == 0)
			continue;
		if ((next->n_flag & INDFS) == 0) {
			dfs(next);
			if (next->n_root != next)
				n->n_root = next->n_root;
		} else
			n->n_root = next->n_root;
	}
	n->n_flag &= ~INDFS;
}

showlinks() 
{
	link	*l;
	node	*n;
	int	i;
	FILE	*estream;

	if ((estream = fopen(Linkout, "w")) == 0)
		return;

	for (i = Hashpart; i < Tabsize; i++) {
		n = Table[i];
		if (n == 0)	/* impossible, but ... */
			continue;
		if (l = n->n_link) {
			fprintf(estream, "%s\t%s(%d)", n->n_name,
				l->l_to->n_name,
				l->l_cost ? l->l_cost : DEFCOST);
			for (l = l->l_next; l; l = l->l_next)
				fprintf(estream, ",\n\t%s(%d)", l->l_to->n_name,
					l->l_cost ? l->l_cost : DEFCOST);
			fputc('\n', estream);
		}
	}
	(void) fclose(estream);
}

