#define	NULL	0
extern char	**environ;
static char	*stdenv[] = {
	"PATH=/bin:/usr/bin",
	"IFS= \t\n",
	NULL
};

standard()
{
	environ = stdenv;
	closeall(1);
}

safe()
{
	setgid(getgid());
	setuid(getuid());
	closeall(1);
}
