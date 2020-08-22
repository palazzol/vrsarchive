/*
 *	%W% %G%
 *	Send a message to a specific socket.
 */
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <sys/sd.h>

static long l = -1;		/* For (char *) casts, kernel style	*/

int
sendto(s, buf, len, flags, to, tolen)
int s;				/* Pseudo-descriptor for socket		*/
char *buf;			/* Buffer to receive data into		*/
int len;			/* Length of buffer provided		*/
int flags;			/* Flags for the operation		*/
struct sockaddr_un *to;
int tolen;
{ 
	char *addr;		/* Address of struct socket		*/
	register struct socket *sptr;
	register struct sock_msg *smsg;
	short *qptr;
	struct sockaddr_un myaddr;
	int msgoff;
	int version;

	if ((addr = _get_sock_addr(s)) == NULL)
		return(-1);
	_enter_seg(addr);
	sptr = (struct socket *)addr;
	if ((sptr->so_state & SS_ISCONNECTED)
		&&  strcmp(to->sun_path, addr+sptr->so_name)) {
		_leave_seg(addr);
		errno = EISCONN;
		return(-1);
	} else
		strcpy(myaddr.sun_path, addr+sptr->so_name);
	_leave_seg(addr);
	/*
	 * Attach the destination socket.
	*/
	if ((addr = _attach_seg(to->sun_path)) == NULL) {
		perror("_attach_seg in sendto");
		abort();
	}
	sptr = (struct socket *)addr;
	for (;;) {
		_enter_seg(addr);
		if (sptr->so_state & SS_CANTSENDMORE) {
			_leave_seg(addr);
			errno = ESHUTDOWN;
			return(-1);
		}
		if (msgoff = _mbuf_alloc(sptr, len+sizeof(struct sock_msg)))
			break;
		version = sdgetv(addr);
		_leave_seg(addr);
		/*
		 *	EINTR from sdwaitv just causes extra iteration.  No
		 *	harm done except EINTR not passed back to user code.
		*/
		sdwaitv(addr, version);
	}
	/*
	 *	Have allocated room to queue the message.  Build the message and
	 *	link it into the queue.
	*/
	smsg = (struct sock_msg *)(addr + msgoff);	
	smsg->sm_next = 0;
	smsg->sm_len = len;
	bcopy(buf, (char *)(smsg+1), len);
	if (!(sptr->so_state & SS_ISCONNECTED))
		strcpy(smsg->sm_from.sun_path, addr+sptr->so_name);
	else
		bcopy((char *)&myaddr,
		      (char *)&smsg->sm_from, sizeof(myaddr));
	if (flags & MSG_OOB) 
		qptr = &sptr->so_oob;
	else
		qptr = &sptr->so_rcv;
	while (*qptr)
		qptr = &((struct sock_msg *)(addr + *qptr))->sm_next;
	*qptr = msgoff;
	_leave_seg(addr);
	return(len);
}
