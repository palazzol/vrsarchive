/*
 * Save and load game
 */

#include "adv.h"

save()
{
	register int i;
	register FILE *fp;

	for (;;) {
		getnam();
		if (*buf2 == 0) {
			prt("Whatever you say boss!\n");
			return;
		}
		if ((fp = fopen(buf2, "w")) != NULL)
			break;
		prt("Can't open that file for writing; try again\n");
		prt("(Just hit RETURN if you changed your mind.)\n");
	}

	fprintf(fp, "%s\n", fname);
	fprintf(fp, "%d %d\n", rm, littim);
	for (i = 0; i < nitems; i++)
		fprintf(fp, "%d ", items[i].rm);
	fputs("\n:v\n", fp);
	fwrite(var, sizeof var, 1, fp);
	if (ferror(fp))
		prt("WARNING: error writing file\n");
	fclose(fp);
}

/*
 * As a hack, we just compare the last component of a filename, since
 * Unix lets people use "../../foo" or "/usr/chris/foo" to mean the
 * same file.  Not 100% correct but probably good enough....
 */
static char *
tail(s)
	register char *s;
{
	register char *p = s;

	while (*p)
		if (*p++ == '/' && *p != '/' && *p)
			s = p;
	return (s);
}

load()
{
	register int i;
	register FILE *fp;

	for (;;) {
		getnam();
		if (*buf2 == 0) {
			prt("Ok\n");
			return;
		}
		if ((fp = fopen(buf2, "r")) != NULL) {
			fgets(buf2, sizeof buf2, fp);
			buf2[strlen(buf2) - 1] = 0;
			if (strcmp(tail(buf2), tail(fname)) == 0)
				break;
			prt("That file isn't for this adventure.\n");
		}
		else {
			prt("No such file; try again\n");
			prt("(Just hit RETURN to get out of this mode.)\n");
		}
	}

	(void) fscanf(fp, "%d%d", &rm, &littim);
	for (i = 0; i < nitems; i++)
		(void) fscanf(fp, "%d", &items[i].rm);
	while ((i = getc(fp)) != ':' && i != EOF)
		;
	if (i != ':' || getc(fp) != 'v' || getc (fp) != '\n') {
		prt("Strange file!\n");
		isend = TRUE;	/* abort, stuff probably clobbered */
	}
	else {
		fread(var, sizeof var, 1, fp);
		if (feof(fp)) {
			prt("Unexpected EOF!\n");
			isend = TRUE;
		}
	}
	fclose(fp);
}

/*
 * Filename expansion hack: if the filename contains "glob" characters,
 * call the shell to expand the name.
 */
#define	ISGLOB(c)	((c) == '[' || (c) == '*' || (c) == '`' ||\
			 (c) == '?' || (c) == '{' || (c) == '$')

getnam()
{
	register int i;
	register char *s;
	int pid, pvec[2], status;
	char ch;

	/* Get a name from him */

	prt("Filename: ");
	prt(0);
	wgets(buf2, sizeof buf2 - 8);
	if (*buf2 != '~') {
		for (s = buf2; *s; s++)
			if (ISGLOB(*s))
				break;
		if (*s == 0)	/* No funny characters! */
			return;
	}

	/* Now have the C shell (sigh) glob it for us */
	if (pipe(pvec) < 0) {
nope:
		prt("[can't call csh; name not expanded]\n");
		return;
	}
	if ((pid = fork()) < 0) {
		close(pvec[0]);
		close(pvec[1]);
		goto nope;
	}
	for (i = strlen(buf2) + 1; i >= 0; --i)
		buf2[i+5] = buf2[i];
	buf2[0] = 'e';
	buf2[1] = 'c';
	buf2[2] = 'h';
	buf2[3] = 'o';
	buf2[4] = ' ';
	if (pid == 0) {
		dup2(pvec[1], 1);
		close(pvec[0]);
		close(2);		/* errors just disappear */
		open("/dev/null", 1);
		execl("/bin/csh", "csh", "-c", buf2, 0);
		_exit(1);
	}
	close(pvec[1]);
	while(wait (&status) != pid)	/* Let it run */
		;
	if (status) {
		prt("Warning: csh failed; name not expanded!\n");
		strcpy(buf2, buf2+5);
	}
	else {
		/*
		 * Efficiency?  What efficiency?
		 */
		i = 0;
		while (read(pvec[0], &ch, 1) == 1 && ch != ' ' && ch != '\n')
			buf2[i++] = ch;
		if (ch)
			buf2[i] = 0;
	}
	close(pvec[0]);
}
