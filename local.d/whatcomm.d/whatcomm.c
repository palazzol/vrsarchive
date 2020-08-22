/* whatcomm (help) command, written by Robert K Ware, University of Denver
 *
 * This command is intended for new unix users who are having trouble 
 * finding the name of a command.  Typically they will know what they wish
 * to do, but have not found the command using man or man -k.
 *
 * The command uses a file (FILENAME below) that has the command lines in an
 * outline form.  Format:
 *	There may not be any empty or blank lines.
 *	The level in the outline is denoted by the number of leading blanks
 *	divided by 2.  The deepest level is the command name followed by
 *	a short (one line) description (obtained with man -k <command>)
 *
 *	Format example:

Access control        (login control, etc)
  chfn          change full name of user
  chsh          change default login shell
  mesg          permit or deny messages
  passwd        change login password
  su            substitute user id temporarily
Communication         (i/o)
  Graphics
    graph       draw a graph
    plot        graphics filters
    spline      interpolate smooth curve
    vpr         raster printer/plotter spooler
  Machine to machine
    ftp         file transfer program
    gets        get a string from standard input
    lpq         spool queue examination program
    lpr         line printer spooler

 *
 * To install:
 *
 * 1. Create the data file described above or use the example supplied with
 * the source package.
 * 
 * 2. Define FILENAME to match the path to the above data file.
 *
 * 3. Compile and install.
 *
 * 4. Install the manual.
 *
 * NOTE: The data base supplied is for 4.2 BSD.  I am sure others can come up
 * with better headings and organization.  If you do, you might wish to share
 * it with the rest of us.
 *
 * 
 * 				Bob Ware 11/9/84
 */
#include <stdio.h>

#ifndef FILENAME
#define FILENAME "./whatlist"	/* file name for data list */
#endif
#define MAXSTR 80			/* max string length of items */
#define CKSIZE	(sizeof(struct unpadded))
					/* size of needed structure */
#define PADSIZE	(((CKSIZE)/sizeof(long) + 1) * sizeof(long))
					/* size of structure padded to long */
#define PAD	(PADSIZE - CKSIZE)	/* size of needed pad */
#define SIZE	(sizeof(struct list))	/* size of list structure */
#define MAXCT	16			/* list size to expand each time */
#define NULLPTR	((struct list *)NULL)
#define QUIT -1			/* 'q' response from user mapped to QUIT */
#define BAD -2			/* invalid response from user mapped to BAD */

struct unpadded 
{
	struct list *_next;	/* the next item at same level */
	struct list *_down;	/* the first item at next lower level */
	struct list *_up;	/* the parent item at higher level */
	char _str[MAXSTR+1];	/* one line from command list file */
};

struct list 
{
	struct unpadded u;	/* The unpadded structure */
#define next u._next
#define down u._down
#define up   u._up
#define str  u._str
	char pad[PAD];		/* pad structure to even long */
};
char *argv0;			/* pointer to command name */

