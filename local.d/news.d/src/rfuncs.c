/*
 * rfuncs - functions for readnews.
 */

#ifndef lint
static char	*SccsId = "@(#)rfuncs.c	2.20	9/3/84";
#endif !lint

#include "rparams.h"

long nngsize;	/* The next upcoming value of ngsize. */
long nminartno;	/* Smallest article number in this group */

nextng()
{
	long	curpos;
#ifdef DEBUG
	fprintf(stderr, "nextng()\n");
#endif
	curpos = ftell(actfp);

next:
#ifdef DEBUG
	fprintf(stderr, "next:\n");
#endif
	if (actdirect == BACKWARD) {
		if (back()) {
			fseek(actfp, curpos, 0);
			return 1;
		}
		if (back()) {
			fseek(actfp, curpos, 0);
			return 1;
		}
	}
	if (fgets(afline, BUFLEN, actfp) == NULL)
		return 1;
	if (sscanf(afline, "%s %ld %ld", bfr, &nngsize, &nminartno) < 3) {
		bfr[0] = '\0';
		nngsize = 0;
		nminartno = 0;
	}
#ifdef DEBUG
	fprintf(stderr, "bfr = '%s'\n", bfr);
#endif

	ngcat(bfr);
	if (!ngmatch(bfr, header.nbuf))
		goto next;
	ngdel(bfr);
	if (xflag)
		readmode = SPEC;
	else
		readmode = NEXT;
	if (selectng(bfr, TRUE))
		goto next;
	return 0;
}


selectng(name, fastcheck)
char	*name;
{
	register char	*ptr, punct = ',';
	register int	i;
	register char	*p;
	register long	cur;
	long	next = 0;
	FILE *af;
	long s, sm;
	char buf[100], n[100];

	if (*groupdir)
		updaterc();
	last = 1;
	if (strcmp(name, bfr)) {
		af = xfopen(ACTIVE, "r");
		while (fgets(buf, sizeof buf, af) != NULL) {
			if (sscanf(buf, "%s %ld %ld", n, &s, &sm) == 3 &&
			     strcmp(n, name) == 0) {
				ngsize = s;
				minartno = sm;
				break;
			}
		}
		fclose(af);
	} else {
		ngsize = nngsize;
		minartno = nminartno;
	}
#ifdef DEBUG
	fprintf(stderr, "selectng(%s) sets ngsize to %ld, minartno to %ld\n",
		name, ngsize, minartno);
#endif
	strcpy(groupdir, name);
	if (!xflag) {
		i = findrcline(name);
		if (i >= 0) {
			if (index(rcline[i], '!')) {
				groupdir[0] = 0;
				return 1;
			}
			sprintf(rcbuf, "%s,%ld", rcline[i], ngsize+1);
		}
		else
			sprintf(rcbuf, "ng: %ld", ngsize+1);
	} else
		sprintf(rcbuf, "ng: %ld", ngsize+1);
#ifdef DEBUG
	fprintf(stderr, "rcbuf set to %s\n", rcbuf);
#endif DEBUG

	/*
	 * Fast check for common case: 1-###
	 */
	if (fastcheck) {
		p = rcbuf;
		while (*p != ' ')
			p++;
		while (*p == ' ')
			p++;
		if (*p++ == '1' && *p++ == '-') {
			i = 0;
			while (isdigit(*p))
				i = 10 * i + *p++ - '0';
			if (*p == ',' && i >= ngsize) {
				groupdir[0] = 0;
				return 1;
			}
		}
	}

/*
 * The key to understanding this piece of code is that a bit is set iff
 * that article has NOT been read.  Thus, we fill in the holes when
 * commas are found (e.g. 1-20,30-35 will result in filling in the 21-29
 * holes), and so we assume the newsrc file is properly ordered, the way
 * we write it out.
 */
	if ((ngsize-minartno) > BITMAPSIZE) {
		sprintf(buf," Bitmap not large enough for newsgroup %s", groupdir);
		xerror(buf);
	}

	cur = 0;
	/* Zero out the bitmap */
	p = &bitmap[(ngsize-minartno)/8+1];
	for (ptr = bitmap; ptr <= p;)
		*ptr++ = 0;

	/* Decode the .newsrc line indicating what we have read. */
	for (ptr = rcbuf; *ptr && *ptr != ':'; ptr++)
		;
	while (*ptr) {
		while (!isdigit(*ptr) && *ptr)
			ptr++;
		if (!*ptr)
			break;
		sscanf(ptr, "%ld", &next);
		if (punct == ',') {
			while (++cur < next) {
				set(cur);
			}
		}
		cur = next;
		while (!ispunct(*ptr) && *ptr)
			ptr++;
		punct = *ptr;
	}
	if (rflag)
		bit = ngsize+1;
	else
		bit = minartno -1;
	nextbit();
	ngrp = 1;
	return 0;
}

