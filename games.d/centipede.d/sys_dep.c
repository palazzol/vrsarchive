/* Sys_dep.c:  System dependent constants */


#ifdef __STDC__
#define STR(x)	#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(GAMLIB) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMLIB)/x"
#endif

/* Scorefile: Where the scorefile should live */
char scorefile[] = FILENM(scorefile);


/* Lockfile: Where to create and look for a lockfile controlling access
   to the scorefile */
char lockfile[] = FILENM(lockfile);


/* Helpfile: Where the help file is located */
char helpfile[] = FILENM(cent.doc);


/* Maxload: The maximum allowed load average for playing */
double maxload = 4.0;


/* Niceness: The amount to nice the game by (lower its priority) */
int niceness = 4;

/* def_pager: default pager if we can't otherwise choose one */
char *def_pager = "more";

/* def_pager_opts: options for default pager; include - if you need it */
char *def_pager_opts = "";
