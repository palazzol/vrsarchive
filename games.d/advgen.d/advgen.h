#include	<stdio.h>

typedef int		bool;
typedef	char		smallbool;
typedef unsigned short	smallint;

#define	TRUE		1
#define	FALSE		0

#ifndef	DEFINTERPRETER
#define DEFINTERPRETER	"/usr/games/adv"	/*  Program to run output */
#endif	DEFINTERPRETER
#define	DEFWORDSIZE	3	/* Number of significant chars. in vocab. */
#define DEFINVSIZE	5
#define	DEFLLIFE	240	/* Lamp life */
#define DEFLWARN	25	/* Lamp warning */
#define	DEFOUTPUT	"adv.out"

#define	MAXPARSEERRORS	40

/*  Most of the maxima below are imposed by the interpreter's use of
 *  1-byte integers for storing indices.
 */

#ifndef	MAXCODE
#define	MAXCODE		50000	/* Generated code for daemons and rules */
#endif	MAXCODE
#ifndef	MAXVOCAB
#define	MAXVOCAB	1000	/* Words in noun or verb table */
#endif	MAXVOCAB
#define	MAXROOMS	255	/* Rooms/Locations */
#define	MAXITEMS	255	/* Items */
#define	MAXVARS		255	/* Variable names */
#define	MAXMSGS		255	/* Strings */
#define	MAXWSIZE	20	/* Significant chars. in words (warning) */
#define MAXINVSIZE	30	/* Maximum reasonable inventory size */
#define MAXWORDLIST	25	/* Synonyms for special verbs */

#define	LIMBO	0

typedef struct symbol
{
    char	*s_sym;		/* symbol name */
    smallint	s_type;
#	define		S_UNKNOWN	0
#	define		S_SYNTAX	1
#	define		S_ROOM		2
#	define		S_ITEM		3
#	define		S_VAR		4
#	define		S_MSG		5
    smallint	s_value;	/* meaning depends on type */
    smallint	s_noun;		/* index into noun table */
    smallint	s_verb;		/* index into verb table */
}			symbol;

typedef struct room
{
    char	*r_name;
    char	*r_desc;
    smallint	r_neighbours[6];
    smallbool	r_dark;
    smallbool	r_declared;		/* To cope forward references */
}			room;

typedef struct item
{
    char	*i_name;
    char	*i_desc;
    smallint	i_noun;		/* Word used for this, if any */
    smallint	i_room;		/* Where it starts */
}			item;

typedef struct vocab
{
    char	*v_word;
    smallint	v_value;		/* Equal values for synonyms */
    smallbool	v_dup;			/* Flag for indistinguishable words */
}			vocab;

typedef struct wordtable
{
    smallint	wt_size;		/* Number of entries */
    smallint	wt_wcount;		/* Ditto, excluding duplicates */
    smallint	wt_value;		/* Ditto, excluding synonyms */
    vocab	wt_vocab[MAXVOCAB];
}			wordtable;

typedef struct wordlist		/* Mini-wordtable for special verbs/nouns */
{
    int		wl_count;
    char	*wl_word[MAXWORDLIST];
}			wordlist;

#define	streq(s, t)	(*(s) == *(t) && strcmp((s), (t)) == 0)

#define		CNULL	(char *) NULL
#define		SNULL	(symbol *) NULL
#define		SPNULL	(symbol **) NULL
#define 	WNULL	(wordtable *) NULL

#define		NOTFOUND	255

#include	"extfuncs.h"
