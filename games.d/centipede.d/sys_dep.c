/* Sys_dep.c:  System dependent constants */



/* Scorefile: Where the scorefile should live */
char scorefile[] = "/usr/games/lib/cent/scorefile";


/* Lockfile: Where to create and look for a lockfile controlling access
   to the scorefile */
char lockfile[] = "/usr/games/lib/cent/lockfile";


/* Helpfile: Where the help file is located */
char helpfile[] = "/usr/games/lib/cent/cent.doc";


/* Maxload: The maximum allowed load average for playing */
double maxload = 4.0;


/* Niceness: The amount to nice the game by (lower its priority) */
int niceness = 4;

/* def_pager: default pager if we can't otherwise choose one */
char *def_pager = "less";

/* def_pager_opts: options for default pager; include - if you need it */
char *def_pager_opts = "-qew";
