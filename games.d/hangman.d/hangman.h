# include	<curses.h>
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<ctype.h>
# include	<signal.h>

# define	MINLEN	6
# define	MAXERRS	7
#ifdef __STDC__
#define STR(x)		#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(GAMLIB) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(GAMLIB)/x"
#endif
# define	DICT	FILENM(words)

# define	MESGY	12
# define	MESGX	0
# define	PROMPTY	11
# define	PROMPTX	0
# define	KNOWNY	10
# define	KNOWNX	1
# define	NUMBERY	4
# define	NUMBERX	(COLS - 1 - 26)
# define	AVGY	5
# define	AVGX	(COLS - 1 - 26)
# define	GUESSY	2
# define	GUESSX	(COLS - 1 - 26)


typedef struct {
	short	y, x;
	char	ch;
} ERR_POS;

extern bool	Guessed[];

extern char	Word[], Known[], *Noose_pict[];

extern int	Errors, Wordnum;

extern double	Average;

extern ERR_POS	Err_pos[];

extern FILE	*Dict;

extern off_t	Dict_size;

extern SIG_T	die();

off_t	abs();

#ifndef MKCTRL
#  define MKCTRL(c)	(c&~0140)
#endif