#ifdef TMAIL
catchterm()
{
	unlink(infile);
	unlink(outfile);
	xxit(0);
}


/*
 * The -M (Mail) interface.  This code is a reasonably simple model for
 * writing other interfaces.  We write out all relavent articles to
 * a temp file, then invoke Mail with an option to have it tell us which
 * articles it read.  Finally we count those articles as really read.
 */
Mail()
{
	register FILE *fp = NULL, *ofp;
	struct hbuf h;
	register char	*ptr, *fname;
	int	news = 0;
	register int i;

	for(i=0;i<NUNREC;i++)
		h.unrec[i] = NULL;

	ofp = xfopen(mktemp(outfile), "w");
	if (aflag && *datebuf)
		if ((atime = cgtdate(datebuf)) == -1)
			xerror("Cannot parse date string");
	while (!nextng())
		while (bit <= ngsize) {
			sprintf(filename, "%s/%ld", dirname(groupdir), bit);
			if (access(filename, 4)
			|| ((fp = fopen(filename, "r")) == NULL)
			|| (hread(&h, fp, TRUE) == NULL)
			|| !aselect(&h, FALSE)) {
#ifdef DEBUG
				fprintf(stderr, "Bad article '%s'\n", filename);
#endif
				if (fp != NULL) {
					fclose(fp);
					fp = NULL;
				}
				clear(bit);
				nextbit();
				continue;
			}
			fname = ptr = index(h.from, '(');
			if (fname) {
				while (ptr && ptr[-1] == ' ')
					ptr--;
				if (ptr)
					*ptr = 0;
				fname++;
				ptr = fname + strlen(fname) - 1;
				if (*ptr == ')')
					*ptr = 0;
			}
			h.subtime = cgtdate(h.subdate);
			fprintf(ofp, "From %s %s",
#ifdef INTERNET
			    h.from[0] ? h.from :
#endif
			    h.path, ctime(&h.subtime));
			if (fname)
				fprintf(ofp, "Full-Name: %s\n", fname);
			fprintf(ofp, "Newsgroups: %s\n", h.nbuf);
			fprintf(ofp, "Subject: %s\n", h.title);
			fprintf(ofp, "Article-ID: %s/%ld\n\n", groupdir, bit);
			tprint(fp, ofp, TRUE);
			putc('\n', ofp);
			news = TRUE;
			fclose(fp);
			fp = NULL;
			nextbit();
		}
	updaterc();
	fclose(ofp);
	if (!news) {
		fprintf(stderr, "No news.\n");
		unlink(outfile);
		return;
	}
	signal(SIGHUP, catchterm);
	signal(SIGTERM, catchterm);
	sprintf(bfr, "%s -f %s -T %s", TMAIL, outfile, mktemp(infile));
	fwait(fsubr(ushell, bfr, (char *)NULL));
	ofp = xfopen(infile, "r");
	fseek(actfp, 0L, 0);
	while (fgets(afline, BUFLEN, actfp) != NULL) {
		last = 0;
		if (sscanf(afline, "%s %ld", bfr, &nngsize) < 2) {
			bfr[0] = '\0';
			nngsize = 0;
		}
		ngcat(bfr);
		if (!ngmatch(bfr, header.nbuf))
			continue;
		ngdel(bfr);
		*groupdir = 0;
		if (selectng(bfr, TRUE))
			continue;
		fseek(ofp, 0L, 0);
		while (fgets(groupdir, BUFLEN, ofp) != NULL) {
			nstrip(groupdir);
			ptr = index(groupdir, '/');
			*ptr = 0;
			if (strcmp(bfr, groupdir))
				continue;
			sscanf(++ptr, "%ld", &last);
			clear(last);
		}
		if (last) {
			strcpy(groupdir, bfr);
			updaterc();
		}
	}
	unlink(infile);
	unlink(outfile);
}
#endif

