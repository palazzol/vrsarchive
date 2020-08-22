/*
 * History file
 * each line contains a message-id, install or expire time
 * names of linked files
 */

#include "defs.h"

static char histname[]	 = HISTORY;
static char *histid;			/* messageid to save */
static char *histline;			/* list of linked files */
static long etime;			/* expire time */

typedef enum stypes { 
	chk, delete } stype;

/*
 * do things with history file
 * chk - see if id present
 * delete - delete article with id
 */
static bool
searchhist(id, type)
char *id;
stype type;
{
	register FILE	*f;
	register char *s, *name;
	register bool	found;
	char buf[BUFSIZ * 2];

	extern char *newsdir;

	f = fopenl(histname);

	found = false;
	while (fgets(buf, sizeof(buf), f)) {
		if (s = strchr(buf, ' '))
			*s = '\0';
		else
			error("Bad format: %s", histname);
		if (CMP(buf, id) == 0) {
			found = true;
			break;
		}
	}
	if (found && type == delete) {
		if ((name = strchr(s + 1, ' ')) == NIL(char))
			error("Bad format: %s", histname);
		name++;
		while (name && (s = strpbrk(name, " \n"))) {
			*s = '\0';
			name = newstr3(newsdir, "/", name);
			remove(name);
			free(name);
			name = s + 1;
		}
	}
	fclose(f);
#if !AUSAM
	unlock(histname);
#endif
	return found;
}


/*
 * delete files given id
 */
bool
cancel(id)
char *id;
{
	bool searchhist();

	return searchhist(id, delete);
}


/*
 * check if article has been recieved
 */
bool
chkhist(id)
char *id;
{
	bool searchhist();

	return searchhist(id, chk);
}


/*
 * scan history, clearing uflag list entry if id not seen
 */
scanhist(ulist, usize)
char **ulist;
int usize;
{
	register FILE	*f;
	register char *s, **found;
	register int i;
	char *key[1];
	char buf[BUFSIZ * 2];
	bool		 * seen;

	extern char *newsdir;

	seen = (bool * ) myalloc((int) sizeof(bool) * usize);
	memset((char *)seen, 0, (int) sizeof(bool) * usize);

	f = fopenf(histname, "r");
	while (fgets(buf, sizeof(buf), f)) {
		if (s = strchr(buf, ' '))
			*s = '\0';
		else
			error("Bad format: %s", histname);
		key[0] = buf;
		found = (char **) bsearch((char *) key, (char *) ulist, (unsigned) usize,
		     sizeof(char *), strpcmp);
		if (found)
			seen[found - ulist] = true;
	}
	fclose(f);

	for (i = 0; i < usize; i++)
		if (!seen[i]) {
			free(ulist[i]);
			ulist[i] = NIL(char);
		}
	free((char *)seen);
}


/*
 * open hist file, write id and time
 */
openhist(hp)
header *hp;
{

	histid = newstr(hp->h_messageid);
	if (hp->h_expires)
		etime = atot(hp->h_expires);
	else
		etime = 0L;
	histline = NIL(char);
}


/*
 * write name of file article resides into history file
 */
writehist(fname)
char *fname;
{
	histline = (histline ? catstr2(histline, " ", fname) : newstr(fname));
}


/*
 * close history file
 */
closehist()
{
	register FILE	*f;
	extern long now;

	f = fopenl(histname);
	fseek(f, 0L, 2);
	(void) fprintf(f, "%s %s%ld %s\n", histid, etime ? "E" : "", etime ? etime :
	    now, histline);
	fclose(f);
#if !AUSAM
	unlock(histname);
#endif
	free(histid); 
	free(histline);
}


/*
 * remove a news item
 * check owner first
 */
static
remove(fname)
char *fname;
{
	header			h;
	FILE			 * f;
	register char *s, *mname;

#if AUSAM
	extern struct pwent pe;
#else
	extern struct passwd *pp;
#endif
	extern char systemid[];
	extern bool		su;
	extern bool		pflag;

	if (!su && !pflag) {
		f = fopenf(fname, "r");
		gethead(f, &h);
		fclose(f);
		if (s = strchr(h.h_from, ' '))
			*s = '\0';
		mname = newstr5(
#if AUSAM
				pe.pw_strings[LNAME],
#else
				pp->pw_name,
#endif
			"@", systemid, ".", MYDOMAIN);
		if (CMP(mname, h.h_from) != 0)
			error("Can't cancel articles you didn't write.");
		free(mname);
	}
	if (unlink(fname) != 0)
		error("Couldn't unlink %s", fname);

}
