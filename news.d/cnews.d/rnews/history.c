/*
 * history file bashing
 *
 * B news pulls a dirty (and undocumented) trick and converts message-id's
 * to lower case before using them as keys in the dbm file.  For the sake of
 * the news readers, we do the same, under protest.  Grump.
 */

#include <stdio.h>
#include <sys/types.h>
#include "news.h"
#include "newspaths.h"
#include "headers.h"

/* give 0 & 2 pretty, SVIDish names */
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_END 2
#endif

typedef struct {
	char *dptr;
	int dsize;
} datum;

static FILE *fp = NULL;
static char filename[MAXFILE];

/* forward decls */
extern datum fetch(), getposhist();

static
histname()
{
	if (filename[0] == '\0')
		(void) strcpy(filename, libfile("history"));
}

static int
openhist()
{
	int status = 0;
	static int opened = NO;

	histname();
	if (fp == NULL)
		if ((fp = fopenwclex(filename, "a+")) == NULL)
			status |= ST_DROPPED;	/* fopenwclex complained already */
	if (opened++ == NO && dbminit(filename) < 0)
		status |= ST_DROPPED;	/* dbminit will honk */
	return status;
}

static datum
getposhist(msgid)		/* return seek offset of history entry */
char *msgid;
{
	register char *lcmsgid;
	datum msgidkey, offset;

	msgidkey.dptr = NULL;
	if (openhist()&ST_DROPPED)
		return msgidkey;		/* no data base */

	/* dirty trick (part 1 of 2): convert copy of msgid to lower case */
	lcmsgid = strsave(msgid);
	strlower(lcmsgid);

	msgidkey.dptr = lcmsgid;
	msgidkey.dsize = strlen(lcmsgid) + 1;	/* include NUL */
	offset = fetch(msgidkey);	/* look up offset by l.c. msgid */

	free(lcmsgid);
	return offset;
}

int
alreadyseen(msgid)		/* return true if found in the data base */
char *msgid;
{
	datum posdatum;

	posdatum = getposhist(msgid);
	return posdatum.dptr != NULL;
}

char *				/* NULL if no history entry */
gethistory(msgid)		/* return existing history entry, if any */
char *msgid;
{
	long pos = 0;
	static char histent[MAXLINE+1];
	datum posdatum;

	histent[0] = '\0';
	posdatum = getposhist(msgid);
	if (posdatum.dptr != NULL && posdatum.dsize == sizeof pos) {
		memcpy((char *)&pos, posdatum.dptr, sizeof pos); /* align */
		if (fseek(fp, pos, SEEK_SET) != -1 &&
		    fgets(histent, sizeof histent, fp) != NULL)
			return histent;
	}
	return NULL;
}

char *
findfiles(histent)		/* side-effect: trims \n */
char *histent;
{
	char *tabp;

	trim(histent);
	tabp = rindex(histent, '\t');
	if (tabp != NULL)
		++tabp;		/* skip to start of files list */
	return tabp;
}

int
history(hdrs)				/* generate history entries */
register struct headers *hdrs;
{
	register char *lcmsgid;
	int status = 0;
	time_t now;
	long pos;
	char msgid[MAXLINE];			/* Message-ID sans \t & \n */
	char expiry[MAXLINE];			/* Expires sans \t & \n */
	datum msgidkey, posdatum;

	/* strip \n & \t to keep history file format sane */
	sanitise(hdrs->h_msgid, msgid, sizeof msgid);
	sanitise(hdrs->h_expiry, expiry, sizeof expiry);

	/* TODO: is the 3rd parameter needed anymore? */
	timestamp(stdout, &now, (char **)NULL);
	(void) printf(" got %s", msgid);	/* NB: no newline */

	status |= openhist();
	if (status&ST_DROPPED)
		return status;

	/* generate history file entry */
	(void) fseek(fp, 0L, SEEK_END);
	pos = ftell(fp);			/* get seek ptr for dbm */
	/*
	 * B 2.10.3+ rnews puts out a leading space before received time
	 * if the article contains an Expires: header; tough.
	 * C news does this right instead of compatibly.
	 *
	 * The second field is really two: time-received and Expires: value,
	 * separated by a tilde.  This is an attempt at partial compatibility
	 * with B news, in that C expire can cope with B news history files.
	 */
	(void) fprintf(fp, "%s\t%ld~%s\t%s\n", msgid, now, expiry, hdrs->h_files);
	(void) fflush(fp);			/* for crash-proofness */
	if (ferror(fp))
		status = fulldisk(status|ST_DROPPED, filename);

	/* record (msgid, position) in data base */

	/* dirty trick (part 2 of 2): convert copy of msgid to lower case */
	lcmsgid = strsave(msgid);
	strlower(lcmsgid);

	msgidkey.dptr = lcmsgid;
	msgidkey.dsize = strlen(lcmsgid) + 1;	/* include NUL */
	/*
	 * There is no point to storing pos in network byte order,
	 * since dbm files are machine-dependent and so can't be shared
	 * by dissimilar machines anyway.
	 */
	posdatum.dptr = (char *)&pos;
	posdatum.dsize = sizeof pos;
#ifdef NOSTOREVAL
	/* original v7 dbm store() returned no value */
	(void) store(msgidkey, posdatum);
#else
	if (store(msgidkey, posdatum) < 0)	/* store l.c. msgid */
		status = fulldisk(status|ST_DROPPED, filename);
#endif
	free(lcmsgid);
	return status;
}

/* strip \n & \t from dirty into clean, which is no more than cleanlen long */
sanitise(dirty, clean, cleanlen)
char *dirty;
register char *clean;
unsigned cleanlen;
{
	if (dirty == NULL)
		(void) strncpy(clean, "", (int)cleanlen);
	else
		(void) strncpy(clean, dirty, (int)cleanlen);
	for (; *clean != '\0'; ++clean)
		if (*clean == '\t' || *clean == '\n')
			*clean = ' ';
}
