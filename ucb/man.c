#ifndef lint
static char sccsid[] = "@(#)man.c       1.10 (Berkeley) 9/19/83";
#endif

#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
/*
 * man
 * link also to apropos and whatis
 * This version uses more for underlining and paging.
 */
#define MANDIR  "/usr/man"              /* this is deeply embedded */
#define NROFFCAT "nroff -h -man"        /* for nroffing to cat file */
#define NROFF   "nroff -man"            /* for nroffing to tty */
#define MORE    "more -s"               /* paging filter */
#define CAT     "cat -s"                /* for '-' opt (no more) */

#ifdef VTROFF
#define TROFFCMD \
"troff -t -man /usr/lib/tmac/tmac.vcat %s | /usr/lib/rvsort |/usr/ucb/vpr -t"
#else
#define TROFFCMD "troff -man %s"
#endif

#define ALLSECT "1nl6823457po"  /* order to look through sections */
#define SECT1   "1nlo"          /* sections to look at if 1 is specified */
#define SUBSEC1 "mcgprv"        /* subsections to try in section 1 */
#define SUBSEC2 "v"
#define SUBSEC3 "jxmsnvcf"
#define SUBSEC4 "pfvsn"
#define SUBSEC8 "vc"

extern char    *calloc();
extern char    *strcpy();
extern char    *strcat();

int     nomore;
char    *manpath = "/usr/man";
char    *trim();
int     remove();
int     section;
int     subsec;
int     troffit;

#define eq(a,b) (strcmp(a,b) == 0)

main(argc, argv)
        int argc;
        char *argv[];
{

        if (signal(SIGINT, SIG_IGN) == SIG_DFL) {
                (void) signal(SIGINT, remove);
                (void) signal(SIGQUIT, remove);
                (void) signal(SIGTERM, remove);
        }
        (void) umask(0);
        if (strcmp(argv[0], "apropos") == 0) {
                apropos(argc-1, argv+1);
                (void) exit(0);
        }
        if (strcmp(argv[0], "whatis") == 0) {
                whatis(argc-1, argv+1);
                (void) exit(0);
        }
        if (argc <= 1) {
                (void) fprintf(stderr, "Usage: man [ section ] name ...\n");
                (void) exit(1);
        }
        argc--, argv++;
        while (argc > 0 && argv[0][0] == '-') {
                switch(argv[0][1]) {

                case 0:
                        nomore++;
                        break;

                case 't':
                        troffit++;
                        break;

                case 'k':
                        apropos(argc-1, argv+1);
                        (void) exit(0);

                case 'f':
                        whatis(argc-1, argv+1);
                        (void) exit(0);

                case 'P':
                        argc--, argv++;
                        manpath = *argv;
                        break;
                }
                argc--, argv++;
        }
        if (chdir(manpath) < 0) {
                (void) fprintf(stderr, "Can't chdir to %s.\n", manpath);
                (void) exit(1);
        }
        if (troffit == 0 && nomore == 0 && !isatty(1))
                nomore++;
        section = 0;
        do {
                if (eq(argv[0], "local")) {
                        section = 'l';
                        goto sectin;
                } else if (eq(argv[0], "new")) {
                        section = 'n';
                        goto sectin;
                } else if (eq(argv[0], "old")) {
                        section = 'o';
                        goto sectin;
                } else if (eq(argv[0], "public")) {
                        section = 'p';
                        goto sectin;
                } else if (argv[0][0] >= '0' && argv[0][0] <= '9' && (argv[0][1] == 0 || argv[0][2] == 0)) {
                        section = argv[0][0];
                        subsec = argv[0][1];
sectin:
                        argc--, argv++;
                        if (argc == 0) {
                                (void) fprintf(stderr, "But what do you want from section %s?\n", argv[-1]);
                                (void) exit(1);
                        }
                        continue;
                }
                manual(section, argv[0]);
                argc--, argv++;
        } while (argc > 0);
        (void) exit(0);
}

