/* Sys_dep.c:  System dependent constants */



/* Scorefile: Where the scorefile should live */
char scorefile[] = "/usr/games/centipede/scores";


/* Lockfile: Where to create and look for a lockfile controlling access
   to the scorefile */
char lockfile[] = "/usr/games/centipede/lockfile";


/* Helpfile: Where the help file is located */
char helpfile[] = "/usr/games/centipede/cent.doc";


/* Maxload: The maximum allowed load average for playing */
double maxload = 4.0;


/* Niceness: The amount to nice the game by (lower its priority) */
int niceness = 4;
