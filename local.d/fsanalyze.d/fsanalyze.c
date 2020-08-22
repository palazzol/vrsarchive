/*
 * fsanalyze.c - file system analyzer - v2.04; 7 January 1988
 *
 * Author   : Michael J. Young
 * USmail   : Software Development Technologies, Inc.
 *            375 Dutton Rd
 *            Sudbury MA 01776
 * UUCP     : {decvax|harvard|linus|mit-eddie}!necntc!necis!mrst!sdti!mjy
 * Internet : mjy%sdti.uucp@harvard.harvard.edu
 *
 * =========================================================================
 * Note : This program has been placed in the public domain to permit
 * unrestricted distribution and use.  I have placed no copyright on it, but
 * I request that you keep me informed about any enhancements and bug fixes
 * you make so I can keep an up-to-date copy for further distribution.
 * =========================================================================
 *
 * fsanalyze is a simple tool that estimates file system fragmentation.  It
 * accomplishes this by scanning the data blocks for each i-node in the 
 * file system, looking for block numbers that are out of sequence.  
 * Fragmentation is then computed as the ratio of out-of-sequence blocks to
 * total data blocks.
 * 
 * fsanalyze also provides statistics regarding the number (and identity) of
 * files that are very large, and especially excessively large directories 
 * (which are very inefficient).
 * 
 * To build fsanalyze the steps are:
 *       cc -O -o fsanalyze fsanalyze.c
 *       mv fsanalyze /bin
 *
 * Usage:
 *    fsanalyze [-flags] special [file] ...
 *
 *    where [flags] include the following:
 *      d   display i-node numbers as they are examined
 *      e   report file size inconsistencies
 *      i   report inodes numbers with double and triple indirection
 *	o   override error checking on file system superblock
 *
 *    If the optional file argument(s) are present, only the specified
 *    individual files on the file system are analyzed.  If absent, the
 *    entire file system is analyzed.
 *
 *    Example:
 *      fsanalyze /dev/dsk/0s2
 *
 *    Since fsanalyze uses the superblock info ON THE DISK, more accurate
 *    results will be returned if sync(1) is executed immediately prior to
 *    fsanalyze.
 */

/*
 * Modification History:
 *
 *    Date       Author                   Description
 * -----------  --------  -----------------------------------------------
 * 28 Jul 1987    MJY       Originated
 *  5 Oct 1987    MJY       Capability to analyze individual files,
 *			    Added error checking to file system argument,
 *			    Added -o flag
 *			    Prints out volume and file system name in summary
 * 12 Oct 1987    MJY       Use /etc/fsstat to do file system validity
 *                          checking
 *  9 Nov 1987    MJY       print out warning if file system is mounted
 * 12 Nov 1987    MJY       Volume size statistics now long instead of int
 *  7 Jan 1988    MJY       Modified blk_no() to use l3tol()
 */


# include <stdio.h>
# include <sys/types.h>
# include <sys/ino.h>
# include <sys/param.h>
# include <sys/filsys.h>
# include <sys/stat.h>

# define BLK_SIZE	512		/* block size */
# define IBLK 		2		/* block number of first i-node */

# define I_BLOCK_FACTOR	32		/* number of i-nodes per block */

/*
 * file mode definitions
 */
# define FILE_TYPE(a) 	((a)->di_mode & S_IFMT)
# define IS_SPECIAL(a)	(((a) & S_IFMT == S_IFBLK) || ((a) & S_IFMT == S_IFCHR) || ((a) & S_IFMT == S_IFIFO))

/*
 * basic definitions
 */
# define FALSE	0
# define TRUE	1

typedef int boolean;

/*
 * per-file statistics structure
 */
struct file_data {
	int inode;			/* i-node number */
	long total_blocks;		/* total blocks in file (incl 
					 * indirect blocks */
	long data_blocks;		/* total data blocks in file */
	long potential_seeks;		/* number of potential seeks in file */
	long seeks;			/* actual seeks in file */
	float fragm;			/* fragmentation (seeks/pot.seeks) */
	};

