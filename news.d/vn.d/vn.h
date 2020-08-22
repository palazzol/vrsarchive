/*
** vn news reader.
**
** vn.h - general parameters
**
** see copyright disclaimer / history in vn.c source file
*/

#define TRUE 1
#define FALSE 0

#define ED_MARK '>'
#define ART_MARK '*'
#define ART_WRITTEN '_'
#define ART_UNWRITTEN ' '


#define ANFORM ":%s - %c for help:\n"
#define ANFLINES 1
#define UDKFORM "undefined key - %c for help"
#define HELPFORM "%c for help"

/*
	command characters - don't use numerics or <ESC>
	ALTSAVE is a hack to avoid having to use ctl-s - XON/XOFF.
	Wanted to preserve "s" pneumonic and lower / control /cap
	convention.
*/
#define DIGEST 'd'
#define UP 'k'
#define DOWN 'j'
#define FORWARD '\012'
#define BACK '\010'
#define READ 'r'
#define ALTREAD ' '
#define READALL 'R'
#define READSTRING '\022'
#define SAVE 's'
#define SAVEALL 'S'
#define SAVESTRING '\023'
#define ALTSAVE '\024'
#define PRINT 'p'
#define PRINTALL 'P'
#define PRINTSTRING '\020'
#define MARK 'x'
#define UNMARK 'X'
#define REDRAW '\014'
#define QUIT 'q'
#define SSTAT '#'
#define GRPLIST '%'
#define ORGGRP 'o'
#define ORGSTAT 'O'
#define UPDATE 'w'
#define UNSUBSCRIBE 'u'
#define UPALL 'W'
#define UPSEEN '\027'
#define UNESC '!'
#define NEWGROUP 'n'
#define HEADTOG 'h'
#define SETROT 'z'
#define HELP '?'
#define TOPMOVE 'H'
#define BOTMOVE 'L'
#define ALTBOTTOM 'G'
#define MIDMOVE 'M'
#define PRTVERSION '"'
#define HELP_HEAD "[...] = effect of optional number preceding command\n\
pipes are specified by filenames beginning with |\n\
articles specified as a list of numbers, title search string, or\n\
	* to specify marked articles.  ! may be used to negate any\n"

#define HHLINES 5	/* lines (CRs + 1) contained in HELP_HEAD */