updaterc()
{
	register long	cur = 1, next = 1;
	register int i;
	register char	*ptr;
	char	oldptr;

	sprintf(rcbuf, "%s%c ", groupdir, zapng ? '!' : ':');

	zapng = FALSE;
again:
	ptr = &rcbuf[strlen(rcbuf)];
	while (get(next) && next <= ngsize)
		next++;
	cur = next;
	while (!(get(next)) && next <= ngsize)
		next++;
	if (cur == next) {
		next = ngsize + 1;
		goto skip;
	}
	if (cur + 1 == next)
		sprintf(ptr, "%ld,", cur);
	else
		sprintf(ptr, "%ld-%ld,", cur, next - 1);
skip:
	if ((long) next > ngsize) {
		if (index(rcbuf, ',') != NULL)
			ngdel(rcbuf);
		else if (index(rcbuf, '!') == NULL)
			return;
		ptr = index(rcbuf, ' ');
		ptr--;
		oldptr = *ptr;
		ptr[0] = ':';
		ptr[1] = '\0';
		i = findrcline(groupdir);
		if (i >= 0) {
			ptr[0] = oldptr;
			ptr[1] = ' ';
			rcline[i] = realloc(rcline[i], (unsigned)(strlen(rcbuf) + 1));
			if (rcline[i] == NULL)
				xerror("Cannot realloc");
			strcpy(rcline[i], rcbuf);
			return;
		}
		if (++line > LINES)
			xerror("Too many newsgroups");
		ptr[0] = oldptr;
		ptr[1] = ' ';
		if ((rcline[line] = malloc((unsigned)(strlen(rcbuf) + 1))) == NULL)
			xerror("Not enough memory");
		strcpy(rcline[line], rcbuf);
		return;
	}
	cur = next;
	goto again;
}

newrc(rcname)
char *rcname;
{
	register FILE *fp;

	if (close(creat(rcname, 0666))) {
		sprintf(bfr, "Cannot create %s", newsrc);
		xerror(bfr);
	}

	sprintf(bfr, "%s/users", LIB);
	if ((fp = fopen(bfr, "a")) != NULL) {
		fprintf(fp, "%s\n", username);
		fclose(fp);
		chmod(bfr, 0666);
	}
}

nextbit()
{
#ifdef DEBUG
	fprintf(stderr,"nextbit() bit = %ld\n", bit);
#endif DEBUG
	last = bit;
	if (readmode == SPEC || xflag) {
		if (rflag)
			bit--;
		else
			bit++;
		return;
	}
	if (rflag)
		while (--bit, !get(bit) && bit > minartno)
			;
	else
		while (++bit, !get(bit) && bit <= ngsize)
			;
#ifdef DEBUG
	fprintf(stderr,"nextng leaves bit as %ld\n", bit);
#endif DEBUG
}

xxit(status)
int	status;
{
	unlink(infile);
	unlink(outfile);
#ifdef SORTACTIVE
	if (strncmp(ACTIVE,"/tmp/",4) == 0)	/* Paranoia */
		unlink(ACTIVE);
#endif SORTACTIVE
	exit(status);
}

/*
 * Return TRUE if the user has not ruled out this article.
 */
aselect(hp, insist)
register struct hbuf *hp;
int	insist;
{
	if (insist)
		return TRUE;
	if (tflag && !titmat(hp, header.title))
		return FALSE;
	if (aflag && cgtdate(hp->recdate) < atime)
		return FALSE;
	if (index(hp->nbuf, ',') && !rightgroup(hp))
		return FALSE;
	if (fflag && isfol(hp))
		return FALSE;
	return TRUE;
}

/*
 * Code to avoid showing multiple articles for news.
 * Works even if you exit news.
 * Returns nonzero if we should show this article.
 */
rightgroup(hp)
struct hbuf *hp;
{
	char ng[BUFLEN];
	register char *p, *g;
	int i, flag;

