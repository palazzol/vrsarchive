/*
 * Copyright (C) Rutherford Appleton Laboratory 1987
 * 
 * This source may be copied, distributed, altered or used, but not sold for profit
 * or incorporated into a product except under licence from the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to the source
 * made by persons other than the author should be marked as such.
 * 
 *	Crispin Goswell @ Rutherford Appleton Laboratory caag@uk.ac.rl.vd
 */
#include "main.h"
#include <signal.h>

static int Chdir (), Fork (), Execv (), Wait (), Exit (), System (), Signal (), Kill ();

InitUnix ()
 {
 	InstallOp ("chdir",	Chdir,	1, 1, 0, 0, String);
 	InstallOp ("fork",	Fork,	0, 1, 0, 0);
  	InstallOp ("wait",	Wait,	0, 5, 0, 0);
  	InstallOp ("uexit",	Exit,	1, 0, 0, 0, Integer);
  	InstallOp ("execv",	Execv,	2, 0, 0, 0, Array, String);
  	InstallOp ("system",	System,	1, 1, 0, 0, String);
  	InstallOp ("signal", 	Signal, 2, 1, 0, 0, Integer, Integer);
  	InstallOp ("kill",	Kill,	2, 1, 0, 0, Integer, Integer);
}

static int Chdir (dir) Object dir;
 {
 	char buffer [BUFSIZE];
 	int l = lengthString (dir);
 	
 	VOID strncpy (buffer, BodyString (dir), l);
 	buffer [l] = '\0';
 	
 	return Push (OpStack, MakeBoolean (!chdir (buffer)));
 }

static int Fork ()
 {
 	return Push (OpStack, MakeInteger (fork ()));
 }

static int Exit (status) Object status;
 {
 	exit (BodyInteger (status));
 	
 	return TRUE; /* shuts lint up */
 }

static int Wait ()
 {
 	int pid, status;
 	
 	if ((pid = wait (&status)) < 0)
 		return Push (OpStack, False);
 	return Push (OpStack, MakeBoolean (status & 0200))
 		&& Push (OpStack, MakeInteger (status & 0177))
 		&& Push (OpStack, MakeInteger (status >> 8))
 		&& Push (OpStack, MakeInteger (pid))
 		&& Push (OpStack, True);
 }

static int Execv (args, name) Object args, name;
 {
 	int i, nl, l = lengthArray (args);
 	char **av = (char **) Malloc ((unsigned) (l+1) * sizeof (char *));
 	char buffer [BUFSIZE];
 	
 	for (i = 0; i < l; i++)
 	 {
 	 	Object elem;
 	 	
 	 	elem = getArray (args, i);
 		if (TypeOf (elem) != String)
 			return Error (PTypeCheck);
 		else
 		 {
 		 	int l = lengthString (elem);
 		 	
 			av[i] = Malloc ((unsigned) l + 1);
 			VOID strncpy (av[i], BodyString (elem), l);
 			av[i][l] = '\0';
 		 }
 	 }
 	av[l] = NULL;
 	
 	nl = lengthString (name);
 	VOID strncpy (buffer, BodyString (name), nl);
 	buffer [nl] = '\0';
 	
 	if (execv (buffer, av) == -1)
 	 {
 	 	for (i = 0; i < l; i++)
 	 		Free (av[i]);
 	 	Free ((char *) av);
 	 	
 	 	return Error (PInvFileAccess);
 	 }
 	return TRUE;
 }

static int System (s) Object s;
 {
 	char buffer [BUFSIZE];
 	int l = lengthString (s);
 	
 	VOID strncpy (buffer, BodyString (s), l);
 	buffer [l] = '\0';
 	return Push (OpStack, MakeInteger (system (buffer)));
 }

static int Signal (n, s) Object n, s;
 {
	int sn = BodyInteger (n);
	
 	if (sn < 1 || sn > NSIG)
 		return Error (PRangeCheck);
	return Push (OpStack, MakeInteger (signal (sn, BodyInteger (s))));
 }

static int Kill (n, s) Object n, s;
 {
 	return Push (OpStack, MakeBoolean (0 == kill (BodyInteger (n), BodyInteger (s))));
 }
