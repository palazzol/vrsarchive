/*
 * expire - expire daemon runs around and nails all articles that
 *		 have expired.
 *
 */

#ifndef lint
static char	*SccsId = "@(#)expire.c	2.31	9/3/84";
#endif lint

#include "params.h"
#include <errno.h>
#if defined(BSD4_2) || defined(BSD4_1C)
# include <sys/dir.h>
#else
# include "ndir.h"
#endif

char *Progname = "expire";	/* used by xerror to identify failing program */

/*	Number of array entries to allocate at a time.	*/
#define SPACE_INCREMENT	1000

extern char	groupdir[BUFSIZ], rcbuf[BUFLEN];
extern int	errno;
char	NARTFILE[BUFSIZ], OARTFILE[BUFSIZ];
char	PAGFILE[BUFLEN], DIRFILE[BUFLEN];
char	NACTIVE[BUFSIZ], OACTIVE[BUFSIZ];
extern char *OLDNEWS;
int	verbose = 0;
int	ignorexp = 0;
int	doarchive = 0;
int	nohistory = 0;
int	dorebuild = 0;
int	usepost = 0;
int	frflag = 0;
int	updateactive = 0;
char	baduser[BUFLEN], filename[BUFLEN];

/*
 * This code uses realloc to get more of the multhist array.
 */
struct multhist {
	char	*mh_ident;
	char	*mh_file;
} *multhist;
unsigned int mh_size;
char *calloc();
char *realloc();

typedef struct {
	char *dptr;
	int dsize;
} datum;

long	expincr;
long	atol();
time_t	cgtdate(), time();
FILE *popen();
struct passwd *pw;
struct group *gp;
char	arpat[LBUFLEN];

