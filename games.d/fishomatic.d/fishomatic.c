#include "stdio.h"

#ifndef BUFSIZ
#define BUFSIZ		1024
#endif

#define RANKCOUNT	13

char	leader[] = 	"A234567891JQK";
int	ihave[RANKCOUNT];
int	uhave[RANKCOUNT];

char *	begins(string, beginning)
char *	string;
char *	beginning;
{
	while (*beginning != '\0')
		if (*string++ != *beginning++)
			return 0;
	return string;
}

main(argc, argv)
int	argc;
char *	argv[];
{
	char *	cp;
	int	i;
	int	result;
	int	didpro;
	int	itoufids[2];
	int	utoifids[2];
	char	buf[BUFSIZ];
	char	outbuf[4];

	if (pipe(itoufids) != 0 || pipe(utoifids) != 0)
		return 2;
	switch (fork()) {
		case 0:	/* child */
			if (close(0) == 0 && close(1) == 0 &&
				dup(itoufids[0]) == 0 && dup(utoifids[1]) == 1)
					execl("/usr/games/fish", "fish", 0);
		case -1:
			return 2;
	}
	close(utoifids[1]);
	didpro = 0;
	while ((i = getit(buf, utoifids[0], itoufids[1])) > 0) {
		if (argc == 1)
			printf("%s", buf);
		if (begins(buf, "instructions?")) {
			write(itoufids[1], "n\n", 2);
			if (argc == 1)
				printf("n\n");
		}
		if (cp = begins(buf, "your hand is: ")) {
			for (i = 0; i < RANKCOUNT; ++i)
				ihave[i] = 0;
			for ( ; *cp != '+' && *cp != '\0'; ++cp)
				for (i = 0; i < RANKCOUNT; ++i)
					if (*cp == leader[i])
						ihave[i] = 1;
		}
		if (begins(buf + 1, "ou ask me for: ")) {
			cp = outbuf;
			if (didpro == 0) {
				*cp++ = 'p';
				didpro = 1;
			} else if ((i = which()) >= 0) {
				uhave[i] = 0;
				if ((*cp++ = leader[i]) == '1')
					*cp++ = '0';
			}
			*cp++ = '\n';
			*cp = '\0';
			write(itoufids[1], outbuf, cp - outbuf);
			if (argc == 1)
				printf(outbuf);
		}
		if (cp = begins(buf, "I ask you for: "))
			for (i = 0; i < RANKCOUNT; ++i)
				if (*cp == leader[i])
					uhave[i] = 1;
		if (cp = begins(buf, "Your books: "))
			result = (strlen(cp) - 1) / 2;
	}
	if (i < 0)
		return 2;
	if (argc != 1)
		printf("%d\n", result);
	return result <= RANKCOUNT / 2;
}

int	lasti;

int	which()
{
	int	i;

	for (i = 0; i < RANKCOUNT; ++i)
		if (ihave[i] && uhave[i])
			return lasti = i;
	for (i = 0; i < RANKCOUNT; ++i)
		if (ihave[lasti = (lasti + 1) % RANKCOUNT])
			return lasti;
	return -1;
}

int	unread;
char	mybuf[BUFSIZ + 1];
char *	saveptr = mybuf;

#define WHATLEN	strlen("what?\n")

int	getit(buf, in, out)
char *	buf;
{
	char *	cp;
	int	i;

	cp = buf;
	for ( ; ; ) {
		if (*saveptr == '\0') {
			unread = unread - WHATLEN;
			i = 0;
			while (unread <= BUFSIZ) {
				mybuf[i++] = 'n';
				mybuf[i++] = '\n';
				unread = unread + WHATLEN;
			}
			write(out, mybuf, i);
			i = read(in, mybuf, BUFSIZ);
			if (i <= 0)
				return i;
			mybuf[i] = '\0';
			saveptr = mybuf;
		}
		if ((*cp++ = *saveptr++) == '\n') {
			*cp = '\0';
			if (strcmp(buf, "what?\n") != 0)
				return cp - buf;
			unread = unread - WHATLEN;
			cp = buf;
			continue;
		}
		*cp = '\0';
		if (begins(buf + 1, "ou ask me for: "))
			return cp - buf;
	}
}