FILE *fsys               = NULL;	/* file system under test */
char *special            = NULL;	/* file system device name */

/*
 * file system static data
 */
struct filsys super      = {0};		/* holds file system super block */
int fs_type              = 1;		/* file system logical block size
					 * (in 512 byte units) */
long num_inodes          = 0;		/* number of i-nodes in file system */

/*
 * calculated global statistics
 */
long blocks              = 0;		/* running block count */
long free_inodes         = 0;		/* number of unused i-nodes */
long potential_seeks     = 0;		/* potential number of disk seeks
					 * during sequential access of a 
					 * file */
long seeks               = 0;		/* actual number of disk seeks
					 * required during sequential access
					 * of a file */
long indirects           = 0;		/* number of files w/ more than
					 * 10 data blocks */
long double_indirects    = 0;		/* number of files w/ more than
					/* one level of indirection */
long triple_indirects    = 0;		/* number of files w/ more than
					 * two levels of indirection */
int big_directories      = 0;		/* number of directories with
					 * indirection */
int num_directories      = 0;		/* number of directories */
int linked_files         = 0;		/* number of multiply-linked files */
int size_errors          = 0;		/* number of file size discrepancies */


struct file_data file_log[10] = {0};		/* 10 worst offenders */


/*
 * fsanalyze command-line flags
 */
boolean report_indirects = FALSE;	/* command line option -- report
					 * files with indirect data blocks */
boolean report_errors    = FALSE;	/* command line option -- report
					 * file size discrepancies */
boolean debug            = FALSE;	/* report i-node #s as they are
					 * examined */
boolean override	 = FALSE;	/* override bad fs test */


/*
 * interface to system error messages
 */
extern int errno;
extern char *sys_errlist[];
extern int sys_nerr;

/*
 * usage : prints out a short message describing command usage.  This
 * function does not return.
 */
void usage(){
	printf ("File System analyzer - v2.01\n");
	printf ("Usage:\n   fsanalyze [-[deio]] special [file] ...\n");
	printf ("\n\tIf the [file] argument(s) are missing, the entire\n");
	printf ("\tfile system is scanned.  Otherwise, only the specified\n");
	printf ("\tfiles are examined.  Valid flag are:\n\n");
	printf ("\td\tdisplay i-node numbers as they are examined\n");
	printf ("\te\treport file size inconsistencies\n");
	printf ("\ti\treport data block double and triple indirection\n");
	printf ("\to\toverride error checking on file system argument\n");
	exit(1);
	}

/*
 * check_fs : checks the file system to be sure it is not in need of
 * checking.  If the file system is damaged, the function displays a
 * message from fsstat, then returns FALSE.  Othersize, returns TRUE.
 */
boolean check_fs (special)
char *special;
{
	char buffer[256];			/* buffer to hold cmd */
	int fstat;				/* exit status of fsstat */

	sprintf (buffer, "/etc/fsstat %s 2>/dev/null", special);
	fstat = system (buffer);
	if (fstat != 0 && !override){
		if (fstat == 512 /* why is this byte-swapped? */){
			fprintf (stderr, "warning: file system is mounted\n");
			}
		else {
			/*
			 * run fsstat again to get error message (this is a kludge,
			 * but I hope more portable that having fsanalyze do the
			 * analysis itself)
			 */
			sprintf (buffer, "/etc/fsstat %s", special);
			system(buffer);
			return (FALSE);
			}
		}
	return (TRUE);
	}

/*
 * error : prints out a formatted error string to stderr, followed if
 * possible by an appropriate system error message.  Control is then
 * returned to the system with an error status.  This function does not
 * return.
 */
void error (err, str, arg1, arg2)
int err;			/* value of errno at time of call */
char *str;			/* error string */
char *arg1, *arg2;		/* additional printf arguments, if any */
{
	fprintf (stderr, str, arg1, arg2);
	if (err <= sys_nerr && err > 0)
		fprintf (stderr, "%s\n", sys_errlist[err]);
	else
		fprintf (stderr, "unknown error : %d\n", err);
	exit(1);			/* exit with error status */
	}

