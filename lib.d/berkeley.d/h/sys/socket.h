/*
 *	%W% %G%
 *	Definitions related to AF_UNIX socket emulation.
*/

/*
 *	Types of socket
*/
#define	SOCK_STREAM	1		/* stream socket */

/*
 *	Option flags per-socket.
*/
#define	SO_DEBUG	0x01		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x02		/* socket has had listen() */
#define	SO_LINGER	0x80		/* linger on close if data present */
#define	SO_DONTLINGER	(~SO_LINGER)	/* ~SO_LINGER */

/*
 *	Address families.
*/
#define	AF_UNSPEC	0		/* unspecified */
#define	AF_UNIX		1		/* local to host (pipes, portals) */

/*
 *	Structure used by emulation library to store addresses.
*/
struct	sockaddr_un {
	short	sun_family;		/* AF_UNIX */
	char	sun_path[109];		/* path name (gag) */
};
#define sockaddr	sockaddr_un	/* All socket addresses in AF_UNIX */
#define sa_family	sun_family
#define sa_data		sun_path

/*
 *	Protocol families, same as address families for now.
*/
#define	PF_UNSPEC	AF_UNSPEC
#define	PF_UNIX		AF_UNIX

/*
 *	Maximum queue length specifiable by listen.
*/
#define	SOMAXCONN	5

/*
 *	Message header for recvmsg and sendmsg calls and internal message
 *	representation.
*/
struct msghdr {
	caddr_t	msg_name;		/* optional address */
	int	msg_namelen;		/* size of address */
	struct	iovec *msg_iov;		/* scatter/gather array */
	int	msg_iovlen;		/* # elements in msg_iov */
	caddr_t	msg_accrights;		/* access rights sent/received */
	int	msg_accrightslen;
};

#define	MSG_OOB		0x1		/* process out-of-band data */
#define	MSG_PEEK	0x2		/* peek at incoming message */

#define	MSG_MAXIOVLEN	16

/*
 *	Since our types for parameters are not always identical to the
 *	Berkeley types (int vs long, etc), check argument types at compile
 *	time.  You will almost surely get warnings about select() which
 *	cannot be ignored.
*/
#ifdef lint			/* Lint doesn't understand arguments	*/
extern int accept();
extern void bcopy();
extern int bind();
extern int connect();
extern int getsockname();
extern int getsockopt();
extern int listen();
extern int recv();
extern int recvfrom();
extern int recvmsg();
extern int select();
extern int send();
extern int sendto();
extern int sendmsg();
extern int setsockopt();
extern int shutdown();
extern int socket();
extern int socketpair();
#else
extern int accept(int, struct sockaddr_un *, int *);
extern void bcopy(char *, char *, int);
extern int bind(int, struct sockaddr_un *, int);
extern int connect(int, struct sockaddr_un *, int);
extern int getsockname(int, struct sockaddr_un *, int *);
extern int getsockopt(int, int, int, char *, int *);
extern int listen(int, int);
extern int recv(int, char *, int, int);
extern int recvfrom(int, char *, int, int, struct sockaddr_un *, int *);
extern int recvmsg(int, struct msghdr *, int);
extern int select(int, long *, long *, long *, struct timeval *);
extern int send(int, char *, int, int);
extern int sendto(int, char *, int, int, struct sockaddr_un *, int);
extern int sendmsg(int, struct msghdr *, int);
extern int setsockopt(int, int, int, char *, int);
extern int shutdown(int, int);
extern int socket(int, int, int);
extern int socketpair(int, int, int, int *);
#endif lint
