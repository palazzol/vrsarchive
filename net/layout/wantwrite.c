/*  wantwrite  --  attempt to open file for output
 *
 *  Usage:  i = wantwrite (path,file,fullname,prompt,warn);
 *    int i,warn;
 *    char *path,*file,*fullname,*prompt;
 *
 *  Wantwrite will attempt to open "file" for output somewhere in
 *  the pathlist "path".  If no such file can be created, the user
 *  is given an oportuity to enter a new file name (after the
 *  prompt is printed).  The new file will then be sought using
 *  the same path.
 *  If the path is the null string, the file will be created with
 *  no prefix to the file name.  If the file name is null, the
 *  user will be prompted for a file name immediately.  The user
 *  can always abort wantwrite by typing just a carriage return
 *  to the prompt.
 *  Wantwrite will put the name of the successfully created file
 *  into the "fullname" string (which must therefore be long enough to
 *  hold a complete file name), and return its file descriptor;
 *  if no file is created, -1 will be returned.
 *  Wantwrite examines each entry in the path, to see if the
 *  desired file can be created there.  If "warn" is true, 
 *  wantwrite will first check to ensure that no such file
 *  already exists (if it does, the user is allowed to keep it).
 *  If no such file exists (or the user wants to delete it), then
 *  wantwrite attempts to create the file.  If it is unsuccessful,
 *  the next entry in the pathlist is examined in the same way.
 *
 *  HISTORY
 * 21-Oct-81  Fil Alleva (faa) at Carnegie-Mellon University
 *	Fixed bug which caused an infinite loop when getstr() got
 *	an EOT error and returned NULL. The error return was ignored
 *	and the value of "answer" was not changed which caused the loop.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.
 *
 */

#include <stdio.h>

int strcmp();
int creat();
int searchp();
char *getstr();

static int warnflag;
static int fildes;

static int func (fnam)
char *fnam;
{		/* attempt to create fnam */
	register int goahead;
	goahead = 1;
	if (warnflag) {
		fildes = open (fnam,1);
		if (fildes >= 0) {
			close (fildes);
			printf ("%s already exists!  ",fnam);
			goahead = getbool ("Delete old file?",0);
		}
	}
	if (goahead) {
		fildes = creat (fnam,0644);
		if (fildes < 0) {
			goahead = 0;
		}
	}
	return (!goahead);
}

int wantwrite (path,file,fullname,prompt,warn)
char *path,*file,*fullname,*prompt;
int warn;
{
	register int i;
	char myfile [200], *retval;

	if (*file == '\0') {
		getstr (prompt,"no file",myfile);
		if (strcmp(myfile,"no file") == 0)  return (-1);
	}
	else strcpy (myfile,file);

	warnflag = warn;
	do {
		i = searchp (path,myfile,fullname,func);
		if (i < 0) {
			if (*path && (*myfile != '/')) {
				printf ("%s in path \"%s\":  Can't create.\n",myfile,path);
			} 
			else {
				perror (myfile);
			}
			retval = getstr (prompt,"no file",myfile);
			if ((strcmp(myfile,"no file") == 0) || retval == NULL)
			    return (-1);
		}
	} 
	while (i < 0);

	return (fildes);
}
