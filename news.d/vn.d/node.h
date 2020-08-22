/*
** vn news reader.
**
** node.h - NODE structure
**
** see copyright disclaimer / history in vn.c source file
*/

/* newsgroup status flags */
#define FLG_SUB 1	/* user subscribed to newsgroup */
#define FLG_PAGE 2	/* a page exists to display */
#define FLG_NEW 4	/* new newsgroup */
#define FLG_ECHG 8	/* edit change by user */
#define FLG_SEARCH 16	/* newsgroup was searched */
#define FLG_ACC 32	/* newsgroup had articles accessed */
#define FLG_STAT 64	/* stat's written */

/*
	newsgroup information (hash table node)

	items unaccessed by server interface:
		next - hashtable link
		pnum - page number
		pages - number of pages for news display
		pgshwn - pages shown mask
		pgrd - article number on highest conecutively shown page
		order - order of appearance in Newsorder array.
		orgrd - original articles read number

	may be read following hashfind by server interface, but not written:
		nd_name - name of newsgroup (key to reach node by)
			this will be a permanent copy of the name.
		highnum - high article number in group
		lownum - low article number in group

	legal for vns_write to read, but not written by server interface:
		flags - bit mask of FLG_xxxx flags.
		rdnum - articles read.

	unused by vn user interface, intended for use by server interface:
		state - state variable.  initted 0.
		data - arbitrary data pointer.  initted NULL.
*/
typedef struct _node
{
	struct _node *next;
	char *nd_name;
	int highnum, lownum;
	int pnum, pages, rdnum, orgrd, pgrd;
	unsigned long pgshwn;
	unsigned flags;
	int order;
	unsigned state;
	char *data;
} NODE;