main(argc, argv)
int	argc;
char	**argv;
{
	register FILE *fp = NULL;
	struct hbuf h;
	register time_t now, newtime;
	char	ngpat[LBUFLEN];
	char	afline[BUFLEN];
	char	*p1, *p2, *p3;
	FILE	*ohfd, *nhfd;
	DIR	*ngdirp;
	static struct direct *ngdir;
	char fn[BUFLEN];
	int uid, gid, duid, dgid;
	int i;

	pathinit();
	umask(N_UMASK);

	uid = getuid();
	gid = getgid();
	duid = geteuid();
	dgid = getegid();
	if (uid == 0 && geteuid() == 0) {
		/*
		 * Must go through with this kludge since
		 * some systems do not honor the setuid bit
		 * when root invokes a setuid program.
		 */
		if ((pw = getpwnam(NEWSUSR)) == NULL)
			xerror("Cannot get NEWSUSR pw entry");

		duid = pw->pw_uid;
		if ((gp = getgrnam(NEWSGRP)) == NULL)
			xerror("Cannot get NEWSGRP gr entry");
		dgid = gp->gr_gid;
		setuid(duid);
		setgid(dgid);
	}

	expincr = DFLTEXP;
	ngpat[0] = '\0';
	while (argc > 1) {
		switch (argv[1][1]) {
		case 'v':
			if (isdigit(argv[1][2]))
				verbose = argv[1][2] - '0';
			else if (argc > 2 && argv[2][0] != '-') {

				argv++;
				argc--;
				verbose = atoi(argv[1]);
			} else
				verbose = 1;
			if (verbose < 3)
				setbuf(stdout, (char *)NULL);
			break;
		case 'e':	/* Use this as default expiration time */
			if (argc > 2 && argv[2][0] != '-') {
				argv++;
				argc--;
				expincr = atol(argv[1]) * DAYS;
			} else if (isdigit(argv[1][2]))
				expincr = atol(&argv[1][2]) * DAYS;
			break;
		case 'I':	/* Ignore any existing expiration date */
			ignorexp = 2;
			break;
		case 'i':	/* Ignore any existing expiration date */
			ignorexp = 1;
			break;
		case 'n':
			if (argc > 2) {
				argv++;
				argc--;
				while (argc > 1 && argv[1][0] != '-') {
					strcat(ngpat, argv[1]);
					ngcat(ngpat);
					argv++;
					argc--;
				}
				argv--;
				argc++;
			}
			break;
		case 'a':	/* archive expired articles */
			if (access(OLDNEWS,0) < 0 ){
				perror(OLDNEWS);
				fprintf(stderr,"No archiving possible\n");
				xxit(1);
			}
			doarchive++;
			if (argc > 2) {
				argv++;
				argc--;
				while (argc > 1 && argv[1][0] != '-') {
					strcat(arpat, argv[1]);
					ngcat(arpat);
					argv++;
					argc--;
				}
				argv--;
				argc++;
			}
			break;
		case 'h':	/* ignore history */
			nohistory++;
			break;
		case 'r':	/* rebuild history file */
			dorebuild++;
			nohistory++;
			break;
		case 'p':
			usepost++;
			break;
		case 'f':
			frflag++;
			if (argc > 2) {
				strcpy(baduser, argv[2]);
				argv++;
				argc--;
			}
			break;
		case 'u':
			updateactive++;
			break;
		default:
			printf("Usage: expire [ -v [level] ] [-e days ] [-i] [-a] [-r] [-h] [-p] [-u] [-f username] [-n newsgroups]\n");
			xxit(1);
		}
		argc--;
		argv++;
	}
	if (ngpat[0] == 0)
		strcpy(ngpat, "all,");
	if (arpat[0] == 0)
		strcpy(arpat, "all,");
	now = time((time_t)0);
	if (chdir(SPOOL))
		xerror("Cannot chdir %s", SPOOL);

	if (verbose) {
		printf("expire: nohistory %d, rebuild %d, doarchive %d\n",
			nohistory, dorebuild, doarchive);
		printf("newsgroups: %s\n",ngpat);
		if (doarchive)
			printf("archiving: %s\n",arpat);
	}

	sprintf(OARTFILE, "%s/%s", LIB, "ohistory");
	sprintf(NARTFILE, "%s/%s", LIB, "nhistory");

	sprintf(OACTIVE, "%s/%s", LIB, "oactive");
	sprintf(NACTIVE, "%s/%s", LIB, "nactive");

	if (updateactive)
		goto doupdateactive;

#ifdef DBM
	if (!dorebuild)
	{
		sprintf(PAGFILE, "%s/%s", LIB, "nhistory.pag");
		sprintf(DIRFILE, "%s/%s", LIB, "nhistory.dir");
		close(creat(PAGFILE, 0666));
		close(creat(DIRFILE, 0666));
 		dbminit(NARTFILE);
	}
#endif

	if (nohistory) {
		ohfd = xfopen(ACTIVE, "r");
		if (dorebuild) {
			/* Allocate initial space for multiple newsgroup (for an
			   article) array */
			multhist = (struct multhist *)
				calloc (SPACE_INCREMENT,
					sizeof (struct multhist));
			mh_size = SPACE_INCREMENT;

			sprintf(afline, "sort -t\t +1 >%s", NARTFILE);
			if ((nhfd = popen(afline, "w")) == NULL)
				xerror("Cannot exec %s", NARTFILE);
		} else
			nhfd = xfopen("/dev/null", "w");
	} else {
		ohfd = xfopen(ARTFILE, "r");
		nhfd = xfopen(NARTFILE, "w");
	}

	for(i=0;i<NUNREC;i++)
		h.unrec[i] = NULL;

	while (TRUE) {
		if (nohistory) {
			do {
				if (ngdir == NULL) {
					if ( ngdirp != NULL )
						closedir(ngdirp);
					if (fgets(afline, BUFLEN, ohfd) == NULL)
						goto out;
					strcpy(groupdir, afline);
					p1 = index(groupdir, ' ');
					if (p1 == NULL)
						p1 = index(groupdir, '\n');
					if (p1 != NULL)
						*p1 = NULL;
					if (!ngmatch(groupdir, ngpat))
						continue;

					/* Change a group name from
					   a.b.c to a/b/c */
					for (p1=groupdir; *p1; p1++)
						if (*p1 == '.')
							*p1 = '/';

					if ((ngdirp = opendir(groupdir)) == NULL)
						continue;

				}
				ngdir = readdir(ngdirp);
			/*	Continue looking if not an article.	*/
			} while ( ngdir == NULL || !islegal(fn,groupdir,ngdir->d_name));

			p2 = fn;
			if (verbose > 2)
				printf("article: %s\n", fn);
		} else {
			if (fgets(afline, BUFLEN, ohfd) == NULL)
				break;
			if (verbose > 2)
				printf("article: %s", afline);
			p1 = index(afline, '\t');
			if (p1)
				p2 = index(p1 + 1, '\t');
			else
				continue;
			if (!p2)
				continue;
			p2++;
			strcpy(groupdir, p2);
			p3 = index(groupdir, '/');
			if (p3)
				*p3 = 0;
			else {
				/*
				 * Nothing after the 2nd tab.  This happens
				 * when a control message is stored in the
				 * history file.  Use the date in the history
				 * file to decide expiration.
				 */
				h.expdate[0] = 0;
				strcpy(h.recdate, p1+1);
				goto checkdate;
			}
			if (!ngmatch(groupdir, ngpat)) {
				fputs(afline, nhfd);
				*p1 = 0;
				remember(afline);
				continue;
			}
			strcpy(fn, p2);
			p1 = index(fn, ' ');
			if (p1 == 0)
				p1 = index(fn, '\n');
			if (p1)
				*p1 = 0;
		}

		strcpy(filename, dirname(fn));
		if (access(filename, 4)
		|| (fp = fopen(filename, "r")) == NULL) {
			if (verbose)
				printf("Can't open %s.\n", filename);
			continue;
		}
		for(i=0;i<NUNREC;i++)
			if (h.unrec[i] != NULL)
				free(h.unrec[i]);
			else
				break;
		if (hread(&h, fp, TRUE) == NULL) {
			printf("Garbled article %s.\n", filename);
			fclose(fp);
			continue;
		}
		if (dorebuild) {
			register char	*cp;
			register struct multhist *mhp;

			/* Format of filename until now was /SPOOL/a/b/c.4
			   and this code changes it to a.b.c/4 (the correct
			   kind of entry in the history file.  */
			strcpy (filename, filename + strlen(SPOOL)+1);
			for (p1 = filename; p1 != NULL && *p1 != '\0'; p1++)
				if (*p1 == '/' && p1 != rindex (p1, '/'))
					*p1 = '.';

			if ((cp = index(h.nbuf, NGDELIM)) == NULL) {
saveit:
				fprintf(nhfd, "%s\t%s\t%s \n", h.ident, h.recdate, filename);
				fclose(fp);
				continue;
			}
			for (mhp = multhist; mhp < multhist+mh_size && mhp->mh_ident != NULL; mhp++) {
				if (mhp->mh_file == NULL)
					continue;
				if (strcmp(mhp->mh_ident, h.ident) != 0)
					continue;
				if (index(mhp->mh_file, ' ') != NULL)
					cp = index(++cp, NGDELIM);
				strcat(filename, " ");
				strcat(filename, mhp->mh_file);
				free(mhp->mh_file);
				mhp->mh_file = NULL;
				if (*cp == NULL || (cp = index(++cp, NGDELIM)) == NULL)
					goto saveit;
				else
					break;
			}

			/* Here is where we realloc the multhist space rather
			   than the old way of static allocation.  It's
			   really trivial.  We just clear out the space
			   in case it was reused.  The old static array was
			   guaranteed to be cleared since it was cleared when
			   the process started.  */
			if (mhp >= multhist + mh_size) {
				multhist = (struct multhist *)
					realloc (multhist,
					  sizeof (struct multhist) *
					  (SPACE_INCREMENT + mh_size));
				if (multhist == NULL)
					xerror("Too many articles with multiple newsgroups");
				for (mhp = multhist + mh_size;
				  mhp < multhist+mh_size+SPACE_INCREMENT;
					mhp++) {
					mhp->mh_ident = NULL;
					mhp->mh_file = NULL;
				}
				mhp = multhist + mh_size;
				mh_size += SPACE_INCREMENT;
			}

			mhp->mh_ident = malloc(strlen(h.ident)+1);
			strcpy(mhp->mh_ident, h.ident);
			cp = malloc(strlen(filename) + 1);
			if ( cp == NULL)
				xerror("Out of memory");
			strcpy(cp, filename);
			mhp->mh_file = cp;
			fclose(fp);
			continue;
		}

		fclose(fp);
checkdate:
		if (h.expdate[0])
			h.exptime = cgtdate(h.expdate);
		newtime = cgtdate(usepost ? h.subdate : h.recdate) + expincr;
		if (!h.expdate[0] || ignorexp == 2 ||
		    (ignorexp == 1 && newtime < h.exptime))
			h.exptime = newtime;
		if (frflag ? strcmp(baduser,h.from)==0 : now >= h.exptime) {
#ifdef DEBUG
			printf("cancel %s\n", filename);
#else
			if (verbose)
				printf("cancel %s\n", filename);
			ulall(p2, &h);
#endif
		} else {
			fputs(afline, nhfd);
			if (!dorebuild)
				remember(h.ident);
			if (verbose > 2)
				printf("Good article %s\n", rcbuf);
		}
	}

out:
	if (dorebuild) {
		register struct multhist *mhp;
		for (mhp = multhist; mhp < multhist+mh_size && mhp->mh_ident != NULL; mhp++)
			/* should "never" happen */
			if (mhp->mh_file != NULL ) {
				printf("Article: %s %s Cannot find all links\n", mhp->mh_ident, mhp->mh_file);
				sprintf(filename,"%s/%s",SPOOL,mhp->mh_file);
				for (p1 = filename; *p1 != ' ' && *p1 != '\0'; p1++)
					if (*p1 == '.')
						*p1 = '/';
				*p1 = '\0';
				if ((fp = fopen(filename, "r")) == NULL) {
					if (verbose)
						printf("Can't open %s.\n", filename);
					continue;
				}
				for(i=0;i<NUNREC;i++)
					if (h.unrec[i] != NULL)
						free(h.unrec[i]);
					else
						break;
				if (hread(&h, fp, TRUE) == NULL) {
					printf("Garbled article %s.\n", filename);
					fclose(fp);
					continue;
				}

				fprintf(nhfd, "%s\t%s\t%s \n", h.ident, h.recdate, mhp->mh_file);
				fclose(fp);
				continue;
			}
		pclose(nhfd);
		free (multhist);
	}

	if (dorebuild || !nohistory) {
		rename(ARTFILE, OARTFILE);
		rename(NARTFILE, ARTFILE);
#ifdef DBM
		if (dorebuild)
			rebuilddbm ( );
		else
		{
			char bfr[BUFLEN];
			sprintf(bfr,"%s.pag", ARTFILE);
			strcat(OARTFILE, ".pag");
			strcat(NARTFILE, ".pag");
			rename(bfr, OARTFILE);
			rename(NARTFILE, bfr);
			sprintf(bfr,"%s.dir", ARTFILE);
			strcpy(rindex(OARTFILE, '.'), ".dir");
			strcpy(rindex(NARTFILE, '.'), ".dir");
			rename(bfr, OARTFILE);
			rename(NARTFILE, bfr);
		}
#endif
	}

doupdateactive:
	ohfd = xfopen(ACTIVE, "r");
	nhfd = xfopen(NACTIVE, "w");
	do {
		long n;
		long maxart, minart;
		char cansub;
		int gdsize;
		struct stat stbuf;

		if (fgets(afline, BUFLEN, ohfd) == NULL)
			continue;
		if (sscanf(afline,"%s %ld %ld %c",groupdir,&maxart, &minart,
		    &cansub) < 4)
			xerror("Active file corrupt");
		minart = maxart > 0 ? maxart : 1L;
		/* Change a group name from
		   a.b.c to a/b/c */
		for (p1=groupdir; *p1; p1++)
			if (*p1 == '.')
				*p1 = '/';

		gdsize = strlen(groupdir);
		if ((ngdirp = opendir(groupdir)) != NULL) {
			while (ngdir = readdir(ngdirp)) {
				groupdir[gdsize] = '/';
				strcpy(&groupdir[gdsize+1],ngdir->d_name);
				/* We have to do a stat because of micro.6809 */
				if (stat(groupdir,&stbuf) < 0 ||
				   !(stbuf.st_mode&S_IFREG) )
					continue;
				n = atol(ngdir->d_name);
				if (n > 0 && n < minart)
					minart = n;
				if (n > 0 && n > maxart)
					maxart = n;
			}
			closedir(ngdirp);
		}
		afline[gdsize] = '\0';
		fprintf(nhfd,"%s %05ld %05ld %c\n", afline, maxart, minart, cansub);
	} while ( !feof(ohfd));
	fclose(nhfd);
	fclose(ohfd);

	rename(ACTIVE, OACTIVE);
	rename(NACTIVE, ACTIVE);

	xxit(0);
}

