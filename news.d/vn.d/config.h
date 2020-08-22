/*
** vn news reader.
**
** config.h - system configuration parameters
**
** see copyright disclaimer / history in vn.c source file
*/

#define DEF_ED "/usr/ucb/vi"	/* editor to use if no EDITOR variable */
#define DEF_PS1 "$ "		/* ! command prompt if no PS1 */
#define DEF_SAVE ""		/* save file */

#define DEF_PRINT "/usr/ucb/lpr"		/* print command */

#define DEF_CCFILE "author_copy"
#define DEF_KEYXLN ".vnkey"

/*
** this is the "pre-typed" string the user will be presented with
** in answer to the "update" question following the QUIT command.
** Set to "" if you don't like it answering "yes" for you, or "y"
** if you only want to have to erase one character to say "no", etc.
*/
#define QUIT_ANSWER "yes"

/*
** default terminal assumed if TERM variable is unset.  Since TERM has to
** be set for most UNIX tools, you probably want to make this something
** which will cause failure, unless EVERYBODY has the same kind of terminal
** or you don't really use a standard UNIX environment.
*/
#define DEF_TERM "<unspecified TERM variable>"

/*
** foreground flag for messages.  applies only if JOBCONTROL undefined
** (SYS V). set to 1 to see newsgroup messages, etc. during reading phase,
** 0 for "silent" operation - be warned that this may suppress some
** non-fatal diagnostic messages - find all references to fgprintf to
** see what is suppressed.
*/
#define NOJOB_FG 1

/*
** arrow key treatment.  If PAGEARROW is defined, right and left arrow
** keys will be synonyms for <return> (next-page) and <backspace> (previous).
** Otherwise, the right arrow will function as down, and the left as up.
** Made configurable because while there is no lateral motion on the screen
** to associate with the right and left arrows, you might not like them
** changing pages on you.
*/
#define PAGEARROW

/*
** if USEVS is defined, terminal initialization / exit for vn will include the
** "vs"/"ve" pair as well as "ti"/"te".  This doesn't matter on a lot of
** terminals, but may make vn display behaviour closer to "vi" since vs/ve
** is vi's "visual mode" sequence.  For instance, I believe the commonly
** used definitions for these strings on multi-page concepts allows the
** program to run in the first page of the terminal, preserving the more
** recent part of your session on exit
**
** #define USEVS
*/

/*
** temp file name template for mktemp().  Used in tmpnam.c, does not apply
** if you use a system library tmpnam().   BE CAREFUL - VNTEMPNAME MUST
** contain a string of 6 X's for mktemp() (actually, a place where 6 X's
** are intended to go).  TMP_XOFFSET absolutely MUST point to the first of
** the X's.  Yes, writing into a literal string is sloppy.  To the best of
** my knowledge, tmpnam.c is the only place you'll find vn code doing it.
** We make this configurable in case you want temp files somewhere else.
*/
#define VNTEMPNAME "/usr/tmp/vnXXXXXX"
#define TMP_XOFFSET 11

/*
** VNLOGFILE and VNSTATFILE.  If these files EXIST, the corresponding data
** collection will be turned on.  If they don't it will be turned off.
** To turn it back on again, create the files empty.  Garbage in VNLOGFILE
** won't hurt collection but VNSTATFILE requires very strict syntax, so
** make sure its always an empty file or EXACTLY the right syntax.  See stat.c
**
** VNLOGFILE logs user sessions.  VNSTATFILE keeps a running breakdown
** of newsgroup activity.  I add these with some hesitancy, as I find
** use of things like this for Gestapo-like purposes repugnant in the
** extreme.  However, they can also be useful for system tuning purposes
** such as verifying what newsgroups are being read, and when load on
** the system due to newsreading is occurring.
**
** If VNLOGFILE and VNSTATFILE are NOT DEFINED, the code for doing logging
** and statistical collection will not be compiled in, saving some overhead,
** and avoiding calls to system functions like ctime() and time() which may
** have system dependent quirks.
**
#define VNLOGFILE "/usr/rti/fe/bobm/vn.log"
#define VNSTATFILE "/usr/rti/fe/bobm/vn.stat"
*/
