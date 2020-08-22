/*
** vn news reader.
**
** page.h - display page structure
**
** see copyright disclaimer / history in vn.c source file
*/

/*
	page display format and dependent parameters
*/
#define HFORMAT "\n%s (page %d of %d):"
#define DHFORMAT "\n%s (DIGEST EXTRACTION):"
#define TFORMAT "%s ~ %s %s"
#define AFORMAT "\n%c%c%d) "	/* begin with newline - see show routine */
#define AFLEN 5		/* min. char. in article id - depends on AFORMAT */
#define CFORMAT "page %d of %d (%d shown), newsgroup %d of %d"
#define RECBIAS 2	/* lines before articles - depends on HFORMAT */
#define WRCOL 1		/* column of written mark.  depends on AFORMAT */
#define INFOLINE 0	/* HFORMAT TFORMAT leaves for use */
#define REQLINES 7	/* required terminal lines for display, incl. help */

/*
	newsgroup information for page display
	name - of group
	group - pointer to table entry
	artnum - number of articles
*/
typedef struct
{
	char *name;
	NODE *group;
	int artnum;
} HEAD;

/*
	article information - id (spool) number, title string, mark, written.
*/
typedef struct
{
	int art_id;
	char art_mark;
	char art_t[MAX_C-AFLEN];
} BODY;

typedef struct
{
	HEAD h;
	BODY *b;
} PAGE;