/*
 * init_stats : initializes per-file statistics structure
 */
void init_stats (data, inode)
struct file_data *data;				/* file stats to init */
int inode;					/* i-node number */
{
	data->inode = inode;
	data->total_blocks = data->data_blocks = 0;
	data->potential_seeks = data->seeks = 0;
	data->fragm = 0.0;
	}

/*
 * init : parses command line flags and the file system device name, then
 * reads the file system super block.  init returns the number of the
 * parameter AFTER the file system device name.  All subsequent parameters
 * are assumed to be specific files to be tested.
 */
int init (argc, argv)
int argc;
char *argv[];
{
	int i;					/* loop counter */
	char *cp;				/* cmd-line flag pointer */
	boolean special_found = FALSE;		/* TRUE = found special arg */
	
	for (i = 1; i < argc && !special_found; i++){
		cp = argv[i];
		if (*cp == '-'){
			while (*++cp){
				switch (*cp){

						/* report i-nodes with
						 * one or more levels of
						 * indirection */
					case 'i':
						report_indirects = TRUE;
						break;

						/* report file size errors */
					case 'e':
						report_errors = TRUE;
						break;

						/* debugging flag */
					case 'd':
						debug = TRUE;
						break;

						/* override bad fs test */
					case 'o':
						override = TRUE;
						break;
					default:
						fprintf (stderr, "illegal option : %c\n", *cp);
						usage();
					}
				}
			}
		else {
			special = argv[i];
			special_found = TRUE;
			}
		}
	if (special == NULL)usage();		/* no fs parameter */

	/*
	 * check for good file system (let fsstat do the dirty work!)
	 */
	if (check_fs (special)){

		/*
		 * open file system and read the super block
		 */
		if ((fsys=fopen (special, "r")) == NULL){
			error (errno, "error opening \"%s\"\n", special);
			/* NOTREACHED */
			}
		if (fseek (fsys, 512L, 0)){
			error (errno, "error seeking superblock");
			/* NOTREACHED */
			}
		if (fread (&super, sizeof (struct filsys), 1, fsys) != 1){
			error (errno, "error reading superblock");
			/* NOTREACHED */
			}

		num_inodes = (super.s_isize-2) * 16;
		if (super.s_magic != FsMAGIC)
			fs_type = Fs1b;
		else fs_type = super.s_type;

		return i;		/* return # of next argument to be processed */
		}
	else {
		exit(1);		/* bad file system, error status */
		}
	}

/*
 * blk_no : given a pointer to a 3-byte binary number, returns a (long)
 * block number.  Used to access the first 10 data block numbers of an
 * i-node.  The function l3tol is used for portability.
 */
daddr_t blk_no (off)
unsigned char off[];				/* 3-byte offset */
{
	extern void l3tol();
	daddr_t temp = 0;
	int n;

	l3tol (&temp, off, 1);
	return temp;
	}

/*
 * get_inodes : given an initial i-node number, reads a block of i-nodes
 * into an i-node array.
 */
void get_inodes (in, inp, num)
int in;						/* inode number */
int num;					/* # inodes to get */
struct dinode *inp;				/* buffer to hold info */
{
	long position;				/* computed position of the
						 * first requested i-node */

	position = (IBLK * BLK_SIZE * fs_type) + 
		   (sizeof (struct dinode) * ((long)in-1));

	if (fseek (fsys, position, 0)){
		error (errno, "\nerror seeking inode %d, pos = %ld\n", in, position);
		/* NOTREACHED */
		}
	else {
		if (fread (inp, sizeof (struct dinode), num, fsys) != num){
			error (errno, "\nerror reading inode %d\n", in);
			/* NOTREACHED */
			}
		}
	}

/*
 * check_indirects : scans a block containing data block numbers, looking
 * for block numbers that are not sequential.
 */