manual(sec, name)
        char sec;
        char *name;
{
        char sect = sec;
        char work[100], work2[100], cmdbuf[150];
        int ss;
        struct stat stbuf, stbuf2;
        int last;
        char *sp = ALLSECT;

        (void) strcpy(work, "manx/");
        (void) strcat(work, name);
        (void) strcat(work, ".x");
        last = strlen(work) - 1;
        if (sect == '1') {
                sp = SECT1;
                sect = 0;
        }
        if (sect == 0) {
                ss = 0;
                for (sect = *sp++; sect; sect = *sp++) {
                        work[3] = sect;
                        work[last] = sect;
                        work[last+1] = 0;
                        work[last+2] = 0;
                        if (stat(work, &stbuf) >= 0)
                                break;
                        if (work[last] >= '1' && work[last] <= '8') {
                                char *cp;
search:
                                switch (work[last]) {
                                case '1': cp = SUBSEC1; break;
                                case '2': cp = SUBSEC2; break;
                                case '3': cp = SUBSEC3; break;
                                case '4': cp = SUBSEC4; break;
                                case '8': cp = SUBSEC8; break;
                                }
                                while (*cp) {
                                        work[last+1] = *cp++;
                                        if (stat(work, &stbuf) >= 0) {
                                                ss = work[last+1];
                                                goto found;
                                        }
                                }
                                if (ss == 0)
                                        work[last+1] = 0;
                        }
                }
                if (sect == 0) {
                        if (sec == 0)
                                (void) printf("No manual entry for %s.\n", name);
                        else
                                (void) printf("No entry for %s in section %c of the manual.\n", name, sec);
                        return;
                }
        } else {
                work[3] = sect;
                work[last] = sect;
                work[last+1] = subsec;
                work[last+2] = 0;
                if (stat(work, &stbuf) < 0) {
                        if ((sect >= '1' && sect <= '8') && subsec == 0) {
                                sp = "\0";
                                goto search;
                        }
                        (void) printf("No entry for %s in section %c", name, sect);
                        if (subsec)
                                putchar(subsec);
                        (void) printf(" of the manual.\n");
                        return;
                }
        }
found:
        if (troffit)
                troff(work);
        else {
                FILE *it;
                char abuf[BUFSIZ];

                if (!nomore) {
                        if ((it = fopen(work, "r")) == NULL) {
                                perror(work);
                                (void) exit(1);
                        }
                        if (fgets(abuf, BUFSIZ-1, it) &&
                           abuf[0] == '.' && abuf[1] == 's' &&
                           abuf[2] == 'o' && abuf[3] == ' ') {
                                register char *cp = abuf+strlen(".so ");
                                char *dp;

                                while (*cp && *cp != '\n')
                                        cp++;
                                *cp = 0;
                                while (cp > abuf && *--cp != '/')
                                        ;
                                dp = ".so man";
                                if (cp != abuf+strlen(dp)+1) {
tohard:
                                        nomore = 1;
                                        (void) strcpy(work, abuf+4);
                                        goto hardway;
                                }
                                for (cp = abuf; *cp == *dp && *cp; cp++, dp++)
                                        ;
                                if (*dp)
                                        goto tohard;
                                (void) strcpy(work, cp-3);
                        }
                        (void) fclose(it);
                        (void) strcpy(work2, "cat");
                        (void) strcpy(work2+3, work+3);
                        work2[4] = 0;
                        if (stat(work2, &stbuf2) < 0)
                                goto hardway;
                        (void) strcpy(work2+3, work+3);
                        if (stat(work2, &stbuf2) < 0 || stbuf2.st_mtime < stbuf.st_mtime) {
                                (void) printf("Reformatting page.  Wait...");
                                (void) fflush(stdout);
                                (void) unlink(work2);
                                (void) sprintf(cmdbuf,
                        "%s %s > /tmp/man%d; trap '' 1 15; mv /tmp/man%d %s",
                                    NROFFCAT, work, getpid(), getpid(), work2);
                                if (msystem(cmdbuf)) {
                                        (void) printf(" aborted (sorry)\n");
                                        remove();
                                        /*NOTREACHED*/
                                }
                                (void) printf(" done\n");
                        }
                        (void) strcpy(work, work2);
                }
hardway:
                nroff(work);
        }
}

nroff(cp)
        char *cp;
{
        char cmd[BUFSIZ];

        if (cp[0] == 'c')
                (void) sprintf(cmd, "%s %s", nomore? CAT : MORE, cp);
        else
                (void) sprintf(cmd, nomore? "%s %s" : "%s %s|%s", NROFF, cp, MORE);
        (void) msystem(cmd);
}

troff(cp)
        char *cp;
{
        char cmdbuf[BUFSIZ];

        (void) sprintf(cmdbuf, TROFFCMD, cp);
        (void) msystem(cmdbuf);
}

any(c, sp)
        register int c;
        register char *sp;
{
        register int d;

        while (d = *sp++)
                if (c == d)
                        return (1);
        return (0);
}

remove()
{
        char name[15];

        (void) sprintf(name, "/tmp/man%d", getpid());
        (void) unlink(name);
        (void) exit(1);
}