/* Unlink (using tail recursion) all the articles in 'artlist'. */
ulall(artlist, h)
char	*artlist;
struct hbuf *h;
{
	register char	*p, *q;
	int	last = 0;
	char	newname[BUFLEN];
	time_t	timep[2];
	char *fn;

	if (verbose > 2)
		printf("ulall '%s', '%s'\n", artlist, h->subdate);
	if (nohistory) {
		last = 1;
	} else {
		while (*artlist == ' ' || *artlist == '\n' || *artlist == ',')
			artlist++;
		if (*artlist == 0)
			return;
		p = index(artlist, ' ');
		if (p == 0) {
			last = 1;
			p = index(artlist, '\n');
		}
		if (p == 0) {
			last = 1;
			p = index(artlist, ',');
		}
		if (p == 0) {
			last = 1;
			fn = dirname(artlist);
			unlink(fn);
			return;
		}
		if (p)
			*p = 0;
	}
	fn = dirname(artlist);
	if (doarchive){
		strcpy(newname, artlist);
		q = index(newname,'/');
		if (q) {
			*q++ = NGDELIM;
			*q = '\0';
		}
		if (ngmatch(newname, arpat)) {
			q = fn + strlen(SPOOL) + 1;
			sprintf(newname, "%s/%s", OLDNEWS, q);
			if (verbose)
				printf("link %s to %s\n", fn, newname);
			if (link(fn, newname) == -1) {
				if (mkparents(newname) == 0)
					if (link(fn, newname) == -1)
						fcopy(fn, newname);
			}
			timep[0] = timep[1] = cgtdate(h->subdate);
			utime(newname, timep);
		}
	}
	if (verbose)
		printf("unlink %s\n", fn);
	unlink(fn);
	if (!last)
		ulall(p + 1, h);
}

