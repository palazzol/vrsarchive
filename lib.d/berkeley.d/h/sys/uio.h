/*
 *	%W% %G%
 *	Declare a struct iovec (part of a message header).
*/
struct iovec {
	caddr_t	iov_base;
	int	iov_len;
};
