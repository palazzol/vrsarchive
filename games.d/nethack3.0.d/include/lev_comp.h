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

#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

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




#ifndef YYSTYPE
#line 141 "lev_comp.y"
typedef union {
	int	i;
	char*	map;
} yystype;
/* Line 1281 of /usr/share/bison/yacc.c.  */
#line 119 "y.tab.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;


#endif /* not BISON_Y_TAB_H */