daddr_t check_indirects (block, cur_pos, data)
daddr_t block;					/* indirect block to check */
daddr_t cur_pos;				/* current block offset */
struct file_data *data;				/* current file statistics */
{
	daddr_t indirect_blk[256];		/* holds an indirect block */
	int num_blocks;				/* number of data blocks in
						 * an indirect block */
	daddr_t pos;				/* current data block */
	daddr_t new_pos;			/* next data block */
	int i;					/* loop counter */

	num_blocks = 128 * fs_type;
	data->total_blocks++;

	/*
	 * get the indirect block
	 */
	if (fseek (fsys, block * 512L * fs_type, 0)){
		error (errno, "\nerror seeking indirect block %ld\n", block);
		/* NOTREACHED */
		}
	if (fread (indirect_blk, sizeof (daddr_t), num_blocks, fsys) != num_blocks){
		error (errno, "\nerror reading indirect block %ld\n", block);
		/* NOTREACHED */
		}
	pos = cur_pos;

	/*
	 * scan the data blocks looking for numbers out of sequence
	 */
	for (i = 0; i < num_blocks; i++){
		new_pos = indirect_blk [i];
		if (new_pos == 0){
			return pos;
			}
		data->data_blocks++;
		data->total_blocks++;
		data->potential_seeks++;
		if (new_pos != pos + 1){
			data->seeks++;
			}
		pos = new_pos;
		}
	return pos;
	}

/*
 * check_double_indirects : scans a block containing a list of indirect
 * blocks, checking for data block numbers that are out of sequence.
 */
daddr_t check_double_indirects (block, cur_pos, data)
daddr_t block;					/* indirect block to check */
daddr_t cur_pos;				/* current block offset */
struct file_data *data;				/* current file statistics */
{
	daddr_t dindirect_blk[256];		/* holds a double-indirect
						 * block */
	int i;					/* loop counter */
	int num_blocks;				/* number of indirect blocks
						 * in a d-i block */

	num_blocks = 128 * fs_type;
	data->total_blocks++;

	/*
	 * the double-indirect block itself should be in sequence with the
	 * data blocks
	 */
	data->potential_seeks++;
	if (block != cur_pos + 1){
		data->seeks++;
		}

	/*
	 * get the d-i block
	 */
	if (fseek (fsys, block * 512L * fs_type, 0)){
		error (errno, "\nerror seeking double indirect block %ld\n",
			 block);
		/* NOTREACHED */
		}
	if (fread (dindirect_blk, sizeof (daddr_t), num_blocks, fsys) != num_blocks){
		error (errno, "\nerror reading double indirect block %ld\n",
			 block);
		/* NOTREACHED */
		}

	/*
	 * scan through the d-i block
	 */
	for (i = 0; i < num_blocks; i++){
		if (dindirect_blk[i] == 0){
			break;
			}
		cur_pos = check_indirects (dindirect_blk[i],
					   block,
					   data);
		}
	return cur_pos;
	}

/*
 * check_triple_indirects : scans a block containing a list of double
 * indirect blocks, looking for data block numbers that are out of sequence.
 */
daddr_t check_triple_indirects (block, cur_pos, data)
daddr_t block;					/* indirect block to check */
daddr_t cur_pos;				/* current block offset */
struct file_data *data;				/* current file statistics */
{
	daddr_t tindirect_blk[256];		/* holds a triple-indirect
						 * block */
	int i;					/* loop counter */
	int num_blocks;				/* number of double-indirect
						 * blocks in a triple-i blk */


	num_blocks = 128 * fs_type;
	data->total_blocks++;

	/*
	 * the triple-indirect block itself should be in sequence with the
	 * data blocks
	 */
	data->potential_seeks++;
	if (block != cur_pos + 1){
		data->seeks++;
		}