fcopy(fn, newname)
char *fn, *newname;
{
	int f1, f2;
	int r;
	char buf[BUFSIZ];
	f1 = open(fn, 0);
	if (f1 < 0)
		return -1;
	f2 = open(newname, 1);
	if (f2 < 0) {
		if (errno == ENOENT) {
			f2 = creat(newname,0644);
			if (f2 < 0)
				return -1;
		} else
			return -1;
	}
	while((r=read(f1, buf, BUFSIZ)) > 0)
		write(f2, buf, r);
	close(f1);
	close(f2);
	return 0;
}

/*
 * If any parent directories of this dir don't exist, create them.
 */
mkparents(dirname)
char *dirname;
{
	char buf[200], sysbuf[200];
	register char *p;
	int rc;
	struct passwd *pw;

	strcpy(buf, dirname);
	p = rindex(buf, '/');
	if (p)
		*p = '\0';
	if (access(buf,0) == 0)
		return 0;
	mkparents(buf);
	sprintf(sysbuf, "mkdir %s", buf);
	rc = system(sysbuf);
	sprintf(sysbuf, "%s", buf);
	if (verbose)
		printf("mkdir %s, rc %d\n", sysbuf, rc);
	chmod(sysbuf, 0755);
	chown(sysbuf, duid, dgid);

	return rc;
}