	strcpy(ng, hp->nbuf);
	ngcat(ng);
	g = ng;
	flag = 1;
	while ((p = index(g, ',')) != NULL) {
		*p++ = '\0';
		while (*p == ' ')
			p++;
		if (strcmp(g, groupdir) == 0)
			return flag;
		if (ngmatch(g, header.nbuf)
		    && ((i = findrcline(g)) >= 0
		    && index(rcline[i], '!') == NULL))
			flag = 0;
		g = p;
	}
	/* we must be in "junk" or "control" */
	return TRUE;
}

/*
 * Return TRUE if this article is a followup to something.
 */
isfol(hp)
register struct hbuf *hp;
{
	if (hp->followid[0])
		return TRUE;
	if (strncmp(hp->title, "Re:", 3) == 0)
		return TRUE;
	return FALSE;
}

back()
{
	while (fseek(actfp, -2L, 1) != -1 && ftell(actfp) > 0L) {
		if (getc(actfp) == '\n')
			return 0;
	}
	if (ftell(actfp) == 0L)
		return 0;
	return 1;
}

/*
 * Trap interrupts.
 */
onsig(n)
int	n;
{
	signal(n, onsig);
	sigtrap = n;
	if (rcreadok < 2) {
		fprintf(stderr, "Aborted early\n");
		xxit(0);
	}
}

/*
 * finds the line in your .newsrc file (actually the in-core "rcline"
 * copy of it) and returns the index into the array where it was found.
 * -1 means it didn't find it.
 *
 * We play clever games here to make this faster.  It's inherently
 * quadratic - we spend lots of CPU time here because we search through
 * the whole .newsrc for each line.  The "prev" variable remembers where
 * the last match was found; we start the search there and loop around
 * to the beginning, in the hopes that the calls will be roughly in order.
 */
int
findrcline(name)
char *name;
{
	register char *p, *ptr;
	register int cur;
	register int i;
	register int top;
	static int prev = 0;

	top = line; i = prev;
loop:
	for (; i <= top; i++) {
		for (p = name, ptr = rcline[i]; (cur = *p++); ) {
			if (cur != *ptr++)
				goto contin2;
		}
		if (*ptr != ':' && *ptr != '!')
			continue;
		prev = i;
		return i;
contin2:
		;
	}
	if (i > line && line > prev-1) {
		i = 0;
		top = prev-1;
		goto loop;
	}
	return -1;
}

#ifdef SORTACTIVE
/*
 * sortactive - make a local copy of the active file, sorted according
 *   to the user's preferences, according to his .newsrc file.
 * Marvin Solomon, University of Wisconsin, 1/2/84
 */

struct table_elt {
	long actpos, rcpos;
};

static int
rcsort(a,b)
struct table_elt *a, *b;
{
	return(a->rcpos - b->rcpos);
}

sortactive()
{
	char newactivename[BUFLEN], aline[BUFLEN], *strcpy(), *p;
	register FILE *nfp, *afp;
	struct table_elt table[LINES];
	int nlines = 0, i;
	long actpos;

	/* make a new sorted copy of ACTIVE in the user's home directory */
	strcpy(newactivename, mktemp("/tmp/newsaXXXXXX"));
	nfp = fopen(newactivename, "w");
	if (nfp == NULL) {
		perror(newactivename);
		return;
	}

	/* look up all the lines in ACTIVE, finding their positions in .newsrc */
	afp = xfopen(ACTIVE, "r");
	actpos = ftell(afp);
	while (fgets(aline, sizeof aline, afp) != NULL) {
		if (p = index(aline, ' '))
			*p = '\0';
		table[nlines].rcpos = findrcline(aline);
		table[nlines].actpos = actpos;
		nlines++;
		actpos = ftell(afp);
	}

	/* sort by position in user's .newsrc file (new groups come up first) */
	qsort((char *)table, nlines, sizeof table[0], rcsort);

	/* copy active to newactive, in the new order */
	for (i=0; i<nlines; i++) {
		(void) fseek(afp, table[i].actpos, 0);
		(void) fgets(aline, sizeof aline, afp);
		(void) fprintf(nfp, "%s", aline);
	}
	(void) fclose(afp);
	(void) fclose(nfp);

	/* make the rest of readnews think the local active file is the real one */
	free(ACTIVE);
	ACTIVE = AllocCpy(newactivename);
}
#endif SORTACTIVE
