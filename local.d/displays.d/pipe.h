/*
 *	Structure used to communicate on the pipe.
 *
 *	The pipe is used 
*/
struct pipe_msg {
	int id;			/* Who is speaking?		*/
	int count;		/* > 0 if ch is real		*/
				/* = 0 for EOF			*/
	char ch;		/* Character, if any		*/
};