main(argc, argv)
int argc;
char *argv[];
{
/*....................VARIABLES...............................................*/
	struct list *loadlist();	/* pointer to start of list   */
	struct list *item_ptr;		/* pointer to current item of list    */
#ifdef bugs
	struct list *send_man();	/* send manual and reset item pointer */
#else
	void send_man();		/* send manual			      */
#endif
	int item_count;			/* max item number for a list segment */
	int item, get_item();		/* response code from user	      *
					 *   invalid response = BAD	      *
					 *   'q' = QUIT			      *
					 *   0 = backup to previous level     *
					 *   1 to item_count = valid list no. */

/*...................INIALIZE.................................................*/
	argv0 = argv[0];		/* innialize command name */
	item_ptr = loadlist();		/* load data list from FILENAME */

/*...................MAIN LOOP................................................*/
	for(item = 0; item != QUIT;)	/* keep going until QUIT response */
	{
		if (item == 0)			/* for response = 'up' */
		{
			if(item_ptr->up != NULLPTR)
				item_ptr = item_ptr->up; 
			item_count = send_list(item_ptr->down);
		}
		else if(item == BAD)		/* send a prompt for input */
		{
			bad_prompt(item_count);
                        if(item_ptr->down == NULLPTR)
                                item_ptr = item_ptr->up;
                        item_count = send_list(item_ptr->down);
		}
		else 				/* for valid response */
		{
			/* set new pointer */
			item_ptr = item_ptr->down;
			while(item-- > 1)
				item_ptr = item_ptr->next;

			/* send to stdio */
#ifdef bugs
			if (item_ptr->down == NULLPTR)	/* must be manual */
				item_ptr = send_man(item_ptr);
			else				/* else must be list */
				item_count = send_list(item_ptr->down);
#else
			if(item_ptr->down == NULLPTR) {	/* must be manual */
				send_man(item_ptr);
				item_ptr = item_ptr->up;
			}
			item_count = send_list(item_ptr->down);
#endif
	
		}
		prompt(item_count);

		item = get_item(item_count);	/* get input code from tty */
	}

/*.....................EXIT................................................*/
	printf("\nDid not find the command you wanted?\n");
	printf("\tThen try: apropos <keyword>\n");
}
struct list *loadlist()
{
	/********************************************************
	*  1. Opens the data file (FILENAME)                    *
	*  2. Interprets and loads a linked list of structures  *
	*     from the date                                     *
	*  3. Makes memory available as needed for the list     *
	*  4. Returns a pointer to the first start of the list  *
	********************************************************/

/*.........................VARIABLES....................................*/
	extern char *calloc();

	FILE *file_ptr;			/* file pointer */
	register int i;			/* all purpose counter */
	register int chr_ct;		/* position counter for string */
	register int this_level = -1;	/* level number (-1 = top) */
	register int list_count ;	/* item counter for the list */
	register int last_level;	/* previous level number */
	char string[MAXSTR+1];		/* i/o buffer for data */
	struct list *this_ptr;		/* current list pointer */
	struct list *last_ptr;		/* last list pointer */
	struct list *start_ptr;		/* point to start of list */

/*.........................open data file....................................*/
	if((file_ptr = fopen(FILENAME, "r")) == NULL)
	{
		fprintf(stderr,"Attempting open: %s\n",FILENAME);
		bomb("Unable to open above data file");
	}
/*.........................allocate first chunk of memory....................*/
	if((start_ptr = (struct list *)calloc(MAXCT,SIZE)) == 0)
		bomb("out of memory");
	list_count = MAXCT;

/*........................set up initial pointers............................*/
	this_ptr = start_ptr;
	this_ptr++; 
	list_count--;
	this_ptr->up = start_ptr;
        start_ptr->up = NULLPTR;
	start_ptr->down = this_ptr;
	start_ptr->next = NULLPTR;
	last_ptr = start_ptr;

/*.........................main loop.........................................*
 *	Ok, we now have opened the data file, allocated a chunk of memory    *
 *	and set up initial pointer values.				     *
 *     									     *
 *	We now need to:							     *
 *	Read one line from file and copy to structure.  If line was longer   *
 *	than MAXSTR (+ \0) then complain and quit.  And interpret the level  *
 *	and load rest of structure.  					     *
 *	Repeat until end of file is reached.				     *
 *...........................................................................*/