	/*
	 * get the t-i block
	 */
	if (fseek (fsys, block * 512L * fs_type, 0)){
		error (errno, "\nerror seeking triple indirect block %ld\n",
			 block);
		/* NOTREACHED */
		}
	if (fread (tindirect_blk, sizeof (daddr_t), num_blocks, fsys) != num_blocks){
		error (errno, "\nerror reading triple indirect block %ld\n",
			 block);
		/* NOTREACHED */
		}

	/*
	 * scan through the t-i block
	 */
	for (i = 0; i < num_blocks; i++){
		if (tindirect_blk[i] == 0){
			break;
			}
		cur_pos = check_double_indirects (tindirect_blk[i],
						  block,
						  data);
		}
	return cur_pos;
	}

/*
 * check_file : scans the data block numbers of an i-node, looking for
 * block numbers that are out of sequence, and would thus result in excess
 * track-to-track seeking.  This function also checks directory files for
 * indirection (more than 10 data blocks), and performs a simple consistency
 * check on all file sizes
 */
void check_file (inode, inode_number, data)
struct dinode *inode;				/* inode info structure */
int inode_number;				/* inode number to be
						 * checked */
struct file_data *data;				/* current file statistics */
{
	daddr_t pos;				/* current block */
	daddr_t new_pos;			/* next block in file */
	int i;					/* loop counter */
	long file_size;				/* file size computed by
						 * actual byte count */

	pos = blk_no (&inode->di_addr[0]);	/* first data block */

	if (inode->di_size == 0 || pos == 0)return; /* ignore 0-size files */

	data->data_blocks = data->total_blocks = 1;

	/*
	 * do some simple-minded statistics gathering
	 */
	if (FILE_TYPE (inode) == S_IFDIR){ /* got a directory */
		num_directories++;
		}
	if (inode->di_nlink > 1){		/* multi-linked files */
		linked_files++;
		}

	/*
	 * scan the data blocks looking for numbers out of sequence
	 */
	for (i = 1; i < 10; i++){
		new_pos = blk_no (&inode->di_addr[i*3]);
		if (new_pos == 0){		/* end of file */
			break;
			}
		data->data_blocks++;
		data->total_blocks++;
		data->potential_seeks++;
		if (new_pos != pos + 1){	/* out of sequence ? */
			data->seeks++;
			}
		pos = new_pos;
		}

	/*
	 * block 10, if non-zero, is the number of the block which contains
	 * the next (128 * fs_type) data block numbers.  It should also be
	 * in sequence with the data blocks.  Block 10 is called an
	 * "indirect" block.
	 */
	if (blk_no (&inode->di_addr[10*3])){
		indirects++;

		/*
		 * if a directory contains indirection, it is too large for
		 * efficient access.  Report it.
		 */
		if (FILE_TYPE (inode) == S_IFDIR){
			printf ("inode %d is a large directory\n", inode_number);
			big_directories++;
			}
		pos = check_indirects (blk_no (&inode->di_addr[10*3]),
				       pos,
				       data);
		}

	/*
	 * block 11, if non-zero, is the number of the block which contains
	 * the next (128 * fs_type) INDIRECT block numbers.  It should also be
	 * in sequence with the data blocks.  Block 11 is called a "double-
	 * indirect" block.
	 */
	if (blk_no (&inode->di_addr[11*3])){
		double_indirects++;
		if (report_indirects){
			printf ("double indirection : %d\n", inode_number);
			}
		pos = check_double_indirects (blk_no (&inode->di_addr[11*3]),
					      pos,
					      data);
		}

	/*
	 * block 12, if non-zero, is the number of the block which contains
	 * the next (128 * fs_type) DOUBLE-INDIRECT block numbers.  It should
	 * also be in sequence with the data blocks.  Block 12 is called a
	 * "triple-indirect" block.
	 */
	if (blk_no (&inode->di_addr[12*3])){
		triple_indirects++;
		if (report_indirects){
			printf ("triple indirection : %d\n", inode_number);
			}
		pos = check_triple_indirects (blk_no (&inode->di_addr[12*3]),
					      pos,
					      data);
		}