/*	Make sure this file is a legal article. */
islegal(fullname, path, name)
	register char *fullname;
	register char *path;
	register char *name;
{
	struct stat buffer;

	sprintf(fullname, "%s/%s", path, name);

	/* make sure the article is numeric. */
	while (*name != '\0')
		if (!isascii(*name) || !isdigit(*name))
			return 0;
		else
			name++;

	/*  Now make sure we don't have a group like net.micro.432,
	 *  which is numeric but not a regular file -- i.e., check
	 *  for being a regular file.
	 */
	if ((stat(fullname, &buffer) == 0) &&
		((buffer.st_mode & S_IFMT) == S_IFREG)) {
		/* Now that we found a legal group in a/b/c/4
		   notation, switch it to a.b.c/4 notation.  */
		for (name = fullname; name != NULL && *name != '\0'; name++)
			if (*name == '/' && name != rindex (name, '/'))
				*name = '.';

			return 1;
	}
	return 0;
}

#ifdef DBM
/*
 * This is taken mostly intact from ../cvt/cvt.hist.c and is used at the
 * end by the options that make a new history file.
 * Routine to convert history file to dbm file.  The old 3 field
 * history file is still kept there, because we need it for expire
 * and for a human readable copy.  But we keep a dbm hashed copy
 * around by message ID so we can answer the yes/no question "have
 * we already seen this message".  The content is the ftell offset
 * into the real history file when we get the article - you can't
 * really do much with this because the file gets compacted.
 */

