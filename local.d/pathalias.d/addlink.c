/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)addlink.c	7.1 (down!honey) 85/08/06";
#endif lint

#include "def.h"
link	*
addlink(from, to, cost, netchar, netdir)
node	*from;
register node	*to;
Cost	cost;
char	netchar;
char	netdir;
{
	register link	*l, *prev = 0;

#ifndef SQUANDER
	/* welcome to cycle city -- inner loop follows */

	/*
	 * link chains are sorted by host struct address, largest to smallest.
	 * thus, newly declared hosts are at the front of the list.
	 */
	for (l = from->n_link; l; l = l->l_next) {
		if (to >= l->l_to)
			break;
		prev = l;
	}
	/* you are now leaving cycle city -- hope you enjoyed your stay */

	if (l && (to == l->l_to)) {
		/*
		 * this is twisted by the awful gateway semantics.
		 *
		 * if "to" is a dead network, use the cheapest non-zero cost.
		 * ("from" is a gateway.)
		 *
		 * otherwise, use new cost if cheaper.
		 */
		if ((DEADNET(to) && l->l_cost == 0) || cost < l->l_cost) {
			l->l_cost = cost;
			netbits(l, netchar, netdir);
		}
		return(l);
	}

#endif !SQUANDER
	l = newlink();

	if (prev) {
		l->l_next = prev->l_next;
		prev->l_next = l;
	} else {
		l->l_next = from->n_link;
		from->n_link = l;
	}

	l->l_to = to;
	l->l_cost = cost;
	if (netchar == 0) {
		netchar = DEFNET;
		netdir = DEFDIR;
	}
	netbits(l, netchar, netdir);

	return(l);
}

deadlink(s) 
char	*s;
{
	char	*t, c;
	link	*l;

	for (t = s; !isnetc(*t); t++)
		if (*t == 0) 
			break;
	if ((c = *t) != 0) {
		*t = 0;
		l = addlink(addnode(s), addnode(t + 1), INF / 2, c, DEFDIR);
		l->l_flag |= LDEAD;
	} else 
		addnode(s)->n_flag |= NDEAD;
}

netbits(l, netchar, netdir)
link	*l;
char	netchar, netdir;
{
	int	isadomain = 0;
	char	*nptr;

	if (netchar & 0200) {
		netchar &= 0177;
		isadomain = LDOMAIN;
	}
	if ((nptr = index(Netchars, netchar)) == 0) {
		fprintf(stderr, "%s: unknown network operator: %c\n",
								ProgName, netchar);
		badmagic(1);
	}
	l->l_flag &= ~(LNETCHARS|LDIR|LDOMAIN);
	l->l_flag |= (nptr - Netchars) | dirbits(netdir) | isadomain;
}
