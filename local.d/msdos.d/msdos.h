/*
 * msdos common header file
 */

#define MSECSIZ	512
#define MDIRSIZ	32

/*
 * If your device driver does not support any of the following formats,
 * then leave those devices undefined.
 */

#define	FLOPPY		"/dev/rflp"
#define	FLP_40_8_1	"/dev/rflp40t8s1s"
#define	FLP_40_8_2	"/dev/rflp40t8s2s"
#define	FLP_40_9_1	"/dev/rflp40t9s1s"
#define	FLP_40_9_2	"/dev/rflp40t9s2s"
#define FLP_80_15_2	"/dev/rflp80t15s2s"
#define FLP_80_9_2	"/dev/rflp80t9s2s"

struct directory {
	unsigned char	name[8];	/* file name */
	unsigned char	ext[3];		/* file extent */
	unsigned char	attr;		/* attribute byte */
	unsigned char	reserved[10];	/* ?? */
	unsigned char	time[2];		
	unsigned char	date[2];
	unsigned char	start[2];	/* starting cluster number */
	unsigned char	size[4];	/* size of the file */
};
