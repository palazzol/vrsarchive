/*
 *	Generic serial device top half data structures
*/

/*
 *	This data structure parallels the tty structure array and is used
 *	to route requests for serial device services.
*/
struct serialdevice {
	int	(*proc)();		/* Device Manipulation		*/
	void (*connect)();	/* Connect Request routine	*/
	void (*co)();		/* Console Output routine	*/
	int	(*ci)();		/* Console Input routine	*/
};
