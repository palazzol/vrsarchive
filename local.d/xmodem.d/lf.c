/*  Version 1.0		6/12/85
	Jim Hein
	Nicolet Instrument Corp
	5225 Verona Rd
	Madison, WI  53611-0288
*/
#
#include <fcntl.h>
#ifndef L_SET
#define L_SET	0
#define L_INCR	1
#define L_XTND	2
#endif  L_SET

#define CR	13
#define NL	10
#define BLOCK	512
#define STOP	0xff

CR2LF(file)
char *file; {
	int id, oid;
	char buf[BLOCK+1], *index();
	char temp[11], temp1[11];
	register char *ptr = buf;
	register int len;

	strcpy(temp,"tempXXXXXX");
	mktemp(temp);
	if ((oid = open(temp,O_CREAT | O_RDWR,0644)) < 0) {
		perror(temp);
		unlink(temp);
		exit(-1);
	}

	if ((id = open(file,O_RDONLY,0644)) < 0) {
		perror(file);
		unlink(temp);
		exit(-1);
	}

	buf[BLOCK] = 0;
	while ((len = read(id,ptr,BLOCK)) > 0) {
		while ( ptr = index(ptr,CR) ) {
			*ptr = NL;
		}
		write(oid, ptr = buf, len);
	}
	
	close(id);
	lseek(oid,1L,L_XTND);
	do {
		lseek(oid,-2L,L_INCR);
		read(oid,ptr,1);
	} while ( *ptr != NL );
	*ptr = STOP;
	write(oid,ptr,1);
	lseek(oid,0L,L_SET);

	strcpy(temp1,"tempXXXXXX");
	mktemp(temp1);
	if ((id = open(temp1,O_CREAT | O_WRONLY,0644)) < 0) {
		perror(temp1);
		unlink(temp1);
		unlink(temp);
		exit(-1);
	}

	ptr = buf;
	while ((len = read(oid,ptr,BLOCK)) > 0) {
		if (ptr = index(ptr,STOP))  {
			*ptr = 0;
			write(id, buf, strlen(buf));
			break;
		}
		write(id, ptr = buf, len);
	}

	close(oid); close(id);
	strcpy(buf,"mv ");
	strcat(buf,temp1);
	strcat(buf," ");
	strcat(buf,file);
	system(buf);
	unlink(temp);
}
