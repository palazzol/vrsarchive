/*
 * sendnews - send news article by mail.
 */

#ifndef lint
static char	*SccsId = "@(#)sendnews.c	2.8	8/14/84";
#endif !lint

#include <stdio.h>
#include <ctype.h>
#ifndef USG
struct utsname {
	char	Sysname[9];
	char	nodename[33];
	char	release[9];
	char	version[9];
};
#else
#include <sys/utsname.h>
#endif

#define	eq(a,b)	(strcmp(a,b) == 0)
#define	LNLEN	7			/* strlen("ucbvax!") */

char *index();
char buffer[BUFSIZ];
int linecount, oflag = 0, aflag = 0, bflag = 0, toflag = 0;

FILE *popen();
main(argc, argv)
char **argv;
{
	FILE *out;
	char newsgroup[100];
	char sysn[20];
	struct utsname ubuf;
#ifdef lint
	argc = argc;
#endif lint

	while (**(++argv) == '-') {
		if (*++*argv == 'o')
			oflag++;
		else if (**argv == 'a')
			aflag++;
		else if (**argv == 'b')
			bflag++;
		else if (**argv == 'n')
			strcpy(newsgroup, *(++argv));
	}
	if (aflag && bflag) {
		fprintf(stderr, "'-a' and '-b' options mutually exclusive.\n");
		exit(1);
	}

#ifdef debug
	printf("mail %s\n", *argv);
	sprintf(buffer, "cat");
#else
	sprintf(buffer, "mail %s", *argv);
#endif
	out = popen(buffer, "w");
	uname(&ubuf);
	strcpy(sysn, ubuf.nodename);
	strcat(sysn, "!");

	/* Standard mail prelude to make the formatters happy */
	fprintf(out, "To: %s\n", *argv);
	fprintf(out, "Subject: network news article\n");
	fprintf(out, "\n");

	while (fgets(buffer, sizeof buffer, stdin)) {
		if (*newsgroup && ngline()) {
			if (oflag)
				sprintf(buffer, "%s\n", newsgroup);
			else
				sprintf(buffer, "Newsgroups: %s\n", newsgroup);
		}
		putc('N', out);
		fputs(buffer, out);
	}
	pclose(out);
	exit(0);
}

ngline()
{
	if (oflag)
		return linecount == 2;
	if (!toflag && (!strncmp("Newsgroups: ", buffer, 12) ||
		!strncmp("To: ",buffer, 4)))
		return ++toflag;
	return 0;
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}