	/*
	 * do a simple check to detect possible file-size errors (a la
	 * fsck phase 1)
	 */
	file_size = (inode->di_size + (512 * fs_type - 1)) / (512 * fs_type);
	if (file_size != data->data_blocks){
		size_errors++;
		if (report_errors){
			printf ("inode %d, inconsistent file size : actual blocks = %ld, computed = %ld (%ld bytes)\n",
				inode_number, data->data_blocks, 
				file_size, inode->di_size);
			}
		}
	}

/*
 * log_stats : updates global statistics based on the current file statistics.
 * The current file is then checked to see if it qualifies as one of the 10
 * worst offenders (i.e., most fragmented) encountered thus far.  The 10 worst
 * offenders are determined based on their absolute number of disk seeks
 * required to read the entire file.  Such an absolute test (viz a viz a
 * relative percentage test) ensures that very small, but fragmented, files
 * will not clutter the output.
 */
void log_stats (data)
struct file_data *data;				/* file statistics to be
						 * logged */
{
	int i, j;				/* loop counters */


	/*
	 * update global statistics
	 */
	blocks += data->total_blocks;
	potential_seeks += data->potential_seeks;
	seeks += data->seeks;
	data->fragm = data->potential_seeks ? (float)data->seeks/(float)data->potential_seeks : 0.0;

	/*
	 * update 10 worst offender array
	 */
	for (i = 0; i < 10; i++){
		if (data->seeks > file_log[i].seeks){
			for (j = 9; j > i; j--){
				file_log[j] = file_log[j-1];
				}
			file_log[i] = *data;
			break;
			}
		}
	}

/*
 * scan : scan through each i-node of a file system, compiling statistics
 * regarding fragmentation and indirection.
 */
void scan (){
	int	i, j;				/* loop counters */
	struct dinode i_node[I_BLOCK_FACTOR];	/* holds a block of inodes */
	struct file_data data;			/* per-inode statistics */

	for (i = 1; i < num_inodes; i+=I_BLOCK_FACTOR){

		/*
		 * for efficiency, read a block of i-nodes at a time
		 */
		get_inodes (i, i_node, I_BLOCK_FACTOR);

		/*
		 * scan through each i-node that was read in
		 */
		for (j = 0; i+j < num_inodes && j < I_BLOCK_FACTOR; j++){
			if (debug){
				printf ("inode %d\r", i+j);
				}
			if (i+j <= 1)continue;	/* don't scan i-node 1 */
			if (i_node[j].di_mode != 0){ /* unused i-node ? */
				init_stats (&data, i+j);
				check_file (&i_node[j], i+j, &data); /* scan blocks in file */
				log_stats (&data);
				}
			else {
				free_inodes++;
				}
			}
		}
	}

/*
 * print_report : calculates percentages and prints a summary report of
 * file system statistics
 */
