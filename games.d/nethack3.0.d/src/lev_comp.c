/* A Bison parser, made from lev_comp.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CHAR = 258,
     INTEGER = 259,
     MAZE_ID = 260,
     LEVEL_ID = 261,
     GEOMETRY_ID = 262,
     OBJECT_ID = 263,
     MONSTER_ID = 264,
     TRAP_ID = 265,
     DOOR_ID = 266,
     DRAWBRIDGE_ID = 267,
     MAZEWALK_ID = 268,
     REGION_ID = 269,
     RANDOM_OBJECTS_ID = 270,
     RANDOM_MONSTERS_ID = 271,
     RANDOM_PLACES_ID = 272,
     ALTAR_ID = 273,
     LADDER_ID = 274,
     NON_DIGGABLE_ID = 275,
     ROOM_ID = 276,
     DOOR_STATE = 277,
     LIGHT_STATE = 278,
     DIRECTION = 279,
     RANDOM_TYPE = 280,
     O_REGISTER = 281,
     M_REGISTER = 282,
     P_REGISTER = 283,
     A_REGISTER = 284,
     ALIGNMENT = 285,
     LEFT_OR_RIGHT = 286,
     CENTER = 287,
     TOP_OR_BOT = 288,
     ALTAR_TYPE = 289,
     UP_OR_DOWN = 290,
     STRING = 291,
     MAP_ID = 292
   };
#endif
#define CHAR 258
#define INTEGER 259
#define MAZE_ID 260
#define LEVEL_ID 261
#define GEOMETRY_ID 262
#define OBJECT_ID 263
#define MONSTER_ID 264
#define TRAP_ID 265
#define DOOR_ID 266
#define DRAWBRIDGE_ID 267
#define MAZEWALK_ID 268
#define REGION_ID 269
#define RANDOM_OBJECTS_ID 270
#define RANDOM_MONSTERS_ID 271
#define RANDOM_PLACES_ID 272
#define ALTAR_ID 273
#define LADDER_ID 274
#define NON_DIGGABLE_ID 275
#define ROOM_ID 276
#define DOOR_STATE 277
#define LIGHT_STATE 278
#define DIRECTION 279
#define RANDOM_TYPE 280
#define O_REGISTER 281
#define M_REGISTER 282
#define P_REGISTER 283
#define A_REGISTER 284
#define ALIGNMENT 285
#define LEFT_OR_RIGHT 286
#define CENTER 287
#define TOP_OR_BOT 288
#define ALTAR_TYPE 289
#define UP_OR_DOWN 290
#define STRING 291
#define MAP_ID 292




/* Copy the first part of user declarations.  */
#line 1 "lev_comp.y"
 
/*	SCCS Id: @(#)lev_comp.c	3.0	89/07/02
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the Level Compiler code
 * It may handle special mazes & special room-levels
 */

/* block some unused #defines to avoid overloading some cpp's */
#define MONDATA_H
#include "hack.h"
#include "sp_lev.h"
#include <fcntl.h>

#ifdef AMIGA
char *fgets();
# define    alloc   malloc
# undef     fopen
# undef     printf
# undef     Printf
# define    Printf  printf
# define    memset(addr,val,len)    setmem(addr,len,val)
#endif

#ifdef MSDOS
# undef exit
#endif

#define MAX_REGISTERS	10
#define ERR		(-1)

struct reg {
	int x1, y1;
	int x2, y2;
}		current_region;

struct coord {
	int x;
	int y;
}		current_coord;

struct {
	char *name;
	short type;
} trap_types[TRAPNUM-1] = {
	"monster",	MONST_TRAP,
	"statue",	STATUE_TRAP,
	"bear",		BEAR_TRAP,
	"arrow",	ARROW_TRAP,
	"dart",		DART_TRAP,
	"trapdoor",	TRAPDOOR,
	"teleport",	TELEP_TRAP,
	"pit",		PIT,
	"sleeping gas",	SLP_GAS_TRAP,
	"magic",	MGTRP,
	"board",	SQBRD,
	"web",		WEB,
	"spiked pit",	SPIKED_PIT,
	"level teleport",LEVEL_TELEP,
#ifdef SPELLS
	"anti magic",	ANTI_MAGIC,
#endif
	"rust",		RUST_TRAP
#ifdef POLYSELF
	, "polymorph",	POLY_TRAP
#endif
#ifdef ARMY
	, "land mine",	LANDMINE
#endif
  };

struct {
	char *name;
	int type;
} room_types[SHOPBASE-1] = {
	/* for historical reasons, room types are not contiguous numbers */
	/* (type 1 is skipped) */
	"ordinary",	OROOM,
#ifdef THRONES
	"throne",	COURT,
#endif
	"swamp",	SWAMP,
	"vault",	VAULT,
	"beehive",	BEEHIVE,
	"morgue",	MORGUE,
#ifdef ARMY
	"barracks",	BARRACKS,
#endif
	"zoo",		ZOO,
	"temple",	TEMPLE,
	"shop",		SHOPBASE,
};

short db_dirs[4] = {
	DB_NORTH,
	DB_EAST,
	DB_SOUTH,
	DB_WEST
};

#ifdef ALTARS
static altar *tmpaltar[256];
#endif /* ALTARS /**/
static lad *tmplad[256];
static dig *tmpdig[256];
static char *tmpmap[ROWNO];
static region *tmpreg[16];
static door *tmpdoor[256];
static trap *tmptrap[256];
static monster *tmpmonst[256];
static object *tmpobj[256];
static drawbridge *tmpdb[256];
static walk *tmpwalk[256];
static mazepart *tmppart[10];
static room *tmproom[MAXNROFROOMS];
static specialmaze maze;

static char olist[MAX_REGISTERS], mlist[MAX_REGISTERS];
static struct coord plist[MAX_REGISTERS];
static int n_olist = 0, n_mlist = 0, n_plist = 0;

