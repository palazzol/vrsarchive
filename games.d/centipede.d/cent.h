#include <curses.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
       /* stdio.h and sgtty.h are included by curses.h */
#ifdef SYSV
#   include <fcntl.h>
#   include <sgtty.h>
#endif

#define FREEMAN 12000
#define CENTLENGTH 20

/* Things appearing on the screen */
#define HEAD 'O'
#define BODY 'o'
#define UNSHOTMUSHROOM 'P'
#define ONCESHOTMUSHROOM 'p'
#define TWICESHOTMUSHROOM '.'
#define UNSHOTPOISON 'X'
#define ONCESHOTPOISON 'x'
#define TWICESHOTPOISON ','
#define YOU '!'
#define SHOT '*'
#define FLEA '@'

#define UNPOISONED 0
#define POISONED 1
#define WASPOISONED 2


typedef struct {
    int y;
    int x;
    } COORD;

typedef struct pede {
    struct pede *next;  /* next pede in linked list of creatures */
    struct pede *prev;  /* previous pede in list */
    char type;          /* head or body */
    COORD pos;
    COORD oldpos;
    COORD speed;
    int overlap;        /* Did the piece overlap another last time? */
    int poisoned;       /* state of being poisoned */
    } PEDE;

extern PEDE *centipede, *lastpede;
extern char mushw[24][57];
extern int inter,stopped,dead,fired,finished,board,fleahere,
	fleashot,extramen,breeding,breedtime,moves,nummushrooms,
	scorphere,scorpthisboard,scorpvel,gameover,spiderhere,spiderdir,
	spidcount,gameover,author,gamestarted,numpedes,fleafreq,niceness;
#ifdef WIZARD
extern int author;
#endif
extern long score,nextman;
extern COORD guy,shot,flea,scorp,spider,spidervel;
extern char name[],rscorpion[],lscorpion[],*scorppic,*spiderpic[];
extern char in_map[];
#ifdef old_input
extern char fichar,upchar,dochar,lechar,richar,ulchar,urchar,dlchar,drchar,
	flchar,frchar,pachar;
#endif
extern char scorefile[],lockfile[],helpfile[];
extern struct sgttyb origterm;
extern float version;
extern double maxload;

extern PEDE *getpede();
extern char *addcommas(),*getlogin(),*getenv(),*index();
extern int catchint(),catchstop(),stopawhile(),quit(),catchalarm();
extern FILE *popen();

#define COMPSPOTS(s1,s2) ((s1).y == (s2).y && (s1).x == (s2).x)
#define ADDPIECE(piece) mvaddch((piece)->pos.y,(piece)->pos.x,(piece)->type)
#define ERASE(y,x) mvaddch(y,x,mushw[y][x])