FILE *fd;

char namebuf[BUFSIZ];
char lb[BUFSIZ];

rebuilddbm( )
{
	register char *p;
	long fpos;
	datum lhs, rhs;

	umask(0);
	sprintf(namebuf, "%s.dir", ARTFILE);
	close(creat(namebuf, 0666));
	sprintf(namebuf, "%s.pag", ARTFILE);
	close(creat(namebuf, 0666));
	sprintf(namebuf, "%s", ARTFILE);

	fd = fopen(namebuf, "r");
	if (fd == NULL) {
		perror(namebuf);
		xxit(2);
	}

	dbminit(namebuf);
	while (fpos=ftell(fd), fgets(lb, BUFSIZ, fd) != NULL) {
		p = index(lb, '\t');
		if (p)
			*p = 0;
		p = lb;
		while (*++p)
			if (isupper(*p))
				*p = tolower(*p);
		lhs.dptr = lb;
		lhs.dsize = strlen(lb) + 1;
		rhs.dptr = (char *) &fpos;
		rhs.dsize = sizeof fpos;
		if (store(lhs, rhs) < 0)
			fprintf(stderr, "store(%s) failed\n", lb);
	}
}
#endif DBM

remember(article)
	char	*article;
{
	static long number;
	register char *rcp;
	datum	lhs, rhs;

#ifdef DBM
	strcpy(lb, article);
	rcp = lb;
	while (*++rcp)
		if (isupper(*rcp))
			*rcp = tolower(*rcp);
	lhs.dptr = lb;
	lhs.dsize = strlen(lb)+1;

	number++;

	rhs.dptr = (char *) &number;
	rhs.dsize = sizeof(number);

	store(lhs, rhs);
#endif
}

#if !defined(BSD4_2) && !defined(BSD4_1C)
rename(from,to)
register char *from, *to;
{
	unlink(to);
	if (link(from, to) < 0)
		return -1;

	unlink(from);
	return 0;
}
#endif !BSD4_2 && ! BSD4_1C

xxit(i)
{
	exit(i);
}