unsigned
blklen(ip)
        register int *ip;
{
        register int i = 0;

        while (*ip++)
                i++;
        return (i);
}

apropos(argc, argv)
        int argc;
        char **argv;
{
        char buf[BUFSIZ];
        char *gotit;
        register char **vp;

        if (argc == 0) {
                (void) fprintf(stderr, "apropos what?\n");
                (void) exit(1);
        }
        if (freopen("/usr/local/lib/whatis", "r", stdin) == NULL) {
                perror("/usr/local/lib/whatis");
                (void) exit(1);
        }
        gotit = calloc(1, blklen((int *)argv));
        while (fgets(buf, sizeof buf, stdin) != NULL)
                for (vp = argv; *vp; vp++)
                        if (match(buf, *vp)) {
                                (void) printf("%s", buf);
                                gotit[vp - argv] = 1;
                                for (vp++; *vp; vp++)
                                        if (match(buf, *vp))
                                                gotit[vp - argv] = 1;
                                break;
                        }
        for (vp = argv; *vp; vp++)
                if (gotit[vp - argv] == 0)
                        (void) printf("%s: nothing apropriate\n", *vp);
}

match(buf, str)
        char *buf, *str;
{
        register char *bp;

        bp = buf;
        for (;;) {
                if (*bp == 0)
                        return (0);
                if (amatch(bp, str))
                        return (1);
                bp++;
        }
}

amatch(cp, dp)
        register char *cp, *dp;
{

        while (*cp && *dp && lmatch(*cp, *dp))
                cp++, dp++;
        if (*dp == 0)
                return (1);
        return (0);
}

lmatch(c, d)
        char c, d;
{

        if (c == d)
                return (1);
        if (!isalpha(c) || !isalpha(d))
                return (0);
        if (islower(c))
                c = toupper(c);
        if (islower(d))
                d = toupper(d);
        return (c == d);
}

whatis(argc, argv)
        int argc;
        char **argv;
{
        register char **avp;

        if (argc == 0) {
                (void) fprintf(stderr, "whatis what?\n");
                (void) exit(1);
        }
        if (freopen("/usr/local/lib/whatis", "r", stdin) == NULL) {
                perror("/usr/local/lib/whatis");
                (void) exit(1);
        }
        for (avp = argv; *avp; avp++)
                *avp = trim(*avp);
        whatisit(argv);
        (void) exit(0);
}

whatisit(argv)
        char **argv;
{
        char buf[BUFSIZ];
        register char *gotit;
        register char **vp;

        gotit = calloc(1, blklen((int *)argv));
        while (fgets(buf, sizeof buf, stdin) != NULL)
                for (vp = argv; *vp; vp++)
                        if (wmatch(buf, *vp)) {
                                (void) printf("%s", buf);
                                gotit[vp - argv] = 1;
                                for (vp++; *vp; vp++)
                                        if (wmatch(buf, *vp))
                                                gotit[vp - argv] = 1;
                                break;
                        }
        for (vp = argv; *vp; vp++)
                if (gotit[vp - argv] == 0)
                        (void) printf("%s: not found\n", *vp);
}

wmatch(buf, str)
        char *buf, *str;
{
        register char *bp, *cp;

        bp = buf;
again:
        cp = str;
        while (*bp && *cp && lmatch(*bp, *cp))
                bp++, cp++;
        if (*cp == 0 && (*bp == '(' || *bp == ',' || *bp == '\t' || *bp == ' '))
                return (1);
        while (isalpha(*bp) || isdigit(*bp))
                bp++;
        if (*bp != ',')
                return (0);
        bp++;
        while (isspace(*bp))
                bp++;
        goto again;
}

char *
trim(cp)
        register char *cp;
{
        register char *dp;

        for (dp = cp; *dp; dp++)
                if (*dp == '/')
                        cp = dp + 1;
        if (cp[0] != '.') {
                if (cp + 3 <= dp && dp[-2] == '.' && any(dp[-1], "cosa12345678npP"))
                        dp[-2] = 0;
                if (cp + 4 <= dp && dp[-3] == '.' && any(dp[-2], "13") && isalpha(dp[-1]))
                        dp[-3] = 0;
        }
        return (cp);
}

msystem(s)
char *s;
{
        int status, pid, w;

        if ((pid = fork()) == 0) {
                (void) execl("/bin/sh", "sh", "-c", s, 0);
                (void) _exit(127);
        }
        while ((w = wait(&status)) != pid && w != -1)
                ;
        if (w == -1)
                status = -1;
        return (status);
}
