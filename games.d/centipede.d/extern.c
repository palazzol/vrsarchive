#include "cent.h"

float version = 1.7;
PEDE *centipede;                /* head of the list */
PEDE *lastpede;                 /* last pede in list */
int numpedes;
char mushw[24][57];             /* Array to store mushrooms */
struct sgttyb origterm;         /* the terminal before */
int inter = 0;
int stopped = 0;
char name[10];
#ifdef WIZARD
int author;
#endif
int gamestarted = 0;
int gameover = 0;
int dead = 0;
COORD guy = {22,28};            /* your coordinates */
COORD shot;                     /* the shot's coordinates */
int fired = 0;                  /* a shot has been fired */
long score = 0;
int board = 1;
int extramen = 3;
long nextman = FREEMAN;
int finished;                   /* time since board ended */
int breeding;                   /* are they breeding */
int breedtime = 300;            /* moves between breeds */
int moves = 0;
int fleahere = 0;               /* a flea is on the screen */
COORD flea;
int fleashot;                   /* was it shot once */
int fleafreq;                   /* a figure which helps determine the
				   frequency of fleas on a board */
int nummushrooms;               /* number of mushrooms in player area */
char lscorpion[] = "\`oo'--/";
char rscorpion[] = "\\--\`oo'";
char *scorppic;
int scorphere = 0;              /* is there a scorpion on the screen */
int scorpthisboard;
int scorpvel;
COORD scorp;
char *spiderpic[] = {
    "/\\  /\\",
    "/\\oo/\\"};
int spiderhere = 0;
COORD spider;
COORD spidervel;
int spiderdir;
int spidcount;
char in_map[128];
#ifdef old_input
char fichar = FIRE,             /* Characters used for movement */
     upchar = UPWARD,           /* commands by the user (settable) */
     dochar = DOWN,
     lechar = LEFT,
     richar = RIGHT,
     ulchar = UPLEFT,
     urchar = UPRIGHT,
     dlchar = DOWNLEFT,
     drchar = DOWNRIGHT,
     flchar = FASTLEFT,
     frchar = FASTRIGHT,
     pachar = PAUSEKEY;
#endif
