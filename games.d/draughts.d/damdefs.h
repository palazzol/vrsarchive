#define VERSION "3.0"
#define DATE    "770531"

#define EMPTY   0
#define WHITE   01000
#define BLACK   02000
#define EDGE    03000
#define DAM     04000
#define CAPT    0400
#define FLAG    CAPT
#define FLAG2   010000
#define FLAGS   FLAG+FLAG2
#define COL     WHITE+BLACK
#define VAL     COL+DAM
#define MASK    CAPT+COL

/* the values for USER and PDP are related to answer() */
#define USER    1
#define PDP     2
#define UNDEF   -1

#define DRAWWT  25
#define PVAL    100
#define DVAL    350
#define DMVAL   10

#define TPVAL   10

#define GIANT   10000
#define MVMAX   50
