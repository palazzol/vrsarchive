/*
 * ngmatch - newsgroup name matching
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"

#define truth(bool) ((bool)? "yes": "no")

#ifndef STATIC
#define STATIC /* static */
#endif

#define ALL "all"			/* word wildcard */

static int debug = NO;

matchdebug(state)
int state;
{
	debug = state;
}

/*
 * ngmatch returns true iff the newsgroup(s) in ngs match
 * the pattern(s) in ngpat, where
 *
 * 	ngpats: { ngpat { "," ngpat }* }?
 *	ngpat: "!"? word { "." word }*
 *	word: { alphanum }+ | "all"
 *
 * Only one group need match for success.  (Redundant?)
 *
 * For each group, note the depth of each match against the patterns,
 * negated or not.  Ignore mismatches.  The deepest match wins at the end;
 * if it's a tie, negated matches are rejections.
 *
 * A match of any group against the patterns is a success.
 * Failure to match any pattern with a group is a mismatch of that group.
 * Failure to match any group against any pattern is a total failure.
 *
 * "all" in a pattern is a wildcard that matches exactly one word;
 * it does not cross "." (NGDELIM) delimiters.
 */

int
ngmatch(ngpat, ngs)
char *ngpat, *ngs;
{
	register char *ngp;			/* point at current group */
	register char *ngcomma;
	register char *rngpat = ngpat;

	if (debug)
		(void) fprintf(stderr, "ngmatch(`%s', `%s')\n", rngpat, ngs);
	for (ngp = ngs; ngp != NULL; ngp = ngcomma) {
		register int match;

		INDEX(ngp, NGSEP, ngcomma);
		if (ngcomma != NULL)
			*ngcomma = '\0';	/* will be restored below */
		match = mpatsmatch(rngpat, ngp);	/* try 1 group, multi-patterns */
		if (ngcomma != NULL)
			*ngcomma++ = NGSEP;	/* point after the comma */
		if (match)
			return YES;
	}
	return NO;				/* no pattern matched any group */
}

/*
 * Match one group against multiple patterns, as above.
 */
STATIC int
mpatsmatch(ngpat, grp)
char *ngpat, *grp;
{
	register char *patp;		/* point at current pattern */
	register char *patcomma;
	register int depth;
	register int faildeepest = 0, hitdeepest = 0;	/* in case of no match */
	register int negation;

	if (debug)
		(void) fprintf(stderr, "mpatsmatch(`%s', `%s')\n", ngpat, grp);
	for (patp = ngpat; patp != NULL; patp = patcomma) {
		negation = 0;
		INDEX(patp, NGSEP, patcomma);
		if (patcomma != NULL)
			*patcomma = '\0';	/* will be restored below */
		if (*patp == NGNEG) {
			++patp;
			++negation;
		}
		depth = onepatmatch(patp, grp);	/* try 1 pattern, 1 group */
		if (patcomma != NULL)
			*patcomma++ = NGSEP;	/* point after the comma */
		if (depth == 0)			/* mis-match */
			;			/* ignore it */
		else if (negation) {
			/* record ordinal # of deepest negated matched word */
			if (depth > faildeepest)
				faildeepest = depth;
		} else {
			/* record ordinal # of deepest plain matched word */
			if (depth > hitdeepest)
				hitdeepest = depth;
		}
	}
	if (debug)
		(void) fprintf(stderr, "mpatsmatch(`%s', `%s') returns %s\n",
			ngpat, grp, truth(hitdeepest > faildeepest));
	return hitdeepest > faildeepest;
}

/*
 * Match a pattern against a group by looking at each word of pattern in turn.
 *
 * On a match, return the ordinal number of the rightmost word that matches.
 * If group runs out first, the match fails; else it succeeds.
 * On a failure, return zero.
 */
STATIC int
onepatmatch(patp, grp)			/* match 1 pattern vs 1 group */
char *patp, *grp;
{
	register char *rpatwd;		/* used by word match (inner loop) */
	register char *patdot, *grdot;	/* point at dots after words */
	register char *patwd, *grwd;	/* point at current words */
	register int depth = 0;

	for (patwd = patp, grwd = grp; patwd != NULL && grwd != NULL;
	    patwd = patdot, grwd = grdot, depth++) {
		register int match;

	    	/* null-terminate words */
	    	INDEX(patwd, NGDELIM, patdot);
		if (patdot != NULL)
			*patdot = '\0';		/* will be restored below */
	    	INDEX(grwd, NGDELIM, grdot);
		if (grdot != NULL)
			*grdot = '\0';		/* will be restored below */

		/*
		 * Match one word of pattern with one word of group.
		 * A pattern word of "all" matches any group word.
		 */
#ifdef FAST_STRCMP
		match = STREQ(patwd, grwd) || STREQ(patwd, ALL);
#else
		match = NO;
		for (rpatwd = patwd; *rpatwd == *grwd++; )
			if (*rpatwd++ == '\0') {
				match = YES;		/* literal match */
				break;
			}
		if (!match) {
			/* ugly special case match for "all" */
			rpatwd = patwd;
			match = *rpatwd++ == 'a' && *rpatwd++ == 'l' &&
			        *rpatwd++ == 'l' && *rpatwd   == '\0';
		}
#endif				/* FAST_STRCMP */

		if (patdot != NULL)
			*patdot++ = NGDELIM;	/* point after the dot */
		if (grdot != NULL)
			*grdot++ = NGDELIM;
		if (!match) {
			depth = 0;		/* words differed - mismatch */
			break;
		}
	}
	/* if group name ran out before pattern, then match fails */
	if (grwd == NULL && patwd != NULL)
		depth = 0;
	if (debug)
		(void) fprintf(stderr, "onepatmatch(`%s', `%s') returns %d\n",
			patp, grp, depth);
	return depth;
}

#ifdef CROSS_POSTINGS_RESTRICTED
/*
 * ngtopsame(ngs) - true iff ngs are all in the same top-level distribution
 */
int
ngtopsame(ngs)
register char *ngs;
{
	register char *nextng;

	INDEX(ngs, NGSEP, nextng);
	if (nextng == NULL)		/* no groups left */
		return YES;
	++nextng;			/* skip NGSEP */
	return firstsame(ngs, nextng) && ngtopsame(nextng);
}

/*
 * firstsame(ng1, ng2) - true iff first characters (up to the first
 * NGDELIM or NGSEP) are the same in each string.  Neither string
 * is guaranteed to be null-terminated (a small lie; one *is*).
 */
STATIC int
firstsame(ng1, ng2)
register char *ng1, *ng2;
{
	register int ng1brk;
	static char delimstr[] = { NGSEP, NGDELIM, '\0' };
	extern int strcspn();

	ng1brk = strcspn(ng1, delimstr);
	return ng1brk == strcspn(ng2, delimstr) && STREQN(ng1, ng2, ng1brk);
}
#endif