void print_report (){
	long	num_files;			/* number of inodes used
						 * in file system */
	int	i;				/* loop counter */
	
	float	fragm,				/* percent fragmentation */
		ind_p,				/* percent indirections */
		dind_p,				/* percent double indirects */
		tind_p,				/* percent triple indirects */
		bused_p,			/* percent data blocks used */
		iused_p,			/* percent inodes used */
		bdir_p;				/* percent of directories
						 * that have more than 10
						 * blocks */

	/*
	 * calculate percentages for report
	 */
	fragm = potential_seeks ? (float)seeks/(float)potential_seeks : 0.0;
	bused_p = (float)blocks/(float)(blocks+super.s_tfree);
	num_files = num_inodes - free_inodes;
	iused_p = (float)num_files/(float)num_inodes;
	ind_p = num_files ? (float)indirects/(float)num_files : 0.0;
	dind_p = num_files ? (float)double_indirects/(float)num_files : 0.0;
	tind_p = num_files ? (float)triple_indirects/(float)num_files : 0.0;
	bdir_p = num_directories ? (float)big_directories/(float)num_directories : 0.0;

	/*
	 * print out report
	 */
	printf ("\n\nFile system name = \"%.6s\", Volume name = \"%.6s\"\n",
		super.s_fname, super.s_fpack);
	printf ("File system logical block size = %d bytes\n", 512 * fs_type);
	printf ("Volume Size = %ld blocks (%ld bytes)\n",
		super.s_fsize, super.s_fsize * 512L * fs_type);
	printf ("\t%u blocks reserved for super block and inodes\n", super.s_isize);
	printf ("\t%lu blocks reserved for data\n", 
		super.s_fsize - super.s_isize);
	printf ("Total inodes = %d\n", num_inodes);
	printf ("%.2f%% inodes used (%ld used, %ld free)\n", 
		iused_p*100, num_inodes - free_inodes, free_inodes);
	printf ("%.2f%% data blocks used (%ld used, %ld free)\n",
		bused_p*100, blocks, super.s_tfree);
	printf ("%d directories\n", num_directories);
	printf ("%d multiply-linked files\n", linked_files);
	printf ("\nFragmentation         = %.2f%%\n", fragm*100);
/*	printf ("(%ld seeks of %ld potential)\n", seeks, potential_seeks); */
	printf ("Indirects             = %ld (%.2f%%)\n",
		indirects, ind_p*100);
	printf ("Double indirects      = %ld (%.2f%%)\n",
		double_indirects, dind_p*100);
	printf ("Triple indirects      = %ld (%.2f%%)\n",
		triple_indirects, tind_p*100);
	printf ("Oversized directories = %d (%.2f%%)\n",
		big_directories, bdir_p*100);
	printf ("10 worst offenders:\n");
	printf ("   i-node   Size  Fragments    %%\ti-node   Size  Fragments    %%\n");
	for (i = 0; i < 5; i++){
		if (file_log[i].inode != 0){
			printf ("   %6d %6ld   %6ld   %6.2f%%",
				file_log[i].inode,
				file_log[i].data_blocks,
				file_log[i].seeks+1,
				file_log[i].fragm*100);
			if (file_log[i+5].inode != 0){
				printf ("\t%6d %6ld   %6ld   %6.2f%%",
					file_log[i+5].inode,
					file_log[i+5].data_blocks,
					file_log[i+5].seeks+1,
					file_log[i+5].fragm*100);
				}
			printf ("\n");
			}
		else break;
		}
	}

/*
 * anal_file : analyzes a single file for fragmentation.
 */
void anal_file (ino, fname)
int ino;					/* i-node number */
char *fname;					/* filename of this inode */
{
	struct file_data data;			/* current file statistics */

	struct dinode i_node;			/* current inode data */

	get_inodes (ino, &i_node, 1);
	init_stats(&data, ino);
	check_file (&i_node, ino, &data);
	log_stats (&data);
	printf ("   %-14s\t%6d    %6ld   %6ld   %6.2f%%\n",
		fname, data.inode, data.seeks+1, 
                       data.total_blocks, data.fragm*100);
	}

main (argc, argv)
int argc;
char *argv[];
{
	int next_param;
	struct stat f_stat;

	/*
	 * perform various initialization functions
	 */
	next_param = init (argc, argv);

	if (next_param == argc){
		/*
		 * no individual files to check, scan entire file system
		 */
		printf ("Analyzing file system %s...\n", special);

		/*
		 * scan through all i-nodes in the file system
		 */
		scan();

		/*
		 * print out statistics summary
		 */
		print_report();
		}
	else {
		/*
		 * scan individual files instead of entire file system
		 */
		printf ("        Name      \ti-node    Fragments  Size      %%\n");
		for (; next_param < argc; next_param++){
			if (stat (argv[next_param], &f_stat) != 0){
				error (errno, "error opening \"%s\"\n",
				       argv[next_param]);
				}
			else {
				if (!IS_SPECIAL (f_stat.st_mode)){
					anal_file (f_stat.st_ino, argv[next_param]);
					}
				}
			}
		}
	}
