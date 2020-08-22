/*
 *	%W% %G%
 *	Internals to the socket implementation.
*/

/*
 *	Memory allocation data structure.  This has nothing to do with
 *	the BSD concept of an mbuf.
*/
struct mbuf {
	short	sm_length;	/* Length of this allocation	*/
	short	sm_next;	/* Offset in SD to next mbuf	*/
};

/*
 *	The so_oob and so_recv queues are linked lists of messages.  Each
 *	message has a sock_msg header, followed by the data for that message.
*/
struct sock_msg {
	short sm_next;			/* Length of message sent	*/
	short sm_len;			/* Length of message sent	*/
	struct sockaddr_un sm_from;	/* Who message sent by		*/
};

/*
 *	Internal structure of a socket.
 *	Contains send and receive buffer queues, private data, etc.
*/
struct socket {
	short	so_name;		/* offset of socket's name	*/
	short	so_type;		/* SOCK_STREAM, etc		*/
	short	so_options;		/* SOCK_DEBUG, etc		*/
	short	so_linger;		/* time for SO_LINGER		*/
	short	so_state;		/* internal state flags SS_*, below */
	short	so_error;		/* error affecting connection	*/
	short	so_pgrp;		/* pgrp for signals		*/
	/*
	 *	Variables for listen, connect, and accept.
	*/
	struct	sockaddr_un so_q[SOMAXCONN]; /* queue of incoming connections */
	short	so_first;		/* First queued connection on so_q */
	short	so_qlen;		/* number of connections on so_q */
	short	so_qlimit;		/* max number queued connections */
	short	so_timeo;		/* connection timeout		*/
	/*
	 *	Socket buffers - separate buffers for OOB data
	*/
	short	so_oob;			/* Offset of OOB queue head	*/
	short	so_rcv;			/* Offset of In band queue head	*/
	/*
	 *	Memory allocation voodoo.  Must be last.
	*/
	short	so_free;		/* Offset of first free segment	*/
	struct mbuf so_mbuf;		/* Dummy mbuf for mbuf_init()	*/
					/* so_mbuf is part of the memory*/
					/* allocation arena		*/
};

struct sdcache {
  struct sockaddr_un sd_name;		/* Cached segment name		*/
  char *sd_addr;			/* Address in this process	*/
};

#ifndef GLOBAL
#define GLOBAL extern
#endif  GLOBAL

GLOBAL struct sdcache *_sd_cache;	/* BSS - Initialized to NULL	*/
GLOBAL char *_socktab[NOFILE];		/* BSS - Initialized to NULL	*/

/*
 * Socket state bits.
*/
#define	SS_ISBOUND		0x001	/* socket bound to an address	*/
#define	SS_ISCONNECTED		0x002	/* socket connected to a peer	*/
#define	SS_ISCONNECTING		0x004	/* socket connecting to a peer	*/
#define	SS_CANTSENDMORE		0x010	/* can't send more data to peer	*/
#define	SS_CANTRCVMORE		0x020	/* can't receive more data from peer */
#define	SS_NBIO			0x100	/* non-blocking ops		*/

/*
 *	Get the offset of s->f in the structure pointed to by s.
*/
#define offset(s, f)	((char *)&s->f - (char *)s)

/*
 *	Handy min/max macros.  Why aren't these standard C or UNIX?
*/
#define min(a,b)	((a < b) ? a : b)
#define max(a,b)	((a > b) ? a : b)

/*
 *	Size of the implementation of a socket.  Allocated per socket
 *	in user's address space.
*/
#define SDSIZE	4096
#define SDMODE	0666

#ifdef lint			/* Lint doesn't understand arguments	*/
extern void mbuf_init();
extern short mbuf_alloc();
extern void mbuf_free();

extern char *_get_sock_addr();
extern char *_attach_seg();
extern void _enter_seg();
extern void _leave_seg();
#else
extern void mbuf_init(struct socket *);
extern short mbuf_alloc(struct socket *, int);
extern void mbuf_free(struct socket *, short);

extern char *_get_sock_addr(int);
extern char *_attach_seg(char *);
extern void _enter_seg(char *);
extern void _leave_seg(char *);
#endif lint
