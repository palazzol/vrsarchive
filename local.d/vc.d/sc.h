/*	SC	A Table Calculator
 *		Common definitions
 *
 *		original by James Gosling, September 1982
 *		modified by Mark Weiser and Bruce Israel,
 *			University of Maryland
 *		R. Bond  12/86
 *
 */



#define MAXROWS 200
#define MAXCOLS 40
#define RESCOL 4  /* columns reserved for row numbers */
#define RESROW 3  /* rows reserved for prompt, error, and column numbers */
#define error move(1,0), clrtoeol(), (void) printw

struct ent_ptr {
    int vf;
    struct ent *vp;
};

struct range_s {
	struct ent_ptr left, right;
};

/*
 * If you want to save room, make row and col below into unsigned
 * chars and make sure MAXROWS and MAXCOLS above are both less
 * than 256.  (128 if your compiler doesn't support unsigned char).
 *
 * Some not too obvious things about the flags:
 *    is_valid means there is a valid number in v.
 *    label set means it points to a valid constant string.
 *    is_strexpr set means expr yields a string expression.
 *    If is_strexpr is not set, and expr points to an expression tree, the
 *        expression yields a numeric expression.
 *    So, either v or label can be set to a constant. 
 *        Either (but not both at the same time) can be set from an expression.
 */

#define VALID_CELL(p, r, c) ((p = tbl[r][c])&&((p->flags&is_valid)||p->label))

struct ent {
    double v;
    char *label;
    struct enode *expr;
    short flags;
    short row, col;
    struct ent *next;
};

struct range {
    struct ent_ptr r_left, r_right;
    char *r_name;
    struct range *r_next, *r_prev;
    int r_is_range;
};

#define FIX_ROW 1
#define FIX_COL 2

struct enode {
    int op;
    union {
	double k;
	struct ent_ptr v;
	struct range_s r;
	char *s;
	struct {
	    struct enode *left, *right;
	} o;
    } e;
};

/* op values */
#define O_VAR 'v'
#define O_CONST 'k'
#define O_SCONST '$'
#define O_REDUCE(c) (c+0200)

#define ACOS 0
#define ASIN 1
#define ATAN 2
#define CEIL 3
#define COS 4
#define EXP 5 
#define FABS 6 
#define FLOOR 7
#define HYPOT 8
#define LOG 9
#define LOG10 10
#define POW 11
#define SIN 12
#define SQRT 13
#define TAN 14
#define DTR 15
#define RTD 16
#define MIN 17
#define MAX 18
#define RND 19
#define HOUR 20
#define MINUTE 21
#define SECOND 22
#define MONTH 23
#define DAY 24
#define YEAR 25
#define NOW 26
#define DATE 27
#define FMT 28
#define SUBSTR 29
#define STON 30
#define EQS 31

/* flag values */
#define is_valid     0001
#define is_changed   0002
#define is_strexpr   0004
#define is_leftflush 0010
#define is_deleted   0020

#define ctl(c) (c&037)

extern struct ent *tbl[MAXROWS][MAXCOLS];

extern int strow, stcol;
extern int currow, curcol;
extern int savedrow, savedcol;
extern int FullUpdate;
extern int maxrow, maxcol;
extern int fwidth[MAXCOLS];
extern int precision[MAXCOLS];
extern char col_hidden[MAXCOLS];
extern char row_hidden[MAXROWS];
extern char line[1000];
extern int linelim;
extern int changed;
extern struct ent *to_fix;
extern int showsc, showsr;
extern struct enode *new();
extern struct enode *new_const();
extern struct enode *new_var();
extern struct enode *new_str();
extern struct enode *new_range();
extern struct ent *lookat();
extern struct enode *copye();
extern char *coltoa();
extern FILE *openout();
extern struct range *find_range();
extern char *v_name();
extern char *r_name();
extern double eval();
extern char *seval();
extern int modflg;
extern int Crypt;
extern char *mdir;
extern char *xmalloc();
extern int xfree();

#if BSD42 || SYSIII

#ifndef cbreak
#define	cbreak		crmode
#define	nocbreak	nocrmode
#endif

#endif

#ifdef SYS5
#define SYSV3	1
#endif
