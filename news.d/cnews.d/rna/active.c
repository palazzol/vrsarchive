/*
 * active file handling routines
 *
 * format of file:
 *	<groupname> ' ' <5 digit #> ' ' <5 digit #> ' ' flag '\n'
 *			  (seq)		  (low)
 */

#include "defs.h"

static char actname[]	 = ACTIVE;
static int lineno;
static active	*alist;

/*
 * getseq - Get next sequence number for this group
 *	    and update active file.
 *	    If group missing append to file.
 */
char *
getseq(group)
char *group;
{
	register FILE	*f;
	register int i;
	char gbuf[BUFSIZ / 2], dbuf[BUFSIZ / 4], dbuf2[BUFSIZ / 4];
	extern char *itoa();

	f = fopenl(actname);
	lineno = 0;
	while (getline(f, gbuf, dbuf, dbuf2))
		if (CMP(gbuf, group) == 0) {
			i = atoi(dbuf);
			i++;
			fseek(f, -12L, 1);
			(void) fprintf(f, "%05d", i);
			fclose(f);
#if !AUSAM
			unlock(actname);
#endif
			return itoa(i);
		}
	(void) fprintf(f, "%s 00001 00001 y\n", group);
	fclose(f);
#if !AUSAM
	unlock(actname);
#endif
	return itoa(1);
}


static
getline(f, g, d, d2)
register FILE *f;
char *g, *d, *d2;
{
	register int c;
	register char *s;

	lineno++;
	s = g;
	while ((c = getc(f)) != ' ' && c != EOF)
		*s++ = c;
	*s = '\0';

	if (c != EOF) {
		s = d;
		while ((c = getc(f)) != EOF && isdigit(c))
			*s++ = c;
		*s = '\0';

		s = d2;
		if (c == ' ')
			while ((c = getc(f)) != EOF && isdigit(c))
				*s++ = c;
		*s = '\0';

		if (c == ' ')
			while ((c = getc(f)) != EOF && c != '\n')
				;		/* eat flag */
	}

	if (c != EOF && (c != '\n' || !*d || !*d2))
		error("%s: bad format: line %d", actname, lineno);
	return c != EOF;
}


/*
 * build internal active file structure
 */
active *
readactive()
{
	register FILE	*f;
	register active	*ap, *last;
	char gbuf[BUFSIZ / 2], dbuf[BUFSIZ / 4], dbuf2[BUFSIZ / 4];

	alist = last = NIL(active);
	f = fopenf(actname, "r");
	lineno = 0;
	while (getline(f, gbuf, dbuf, dbuf2)) {
		ap = NEW(active);
		ap->a_name = newstr(gbuf);
		ap->a_seq = atoi(dbuf);
		ap->a_low = atoi(dbuf2);
		ap->a_next = NIL(active);
		if (!alist)
			alist = ap;
		else
			last->a_next = ap;
		last = ap;
	}
	fclose(f);
	return alist;
}


/*
 * return pointer to named group
 */
active *
activep(grp)
register char *grp;
{
	register active	*ap;

	for (ap = alist; ap; ap = ap->a_next)
		if (CMP(grp, ap->a_name) == 0)
			break;
	return ap;
}


/*
 * setlow - set the low number for this group
 */
setlow(group, low)
char *group;
int low;
{
	register FILE	*f;
	char gbuf[BUFSIZ / 2], dbuf[BUFSIZ / 4], dbuf2[BUFSIZ / 4];
	extern char *itoa();

	f = fopenl(actname);
	lineno = 0;
	while (getline(f, gbuf, dbuf, dbuf2))
		if (CMP(gbuf, group) == 0) {
			fseek(f, -6L, 1);
			(void) fprintf(f, "%05d", low);
			break;
		}
	fclose(f);
#if !AUSAM
	unlock(actname);
#endif
}



/*
 * initgrp - initialise an entry for this group
 */
initgrp(group)
char *group;
{
	register FILE	*f;
	char gbuf[BUFSIZ / 2], dbuf[BUFSIZ / 4], dbuf2[BUFSIZ / 4];

	f = fopenl(actname);
	lineno = 0;
	while (getline(f, gbuf, dbuf, dbuf2))
		if (CMP(gbuf, group) == 0) {
#if !AUSAM
			unlock(actname);
#endif
			return;
		}
	(void) fprintf(f, "%s 00000 00001\n", group);

}