unsigned int nreg = 0, ndoor = 0, ntrap = 0, nmons = 0, nobj = 0;
unsigned int ndb = 0, nwalk = 0, npart = 0, ndig = 0, nlad = 0;
#ifdef ALTARS
unsigned int naltar = 0;
#endif /* ALTARS /*/

unsigned int max_x_map, max_y_map;

extern int fatal_error;
extern char* fname;

boolean check_monster_char(), check_object_char();
void scan_map(), store_part(), write_maze();



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 141 "lev_comp.y"
typedef union {
	int	i;
	char*	map;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 291 "y.tab.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 312 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  8
#define YYLAST   143

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  86
/* YYNRULES -- Number of states. */
#define YYNSTATES  176

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   292

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      38,    39,     2,     2,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,     2,    41,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    42,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    16,    20,
      22,    25,    29,    32,    38,    40,    42,    44,    46,    47,
      50,    54,    58,    62,    64,    68,    70,    74,    76,    80,
      81,    84,    86,    88,    90,    92,    94,    96,    98,   100,
     102,   104,   112,   120,   126,   132,   140,   146,   152,   156,
     164,   172,   174,   176,   178,   180,   182,   184,   186,   188,
     190,   192,   194,   196,   198,   200,   202,   204,   206,   208,
     210,   212,   214,   216,   218,   220,   222,   224,   229,   234,
     239,   244,   246,   248,   250,   252,   258
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      45,     0,    -1,    -1,    46,    -1,    47,    -1,    46,    47,
      -1,    48,    -1,    49,    50,    -1,     5,    37,    91,    -1,
      51,    -1,    50,    51,    -1,    52,    56,    61,    -1,    53,
      43,    -1,     7,    37,    54,    36,    55,    -1,    31,    -1,
      32,    -1,    33,    -1,    32,    -1,    -1,    56,    57,    -1,
      15,    37,    58,    -1,    17,    37,    60,    -1,    16,    37,
      59,    -1,    90,    -1,    58,    36,    90,    -1,    89,    -1,
      59,    36,    89,    -1,    88,    -1,    60,    36,    88,    -1,
      -1,    61,    62,    -1,    63,    -1,    64,    -1,    65,    -1,
      66,    -1,    67,    -1,    71,    -1,    72,    -1,    68,    -1,
      69,    -1,    70,    -1,     9,    37,    73,    36,    75,    36,
      79,    -1,     8,    37,    74,    36,    76,    36,    79,    -1,
      11,    37,    80,    36,    79,    -1,    10,    37,    77,    36,
      79,    -1,    12,    37,    79,    36,    24,    36,    80,    -1,
      13,    37,    79,    36,    24,    -1,    19,    37,    79,    36,
      35,    -1,    20,    37,    93,    -1,    14,    37,    93,    36,
      81,    36,    78,    -1,    18,    37,    79,    36,    82,    36,
      83,    -1,    89,    -1,    25,    -1,    86,    -1,    90,    -1,
      25,    -1,    85,    -1,    91,    -1,    25,    -1,    91,    -1,
      25,    -1,    91,    -1,    25,    -1,    91,    -1,    25,    -1,
      92,    -1,    84,    -1,    25,    -1,    22,    -1,    25,    -1,
      23,    -1,    25,    -1,    30,    -1,    87,    -1,    25,    -1,
      34,    -1,    25,    -1,    28,    40,     4,    41,    -1,    26,
      40,     4,    41,    -1,    27,    40,     4,    41,    -1,    29,
      40,     4,    41,    -1,    92,    -1,     3,    -1,     3,    -1,
      42,    -1,    38,     4,    36,     4,    39,    -1,    38,     4,
      36,     4,    36,     4,    36,     4,    39,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   161,   161,   162,   164,   165,   167,   169,   195,   200,
     201,   203,   208,   219,   224,   225,   227,   228,   230,   231,
     233,   243,   258,   269,   276,   284,   291,   299,   306,   314,
     315,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   328,   349,   370,   379,   388,   403,   412,   421,   431,
     443,   457,   458,   462,   464,   465,   469,   471,   472,   477,
     478,   483,   490,   493,   502,   505,   506,   507,   512,   513,
     516,   517,   520,   521,   522,   527,   528,   531,   540,   549,
     558,   567,   570,   580,   599,   602,   611
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHAR", "INTEGER", "MAZE_ID", "LEVEL_ID", 
  "GEOMETRY_ID", "OBJECT_ID", "MONSTER_ID", "TRAP_ID", "DOOR_ID", 
  "DRAWBRIDGE_ID", "MAZEWALK_ID", "REGION_ID", "RANDOM_OBJECTS_ID", 
  "RANDOM_MONSTERS_ID", "RANDOM_PLACES_ID", "ALTAR_ID", "LADDER_ID", 
  "NON_DIGGABLE_ID", "ROOM_ID", "DOOR_STATE", "LIGHT_STATE", "DIRECTION", 
  "RANDOM_TYPE", "O_REGISTER", "M_REGISTER", "P_REGISTER", "A_REGISTER", 
  "ALIGNMENT", "LEFT_OR_RIGHT", "CENTER", "TOP_OR_BOT", "ALTAR_TYPE", 
  "UP_OR_DOWN", "','", "':'", "'('", "')'", "'['", "']'", "STRING", 
  "MAP_ID", "$accept", "file", "levels", "level", "maze_level", 
  "maze_def", "regions", "aregion", "map_definition", "map_geometry", 
  "h_justif", "v_justif", "reg_init", "init_reg", "object_list", 
  "monster_list", "place_list", "map_details", "map_detail", 
  "monster_detail", "object_detail", "door_detail", "trap_detail", 
  "drawbridge_detail", "mazewalk_detail", "ladder_detail", 
  "diggable_detail", "region_detail", "altar_detail", "monster_c", 
  "object_c", "m_name", "o_name", "trap_name", "room_type", "coordinate", 
  "door_state", "light_state", "alignment", "altar_type", "p_register", 
  "o_register", "m_register", "a_register", "place", "monster", "object", 
  "string", "coord", "region", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,    44,    58,    40,    41,
      91,    93,   291,   292
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    48,    49,    50,
      50,    51,    52,    53,    54,    54,    55,    55,    56,    56,
      57,    57,    57,    58,    58,    59,    59,    60,    60,    61,
      61,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    73,    73,    74,    74,    74,    75,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    79,    80,    80,
      81,    81,    82,    82,    82,    83,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     2,     3,     1,
       2,     3,     2,     5,     1,     1,     1,     1,     0,     2,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     7,     7,     5,     5,     7,     5,     5,     3,     7,
       7,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       4,     1,     1,     1,     1,     5,     9
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     0,     3,     4,     6,     0,     0,     1,     5,
       0,     7,     9,    18,     0,    84,     8,     0,    10,    29,
      12,    14,    15,     0,     0,     0,     0,    19,    11,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,    31,    32,    33,    34,    35,    38,
      39,    40,    36,    37,    17,    16,    13,    83,    20,    23,
      82,    22,    25,     0,    21,    27,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,     0,     0,    56,    54,    52,     0,     0,    53,
      51,    62,     0,    61,    68,    69,     0,    67,     0,     0,
      66,    65,     0,     0,     0,     0,     0,    48,    24,    26,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,     0,    59,
       0,    58,     0,    57,    44,    43,     0,     0,    46,     0,
      70,    71,     0,    74,     0,    72,     0,    73,    47,    85,
      78,     0,    79,     0,    77,     0,     0,     0,     0,     0,
      42,    41,    45,     0,    64,    49,    63,     0,    76,    75,
      50,     0,    80,     0,     0,    86
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     2,     3,     4,     5,     6,    11,    12,    13,    14,
      23,    56,    19,    27,    58,    61,    64,    28,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    88,
      83,   132,   128,    92,   165,    99,    96,   142,   146,   170,
     100,    84,    89,   147,    65,    62,    59,    16,   101,   104
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -70
static const yysigned_char yypact[] =
{
      11,     8,    17,    11,   -70,   -70,    32,    20,   -70,   -70,
      29,    32,   -70,   -70,   -21,   -70,   -70,    27,   -70,    37,
     -70,   -70,   -70,    31,    33,    34,    35,   -70,    23,    28,
      60,    61,    30,    36,    38,    39,    40,    41,    42,    43,
      44,    45,    47,   -70,   -70,   -70,   -70,   -70,   -70,   -70,
     -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,    50,   -70,
     -70,    51,   -70,    65,    53,   -70,   -70,     2,    -2,   -22,
      -7,   -17,   -17,    52,   -17,   -17,    52,    60,    61,    55,
      30,   -70,    25,    56,   -70,   -70,   -70,    54,    57,   -70,
     -70,   -70,    59,   -70,   -70,   -70,    62,   -70,    63,    64,
     -70,   -70,    66,    70,    68,    69,    71,   -70,   -70,   -70,
      92,   -70,    93,   -16,    95,   -13,   -17,   -17,    97,    82,
      84,    73,    15,    26,    75,    72,    74,   -70,    76,   -70,
      77,   -70,    78,   -70,   -70,   -70,    79,    80,   -70,   109,
     -70,   -70,    81,   -70,    83,   -70,    85,   -70,   -70,   -70,
     -70,   -17,   -70,   -17,   -70,    -7,    86,   -12,   115,   -15,
     -70,   -70,   -70,   120,   -70,   -70,   -70,    87,   -70,   -70,
     -70,    89,   -70,   122,    88,   -70
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -70,   -70,   -70,   126,   -70,   -70,   -70,   119,   -70,   -70,
     -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,
     -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,
     -70,   -70,   -70,   -70,   -70,   -68,   -24,   -70,   -70,   -70,
     -70,   -70,   -70,   -70,    58,   -54,   -20,   -69,   -30,    67
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      93,    60,    66,    91,   102,    57,   105,   106,    97,   127,
     168,    98,   131,   164,    90,    94,     1,     8,    95,   169,
      15,    63,    20,    86,   109,    87,    15,    81,    82,    15,
      15,    33,    34,    35,    36,    37,    38,    39,   140,    10,
     141,    40,    41,    42,   129,     7,   133,    85,   134,   135,
      66,   143,    24,    25,    26,   144,   145,   108,    21,    22,
      54,    55,    15,    57,    60,   112,    17,    29,    63,    79,
      30,    31,    32,    67,   121,    68,    69,    70,    71,    72,
      73,    74,    75,   160,    76,   161,    77,    78,   166,    80,
     103,   110,   113,   115,   114,   116,   125,   126,   117,   130,
     119,   136,   120,   118,   122,   123,   137,   124,   138,   139,
     148,   149,   151,   156,   153,   150,   155,   157,   152,   167,
     154,   159,   163,   158,   171,   173,   174,   175,   172,     9,
      18,   162,     0,     0,     0,     0,     0,     0,   111,     0,
       0,     0,     0,   107
};

static const short yycheck[] =
{
      69,     3,    32,    25,    72,     3,    74,    75,    25,    25,
      25,    28,    25,    25,    68,    22,     5,     0,    25,    34,
      42,    38,    43,    25,    78,    27,    42,    25,    26,    42,
      42,     8,     9,    10,    11,    12,    13,    14,    23,     7,
      25,    18,    19,    20,   113,    37,   115,    67,   116,   117,
      80,    25,    15,    16,    17,    29,    30,    77,    31,    32,
      32,    33,    42,     3,     3,    40,    37,    36,    38,     4,
      37,    37,    37,    37,     4,    37,    37,    37,    37,    37,
      37,    37,    37,   151,    37,   153,    36,    36,   157,    36,
      38,    36,    36,    36,    40,    36,     4,     4,    36,     4,
      36,     4,    36,    40,    36,    36,    24,    36,    24,    36,
      35,    39,    36,     4,    36,    41,    36,    36,    41,     4,
      41,    36,    36,    40,     4,    36,     4,    39,    41,     3,
      11,   155,    -1,    -1,    -1,    -1,    -1,    -1,    80,    -1,
      -1,    -1,    -1,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     5,    45,    46,    47,    48,    49,    37,     0,    47,
       7,    50,    51,    52,    53,    42,    91,    37,    51,    56,
      43,    31,    32,    54,    15,    16,    17,    57,    61,    36,
      37,    37,    37,     8,     9,    10,    11,    12,    13,    14,
      18,    19,    20,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    32,    33,    55,     3,    58,    90,
       3,    59,    89,    38,    60,    88,    92,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    36,    36,     4,
      36,    25,    26,    74,    85,    90,    25,    27,    73,    86,
      89,    25,    77,    91,    22,    25,    80,    25,    28,    79,
      84,    92,    79,    38,    93,    79,    79,    93,    90,    89,
      36,    88,    40,    36,    40,    36,    36,    36,    40,    36,
      36,     4,    36,    36,    36,     4,     4,    25,    76,    91,
       4,    25,    75,    91,    79,    79,     4,    24,    24,    36,
      23,    25,    81,    25,    29,    30,    82,    87,    35,    39,
      41,    36,    41,    36,    41,    36,     4,    36,    40,    36,
      79,    79,    80,    36,    25,    78,    91,     4,    25,    34,
      83,     4,    41,    36,     4,    39
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 7:
#line 170 "lev_comp.y"
    {
			  int fout, i;

			  if (fatal_error > 0)
				  fprintf(stderr,"%s : %d errors detected. No output created!\n", fname, fatal_error);
			  else {
				  fout = open(yyvsp[-1].map, O_WRONLY | O_CREAT
#ifdef MSDOS
					      | O_BINARY
#endif /* MSDOS */
					      , 0644);
				  if (fout < 0) {
					  yyerror("Can't open output file!!");
					  exit(1);
				  }
				  maze.numpart = npart;
				  maze.parts = (mazepart**) alloc(sizeof(mazepart*)*npart);
				  for(i=0;i<npart;i++)
				      maze.parts[i] = tmppart[i];
				  write_maze(fout, &maze);
				  (void) close(fout);
				  npart = 0;
			  }
		  }
    break;

  case 8:
#line 196 "lev_comp.y"
    {
			  yyval.map = yyvsp[0].map;
		  }
    break;

  case 11:
#line 204 "lev_comp.y"
    {
			store_part();
		  }
    break;

  case 12:
#line 209 "lev_comp.y"
    {
			tmppart[npart] = (mazepart *) alloc(sizeof(mazepart));
			tmppart[npart]->halign = yyvsp[-1].i % 10;
			tmppart[npart]->valign = yyvsp[-1].i / 10;
			tmppart[npart]->nrobjects = 0;
			tmppart[npart]->nloc = 0;
			tmppart[npart]->nrmonst = 0;
			scan_map(yyvsp[0].map);
		  }
    break;

  case 13:
#line 220 "lev_comp.y"
    {
			  yyval.i = yyvsp[-2].i + ( yyvsp[0].i * 10 );
		  }
    break;

  case 20:
#line 234 "lev_comp.y"
    {
			  if (tmppart[npart]->nrobjects)
			      yyerror("Object registers already initialized!");
			  else {
				  tmppart[npart]->robjects = (char *) alloc(n_olist);
				  memcpy(tmppart[npart]->robjects, olist, n_olist);
				  tmppart[npart]->nrobjects = n_olist;
			  }
		  }
    break;

  case 21:
#line 244 "lev_comp.y"
    {
			  if (tmppart[npart]->nloc)
			      yyerror("Location registers already initialized!");
			  else {
				  register int i;
				  tmppart[npart]->rloc_x = (char *) alloc(n_plist);
				  tmppart[npart]->rloc_y = (char *) alloc(n_plist);
				  for(i=0;i<n_plist;i++) {
					  tmppart[npart]->rloc_x[i] = plist[i].x;
					  tmppart[npart]->rloc_y[i] = plist[i].y;
				  }
				  tmppart[npart]->nloc = n_plist;
			  }
		  }
    break;

  case 22:
#line 259 "lev_comp.y"
    {
			  if (tmppart[npart]->nrmonst)
			      yyerror("Monster registers already initialized!");
			  else {
				  tmppart[npart]->rmonst = (char *) alloc(n_mlist);
				  memcpy(tmppart[npart]->rmonst, mlist, n_mlist);
				  tmppart[npart]->nrmonst = n_mlist;
			  }
		  }
    break;

  case 23:
#line 270 "lev_comp.y"
    {
			  if (n_olist < MAX_REGISTERS)
			      olist[n_olist++] = yyvsp[0].i;
			  else
			      yyerror("Object list too long!");
		  }
    break;

  case 24:
#line 277 "lev_comp.y"
    {
			  if (n_olist < MAX_REGISTERS)
			      olist[n_olist++] = yyvsp[0].i;
			  else
			      yyerror("Object list too long!");
		  }
    break;

  case 25:
#line 285 "lev_comp.y"
    {
			  if (n_mlist < MAX_REGISTERS)
			      mlist[n_mlist++] = yyvsp[0].i;
			  else
			      yyerror("Monster list too long!");
		  }
    break;

  case 26:
#line 292 "lev_comp.y"
    {
			  if (n_mlist < MAX_REGISTERS)
			      mlist[n_mlist++] = yyvsp[0].i;
			  else
			      yyerror("Monster list too long!");
		  }
    break;

  case 27:
#line 300 "lev_comp.y"
    {
			  if (n_plist < MAX_REGISTERS)
			      plist[n_plist++] = current_coord;
			  else
			      yyerror("Location list too long!");
		  }
    break;

  case 28:
#line 307 "lev_comp.y"
    {
			  if (n_plist < MAX_REGISTERS)
			      plist[n_plist++] = current_coord;
			  else
			      yyerror("Location list too long!");
		  }
    break;

  case 41:
#line 329 "lev_comp.y"
    {
			  int token;

			  tmpmonst[nmons] = (monster *) alloc(sizeof(monster));
			  tmpmonst[nmons]->x = current_coord.x;
			  tmpmonst[nmons]->y = current_coord.y;
			  tmpmonst[nmons]->class = yyvsp[-4].i;
			  if (!yyvsp[-2].map)
			      tmpmonst[nmons]->id = -1;
			  else {
				  token = get_monster_id(yyvsp[-2].map, (char) yyvsp[-4].i);  
				  if (token == ERR) {
				      yywarning("Illegal monster name!  Making random monster.");
				      tmpmonst[nmons]->id = -1;
				  } else
				      tmpmonst[nmons]->id = token;
			  }
			  nmons++;
		  }
    break;

  case 42:
#line 350 "lev_comp.y"
    {
			  int token;

			  tmpobj[nobj] = (object *) alloc(sizeof(object));
			  tmpobj[nobj]->x = current_coord.x;
			  tmpobj[nobj]->y = current_coord.y;
			  tmpobj[nobj]->class = yyvsp[-4].i;
			  if (!yyvsp[-2].map)
			      tmpobj[nobj]->id = -1;
			  else {
				  token = get_object_id(yyvsp[-2].map, (char) yyvsp[-4].i);
				  if (token == ERR) {
				      yywarning("Illegal object name!  Making random object.");
				      tmpobj[nobj]->id = -1;
				  } else
				      tmpobj[nobj]->id = token;
			  }
			  nobj++;
		  }
    break;

  case 43:
#line 371 "lev_comp.y"
    {
			tmpdoor[ndoor] = (door *) alloc(sizeof(door));
			tmpdoor[ndoor]->x = current_coord.x;
			tmpdoor[ndoor]->y = current_coord.y;
			tmpdoor[ndoor]->mask = yyvsp[-2].i;
			ndoor++;
		  }
    break;

  case 44:
#line 380 "lev_comp.y"
    {
			tmptrap[ntrap] = (trap *) alloc(sizeof(trap));
			tmptrap[ntrap]->x = current_coord.x;
			tmptrap[ntrap]->y = current_coord.y;
			tmptrap[ntrap]->type = yyvsp[-2].i;
			ntrap++;
		  }
    break;

  case 45:
#line 389 "lev_comp.y"
    {
			tmpdb[ndb] = (drawbridge *) alloc(sizeof(drawbridge));
			tmpdb[ndb]->x = current_coord.x;
			tmpdb[ndb]->y = current_coord.y;
			tmpdb[ndb]->dir = db_dirs[yyvsp[-2].i];
			if ( yyvsp[0].i == D_ISOPEN )
			  tmpdb[ndb]->open = 1;
			else if ( yyvsp[0].i == D_CLOSED )
			  tmpdb[ndb]->open = 0;
			else
			  yyerror("A drawbridge can only be open or closed!");
			ndb++;
		   }
    break;

  case 46:
#line 404 "lev_comp.y"
    {
			tmpwalk[nwalk] = (walk *) alloc(sizeof(walk));
			tmpwalk[nwalk]->x = current_coord.x;
			tmpwalk[nwalk]->y = current_coord.y;
			tmpwalk[nwalk]->dir = yyvsp[0].i;
			nwalk++;
		  }
    break;

  case 47:
#line 413 "lev_comp.y"
    {
			tmplad[nlad] = (lad *) alloc(sizeof(lad));
			tmplad[nlad]->x = current_coord.x;
			tmplad[nlad]->y = current_coord.y;
			tmplad[nlad]->up = yyvsp[0].i;
			nlad++;
		  }
    break;

  case 48:
#line 422 "lev_comp.y"
    {
			tmpdig[ndig] = (dig *) alloc(sizeof(dig));
			tmpdig[ndig]->x1 = current_region.x1;
			tmpdig[ndig]->y1 = current_region.y1;
			tmpdig[ndig]->x2 = current_region.x2;
			tmpdig[ndig]->y2 = current_region.y2;
			ndig++;
		  }
    break;

  case 49:
#line 432 "lev_comp.y"
    {
			tmpreg[nreg] = (region *) alloc(sizeof(region));
			tmpreg[nreg]->x1 = current_region.x1;
			tmpreg[nreg]->y1 = current_region.y1;
			tmpreg[nreg]->x2 = current_region.x2;
			tmpreg[nreg]->y2 = current_region.y2;
			tmpreg[nreg]->rlit = yyvsp[-2].i;
			tmpreg[nreg]->rtype = yyvsp[0].i;
			nreg++;
		  }
    break;

  case 50:
#line 444 "lev_comp.y"
    {
#ifndef ALTARS
			yywarning("Altars are not allowed in this version!  Ignoring...");
#else
			tmpaltar[naltar] = (altar *) alloc(sizeof(altar));
			tmpaltar[naltar]->x = current_coord.x;
			tmpaltar[naltar]->y = current_coord.y;
			tmpaltar[naltar]->align = yyvsp[-2].i;
			tmpaltar[naltar]->shrine = yyvsp[0].i;
			naltar++;
#endif /* ALTARS */
		  }
    break;

  case 52:
#line 459 "lev_comp.y"
    {
			  yyval.i = - MAX_REGISTERS - 1;
		  }
    break;

  case 55:
#line 466 "lev_comp.y"
    {
			  yyval.i = - MAX_REGISTERS - 1;
		  }
    break;

  case 58:
#line 473 "lev_comp.y"
    {
			  yyval.map = (char *) 0;
		  }
    break;

  case 60:
#line 479 "lev_comp.y"
    {
			  yyval.map = (char *) 0;
		  }
    break;

  case 61:
#line 484 "lev_comp.y"
    {
		  	int token = get_trap_type(yyvsp[0].map);
			if (token == ERR)
				yyerror("unknown trap type!");
			yyval.i = token;
		  }
    break;

  case 63:
#line 494 "lev_comp.y"
    {
			int token = get_room_type(yyvsp[0].map);
			if (token == ERR) {
				yywarning("Unknown room type!  Making ordinary room...");
				yyval.i = OROOM;
			} else
				yyval.i = token;
		  }
    break;

  case 67:
#line 508 "lev_comp.y"
    {
			  current_coord.x = current_coord.y = -MAX_REGISTERS-1;
		  }
    break;

  case 74:
#line 523 "lev_comp.y"
    {
			  yyval.i = - MAX_REGISTERS - 1;
		  }
    break;

  case 77:
#line 532 "lev_comp.y"
    {
			if ( yyvsp[-1].i >= MAX_REGISTERS ) {
				yyerror("Register Index overflow!");
			} else {
				current_coord.x = current_coord.y = - yyvsp[-1].i - 1;
			}
		  }
    break;

  case 78:
#line 541 "lev_comp.y"
    {
			if ( yyvsp[-1].i >= MAX_REGISTERS ) {
				yyerror("Register Index overflow!");
			} else {
				yyval.i = - yyvsp[-1].i - 1;
			}
		  }
    break;

  case 79:
#line 550 "lev_comp.y"
    {
			if ( yyvsp[-1].i >= MAX_REGISTERS ) {
				yyerror("Register Index overflow!");
			} else {
				yyval.i = - yyvsp[-1].i - 1;
			}
		  }
    break;

  case 80:
#line 559 "lev_comp.y"
    {
			if ( yyvsp[-1].i >= 3 ) {
				yyerror("Register Index overflow!");
			} else {
				yyval.i = - yyvsp[-1].i - 1;
			}
		  }
    break;

  case 82:
#line 571 "lev_comp.y"
    {
			if (check_monster_char(yyvsp[0].i))
				yyval.i = yyvsp[0].i ;
			else {
				yyerror("unknown monster class!");
				yyval.i = ERR;
			}
		  }
    break;

  case 83:
#line 581 "lev_comp.y"
    {
			char c;

			c = yyvsp[0].i;
#ifndef SPELLS
			if ( c == '+') {
				c = '?';
				yywarning("Spellbooks are not allowed in this version! (converted into scroll)");
			}
#endif
			if (check_object_char(c))
				yyval.i = c;
			else {
				yyerror("Unknown char class!");
				yyval.i = ERR;
			}
		  }
    break;

  case 85:
#line 603 "lev_comp.y"
    {
		        if (yyvsp[-3].i < 0 || yyvsp[-3].i > max_x_map ||
			    yyvsp[-1].i < 0 || yyvsp[-1].i > max_y_map)
			    yyerror("Coordinates out of map range!");
			current_coord.x = yyvsp[-3].i;
			current_coord.y = yyvsp[-1].i;
		  }
    break;

  case 86:
#line 612 "lev_comp.y"
    {
		        if (yyvsp[-7].i < 0 || yyvsp[-7].i > max_x_map ||
			    yyvsp[-5].i < 0 || yyvsp[-5].i > max_y_map ||
			    yyvsp[-3].i < 0 || yyvsp[-3].i > max_x_map ||
			    yyvsp[-1].i < 0 || yyvsp[-1].i > max_y_map)
			    yyerror("Region out of map range!");
			current_region.x1 = yyvsp[-7].i;
			current_region.y1 = yyvsp[-5].i;
			current_region.x2 = yyvsp[-3].i;
			current_region.y2 = yyvsp[-1].i;
		  }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 1764 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 625 "lev_comp.y"


/* 
 * Find the type of a room in the table, knowing its name.
 */

int
get_room_type(s)
char *s;
{
	register int i;
	
	for(i=0; i < SHOPBASE -1; i++)
	    if (!strcmp(s, room_types[i].name))
		return room_types[i].type;
	return ERR;
}

/* 
 * Find the type of a trap in the table, knowing its name.
 */

int
get_trap_type(s)
char *s;
{
	register int i;
	
	for(i=0; i < TRAPNUM - 1; i++)
	    if(!strcmp(s,trap_types[i].name))
		return(trap_types[i].type);
	return ERR;
}

/* 
 * Find the index of a monster in the table, knowing its name.
 */

int
get_monster_id(s, c)
char *s;
char c;
{
	register int i;
	
	for(i=0; mons[i].mname[0]; i++)
	    if(!strncmp(s, mons[i].mname, strlen(mons[i].mname))
	       && c == mons[i].mlet)
		return i;
	return ERR;
}

/* 
 * Find the index of an object in the table, knowing its name.
 */

int
get_object_id(s, c)
char *s;
char c;
{
	register int i;
	
	for(i=0; i<=NROFOBJECTS;i++)
	    if(objects[i].oc_name &&
	       !strncmp(s, objects[i].oc_name, strlen(objects[i].oc_name))
	       && c == objects[i].oc_olet)
		return i;
	return ERR;
}

/* 
 * Is the character 'c' a valid monster class ?
 */

boolean
check_monster_char(c)
char c;
{
	register int i;
	
	for(i=0; mons[i].mname[0]; i++)
	    if( c ==  mons[i].mlet)
		return 1;
	return(0);
}

/* 
 * Is the character 'c' a valid object class ?
 */

boolean
check_object_char(c)
char c;
{
	register int i;
	
	for(i=0; i<=NROFOBJECTS;i++)
	    if( c == objects[i].oc_olet)
		return 1;
	return 0;
}

/* 
 * Yep! LEX gives us the map in a raw mode.
 * Just analyze it here.
 */

void scan_map(map)
char *map;
{
	register int i, len;
	register char *s1, *s2;
	int max_len = 0;
	int max_hig = 0;
	
	/* First : find the max width of the map */

	s1 = map;
	while (s1 && *s1) {
		s2 = index(s1, '\n');
		if (s2) {
			if (s2-s1 > max_len)
			    max_len = s2-s1;
			s1 = s2 + 1;
		} else {
			if (strlen(s1) > max_len)
			    max_len = strlen(s1);
			s1 = (char *) 0;
		}
	}

	/* Then parse it now */

	while (map && *map) {
		tmpmap[max_hig] = (char *) alloc(max_len);
		s1 = index(map, '\n');
		if (s1) {
			len = s1 - map;
			s1++;
		} else {
			len = strlen(map);
			s1 = map + len;
		}
		for(i=0; i<len; i++)
		    switch(map[i]) {
			  case '-' : tmpmap[max_hig][i] = HWALL; break;
			  case '|' : tmpmap[max_hig][i] = VWALL; break;
			  case '+' : tmpmap[max_hig][i] = DOOR; break;
			  case 'S' : tmpmap[max_hig][i] = SDOOR; break;
			  case '{' : 
#ifdef FOUNTAINS
			      tmpmap[max_hig][i] = FOUNTAIN;
#else
			      tmpmap[max_hig][i] = ROOM;
			      yywarning("Fountains are not allowed in this version!  Ignoring...");
#endif
			      break;
			  case '\\' : 
#ifdef THRONES
			      tmpmap[max_hig][i] = THRONE;
#else
			      tmpmap[max_hig][i] = ROOM;
			      yywarning("Thrones are not allowed in this version!  Ignoring...");
#endif
			      break;
			  case 'K' : 
#ifdef SINKS
			      tmpmap[max_hig][i] = SINK;
#else
			      tmpmap[max_hig][i] = ROOM;
			      yywarning("Sinks are not allowed in this version!  Ignoring...");
#endif
			      break;
			  case '}' : tmpmap[max_hig][i] = MOAT; break;
			  case '#' : tmpmap[max_hig][i] = CORR; break;
			  default  : tmpmap[max_hig][i] = ROOM; break;
		    }
		while(i < max_len)
		    tmpmap[max_hig][i++] = ROOM;
		map = s1;
		max_hig++;
	}

	/* Memorize boundaries */

	max_x_map = max_len - 1;
	max_y_map = max_hig - 1;

	/* Store the map into the mazepart structure */

	tmppart[npart]->xsize = max_len;
	tmppart[npart]->ysize = max_hig;
	tmppart[npart]->map = (char **) alloc(max_hig*sizeof(char *));
	for(i = 0; i< max_hig; i++)
	    tmppart[npart]->map[i] = tmpmap[i];
}

/* 
 * Here we want to store the maze part we just got.
 */

void
store_part()
{
	register int i;

	/* Ok, We got the whole part, now we store it. */
	
	/* The Regions */

	if(tmppart[npart]->nreg = nreg) {
		tmppart[npart]->regions = (region**)alloc(sizeof(region*) * nreg);
		for(i=0;i<nreg;i++)
		    tmppart[npart]->regions[i] = tmpreg[i];
	}
	nreg = 0;

	/* the doors */

	if(tmppart[npart]->ndoor = ndoor) {
		tmppart[npart]->doors = (door **)alloc(sizeof(door *) * ndoor);
		for(i=0;i<ndoor;i++)
		    tmppart[npart]->doors[i] = tmpdoor[i];
	}
	ndoor = 0;

	/* the traps */

	if(tmppart[npart]->ntraps = ntrap) {
		tmppart[npart]->traps = (trap **)alloc(sizeof(trap*) * ntrap);
		for(i=0;i<ntrap;i++)
		    tmppart[npart]->traps[i] = tmptrap[i];
	}
	ntrap = 0;

	/* the monsters */

	if(tmppart[npart]->nmonster = nmons) {
		tmppart[npart]->monsters = (monster**)alloc(sizeof(monster*)*nmons);
		for(i=0;i<nmons;i++)
		    tmppart[npart]->monsters[i] = tmpmonst[i];
	}
	nmons = 0;

	/* the objects */

	if(tmppart[npart]->nobjects = nobj) {
		tmppart[npart]->objects = (object**)alloc(sizeof(object*)*nobj);
		for(i=0;i<nobj;i++)
		    tmppart[npart]->objects[i] = tmpobj[i];
	}
	nobj = 0;

	/* the drawbridges */

	if(tmppart[npart]->ndrawbridge = ndb) {
		tmppart[npart]->drawbridges = (drawbridge**)alloc(sizeof(drawbridge*)*ndb);
		for(i=0;i<ndb;i++)
		    tmppart[npart]->drawbridges[i] = tmpdb[i];
	}
	ndb = 0;

	/* The walkmaze directives */

	if(tmppart[npart]->nwalk = nwalk) {
		tmppart[npart]->walks = (walk**)alloc(sizeof(walk*)*nwalk);
		for(i=0;i<nwalk;i++)
		    tmppart[npart]->walks[i] = tmpwalk[i];
	}
	nwalk = 0;

	/* The non_diggable directives */

	if(tmppart[npart]->ndig = ndig) {
		tmppart[npart]->digs = (dig **) alloc(sizeof(dig*) * ndig);
		for(i=0;i<ndig;i++)
		    tmppart[npart]->digs[i] = tmpdig[i];
	}
	ndig = 0;

	/* The ladders */

	if(tmppart[npart]->nlad = nlad) {
		tmppart[npart]->lads = (lad **) alloc(sizeof(lad*) * nlad);
		for(i=0;i<nlad;i++)
		    tmppart[npart]->lads[i] = tmplad[i];
	}
	nlad = 0;
#ifdef ALTARS
	/* The altars */

	if(tmppart[npart]->naltar = naltar) {
		tmppart[npart]->altars = (altar**)alloc(sizeof(altar*) * naltar);
		for(i=0;i<naltar;i++)
		    tmppart[npart]->altars[i] = tmpaltar[i];
	}
	naltar = 0;
#endif /* ALTARS /**/
	npart++;
	n_plist = n_mlist = n_olist = 0;
}

/* 
 * Here we write the structure of the maze in the specified file (fd).
 * Also, we have to free the memory allocated via alloc()
 */

void
write_maze(fd, maze)
int fd;
specialmaze *maze;
{
	char c;
	short i,j;
	mazepart *pt;

	c = 2;
	(void) write(fd, &c, 1);	/* Header for special mazes */
	(void) write(fd, &(maze->numpart), 1);	/* Number of parts */
	for(i=0;i<maze->numpart;i++) {
	    pt = maze->parts[i];

	    /* First, write the map */

	    (void) write(fd, &(pt->halign), 1);
	    (void) write(fd, &(pt->valign), 1);
	    (void) write(fd, &(pt->xsize), 1);
	    (void) write(fd, &(pt->ysize), 1);
	    for(j=0;j<pt->ysize;j++) {
		    (void) write(fd, pt->map[j], pt->xsize);
		    free(pt->map[j]);
	    }
	    free(pt->map);

	    /* The random registers */
	    (void) write(fd, &(pt->nrobjects), 1);
	    if(pt->nrobjects) {
		    (void) write(fd, pt->robjects, pt->nrobjects);
		    free(pt->robjects);
	    }
	    (void) write(fd, &(pt->nloc), 1);
	    if(pt->nloc) {
		(void) write(fd,pt->rloc_x, pt->nloc);
		(void) write(fd,pt->rloc_y, pt->nloc);
		free(pt->rloc_x);
		free(pt->rloc_y);
	    }
	    (void) write(fd,&(pt->nrmonst), 1);
	    if(pt->nrmonst) {
		    (void) write(fd, pt->rmonst, pt->nrmonst);
		    free(pt->rmonst);
	    }

	    /* subrooms */
	    (void) write(fd, &(pt->nreg), 1);
	    for(j=0;j<pt->nreg;j++) {
		    (void) write(fd,(genericptr_t) pt->regions[j], sizeof(region));
		    free(pt->regions[j]);
	    }
	    if(pt->nreg > 0)
		free(pt->regions);

	    /* the doors */
	    (void) write(fd,&(pt->ndoor),1);
	    for(j=0;j<pt->ndoor;j++) {
		    (void) write(fd,(genericptr_t) pt->doors[j], sizeof(door));
		    free(pt->doors[j]);
	    }
	    if (pt->ndoor > 0)
		free(pt->doors);

	    /* The traps */
	    (void) write(fd,&(pt->ntraps), 1);
	    for(j=0;j<pt->ntraps;j++) {
		    (void) write(fd,(genericptr_t) pt->traps[j], sizeof(trap));
		    free(pt->traps[j]);
	    }
	    if (pt->ntraps)
		free(pt->traps);

	    /* The monsters */
	    (void) write(fd, &(pt->nmonster), 1);
	    for(j=0;j<pt->nmonster;j++) {
		    (void) write(fd,(genericptr_t) pt->monsters[j], sizeof(monster));
		    free(pt->monsters[j]);
	    }
	    if (pt->nmonster > 0)
		free(pt->monsters);

	    /* The objects */
	    (void) write(fd, &(pt->nobjects), 1);
	    for(j=0;j<pt->nobjects;j++) {
		    (void) write(fd,(genericptr_t) pt->objects[j], sizeof(object));
		    free(pt->objects[j]);
	    }
	    if(pt->nobjects > 0)
		free(pt->objects);

	    /* The drawbridges */
	    (void) write(fd, &(pt->ndrawbridge),1);
	    for(j=0;j<pt->ndrawbridge;j++) {
		    (void) write(fd,(genericptr_t) pt->drawbridges[j], sizeof(drawbridge));
		    free(pt->drawbridges[j]);
	    }
	    if(pt->ndrawbridge > 0)
		free(pt->drawbridges);

	    /* The mazewalk directives */
	    (void) write(fd, &(pt->nwalk), 1);
	    for(j=0; j<pt->nwalk; j++) {
		    (void) write(fd,(genericptr_t) pt->walks[j], sizeof(walk));
		    free(pt->walks[j]);
	    }
	    if (pt->nwalk > 0)
		free(pt->walks);

	    /* The non_diggable directives */
	    (void) write(fd, &(pt->ndig), 1);
	    for(j=0;j<pt->ndig;j++) {
		    (void) write(fd,(genericptr_t) pt->digs[j], sizeof(dig));
		    free(pt->digs[j]);
	    }
	    if (pt->ndig > 0)
		free(pt->digs);

	    /* The ladders */
	    (void) write(fd, &(pt->nlad), 1);
	    for(j=0;j<pt->nlad;j++) {
		    (void) write(fd,(genericptr_t) pt->lads[j], sizeof(lad));
		    free(pt->lads[j]);
	    }
	    if (pt->nlad > 0)
		free(pt->lads);
#ifdef ALTARS
	    /* The altars */
	    (void) write(fd, &(pt->naltar), 1);
	    for(j=0;j<pt->naltar;j++) {
		    (void) write(fd,(genericptr_t) pt->altars[j], sizeof(altar));
		    free(pt->altars[j]);
	    }
	    if (pt->naltar > 0)
		free(pt->altars);
#endif /* ALTARS /**/
	    free(pt);
	}
}

