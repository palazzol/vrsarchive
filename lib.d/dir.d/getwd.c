/*
 * getwd - get working directory
 *
 * Probably should use lstat rather than stat throughout, if run on
 * a system with symbolic links.
 */

#include <stdio.h>
#include <sys/types.h>
#include <dir.h>
#include <sys/stat.h>

static char *trygetwd();
static prepend();

/*
 * getwd - master control
 */
char *
getwd(pathname)
register char pathname[];
{
	register char *ret;
	register FILE *pwd;
	extern FILE *popen();

	ret = trygetwd(pathname);
	if (ret != NULL)
		return(pathname);

	/*
	 * The simple approach failed.  Try doing it the hard way.
	 */
	pwd = popen("PATH=/bin:/usr/bin pwd", "r");
	ret = fgets(pathname, 1024, pwd);
	pclose(pwd);
	if (ret != NULL) {
		pathname[strlen(pathname)-1] = '\0';	/* Junk the \n. */
		return(pathname);
	}

	/*
	 * Total failure.
	 */
	strcpy(pathname, "getwd-failed");
	return(NULL);
}

/*
 * trygetwd - try to get the path without resorting to extreme measures
 */
static char *
trygetwd(pathname)
char pathname[];
{
	char parent[1024];		/* ../../.. and so forth. */
	char *parend;			/* See comment where used. */
	register DIR *par;
	register struct direct *direntry;
	struct stat parstat;
	struct stat maybe;
	register int statall;		/* Looking for a mounted fs? */
	ino_t lastino;
	dev_t lastdev;
	ino_t rootino;
	dev_t rootdev;

	if (stat(".", &parstat) < 0)
		return(NULL);
	lastino = parstat.st_ino;
	lastdev = parstat.st_dev;
	if (stat("/", &parstat) < 0)
		return(NULL);
	rootino = parstat.st_ino;
	rootdev = parstat.st_dev;
	strcpy(parent, "..");
	pathname[0] = '\0';

	/*
	 * Build up the pathname, ascending one level of
	 * directory on each iteration.
	 */
	while (lastino != rootino || lastdev != rootdev) {
		if (stat(parent, &parstat) < 0)
			return(NULL);

		/*
		 * Scan directory, looking for an inode-number match with
		 * the child directory.  There are two tricky cases:
		 *
		 * First, getting an inode-number match is not sufficient,
		 * because inode numbers are unique only within a filesystem.
		 * We must check that a promising-looking directory entry
		 * really does point to the place we came up from.  So we
		 * use stat() to verify number matches.
		 *
		 * Second, getting an inode-number match is not necessary
		 * either, because the directory entry for the top of a
		 * mounted filesystem carries the inode number of the place
		 * where the filesystem is mounted, so the entry doesn't
		 * look like it's for the-place-we-came-from until we do
		 * a stat.  So if we run out of directory entries without
		 * finding the child, we go through again statting everything.
		 */
		par = opendir(parent);
		if (par == NULL)
			return(NULL);
		statall = 0;
		for (;;) {
			direntry = readdir(par);
			if (direntry == NULL && statall)
				return(NULL);	/* Both passes failed. */
			if (direntry == NULL && !statall) {
				/* Maybe we've hit a mount boundary... */
				rewinddir(par);
				statall = 1;
				direntry = readdir(par);
			}
			if (direntry->d_ino == lastino || statall) {
				/*
				 * Use stat to check things out.  Build
				 * a suitable pathname on the end of the
				 * "parent" string, remembering where it
				 * ended so we can put it back later.
				 */
				parend = parent + strlen(parent);
				strcat(parent, "/");
				strcat(parent, direntry->d_name);
				if (stat(parent, &maybe) < 0)
					return(NULL);
				*parend = '\0';
				if (maybe.st_dev == lastdev && maybe.st_ino == lastino)
					break;		/* Found child! */
			}
		}
		if (pathname[0] != '\0')
			prepend(direntry->d_name, pathname);
		else
			strcpy(pathname, direntry->d_name);
		closedir(par);

		lastino = parstat.st_ino;
		lastdev = parstat.st_dev;
		strcat(parent, "/..");
	}

	prepend("", pathname);	/* Supply leading slash. */
	return(pathname);
}

/*
 * prepend - prepend a new component to a filename, with / in between
 */
static
prepend(cpt, name)
char *cpt;
char *name;
{
	char tmpname[1024];

	strcpy(tmpname, name);
	strcpy(name, cpt);
	strcat(name, "/");
	strcat(name, tmpname);
}

#ifdef TESTING

main()
{
	char buf[1024];
	printf("%s\n", getwd(buf));
}

#endif