	while((fgets(string,MAXSTR+2,file_ptr)) != NULL)
	{
		/* what level is this item? */
		last_level = this_level;
		for(chr_ct = 0; string[chr_ct] == ' '; chr_ct++);
		this_level = chr_ct/2;

		/* load string in structure */
		for(i=0; chr_ct < MAXSTR+1;)
		{
			if( (string[chr_ct]=='\0') || (string[chr_ct]=='\n') )
				break;
			this_ptr->str[i++] = string[chr_ct++];
		}
		if(chr_ct > MAXSTR)
			bomb("Data file has a line that is too long.");
		this_ptr->str[i] = '\0';

		/* load rest of structure */
		if(this_level > last_level)
		{
			last_ptr->next = NULLPTR;
			last_ptr->down = this_ptr;
			this_ptr->up = last_ptr;
		}
		if(this_level == last_level)
		{
			last_ptr->next = this_ptr;
			last_ptr->down = NULLPTR;
			this_ptr->up = last_ptr->up;
		}
		if(this_level < last_level)
		{
			last_ptr->next = NULLPTR;
			last_ptr->down = NULLPTR;
			while(last_level-- > this_level)
				last_ptr = last_ptr->up;
                        last_ptr->next = this_ptr;
			this_ptr->up = last_ptr->up;
		}

		/* reset this_ptr to next item */
		last_ptr = this_ptr;		/* save it */
		if(list_count-- <= 1)		/* out of memory? */
		{
			if((this_ptr = (struct list *)calloc(MAXCT,SIZE)) == 0)
				bomb("out of memory");
			list_count = MAXCT;
		}
		else
			this_ptr++;		/* no, then next item */
	}

/*......................finish up......................................*/
	last_ptr->next = NULLPTR;
	last_ptr->down = NULLPTR;

	return(start_ptr);
}
bad_prompt(item_count)
int item_count;
{
	printf("\n........................................\n");
	printf(". Input must be a number from 1 to %2d  .\n",item_count);
	printf(".                  OR                  .\n");
	printf(".    '0' to go back one step           .\n");
	printf(".    'q' to quit                       .\n");
	printf("........................................\n\n");
}
prompt(item_count)
int item_count;
{
	printf("Enter # (0-%d) or q (quit): ",item_count);
}
get_item(item_count)
int item_count;
{
	char tstr[MAXSTR+1];		/* input string buffer */
	int item;			/* item number (converted from input) */

	gets(tstr);

	/* interpret the response */

	item = atoi(tstr);

	if ( (*tstr == 'q') || (*tstr == 'Q') )		/* quit? */
		item = QUIT;
	else if( (item < 0) || (item > item_count) )	/* outside range? */
		item = BAD;
	else if ( (*tstr < '0') || (*tstr > '9') )	/* not a number? */
		item = BAD;

	return(item);
}
send_list(item_ptr)
struct list *item_ptr;
{
	/************************************************
	* 1. sends a list of the next items to stdio.	*
	*    (starting at item_ptr)			*
	* 2. Counts the number of items in the list.	*
	* 3. Returns the count.				*
	************************************************/

	int item_count = 0;

	while(item_ptr != NULLPTR)
	{
		item_count++;
		printf("%2d %s\n", item_count, item_ptr->str);
		item_ptr = item_ptr->next;
	}
	return(item_count);
}
#ifdef bugs
struct list *
#else
void
#endif
send_man(item_ptr)
struct list *item_ptr;
{

	char *command;		/* pointer to item string */
	char s[MAXSTR+1];	/* command name */
	int i;			/* scratch pad */
	int pid,sts;

	command = item_ptr->str;

	/* strip white space and trailing info and load command name */

	for(i=0;(i<MAXSTR)&&(*command!=' ')&&(*command!='\t');i++,command++)
		s[i] = *command;
	s[i] = '\0';

	/* attempt to create a process */

	if ((pid = fork()) == -1)
	  	bomb("no processes left - fork failed\n");

	if (pid)
	  { while (wait(&sts) != pid)
		   { if (wait(&sts) == -1) 
#ifdef bugs
				return(item_ptr);
#else
				return;
#endif
 		   }
#ifdef bugs
		return(item_ptr);
#else
		return;
#endif
	  }

	/* do it */

	execlp("man","man",s,0);

#ifdef bugs
	return(item_ptr->up);
#else
	return;
#endif
}
bomb(error_msg)
char *error_msg;
{
	fprintf(stderr,"\n%s: %s\n", argv0, error_msg);
	exit(1);
}
