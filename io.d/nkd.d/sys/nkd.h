/*
 *	Definitions for EGA display driver
*/
#define COLOR_BASE	0xb8000	/* location of color display memory */
#define EGA_SIZE	32768	/* EGA has at least 32K in planes 0,1 */

struct kdvtinfo {	/* device specific info for kd vt driver */
	unchar			kbmode;		/* keyboard mode */
	unchar			dmode;		/* display mode */
	unchar			flags;		/* flags */
	struct ansistate	as;	/* ansi terminal state */
};

#define	KD_TEXT		0	/* ansi x3.64 emulation mode */
#define	KD_GRAPHICS	1	/* graphics mode */
