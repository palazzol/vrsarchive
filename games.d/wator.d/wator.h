/*
 * Header information for wator.  Two user definable variables of note:
 *   FOUR2  -- define this to use 4.2 routines (random number generation)
 *   CHECK8 -- look in 8 adjacent cells instead of 4 (see fill_adj)
 */

#define FOUR2		1		/* use 4.2 routines */
#define CHECK8		1		/* look in 8 cells instead of 4 */

#define EMPTY		0
#define SHARK		1
#define FISH		2

#define W_MAX		80		/* width - max lines supported */
#define L_MAX		24		/* lines - max columns supported */

struct	item {
	struct	item *prev;		/* previous item in linked list */
	struct	item *next;		/* next item in list */
	unsigned char xloc;		/* position -- x (column) */
	unsigned char yloc;		/* position -- y (line) */
	unsigned char age;		/* how long since breeding */
	unsigned char eat;		/* how long since eating */
};

struct cell {
	int	item_type;
	struct	item *item_ptr;
} screen[W_MAX][L_MAX-1];

struct	item *active_shark;
struct	item *active_fish;
struct	item *free_list;

#define NULL_PTR	((struct item *) NULL)

int	num_fish;			/* number fish this generation */
int	num_shark;			/* number sharks this generation */
int	f_breed;			/* age at which fish breed */
int	s_breed;			/* age at which sharks breed */
int	starve;				/* how often must sharks eat */
int	generation;			/* current generation */
int	lines;				/* # lines on display */
int	wide;				/* # columns on display */
int	seed;				/* seed for random # generator */
char	*C_move;			/* string for cursor movement */
#ifdef FOUR2
char	state[128];			/* for random num generator */
#endif

FILE *stat;				/* used to accumulate stats */
