/* pathalias -- by steve bellovin, as told to peter honeyman */
#ifndef lint
static char	*sccsid = "@(#)extern.c	7.1 (down!honey) 85/08/06";
#endif lint

#include "def.h"

node	*Home;
int	Fcnt = -1;
char	*Cfile;
char	**Ifiles;
char	*ProgName;
int	Vflag;
int	Cflag;
int	Iflag;
int	Lineno = 1;
char	*Netchars = "!:@%";	/* sparse, but sufficient */
int	Ncount;
int	Lcount;
char	*Graphout;
char	*Linkout;
node	*Private;		/* list of private nodes in this file */
int	Hashpart;		/* used while mapping -- above is heap */
int	Scanstate = NEWLINE;	/* scanner (yylex) state */
